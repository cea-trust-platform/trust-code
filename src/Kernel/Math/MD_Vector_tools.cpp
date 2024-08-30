/****************************************************************************
* Copyright (c) 2024, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <Schema_Comm_Vecteurs.h>
#include <MD_Vector_composite.h>
#include <Echange_EV_Options.h>
#include <MD_Vector_tools.h>
#include <MD_Vector_seq.h>
#include <communications.h>
#include <stat_counters.h>
#include <Schema_Comm.h>
#include <TRUSTTrav.h>
#include <vector>

static Schema_Comm_Vecteurs comm;
static MD_Vector last_md;
static int last_isdouble = -1;
static int last_linesize = 0;
static Echange_EV_Options last_opt;


class MD_Vector_renumber
{
public:
  // for each item in the source vector, should it be kept
  ArrOfBit items_to_keep_;
  // new C index of each item in the source vector to renumber.
  //  (for items not kept, must point to a geometrically equivalent item)
  // renum_ can be a global or a local number.
  IntVect renum_;
  // number of non-zero items in items_to_keep_:
  int  nb_items_to_keep_;
};


template <class VECT, class TAB>
static void creer_tableau_distribue_(const MD_Vector& md, VECT& v, RESIZE_OPTIONS opt)
{
  if (v.get_md_vector().non_nul())
    {
      // Si ce message apparait et qu'on est sur de ce qu'on fait,
      // faire ceci avant d'appeler creer_tableau_distribue:
      //  vect.set_md_vector(MD_Vector()); // Annule le descripteur et garde le vecteur.
      // ou
      //  vect.reset(); // Detruit completement le contenu du vecteur
      Cerr << "Internal error in MD_Vector_tools::creer_tableau_distribue:\n"
           << " Vector already has a parallel vector structure" << finl;
      Process::exit();
    }
  int sz = md->get_nb_items_tot();
  // Attention, sz_r peut valoir -1 dans certains cas. Alors le test n==sz_r sera toujours faux,
  //  mais c'est bien ce qu'on veut...
  int sz_r = md->get_nb_items_reels();
  int err = 0;
  TAB* vv = dynamic_cast<TAB*>(&v);
  if (vv)
    {
      TAB& t = *vv;
      const int n = t.dimension_tot(0);
      if (n == sz_r || n == 0) t.resize_dim0(sz, opt);
      else if (n == sz) { /* ok no resize */  }
      else {  err = 1; }
    }
  else
    {
      const int n = v.size_totale();
      if (n == sz_r || n == 0) {  v.resize(sz, opt); }
      else if (n == sz) { /* ok no resize */ }
      else { err = 1; }
    }
  if (err)
    {
      Cerr << "Internal error in MD_Vector_tools::creer_tableau_distribue:\n"
           << " Input vector has wrong size or dimension(0): expected 0 or size_reele=" << sz_r << finl;
      Process::exit();
    }
  v.set_md_vector(md);
}

template <typename _TYPE_, typename _SIZE_>
static void creer_tableau_seq_(const MD_Vector& md, TRUSTVect<_TYPE_,_SIZE_>& v, RESIZE_OPTIONS opt)
{
  using TAB = TRUSTTab<_TYPE_, _SIZE_>;

  const MD_Vector_seq * md_seq = dynamic_cast<const MD_Vector_seq *>(&md.valeur());
  assert(md_seq != nullptr); // sequential, cast should always be OK

  trustIdType nb64 = md_seq->get_nb_items();
  assert(nb64 < std::numeric_limits<int>::max());
  int sz = (int)nb64;

  // TODO IG FIXME factorize with above:
  bool err = false;
  // Should we use resize() or resize_dim0 ...:
  TAB* vv = dynamic_cast<TAB*>(&v);
  if (vv)
    {
      TAB& t = *vv;
      const _SIZE_ n = t.dimension_tot(0);
      if (n == 0) t.resize_dim0(sz, opt);
      else if (n == sz) { /* ok no resize */  }
      else {  err = true; }
    }
  else
    {
      const _SIZE_ n = v.size_totale();
      if (n == 0) v.resize(sz, opt);
      else if (n == sz) { /* ok no resize */ }
      else { err = true; }
    }
  if (err)
    {
      Cerr << "Internal error in MD_Vector_tools::creer_tableau_seq_:\n"
           << " Input vector has wrong size or dimension(0)" << finl;
      Process::exit();
    }
  // Important - keep the same MD_Vector object:
  v.set_md_vector(md);
}


/*! @brief transforme v en un tableau parallele ayant la structure md.
 *
 * md doit est non nul !
 *   Les dimension(i>=1) du tableau v (si c'est un IntTab ou DoubleTab) sont conservees,
 *   les dimension(0) et dimension_tot(0) sont modifiees en fonction du nombre d'items
 *   specifies dans le MD_Vector.
 *   La dimension initiale du vecteur doit etre soit 0, soit md.get_nb_items_reels(),
 *    soit md.get_nb_items_tot(). Si besoin, la taille du tableau est modifiee et on
 *    initialise le tableau selon opt.
 *  ATTENTION, echange_espace_virtuel() n'est PAS appele. Les cases virtuelles ne sont pas initialisees...
 */
void MD_Vector_tools::creer_tableau_distribue(const MD_Vector& md, Array_base& v, RESIZE_OPTIONS opt)
{
  if (!md.non_nul())
    {
      Cerr << "Error in MD_Vector_tools::creer_tableau_distribue(): MD_Vector is null" << finl;
      Process::exit();
    }

  IntVect* intV = dynamic_cast<IntVect*>(&v);
#if INT_is_64_ == 2
  TIDVect* tidV = dynamic_cast<TIDVect*>(&v);
#endif
  DoubleVect* doubleV = dynamic_cast<DoubleVect*>(&v);
  FloatVect* floatV = dynamic_cast<FloatVect*>(&v);

  if(sub_type(MD_Vector_seq, md.valeur()))
    {
      /* [ABN] Here we would like to do
              assert(Process::is_sequential());
       * but we can not: for example when reading a MED file we construct temporarily a dummy domain
       * which looks like a sequential one, even if we are actually running parallel ... too bad.
       */

      if (intV) creer_tableau_seq_<int, int>(md, *intV, opt);
#if INT_is_64_ == 2
      else if (tidV) creer_tableau_seq_<trustIdType, int>(md, *tidV, opt);
#endif
      else if (doubleV) creer_tableau_seq_<double, int>(md, *doubleV, opt);
      else if (floatV) creer_tableau_seq_<float, int>(md, *floatV, opt);
      else
        {
          Cerr << "Internal error in MD_Vector_tools::creer_tableau_seq():\n"
               << " Invalid array type " << v.que_suis_je()
               << "\n Array must be a subtype of IntVect or DoubleVect or FloatVect" << finl;
          Process::exit();
        }
    }
  else  // parallel computation **or** composite descriptor (MD_Vector_composite)
    {
      if (intV) creer_tableau_distribue_<IntVect, IntTab>(md, *intV, opt);
#if INT_is_64_ == 2
      else if (tidV) creer_tableau_distribue_<TIDVect, TIDTab>(md, *tidV, opt);
#endif
      else if (doubleV) creer_tableau_distribue_<DoubleVect, DoubleTab>(md, *doubleV, opt);
      else if (floatV) creer_tableau_distribue_<FloatVect, FloatTab>(md, *floatV, opt);
      else
        {
          Cerr << "Internal error in MD_Vector_tools::creer_tableau_distribue(const MD_Vector & md, Array_base & v):\n"
               << " Invalid array type " << v.que_suis_je()
               << "\n Array must be a subtype of IntVect or DoubleVect or FloatVect" << finl;
          Process::exit();
        }
    }
}


template <typename _TYPE_>
void echange_espace_virtuel_(const MD_Vector& md, TRUSTVect<_TYPE_>& v, const Echange_EV_Options& opt = echange_ev_opt_default)
{
  const MD_Vector_base& mdv = md.valeur();
  const int is_double = (std::is_same<_TYPE_,double>::value) ? 1 : 0;

  if (md == last_md && v.line_size() == last_linesize && last_isdouble == is_double &&  last_opt == opt) { /* Do nothing si double et pas 1er passage */ }
  else
    {
      last_md = md;
      last_linesize = v.line_size();
      last_isdouble = is_double;
      last_opt = opt;
      comm.begin_init();
      mdv.initialize_comm(opt, comm, v);
      comm.end_init();
    }
  bool bufferOnDevice = Process::is_parallel() && v.isDataOnDevice() && Objet_U::computeOnDevice;
  comm.begin_comm(bufferOnDevice);     // buffer allocated on device
  mdv.prepare_send_data(opt, comm, v); // pack buffer on device (read_from_vect_items)
  comm.exchange(bufferOnDevice);       // buffer d2h + MPI + buffer h2d
  mdv.process_recv_data(opt, comm, v); // unpack buffer on device (write_to_vect_items + write_to_vect_blocs)
  comm.end_comm();
}

template<typename _TYPE_>
void echange_espace_virtuel1_(const MD_Vector& md, TRUSTVect<_TYPE_>& v, MD_Vector_tools::Operations_echange opt)
{
  switch(opt)
    {
    case MD_Vector_tools::ECHANGE_EV:
      echange_espace_virtuel_(md, v, echange_ev_opt_default);
      break;
    case MD_Vector_tools::EV_SOMME:
      echange_espace_virtuel_(md, v, Echange_EV_Options(Echange_EV_Options::SUM));
      break;
    case MD_Vector_tools::EV_SOMME_ECHANGE:
      echange_espace_virtuel_(md, v, Echange_EV_Options(Echange_EV_Options::SUM));
      echange_espace_virtuel_(md, v, echange_ev_opt_default);
      break;
    case MD_Vector_tools::EV_MAX:
      echange_espace_virtuel_(md, v, Echange_EV_Options(Echange_EV_Options::MAX));
      break;
    case MD_Vector_tools::EV_MINCOL1:
      echange_espace_virtuel_(md, v, Echange_EV_Options(Echange_EV_Options::MINCOL1));
      break;
    default:
      Cerr << "echange_espace_virtuel1_ operation not implemented" << finl;
      Process::exit();
    }
}

template<typename _TYPE_>
inline void call_echange_espace_virtuel(TRUSTVect<_TYPE_>& v, MD_Vector_tools::Operations_echange opt)
{
  const MD_Vector& md = v.get_md_vector();
  if (md.non_nul() && Process::is_parallel())
    {
      // [ABN] in some weird cases (like building a temporary domain when reading a MED file)
      // we might end up calling the current method with a sequential MD_Vector:
      if (sub_type(MD_Vector_seq, md.valeur())) return;

      statistiques().begin_count(echange_vect_counter_);
      echange_espace_virtuel1_(v.get_md_vector(), v, opt);
      statistiques().end_count(echange_vect_counter_);
    }
  //else Cerr << "Warning: A call to ::echange_espace_virtuel() is done on a non-distributed vector." << finl; /Process::exit();
}

void MD_Vector_tools::echange_espace_virtuel(IntVect& v, Operations_echange opt) { call_echange_espace_virtuel<int>(v,opt); }
#if INT_is_64_ == 2
void MD_Vector_tools::echange_espace_virtuel(TIDVect& v, Operations_echange opt) { call_echange_espace_virtuel<trustIdType>(v,opt); }
#endif
void MD_Vector_tools::echange_espace_virtuel(DoubleVect& v, Operations_echange opt) { call_echange_espace_virtuel<double>(v,opt); }
void MD_Vector_tools::echange_espace_virtuel(FloatVect& v, Operations_echange opt) { call_echange_espace_virtuel<float>(v,opt); }

void MD_Vector_tools::compute_sequential_items_index(const MD_Vector&, MD_Vector_renumber&, int line_size)
{
  Cerr << "MD_Vector_tools::compute_sequential_items_index" << finl;
  Process::exit();
}

/*! @brief cree un descripteur pour un sous-ensemble d'un vecteur.
 *
 * renum fournit la structure et le descripteur du vecteur source (doit avoir line_size==1)
 *   renum doit contenir -1 pour les items du vecteur source a ne pas conserver et une
 *    valeur positive ou nulle pour les items a conserver.
 *   La valeur renum[i] donne l'indice de cet item dans le nouveau tableau.
 *   Attention, si un item du vecteur source est recu d'un autre processeur et doit etre conserve,
 *    ce meme item doit aussi etre conserve sur le processeur qui envoie cet item.
 *
 */
void MD_Vector_tools::creer_md_vect_renum(const IntVect& renum, MD_Vector& md_vect)
{
  const MD_Vector& src_md = renum.get_md_vector();
  if (!src_md.non_nul())
    {
      Cerr << "Internal error in MD_Vector_tools::creer_md_vect_renum: descripteur nul !" << finl;
      Process::exit();
    }
  if (renum.line_size() != 1)
    {
      Cerr << "Internal error in MD_Vector_tools::creer_md_vect_renum: line_size != 1" << finl;
      Process::exit();
    }
  src_md->fill_md_vect_renum(renum, md_vect);
}

/*! @brief Idem que creer_md_vect_renum() mais cree une numerotation par defaut.
 *
 * Le tableau flags_renum doit contenir en entree une valeur POSITIVE OU NULLE pour les
 *   valeurs a conserver et une valeur negative pour les autres.
 *   En sortie, flags_renum contient l'indice de l'item dans le tableau reduit s'il est
 *    conserve, sinon la valeur d'origine non modifiee.
 *   Les items sont places dans l'ordre croissant de leur indice local sur dans le descripteur
 *    d'origine.
 *
 */
void MD_Vector_tools::creer_md_vect_renum_auto(IntVect& flags_renum, MD_Vector& md_vect)
{
  const int n = flags_renum.size_array();
  int count = 0;
  for (int i = 0; i < n; i++)
    {
      int x = flags_renum[i];
      if (x >= 0)
        flags_renum[i] = count++;
    }
  creer_md_vect_renum(flags_renum, md_vect);
}

/*! @brief dumps vector v with its parallel descriptor to os.
 *
 * os must be a file type with one file per process (no shared file).
 *
 */
void MD_Vector_tools::dump_vector_with_md(const DoubleVect& v, Sortie& os)
{
  assert(v.line_size() == 1); // Only Vect! not tab.

  const MD_Vector_base& md = v.get_md_vector().valeur();
  os << md.que_suis_je() << finl;
  os << md << finl;

  os << v.size_array() << finl;
  os.put(v.addr(), v.size_array(), v.line_size());
  os << finl;
}

/*! @brief restores a vector saved by dump_vector_with_md.
 *
 * The code must run with the same number of processors.
 *   "v" will have a newly created copy of the descriptor (not equal to any
 *   other descriptor previously loaded). If "identical" descriptors are needed
 *   for several vectors, you might want to call set_md_vector() afterwards to
 *   attach another descriptor. See save_matrice_conditionnel()
 */
void MD_Vector_tools::restore_vector_with_md(DoubleVect& v, Entree& is)
{

  v.reset();
  OWN_PTR(MD_Vector_base) md_ptr;
  Nom md_type;
  is >> md_type;
  md_ptr.typer(md_type);
  is >> md_ptr.valeur();

  // Creation du MD_Vector attache au tableau
  MD_Vector md;
  md.copy(md_ptr.valeur());

  DoubleVect toto;
  toto.resize(md_ptr->get_nb_items_tot(), RESIZE_OPTIONS::NOCOPY_NOINIT);
  int size_tot;
  is >> size_tot;
  is.get(toto.addr(), size_tot);
  toto.set_md_vector(md);

  // Attache v a toto et oublie toto...
  v.reset();
  v.ref(toto);
}
