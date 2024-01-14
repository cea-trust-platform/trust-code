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

#include <MD_Vector_tools.h>
#include <communications.h>
#include <TRUSTList.h>
#include <Sous_Domaine.h>
#include <Domaine.h>
#include <Octree.h>
#include <TRUSTTrav.h>
#include <Scatter.h>
#include <Poly_geom_base.h>
#include <Array_tools.h>
#include <Schema_Comm.h>
#include <MD_Vector_std.h>
#include <MD_Vector_tools.h>

#include <Interprete_bloc.h>
#include <Extraire_surface.h>
#include <Octree_Double.h>
#include <Periodique.h>
#include <Reordonner_faces_periodiques.h>
#include <Frontiere_dis_base.h>
#include <Frontiere.h>
#include <Conds_lim.h>
#include <NettoieNoeuds.h>
#include <Polyedre.h>
#include <TRUST_2_MED.h>

#ifdef MEDCOUPLING_
using MEDCoupling::DataArrayInt;
using MEDCoupling::DataArrayDouble;
#endif

Implemente_instanciable_sans_constructeur(Domaine,"Domaine",Objet_U);

Domaine::Domaine()
{
  clear();
}

/*! @brief Reset the Domaine completely except for its name.
 */
void Domaine::clear()
{
  sommets_.reset();
  renum_som_perio_.reset();
  elem_ = Elem_geom();
  mes_elems_.reset();
  aretes_som_.reset();
  elem_aretes_.reset();
  mes_faces_bord_.vide();
  mes_faces_raccord_.vide();
  mes_bords_int_.vide();
  mes_groupes_faces_.vide();
  mes_faces_joint_.vide();
  ind_faces_virt_bord_.reset();
  cg_moments_.reset();
  elem_virt_pe_num_.reset();
  domaines_frontieres_.vide();
  les_ss_domaines_.vide();

  moments_a_imprimer_ = 0;
  bords_a_imprimer_.vide();
  bords_a_imprimer_sum_.vide();

  axi1d_ = 0;
  epsilon_ = Objet_U::precision_geom;
  deformable_ = false;
  fichier_lu_ = Nom();

#ifdef MEDCOUPLING_
  mc_mesh_.nullify();
  mc_face_mesh_.nullify();
  rmps.clear();
#endif

  volume_total_ = -1;
}

/*! @brief Ecrit la Domaine sur un flot de sortie.
 *
 * On ecrit le nom, le type des elements, les elements
 *     et les bords, les bords periodiques, les joints, les
 *     raccords et les bords internes.
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Domaine::printOn(Sortie& s) const
{
  Cerr << "Writing of " << nb_som() << " nodes." << finl;
#ifdef SORT_POUR_DEBOG
  s.setf(ios::scientific);
  s.precision(20);
#endif
  s << nom_ << finl;
  s << sommets_;

  // Now write what was formerly the domaines (before TRUST 1.9.2):
  // Write them in the form of a list with a single element, for backward compat (Domains used to contain a list of Zons)
  s << "{" << finl;
  Cerr << "Writing of " << nb_elem() << " elements." << finl;
  s << "DUMMY_ZONE" << finl; // really just to keep a name here for backward compat
  s << elem_ << finl;
  s << mes_elems_;
  s << mes_faces_bord_;
  s << mes_faces_joint_;
  s << mes_faces_raccord_;
  s << mes_bords_int_;
  if (nb_groupes_faces() !=0)
    {
      s << finl << "groupes_faces" << finl;
      s << mes_groupes_faces_;
    }
  s << "}" << finl;
  //

  return s;
}

template<class LIST_FRONTIERE>
void check_frontiere(const LIST_FRONTIERE& list, const char *msg)
{
  int n = list.size();
  if (!is_parallel_object(n))
    {
      Cerr << " Fatal error: processors don't have the same number of boundaries " << msg << finl;
      Process::barrier();
      Process::exit();
    }
  for (int i = 0; i < n; i++)
    {
      const Nom& nom = list[i].le_nom();
      Cerr << "  Boundary " << msg << " : " << nom << finl;
      if (!is_parallel_object(nom))
        {
          Cerr << " Fatal error: processors don't have the same number of boundaries " << msg << finl;
          Process::barrier();
          Process::exit();
        }
    }
}

// S'il y a un proc qui a un type different de vide_OD, on type les faces sur tous
// les processeurs avec ce type:
static void corriger_type(Faces& faces, const Elem_geom_base& type_elem)
{
  int typ = faces.type_face();
  const int pe = (faces.type_face() == Faces::vide_0D) ? Process::nproc() - 1 : Process::me();
  const int min_pe = ::mp_min(pe);
  // Le processeur min_pe envoie son type a tous les autres
  int typ_commun = typ;
  envoyer_broadcast(typ_commun, min_pe);

  if (typ_commun != typ)
    {
      if (typ != Faces::vide_0D)
        {
          Cerr << "Error in Domaine.cpp corriger_type: invalid boundary face type" << finl;
          Process::exit();
        }
      Type_Face tt = (Type_Face) typ_commun;
      faces.typer(tt);
//     int n= faces.nb_som_faces();
      int n = type_elem.nb_som_face();
      faces.les_sommets().resize(0, n);
    }
}

/*! @brief Lit les objets constituant un Domaine a partir d'un flot d'entree.
 *
 * Une fois les objets
 *     lus on les associe au domaine.
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Domaine::readOn(Entree& s)
{
#ifdef SORT_POUR_DEBOG
  s.setf(ios::scientific);
  s.precision(20);
#endif
  // Ajout BM: reset de la structure (a pour effet de debloquer la structure parallele)
  sommets_.reset();
  renum_som_perio_.reset();
  // ne pas faire reset du nom (deja lu)
  // pour deformable je ne sais pas...

  Nom tmp;
  s >> tmp;
  // Si le domaine n'est pas nomme, on prend celui lu
  if (nom_=="??") nom_=tmp;
  Cerr << "Reading domain " << le_nom() << finl;
  s >> sommets_;
  // PL : pas tout a fait exact le nombre affiche de sommets, on compte plusieurs fois les sommets des joints...
  int nbsom = mp_sum(sommets_.dimension(0));
  Cerr << " Number of nodes: " << nbsom << finl;

  // Reading element description (what was fomerly the "domaine" part) - this used to be a list so check for '{ }'
  Nom acc;
  s >> acc;
  assert (acc == "{");
  read_former_domaine(s);
  check_domaine();

  if (Process::is_sequential() && (NettoieNoeuds::NettoiePasNoeuds==0) )
    {
      NettoieNoeuds::nettoie(*this);
      nbsom = mp_sum(sommets_.dimension(0));
      Cerr << " Number of nodes after node-cleanup: " << nbsom << finl;
    }

  // On initialise les descripteurs "sequentiels" (attention, cela bloque le resize des tableaux sommets et elements !)
  Scatter::init_sequential_domain(*this);

  check_domaine();
  return s;
}


/*! @brief read what was (before TRUST 1.9.2) the "domaine" part from the input stream
 * i.e. (roughly) the element description.
 */
void Domaine::read_former_domaine(Entree& s)
{
  Nom dnu, acc;
  Cerr << " Reading part of domain " << le_nom() << finl;
  s >> dnu; // Name of the Domaine, now unused ...
  s >> elem_;
  mes_elems_.reset();
  s >> mes_elems_;
  mes_faces_bord_.vide();
  s >> mes_faces_bord_;
  mes_faces_joint_.vide();
  s >> mes_faces_joint_;
  mes_faces_raccord_.vide();
  s >> mes_faces_raccord_;
  mes_bords_int_.vide();
  s >> mes_bords_int_;
  mes_groupes_faces_.vide();
  s >> acc;
  if (acc == "groupes_faces")
    {
      s >> mes_groupes_faces_;
      s >> acc;
    }
  if (acc != "}")
    Process::exit( "misformatted domain file : One expected a closing bracket } to end. ");
}

/*! @brief associate the read objects to the domaine and check that the reading objects are coherent
 *
 */
void Domaine::check_domaine()
{
  // remplacer Faces::vide_0D par le bon type pour les procs qui n'ont pas de faces de bord:
  {
    int i;
    int n = nb_front_Cl();
    for (i = 0; i < n; i++)
      corriger_type(frontiere(i).faces(), type_elem().valeur());
  }

  if (mes_faces_bord_.size() == 0 && mes_faces_raccord_.size() == 0 && Process::is_sequential())
    Cerr << "Warning, the reread domaine " << nom_ << " has no defined boundaries (none boundary or connector)." << finl;

  mes_faces_bord_.associer_domaine(*this);
  mes_faces_joint_.associer_domaine(*this);
  mes_faces_raccord_.associer_domaine(*this);
  mes_bords_int_.associer_domaine(*this);
  mes_groupes_faces_.associer_domaine(*this);
  elem_.associer_domaine(*this);
  fixer_premieres_faces_frontiere();

  const int nbelem = mp_sum(mes_elems_.dimension(0));
  Cerr << "  Number of elements: " << nbelem << finl;

  // Verifications sanitaires:
  // On doit avoir le meme nombre de frontieres et les memes noms sur tous les procs
  check_frontiere(mes_faces_bord_, "(Bord)");
  check_frontiere(mes_faces_raccord_, "(Raccord)");
  check_frontiere(mes_bords_int_, "(Bord_Interne)");
  check_frontiere(mes_groupes_faces_, "(Groupe_Faces)");
}

Entree& Domaine::lire_bords_a_imprimer(Entree& is)
{
  Nom nom_lu;
  Motcle motlu, accolade_fermee = "}", accolade_ouverte = "{";
  is >> motlu;
  Cerr << "Reading boundaries to be print:" << finl;
  if (motlu != accolade_ouverte)
    {
      Cerr << "We expected a list { ... } of boundaries to be print" << finl;
      Cerr << "and not : " << motlu << finl;
      exit();
    }
  is >> nom_lu;
  while (nom_lu != accolade_fermee)
    {
      Cerr << nom_lu << " ";
      bords_a_imprimer_.add(Nom(nom_lu));
      is >> nom_lu;
    }
  Cerr << finl;
  return is;
}

Entree& Domaine::lire_bords_a_imprimer_sum(Entree& is)
{
  Nom nom_lu;
  Motcle motlu, accolade_fermee = "}", accolade_ouverte = "{";
  is >> motlu;
  Cerr << "Reading boundaries to be print:" << finl;
  if (motlu != accolade_ouverte)
    {
      Cerr << "We expected a list { ... } of boundaries to be print" << finl;
      Cerr << "and not : " << motlu << finl;
      exit();
    }
  is >> nom_lu;
  while (nom_lu != accolade_fermee)
    {
      Cerr << nom_lu << " ";
      bords_a_imprimer_sum_.add(Nom(nom_lu));
      is >> nom_lu;
    }
  Cerr << finl;
  return is;
}


/*! @brief che les numeros (indices) des elements contenants les sommets specifies par le parametre "sommets".
 *
 *     Utilise:
 *      ArrOfInt& Domaine::chercher_elements(const DoubleTab&,ArrOfInt&) const
 *
 * @param (IntTab& sommets) le tableau des numeros des sommets dont on cherche les elements correspondants
 * @param (ArrOfInt& elem_) le tableau contenant les numeros des elements contenant les sommets specifies
 * @return (ArrOfInt&) le tableau des numeros des sommets dont on cherche les elements correspondants
 */
ArrOfInt& Domaine::indice_elements(const IntTab& sommets, ArrOfInt& elem, int reel) const
{
  int i, j, k;
  const DoubleTab& les_coord = sommets_;
  int sz_sommets = sommets.dimension(0);
  DoubleTab xg(sz_sommets, Objet_U::dimension);
  for (i = 0; i < sz_sommets; i++)
    for (j = 0; j < nb_som_elem(); j++)
      for (k = 0; k < Objet_U::dimension; k++)
        xg(i, k) += les_coord(sommets(i, j), k);

  xg /= nb_som_elem();
  return chercher_elements(xg, elem, reel);
}

/*! @brief Recherche des elements contenant les points dont les coordonnees sont specifiees.
 *
 * @param (DoubleTab& positions) les coordonnees des points dont on veut connaitre l'element correspondant
 * @param (ArrOfInt& elements) le tableau des numeros des elements contenant les points specifies
 * @return (ArrOfInt&) le tableau des numeros des elements contenant les points specifies
 */
static double cached_memory = 0;
bool sameDoubleTab(const DoubleTab& a, const DoubleTab& b)
{
  int size_a = a.size_array();
  int size_b = b.size_array();
  if (size_a != size_b)
    return false;
  for (int i = 0; i < size_a; i++)
    if (a.addr()[i] != b.addr()[i])
      return false;
  return true;
}
ArrOfInt& Domaine::chercher_elements(const DoubleTab& positions, ArrOfInt& elements, int reel) const
{
  bool set_cache = false;
  // PL: On devrait faire un appel a chercher_elements(x,y,z,elem) si positions.dimension(0)=1 ...
  if (!deformable() && positions.dimension(0) > 1)
    {
      set_cache = true;
      if (!deriv_octree_.non_nul() || !deriv_octree_.valeur().construit())
        {
          // Vide le cache
          cached_elements_.reset();
          cached_positions_.reset();
        }
      else
        // Recherche dans le cache:
        for (int i = 0; i < cached_positions_.size(); i++)
          if (sameDoubleTab(positions, cached_positions_[i]))
            {
              elements.resize_tab(cached_positions_[i].dimension(0), ArrOfInt::NOCOPY_NOINIT);
              elements = cached_elements_[i];
              /*
               //Cerr << "Reuse " << i << "th array cached in memory for Domaine::chercher_elements(...): " << finl;
               if (i!=0)
               {
               // Permute pour avoir le tableau en premier
               cached_elements_[i].resize_tab(cached_positions_[0].dimension(0), ArrOfInt::NOCOPY_NOINIT);
               cached_elements_[i] = cached_elements_[0];
               cached_elements_[0].resize_tab(cached_positions_[i].dimension(0), ArrOfInt::NOCOPY_NOINIT);
               cached_elements_[0] = elements;
               DoubleTab tmp(cached_positions_[i]);
               cached_positions_[i] = cached_positions_[0];
               cached_positions_[0] = tmp;
               }
               */
              return elements;
            }
    }
  const OctreeRoot& octree = construit_octree(reel);
  int sz = positions.dimension(0);
  const int dim = positions.dimension(1);
  // resize_tab est virtuelle, si c'est un Vect ou un Tab elle appelle le
  // resize de la classe derivee:
  elements.resize_tab(sz, ArrOfInt::NOCOPY_NOINIT);
  double y = 0, z = 0;
  for (int i = 0; i < sz; i++)
    {
      double x = positions(i, 0);
      if (dim > 1)
        y = positions(i, 1);
      if (dim > 2)
        z = positions(i, 2);
      elements[i] = octree.rang_elem(x, y, z);
    }
  if (set_cache)
    {
      // Securite car vu sur un calcul FT (cache qui augmente indefiniment, nombre de particules variables...)
      if (cached_memory>1e8) // 100Mo/proc
        {
          // Vide le cache
          cached_elements_.reset();
          cached_positions_.reset();
        }
      else
        {
          // Met en cache
          cached_positions_.add(positions);
          cached_elements_.add(elements);
          cached_memory += positions.size_array() * (int) sizeof(double);
          cached_memory += elements.size_array() * (int) sizeof(int);
          if (cached_memory > 1e7)   // 10Mo
            {
              Cerr << 2 * cached_positions_.size() << " arrays cached in memory for Zone::chercher_elements(...): ";
              if (cached_memory < 1e6)
                Cerr << int(cached_memory / 1024) << " KBytes" << finl;
              else
                Cerr << int(cached_memory / 1024 / 1024) << " MBytes" << finl;
            }
        }
    }
  return elements;
}

/*! @brief Recherche des elements contenant les points dont les coordonnees sont specifiees.
 *
 * @param (DoubleVect& positions) les coordonnees du point dont on veut connaitre l'element correspondant
 * @param (ArrOfInt& elements) le tableau des numeros des elements contenant les points specifies
 * @return (ArrOfInt&) le tableau des numeros des elements contenant les points specifies
 */
ArrOfInt& Domaine::chercher_elements(const DoubleVect& positions, ArrOfInt& elements, int reel) const
{
  //  Cerr<<"Dans Domaine::chercher_elements(const DoubleVect&"<<finl;
  //  Cerr<<"codage lourd....."<<finl;
  int n = positions.size();
  if (n != dimension)
    {
      Cerr << "Domaine::chercher_elements(const DoubleVect& positions, ArrOfInt& elements, int reel) const -> Coding is made to copy a doublevect(dimesnion) in a DoubleTab(1,dimension)" << finl;
      Cerr << "But, it comes with a DoubleVect of size " << n << " instead of " << dimension << finl;
      assert(0);
      exit();
    }
  DoubleTab positions2(1, n);
  for (int ii = 0; ii < n; ii++)
    positions2(0, ii) = positions(ii);
  return chercher_elements(positions2, elements, reel);
}

/*! @brief Renvoie le nombre de faces qui sont des bords.
 *
 * @return (int) le nombre de faces qui sont des bords
 */
int Domaine::nb_faces_bord() const
{
  return mes_faces_bord_.nb_faces();
}

/*! @brief Renvoie le nombre de joints du domaine.
 *
 * @return (int) le nombre de joints du domaine
 */
int Domaine::nb_faces_joint() const
{
  return mes_faces_joint_.nb_faces();
}

/*! @brief Renvoie le nombre de raccords du domaine.
 *
 * @return (int) le nombre de raccords du domaine
 */
int Domaine::nb_faces_raccord() const
{
  return mes_faces_raccord_.nb_faces();
}

/*! @brief Renvoie le nombre de face de bord internes du domaine.
 *
 * @return (int) le nombre de face bords internes du domaine
 */
int Domaine::nb_faces_bords_int() const
{
  return mes_bords_int_.nb_faces();
}

/*! @brief Renvoie le nombre de faces dans les Groupe_Faces du domaine.
 *
 * @return (int) le nombre de faces dans les Groupe_Faces du domaine
 */
int Domaine::nb_faces_groupes_faces() const
{
  return mes_groupes_faces_.nb_faces();
}

/*! @brief Renvoie le nombre de sommets du domaine.
 *
 * @return (int) le nombre de sommets du domaine
 */
int Domaine::nb_som() const
{
  return sommets_.dimension(0);
}

/*! @brief Renvoie le nombre total de sommets du domaine.
 *
 * i.e. le nombre de sommets reels et virtuels
 *     sur le processeur courant.
 *
 * @return (int) le nombre total de sommets du domaine
 */
int Domaine::nb_som_tot() const
{
  return sommets_.dimension_tot(0);
}

/*! @brief Renvoie le nombre de faces du i-ieme bord.
 *
 * @param (int i) le numero du bord dont on veut connaitre le nombre de faces
 * @return (int) le nombre de faces du i-ieme bord
 */
int Domaine::nb_faces_bord(int i) const
{
  return mes_faces_bord_(i).nb_faces();
}

/*! @brief Renvoie le nombre de faces du i-ieme joint.
 *
 * @param (int i) le numero du joint dont on veut connaitre le nombre de faces
 * @return (int) nombre de faces du i-ieme joint
 */
int Domaine::nb_faces_joint(int i) const
{
  return mes_faces_joint_(i).nb_faces();
}

/*! @brief Renvoie le nombre de faces du i-ieme raccord.
 *
 * @param (int i) le numero du raccord dont on veut connaitre le nombre de faces
 * @return (int) le nombre de faces du i-ieme raccord
 */
int Domaine::nb_faces_raccord(int i) const
{
  return mes_faces_raccord_(i)->nb_faces();
}
/*! @brief Renvoie le nombre de faces de la i-ieme liste de faces de bords internes
 *
 * @param (int i) le numero de la liste de bords internes dont on veut connaitre le nombre de faces
 * @return (int i) le nombre de faces de la i-ieme liste de bords internes
 */
int Domaine::nb_faces_bords_int(int i) const
{
  return mes_bords_int_(i).nb_faces();
}

/*! @brief Renvoie le nombre de faces de la i-ieme liste de groupes de faces
 *
 * @param (int i) le numero de la liste de groupes de faces dont on veut connaitre le nombre de faces
 * @return (int i) le nombre de faces de la i-ieme liste de groupes de faces
 */
int Domaine::nb_faces_groupes_faces(int i) const
{
  return mes_groupes_faces_(i).nb_faces();
}

/*! @brief Renumerotation des noeuds: Le noeud de numero k devient le noeud de numero Les_Nums[k]
 *
 * @param (IntVect& Les_Nums) le vecteur contenant la nouvelle numerotation Nouveau_numero_noeud_i = Les_Nums[Ancien_numero_noeud_i]
 */
void Domaine::renum(const IntVect& Les_Nums)
{
  int dim0 = mes_elems_.dimension(0);
  int dim1 = mes_elems_.dimension(1);

  for (int i = 0; i < dim0; i++)
    for (int j = 0; j < dim1; j++)
      mes_elems_(i, j) = Les_Nums[mes_elems_(i, j)];

  for (int i = 0; i < nb_bords(); i++)
    mes_faces_bord_(i).renum(Les_Nums);
  for (int i = 0; i < nb_joints(); i++)
    mes_faces_joint_(i).renum(Les_Nums);
  for (int i = 0; i < nb_raccords(); i++)
    mes_faces_raccord_(i)->renum(Les_Nums);
  for (int i = 0; i < nb_frontieres_internes(); i++)
    mes_bords_int_(i).renum(Les_Nums);
  for (int i = 0; i < nb_groupes_faces(); i++)
    mes_groupes_faces_(i).renum(Les_Nums);
}

/*! @brief Renumerotation des noeuds et des elements presents dans les items communs des joints Le noeud de numero k devient le noeud de numero Les_Nums[k]
 *
 *       l'element de numero e devient l'element de numero e+elem_offset
 *
 * @param (IntVect& Les_Nums) le vecteur contenant la nouvelle numerotation Nouveau_numero_noeud_i = Les_Nums[Ancien_numero_noeud_i]
 */
void Domaine::renum_joint_common_items(const IntVect& Les_Nums, const int elem_offset)
{
  for (int i_joint = 0; i_joint < nb_joints(); i_joint++)
    {
      ArrOfInt& sommets_communs = mes_faces_joint_[i_joint].set_joint_item(Joint::SOMMET).set_items_communs();
      for (int index = 0; index < sommets_communs.size_array(); index++)
        sommets_communs[index] = Les_Nums[sommets_communs[index]];

      ArrOfInt& elements_distants = mes_faces_joint_[i_joint].set_joint_item(Joint::ELEMENT).set_items_distants();
      elements_distants += elem_offset;
    }
}

/*! @brief Renvoie -1 si face n'est pas une face de bord interne Renvoie le numero de la face dupliquee sinon.
 *
 * @param (int face) le numero de la face de bord interne a chercher
 * @return (int) -1 si la face specifiee n'est pas une face de bord interne le numero de la face dupliquee sinon
 * @throws erreur TRUST (face non trouvee)
 */
int Domaine::face_bords_interne_conjuguee(int face) const
{
  if ((face) >= nb_faces_frontiere())
    return -1;
  int compteur = nb_faces_frontiere() - nb_faces_bords_int();
  if ((face) < compteur)
    return -1;

  for (const auto& itr : mes_bords_int_)
    {
      const Faces& les_faces = itr.faces();
      int nbf = les_faces.nb_faces();
      if (face < nbf + compteur)
        {
          nbf /= 2;
          if ((face - compteur) < nbf)
            return face + nbf;
          else
            return face - nbf;
        }
      compteur += (2 * nbf);
    }

  Cerr << "TRUST error in Domaine::face_bords_interne_conjuguee " << finl;
  Process::exit();
  return -1;
}


/*! @brief Correcting type of borders if they were empty before merge (ie equal to vide_0D)
 *
 * Difference with corriger_type is that we don't want to delete faces inside borders afterwards.
 */
void Domaine::correct_type_of_borders_after_merge()
{
  {
    // Les Bords
    auto& list = mes_faces_bord_.get_stl_list();

    // first loop over list elements
    for (auto it = list.begin(); it != list.end(); ++it)
      {
        Frontiere& front = *it;
        if (front.faces().type_face() == Faces::vide_0D)
          {
            // second loop over list elements, starting from an incremented position
            for (auto it2 = std::next(it); it2 != list.end();)
              {
                Frontiere& front2 = *it2;
                if (front.le_nom() == front2.le_nom())
                  {
                    front.faces().typer(front2.faces().type_face());
                    break;
                  }
                else
                  ++it2;
              }
          }
      }
  }

  {
    // Les Bords Internes :
    auto& list = mes_bords_int_.get_stl_list();
    for (auto it = list.begin(); it != list.end(); ++it)
      {
        Frontiere& front = *it;
        if (front.faces().type_face() == Faces::vide_0D)
          {
            for (auto it2 = std::next(it); it2 != list.end();)
              {
                Frontiere& front2 = *it2;
                if (front.le_nom() == front2.le_nom())
                  {
                    front.faces().typer(front2.faces().type_face());
                    break;
                  }
                else
                  ++it2;
              }
          }
      }
  }

  {
    // Les Raccords
    auto& list = mes_faces_raccord_.get_stl_list();
    for (auto it = list.begin(); it != list.end(); ++it)
      {
        Frontiere& front = (*it).valeur();
        if (front.faces().type_face() == Faces::vide_0D)
          {
            for (auto it2 = std::next(it); it2 != list.end();)
              {
                Frontiere& front2 = (*it2).valeur();
                if (front.le_nom() == front2.le_nom())
                  {
                    front.faces().typer(front2.faces().type_face());
                    break;
                  }
                else
                  ++it2;
              }
          }
      }
  }

  {
    // Les Groupes de faces:
    auto& list = mes_groupes_faces_.get_stl_list();
    for (auto it = list.begin(); it != list.end(); ++it)
      {
        Frontiere& front = *it;
        if (front.faces().type_face() == Faces::vide_0D)
          {
            for (auto it2 = std::next(it); it2 != list.end();)
              {
                Frontiere& front2 = *it2;
                if (front.le_nom() == front2.le_nom())
                  {
                    front.faces().typer(front2.faces().type_face());
                    break;
                  }
                else
                  ++it2;
              }
          }
      }
  }

  {
    //Les joints
    auto& list = mes_faces_joint_.get_stl_list();
    for (auto it = list.begin(); it != list.end(); ++it)
      {
        Frontiere& front = *it;
        if (front.faces().type_face() == Faces::vide_0D)
          {
            for (auto it2 = std::next(it); it2 != list.end();)
              {
                Frontiere& front2 = *it2;
                if (front.le_nom() == front2.le_nom())
                  {
                    front.faces().typer(front2.faces().type_face());
                    break;
                  }
                else
                  ++it2;
              }
          }
      }
  }
}

/*! @brief Concatene les joints de meme nom
 *
 */
int Domaine::comprimer_joints()
{
  auto& list = mes_faces_joint_.get_stl_list();
  for (auto it = list.begin(); it != list.end(); ++it)
    {
      Frontiere& front = *it;
      for (auto it2 = std::next(it); it2 != list.end();)
        {
          Frontiere& front2 = *it2;
          if (front.le_nom() == front2.le_nom())
            {
              front.add(front2);
              it2 = list.erase(it2);
            }
          else
            ++it2;
        }
    }
  return 1;
}

/*! @brief Concatene les bords de meme nom et ceci pour: les bords, les bords periodiques, les bords internes et les groupes de faces.
 *
 */
int Domaine::comprimer()
{
  {
    // Les Bords
    auto& list = mes_faces_bord_.get_stl_list();

    // first loop over list elements
    for (auto it = list.begin(); it != list.end(); ++it)
      {
        Frontiere& front = *it;
        front.associer_domaine(*this); // Au cas ou le domaine de la frontiere n'est pas la bonne domaine
        Journal() << "Domaine::comprimer() bord : " << front.le_nom() << finl;

        // second loop over list elements, starting from an incremented position
        for (auto it2 = std::next(it); it2 != list.end(); )
          {
            Frontiere& front2 = *it2;
            if (front.le_nom() == front2.le_nom())
              {
                Journal() << "On agglomere le bord : " << front.le_nom() << finl;
                front.add(front2);
                it2 = list.erase(it2);
              }
            else
              ++it2;

            Journal() << front.le_nom() << " est associee a : " << front.domaine().le_nom() << finl;
          }
      }
  }

  {
    // Les Bords Internes :
    auto& list = mes_bords_int_.get_stl_list();
    for (auto it = list.begin(); it != list.end(); ++it)
      {
        Frontiere& front = *it;
        for (auto it2 = std::next(it); it2 != list.end(); )
          {
            Frontiere& front2 = *it2;
            if (front.le_nom() == front2.le_nom())
              {
                front.add(front2);
                it2 = list.erase(it2);
              }
            else
              ++it2;
          }
      }
  }

  {
    // Les Groupes de faces :
    auto& list = mes_groupes_faces_.get_stl_list();
    for (auto it = list.begin(); it != list.end(); ++it)
      {
        Frontiere& front = *it;
        for (auto it2 = std::next(it); it2 != list.end(); )
          {
            Frontiere& front2 = *it2;
            if (front.le_nom() == front2.le_nom())
              {
                front.add(front2);
                it2 = list.erase(it2);
              }
            else
              ++it2;
          }
      }
  }

  {
    // Les Raccords
    auto& list = mes_faces_raccord_.get_stl_list();
    for (auto it = list.begin(); it != list.end(); ++it)
      {
        Frontiere& front = (*it).valeur();
        Journal() << "Raccord : " << front.le_nom() << finl;
        for (auto it2 = std::next(it); it2 != list.end(); )
          {
            Frontiere& front2 = (*it2).valeur();
            if (front.le_nom() == front2.le_nom())
              {
                front.add(front2);
                it2 = list.erase(it2);
              }
            else
              ++it2;
          }
      }
  }
  return 1;
}

/*! @brief Merge another Domaine into this, without considering vertices which are handled separately
 *
 */
void Domaine::merge_wo_vertices_with(Domaine& dom2)
{
  Cerr << "   Merging elem info for domain "<< nom_ << " with " << dom2.nom_ << finl;

  // Prepare type if first merge:
  if (!elem_.non_nul())
    elem_ = dom2.elem_;

  // Prepare correct initial elem size if first merge
  if (nb_elem() == 0)
    les_elems().resize(0, dom2.les_elems().dimension(1));

  int sz1 = les_elems().dimension(0);
  int sz2 = dom2.les_elems().dimension(0);
  int nb_ccord = les_elems().dimension(1);
  IntTab& elems1 = les_elems();
  IntTab& elems2 = dom2.les_elems();
  elems1.resize(sz1+sz2, nb_ccord);
  for(int i=0; i<sz2; i++)
    for(int j=0; j<nb_ccord; j++)
      elems1(sz1+i,j)=elems2(i,j);

  dom2.faces_bord().associer_domaine(*this);
  faces_bord().add(dom2.faces_bord());

  dom2.faces_joint().associer_domaine(*this);
  faces_joint().add(dom2.faces_joint());

  dom2.faces_raccord().associer_domaine(*this);
  faces_raccord().add(dom2.faces_raccord());

  dom2.bords_int().associer_domaine(*this);
  bords_int().add(dom2.bords_int());

  dom2.groupes_faces().associer_domaine(*this);
  groupes_faces().add(dom2.groupes_faces());

  correct_type_of_borders_after_merge();
  comprimer();
  comprimer_joints();
  invalide_octree();
}

/*! @brief Fills the Domaine from a list of Domaine objects by aggregating them.
 *
 * See Mailler for example
 */
void Domaine::fill_from_list(std::list<Domaine*>& lst)
{
  Cerr << "Filling domain from list of domains in progress... " << finl;
  if (Process::is_parallel())
    Process::exit("Error in Domaine::fill_from_list() : compression prohibited in parallel mode");
  if (lst.size() == 0)
    Process::exit("Error in Domaine::fill_from_list() : compression prohibited in parallel mode");

  for(auto& elem: lst)
    elem->comprimer();

#ifndef NDEBUG
  Domaine& fst_dom = *lst.front();
  Nom typ_elem = fst_dom.type_elem()->que_suis_je();
#endif
  for(auto& it: lst)
    {
      Domaine& dom2 = *it;
      Cerr << "   Concatenating Domains "<< nom_ << " and " << dom2.nom_ << finl;;
      // Check single geometrical type:
      assert(typ_elem == dom2.type_elem()->que_suis_je());
      // Handle nodes:
      IntVect les_nums;
      // Copy sommets to this
      ajouter(dom2.sommets_, les_nums);  // les_nums: out parameter
      // Renumber current Domaine to prepare addition of elements
      dom2.renum(les_nums);
      // Merge elem info:
      merge_wo_vertices_with(dom2);
    }

  Cerr << "Filling from list - End!" << finl;
}


/*! @brief Ecriture des noms des bords sur un flot de sortie
 *
 * Ecrit les noms des: bords, bords periodiques, raccords et groupes de faces.
 *
 * @param (Sortie& os) un flot de sortie
 */
void Domaine::ecrire_noms_bords(Sortie& os) const
{
  // Les Bords
  for (const auto &itr : mes_faces_bord_)
    os << itr.le_nom() << finl;

  // Les Raccords :
  for (const auto &itr : mes_faces_raccord_)
    os << itr->le_nom() << finl;

  // Les Bords Internes :
  for (const auto &itr : mes_bords_int_)
    os << itr.le_nom() << finl;

  // Les Groupes de faces :
  for (const auto &itr : mes_groupes_faces_)
    os << itr.le_nom() << finl;
}

double Domaine::epsilon() const
{
  return epsilon_;
}

/*! @brief Renvoie le nombre de faces du type specifie.
 *
 * @param (Type_Face type) un type de face
 * @return (int) le nombre de faces du type specifie
 */
int Domaine::nb_faces_bord(Type_Face type) const
{
  return mes_faces_bord_.nb_faces(type);
}

/*! @brief Renvoie le nombre de joints du type specifie
 *
 * @param (Type_Face type) un type de face
 * @return (int) le nombre de joints du type specifie
 */
int Domaine::nb_faces_joint(Type_Face type) const
{
  return mes_faces_joint_.nb_faces(type);
}

/*! @brief Renvoie le nombre de raccords du type specifie
 *
 * @param (Type_Face type) un type de face
 * @return (int) le nombre de raccords du type specifie
 */
int Domaine::nb_faces_raccord(Type_Face type) const
{
  return mes_faces_raccord_.nb_faces(type);
}

/*! @brief Renvoie le nombre de faces de bords interieures du type specifie
 *
 * @param (Type_Face type) un type de face
 * @return (int) le nombre de faces de bords interieures du type specifie
 */
int Domaine::nb_faces_bords_int(Type_Face type) const
{
  return mes_bords_int_.nb_faces(type);
}

/*! @brief Renvoie le nombre de goupes de faces du type specifie
 *
 * @param (Type_Face type) un type de face
 * @return (int) le nombre de groupes de faces du type specifie
 */
int Domaine::nb_faces_groupes_faces(Type_Face type) const
{
  return mes_groupes_faces_.nb_faces(type);
}

/*! @brief Renvoie le rang de l'element contenant le point dont les coordonnees sont specifiees.
 *
 * @param (double x) coordonnee en X
 * @param (double y) coordonnee en Y
 * @param (double z) coordonnee en Z
 * @return (int) le rang de l'element contenant le point dont les coordonnees sont specifiees.
 */
int Domaine::chercher_elements(double x, double y, double z, int reel) const
{
  const OctreeRoot& octree = construit_octree(reel);
  return octree.rang_elem(x, y, z);
}

/*! @brief
 *
 * @param (DoubleTab& pos)
 * @param (ArrOfInt& som)
 * @return (ArrOfInt&)
 */
ArrOfInt& Domaine::chercher_sommets(const DoubleTab& pos, ArrOfInt& som, int reel) const
{
  const OctreeRoot& octree = construit_octree(reel);
  octree.rang_sommet(pos, som);
  return som;
}

/*! @brief
 *
 * @param (DoubleTab& pos)
 * @param (IntTab& aretes_som) la definition des aretes par leurs sommets
 * @return (ArrOfInt& aretes) Liste des aretes trouvees
 */
ArrOfInt& Domaine::chercher_aretes(const DoubleTab& pos, ArrOfInt& aretes, int reel) const
{
  const OctreeRoot& octree = construit_octree(reel);
  octree.rang_arete(pos, aretes);
  return aretes;
}

/*! @brief
 *
 * @param (double x) coordonnee en X
 * @param (double y) coordonnee en Y
 * @param (double z) coordonnee en Z
 */
int Domaine::chercher_sommets(double x, double y, double z, int reel) const
{
  const OctreeRoot& octree = construit_octree(reel);
  return octree.rang_sommet(x, y, z);
}

int Domaine::rang_frontiere(const Nom& un_nom) const
{
  int i = 0;
  for (const auto &itr : mes_faces_bord_)
    {
      if (itr.le_nom() == un_nom)
        return i;
      ++i;
    }

  for (const auto &itr : mes_faces_raccord_)
    {
      if (itr->le_nom() == un_nom)
        return i;
      ++i;
    }

  for (const auto &itr : mes_bords_int_)
    {
      if (itr.le_nom() == un_nom)
        return i;
      ++i;
    }

  for (const auto &itr : mes_groupes_faces_)
    {
      if (itr.le_nom() == un_nom)
        return i;
      ++i;
    }
  Cerr << "Domaine::rang_frontiere(): We have not found a boundary with name " << un_nom << finl;
  Process::exit();
  return -1;
}

const Frontiere& Domaine::frontiere(const Nom& un_nom) const
{
  int i = rang_frontiere(un_nom);
  return frontiere(i);
}

Frontiere& Domaine::frontiere(const Nom& un_nom)
{
  int i = rang_frontiere(un_nom);
  return frontiere(i);
}

void Domaine::fixer_premieres_faces_frontiere()
{
  Journal() << "Domaine::fixer_premieres_faces_frontiere()" << finl;
  int compteur = 0;
  for (auto &itr : mes_faces_bord_)
    {
      itr.fixer_num_premiere_face(compteur);
      compteur += itr.nb_faces();
      Journal() << "Le bord " << itr.le_nom() << " commence a la face : " << itr.num_premiere_face() << finl;
    }
  for (auto &itr : mes_faces_raccord_)
    {
      itr->fixer_num_premiere_face(compteur);
      compteur += itr->nb_faces();
      Journal() << "Le raccord " << itr->le_nom() << " commence a la face : " << itr->num_premiere_face() << finl;
    }
  for (auto &itr : mes_faces_joint_)
    {
      itr.fixer_num_premiere_face(compteur);
      compteur += itr.nb_faces();
      Journal() << "Le joint " << itr.le_nom() << " commence a la face : " << itr.num_premiere_face() << finl;
    }
  for (auto &itr : mes_groupes_faces_)
    itr.fixer_num_premiere_face(-1);
}

// Construction du tableau elem_virt_pe_num_ a partir du tableau mes_elems
// (on se sert des espaces distants et virtuels de mes_elems).
// Algorithme non optimal en memoire : on duplique mes_elems alors qu'on a
// besoin que d'un tableau a deux colonnes.
// Voir Domaine.h : elem_virt_pe_num_
void Domaine::construire_elem_virt_pe_num()
{
  IntTab tableau_echange(mes_elems_);
  assert(tableau_echange.dimension(1) >= 2);
  int i;
  const int n = nb_elem();
  const int n_virt = nb_elem_tot() - n;
  const int moi = me();
  for (i = 0; i < n; i++)
    {
      tableau_echange(i, 0) = moi;
      tableau_echange(i, 1) = i;
    }
  tableau_echange.echange_espace_virtuel();

  elem_virt_pe_num_.resize(n_virt, 2);
  for (i = 0; i < n_virt; i++)
    {
      elem_virt_pe_num_(i, 0) = tableau_echange(n + i, 0);
      elem_virt_pe_num_(i, 1) = tableau_echange(n + i, 1);
    }
}

void Domaine::construire_elem_virt_pe_num(IntTab& elem_virt_pe_num_cpy) const
{
  IntTab tableau_echange(mes_elems_);
  assert(tableau_echange.dimension(1) >= 2);
  int i;
  const int n = nb_elem();
  const int n_virt = nb_elem_tot() - n;
  const int moi = me();
  for (i = 0; i < n; i++)
    {
      tableau_echange(i, 0) = moi;
      tableau_echange(i, 1) = i;
    }
  tableau_echange.echange_espace_virtuel();

  elem_virt_pe_num_cpy.resize(n_virt, 2);
  for (i = 0; i < n_virt; i++)
    {
      elem_virt_pe_num_cpy(i, 0) = tableau_echange(n + i, 0);
      elem_virt_pe_num_cpy(i, 1) = tableau_echange(n + i, 1);
    }
}

const IntTab& Domaine::elem_virt_pe_num() const
{
  return elem_virt_pe_num_;
}
/*! @brief Calcule le centre de gravite du domaine
 *
 */
void Domaine::calculer_mon_centre_de_gravite(ArrOfDouble& c)
{
  c = 0;
  // Volumes computed cause stored in Domaine_VF and so not available in Domaine...
  DoubleVect volumes;
  DoubleVect inverse_volumes;
  calculer_volumes(volumes, inverse_volumes);
  DoubleTab xp;
  calculer_centres_gravite(xp);
  double volume = 0;
  for (int i = 0; i < nb_elem(); i++)
    for (int j = 0; j < dimension; j++)
      {
        c[j] += xp(i, j) * volumes(i);
        volume += volumes(i);
      }
  // Cas de Domaine vide:
  if (volume > 0)
    c /= volume;
  cg_moments_ = c;
  volume_total_ = mp_somme_vect(volumes);
}

/*! @brief Calcule les volumes des elements du domaine.
 *
 * @param (DoubleVect& volumes) le tableau contenant les volumes des elements du domaine
 */
void Domaine::calculer_volumes(DoubleVect& volumes, DoubleVect& inverse_volumes) const
{
  if (!volumes.get_md_vector().non_nul())
    creer_tableau_elements(volumes, Array_base::NOCOPY_NOINIT);
  elem_.calculer_volumes(volumes); // Dimensionne et calcule le DoubleVect volumes
  // Check and fill inverse_volumes
  if (!inverse_volumes.get_md_vector().non_nul())
    creer_tableau_elements(inverse_volumes, Array_base::NOCOPY_NOINIT);
  int size = volumes.size_totale();
  for (int i = 0; i < size; i++)
    {
      double v = volumes(i);
      if (v <= 0.)
        {
          Cerr << "Volume[" << i << "]=" << v << finl;
          Cerr << "Several volumes of the mesh are not positive." << finl;
          Cerr << "Something is wrong in the mesh..." << finl;
          exit();
        }
      inverse_volumes(i) = 1. / v;
    }
}

/*! @brief Calcule les centres de gravites des aretes du domaine.
 *
 * @param (DoubleTab& xa) le tableau contenant les centres de gravites des aretes du domaine
 */
void Domaine::calculer_centres_gravite_aretes(DoubleTab& xa) const
{
  const DoubleTab& coord = sommets_;
  // Calcule les centres de gravite des aretes reelles seulement
  xa.resize(nb_aretes(), dimension);
  for (int i = 0; i < nb_aretes(); i++)
    for (int j = 0; j < dimension; j++)
      xa(i, j) = 0.5 * (coord(aretes_som_(i, 0), j) + coord(aretes_som_(i, 1), j));
}

void Domaine::rang_elems_sommet(ArrOfInt& elems, double x, double y, double z) const
{
  const OctreeRoot octree = construit_octree();
  octree.rang_elems_sommet(elems, x, y, z);
}

void Domaine::invalide_octree()
{
  if (deriv_octree_.non_nul())
    deriv_octree_.detach();
}

const OctreeRoot& Domaine::construit_octree() const
{
  if (!deriv_octree_.non_nul())
    deriv_octree_.typer("OctreeRoot");
  OctreeRoot& octree = deriv_octree_.valeur();
  if (!octree.construit())
    {
      octree.associer_Domaine(*this);
      octree.construire();
    }
  return octree;
}

/*! @brief construction de l'octree si pas deja fait
 *
 */
const OctreeRoot& Domaine::construit_octree(int& reel) const
{
  if (!deriv_octree_.non_nul())
    deriv_octree_.typer("OctreeRoot");
  OctreeRoot& octree = deriv_octree_.valeur();
  if (!octree.construit() || (reel != octree.reel()))
    {
      octree.associer_Domaine(*this);
      octree.construire(reel);
    }
  return octree;
}

/*! @brief creation d'un tableau parallele de valeurs aux elements.
 *
 * Voir MD_Vector_tools::creer_tableau_distribue()
 *
 */
void Domaine::creer_tableau_elements(Array_base& x, Array_base::Resize_Options opt) const
{
  const MD_Vector& md = md_vector_elements();
  MD_Vector_tools::creer_tableau_distribue(md, x, opt);
}

/*! @brief renvoie le descripteur parallele des tableaux aux elements du domaine
 *
 */
const MD_Vector& Domaine::md_vector_elements() const
{
  const MD_Vector& md = mes_elems_.get_md_vector();
  if (!md.non_nul())
    {
      Cerr << "Internal error in Domaine::md_vector_elements(): descriptor for elements not initialized\n"
           << " You might use a buggy Domain constructor that does not build descriptors,\n"
           << " Use the following syntax to finish the domain construction\n"
           << "  Scatter ; " << le_nom() << finl;
      exit();
    }
  // Pour l'instant je prends le descripteur dans le tableau mes_elems, mais on
  // pourrait en stocker une copie dans le domaine si ca a un interet...
  return md;
}

double Domaine::volume_total() const
{
  assert(volume_total_ >= 0.); // Pas calcule ???
  return volume_total_;
}

// Selection d'un item unique (sommet, face ...) parmi une liste (item_possible)
// afin d'assurer le parallelisme de certains algorithmes
// La selection est faite en testant la distance entre les coordonnees (coord_possible)
// localisant ces items par rapport aux coordonnes (coord_ref) d'un point de reference.
// L'item retenu est celui qui presente la distance minimum par rapport au point de reference.
// S'il reste plusieurs items se trouvant a la meme distance du point de reference
// alors on repete le test en translatant le point de reference

int Domaine::identifie_item_unique(IntList& item_possible, DoubleTab& coord_possible, const DoubleVect& coord_ref)
{
  int it_selection = -1;
  DoubleTab decentre_face(4, Objet_U::dimension);
  decentre_face = 0.;
  for (int t = 1; t < 4; t++)
    for (int dir = 0; dir < Objet_U::dimension; dir++)
      if (dir == (t - 1))
        decentre_face(t, dir) = 1.;
  // decentre_face(0,0:dim)={0,0,0}
  // decentre_face(1,0:dim)={1,0,0}
  // decentre_face(2,0:dim)={0,1,0}
  // decentre_face(3,0:dim)={0,0,1}

  //Au premier passage (t=0) pas de translation effectuee
  DoubleVect dist;
  assert(item_possible.size() != 0);
  int t = 0;
  while ((item_possible.size() != 1) && (t < 4))
    {
      double distmin = DMAXFLOAT;
      int size_initiale = item_possible.size();
      dist.resize(size_initiale);
      dist = 0.;

      for (int ind_it = 0; ind_it < size_initiale; ind_it++)
        {
          for (int dir = 0; dir < Objet_U::dimension; dir++)
            dist[ind_it] += (coord_possible(ind_it, dir) - (coord_ref(dir) + decentre_face(t, dir))) * (coord_possible(ind_it, dir) - (coord_ref(dir) + decentre_face(t, dir)));
          if (dist[ind_it] <= distmin)
            distmin = dist[ind_it];
        }

      int ind_it = 0;
      int nb_it_suppr = 0;
      while (ind_it < size_initiale)
        {
          if (!est_egal(dist[ind_it], distmin))
            {
              int ind_it_suppr = ind_it - nb_it_suppr;
              int it_suppr = item_possible[ind_it_suppr];
              item_possible.suppr(it_suppr);

              int size_actuelle = item_possible.size();
              for (int ind = ind_it_suppr; ind < size_actuelle; ind++)
                for (int dir = 0; dir < dimension; dir++)
                  coord_possible(ind, dir) = coord_possible(ind + 1, dir);
              coord_possible.resize(size_actuelle, dimension, Array_base::COPY_NOINIT);
              nb_it_suppr++;
            }
          ind_it++;
        }
      t++;
    }
  if (item_possible.size() == 1)
    it_selection = item_possible[0];
  else
    {
      Cerr << "Domaine::identifie_item_unique()" << finl;
      Cerr << "An item has not been found among the list." << finl;
      Cerr << "Please contact TRUST support." << finl;
      Process::exit();
    }
  return it_selection;
}

/*! @brief Methode appelee par Domaine_VF::discretiser().
 *
 * Construction du descripteur pour les faces de bord
 *   Remplissage de ind_faces_virt_bord et des tableaux get_faces_virt() des frontieres
 *   a partir du descripteur parallele des faces.
 *   Note B.M.: le fait d'avoir mis les faces dans le Domaine_VF, les aretes dans le Domaine,
 *    certaines parties des proprietes des faces de bord dans le Domaine_VF et d'autres dans le Domaine
 *    fait que l'initialisation passe par des chemins un peu tordus... il faudra nettoyer ca.
 *
 */
void Domaine::init_faces_virt_bord(const MD_Vector& md_vect_faces, MD_Vector& md_vect_faces_front)
{
  // ***************************************
  // 1) Construction des structures de tableaux pour toutes les faces de bord
  //   (faces de 0 a nb_faces_frontiere())
  const int nb_faces_fr = nb_faces_frontiere();
  //  Marquage des faces de bord (-1=>pas une face de bord, 0=>face de bord)
  IntVect vect_renum;
  MD_Vector_tools::creer_tableau_distribue(md_vect_faces, vect_renum, Array_base::NOCOPY_NOINIT);
  vect_renum = -1;
  for (int i = 0; i < nb_faces_fr; i++)
    vect_renum[i] = 0;
  vect_renum.echange_espace_virtuel();

  // Creation du descripteur pour les faces de bord (par extraction d'un sous ensemble du descripteur
  //  des faces). On utilise la numerotation par defaut dans l'ordre croissant:
  MD_Vector_tools::creer_md_vect_renum_auto(vect_renum, md_vect_faces_front);

  //  Remplissage du tableau ind_faces_virt_bord. C'est juste la partie virtuelle du tableau renum.
  //  (la partie reelle est triviale: c'est une numerotation contigue de 0 a nb_faces_frontiere()
  const int nb_faces = vect_renum.size();
  const int nb_faces_tot = vect_renum.size_totale();
  const int nb_faces_virt = nb_faces_tot - nb_faces;
  ind_faces_virt_bord_.resize_array(nb_faces_virt, Array_base::NOCOPY_NOINIT);
  for (int i = 0; i < nb_faces_virt; i++)
    ind_faces_virt_bord_[i] = vect_renum[nb_faces + i];

  // **************************************
  // 2) Construction des structures de tableaux pour chaque frontiere

  // Remplissage des tableaux
  //   frontiere(i).get_faces_virt() pour 0 <= i < nb_front_Cl()
  // Ce tableau contient les indices dans la Domaine_VF des faces virtuelles
  // qui sont sur la frontiere i.
  // Calcul de l'espace virtuel des faces de chaque frontiere

  // Nombre de frontieres:
  const int nb_frontieres = nb_front_Cl();
  int i_frontiere;
  // Remplissage des tableaux get_faces_virt():
  // et constructrion des MD_Vector de chaque frontiere (associe au tableau des faces)
  for (i_frontiere = 0; i_frontiere < nb_frontieres; i_frontiere++)
    {
      Frontiere& front = frontiere(i_frontiere);
      IntTab& faces_sommets_frontiere = front.les_sommets_des_faces();
      // Certains problemes ont plusieurs objets Domaine_VF attaches a la meme Domaine (rayonnement)
      // Si on est deja passe par ici, ne pas refaire le travail:
      if (faces_sommets_frontiere.get_md_vector().non_nul())
        {
          continue;
        }
      //les tableaux faces_sommets_frontiere doivent faire la meme largeur sur tous les procs avant echange
      int nb_som_faces = Process::mp_max(faces_sommets_frontiere.dimension(1));
      if (faces_sommets_frontiere.dimension(1) < nb_som_faces)
        {
          IntTrav fsf_old;
          fsf_old = faces_sommets_frontiere;
          faces_sommets_frontiere.resize(fsf_old.dimension_tot(0), nb_som_faces);
          faces_sommets_frontiere = -1;
          for (int i = 0, j; i < fsf_old.dimension_tot(0); i++)
            for (j = 0; j < fsf_old.dimension(1); j++)
              faces_sommets_frontiere(i, j) = fsf_old(i, j);
        }

      vect_renum = -1;
      const int i_premiere_face = front.num_premiere_face();
      const int nb_faces_front = front.nb_faces();
      // Marquage des faces de cette frontiere
      for (int i = i_premiere_face; i < i_premiere_face + nb_faces_front; i++)
        vect_renum[i] = 0;
      vect_renum.echange_espace_virtuel();
      // Construction d'un descripteur contenant le sous-ensemble des faces de cette frontiere
      MD_Vector md_frontiere;
      MD_Vector_tools::creer_md_vect_renum_auto(vect_renum, md_frontiere);

      // Creation de l'espace virtuel des faces de la frontiere
      // (c'est ici qu'on associe le descripteur md_frontiere au tableau des faces)
      const MD_Vector& md_sommets = les_sommets().get_md_vector();
      Scatter::construire_espace_virtuel_traduction(md_frontiere, /* tableau indexe par des numeros de faces de bord */
                                                    md_sommets, /* contenant des indices de sommets du domaine */
                                                    faces_sommets_frontiere, /* tableau a traiter */
                                                    1 /* erreur fatale: si un sommet est manquant, c'est une erreur */);

      // On recupere dans renum l'indice renumerote de chaque face:
      //  on extrait les indices des faces virtuelles de cette frontiere
      ArrOfInt& tab = front.get_faces_virt();
      assert(faces_sommets_frontiere.dimension(0) == nb_faces_front);
      const int nb_faces_tot_frontiere = faces_sommets_frontiere.dimension_tot(0);
      const int nb_faces_virt_frontiere = nb_faces_tot_frontiere - nb_faces_front;
      tab.resize_array(nb_faces_virt_frontiere);
      const int ndebut = nb_faces; // nombre de faces du Domaine !
      const int nfin = nb_faces_tot; // idem !
      for (int i = ndebut; i < nfin; i++)
        {
          const int j = vect_renum[i];
          if (j >= 0)
            {
              assert(j >= nb_faces_front && j < nb_faces_tot_frontiere);
              // La face i est virtuelle et sur cette frontiere
              tab[j - nb_faces_front] = i;
            }
        }
    }
}

namespace
{
/*! @brief Cette methode permet de faire un echange espace virtuel d'un tableau aux aretes sans passer par le descripteur des aretes.
 *
 * On utilise le tableau elem_aretes et l'echange
 *   espace virtuel des elements
 *
 */
void echanger_tableau_aretes(const IntTab& elem_aretes, int nb_aretes_reelles, ArrOfInt& tab_aretes)
{
  const int moi = Process::me();

  const int nb_elem = elem_aretes.dimension(0);
  const int nb_elem_tot = elem_aretes.dimension_tot(0);
  const int nb_aretes_elem = elem_aretes.dimension(1);
  int i;

  // **********************
  // I) Echange pour mettre a jour les items communs
  //  Algo un peu complique pour mettre a jour les items communs: pour chaque arete reele,
  //  la valeur de tab_aretes doit etre egale a la valeur initiale de tab_arete donnee par
  //  le processeur de rang le plus petit parmi ceux qui partagent l'arete (c'est a dire
  //  les processeurs qui ont un element adjacent a cette arete).

  // Tableau permettant de connaitre le processeur proprietaire d'une arete reele
  ArrOfInt pe_arete(nb_aretes_reelles);
  pe_arete = moi;
  // Tableau qui donne, pour chaque element, le processeur proprietaire
  IntVect pe_elem(nb_elem_tot);
  pe_elem = moi; // initialise avec "moi"
  {
    pe_elem.set_md_vector(elem_aretes.get_md_vector());
    pe_elem.echange_espace_virtuel();
    // On range dans pe_arete le numero du plus petit processeur proprietaire des
    // elements adjacents a cette arete
    // Inutile de parcourir les elements reels, on va trouver pe_elem[i]==moi...
    // Si l'arete se trouve sur un processeur de rang inferieur, on lui attribue
    for (i = nb_elem; i < nb_elem_tot; i++)
      for (int pe = pe_elem[i], j = 0, a; j < nb_aretes_elem && (a = elem_aretes(i, j)) >= 0; j++)
        if (a < nb_aretes_reelles && pe_arete[a] > pe)
          pe_arete[a] = pe;
  }
  // On suppose que l'espace virtuel des elements contient au moins une couche d'elements virtuels
  //   (tous les voisins des elements reels par des sommets) alors les aretes reelles sont
  //   echangees (pas encore les aretes virtuelles)
  // Dans ce cas, pe_arete est maintenant correctement rempli pour les aretes reelles.

  IntTab tmp;
  tmp.copy(elem_aretes, Array_base::NOCOPY_NOINIT); // copier uniquement la structure

  // Copier tab_aretes dans la structure tmp (on sait echanger tmp, pas tab_aretes)
  for (i = 0; i < nb_elem; i++)
    for (int j = 0, a; j < nb_aretes_elem && (a = elem_aretes(i, j)) >= 0; j++)
      tmp(i, j) = tab_aretes[a];

  // 2) Echange du tableau
  tmp.echange_espace_virtuel();

  // 3) On reverse dans la partie reelle de tab_aretes les valeurs prises dans tmp:
  //    pour une arete partagee par plusieurs procs, c'est le proc de rang le plus petit
  //    qui donne la valeur
  // Inutile de parcourir les elements reels, la valeur ne changerait pas
  for (i = nb_elem; i < nb_elem_tot; i++)
    for (int pe = pe_elem[i], j = 0, a; j < nb_aretes_elem && (a = elem_aretes(i, j)) >= 0; j++)
      if (a < nb_aretes_reelles && pe_arete[a] == pe)
        tab_aretes[a] = tmp(i, j);

  // tab_aretes contient maintenant des valeurs correctes pour toutes les aretes reeles
  //  (les items communs sont a jour). On fait encore un echange en passant par tmp pour
  //  mettre a jour les items virtuels:

  // ******************
  // II) echange pour mettre a jour l'espace virtuel des aretes

  // Copier encore une fois tab_aretes dans la structure tmp
  for (i = 0; i < nb_elem; i++)
    for (int j = 0, a; j < nb_aretes_elem && (a = elem_aretes(i, j)) >= 0; j++)
      tmp(i, j) = tab_aretes[a];

  // Echange du tableau
  tmp.echange_espace_virtuel();
  // Recopie de tmp dans tab_aretes
  for (i = nb_elem; i < nb_elem_tot; i++)
    for (int j = 0, a; j < nb_aretes_elem && (a = elem_aretes(i, j)) >= 0; j++)
      tab_aretes[a] = tmp(i, j);
}
}

/* version de creer_aretes compatible avec les polyedres */
void Domaine::creer_aretes()
{
  const IntTab& elem_som = les_elems();
  // Nombre d'elements reels:
  const int nbelem = elem_som.dimension(0);
  // Les elements virtuels sont deja construits:
  const int nbelem_tot = elem_som.dimension_tot(0);

  aretes_som_.set_smart_resize(1);
  aretes_som_.resize(0, 2);
  bool is_poly = sub_type(Poly_geom_base, type_elem().valeur());

  std::vector<std::vector<int> > v_e_a(nbelem_tot);  //liste des aretes de chaque element
  int nb_aretes_reelles = 0, i, j;
  {
    // Une liste chainee pour retrouver, pour chaque sommet, la liste des aretes
    // attachees a ce sommet. Le tableau est de meme taille que Aretes_som.dimension(0)
    // chaine_aretes_sommets[i] contient l'indice de la prochaine arete attachee au
    // meme sommet ou -1 si c'est la derniere
    ArrOfInt chaine_aretes_sommets;
    chaine_aretes_sommets.set_smart_resize(1);
    // Indice de la premiere arete attachee a chaque sommet dans chaine_aretes_sommets
    ArrOfInt premiere_arete_som(nb_som_tot());
    premiere_arete_som = -1;

    std::map<std::array<double, 3>, std::array<int, 2> > aretes_loc; //aretes de l'element considere : aretes_loc[{xa, ya, za}] = { s1, s2}
    //l'utilisation d'un map permet de s'assurer que les aretes soient dans le meme ordre sur tous les procs!
    for (int i_elem = 0; i_elem < nbelem_tot; aretes_loc.clear(), i_elem++)
      {
        /* 1. on retrouve les aretes de l'element en iterant sur ses faces */
        const Elem_geom_base& elem_g = ref_cast(Elem_geom_base, type_elem().valeur());
        IntTrav f_e_r;
        if (is_poly)
          {
            const Poly_geom_base& poly_g = ref_cast(Poly_geom_base, type_elem().valeur());
            poly_g.get_tab_faces_sommets_locaux(f_e_r, i_elem);
          }
        else
          elem_g.get_tab_faces_sommets_locaux(f_e_r);

        for (i = 0; i < f_e_r.dimension(0) && f_e_r(i, 0) >= 0; i++)
          for (j = 0; j < f_e_r.dimension(1) && f_e_r(i, j) >= 0; j++)
            {
              int s1 = elem_som(i_elem, f_e_r(i, j)), s2 = elem_som(i_elem, f_e_r(i, j + 1 < f_e_r.dimension(1) && f_e_r(i, j + 1) >= 0 ? j + 1 : 0));
              std::array<double, 3> key;
              for (int l = 0; l < 3; l++)
                key[l] = (sommets_(s1, l) + sommets_(s2, l)) / 2;
              aretes_loc[key] = {{ std::min(s1, s2), std::max(s1, s2) }};
            }

        for (auto &&kv : aretes_loc)
          {
            //a-t-on deja vu cette arete ?
            int k = premiere_arete_som[kv.second[0]];
            while (k >= 0 && (aretes_som_(k, 0) != kv.second[0] || aretes_som_(k, 1) != kv.second[1]))
              k = chaine_aretes_sommets[k];
            if (k < 0) //on n'a pas encore trouve l'arete -> maj de premiere_arete_som et chaine_arete_sommets
              {
                // L'arete n'existe pas encore
                k = chaine_aretes_sommets.size_array();
                assert(k == aretes_som_.dimension(0));
                aretes_som_.append_line(kv.second[0], kv.second[1]);
                // Insertion de l'arete en tete de la liste chainee
                int old_head = premiere_arete_som[kv.second[0]];
                // Indice de la nouvelle arete
                int new_head = chaine_aretes_sommets.size_array();
                chaine_aretes_sommets.append_array(old_head);
                premiere_arete_som[kv.second[0]] = new_head;
              }
            v_e_a[i_elem].push_back(k); //ajout de l'arete a la liste des aretes de l'element
          }
        if (i_elem == nbelem - 1)
          {
            // On vient de finir les aretes reelles
            nb_aretes_reelles = aretes_som_.dimension(0);
          }
      }
  }
  /* remplissage du tableau elem_aretes a l'aide de v_e_a */
  int nb_aretes_elem = 0;
  for (i = 0; i < nbelem_tot; i++)
    nb_aretes_elem = std::max(nb_aretes_elem, (int) v_e_a[i].size());
  nb_aretes_elem = mp_max(nb_aretes_elem);
  elem_aretes_.resize(0, nb_aretes_elem);
  creer_tableau_elements(elem_aretes_, Array_base::NOCOPY_NOINIT);
  for (i = 0, elem_aretes_ = -1; i < nbelem_tot; i++)
    for (j = 0; j < (int) v_e_a[i].size(); j++)
      elem_aretes_(i, j) = v_e_a[i][j];

  // Ajuste la taille du tableau Aretes_som
  const int n_aretes_tot = aretes_som_.dimension(0); // attention, nb_aretes_tot est une methode !
  aretes_som_.append_line(-1, -1); // car le resize suivant ne fait quelque chose que si on change de taille
  aretes_som_.set_smart_resize(0);
  aretes_som_.resize(n_aretes_tot, 2);

  Journal() << "Domaine " << le_nom() << " nb_aretes=" << nb_aretes_reelles << " nb_aretes_tot=" << n_aretes_tot << finl;

  // Construction du descripteur parallele
  {
    // Pour chaque arete, indice du processeur proprietaire de l'arete
    const int moi = Process::me();
    ArrOfInt pe_aretes(n_aretes_tot);
    pe_aretes = moi;
    echanger_tableau_aretes(elem_aretes_, nb_aretes_reelles, pe_aretes);

    // Pour chaque arete, indice de l'arete sur le processeur proprietaire
    ArrOfInt indice_aretes_owner;
    indice_aretes_owner.resize_array(n_aretes_tot, Array_base::NOCOPY_NOINIT);
    for (i = 0; i < nb_aretes_reelles; i++)
      indice_aretes_owner[i] = i;
    echanger_tableau_aretes(elem_aretes_, nb_aretes_reelles, indice_aretes_owner);

    // Construction de pe_voisins
    ArrOfInt pe_voisins;
    pe_voisins.set_smart_resize(1);
    for (i = 0; i < n_aretes_tot; i++)
      if (pe_aretes[i] != moi)
        pe_voisins.append_array(pe_aretes[i]);

    ArrOfInt liste_pe;
    liste_pe.set_smart_resize(1);
    reverse_send_recv_pe_list(pe_voisins, liste_pe);

    // On concatene les deux listes.
    for (i = 0; i < liste_pe.size_array(); i++)
      pe_voisins.append_array(liste_pe[i]);
    array_trier_retirer_doublons(pe_voisins);

    int nb_voisins = pe_voisins.size_array();
    ArrOfInt indices_pe(nproc());
    indices_pe = -1;
    for (i = 0; i < nb_voisins; i++)
      indices_pe[pe_voisins[i]] = i;

    ArrsOfInt aretes_communes_to_recv(nb_voisins);
    ArrsOfInt blocs_aretes_virt(nb_voisins);
    ArrsOfInt aretes_to_send(nb_voisins);
    for (i = 0; i < nb_voisins; i++)
      {
        aretes_communes_to_recv[i].set_smart_resize(1);
        blocs_aretes_virt[i].set_smart_resize(1);
        aretes_to_send[i].set_smart_resize(1);
      }
    // Parcours des aretes: recherche des aretes a recevoir d'un autre processeur.
    // Aretes reeles (items communs)
    for (i = 0; i < nb_aretes_reelles; i++)
      {
        const int pe = pe_aretes[i];
        if (pe != moi)
          {
            const int indice_pe = indices_pe[pe];
            if (indice_pe < 0)
              {
                Cerr << "Error: indice_pe=" << indice_pe << " shouldn't be negative in Domaine::creer_aretes." << finl;
                Cerr << "It is a TRUST bug on this mesh with the Pa discretization, contact support." << finl;
                Cerr << "You could also try another partitioned mesh to get around this issue." << finl;
                exit();
              }
            // Je recois cette arete d'un autre proc
            const int indice_distant = indice_aretes_owner[i];
            aretes_to_send[indice_pe].append_array(indice_distant); // indice sur le pe voisin
            aretes_communes_to_recv[indice_pe].append_array(i); // indice local de l'arete
          }
      }
    // Aretes virtuelles
    for (i = nb_aretes_reelles; i < n_aretes_tot; i++)
      {
        const int pe = pe_aretes[i];
        assert(pe < nproc() && pe != moi);
        const int indice_pe = indices_pe[pe];
        if (indice_pe < 0)
          {
            Cerr << "Error: indice_pe=" << indice_pe << " shouldn't be negative in Domaine::creer_aretes." << finl;
            Cerr << "It is a TRUST bug on this mesh with the Pa discretization, contact support." << finl;
            Cerr << "You could also try another partitioned mesh to get around this issue." << finl;
            exit();
          }
        const int indice_distant = indice_aretes_owner[i];
        aretes_to_send[indice_pe].append_array(indice_distant); // indice sur le pe voisin
        MD_Vector_base2::append_item_to_blocs(blocs_aretes_virt[indice_pe], i);
      }
    {
      Schema_Comm schema;
      schema.set_send_recv_pe_list(pe_voisins, pe_voisins);
      schema.begin_comm();
      // On empile le tableau aretes_to_send et le nombre d'aretes commune avec ce pe:
      for (i = 0; i < nb_voisins; i++)
        schema.send_buffer(pe_voisins[i]) << aretes_to_send[i];
      schema.echange_taille_et_messages();
      // Reception
      for (i = 0; i < nb_voisins; i++)
        schema.recv_buffer(pe_voisins[i]) >> aretes_to_send[i];
      schema.end_comm();
    }

    // Construit l'objet descripteur
    MD_Vector_std md_aretes(n_aretes_tot, nb_aretes_reelles, pe_voisins, aretes_to_send, aretes_communes_to_recv, blocs_aretes_virt);
    Cerr << "Total number of edges = " << md_aretes.nb_items_seq_tot_ << finl;
    // Range l'objet dans un MD_Vector (devient const)
    MD_Vector md;
    md.copy(md_aretes);
    // Attache le descripteur au tableau
    aretes_som_.set_md_vector(md);
  }
}

/////
/////  Methods that were formerly in Domaine class:
/////


// Creation des domaines frontieres (appele lors de la discretisation)
// Actuellement une liste statique de Domaines ou l'on a besoin pour
// chaque domaine de connaitre le premier element
void Domaine::creer_mes_domaines_frontieres(const Domaine_VF& domaine_vf)
{
  const Nom expr_elements("1");
  const Nom expr_faces("1");
  int nb_frontieres = nb_front_Cl();
  domaines_frontieres_.vide();

  for (int i=0; i<nb_frontieres; i++)
    {
      // Nom de la frontiere
      Noms nom_frontiere(1);
      nom_frontiere[0]=frontiere(i).le_nom();
      // Nom du domaine surfacique que l'on va construire
      Nom nom_domaine_surfacique=le_nom();
      nom_domaine_surfacique+="_boundaries_";
      nom_domaine_surfacique+=frontiere(i).le_nom();
      // Creation
      Cerr << "Creating a surface domain named " << nom_domaine_surfacique << " for the boundary " << nom_frontiere[0] << " of the domain " << le_nom() << finl;

      Interprete_bloc& interp = Interprete_bloc::interprete_courant();
      if (interp.objet_global_existant(nom_domaine_surfacique))
        {
          Cerr << "Domain " << nom_domaine_surfacique
               << " already exists, writing to this object." << finl;

          Domaine& dom_new = ref_cast(Domaine, interprete().objet(nom_domaine_surfacique));
          Scatter::uninit_sequential_domain(dom_new);
        }
      else
        {
          DerObjU ob;
          ob.typer("Domaine");
          interp.ajouter(nom_domaine_surfacique, ob);
        }
      Domaine& dom_new = ref_cast(Domaine, interprete().objet(nom_domaine_surfacique));

      Extraire_surface::extraire_surface(dom_new,*this,nom_domaine_surfacique,domaine_vf,expr_elements,expr_faces,0,nom_frontiere);
      REF(Domaine)& ref_dom_new=domaines_frontieres_.add(REF(Domaine)());
      ref_dom_new=dom_new;
    }
}

/*! @brief Association d'un Sous_Domaine au Domaine.
 *
 * L'interface permet de passer n'importe quel
 *     Objet_U mais ne gere (dynamiquement) que
 *     l'association d'un objet derivant Sous_Domaine.
 *
 * @param (Objet_U& ob) l'objet a associer
 * @return (int) 1 si l'association a reussie 0 sinon (l'objet n'etait pas derive Sous_Domaine)
 */
int Domaine::associer_(Objet_U& ob)
{
  if( sub_type(Sous_Domaine, ob))
    {
      add(ref_cast(Sous_Domaine, ob));
      ob.associer_(*this);
      return 1;
    }
  return 0;
}

/*! @brief Ajoute un Sous_Domaine au domaine.
 *
 * @param (Sous_Domaine& ssz) le Sous-domaine a ajouter au domaine
 */
void Domaine::add(const Sous_Domaine& ssz)
{
  les_ss_domaines_.add(ssz);
}

DoubleTab Domaine::getBoundingBox() const
{
  DoubleTab BB(dimension, 2);
  int nbsom=sommets_.dimension(0);
  for (int j=0; j<dimension; j++)
    {
      double min_=0.5*DMAXFLOAT;
      double max_=-0.5*DMAXFLOAT;
      for (int i=0; i<nbsom; i++)
        {
          double c = sommets_(i,j);
          min_ = (c < min_ ? c : min_);
          max_ = (c > max_ ? c : max_);
        }
      BB(j,0) = min_;
      BB(j,1) = max_;
    }
  return BB;
}

/*! @brief Ajoute des noeuds (ou sommets) au domaine (sans verifier les doublons)
 *
 * @param (DoubleTab& soms) le tableau contenant les coordonnees des noeuds a ajouter au domaine
 */
void Domaine::ajouter(const DoubleTab& soms)
{
  int oldsz=sommets_.dimension(0);
  int ajoutsz=soms.dimension(0);
  int dim=soms.dimension(1);
  sommets_.resize(oldsz+ajoutsz,dim);
  for(int i=0; i<ajoutsz; i++)
    for(int k=0; k<dim; k++)
      sommets_(oldsz+i,k)=soms(i,k) ;
}

/*! @brief Ajoute des noeuds au domaine avec elimination des noeuds double au retour nums contient les nouveaux numeros des noeuds de soms
 *
 *     apres elimination des doublons.
 *
 * @param (DoubleTab& soms) le tableau contenant les coordonnees des noeuds a ajouter au domaine
 * @param (IntVect& nums) le tableau des nouveaux numeros apres ajout des nouveaux noeuds et elimination des doublons.
 * @throws des noeuds double ont ete trouve
 */
void Domaine::ajouter(const DoubleTab& soms, IntVect& nums)
{
  int oldsz=sommets_.dimension(0);
  int ajoutsz=soms.dimension(0);
  int dim=soms.dimension(1);
  nums.resize(ajoutsz);
  nums=-1;
  if(oldsz!=0)
    {
      assert(dim==sommets_.dimension(1));
      Octree_Double octree;
      octree.build_nodes(les_sommets(), 0 /* ne pas inclure les sommets virtuels */);

      int i, k;
      int compteur=0;
      ArrOfDouble tab_coord(dim);
      ArrOfInt liste_sommets;
      liste_sommets.set_smart_resize(1);
      for( i=0; i< ajoutsz; i++)
        {
          for (int j = 0; j < dim; j++)
            tab_coord[j] = soms(i,j);
          octree.search_elements_box(tab_coord, epsilon_, liste_sommets);
          octree.search_nodes_close_to(tab_coord, les_sommets(), liste_sommets, epsilon_);
          const int nb_sommets_proches = liste_sommets.size_array();
          if (nb_sommets_proches == 0)
            {
              // Aucun sommet du premier domaine n'est proche du sommet i.
              // Garder i.
            }
          else if (nb_sommets_proches == 1)
            {
              // Un sommet est confondu avec le sommet i a epsilon_ pres.
              // Ne pas garder le sommet
              nums(i) = liste_sommets[0];
              compteur++;
            }
          else
            {
              // Plusieurs sommets du domaine initial sont dans un rayon de epsilon.
              // epsilon est trop grand.
              Cerr << "Error : several nodes of the domain 1 are within radius epsilon="
                   << epsilon_ << " of point " << tab_coord << ". We must reduce epsilon. " << finl;
              Process::exit();
            }
        }
      Cerr << compteur << " double nodes were found \n";
      sommets_.resize(oldsz+ajoutsz-compteur,dim);
      compteur=0;
      for( i=0; i<ajoutsz; i++)
        if(nums(i)==-1)
          {
            nums(i)=oldsz+compteur;
            compteur++;
            for( k=0; k<dim; k++)
              sommets_(nums(i),k)=soms(i,k) ;
          }
    }
  else
    {
      sommets_=soms;
      // if som has a descriptor, delete it:
      sommets_.set_md_vector(MD_Vector());
      for(int i=0; i<ajoutsz; i++)
        nums(i)=i;
    }
}

void Domaine::construire_renum_som_perio(const Conds_lim& les_cl,
                                         const Domaine_dis& domaine_dis)
{
  Noms bords_perio;
  const int nb_bords = les_cl.size();
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      if (sub_type(Periodique, les_cl[n_bord].valeur()))
        bords_perio.add(les_cl[n_bord].frontiere_dis().frontiere().le_nom());
    }

  Reordonner_faces_periodiques::renum_som_perio(*this, bords_perio, renum_som_perio_,
                                                1 /* Calculer les valeurs pour les sommets virtuels */);
}

/*! @brief Cree un tableau ayant une "ligne" par sommet du maillage.
 *
 * Voir MD_Vector_tools::creer_tableau_distribue()
 */
void Domaine::creer_tableau_sommets(Array_base& v, Array_base::Resize_Options opt) const
{
  const MD_Vector& md = md_vector_sommets();
  MD_Vector_tools::creer_tableau_distribue(md, v, opt);
}


/*! @brief only read vertices from the stream s
 */
void Domaine::read_vertices(Entree& s)
{
  // Ajout BM: reset de la structure (a pour effet de debloquer la structure parallele)
  sommets_.reset();
  renum_som_perio_.reset();

  Nom tmp;
  s >> tmp;
  // Si le domaine n'est pas nomme, on prend celui lu
  if (nom_=="??") nom_=tmp;
  Cerr << "Reading vertices for domain " << le_nom() << finl;
  s >> sommets_;
}

void Domaine::imprimer() const
{
  Cerr << "==============================================" << finl;
  Cerr << "The extreme coordinates of the domain " << le_nom() << " are:" << finl;
  // Il n'existe pas de recherche du min et du max dans DoubleTab donc je code:
  DoubleTab BB = getBoundingBox();
  for (int j=0; j<dimension; j++)
    {
      double min_ = mp_min(BB(j,0));
      double max_ = mp_max(BB(j,1));
      if (j==0) Cerr << "x ";
      if (j==1) Cerr << "y ";
      if (j==2) Cerr << "z ";
      Cerr << "is between " << min_ << " and " << max_ << finl;
    }
  Cerr << "==============================================" << finl;
  // We recompute volumes (cause stored in Domaine_VF and so not available from Domaine...):
  DoubleVect volumes;
  DoubleVect inverse_volumes;
  calculer_volumes(volumes,inverse_volumes);
  Cerr << "==============================================" << finl;
  Cerr << "The volume cells of the domain " << le_nom() << " are:" << finl;
  const int i_vmax = imax_array(volumes);
  const int i_vmin = imin_array(volumes);
  const double vmin_local = (i_vmin < 0) ? 1e40 : volumes[i_vmin];
  const double vmax_local = (i_vmax < 0) ? -1e40 : volumes[i_vmax];
  const double volmin = mp_min(vmin_local);
  const double volmax = mp_max(vmax_local);
  double volume_total = mp_somme_vect(volumes);
  const int nbe = nb_elem();
  double volmoy = volume_total / Process::mp_sum(nbe);
  Cerr << "sum(volume cells)= "  << volume_total << finl;
  Cerr << "mean(volume cells)= " << volmoy << finl;
  Cerr << "min(volume cells)= "  << volmin << finl;
  Cerr << "max(volume cells)= "  << volmax << finl;
  if (volmin*1000<volmoy)
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "Warning, a cell volume is more than 1000 times smaller than the average cell volume. Check your mesh." << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
    }
  Cerr << "==============================================" << finl;
}

/*! @brief Build the MEDCoupling mesh corresponding to the TRUST mesh.
 */
void Domaine::build_mc_mesh() const
{
#ifdef MEDCOUPLING_
  Cerr << "   Domaine: Creating a MEDCouplingUMesh object for the domain '" << le_nom() << "'" << finl;

  // Initialize mesh
  Nom type_ele = elem_->que_suis_je();
  int mesh_dim;
  INTERP_KERNEL::NormalizedCellType cell_type = type_geo_trio_to_type_medcoupling(type_ele, mesh_dim);
  mc_mesh_ = MEDCouplingUMesh::New(nom_.getChar(), mesh_dim);

  //
  // Nodes
  //
  int nnodes = sommets_.dimension(0);
  MCAuto<DataArrayDouble> coord(DataArrayDouble::New());
  if (nnodes==0)
    coord->alloc(0, Objet_U::dimension);
  else
    // Avoid deep copy of vertices:
    coord->useArray(sommets_.addr(), false, MEDCoupling::DeallocType::CPP_DEALLOC, nnodes, Objet_U::dimension);
  coord->setInfoOnComponent(0, "x");
  coord->setInfoOnComponent(1, "y");
  if (Objet_U::dimension == 3) coord->setInfoOnComponent(2, "z");
  mc_mesh_->setCoords(coord);

  //
  // Connectivity
  //
  int ncells = mes_elems_.dimension(0);
  int nverts = mes_elems_.dimension(1);

  // Connectivite TRUST -> MED
  IntTab les_elems2(mes_elems_);
  conn_trust_to_med(les_elems2, type_ele, true);

  mc_mesh_->allocateCells(ncells);
  if (cell_type == INTERP_KERNEL::NORM_POLYHED)
    {
      // Polyedron is special, seepage 10:
      // http://trac.lecad.si/vaje/chrome/site/doc8.3.0/extra/Normalisation_pour_le_couplage_de_codes.pdf
      const Polyedre& poly = ref_cast(Polyedre, elem_.valeur());
      ArrOfInt nodes_glob;
      poly.remplir_Nodes_glob(nodes_glob, les_elems2);
      const ArrOfInt& facesIndex = poly.getFacesIndex();
      const ArrOfInt& polyhedronIndex = poly.getPolyhedronIndex();
      assert(ncells == polyhedronIndex.size_array() - 1);
      for (int i = 0; i < ncells; i++)
        {
          int size = 0;
          for (int face = polyhedronIndex[i]; face < polyhedronIndex[i + 1]; face++)
            size += facesIndex[face + 1] - facesIndex[face] + 1;
          size--; // No -1 at the end of the cell
          ArrOfInt cell_def(size);
          size = 0;
          for (int face = polyhedronIndex[i]; face < polyhedronIndex[i + 1]; face++)
            {
              for (int node = facesIndex[face]; node < facesIndex[face + 1]; node++)
                cell_def[size++] = nodes_glob[node];
              if (size < cell_def.size_array())
                // Add -1 to mark the end of a face:
                cell_def[size++] = -1;
            }
          mc_mesh_->insertNextCell(cell_type, cell_def.size_array(), cell_def.addr());
        }
    }
  else
    {
      // Other cells:
      for (int i = 0; i < ncells; i++)
        {
          int nvertices = nverts;
          for (int j = 0; j < nverts; j++)
            if (les_elems2(i, j) < 0)
              nvertices--; // Some cell type has not a constant number of vertices (eg: Polyhedron)
          mc_mesh_->insertNextCell(cell_type, nvertices, les_elems2.addr() + i * nverts);
        }
    }

  mc_mesh_ready_ = true;

#endif
}

/*! @brief Build the MEDCoupling **face** mesh.
 *  It is always made of polygons (in 3D) for simplicity purposes.
 *  Face numbers (and node numbers) are the same as in TRUST.
 *
 *  It unfortunately needs a Domaine_dis_base since this is at this level that the face_sommets relationship is held ...
 */
void Domaine::build_mc_face_mesh(const Domaine_dis_base& domaine_dis_base) const
{
#ifdef MEDCOUPLING_
  using DAId = MCAuto<DataArrayIdType>;

  // Build descending connectivity and convert it to polygons
  assert(mc_mesh_.isNotNull());
  DAId desc(DataArrayIdType::New()), descIndx(DataArrayInt::New()), revDesc(DataArrayInt::New()), revDescIndx(DataArrayInt::New());
  mc_face_mesh_ = mc_mesh_->buildDescendingConnectivity(desc, descIndx, revDesc, revDescIndx);
  if (Objet_U::dimension == 3) mc_face_mesh_->convertAllToPoly();

  // Build second temporary mesh (with shared nodes!) having the TRUST connectivity
  MCAuto<MEDCouplingUMesh> faces_tmp = mc_face_mesh_->deepCopyConnectivityOnly();
  const IntTab& faces_sommets = ref_cast(Domaine_VF, domaine_dis_base).face_sommets();
  int nb_fac = faces_sommets.dimension(0);
  int max_som_fac = faces_sommets.dimension(1);
  assert((int)mc_face_mesh_->getNumberOfCells() == nb_fac);
  DAId c(DataArrayInt::New()), cI(DataArrayInt::New());
  c->alloc(0,1);
  cI->alloc(nb_fac+1, 1);
  mcIdType *cIP = cI->getPointer();
  cIP[0] = 0; // better not forget this ...
  mcIdType typ = Objet_U::dimension == 3 ? INTERP_KERNEL::NormalizedCellType::NORM_POLYGON : INTERP_KERNEL::NormalizedCellType::NORM_SEG2;

  for (int fac=0; fac<nb_fac; fac++)  // Fills the two MC arrays c and cI describing the connectivity of the face mesh
    {
      c->pushBackSilent(typ);
      int i=0, s=-1;
      for (; i <  max_som_fac && (s = faces_sommets(fac, i)) >= 0; i++)  c->pushBackSilent(s);
      cIP[fac+1] = cIP[fac] + i + 1; // +1 because of type
    }
  faces_tmp->setConnectivity(c, cI);

  // Then we can simply identify cells by their nodal connectivity:
  DataArrayIdType * mP;
  mc_face_mesh_->areCellsIncludedIn(faces_tmp,2, mP);
  // DAId renum(mP); //useful to automatically free memory allocated in mP
  DAId renum2(mP->invertArrayN2O2O2N(nb_fac));
#ifndef NDEBUG
  // All cells should be found:
  DAId outliers = renum2->findIdsNotInRange(0, nb_fac);
  if (outliers->getNumberOfTuples() != 0)
    Process::exit("Invalid renumbering arrays! Should not happen. Some faces could not be matched between the TRUST face domain and the buildDescendingConnectivity() version.");
#endif

#ifdef NDEBUG
  bool check = false;
#else
  bool check = true;
#endif
  // Apply the renumbering so that final mc_face_mesh_ has the same face number as in TRUST
  mc_face_mesh_->renumberCells(renum2->begin(), check);
#ifndef NDEBUG
  mc_face_mesh_->checkConsistency();
#endif
  mP->decrRef();
#endif // MEDCOUPLING_
}

/*! @brief Initialize the renumerotation array for periodicity
 */
void Domaine::init_renum_perio()
{
  const int nb_s = sommets_.dimension(0);
  IntTab renum(nb_s);
  for (int i = 0; i < nb_s; i++)
    renum[i] = i;
  set_renum_som_perio(renum);
}

void Domaine::prepare_rmp_with(Domaine& other_domain)
{
#ifdef MEDCOUPLING_
  using namespace MEDCoupling;

  // Retrieve mesh upfront to possibly build them if they were not already:
  get_mc_mesh();
  const MEDCouplingUMesh* oth_msh = other_domain.get_mc_mesh();

  Cerr << "Building remapper between " << le_nom() << " (" << (int)mc_mesh_->getSpaceDimension() << "D) mesh with " << (int)mc_mesh_->getNumberOfCells()
       << " cells and " << other_domain.le_nom() << " (" << (int)other_domain.get_mc_mesh()->getSpaceDimension() << "D) mesh with "
       << (int)other_domain.get_mc_mesh()->getNumberOfCells() << " cells" << finl;
  rmps[&other_domain].prepare(oth_msh, mc_mesh_, "P0P0");
  Cerr << "remapper prepared with " << rmps.at(&other_domain).getNumberOfColsOfMatrix() << " columns in matrix, with max value = " << rmps.at(&other_domain).getMaxValueInCrudeMatrix() << finl;
#else
  Process::exit("Domaine::prepare_rmp_with should not be called since it requires a TRUST version compiled with MEDCoupling !");
#endif
}

#ifdef MEDCOUPLING_
MEDCoupling::MEDCouplingRemapper* Domaine::get_remapper(Domaine& other_domain)
{
  if (!rmps.count(&other_domain)) prepare_rmp_with(other_domain);
  return &rmps.at(&other_domain);
}
#endif
