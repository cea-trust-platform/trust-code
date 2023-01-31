/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Domaine_included
#define Domaine_included

#include <Interprete_geometrique_base.h>
#include <Faces_Internes.h>
#include <TRUSTArrays.h>
#include <TRUST_Deriv.h>
#include <TRUST_List.h>
#include <TRUSTList.h>
#include <Elem_geom.h>
#include <TRUSTTabs.h>
#include <TRUST_Ref.h>
#include <Raccords.h>
#include <Octree.h>
#include <Joints.h>
#include <Bords.h>
#include <Noms.h>

#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingUMesh.hxx>
using MEDCoupling::MEDCouplingUMesh;
using MEDCoupling::MCAuto;
#endif

#include <list>

class Conds_lim;
class Domaine_dis;
class Domaine_dis_base;
class Domaine_dis;
class Sous_Domaine;
class Probleme_base;

/*! @brief classe Domaine Une Domaine est un maillage compose d'un ensemble d'elements geometriques
 *
 *     de meme type. Les differents types d'elements sont des objets de
 *     classes derivees de Elem_geom_base.
 *     Une domaine est constituee  de noeuds, d'elements, de bords, de bords periodiques,
 *     de joints, de raccords et de face internes.
 *
 * @sa Domaine SousDomaine Frontiere Elem_geom Elem_geom_base, Bord Bord_perio Joint Raccord Faces_Interne
 */
class Domaine : public Objet_U
{
  Declare_instanciable(Domaine);

public:
  ///
  /// General
  ///
  inline const Nom& le_nom() const override;
  inline void nommer(const Nom& ) override;
  inline void typer(const Nom&);
  double epsilon() const;

  ///
  /// Nodes
  ///
  inline double coord(int i, int j) const { return sommets_(i,j); }
  inline double& coord(int i, int j) { return sommets_(i,j); }
  inline const DoubleTab& coord_sommets() const { return sommets_; }
  inline DoubleTab& les_sommets()  { return sommets_; }
  inline const DoubleTab& les_sommets() const  { return sommets_; }
  DoubleTab getBoundingBox() const;
  void ajouter(const DoubleTab&);
  void ajouter(const DoubleTab&, IntVect&);
  virtual void creer_tableau_sommets(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual const MD_Vector& md_vector_sommets() const { return sommets_.get_md_vector(); }
  int nb_som() const ;
  int nb_som_tot() const ;
  void read_vertices(Entree& s);

  ///
  /// Elements
  ///
  inline const Elem_geom& type_elem() const;
  inline Elem_geom& type_elem();
  inline IntTab& les_elems();
  inline const IntTab& les_elems() const;
  inline int nb_elem() const ;
  inline int nb_elem_tot() const ;
  inline int nb_som_elem() const;
  inline int nb_faces_elem(int=0) const ;
  inline int sommet_elem(int , int ) const;

  ///
  /// Aretes
  ///
  inline int nb_aretes() const;
  inline int nb_aretes_tot() const;
  void creer_aretes();

  ///
  /// Faces
  ///
  int nb_faces_bord() const ;
  int nb_faces_bord(int ) const ;
  int nb_faces_bord(Type_Face type) const ;
  int nb_faces_joint() const ;
  int nb_faces_joint(int ) const ;
  int nb_faces_joint(Type_Face type) const ;
  int nb_faces_raccord() const ;
  int nb_faces_raccord(int ) const ;
  int nb_faces_raccord(Type_Face type) const ;
  int nb_faces_int() const ;
  int nb_faces_int(int ) const ;
  int nb_faces_int(Type_Face type) const ;
  inline int nb_faces_frontiere() const ;
  inline int nb_faces_frontiere(Type_Face type) const ;

  inline const Faces_Interne& faces_interne(int) const;
  inline Faces_Interne& faces_interne(int);
  inline const Faces_Internes& faces_int() const;
  inline Faces_Internes& faces_int();
  inline const Faces_Interne& faces_interne(const Nom&) const;
  inline Faces_Interne& faces_interne(const Nom&);
  int face_interne_conjuguee(int) const;

  inline const Bords& faces_bord() const;
  inline Bords& faces_bord();

  inline const Joints& faces_joint() const;
  inline Joints& faces_joint();

  inline const Raccords& faces_raccord() const;
  inline Raccords& faces_raccord();

  ///
  /// Bords
  ///
  inline int nb_bords() const ;
  inline const Bord& bord(int) const;
  inline Bord& bord(int);
  inline const Bord& bord(const Nom&) const;
  inline Bord& bord(const Nom&);

  ///
  /// Frontieres
  ///
  inline const Frontiere& frontiere(int i) const;
  inline Frontiere& frontiere(int i);
  int rang_frontiere(const Nom& ) const;
  const Frontiere& frontiere(const Nom&) const;
  Frontiere& frontiere(const Nom&);
  inline int nb_frontieres_internes() const ;
  inline int nb_front_Cl() const ;

  ///
  /// Joints
  ///
  inline int nb_joints() const ;
  inline const Joint& joint(int) const;
  inline Joint& joint(int);
  inline Joint& joint_of_pe(int);
  inline const Joint& joint_of_pe(int) const;
  inline const Joint& joint(const Nom&) const;
  inline Joint& joint(const Nom&);
  void renum_joint_common_items(const IntVect& Les_Nums, const int elem_offset);
  int comprimer_joints();

  ///
  /// Raccords
  ///
  inline int nb_raccords() const ;
  inline const Raccord& raccord(int) const;
  inline Raccord& raccord(int);
  inline const Raccord& raccord(const Nom&) const;
  inline Raccord& raccord(const Nom&);

  ///
  /// Sous_Domaine
  ///
  inline int nb_ss_domaines() const { return les_ss_domaines_.size(); }
  inline const Sous_Domaine& ss_domaine(int i) const { return les_ss_domaines_[i].valeur(); }
  inline Sous_Domaine& ss_domaine(int i) { return les_ss_domaines_[i].valeur(); }
  inline const Sous_Domaine& ss_domaine(const Nom& nom) const { return les_ss_domaines_(nom).valeur(); }
  inline Sous_Domaine& ss_domaine(const Nom& nom)  { return les_ss_domaines_(nom).valeur(); }
  void add(const Sous_Domaine&);
  int associer_(Objet_U&) override;  // Associate sous_domaine
  inline const LIST(REF(Sous_Domaine))& ss_domaines() const { return les_ss_domaines_; }

  ///
  /// Periodicity
  ///
  void init_renum_perio();
  inline int get_renum_som_perio(int i) const { return renum_som_perio_[i]; }
  void construire_renum_som_perio(const Conds_lim&, const Domaine_dis&) ;
  inline void set_renum_som_perio(IntTab& renum)  {    renum_som_perio_=renum;   };

  ///
  /// Time-dependency
  ///
  virtual void initialiser (double temps, Domaine_dis&, Probleme_base&) {}
  virtual void set_dt(double& dt_) {}
  virtual void mettre_a_jour(double temps, Domaine_dis&, Probleme_base&) {}

  ///
  /// Domaines frontieres
  ///
  void creer_mes_domaines_frontieres(const Domaine_VF& domaine_vf);
  inline const LIST(REF(Domaine))& domaines_frontieres() const { return domaines_frontieres_; }

  ///
  /// Geometric computations
  ///
  inline void calculer_centres_gravite(DoubleTab& ) const;
  void calculer_centres_gravite_aretes(DoubleTab& ) const;
  void calculer_volumes(DoubleVect&,DoubleVect& ) const;
  void calculer_mon_centre_de_gravite(ArrOfDouble& );
  double volume_total() const;
  inline const ArrOfDouble& cg_moments() const  {  return cg_moments_;  }
  inline ArrOfDouble& cg_moments() {  return cg_moments_;  }

  ///
  /// Lookup methods and mapping arrays
  ///
  ArrOfInt& chercher_elements(const DoubleTab& ,ArrOfInt& ,int reel=0) const;
  ArrOfInt& chercher_elements(const DoubleVect& ,ArrOfInt& ,int reel=0) const;
  ArrOfInt& indice_elements(const IntTab&, ArrOfInt&,int reel=0) const;
  int chercher_elements(double x, double y=0, double z=0,int reel=0) const;
  ArrOfInt& chercher_sommets(const DoubleTab& ,ArrOfInt& ,int reel=0) const;
  int chercher_sommets(double x, double y=0, double z=0,int reel=0) const;
  ArrOfInt& chercher_aretes(const DoubleTab& ,ArrOfInt& ,int reel=0) const;
  inline int arete_sommets(int i, int j) const;
  inline int elem_aretes(int i, int j) const;
  inline const IntTab& aretes_som() const;
  inline const IntTab& elem_aretes() const;
  inline IntTab& set_aretes_som();
  inline IntTab& set_elem_aretes();
  void rang_elems_sommet(ArrOfInt&, double x, double y=0, double z=0) const;
  void invalide_octree();
  const OctreeRoot& construit_octree() const;
  const OctreeRoot& construit_octree(int&) const;

  ///
  /// Printing/export stuff
  ///
  void ecrire_noms_bords(Sortie& ) const;
  inline void associer_bords_a_imprimer(LIST(Nom) liste)     { bords_a_imprimer_=liste; }
  inline void associer_bords_a_imprimer_sum(LIST(Nom) liste) { bords_a_imprimer_sum_=liste; }
  Entree& lire_bords_a_imprimer(Entree& s) ;
  Entree& lire_bords_a_imprimer_sum(Entree& s) ;
  inline const LIST(Nom)& bords_a_imprimer() const { return bords_a_imprimer_; }
  inline const LIST(Nom)& bords_a_imprimer_sum() const { return bords_a_imprimer_sum_; }
  inline int  moments_a_imprimer() const  {  return moments_a_imprimer_;  }
  inline int& moments_a_imprimer() {  return moments_a_imprimer_;  }
  inline void exporter_mon_centre_de_gravite(ArrOfDouble c)  {  cg_moments_ = c; }

  ///
  /// Various
  ///
  void clear();
  void fill_from_list(std::list<Domaine*>& lst);
  void merge_wo_vertices_with(Domaine& z);
  inline bool axi1d() const {  return axi1d_;  }
  inline void fixer_epsilon(double eps)  { epsilon_=eps; }
  inline int deformable() const  {   return deformable_;  }
  inline int& deformable() {   return deformable_;  }
  inline void set_fichier_lu(Nom& nom)  {    fichier_lu_=nom;   }
  inline const Nom& get_fichier_lu() const  {   return fichier_lu_;  }
  void imprimer() const;
  void fixer_premieres_faces_frontiere();
  void init_faces_virt_bord(const MD_Vector& md_vect_faces, MD_Vector& md_vect_faces_bord);
  void read_former_domaine(Entree& s);
  void check_domaine();
  void correct_type_of_borders_after_merge();
  int comprimer() ;
  void renum(const IntVect&);
  inline void reordonner();

  ///
  /// MEDCoupling:
  ///
#ifdef MEDCOUPLING_
  inline MCAuto<MEDCouplingUMesh>& getUMesh() const  {   return mesh_;   };
  inline MCAuto<MEDCouplingUMesh>& getUFacesMesh() const   {   return faces_mesh_;   };
  inline void setUMesh(MCAuto<MEDCouplingUMesh>& m) const  {   mesh_ = m->clone(true);   };
#endif
  void buildUFacesMesh(const Domaine_dis_base& domaine_dis_base) const;

  ///
  /// Parallelism and virtual items management
  ///
  inline const ArrOfInt& ind_faces_virt_bord() const;
  void construire_elem_virt_pe_num();
  void construire_elem_virt_pe_num(IntTab& elem_virt_pe_num_cpy) const;
  const IntTab& elem_virt_pe_num() const;
  virtual void creer_tableau_elements(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual const MD_Vector& md_vector_elements() const;
  static int identifie_item_unique(IntList& item_possible, DoubleTab& coord_possible, const DoubleVect& coord_ref);

protected:
  // Domaine name
  Nom nom_;
  // Array of vertices
  DoubleTab sommets_;
  // Renumbering array for periodicity
  ArrOfInt renum_som_perio_;
  // Type de l'element geometrique de cette Domaine
  Elem_geom elem_;
  // Description des elements (pour le multi-element, le tableau peut contenir des -1 !!!)
  IntTab mes_elems_;
  // Definition des aretes des elements (pour chaque arete, indices des deux sommets)
  //  (ce tableau n'est pas toujours rempli, selon la discretisation)
  IntTab aretes_som_;
  // Pour chaque element, indices de ses aretes dans Aretes_som (voir Elem_geom_base::get_tab_aretes_sommets_locaux())
  IntTab elem_aretes_;
  // Bords, raccords et faces_internes forment les "faces_frontiere" sur lesquelles
  //  sont definies les conditions aux limites.
  Bords mes_faces_bord_;
  Raccords mes_faces_raccord_;
  Faces_Internes mes_faces_int_;
  // Les faces de joint sont les faces communes avec les autres processeurs (bords
  //  du domaine locale a ce processeur qui se raccordent a un processeur voisin)
  Joints mes_faces_joint_;
  // Pour les faces virtuelles de la Domaine_VF, indices de la meme face dans le tableau des faces de bord
  // (voir Domaine::init_faces_virt_bord())
  ArrOfInt ind_faces_virt_bord_; // contient les indices des faces virtuelles de bord

  // L'octree est mutable: on le construit a la volee lorsqu'il est utilise dans les methodes const
  mutable DERIV(OctreeRoot) deriv_octree_;
  ArrOfDouble cg_moments_;

  // Pour chaque element virtuel i avec nb_elem<=i<nb_elem_tot on a :
  // elem_virt_pe_num_(i-nb_elem,0) = numero du PE qui possede l'element
  // elem_virt_pe_num_(i-nb_elem,1) = numero local de cet element sur le PE qui le possede
  IntTab elem_virt_pe_num_;

  LIST(REF(Domaine)) domaines_frontieres_;

  LIST(REF(Sous_Domaine)) les_ss_domaines_;

  int moments_a_imprimer_;
  LIST(Nom) bords_a_imprimer_;
  LIST(Nom) bords_a_imprimer_sum_;

  int axi1d_;
  double epsilon_;
  int deformable_;
  Nom fichier_lu_;

#ifdef MEDCOUPLING_
  ///! MEDCoupling version of the domain:
  mutable MCAuto<MEDCouplingUMesh> mesh_;
  ///! MEDCoupling version of the faces domain:
  mutable MCAuto<MEDCouplingUMesh> faces_mesh_;
#endif

  void duplique_faces_internes();

private:
  // Volume total du domaine (somme sur tous les processeurs)
  double volume_total_;
  // Cached infos to accelerate Domaine::chercher_elements():
  mutable DoubleTabs cached_positions_;
  mutable ArrsOfInt cached_elements_;
};

/*! @brief Renvoie le nombre d'elements du domaine.
 *
 * @return (int) le nombre d'elements du domaine
 */
inline int Domaine::nb_elem() const  { return mes_elems_.dimension(0); }

/*! @brief Renvoie le nombre total d'elements du domaine.
 *
 * @return (int) le nombre total d'elements du domaine
 */
inline int Domaine::nb_elem_tot() const { return mes_elems_.dimension_tot(0); }

/*! @brief Renvoie le numero (global) j-ieme sommet du i-ieme element
 *
 * @param (int i) le numero de l'element
 * @param (int j) le numero local (sur l'element) du sommet dont on veut connaitre le numero global
 * @return (int) le numero (global) j-ieme sommet du i-ieme element
 */
inline int Domaine::sommet_elem(int i, int j) const {  return mes_elems_(i,j); }

/*! @brief Renvoie le nom du domaine.
 *
 * @return (Nom&) le nom du domaine
 */
inline const Nom& Domaine::le_nom() const {   return nom_; }

/*! @brief Donne un nom au domaine.
 *
 * @param (Nom& nom) le nom a donner au domaine
 */
inline void Domaine::nommer(const Nom& nom) {  nom_ = nom; }

/*! @brief Renvoie le tableau des sommets des elements
 *
 * @return (IntTab&) le tableau des sommets des elements
 */
inline IntTab& Domaine::les_elems() {  return mes_elems_; }

/*! @brief Renvoie le tableau des sommets des elements (version const)
 *
 * @return (IntTab&) le tableau des sommets des elements
 */
inline const IntTab& Domaine::les_elems() const {  return mes_elems_; }

/*! @brief Renvoie le nombre de sommets des elements geometriques constituants le domaine.
 *
 * Tous les elements du domaine etant
 *     du meme type ils ont tous le meme nombre de sommets
 *     qui est le nombre de sommet du type des elements geometriques
 *     du domaine.
 *
 * @return (int) le nombre de sommets par element
 */
inline int Domaine::nb_som_elem() const {   return elem_.nb_som(); }

/*! @brief Renvoie le nombre de face de type i des elements geometriques constituants le domaine.
 *
 *     Ex: les objets de la classe Prisme ont 2 types
 *         de faces: triangle ou quadrangle.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de face de type i des elements geometriques constituants le domaine
 */
inline int Domaine::nb_faces_elem(int i) const {  return elem_.nb_faces(i); }

/*! @brief Renvoie un element geometrique du type de ceux qui constituent le domaine.
 *
 *     (version const)
 *
 * @return (Elem_geom&) un element geometrique du type de ceux qui constituent le domaine
 */
inline const Elem_geom& Domaine::type_elem() const { return elem_; }

/*! @brief Renvoie un element geometrique du type de ceux qui constituent le domaine.
 *
 * @return (Elem_geom&) un element geometrique du type de ceux qui constituent le domaine
 */
inline Elem_geom& Domaine::type_elem() {  return elem_; }

/*! @brief Renvoie le nombre de bords du domaine.
 *
 * @return (int) le nombre de bords du domaine
 */
inline int Domaine::nb_bords() const {  return mes_faces_bord_.nb_bords(); }

/*! @brief Renvoie le nombre de joints du domaine.
 *
 * @return (int) le nombre de joints du domaine
 */
inline int Domaine::nb_joints() const { return mes_faces_joint_.nb_joints(); }

/*! @brief Renvoie le nombre de raccords du domaine.
 *
 * @return (int) le nombre de raccords du domaine
 */
inline int Domaine::nb_raccords() const {  return mes_faces_raccord_.nb_raccords(); }

/*! @brief Renvoie le nombre de frontieres internes du domaine.
 *
 * @return (int) le nombre de frontieres internes du domaine
 */
inline int Domaine::nb_frontieres_internes() const {  return mes_faces_int_.nb_faces_internes(); }

/*! @brief Renvoie le nombre de faces frontiere du domaine.
 *
 * C'est la somme des nombres de  bords, de raccords et de faces internes
 *
 */
inline int Domaine::nb_faces_frontiere() const { return nb_faces_bord() + nb_faces_raccord() + nb_faces_int(); }

/*! @brief Renvoie le nombre de bords + le nombre de raccords
 *
 *           + le nombre de faces frontieres internes
 *
 * @return (int) le nombre de frontieres ayant des conditions aux limites.
 */
inline int Domaine::nb_front_Cl() const  {   return nb_bords() +nb_raccords() + nb_frontieres_internes(); }
/*! @brief Type les elements du domaine avec le nom passe en parametre.
 *
 * Et associe le type d'element au domaine.
 * @param (Nom& typ) le nom du type des elements geometriques du domaine.
 */
inline void Domaine::typer(const Nom& typ)
{
  elem_.typer(typ);
  elem_.associer_domaine(*this);
}

/*! @brief Calcule les centres de gravites des elements du domaine.
 *
 * @param (DoubleTab& xp) le tableau contenant les centres de gravites des elements du domaine
 */
inline void Domaine::calculer_centres_gravite(DoubleTab& xp) const {  elem_.calculer_centres_gravite(xp); }

/*! @brief Renvoie le bord dont le nom est specifie.
 *
 * (version const)
 *
 * @param (Nom& nom) le nom du bord a renvoyer
 * @return (Bord&) le bord dont le nom est specifie
 */
inline const Bord& Domaine::bord(const Nom& nom) const {   return mes_faces_bord_(nom); }

/*! @brief Renvoie le bord dont le nom est specifie.
 *
 * @param (Nom& nom) le nom du bord a renvoyer
 * @return (Bord&) le bord dont le nom est specifie
 */
inline Bord& Domaine::bord(const Nom& nom) {  return mes_faces_bord_(nom); }

/*! @brief Renvoie le joint dont le nom est specifie (version const)
 *
 * @param (Nom& nom) le nom du joint a renvoyer
 * @return (Joint&) le joint dont le nom est specifie
 */
inline const Joint& Domaine::joint(const Nom& nom) const {   return mes_faces_joint_(nom); }

/*! @brief Renvoie le joint dont le nom est specifie
 *
 * @param (Nom& nom) le nom du joint a renvoyer
 * @return (Joint&) le joint dont le nom est specifie
 */
inline Joint& Domaine::joint(const Nom& nom) {  return mes_faces_joint_(nom); }

/*! @brief Renvoie le raccord dont le nom est specifie (version const)
 *
 * @param (Nom& nom) le nom du raccord a renvoyer
 * @return (Raccord&) le raccord dont le nom est specifie
 */
inline const Raccord& Domaine::raccord(const Nom& nom) const {   return mes_faces_raccord_(nom); }

/*! @brief Renvoie le raccord dont le nom est specifie
 *
 * @param (Nom& nom) le nom du raccord a renvoyer
 * @return (Raccord&) le raccord dont le nom est specifie
 */
inline Raccord& Domaine::raccord(const Nom& nom) {  return mes_faces_raccord_(nom); }

/*! @brief Renvoie les faces_internes dont le nom est specifie (version const)
 *
 * @param (Nom& nom) le nom des faces internes a renvoyer
 * @return (Faces_Interne&) les faces_internes dont le nom est specifie
 */
inline const Faces_Interne& Domaine::faces_interne(const Nom& nom) const {   return mes_faces_int_(nom); }

/*! @brief Renvoie les faces_internes dont le nom est specifie
 *
 * @param (Nom& nom) le nom des faces internes a renvoyer
 * @return (Faces_Interne&) les faces_internes dont le nom est specifie
 */
inline Faces_Interne& Domaine::faces_interne(const Nom& nom) {   return mes_faces_int_(nom); }

/*! @brief Renvoie le i-ieme bord du domaine (version const)
 *
 * @param (int i) l'indice du bord renvoyer
 * @return (Bord&) le i-ieme bord du domaine
 */
inline const Bord& Domaine::bord(int i) const {   return mes_faces_bord_(i); }

inline const Frontiere& Domaine::frontiere(int i) const
{
  int fin=nb_bords();
  if(i<fin)
    return mes_faces_bord_(i);
  i-=fin;
  fin=nb_raccords();
  if(i<fin)
    return mes_faces_raccord_(i).valeur();
  i-=fin;
  fin=nb_frontieres_internes();
  if(i<fin)
    return mes_faces_int_(i);
  assert(0);
  exit();
  return frontiere(i);
}

inline Frontiere& Domaine::frontiere(int i)
{
  int fin=nb_bords();
  if(i<fin)
    return mes_faces_bord_(i);
  i-=fin;
  fin=nb_raccords();
  if(i<fin)
    return mes_faces_raccord_(i).valeur();
  i-=fin;
  fin=nb_frontieres_internes();
  if(i<fin)
    return mes_faces_int_(i);
  assert(0);
  exit();
  return frontiere(i);
}

/*! @brief Renvoie le i-ieme bord du domaine
 *
 * @param (int i) l'indice du bord a renvoyer
 * @return (Bord&) le i-ieme bord du domaine
 */
inline Bord& Domaine::bord(int i) {  return mes_faces_bord_(i); }

/*! @brief Renvoie le i-ieme joint du domaine (version const)
 *
 * @param (int i) l'indice du joint renvoyer
 * @return (Joint&) le i-ieme joint du domaine
 */
inline const Joint& Domaine::joint(int i) const { return mes_faces_joint_(i); }

/*! @brief Renvoie le i-ieme joint du domaine
 *
 * @param (int i) l'indice du joint a renvoyer
 * @return (Joint&) le i-ieme joint du domaine
 */
inline Joint& Domaine::joint(int i) {   return mes_faces_joint_(i); }

/*! @brief Renvoie le joint correspondant au PE specifie.
 *
 * (PE = Processeur Elementaire, ou numero de processus)
 *     Un joint est une frontiere entre 2 PE.
 *
 * @param (int pe) un numero de PE
 * @return (Joint&) le joint correspondant au PE specifie
 */
inline const Joint& Domaine::joint_of_pe(int pe) const
{
  int i;
  for(i=0; i<nb_joints(); i++)
    if(mes_faces_joint_(i).PEvoisin()==pe)
      break;
  return mes_faces_joint_(i);
}

inline Joint& Domaine::joint_of_pe(int pe)
{
  int i;
  for(i=0; i<nb_joints(); i++)
    if(mes_faces_joint_(i).PEvoisin()==pe)
      break;
  return mes_faces_joint_(i);
}

/*! @brief Renvoie le i-ieme raccord du domaine (version const)
 *
 * @param (int i) l'indice du raccord renvoyer
 * @return (Raccord&) le i-ieme raccord du domaine
 */
inline const Raccord& Domaine::raccord(int i) const { return mes_faces_raccord_(i); }

/*! @brief Renvoie le i-ieme raccord du domaine
 *
 * @param (int i) l'indice du raccord a renvoyer
 * @return (Raccord&) le i-ieme raccord du domaine
 */
inline Raccord& Domaine::raccord(int i) {  return mes_faces_raccord_(i); }

/*! @brief Renvoie les i-ieme faces internes du domaine (version const)
 *
 * @param (int i) l'indice des faces internes renvoyer
 * @return (Faces_Internes&) les i-ieme faces internes du domaine
 */
inline const Faces_Interne& Domaine::faces_interne(int i) const {  return mes_faces_int_(i); }

/*! @brief Renvoie les i-ieme faces internes du domaine
 *
 * @param (int i) l'indice des faces internes a renvoyer
 * @return (Facesr_Internes&) les i-ieme faces internes du domaine
 */
inline Faces_Interne& Domaine::faces_interne(int i) {   return mes_faces_int_(i); }

/*! @brief Renvoie la liste des bords du domaine.
 * (version const)
 *
 * @return (Bords&) la liste des bords du domaine
 */
inline const Bords& Domaine::faces_bord() const { return mes_faces_bord_; }

/*! @brief Renvoie la liste des bords du domaine.
 *
 * @return (Bords&) la liste des bords du domaine
 */
inline Bords& Domaine::faces_bord() {  return mes_faces_bord_; }

/*! @brief Renvoie la liste des joints du domaine.
 * (version const)
 *
 * @return (Joints&) la liste des joints du domaine
 */
inline const Joints& Domaine::faces_joint() const {  return mes_faces_joint_; }

/*! @brief Renvoie la liste des joints du domaine.
 *
 * @return (Joints&) la liste des joints du domaine
 */
inline Joints& Domaine::faces_joint() { return mes_faces_joint_; }

/*! @brief Renvoie la liste des racoords du domaine.
 * (version const)
 *
 * @return (Raccords&) la liste des raccords du domaine
 */
inline const Raccords& Domaine::faces_raccord() const { return mes_faces_raccord_; }

/*! @brief Renvoie la liste des racoords du domaine.
 *
 * @return (Raccords&) la liste des raccords du domaine
 */
inline Raccords& Domaine::faces_raccord() { return mes_faces_raccord_; }

/*! @brief Renvoie la liste des faces internes du domaine.
 * (version const)
 *
 * @return (Faces_Internes&) la liste des faces internes du domaine
 */
inline const Faces_Internes& Domaine::faces_int() const { return mes_faces_int_; }

/*! @brief Renvoie la liste des faces internes du domaine.
 *
 * @return (Faces_Internes&) la liste des faces internes du domaine
 */
inline Faces_Internes& Domaine::faces_int() { return mes_faces_int_; }

/*! @brief Reordonne les elements suivant la convention employe par Trio-U.
 */
inline void Domaine::reordonner() { elem_.reordonner(); }

/*! @brief Renvoie le nombre de faces frontiere du domaine du type specifie.
 *
 *     C'est la somme des nombres de  bords, de raccords
 *     et de faces internes du type specifie.
 *
 * @param (Type_Face type) un type de face (certains elements geometriques ont plusieurs types de faces)
 * @return (int) le nombre de faces frontiere du domaine du type specifie
 */
inline int Domaine::nb_faces_frontiere(Type_Face type) const
{
  return
    nb_faces_bord(type) +
    nb_faces_int(type) +
    nb_faces_raccord(type);
}

// Decription:
// Renvoie le tableau des indices des faces distantes de bord
inline const ArrOfInt& Domaine::ind_faces_virt_bord() const { return ind_faces_virt_bord_; }

/*! @brief renvoie le nombre d'aretes reelles.
 */
inline int Domaine::nb_aretes() const { return aretes_som_.dimension(0); }

/*! @brief renvoie le nombre d'aretes total (reelles+virtuelles).
 */
inline int Domaine::nb_aretes_tot() const { return aretes_som_.dimension_tot(0); }

/*! @brief renvoie le numero du jeme sommet de la ieme arete.
 */
inline int Domaine::arete_sommets(int i, int j) const { return aretes_som_(i, j); }

/*! @brief renvoie le numero de la jeme arete du ieme element.
 */
inline int Domaine::elem_aretes(int i, int j) const {  return elem_aretes_(i, j); }

/*! @brief renvoie le tableau de connectivite aretes/sommets.
 */
inline const IntTab& Domaine::aretes_som() const {  return aretes_som_; }

inline IntTab& Domaine::set_aretes_som() {  return aretes_som_; }

/*! @brief renvoie le tableau de connectivite elements/aretes.
 */
inline const IntTab& Domaine::elem_aretes() const {   return elem_aretes_; }
inline IntTab& Domaine::set_elem_aretes() {   return elem_aretes_; }

#endif
