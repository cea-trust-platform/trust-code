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

#ifndef Zone_included
#define Zone_included

#include <Faces_Internes.h>
#include <Ref_Zone.h>
#include <TRUSTArrays.h>
#include <TRUST_Deriv.h>
#include <TRUSTList.h>
#include <Elem_geom.h>
#include <TRUSTTabs.h>
#include <Raccords.h>
#include <Octree.h>
#include <Joints.h>
#include <Bords.h>
#include <Noms.h>
#include <List_Ref_Zone.h>
#include <List_Ref_Sous_Zone.h>
#include <Ref_Zone.h>

#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingUMesh.hxx>
using MEDCoupling::MEDCouplingUMesh;
using MEDCoupling::MCAuto;
#endif

class Conds_lim;
class Zone_dis;
class Zone_dis_base;
class Domaine_dis;
class Motcle;
class Sous_Zone;
class Probleme_base;

/*! @brief classe Zone Une Zone est un maillage compose d'un ensemble d'elements geometriques
 *
 *     de meme type. Les differents types d'elements sont des objets de
 *     classes derivees de Elem_geom_base.
 *     Une zone a un domaine associe (chaque Zone peut referencer son domaine)
 *     elle a aussi un Nom.
 *     Une zone est constituee d'elements, de bords, de bords periodiques,
 *     de joints, de raccords et de face internes.
 *
 * @sa Zone SousZone Frontiere Elem_geom Elem_geom_base, Bord Bord_perio Joint Raccord Faces_Interne
 */
class Zone : public Objet_U
{
  Declare_instanciable(Zone);

public:

  //// @@@@@@@@@@@@@@
  //// TODO FIXME Adrien - A VIRER
  Zone& add(Zone&);
  /////

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

  ///
  /// Sous_Zone
  ///
  inline int nb_ss_zones() const { return les_ss_zones_.size(); }
  inline const Sous_Zone& ss_zone(int i) const { return les_ss_zones_[i].valeur(); }
  inline Sous_Zone& ss_zone(int i) { return les_ss_zones_[i].valeur(); }
  inline const Sous_Zone& ss_zone(const Nom& nom) const { return les_ss_zones_(nom).valeur(); }
  inline Sous_Zone& ss_zone(const Nom& nom)  { return les_ss_zones_(nom).valeur(); }
  void add(const Sous_Zone&);
  int associer_(Objet_U&) override;  // Associate sous_zone
  inline const LIST(REF(Sous_Zone))& ss_zones() const { return les_ss_zones_; }

  ///
  /// Periodicity
  ///
  inline int get_renum_som_perio(int i) const { return renum_som_perio_[i]; }
  void construire_renum_som_perio(const Conds_lim&, const Zone_dis&) ;
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
  void creer_mes_domaines_frontieres(const Zone_VF& zone_vf);
  inline const LIST(REF(Zone))& domaines_frontieres() const { return domaines_frontieres_; }

  ///
  /// Various
  ///
  inline bool axi1d() const {  return axi1d_;  }
  inline void fixer_epsilon(double eps)  { epsilon_=eps; }
  inline int deformable() const  {   return deformable_;  }
  inline int& deformable()  {   return deformable_;  }
  inline void set_fichier_lu(Nom& nom)  {    fichier_lu_=nom;   }
  inline const Nom& get_fichier_lu() const  {   return fichier_lu_;  }
  void imprimer() const;
  void read_vertices(Entree& s);

  ///
  /// MEDCoupling:
  ///
#ifdef MEDCOUPLING_
  inline MCAuto<MEDCouplingUMesh>& getUMesh() const  {   return mesh_;   };
  inline MCAuto<MEDCouplingUMesh>& getUFacesMesh() const   {   return faces_mesh_;   };
  inline void setUMesh(MCAuto<MEDCouplingUMesh>& m) const  {   mesh_ = m->clone(true);   };
#endif
  void buildUFacesMesh(const Zone_dis_base& zone_dis_base) const;

//// @@@@@@@@@@@@@@

  inline const Elem_geom& type_elem() const;
  inline Elem_geom& type_elem();
  inline void typer(const Nom&);
  inline int nb_elem() const ;
  inline int nb_elem_tot() const ;
  inline int nb_aretes() const;
  inline int nb_aretes_tot() const;
  int nb_som() const ;
  int nb_som_tot() const ;
  inline int nb_som_elem() const;
  inline int nb_faces_elem(int=0) const ;
  inline int sommet_elem(int , int ) const;
  inline const Nom& le_nom() const override;
  inline void nommer(const Nom& ) override;
  const Zone& domaine() const;
  Zone& domaine();
  void fixer_premieres_faces_frontiere();
  //

  ArrOfInt& chercher_elements(const DoubleTab& ,ArrOfInt& ,int reel=0) const;
  ArrOfInt& chercher_elements(const DoubleVect& ,ArrOfInt& ,int reel=0) const;
  ArrOfInt& indice_elements(const IntTab&, ArrOfInt&,int reel=0) const;
  int chercher_elements(double x, double y=0, double z=0,int reel=0) const;
  ArrOfInt& chercher_sommets(const DoubleTab& ,ArrOfInt& ,int reel=0) const;
  int chercher_sommets(double x, double y=0, double z=0,int reel=0) const;
  ArrOfInt& chercher_aretes(const DoubleTab& ,ArrOfInt& ,int reel=0) const;

  virtual void associer_domaine(const Zone&);
  void init_faces_virt_bord(const MD_Vector& md_vect_faces, MD_Vector& md_vect_faces_bord);
  inline void calculer_centres_gravite(DoubleTab& ) const;
  void calculer_centres_gravite_aretes(DoubleTab& ) const;
  void creer_aretes();
  void calculer_volumes(DoubleVect&,DoubleVect& ) const;
  void calculer_mon_centre_de_gravite(ArrOfDouble& );
  //
  inline IntTab& les_elems();
  inline const IntTab& les_elems() const;
  inline int arete_sommets(int i, int j) const;
  inline int elem_aretes(int i, int j) const;
  inline const IntTab& aretes_som() const;
  inline const IntTab& elem_aretes() const;
  inline IntTab& set_aretes_som();
  inline IntTab& set_elem_aretes();
  inline int nb_bords() const ;
  inline int nb_joints() const ;
  inline int nb_raccords() const ;
  inline int nb_frontieres_internes() const ;
  inline int nb_front_Cl() const ;

  int nb_faces_bord() const ;
  int nb_faces_joint() const ;
  int nb_faces_raccord() const ;
  int nb_faces_int() const ;
  int nb_faces_bord(int ) const ;
  int nb_faces_joint(int ) const ;
  int nb_faces_raccord(int ) const ;
  int nb_faces_int(int ) const ;
  inline int nb_faces_frontiere(Type_Face type) const ;
  inline int nb_faces_frontiere() const ;
  inline const Bord& bord(int) const;
  inline Bord& bord(int);
  inline const Joint& joint(int) const;
  inline Joint& joint(int);
  inline Joint& joint_of_pe(int);
  inline const Joint& joint_of_pe(int) const;
  inline const Raccord& raccord(int) const;
  inline Raccord& raccord(int);
  inline const Faces_Interne& faces_interne(int) const;
  inline Faces_Interne& faces_interne(int);
  inline const Bord& bord(const Nom&) const;
  inline Bord& bord(const Nom&);
  inline const Frontiere& frontiere(int i) const;
  inline Frontiere& frontiere(int i);
  int rang_frontiere(const Nom& ) const;
  const Frontiere& frontiere(const Nom&) const;
  Frontiere& frontiere(const Nom&);
  inline const Joint& joint(const Nom&) const;
  inline Joint& joint(const Nom&);
  inline const Raccord& raccord(const Nom&) const;
  inline Raccord& raccord(const Nom&);
  inline const Faces_Interne& faces_interne(const Nom&) const;
  inline Faces_Interne& faces_interne(const Nom&);
  inline const Bords& faces_bord() const;
  inline Bords& faces_bord();
  inline const Joints& faces_joint() const;
  inline Joints& faces_joint();
  inline const Raccords& faces_raccord() const;
  inline Raccords& faces_raccord();
  inline const Faces_Internes& faces_int() const;
  inline Faces_Internes& faces_int();
  void renum(const IntVect&);
  void renum_joint_common_items(const IntVect& Les_Nums, const int elem_offset);
  inline void reordonner();
  int face_interne_conjuguee(int) const;
  int comprimer() ;
  int comprimer_joints();
  void ecrire_noms_bords(Sortie& ) const;
  double epsilon() const;
  inline void associer_bords_a_imprimer(LIST(Nom));
  inline void associer_bords_a_imprimer_sum(LIST(Nom));
  Entree& lire_bords_a_imprimer(Entree& s) ;
  Entree& lire_bords_a_imprimer_sum(Entree& s) ;
  inline const LIST(Nom)& bords_a_imprimer() const;
  inline const LIST(Nom)& bords_a_imprimer_sum() const;
  inline int  moments_a_imprimer() const  {  return moments_a_imprimer_;  }
  inline int& moments_a_imprimer() {  return moments_a_imprimer_;  }
  inline const ArrOfDouble& cg_moments() const  {  return cg_moments_;  }
  inline ArrOfDouble& cg_moments() {  return cg_moments_;  }
  inline void exporter_mon_centre_de_gravite(ArrOfDouble c)  {  cg_moments_ = c; }
  double volume_total() const;

  int nb_faces_bord(Type_Face type) const ;
  int nb_faces_joint(Type_Face type) const ;
  int nb_faces_raccord(Type_Face type) const ;
  int nb_faces_int(Type_Face type) const ;

  int rang_elem_depuis(const DoubleTab&, const ArrOfInt&, ArrOfInt&) const;
  void rang_elems_sommet(ArrOfInt&, double x, double y=0, double z=0) const;
  void invalide_octree();
  const OctreeRoot& construit_octree() const;
  const OctreeRoot& construit_octree(int&) const;
  inline const ArrOfInt& ind_faces_virt_bord() const;
  void construire_elem_virt_pe_num();
  void construire_elem_virt_pe_num(IntTab& elem_virt_pe_num_cpy) const;
  const IntTab& elem_virt_pe_num() const;

  virtual void creer_tableau_elements(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual const MD_Vector& md_vector_elements() const;

  static int identifie_item_unique(IntList& item_possible,
                                   DoubleTab& coord_possible,
                                   const DoubleVect& coord_ref);

  //used for the readOn
  void read_zone(Entree& s);
  void check_zone();

  void correct_type_of_borders_after_merge();

protected:
  Nom nom_;

//////@@@@@@@@@@@@@@@@@@@@@@@@@@@
  DoubleTab sommets_;
  ArrOfInt renum_som_perio_;

  LIST(REF(Zone)) domaines_frontieres_;
  LIST(REF(Sous_Zone)) les_ss_zones_;
  double epsilon_;
  int deformable_;
  Nom fichier_lu_;

#ifdef MEDCOUPLING_
  ///! MEDCoupling version of the domain:
  mutable MCAuto<MEDCouplingUMesh> mesh_;
  ///! MEDCoupling version of the faces domain:
  mutable MCAuto<MEDCouplingUMesh> faces_mesh_;
#endif
  int axi1d_;
//////@@@@@@@@@@@@@@@@@@@@@@@@@@@

  // Type de l'element geometrique de cette Zone
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
  //  de la zone locale a ce processeur qui se raccordent a un processeur voisin)
  Joints mes_faces_joint_;
  // Pour les faces virtuelles de la Zone_VF, indices de la meme face dans le tableau des faces de bord
  // (voir Zone::init_faces_virt_bord())
  ArrOfInt ind_faces_virt_bord_; // contient les indices des faces virtuelles de bord

  // L'octree est mutable: on le construit a la volee lorsqu'il est utilise dans les methodes const
  mutable DERIV(OctreeRoot) deriv_octree_;
  ArrOfDouble cg_moments_;

  // Pour chaque element virtuel i avec nb_elem<=i<nb_elem_tot on a :
  // elem_virt_pe_num_(i-nb_elem,0) = numero du PE qui possede l'element
  // elem_virt_pe_num_(i-nb_elem,1) = numero local de cet element sur le PE qui le possede
  IntTab elem_virt_pe_num_;

  void duplique_faces_internes();
  LIST(Nom) bords_a_imprimer_;
  LIST(Nom) bords_a_imprimer_sum_;
  int moments_a_imprimer_;

private:
  // Volume total de la zone (somme sur tous les processeurs)
  double volume_total_;
  // Cached infos to accelerate Zone::chercher_elements():
  mutable DoubleTabs cached_positions_;
  mutable ArrsOfInt cached_elements_;
};

inline const LIST(Nom)& Zone::bords_a_imprimer() const { return bords_a_imprimer_; }
inline const LIST(Nom)& Zone::bords_a_imprimer_sum() const { return bords_a_imprimer_sum_; }
inline void Zone::associer_bords_a_imprimer(LIST(Nom) liste) { bords_a_imprimer_=liste; }
inline void Zone::associer_bords_a_imprimer_sum(LIST(Nom) liste) { bords_a_imprimer_sum_=liste; }


/*! @brief Renvoie le nombre d'elements de la zone.
 *
 * @return (int) le nombre d'elements de la zone
 */
inline int Zone::nb_elem() const  { return mes_elems_.dimension(0); }

/*! @brief Renvoie le nombre total d'elements de la zone.
 *
 * @return (int) le nombre total d'elements de la zone
 */
inline int Zone::nb_elem_tot() const { return mes_elems_.dimension_tot(0); }

/*! @brief Renvoie le numero (global) j-ieme sommet du i-ieme element
 *
 * @param (int i) le numero de l'element
 * @param (int j) le numero local (sur l'element) du sommet dont on veut connaitre le numero global
 * @return (int) le numero (global) j-ieme sommet du i-ieme element
 */
inline int Zone::sommet_elem(int i, int j) const {  return mes_elems_(i,j); }

/*! @brief Renvoie le nom de la zone.
 *
 * @return (Nom&) le nom de la zone
 */
inline const Nom& Zone::le_nom() const {   return nom_; }

/*! @brief Donne un nom a la zone.
 *
 * @param (Nom& nom) le nom a donner a la zone
 */
inline void Zone::nommer(const Nom& nom) {  nom_ = nom; }


/*! @brief Renvoie le tableau des sommets des elements
 *
 * @return (IntTab&) le tableau des sommets des elements
 */
inline IntTab& Zone::les_elems() {  return mes_elems_; }

/*! @brief Renvoie le tableau des sommets des elements (version const)
 *
 * @return (IntTab&) le tableau des sommets des elements
 */
inline const IntTab& Zone::les_elems() const {  return mes_elems_; }

/*! @brief Renvoie le nombre de sommets des elements geometriques constituants la zone.
 *
 * Tous les elements de la zone etant
 *     du meme type ils ont tous le meme nombre de sommets
 *     qui est le nombre de sommet du type des elements geometriques
 *     de la zone.
 *
 * @return (int) le nombre de sommets par element
 */
inline int Zone::nb_som_elem() const {   return elem_.nb_som(); }

/*! @brief Renvoie le nombre de face de type i des elements geometriques constituants la zone.
 *
 *     Ex: les objets de la classe Prisme ont 2 types
 *         de faces: triangle ou quadrangle.
 *
 * @param (int i) le type de face
 * @return (int) le nombre de face de type i des elements geometriques constituants la zone
 */
inline int Zone::nb_faces_elem(int i) const {  return elem_.nb_faces(i); }

/*! @brief Renvoie un element geometrique du type de ceux qui constituent la zone.
 *
 *     (version const)
 *
 * @return (Elem_geom&) un element geometrique du type de ceux qui constituent la zone
 */
inline const Elem_geom& Zone::type_elem() const { return elem_; }

/*! @brief Renvoie un element geometrique du type de ceux qui constituent la zone.
 *
 * @return (Elem_geom&) un element geometrique du type de ceux qui constituent la zone
 */
inline Elem_geom& Zone::type_elem() {  return elem_; }

/*! @brief Renvoie le nombre de bords de la zone.
 *
 * @return (int) le nombre de bords de la zone
 */
inline int Zone::nb_bords() const {  return mes_faces_bord_.nb_bords(); }

/*! @brief Renvoie le nombre de joints de la zone.
 *
 * @return (int) le nombre de joints de la zone
 */
inline int Zone::nb_joints() const { return mes_faces_joint_.nb_joints(); }

/*! @brief Renvoie le nombre de raccords de la zone.
 *
 * @return (int) le nombre de raccords de la zone
 */
inline int Zone::nb_raccords() const {  return mes_faces_raccord_.nb_raccords(); }

/*! @brief Renvoie le nombre de frontieres internes de la zone.
 *
 * @return (int) le nombre de frontieres internes de la zone
 */
inline int Zone::nb_frontieres_internes() const {  return mes_faces_int_.nb_faces_internes(); }

/*! @brief Renvoie le nombre de faces frontiere de la zone.
 *
 * C'est la somme des nombres de  bords, de raccords et de faces internes
 *
 */
inline int Zone::nb_faces_frontiere() const { return nb_faces_bord() + nb_faces_raccord() + nb_faces_int(); }

/*! @brief Renvoie le nombre de bords + le nombre de raccords
 *
 *           + le nombre de faces frontieres internes
 *
 * @return (int) le nombre de frontieres ayant des conditions aux limites.
 */
inline int Zone::nb_front_Cl() const  {   return nb_bords() +nb_raccords() + nb_frontieres_internes(); }
/*! @brief Type les elements de la zone avec le nom passe en parametre.
 *
 * Et associe le type d'element a la zone.
 * @param (Nom& typ) le nom du type des elements geometriques de la zone.
 */
inline void Zone::typer(const Nom& typ)
{
  elem_.typer(typ);
  elem_.associer_zone(*this);
}

/*! @brief Calcule les centres de gravites des elements de la zone.
 *
 * @param (DoubleTab& xp) le tableau contenant les centres de gravites des elements de la zone
 */
inline void Zone::calculer_centres_gravite(DoubleTab& xp) const {  elem_.calculer_centres_gravite(xp); }

/*! @brief Renvoie le bord dont le nom est specifie.
 *
 * (version const)
 *
 * @param (Nom& nom) le nom du bord a renvoyer
 * @return (Bord&) le bord dont le nom est specifie
 */
inline const Bord& Zone::bord(const Nom& nom) const {   return mes_faces_bord_(nom); }

/*! @brief Renvoie le bord dont le nom est specifie.
 *
 * @param (Nom& nom) le nom du bord a renvoyer
 * @return (Bord&) le bord dont le nom est specifie
 */
inline Bord& Zone::bord(const Nom& nom) {  return mes_faces_bord_(nom); }

/*! @brief Renvoie le joint dont le nom est specifie (version const)
 *
 * @param (Nom& nom) le nom du joint a renvoyer
 * @return (Joint&) le joint dont le nom est specifie
 */
inline const Joint& Zone::joint(const Nom& nom) const {   return mes_faces_joint_(nom); }

/*! @brief Renvoie le joint dont le nom est specifie
 *
 * @param (Nom& nom) le nom du joint a renvoyer
 * @return (Joint&) le joint dont le nom est specifie
 */
inline Joint& Zone::joint(const Nom& nom) {  return mes_faces_joint_(nom); }

/*! @brief Renvoie le raccord dont le nom est specifie (version const)
 *
 * @param (Nom& nom) le nom du raccord a renvoyer
 * @return (Raccord&) le raccord dont le nom est specifie
 */
inline const Raccord& Zone::raccord(const Nom& nom) const {   return mes_faces_raccord_(nom); }

/*! @brief Renvoie le raccord dont le nom est specifie
 *
 * @param (Nom& nom) le nom du raccord a renvoyer
 * @return (Raccord&) le raccord dont le nom est specifie
 */
inline Raccord& Zone::raccord(const Nom& nom) {  return mes_faces_raccord_(nom); }

/*! @brief Renvoie les faces_internes dont le nom est specifie (version const)
 *
 * @param (Nom& nom) le nom des faces internes a renvoyer
 * @return (Faces_Interne&) les faces_internes dont le nom est specifie
 */
inline const Faces_Interne& Zone::faces_interne(const Nom& nom) const {   return mes_faces_int_(nom); }

/*! @brief Renvoie les faces_internes dont le nom est specifie
 *
 * @param (Nom& nom) le nom des faces internes a renvoyer
 * @return (Faces_Interne&) les faces_internes dont le nom est specifie
 */
inline Faces_Interne& Zone::faces_interne(const Nom& nom) {   return mes_faces_int_(nom); }

/*! @brief Renvoie le i-ieme bord de la zone (version const)
 *
 * @param (int i) l'indice du bord renvoyer
 * @return (Bord&) le i-ieme bord de la zone
 */
inline const Bord& Zone::bord(int i) const {   return mes_faces_bord_(i); }

inline const Frontiere& Zone::frontiere(int i) const
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

inline Frontiere& Zone::frontiere(int i)
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

/*! @brief Renvoie le i-ieme bord de la zone
 *
 * @param (int i) l'indice du bord a renvoyer
 * @return (Bord&) le i-ieme bord de la zone
 */
inline Bord& Zone::bord(int i) {  return mes_faces_bord_(i); }

/*! @brief Renvoie le i-ieme joint de la zone (version const)
 *
 * @param (int i) l'indice du joint renvoyer
 * @return (Joint&) le i-ieme joint de la zone
 */
inline const Joint& Zone::joint(int i) const { return mes_faces_joint_(i); }

/*! @brief Renvoie le i-ieme joint de la zone
 *
 * @param (int i) l'indice du joint a renvoyer
 * @return (Joint&) le i-ieme joint de la zone
 */
inline Joint& Zone::joint(int i) {   return mes_faces_joint_(i); }

/*! @brief Renvoie le joint correspondant au PE specifie.
 *
 * (PE = Processeur Elementaire, ou numero de processus)
 *     Un joint est une frontiere entre 2 PE.
 *
 * @param (int pe) un numero de PE
 * @return (Joint&) le joint correspondant au PE specifie
 */
inline const Joint& Zone::joint_of_pe(int pe) const
{
  int i;
  for(i=0; i<nb_joints(); i++)
    if(mes_faces_joint_(i).PEvoisin()==pe)
      break;
  return mes_faces_joint_(i);
}

inline Joint& Zone::joint_of_pe(int pe)
{
  int i;
  for(i=0; i<nb_joints(); i++)
    if(mes_faces_joint_(i).PEvoisin()==pe)
      break;
  return mes_faces_joint_(i);
}

/*! @brief Renvoie le i-ieme raccord de la zone (version const)
 *
 * @param (int i) l'indice du raccord renvoyer
 * @return (Raccord&) le i-ieme raccord de la zone
 */
inline const Raccord& Zone::raccord(int i) const { return mes_faces_raccord_(i); }

/*! @brief Renvoie le i-ieme raccord de la zone
 *
 * @param (int i) l'indice du raccord a renvoyer
 * @return (Raccord&) le i-ieme raccord de la zone
 */
inline Raccord& Zone::raccord(int i) {  return mes_faces_raccord_(i); }

/*! @brief Renvoie les i-ieme faces internes de la zone (version const)
 *
 * @param (int i) l'indice des faces internes renvoyer
 * @return (Faces_Internes&) les i-ieme faces internes de la zone
 */
inline const Faces_Interne& Zone::faces_interne(int i) const {  return mes_faces_int_(i); }

/*! @brief Renvoie les i-ieme faces internes de la zone
 *
 * @param (int i) l'indice des faces internes a renvoyer
 * @return (Facesr_Internes&) les i-ieme faces internes de la zone
 */
inline Faces_Interne& Zone::faces_interne(int i) {   return mes_faces_int_(i); }

/*! @brief Renvoie la liste des bords de la zone.
 * (version const)
 *
 * @return (Bords&) la liste des bords de la zone
 */
inline const Bords& Zone::faces_bord() const { return mes_faces_bord_; }

/*! @brief Renvoie la liste des bords de la zone.
 *
 * @return (Bords&) la liste des bords de la zone
 */
inline Bords& Zone::faces_bord() {  return mes_faces_bord_; }

/*! @brief Renvoie la liste des joints de la zone.
 * (version const)
 *
 * @return (Joints&) la liste des joints de la zone
 */
inline const Joints& Zone::faces_joint() const {  return mes_faces_joint_; }

/*! @brief Renvoie la liste des joints de la zone.
 *
 * @return (Joints&) la liste des joints de la zone
 */
inline Joints& Zone::faces_joint() { return mes_faces_joint_; }

/*! @brief Renvoie la liste des racoords de la zone.
 * (version const)
 *
 * @return (Raccords&) la liste des raccords de la zone
 */
inline const Raccords& Zone::faces_raccord() const
{
  return mes_faces_raccord_;
}

/*! @brief Renvoie la liste des racoords de la zone.
 *
 * @return (Raccords&) la liste des raccords de la zone
 */
inline Raccords& Zone::faces_raccord()
{
  return mes_faces_raccord_;
}

/*! @brief Renvoie la liste des faces internes de la zone.
 * (version const)
 *
 * @return (Faces_Internes&) la liste des faces internes de la zone
 */
inline const Faces_Internes& Zone::faces_int() const
{
  return mes_faces_int_;
}

/*! @brief Renvoie la liste des faces internes de la zone.
 *
 * @return (Faces_Internes&) la liste des faces internes de la zone
 */
inline Faces_Internes& Zone::faces_int()
{
  return mes_faces_int_;
}

/*! @brief Reordonne les elements suivant la convention employe par Trio-U.
 */
inline void Zone::reordonner()
{
  elem_.reordonner();
}


/*! @brief Renvoie le nombre de faces frontiere de la zone du type specifie.
 *
 *     C'est la somme des nombres de  bords, de raccords
 *     et de faces internes du type specifie.
 *
 * @param (Type_Face type) un type de face (certains elements geometriques ont plusieurs types de faces)
 * @return (int) le nombre de faces frontiere de la zone du type specifie
 */
inline int Zone::nb_faces_frontiere(Type_Face type) const
{
  return
    nb_faces_bord(type) +
    nb_faces_int(type) +
    nb_faces_raccord(type);
}

// Decription:
// Renvoie le tableau des indices des faces distantes de bord
inline const ArrOfInt& Zone::ind_faces_virt_bord() const { return ind_faces_virt_bord_; }

/*! @brief renvoie le nombre d'aretes reelles.
 */
inline int Zone::nb_aretes() const { return aretes_som_.dimension(0); }

/*! @brief renvoie le nombre d'aretes total (reelles+virtuelles).
 */
inline int Zone::nb_aretes_tot() const { return aretes_som_.dimension_tot(0); }

/*! @brief renvoie le numero du jeme sommet de la ieme arete.
 */
inline int Zone::arete_sommets(int i, int j) const { return aretes_som_(i, j); }

/*! @brief renvoie le numero de la jeme arete du ieme element.
 */
inline int Zone::elem_aretes(int i, int j) const {  return elem_aretes_(i, j); }

/*! @brief renvoie le tableau de connectivite aretes/sommets.
 */
inline const IntTab& Zone::aretes_som() const {  return aretes_som_; }

inline IntTab& Zone::set_aretes_som() {  return aretes_som_; }

/*! @brief renvoie le tableau de connectivite elements/aretes.
 */
inline const IntTab& Zone::elem_aretes() const {   return elem_aretes_; }
inline IntTab& Zone::set_elem_aretes() {   return elem_aretes_; }

#endif
