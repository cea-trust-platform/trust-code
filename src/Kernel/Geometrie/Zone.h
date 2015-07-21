/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Zone.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_included
#define Zone_included

#include <Ref_Domaine.h>
#include <Bords.h>
#include <Joints.h>
#include <Raccords.h>
#include <Faces_Internes.h>
#include <Elem_geom.h>
#include <Noms.h>
#include <Deriv_OctreeRoot.h>
#include <List_Nom.h>
#include <DoubleTab.h>

class Motcle;
class IntList;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Zone
//    Une Zone est un maillage compose d'un ensemble d'elements geometriques
//    de meme type. Les differents types d'elements sont des objets de
//    classes derivees de Elem_geom_base.
//    Une zone a un domaine associe (chaque Zone peut referencer son domaine)
//    elle a aussi un Nom.
//    Une zone est constituee d'elements, de bords, de bords periodiques,
//    de joints, de raccords et de face internes.
// .SECTION voir aussi
//    Domaine SousZone Frontiere Elem_geom Elem_geom_base
//    Bord Bord_perio Joint Raccord Faces_Interne
//////////////////////////////////////////////////////////////////////////////
class Zone : public Objet_U
{
  Declare_instanciable(Zone);

public:

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
  inline const Nom& le_nom() const;
  inline void nommer(const Nom& );
  const Domaine& domaine() const;
  Domaine& domaine();
  void fixer_premieres_faces_frontiere();
  //

  ArrOfInt& chercher_elements(const DoubleTab& ,ArrOfInt& ,int reel=0) const;
  ArrOfInt& chercher_elements(const DoubleVect& ,ArrOfInt& ,int reel=0) const;
  ArrOfInt& indice_elements(const IntTab&, ArrOfInt&,int reel=0) const;
  int chercher_elements(double x, double y=0, double z=0,int reel=0) const;
  ArrOfInt& chercher_sommets(const DoubleTab& ,ArrOfInt& ,int reel=0) const;
  int chercher_sommets(double x, double y=0, double z=0,int reel=0) const;
  ArrOfInt& chercher_aretes(const DoubleTab& ,ArrOfInt& ,int reel=0) const;

  virtual void associer_domaine(const Domaine&);
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
  inline void reordonner();
  int face_interne_conjuguee(int) const;
  int comprimer() ;
  void ecrire_noms_bords(Sortie& ) const;
  double epsilon() const;
  inline void associer_Bords_a_imprimer(LIST(Nom));
  inline void associer_Bords_a_imprimer_sum(LIST(Nom));
  Entree& Lire_Bords_a_imprimer(Entree& s) ;
  Entree& Lire_Bords_a_imprimer_sum(Entree& s) ;
  inline const LIST(Nom)& Bords_a_imprimer() const;
  inline const LIST(Nom)& Bords_a_imprimer_sum() const;
  inline int  Moments_a_imprimer() const
  {
    return Moments_a_imprimer_;
  };
  inline int& Moments_a_imprimer()
  {
    return Moments_a_imprimer_;
  };
  inline const ArrOfDouble& cg_moments() const
  {
    return cg_moments_;
  };
  inline ArrOfDouble& cg_moments()
  {
    return cg_moments_;
  };
  inline void exporter_mon_centre_de_gravite(ArrOfDouble c)
  {
    cg_moments_=c;
  };
  double volume_total() const;

  int nb_faces_bord(Type_Face type) const ;
  int nb_faces_joint(Type_Face type) const ;
  int nb_faces_raccord(Type_Face type) const ;
  int nb_faces_int(Type_Face type) const ;

  //void numeroter_elems_virtuels(const DoubleTab&, IntVect&);

  int rang_elem_depuis(const DoubleTab&, const ArrOfInt&, ArrOfInt&) const;
  void rang_elems_sommet(ArrOfInt&, double x, double y=0, double z=0) const;
  void invalide_octree();
  const OctreeRoot& construit_octree() const;
  const OctreeRoot& construit_octree(int&) const;
  //int postraiter_ijk(Sortie&) const;
  inline const ArrOfInt& ind_faces_virt_bord() const;

  void construire_elem_virt_pe_num();
  const IntTab& elem_virt_pe_num() const;

  virtual void creer_tableau_elements(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual const MD_Vector& md_vector_elements() const;

  static int identifie_item_unique(IntList& item_possible,
                                   DoubleTab& coord_possible,
                                   const DoubleVect& coord_ref);
protected:
  Nom nom;
  REF(Domaine) le_domaine;
  // Type de l'element geometrique de cette Zone
  Elem_geom elem;
  // Description des elements (pour le multi-element, le tableau peut contenir des -1 !!!)
  IntTab mes_elems;
  // Definition des aretes des elements (pour chaque arete, indices des deux sommets)
  //  (ce tableau n'est pas toujours rempli, selon la discretisation)
  IntTab Aretes_som;
  // Pour chaque element, indices de ses aretes dans Aretes_som (voir Elem_geom_base::get_tab_aretes_sommets_locaux())
  IntTab Elem_Aretes;
  // Bords, raccords et faces_internes forment les "faces_frontiere" sur lesquelles
  //  sont definies les conditions aux limites.
  Bords mes_faces_bord;
  Raccords mes_faces_raccord;
  Faces_Internes mes_faces_int;
  // Les faces de joint sont les faces communes avec les autres processeurs (bords
  //  de la zone locale a ce processeur qui se raccordent a un processeur voisin)
  Joints mes_faces_joint;
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
  LIST(Nom) Bords_a_imprimer_;
  LIST(Nom) Bords_a_imprimer_sum_;
  int Moments_a_imprimer_;

private:
  // Volume total de la zone (somme sur tous les processeurs)
  double volume_total_;
};

inline const LIST(Nom)& Zone::Bords_a_imprimer() const
{
  return Bords_a_imprimer_;
}

inline const LIST(Nom)& Zone::Bords_a_imprimer_sum() const
{
  return Bords_a_imprimer_sum_;
}

inline void Zone::associer_Bords_a_imprimer(LIST(Nom) liste)
{
  Bords_a_imprimer_=liste;
}

inline void Zone::associer_Bords_a_imprimer_sum(LIST(Nom) liste)
{
  Bords_a_imprimer_sum_=liste;
}


// Description:
//    Renvoie le nombre d'elements de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre d'elements de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::nb_elem() const
{
  return mes_elems.dimension(0);
}

// Description:
//    Renvoie le nombre total d'elements de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre total d'elements de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::nb_elem_tot() const
{
  return mes_elems.dimension_tot(0);
}

// Description:
//    Renvoie le numero (global) j-ieme sommet du i-ieme element
// Precondition:
// Parametre: int i
//    Signification: le numero de l'element
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification: le numero local (sur l'element) du sommet dont
//                   on veut connaitre le numero global
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le numero (global) j-ieme sommet du i-ieme element
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::sommet_elem(int i, int j) const
{
  return mes_elems(i,j);
}

// Description:
//    Renvoie le nom de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom de la zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Nom& Zone::le_nom() const
{
  return nom;
}

// Description:
//    Donne un nom a la zone.
// Precondition:
// Parametre: Nom& nom_
//    Signification: le nom a donner a la zone
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Zone::nommer(const Nom& nom_)
{
  nom = nom_;
}


// Description:
//    Renvoie le tableau des sommets des elements
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: IntTab&
//    Signification: le tableau des sommets des elements
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline IntTab& Zone::les_elems()
{
  return mes_elems;
}

// Description:
//    Renvoie le tableau des sommets des elements
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: IntTab&
//    Signification: le tableau des sommets des elements
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const IntTab& Zone::les_elems() const
{
  return mes_elems;
}

// Description:
//    Renvoie le nombre de sommets des elements geometriques
//    constituants la zone. Tous les elements de la zone etant
//    du meme type ils ont tous le meme nombre de sommets
//    qui est le nombre de sommet du type des elements geometriques
//    de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de sommets par element
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::nb_som_elem() const
{
  return elem.nb_som();
}

// Description:
//    Renvoie le nombre de face de type i
//    des elements geometriques constituants la zone.
//    Ex: les objets de la classe Prisme ont 2 types
//        de faces: triangle ou quadrangle.
// Precondition:
// Parametre: int i
//    Signification: le type de face
//    Valeurs par defaut:
//    Contraintes: i < nombre de type face des elements
//                 geometriques constituants la zone
//    Acces:
// Retour: int
//    Signification: le nombre de face de type i des
//                   elements geometriques constituants la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::nb_faces_elem(int i) const
{
  return elem.nb_faces(i);
}

// Description:
//    Renvoie un element geometrique du type de ceux
//    qui constituent la zone.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Elem_geom&
//    Signification: un element geometrique du type de ceux
//                   qui constituent la zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Elem_geom& Zone::type_elem() const
{
  return elem;
}

// Description:
//    Renvoie un element geometrique du type de ceux
//    qui constituent la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Elem_geom&
//    Signification: un element geometrique du type de ceux
//                   qui constituent la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Elem_geom& Zone::type_elem()
{
  return elem;
}

// Description:
//    Renvoie le nombre de bords de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de bords de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::nb_bords() const
{
  return mes_faces_bord.nb_bords();
}

// Description:
//    Renvoie le nombre de joints de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de joints de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::nb_joints() const
{
  return mes_faces_joint.nb_joints();
}

// Description:
//    Renvoie le nombre de raccords de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de raccords de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::nb_raccords() const
{
  return mes_faces_raccord.nb_raccords();
}

// Description:
//    Renvoie le nombre de frontieres internes de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de frontieres internes de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::nb_frontieres_internes() const
{
  return mes_faces_int.nb_faces_internes();
}

// Description:
//  Renvoie le nombre de faces frontiere de la zone.
//  C'est la somme des nombres de  bords, de raccords et de faces internes
inline int Zone::nb_faces_frontiere() const
{
  return nb_faces_bord() + nb_faces_raccord() + nb_faces_int();
}

// Description:
//    Renvoie le nombre de bords
//          + le nombre de raccords
//          + le nombre de faces frontieres internes
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de frontieres ayant des
//                   conditions aux limites.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::nb_front_Cl() const
{
  return nb_bords() +nb_raccords() + nb_frontieres_internes();
}
// Description:
//    Type les elements de la zone avec le
//    nom passe en parametre. Et associe le type
//    d'element a la zone.
// Precondition:
// Parametre: Nom& typ
//    Signification: le nom du type des elements geometriques
//                   de la zone.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Zone::typer(const Nom& typ)
{
  elem.typer(typ);
  elem.associer_zone(*this);
}

// Description:
//    Calcule les centres de gravites des elements de la zone.
// Precondition:
// Parametre: DoubleTab& xp
//    Signification: le tableau contenant les centres de
//                   gravites des elements de la zone
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline void Zone::calculer_centres_gravite(DoubleTab& xp) const
{
  elem.calculer_centres_gravite(xp);
}

// Description:
//    Renvoie le bord dont le nom est specifie.
//    (version const)
// Precondition:
// Parametre: Nom& nom_
//    Signification: le nom du bord a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Bord&
//    Signification: le bord dont le nom est specifie
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Bord& Zone::bord(const Nom& nom_) const
{
  return mes_faces_bord(nom_);
}

// Description:
//    Renvoie le bord dont le nom est specifie.
// Precondition:
// Parametre: Nom& nom_
//    Signification: le nom du bord a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Bord&
//    Signification: le bord dont le nom est specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Bord& Zone::bord(const Nom& nom_)
{
  return mes_faces_bord(nom_);
}

// Description:
//    Renvoie le joint dont le nom est specifie
//    (version const)
// Precondition:
// Parametre: Nom& nom_
//    Signification: le nom du joint a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Joint&
//    Signification: le joint dont le nom est specifie
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Joint& Zone::joint(const Nom& nom_) const
{
  return mes_faces_joint(nom_);
}

// Description:
//    Renvoie le joint dont le nom est specifie
// Precondition:
// Parametre: Nom& nom_
//    Signification: le nom du joint a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Joint&
//    Signification: le joint dont le nom est specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Joint& Zone::joint(const Nom& nom_)
{
  return mes_faces_joint(nom_);
}

// Description:
//    Renvoie le raccord dont le nom est specifie
//    (version const)
// Precondition:
// Parametre: Nom& nom_
//    Signification: le nom du raccord a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Raccord&
//    Signification: le raccord dont le nom est specifie
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Raccord& Zone::raccord(const Nom& nom_) const
{
  return mes_faces_raccord(nom_);
}

// Description:
//    Renvoie le raccord dont le nom est specifie
// Precondition:
// Parametre: Nom& nom_
//    Signification: le nom du raccord a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Raccord&
//    Signification: le raccord dont le nom est specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Raccord& Zone::raccord(const Nom& nom_)
{
  return mes_faces_raccord(nom_);
}

// Description:
//    Renvoie les faces_internes dont le nom est specifie
//    (version const)
// Precondition:
// Parametre: Nom& nom_
//    Signification: le nom des faces internes a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Faces_Interne&
//    Signification: les faces_internes dont le nom est specifie
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Faces_Interne& Zone::faces_interne(const Nom& nom_) const
{
  return mes_faces_int(nom_);
}

// Description:
//    Renvoie les faces_internes dont le nom est specifie
// Precondition:
// Parametre: Nom& nom_
//    Signification: le nom des faces internes a renvoyer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Faces_Interne&
//    Signification: les faces_internes dont le nom est specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Faces_Interne& Zone::faces_interne(const Nom& nom_)
{
  return mes_faces_int(nom_);
}

// Description:
//    Renvoie le i-ieme bord de la zone
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'indice du bord renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Bord&
//    Signification: le i-ieme bord de la zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Bord& Zone::bord(int i) const
{
  return mes_faces_bord(i);
}
inline const Frontiere& Zone::frontiere(int i) const
{
  int fin=nb_bords();
  if(i<fin)
    return mes_faces_bord(i);
  i-=fin;
  fin=nb_raccords();
  if(i<fin)
    return mes_faces_raccord(i).valeur();
  i-=fin;
  fin=nb_frontieres_internes();
  if(i<fin)
    return mes_faces_int(i);
  assert(0);
  exit();
  return frontiere(i);
}
inline Frontiere& Zone::frontiere(int i)
{
  int fin=nb_bords();
  if(i<fin)
    return mes_faces_bord(i);
  i-=fin;
  fin=nb_raccords();
  if(i<fin)
    return mes_faces_raccord(i).valeur();
  i-=fin;
  fin=nb_frontieres_internes();
  if(i<fin)
    return mes_faces_int(i);
  assert(0);
  exit();
  return frontiere(i);
}

// Description:
//    Renvoie le i-ieme bord de la zone
// Precondition:
// Parametre: int i
//    Signification: l'indice du bord a renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Bord&
//    Signification: le i-ieme bord de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Bord& Zone::bord(int i)
{
  return mes_faces_bord(i);
}

// Description:
//    Renvoie le i-ieme joint de la zone
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'indice du joint renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Joint&
//    Signification: le i-ieme joint de la zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Joint& Zone::joint(int i) const
{
  return mes_faces_joint(i);
}

// Description:
//    Renvoie le i-ieme joint de la zone
// Precondition:
// Parametre: int i
//    Signification: l'indice du joint a renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Joint&
//    Signification: le i-ieme joint de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Joint& Zone::joint(int i)
{
  return mes_faces_joint(i);
}

// Description:
//    Renvoie le joint correspondant au PE specifie.
//    (PE = Processeur Elementaire, ou numero de processus)
//    Un joint est une frontiere entre 2 PE.
// Precondition:
// Parametre: int pe
//    Signification: un numero de PE
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Joint&
//    Signification: le joint correspondant au PE specifie
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Joint& Zone::joint_of_pe(int pe) const
{
  int i;
  for(i=0; i<nb_joints(); i++)
    if(mes_faces_joint(i).PEvoisin()==pe)
      break;
  return mes_faces_joint(i);
}
inline Joint& Zone::joint_of_pe(int pe)
{
  int i;
  for(i=0; i<nb_joints(); i++)
    if(mes_faces_joint(i).PEvoisin()==pe)
      break;
  return mes_faces_joint(i);
}

// Description:
//    Renvoie le i-ieme raccord de la zone
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'indice du raccord renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Raccord&
//    Signification: le i-ieme raccord de la zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Raccord& Zone::raccord(int i) const
{
  return mes_faces_raccord(i);
}

// Description:
//    Renvoie le i-ieme raccord de la zone
// Precondition:
// Parametre: int i
//    Signification: l'indice du raccord a renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Raccord&
//    Signification: le i-ieme raccord de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Raccord& Zone::raccord(int i)
{
  return mes_faces_raccord(i);
}

// Description:
//    Renvoie les i-ieme faces internes de la zone
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: l'indice des faces internes renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Faces_Internes&
//    Signification: les i-ieme faces internes de la zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Faces_Interne& Zone::faces_interne(int i) const
{
  return mes_faces_int(i);
}

// Description:
//    Renvoie les i-ieme faces internes de la zone
// Precondition:
// Parametre: int i
//    Signification: l'indice des faces internes a renvoyer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Facesr_Internes&
//    Signification: les i-ieme faces internes de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Faces_Interne& Zone::faces_interne(int i)
{
  return mes_faces_int(i);
}

// Description:
//    Renvoie la liste des bords de la zone.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Bords&
//    Signification: la liste des bords de la zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Bords& Zone::faces_bord() const
{
  return mes_faces_bord;
}

// Description:
//    Renvoie la liste des bords de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Bords&
//    Signification: la liste des bords de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Bords& Zone::faces_bord()
{
  return mes_faces_bord;
}

// Description:
//    Renvoie la liste des joints de la zone.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Joints&
//    Signification: la liste des joints de la zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Joints& Zone::faces_joint() const
{
  return mes_faces_joint;
}

// Description:
//    Renvoie la liste des joints de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Joints&
//    Signification: la liste des joints de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Joints& Zone::faces_joint()
{
  return mes_faces_joint;
}

// Description:
//    Renvoie la liste des racoords de la zone.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Raccords&
//    Signification: la liste des raccords de la zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Raccords& Zone::faces_raccord() const
{
  return mes_faces_raccord;
}

// Description:
//    Renvoie la liste des racoords de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Raccords&
//    Signification: la liste des raccords de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Raccords& Zone::faces_raccord()
{
  return mes_faces_raccord;
}

// Description:
//    Renvoie la liste des faces internes de la zone.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Faces_Internes&
//    Signification: la liste des faces internes de la zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Faces_Internes& Zone::faces_int() const
{
  return mes_faces_int;
}

// Description:
//    Renvoie la liste des faces internes de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Faces_Internes&
//    Signification: la liste des faces internes de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Faces_Internes& Zone::faces_int()
{
  return mes_faces_int;
}

// Description:
//    Reordonne les elements suivant la convention
//    employe par Trio-U.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Zone::reordonner()
{
  elem.reordonner();
}


// //    JY 26/08/97
// Description:
//    Renvoie le nombre de faces frontiere de la zone
//    du type specifie.
//    C'est la somme des nombres de  bords, de raccords
//    et de faces internes du type specifie.
// Precondition:
// Parametre: Type_Face type
//    Signification: un type de face
//                  (certains elements geometriques ont plusieurs types de faces)
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de faces frontiere de la zone du type specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Zone::nb_faces_frontiere(Type_Face type) const
{
  return
    nb_faces_bord(type) +
    nb_faces_int(type) +
    nb_faces_raccord(type);
}

// Decription:
// Renvoie le tableau des indices des faces distantes de bord
inline const ArrOfInt& Zone::ind_faces_virt_bord() const
{
  return ind_faces_virt_bord_;
}

// Description:
// renvoie le nombre d'aretes reelles.
inline int Zone::
nb_aretes() const
{
  return Aretes_som.dimension(0);
}

// Description:
// renvoie le nombre d'aretes total (reelles+virtuelles).
inline int Zone::
nb_aretes_tot() const
{
  return Aretes_som.dimension_tot(0);
}

// Description:
// renvoie le numero du jeme sommet de la ieme arete.
inline int Zone::
arete_sommets(int i, int j) const
{
  return Aretes_som(i, j);
}

// Description:
// renvoie le numero de la jeme arete du ieme element.
inline int Zone::
elem_aretes(int i, int j) const
{
  return Elem_Aretes(i, j);
}

// Description:
// renvoie le tableau de connectivite aretes/sommets.
inline const IntTab& Zone::
aretes_som() const
{
  return Aretes_som;
}
inline IntTab& Zone::
set_aretes_som()
{
  return Aretes_som;
}

// Description:
// renvoie le tableau de connectivite elements/aretes.
inline const IntTab& Zone::
elem_aretes() const
{
  return Elem_Aretes;
}
inline IntTab& Zone::
set_elem_aretes()
{
  return Elem_Aretes;
}
#endif




