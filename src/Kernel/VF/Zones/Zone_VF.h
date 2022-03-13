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
// File:        Zone_VF.h
// Directory:   $TRUST_ROOT/src/Kernel/VF/Zones
// Version:     /main/35
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_VF_included
#define Zone_VF_included

#include <Vect_Front_VF.h>
#include <TRUSTArrays.h>
#include <Zone_dis.h>

class Faces;
class Zone_Cl_dis_base;


//.DESCRIPTION class Zone_VF
//
// Cette classe abstraite contient les informations geometriques
// communes aux methodes de Volumes Finis (methodes VDF et VEF par exemple)
//
//.SECTION voir aussi
// Zone_dis_base

class Zone_VF : public Zone_dis_base
{

  Declare_base(Zone_VF);

public :

  virtual double face_normales(int , int ) const =0;
  void calculer_face_surfaces(const DoubleVect& surfaces)
  {
    face_surfaces_ = surfaces;
  };
  virtual const DoubleVect& face_surfaces() const
  {
    return face_surfaces_;
  }
  virtual inline double face_surfaces(int i) const
  {
    return face_surfaces_(i);
  };
  virtual inline double surface(int i) const
  {
    return face_surfaces(i);
  };

  void discretiser() override;
  void infobord();
  void info_elem_som();
  //void creer_face_voisins_virt();
  //void creer_elem_faces_virt();
  //void creer_face_sommets_virt();
  void calculer_porosites();
  void calculer_diametres_hydrauliques();
  void calculer_sections_passage();
  void calculer_coefficient_echange_thermique();
  void calculer_coefficient_frottement();
  void marquer_faces_double_contrib(const Conds_lim&);
  virtual void typer_elem(Zone&);

  virtual void remplir_face_voisins_fictifs(const Zone_Cl_dis_base& ) ;
  virtual Faces* creer_faces();
  virtual void reordonner(Faces& );
  inline int nb_joints() const;
  inline int premiere_face_int() const;
  inline int nb_faces() const;
  inline int nb_faces_tot() const;
  inline int nb_som_face() const;
  inline int nb_faces_bord() const;
  inline int nb_faces_bord_tot() const;
  inline int premiere_face_bord() const;
  inline int nb_faces_internes() const;

  inline double xv(int ,int ) const;
  inline double xp(int ,int ) const;
  inline double xa(int ,int ) const;
  inline double porosite_face(int ) const;
  inline double porosite_elem(int ) const;
  inline double diametre_hydraulique_face(int ) const;
  inline double section_passage_face(int ) const;
  inline double coefficient_frottement(int ) const;
  inline double coefficient_echange_thermique(int ) const;

  inline int face_numero_bord(int num_face) const;
  inline IntTab& face_numero_bord();
  inline const IntTab& face_numero_bord() const;
  void remplir_face_numero_bord();

  inline virtual const IntVect& orientation() const;
  inline virtual int orientation(int ) const;
  inline virtual int orientation_si_definie(int) const;

  DoubleTab normalized_boundaries_outward_vector(int global_face_number, double scale_factor) const;
  inline DoubleTab& xv();
  inline const DoubleTab& xv() const;
  inline DoubleTab& xp();
  inline const DoubleTab& xp() const;
  inline DoubleTab& xa();
  inline const DoubleTab& xa() const;
  inline DoubleVect& volumes_entrelaces();
  inline const DoubleVect& volumes_entrelaces() const;
  inline double volumes_entrelaces(int num_face) const;
  inline const DoubleTab& volumes_entrelaces_dir() const;
  inline DoubleTab& volumes_entrelaces_dir();

  inline DoubleVect& porosite_face();
  inline const DoubleVect& porosite_face() const;
  inline DoubleVect& porosite_elem();
  inline const DoubleVect& porosite_elem() const;
  inline DoubleVect& diametre_hydraulique_face();
  inline const DoubleVect& diametre_hydraulique_face() const;
  inline const DoubleVect section_passage_face() const;
  inline DoubleTab& diametre_hydraulique_elem();
  inline const DoubleTab& diametre_hydraulique_elem() const;
  inline DoubleVect& coefficient_frottement();
  inline const DoubleVect& coefficient_frottement() const;
  inline DoubleVect& coefficient_echange_thermique();
  inline const DoubleVect& coefficient_echange_thermique() const;

  inline const Joint& joint(int i) const;
  inline Joint& joint(int i);

  inline Frontiere_dis_base& frontiere_dis(int ) override;
  inline const Frontiere_dis_base& frontiere_dis(int ) const override;

  inline int nb_frontiere_dis() const
  {
    return les_bords_.size();
  };
  inline const Front_VF& front_VF(int ) const;
  inline double volumes(int i) const;
  inline double inverse_volumes(int i) const;
  inline int face_voisins(int num_face,int i) const;
  inline int elem_faces(int i,int j) const;
  inline int face_sommets(int i,int j) const;

  inline DoubleVect& volumes();
  inline DoubleVect& inverse_volumes();                        // Tableau pour optimiser le code
  inline const DoubleVect& volumes() const;
  inline const DoubleVect& inverse_volumes() const;        // Tableau pour optimiser le code
  inline IntTab& face_voisins() override;
  inline const IntTab& face_voisins() const override;
  inline const IntTab& face_voisins_fictifs() const;
  inline void face_voisins_reel_fictif(int face,int& el0,int& elf) const;
  inline IntTab& elem_faces();
  inline const IntTab& elem_faces() const;
  inline ArrOfInt& faces_doubles();
  inline const ArrOfInt& faces_doubles() const;
  inline IntTab& face_sommets() override;
  inline const IntTab& face_sommets() const override;
  void modifier_pour_Cl(const Conds_lim&) override;

  int numero_face_local(int face, int elem) const;
  inline int numero_sommet_local(int som, int elem) const;

  inline const IntTab& get_num_fac_loc() const
  {
    return num_fac_loc_;
  };
  inline int get_num_fac_loc(int, int) const;
  void construire_num_fac_loc();

  inline const ArrOfInt& ind_faces_virt_bord() const;
  inline int est_une_face_virt_bord(int) const;
  inline int fbord(int f) const //renvoie l'indice de face de bord de f si f est de bord, -1 sinon
  {
    return f < premiere_face_int() ? f : f < nb_faces() ? -1 : ind_faces_virt_bord()[f - nb_faces()];
  }

  void construire_face_virt_pe_num();
  const IntTab& face_virt_pe_num() const;

  virtual void creer_tableau_faces(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual void creer_tableau_aretes(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual void creer_tableau_faces_bord(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual const MD_Vector& md_vector_faces_bord() const
  {
    return md_vector_faces_front_;
  }
  virtual const MD_Vector& md_vector_faces() const
  {
    return md_vector_faces_;
  }
  // Attention, si les aretes ne sont pas remplies, le md_vector_ est nul
  virtual const MD_Vector& md_vector_aretes() const
  {
    return md_vector_aretes_;
  }

  virtual const DoubleTab& xv_bord() const;

private:
  DoubleVect face_surfaces_;                // surface des faces

protected:

  DoubleVect volumes_;                          // volumes des elements
  DoubleVect inverse_volumes_;                  // inverse du volumes des elements
  DoubleVect volumes_entrelaces_;            // volumes entrelaces pour l'integration des Qdm
  DoubleTab volumes_entrelaces_dir_;        // volumes entrelaces par cote

  DoubleVect porosite_elem_;                 // Porosites volumiques pour les volumes de
  // controle de masse
  DoubleVect porosite_face_;              // Porosites surfaciques en masse et volumiques

  // en quantite de mouvement
  DoubleVect diametre_hydraulique_face_;  //diametres hydrauliques des faces
  DoubleTab diametre_hydraulique_elem_;  //diametres hydrauliques des elements
  DoubleVect coefficient_echange_thermique_;           // remplissage des coefficients d'echange_thermique
  DoubleVect coefficient_frottement_;                          // remplissage des coefficient de frottement

  IntTab face_voisins_;                          // connectivite face/elements
  IntTab face_voisins_fictifs_;           // connectivite face/elements fictifs
  DoubleTab xp_;                            // centres de gravite des elements
  DoubleTab xv_;                            // centres de gravite des faces
  mutable DoubleTab xv_bord_;                       //xv_, mais faces de bord seulement (creation sur demande)

  IntTab elem_faces_;                           // connectivite element/faces
  IntTab face_sommets_;                           // sommets des faces
  DoubleTab xa_;                            // centres de gravite des aretes
  IntTab face_numero_bord_;                     // connectivite face/numero_bord

  // Descripteur parallele pour les tableaux aux faces (size() == nb_faces())
  MD_Vector md_vector_faces_;
  // Idem pour les faces frontiere (size() == nb_faces_front())
  MD_Vector md_vector_faces_front_;
  // Celui pour les aretes
  MD_Vector md_vector_aretes_;

  VECT(Front_VF) les_bords_;

  IntTab num_fac_loc_;
  ArrOfInt faces_doubles_;                  // faces a double contribution (faces periodiques et items communs)

  // Pour chaque face virtuelle i avec nb_faces_<=i<nb_faces_tot on a :
  // face_virt_pe_num_(i-nb_faces_,0) = numero du PE qui possede la face
  // face_virt_pe_num_(i-nb_faces_,1) = numero local de cette face sur le PE qui le possede
  IntTab face_virt_pe_num_;

  virtual void remplir_elem_faces()=0;

};

// Renvoie le numero local de face a partir d'un numero de face global et de elem local (0 ou 1)
// Methode beaucoup plus rapide que Zone_VF::numero_face_local(face,elem)
inline int Zone_VF::get_num_fac_loc(int face,int voisin) const
{
  assert(voisin==0 || voisin==1);
  return num_fac_loc_(face,voisin);
}

inline int Zone_VF::numero_sommet_local(int som, int elem) const
{
  int nse=zone().nb_som_elem();
  const IntTab& les_elems=zone().les_elems();
  for(int som_loc=0; som_loc<nse; som_loc++)
    {
      if(les_elems(elem, som_loc)==som)
        return som_loc;
    }
  return -1;
}

// Description:
inline double Zone_VF::porosite_face(int i) const
{
  return porosite_face_[i];
}

// Description:
inline DoubleVect& Zone_VF::porosite_face()
{
  return porosite_face_;
}

// Description:
inline const DoubleVect& Zone_VF::porosite_face() const
{
  return porosite_face_;
}

// Description:
inline DoubleVect& Zone_VF::porosite_elem()
{
  return porosite_elem_;
}

// Description:
inline const DoubleVect& Zone_VF::porosite_elem() const
{
  return porosite_elem_;
}

// Description:
inline double Zone_VF::porosite_elem(int i) const
{
  return porosite_elem_[i];
}

// Description:
inline DoubleTab& Zone_VF::diametre_hydraulique_elem()
{
  return diametre_hydraulique_elem_;
}

// Description:
inline const DoubleTab& Zone_VF::diametre_hydraulique_elem() const
{
  return diametre_hydraulique_elem_;
}

// Description:
inline DoubleVect& Zone_VF::diametre_hydraulique_face()
{
  return diametre_hydraulique_face_;
}

// Description:
inline const DoubleVect& Zone_VF::diametre_hydraulique_face() const
{
  return diametre_hydraulique_face_;
}

// Description:
inline double Zone_VF::diametre_hydraulique_face(int i) const
{
  return diametre_hydraulique_face_[i];
}

// Description:
inline const DoubleVect Zone_VF::section_passage_face() const
{
  // remplissage des sections de passage
  DoubleVect section_passage_face_(face_surfaces_);
  for (int i=0; i<face_surfaces_.size_array(); i++)
    section_passage_face_[i] = section_passage_face(i);
  return section_passage_face_;
}

// Description:
inline double Zone_VF::section_passage_face(int i) const
{
  return face_surfaces_[i] * porosite_face_[i];
}

// Description:
inline DoubleVect& Zone_VF::coefficient_frottement()
{
  return coefficient_frottement_;
}

// Description:
inline const DoubleVect& Zone_VF::coefficient_frottement() const
{
  return coefficient_frottement_;
}

// Description:
inline double Zone_VF::coefficient_frottement(int i) const
{
  return coefficient_frottement_[i];
}

// Description:
inline DoubleVect& Zone_VF::coefficient_echange_thermique()
{
  return coefficient_echange_thermique_;
}

// Description:
inline const DoubleVect& Zone_VF::coefficient_echange_thermique() const
{
  return coefficient_echange_thermique_;
}

// Description:
inline double Zone_VF::coefficient_echange_thermique(int i) const
{
  return coefficient_echange_thermique_[i];
}


// Description:
// renvoie le ieme joint.
inline const Joint& Zone_VF::joint(int i) const
{
  return zone().joint(i);
}

// Description:
// renvoie le ieme joint.
inline Joint& Zone_VF::joint(int i)
{
  return zone().joint(i);
}

// Description:
// renvoie le volume de la maille i.
inline double Zone_VF::volumes(int i) const
{
  return volumes_[i];
}
inline double Zone_VF::inverse_volumes(int i) const
{
  return inverse_volumes_[i];
}

// Description:
// renvoie le tableaux des volumes des mailles.
inline DoubleVect& Zone_VF::volumes()
{
  return volumes_;
}
inline DoubleVect& Zone_VF::inverse_volumes()
{
  return inverse_volumes_;
}

// Description:
// renvoie le tableaux des volumes des mailles.
inline const DoubleVect& Zone_VF::volumes() const
{
  return volumes_;
}
inline const DoubleVect& Zone_VF::inverse_volumes() const
{
  return inverse_volumes_;
}
// Description:
// renvoie l'element voisin de numface dans la direction i.
// i=0 : dans le sens oppose de l'axe orthogonal a la face numface.
// i=1 : dans le sens de l'axe orthogonal a la face numface.
// exemple :
//
//                |   0     |     1    |
//                      numface
// renvoie -1 si l'element n'existe pas (au bord).
inline int Zone_VF::face_voisins(int num_face,int i) const
{
  return face_voisins_(num_face,i);
}

// Description:
// renvoie le tableaux des volumes des connectivites face elements
// cf au dessus.
inline IntTab& Zone_VF::face_voisins()
{
  return face_voisins_;
}

// Description:
// cf au dessus.
inline const IntTab& Zone_VF::face_voisins() const
{
  return face_voisins_;
}

inline const IntTab& Zone_VF::face_voisins_fictifs() const
{
  return face_voisins_fictifs_;
}

// Description:
// renvoie dans el0 le numero de l'elt a l'interieur
// renvoie dans elf le numero de l'elt  fictif (-1 si il n'existe pas)
//
inline void  Zone_VF::face_voisins_reel_fictif(int face,int& el0,int& elf) const
{
  assert(face<premiere_face_int());
  el0 = face_voisins(face,0);
  elf = face_voisins(face,1);
  if (el0==-1)
    {
      el0=elf;
      elf=face_voisins_fictifs_(face,0);
    }
  if (elf==-1) elf=face_voisins_fictifs_(face,1);

}


// Description:
// renvoie le nombre de joints
inline int Zone_VF::nb_joints() const
{
  return zone().nb_joints();
}

// Description:
// une face est interne ssi elle separe deux elements.
// renvoie le numero de la premiere face interne.
inline int Zone_VF::premiere_face_int() const
{
  return nb_faces_bord();
}

// Description:
// renvoie le nombre global de faces.
inline int Zone_VF::nb_faces() const
{
  return face_sommets_.dimension(0);
}

// Description:
// renvoie le nombre total de faces.
// C'est-a-dire faces reelles + faces virtuelles
inline int Zone_VF::nb_faces_tot() const
{
  return face_sommets_.dimension_tot(0);
}


// Description:
// renvoie le nombre de sommets par face.
// Rque : on a suppose que toutes les faces avait
// le meme nombre de sommet ce qui exclue des elements
// comme le prisme.
inline int Zone_VF::nb_som_face() const
{
  return face_sommets_.dimension(1);
}

// Description:
// renvoie le nombre de faces sur lesquelles
// sont appliquees les conditions limites :
// bords, raccords, plaques.
inline int Zone_VF::nb_faces_bord() const
{
  return zone().nb_faces_frontiere();
}


// Description:
// renvoie le numero de la premiere des faces sur lesquelles
// sont appliquees les conditions limites :
// bords, raccords, plaques.
inline int Zone_VF::premiere_face_bord() const
{
  return 0;
}

// Description:
// une face est interne ssi elle separe deux elements.
// renvoie le nombre de faces internes.
inline int Zone_VF::nb_faces_internes() const
{
  return nb_faces()- nb_faces_bord();
}

// Description:
// renvoie le numero de le ieme face de la maille num_elem
// la facon dont ces faces sont numerotees est
// laisse a la responsabilite des classes derivees
inline int Zone_VF::elem_faces(int num_elem, int i) const
{
  return elem_faces_(num_elem, i);
}

// Description:
// renvoie le tableau de connectivite element/faces
inline IntTab& Zone_VF::elem_faces()
{
  return elem_faces_;
}

// Description:
// cf au dessus.
inline const IntTab& Zone_VF::elem_faces() const
{
  return elem_faces_;
}

// Description:
// renvoie 1 pour les faces appartenant a un bord perio ou un item commun, 0 par defaut
inline ArrOfInt& Zone_VF::faces_doubles()
{
  return faces_doubles_;
}

// Description:
// cf au dessus
inline const ArrOfInt& Zone_VF::faces_doubles() const
{
  return faces_doubles_;
}

// Description:
// renvoie le numero du ieme sommet de la face num_face.
inline int Zone_VF::face_sommets(int num_face, int i) const
{
  return face_sommets_(num_face, i);
}

// Description:
// renvoie le tableau de connectivite faces/sommets.
inline IntTab& Zone_VF::face_sommets()
{
  return face_sommets_;
}

// Description:
// cf au dessus.
inline const IntTab& Zone_VF::face_sommets() const
{
  return face_sommets_;
}

// Description:
// renvoie la ieme frontiere_discrete.
inline const Frontiere_dis_base& Zone_VF::frontiere_dis(int i) const
{
  return les_bords_[i];
}

// Description:
// renvoie la ieme frontiere_discrete.
inline Frontiere_dis_base& Zone_VF::frontiere_dis(int i)
{
  return les_bords_[i];
}

// Description:
// renvoie la ieme frontiere_discrete.
inline const Front_VF& Zone_VF::front_VF(int i) const
{
  return les_bords_[i];
}

// Description:
inline double Zone_VF::xv(int num_face,int k) const
{
  return xv_(num_face,k);
}

inline const IntVect& Zone_VF::orientation() const
{
  Cerr<<"Try using orientation(num_face) instead of orientation() if the mesh is axis oriented."<< finl;
  exit();
  throw;
  return orientation();
}

inline int Zone_VF::orientation_si_definie(int num_face) const
{
  for (int dir=0; dir<dimension; dir++)
    {
      if (est_egal(std::fabs(face_normales(num_face, dir)), face_surfaces(num_face)))
        return dir;
    }
  return -1;
}

inline int Zone_VF::orientation(int num_face) const
{
  for (int dir=0; dir<dimension; dir++)
    {
      if (est_egal(std::fabs(face_normales(num_face, dir)), face_surfaces(num_face)))
        return dir;
    }
  Cerr<<"Your mesh is not axis oriented."<<finl;
  Cerr<<"Try recoding using face_normales(). Example for g(face)=gravite[orientation[face]];" << finl;
  Cerr<<"Compute something like: g(face)=Sum(i=0;i<gravite.size();i++) of gravite[i]*face_normales(face,i)/surface(face);" << finl;
  exit();
  return -1;
}

// Description:
inline DoubleTab& Zone_VF::xv()
{
  return xv_;
}

// Description:
inline const DoubleTab& Zone_VF::xv() const
{
  return xv_;
}

// Description:
inline double Zone_VF::xp(int num_elem,int k) const
{
  return xp_(num_elem,k);
}

// Description:
inline DoubleTab& Zone_VF::xp()
{
  return xp_;
}

// Description:
inline const DoubleTab& Zone_VF::xp() const
{
  return xp_;
}

// Description:
inline double Zone_VF::xa(int num_arete,int k) const
{
  return xa_(num_arete,k);
}

// Description:
inline DoubleTab& Zone_VF::xa()
{
  return xa_;
}

// Description:
inline const DoubleTab& Zone_VF::xa() const
{
  return xa_;
}

// Description:
inline DoubleVect& Zone_VF::volumes_entrelaces()
{
  return volumes_entrelaces_;
}

// Description:
inline const DoubleVect& Zone_VF::volumes_entrelaces() const
{
  return volumes_entrelaces_;
}

// Description:
inline double Zone_VF::volumes_entrelaces(int num_face) const
{
  return volumes_entrelaces_[num_face];
}

// Decription:
// renvoie le tableau des volumes entrelaces par cote.
inline DoubleTab& Zone_VF::volumes_entrelaces_dir()
{
  return volumes_entrelaces_dir_;
}

// Decription:
inline const DoubleTab& Zone_VF::volumes_entrelaces_dir() const
{
  return volumes_entrelaces_dir_;
}

inline const ArrOfInt& Zone_VF::ind_faces_virt_bord() const
{
  return zone().ind_faces_virt_bord();
}


// Description: renvoie 1 si face est une face virtuelle de bord, 0 sinon
inline int Zone_VF::est_une_face_virt_bord(int face) const
{
  if (face<nb_faces() || ind_faces_virt_bord()(face-nb_faces())==-1)
    return 0;
  else
    return 1;
}


// Description:
inline int Zone_VF::face_numero_bord(int num_face) const
{
  assert(num_face < nb_faces());
  return face_numero_bord_(num_face);
}

// Description:
inline IntTab& Zone_VF::face_numero_bord()
{
  return face_numero_bord_;
}

// Description:
inline const IntTab& Zone_VF::face_numero_bord() const
{
  return face_numero_bord_;
}

#endif




