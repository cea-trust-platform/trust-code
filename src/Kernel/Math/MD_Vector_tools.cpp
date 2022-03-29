/****************************************************************************
* Copyright (c) 2022, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        MD_Vector_tools.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_Comm_Vecteurs.h>
#include <MD_Vector_composite.h>
#include <Echange_EV_Options.h>
#include <MD_Vector_tools.h>
#include <communications.h>
#include <stat_counters.h>
#include <Schema_Comm.h>
#include <TRUSTTrav.h>
#include <ArrOfBit.h>
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
static void creer_tableau_distribue_(const MD_Vector& md, VECT& v, Array_base::Resize_Options opt)
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
  int sz = md.valeur().get_nb_items_tot();
  // Attention, sz_r peut valoir -1 dans certains cas. Alors le test n==sz_r sera toujours faux,
  //  mais c'est bien ce qu'on veut...
  int sz_r = md.valeur().get_nb_items_reels();
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

// Description: transforme v en un tableau parallele ayant la structure md.
//  md doit est non nul !
//  Les dimension(i>=1) du tableau v (si c'est un IntTab ou DoubleTab) sont conservees,
//  les dimension(0) et dimension_tot(0) sont modifiees en fonction du nombre d'items
//  specifies dans le MD_Vector.
//  La dimension initiale du vecteur doit etre soit 0, soit md.get_nb_items_reels(),
//   soit md.get_nb_items_tot(). Si besoin, la taille du tableau est modifiee et on
//   initialise le tableau selon opt.
// ATTENTION, echange_espace_virtuel() n'est PAS appele.
//  Les cases virtuelles ne sont pas initialisees...
void MD_Vector_tools::creer_tableau_distribue(const MD_Vector& md, Array_base& v, Array_base::Resize_Options opt)
{
  if (!md.non_nul())
    {
      Cerr << "Error in MD_Vector_tools::creer_tableau_distribue(): MD_Vector is null" << finl;
      Process::exit();
    }

  IntVect* intV = dynamic_cast<IntVect*>(&v);
  DoubleVect* doubleV = dynamic_cast<DoubleVect*>(&v);

  if (intV) creer_tableau_distribue_<IntVect, IntTab>(md, *intV, opt);
  else if (doubleV) creer_tableau_distribue_<DoubleVect, DoubleTab>(md, *doubleV, opt);
  else
    {
      Cerr << "Internal error in MD_Vector_tools::creer_tableau_distribue(const MD_Vector & md, Array_base & v):\n"
           << " Invalid array type " << v.que_suis_je()
           << "\n Array must be a subtype of IntVect or DoubleVect" << finl;
      Process::exit();
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
  comm.begin_comm();
  mdv.prepare_send_data(opt, comm, v);
  comm.exchange();
  mdv.process_recv_data(opt, comm, v);
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
  if (v.get_md_vector().non_nul())
    {
      statistiques().begin_count(echange_vect_counter_);
      echange_espace_virtuel1_(v.get_md_vector(), v, opt);
      statistiques().end_count(echange_vect_counter_);
    }
  //else Cerr << "Warning: A call to ::echange_espace_virtuel() is done on a non-distributed vector." << finl; /Process::exit();
}

void MD_Vector_tools::echange_espace_virtuel(IntVect& v, Operations_echange opt) { call_echange_espace_virtuel<int>(v,opt); }
void MD_Vector_tools::echange_espace_virtuel(DoubleVect& v, Operations_echange opt) { call_echange_espace_virtuel<double>(v,opt); }

inline void setflag(ArrOfInt& flags, int i) { flags[i] = 1; }
inline void clearflag(ArrOfInt& flags, int i) { flags[i] = 0; }
inline void clearflag(ArrOfBit& flags, int i) { flags.clearbit(i); }

inline void setflag(ArrOfBit& flags, int i)
{
  // On a initialise le tableau a 1 par defaut dans get_sequential_items_flags()
  assert(flags[i] == 1);
}

template <class FlagsType>
static int get_seq_flags(const MD_Vector_base& mdv, FlagsType& flags, int startidx, int line_size)
{
  int count = 0;
  if (sub_type(MD_Vector_base2, mdv))
    {
      const MD_Vector_base2& ms = ref_cast(MD_Vector_base2, mdv);
      const int nblocs = ms.blocs_items_to_sum_.size_array() >> 1;
      const int *ptr = ms.blocs_items_to_sum_.addr();
      int j = startidx;
      for (int i = nblocs; i; i--)
        {
          // On pourrait optimiser pour les ArrOfBit en ajoutant a cette classe
          //  une methode setflag/clearflag(range_begin, range_end);
          int jmax = startidx + (*(ptr++)) * line_size; // debut du prochain bloc d'items sequentiels
          for (; j < jmax; j++)
            clearflag(flags, j);
          jmax = startidx + (*(ptr++)) * line_size; // fin du bloc d'items sequentiels
          assert(jmax > j);
          count += jmax - j;
          for (; j < jmax; j++)
            setflag(flags, j);
        }
      // Fin du remplissage
      int sz = mdv.get_nb_items_tot() * line_size;
      for (; j < sz; j++)
        clearflag(flags, j);
    }
  else
    {
      Cerr << "Error in MD_Vector_tools::get_sequential_items_flags: unsupported MD_Vector type " << mdv.que_suis_je() << finl;
      Process::exit();
    }
  return count;
}

int MD_Vector_tools::get_sequential_items_flags(const MD_Vector& md, ArrOfBit& flags, int line_size)
{
  const MD_Vector_base& mdv = md.valeur();
  const int sz = mdv.get_nb_items_tot() * line_size;
  // setbit et clearbit font |= et &=, donc valgrind ne voit pas qu'on initialise tout
  // le tableau. On initialise tout explicitement:
  flags.resize_array(sz);
  flags = 1; // valeur par defaut la plus courante (0 est le cas particulier), voir setflag(ArrOfBit)
  return get_seq_flags(mdv, flags, 0, line_size);
}

int MD_Vector_tools::get_sequential_items_flags(const MD_Vector& md, ArrOfInt& flags, int line_size)
{
  const MD_Vector_base& mdv = md.valeur();
  const int sz = mdv.get_nb_items_tot() * line_size;
  flags.resize_array(sz, Array_base::NOCOPY_NOINIT);
  return get_seq_flags(mdv, flags, 0, line_size);
}

void MD_Vector_tools::compute_sequential_items_index(const MD_Vector&, MD_Vector_renumber&, int line_size)
{
  Cerr << "MD_Vector_tools::compute_sequential_items_index" << finl;
  Process::exit();
}

// Description:
// Renvoie le nombre d'items extraits (pas le nombre de blocs)
static int extract_blocs(const ArrOfInt src, const ArrOfInt& renum, ArrOfInt& dest)
{
  const int nblocs_src = src.size_array() / 2;
  dest.set_smart_resize(1);
  dest.resize_array(0);
  int end_last_bloc = -1;
  int count = 0;
  for (int i = 0; i < nblocs_src; i++)
    {
      const int deb = src[i*2];
      const int fin = src[i*2+1];
      for (int j = deb; j < fin; j++)
        {
          const int rj = renum[j];
          if (rj >= 0)
            {
              count++;
              // Blocs construction assumes that renum is ordered
              assert(rj >= end_last_bloc);
              if (rj == end_last_bloc)
                {
                  // item is contiguous to last bloc in destination array, increase bloc size:
                  end_last_bloc++;
                  dest[dest.size_array()-1] = end_last_bloc;
                }
              else
                {
                  // non contiguous item, create new bloc
                  end_last_bloc = rj;
                  dest.append_array(end_last_bloc);
                  end_last_bloc++;
                  dest.append_array(end_last_bloc);
                }
            }
        }
    }
  return count;
}

static void creer_md_vect_renum(const IntVect& renum, const MD_Vector_std& src, MD_Vector_std& dest)
{
  // Calcul de nb_items_tot_ et nb_items_reels_
  // nb_items_tot_ est le nombre d'items pour lesquels renum[i] >= 0
  // Les items reels sont ceux qui etaient reels dans le tableau d'origine et qui sont a conserver
  {
    const int src_size = src.get_nb_items_tot();
    const int src_size_r = src.get_nb_items_reels();
    dest.nb_items_tot_ = 0;
    dest.nb_items_reels_ = 0;
    for (int i = 0; i < src_size; i++)
      {
        if (renum[i] >= 0)
          {
            dest.nb_items_tot_++;
            if (i < src_size_r)
              dest.nb_items_reels_++;
          }
      }
    // Si l'attribut nb_items_reels_ est invalide dans le vecteur source, il est invalide dans le
    // vecteur resultat:
    if (src_size_r < 0)
      dest.nb_items_reels_ = -1;
  }
  // Remplissage de blocs_items_to_sum_ et blocs_items_to_compute_
  // Ce sont les items pour lesquels renum[i] >= 0 et qui sont dans les blocs_items_to_sum_
  // et blocs_items_to_compute_ du descripteur source.
  extract_blocs(src.blocs_items_to_sum_, renum, dest.blocs_items_to_sum_);
  dest.nb_items_seq_local_ = extract_blocs(src.blocs_items_to_compute_, renum, dest.blocs_items_to_compute_);
  dest.nb_items_seq_tot_ = Process::mp_sum(dest.nb_items_seq_local_);

  // ********************************************************
  // Calcul des items a recevoir: ce sont les items pour lesquels renum[i] >= 0 et qui etaient a
  // recevoir dans le descripteur source.
  // Chaque fois qu'on trouve un item a recevoir, il faut informer le processeur voisin qui possede l'item
  // que c'est item doit etre envoye (utilisation du schema_comm). Pour identifier ces items, on utilise
  // sont rang dans le tableau des items a envoyer dans le descripteur source (variable item_rank)
  //
  const int nb_pe_voisins = src.pe_voisins_.size_array();
  Schema_Comm schema_comm;
  schema_comm.set_send_recv_pe_list(src.pe_voisins_, src.pe_voisins_);
  schema_comm.begin_comm();
  // Calcul des listes items_to_recv
  // index+data forment les deux tableau d'un StaticIntLists.
  // On construit pour commencer autant de listes que de processeurs voisins dans le descripteur source.
  // On retirera a la fin les processeurs qui n'echangent pas de donnees
  ArrOfInt dest_items_recv_index;
  ArrOfInt dest_items_recv_data;
  ArrOfInt dest_blocs_recv_index;
  ArrOfInt dest_blocs_recv_data;

  ArrOfInt tmp;
  tmp.set_smart_resize(1);
  {
    dest_items_recv_index.resize_array(nb_pe_voisins + 1, Array_base::NOCOPY_NOINIT);
    dest_items_recv_index[0] = 0;
    dest_blocs_recv_index.resize_array(nb_pe_voisins + 1, Array_base::NOCOPY_NOINIT);
    dest_blocs_recv_index[0] = 0;
    dest.blocs_items_count_.resize_array(nb_pe_voisins, Array_base::NOCOPY_NOINIT);
    dest.nb_items_to_items_.resize_array(nb_pe_voisins, Array_base::NOCOPY_NOINIT);
    // Preallocation de la taille maxi
    dest_items_recv_data.resize_array(src.items_to_recv_.get_data().size_array(), Array_base::NOCOPY_NOINIT);
    dest_items_recv_data.set_smart_resize(1);
    dest_items_recv_data.resize_array(0);
    // On ne peut pas prevoir le nombre de blocs, il peut y en avoir plus que dans la source
    dest_blocs_recv_data.set_smart_resize(1);
    for (int i_pe = 0; i_pe < nb_pe_voisins; i_pe++)
      {
        tmp.resize_array(0);
        int item_rank = 0; // Compteur d'items recus dans le descripteur source
        // 1) Extraction des items individuels
        //    (remplissage de dest_items_recv_index, dest_items_recv_data et tmp)
        const int n = src.items_to_recv_.get_list_size(i_pe);
        for (int i = 0; i < n; i++)
          {
            const int j = src.items_to_recv_(i_pe, i);
            const int rj = renum[j];
            if (rj >= 0)
              {
                // Ajout de cet item
                dest_items_recv_data.append_array(rj);
                // Demander au processeur voisin d'envoyer cet item
                tmp.append_array(item_rank);
              }
            item_rank++;
          }
        // Initialisation de la taille de la liste d'items pour ce processeur:
        dest_items_recv_index[i_pe+1] = dest_items_recv_data.size_array();

        // 2) Extraction des blocs d'items recus de ce processeur
        //    (remplissage de dest_blocs_recv_index dest_blocs_recv_data dest.blocs_items_count_ et tmp)
        const int nblocs = src.blocs_to_recv_.get_list_size(i_pe) / 2;
        int received_count = 0; // nombre d'items recus de ce processeur
        for (int ibloc = 0; ibloc < nblocs; ibloc++)
          {
            const int jdeb = src.blocs_to_recv_(i_pe, ibloc * 2);
            const int jfin = src.blocs_to_recv_(i_pe, ibloc * 2 + 1);
            int last_added = -2; // ne peut jamais etre egal a rj-1
            // Pour chaque item du bloc, s'il doit etre conserve, le reporter dans les blocs a recevoir:
            for (int j = jdeb; j < jfin; j++)
              {
                const int rj = renum[j];
                if (rj >= 0)
                  {
                    if (rj <= last_added)
                      {
                        // Les items dans les blocs a recevoir doivent etre tries dans l'ordre croissant
                        // (sinon cet algorithme ne marche pas, il faut en utiliser un autre pour
                        //  construire les blocs, et en plus le comptage des items reels ne veut peut-etre plus rien dire)
                        Cerr << "Error in creer_md_vect_renum: renum array is not sorted: cannot extract blocs to recv" << finl;
                        Process::exit();
                      }
                    // Cet item est-il contigu avec le precedent ?
                    if (last_added == rj-1)
                      {
                        // Oui, augmenter la taille du dernier bloc
                        const int k = dest_blocs_recv_data.size_array();
                        // fin de bloc = indice du dernier plus 1
                        dest_blocs_recv_data[k-1] = rj+1;
                      }
                    else
                      {
                        // Non, creer un nouveau bloc
                        dest_blocs_recv_data.append_array(rj);
                        dest_blocs_recv_data.append_array(rj+1);
                      }
                    last_added = rj;
                    // Demander au processeur voisin d'envoyer cet item
                    tmp.append_array(item_rank);
                    received_count++;
                  }
                item_rank++;
              }
          }
        // Initialisation de la taille de la liste de blocs pour ce processeur:
        dest_blocs_recv_index[i_pe+1] = dest_blocs_recv_data.size_array();
        // Nombre d'items recus de ce processeur
        dest.blocs_items_count_[i_pe] = received_count;
        // Envoi du tableau tmp au processeur voisin:
        schema_comm.send_buffer(src.pe_voisins_[i_pe]) << tmp;
      }
  }

  schema_comm.echange_taille_et_messages();

  // ********************************************************
  // Construction de dest.items_to_send avec les infos recues
  //
  ArrOfInt dest_items_send_index;
  dest_items_send_index.resize_array(nb_pe_voisins + 1, Array_base::NOCOPY_NOINIT);
  ArrOfInt dest_items_send_data;
  ArrOfInt nb_items_to_items(nb_pe_voisins); // Initialise a zero

  // Allocation d'une taille par borne superieure:
  dest_items_send_data.resize_array(src.items_to_send_.get_data().size_array(), Array_base::NOCOPY_NOINIT);
  dest_items_send_index[0] = 0;
  {
    int count = 0;
    int error = 0;
    for (int i_pe = 0; i_pe < nb_pe_voisins; i_pe++)
      {
        // nb_item_received_single est le nombre d'items dans items_to_send_ qui seront receptionnes
        //  par items_to_recv_. Les suivants sont recus dans blocs_to_recv_.
        const int j_limite_bloc = src.nb_items_to_items_[i_pe];
        int new_j_limite_bloc = 0;
        schema_comm.recv_buffer(src.pe_voisins_[i_pe]) >> tmp;
        const int n = tmp.size_array();
        for (int i = 0; i < n; i++)
          {
            // tmp contient des indices qui pointent dans src.items_to_send_
            int j = tmp[i];
            if (j >= j_limite_bloc)
              new_j_limite_bloc++; // Cet item est dans les blocs_to_recv_ de l'autre cote
            // k est l'indice dans l'ancien tableau de l'item a envoyer
            int k = src.items_to_send_(i_pe, j);
            int renum_k = renum[k];
            // Une erreur possible ici: un processeur a besoin d'un item virtuel et l'item
            // reel a ete supprime sur le processeur source. Il faut alors utiliser un
            // autre algo pour changer le proprietaire de l'item...
            if (renum_k < 0)
              error = 1;
            dest_items_send_data[count++] = renum_k;
          }
        dest_items_send_index[i_pe+1] = count;
        nb_items_to_items[i_pe] = new_j_limite_bloc;
      }
    if (error)
      {
        Cerr << "Internal error in MD_Vector_tools::creer_md_vect_renum:\n"
             << " processor " << Process::me()
             << " received a request for distant items that are not in the new vector" << finl;
        Process::exit();
      }
    // On ajuste a la taille definitive
    dest_items_send_data.set_smart_resize(1);
    dest_items_send_data.resize_array(count);
  }
  schema_comm.end_comm();

  // Construction de la liste de processeurs voisins de dest
  //  (processeurs avec qui on echange des donnees)
  // Et compression des indexes des StaticIntLists pour retirer les
  //  processeurs supprimes.
  {
    int pe_count = 0;
    dest.pe_voisins_.resize_array(nb_pe_voisins, Array_base::NOCOPY_NOINIT);
    for (int i_pe = 0; i_pe < nb_pe_voisins; i_pe++)
      {
        int flag = 0;
        if (dest_items_recv_index[i_pe+1] - dest_items_recv_index[i_pe] > 0)
          flag = 1;
        if (dest_blocs_recv_index[i_pe+1] - dest_blocs_recv_index[i_pe] > 0)
          flag = 1;
        if (dest_items_send_index[i_pe+1] - dest_items_send_index[i_pe] > 0)
          flag = 1;

        if (flag)
          {
            dest_items_recv_index[pe_count+1] = dest_items_recv_index[i_pe+1];
            dest_blocs_recv_index[pe_count+1] = dest_blocs_recv_index[i_pe+1];
            dest_items_send_index[pe_count+1] = dest_items_send_index[i_pe+1];
            dest.blocs_items_count_[pe_count] = dest.blocs_items_count_[i_pe];
            dest.pe_voisins_[pe_count] = src.pe_voisins_[i_pe];
            dest.nb_items_to_items_[pe_count] = nb_items_to_items[i_pe];
            pe_count++;
          }
      }
    dest_items_recv_index.set_smart_resize(1);
    dest_items_recv_index.resize_array(pe_count+1);
    dest_blocs_recv_index.set_smart_resize(1);
    dest_blocs_recv_index.resize_array(pe_count+1);
    dest_items_send_index.set_smart_resize(1);
    dest_items_send_index.resize_array(pe_count+1);
    dest.pe_voisins_.set_smart_resize(1);
    dest.pe_voisins_.resize_array(pe_count);
    dest.blocs_items_count_.set_smart_resize(1);
    dest.blocs_items_count_.resize_array(pe_count);
    dest.nb_items_to_items_.set_smart_resize(1);
    dest.nb_items_to_items_.resize_array(pe_count);
  }
  dest.items_to_send_.set_index_data(dest_items_send_index, dest_items_send_data);
  dest.items_to_recv_.set_index_data(dest_items_recv_index, dest_items_recv_data);
  dest.blocs_to_recv_.set_index_data(dest_blocs_recv_index, dest_blocs_recv_data);
}

// Description: cree un descripteur pour un sous-ensemble d'un vecteur.
//  renum fournit la structure et le descripteur du vecteur source (doit avoir line_size==1)
//  renum doit contenir -1 pour les items du vecteur source a ne pas conserver et une
//   valeur positive ou nulle pour les items a conserver.
//  La valeur renum[i] donne l'indice de cet item dans le nouveau tableau.
//  Attention, si un item du vecteur source est recu d'un autre processeur et doit etre conserve,
//   ce meme item doit aussi etre conserve sur le processeur qui envoie cet item.
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
  const MD_Vector_base& src_md_val = src_md.valeur();
  if (sub_type(MD_Vector_std, src_md_val))
    {
      const MD_Vector_std& src = ref_cast(MD_Vector_std, src_md_val);
      MD_Vector_std dest;
      ::creer_md_vect_renum(renum, src, dest);
      md_vect.copy(dest);
    }
  else if (sub_type(MD_Vector_composite, src_md_val))
    {
      const MD_Vector_composite& src1 = ref_cast(MD_Vector_composite, src_md_val);
      const MD_Vector_std& src = src1.global_md_;
      MD_Vector_std dest;
      ::creer_md_vect_renum(renum, src, dest);
      md_vect.copy(dest);
    }
  else
    {
      Cerr << "Error in MD_Vector_tools::creer_md_vect_renum: unknown MD_Vector type "
           << src_md_val.que_suis_je() << finl;
      Process::exit();
    }
}

// Description:
//  Idem que creer_md_vect_renum() mais cree une numerotation par defaut.
//  Le tableau flags_renum doit contenir en entree une valeur POSITIVE OU NULLE pour les
//  valeurs a conserver et une valeur negative pour les autres.
//  En sortie, flags_renum contient l'indice de l'item dans le tableau reduit s'il est
//   conserve, sinon la valeur d'origine non modifiee.
//  Les items sont places dans l'ordre croissant de leur indice local sur dans le descripteur
//   d'origine.
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

// Description: dumps vector v with its parallel descriptor to os.
//  os must be a file type with one file per process (no shared file).
void MD_Vector_tools::dump_vector_with_md(const DoubleVect& v, Sortie& os)
{
  const MD_Vector_base& md = v.get_md_vector().valeur();
  os << md.que_suis_je() << finl;
  os << md << finl;
  os << "line_size" << space << v.line_size() << finl;

  os << v.size_array() << finl;
  os.put(v.addr(), v.size_array(), v.line_size());
  os << finl;
}

// Description: restores a vector saved by dump_vector_with_md.
//  The code must run with the same number of processors.
//  "v" will have a newly created copy of the descriptor (not equal to any
//  other descriptor previously loaded). If "identical" descriptors are needed
//  for several vectors, you might want to call set_md_vector() afterwards to
//  attach another descriptor. See save_matrice_conditionnel()
void MD_Vector_tools::restore_vector_with_md(DoubleVect& v, Entree& is)
{

  v.reset();
  DERIV(MD_Vector_base) md_ptr;
  Nom md_type;
  is >> md_type;
  md_ptr.typer(md_type);
  is >> md_ptr.valeur();

  // Creation du MD_Vector attache au tableau
  MD_Vector md;
  md.copy(md_ptr.valeur());

  Nom unused;
  int line_size;
  is >> unused >> line_size;

  // Astuce pour restaurer le line_size:
  DoubleTab toto;
  toto.resize(md_ptr.valeur().get_nb_items_tot(), line_size, Array_base::NOCOPY_NOINIT);
  int size_tot;
  is >> size_tot;
  is.get(toto.addr(), size_tot);
  toto.set_md_vector(md);

  // Attache v a toto et oublie toto... line_size est transfere sur v
  v.reset();
  v.ref(toto);
}

MD_Vector MD_Vector_tools::extend(const MD_Vector& src, extra_item_t& items)
{
  if (Process::mp_max(items.size()) == 0) return src; //rien a faire
  /* remplissage de : recep[p] -> liste des items qu'on veut recevoir du processeur p
                      items[{p, i}] -> ou on va placer chaque item dans le MD_Vector elargi */
  int i, j, p, nb_items_tot = src.valeur().get_nb_items_tot(), idx = nb_items_tot;
  const MD_Vector_std *mds = NULL;
  if (sub_type(MD_Vector_std, src.valeur())) mds = &ref_cast(MD_Vector_std, src.valeur());
  else if (sub_type(MD_Vector_composite, src.valeur())) mds = &ref_cast(MD_Vector_composite, src.valeur()).global_md_;

  /* recv[p] : items qu'on veut recevoir du processeur p : taille nrecv[p]. On en profite pour les numeroter dans items */
  VECT(ArrOfInt) recv(Process::nproc());
  for (p = 0; p < Process::nproc(); p++) recv[p].set_smart_resize(1);
  std::vector<int> nrecv(Process::nproc());
  for (auto &kv : items) recv[kv.first[0]].append_array(kv.first[1]), kv.second = idx, nrecv[kv.first[0]]++, idx++;

  /* send[p] : items qu'on doit envoyer au processeur p */
  VECT(ArrOfInt) send(Process::nproc());
  envoyer_all_to_all(recv, send);

  std::map<int, std::array<std::vector<int>, 3>> map; //map[p] = { items a envoyer a p, items a recevoir de p, blocs a recevoir de p }

  for (i = 0; i < mds->pe_voisins_.size_array(); i++)
    {
      p = mds->pe_voisins_(i);
      for (j = 0; j < mds->items_to_send_.get_list_size(i); j++) map[p][0].push_back(mds->items_to_send_(i, j));
      for (j = 0; j < mds->items_to_recv_.get_list_size(i); j++) map[p][1].push_back(mds->items_to_recv_(i, j));
      for (j = 0; j < mds->blocs_to_recv_.get_list_size(i); j++) map[p][2].push_back(mds->blocs_to_recv_(i, j));
    }

  /* ajout de ce qu'on doit envoyer/recevoir en plus */
  for (p = 0; p < Process::nproc(); p++) for (i = 0; i < send[p].size_array(); i++) map[p][0].push_back(send[p][i]);
  for (p = 0, idx = nb_items_tot; p < Process::nproc(); idx += nrecv[p], p++)
    if (nrecv[p]) map[p][2].insert(map[p][2].end(), { idx, idx + nrecv[p] });

  /* reconstruction de tableaux pour MD_Vector_std */
  ArrOfInt pe_voisins(map.size());
  VECT(ArrOfInt) items_to_send(map.size()), items_to_recv(map.size()), blocs_to_recv(map.size());
  for (p = 0, i = 0; p < Process::nproc(); nb_items_tot += nrecv[p],  p++) if (map.count(p))
      {
        pe_voisins(i) = p;
        items_to_send[i].resize(map[p][0].size()), std::copy(map[p][0].begin(), map[p][0].end(), items_to_send[i].addr());
        items_to_recv[i].resize(map[p][1].size()), std::copy(map[p][1].begin(), map[p][1].end(), items_to_recv[i].addr());
        blocs_to_recv[i].resize(map[p][2].size()), std::copy(map[p][2].begin(), map[p][2].end(), blocs_to_recv[i].addr());
        i++;
      }

  /* c'est parti */
  MD_Vector_std mdd(nb_items_tot, mds->get_nb_items_reels(), pe_voisins, items_to_send, items_to_recv, blocs_to_recv);
  MD_Vector res;
  res.copy(mdd);
  return res;
}
