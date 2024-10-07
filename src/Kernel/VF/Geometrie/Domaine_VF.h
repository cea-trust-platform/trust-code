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

#ifndef Domaine_VF_included
#define Domaine_VF_included

#include <Domaine_dis_base.h>
#include <Domaine_forward.h>
#include <TRUSTArrays.h>
#include <Domaine_dis.h>
#include <Front_VF.h>

class Domaine_Cl_dis_base;

/*! @brief class Domaine_VF
 *
 *  Cette classe abstraite contient les informations geometriques
 *  communes aux methodes de Volumes Finis (methodes VDF et VEF par exemple)
 *
 * @sa Domaine_dis_base
 */
class Domaine_VF : public Domaine_dis_base
{
  Declare_base(Domaine_VF);
public :
  virtual double face_normales(int face, int comp) const { return face_normales_(face,comp); }
  virtual DoubleTab& face_normales() { return face_normales_; }
  virtual const DoubleTab& face_normales() const { return face_normales_; }
  void calculer_face_surfaces(const DoubleVect& surfaces) { face_surfaces_ = surfaces; }
  virtual const DoubleVect& face_surfaces() const { return face_surfaces_; }
  virtual inline double face_surfaces(int i) const { return face_surfaces_(i); }
  virtual inline double surface(int i) const { return face_surfaces(i); }

  void discretiser() override;
  void discretiser_no_face() override;
  void typer_discretiser_ss_domaine(int i) override;
  void infobord();
  void info_elem_som();
  void marquer_faces_double_contrib(const Conds_lim&);
  virtual void typer_elem(Domaine&) {}

  virtual void remplir_face_voisins_fictifs(const Domaine_Cl_dis_base& ) ;
  virtual Faces* creer_faces();
  virtual void reordonner(Faces&);
  void renumeroter(Faces&);
  inline int nb_joints() const { return domaine().nb_joints(); }
  inline int premiere_face_int() const;
  inline int nb_faces() const;
  inline int nb_faces_tot() const;
  inline int nb_som_face() const;
  inline int nb_faces_bord() const;
  inline int nb_faces_bord_tot() const;
  inline int premiere_face_bord() const;
  inline int nb_faces_internes() const;

  inline double xv(int num_face,int k) const { return xv_(num_face,k); }
  inline double xp(int num_elem,int k) const { return xp_(num_elem,k); }
  inline double xa(int num_arete,int k) const { return xa_(num_arete,k); }

  inline int face_numero_bord(int num_face) const;
  inline IntTab& face_numero_bord() { return face_numero_bord_; }
  inline const IntTab& face_numero_bord() const { return face_numero_bord_; }
  void remplir_face_numero_bord();

  inline ArrOfInt& est_face_bord() { return est_face_bord_; }
  inline const ArrOfInt& est_face_bord() const { return est_face_bord_; }

  inline virtual const IntVect& orientation() const;
  inline virtual int orientation(int ) const;
  inline virtual int orientation_si_definie(int) const;

  DoubleTab normalized_boundaries_outward_vector(int global_face_number, double scale_factor) const;
  inline DoubleTab& xv() { return xv_;}
  inline const DoubleTab& xv() const { return xv_;}
  inline DoubleTab& xp() { return xp_; }
  inline const DoubleTab& xp() const { return xp_; }
  inline DoubleTab& xa() { return xa_; }
  inline const DoubleTab& xa() const { return xa_; }
  inline DoubleVect& volumes_entrelaces() { return volumes_entrelaces_; }
  inline const DoubleVect& volumes_entrelaces() const { return volumes_entrelaces_; }
  inline double volumes_entrelaces(int num_face) const { return volumes_entrelaces_[num_face]; }
  inline const DoubleTab& volumes_entrelaces_dir() const { return volumes_entrelaces_dir_; }
  inline DoubleTab& volumes_entrelaces_dir() { return volumes_entrelaces_dir_; } // renvoie le tableau des volumes entrelaces par cote.

  inline const Joint& joint(int i) const { return domaine().joint(i); }
  inline Joint& joint(int i) { return domaine().joint(i); }

  inline Frontiere_dis_base& frontiere_dis(int ) override;
  inline const Frontiere_dis_base& frontiere_dis(int ) const override;

  inline int nb_frontiere_dis() const { return les_bords_.size(); }
  inline const Front_VF& front_VF(int i) const { return les_bords_[i]; } // renvoie la ieme frontiere_discrete.
  inline double volumes(int i) const { return volumes_[i]; }
  inline double inverse_volumes(int i) const { return inverse_volumes_[i]; }
  inline int face_voisins(int num_face,int i) const;
  inline int elem_faces(int i,int j) const;
  inline int face_sommets(int i,int j) const;

  inline DoubleVect& volumes() { return volumes_; }
  inline DoubleVect& inverse_volumes() { return inverse_volumes_; } // Tableau pour optimiser le code
  inline const DoubleVect& volumes() const { return volumes_; }
  inline const DoubleVect& inverse_volumes() const { return inverse_volumes_; } // Tableau pour optimiser le code
  inline IntTab& face_voisins() override;
  inline const IntTab& face_voisins() const override;
  inline const IntTab& face_voisins_fictifs() const { return face_voisins_fictifs_; }
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

  inline const IntTab& get_num_fac_loc() const { return num_fac_loc_; }
  inline int get_num_fac_loc(int, int) const;
  void construire_num_fac_loc();

  inline const ArrOfInt& ind_faces_virt_bord() const { return domaine().ind_faces_virt_bord(); }
  inline int est_une_face_virt_bord(int) const;
  inline int fbord(int f) const //renvoie l'indice de face de bord de f si f est de bord, -1 sinon
  {
    return f < premiere_face_int() ? f : f < nb_faces() ? -1 : ind_faces_virt_bord()[f - nb_faces()];
  }

  void construire_face_virt_pe_num();
  const IntTab& face_virt_pe_num() const;

  virtual void creer_tableau_faces(Array_base&, RESIZE_OPTIONS opt = RESIZE_OPTIONS::COPY_INIT) const;
  virtual void creer_tableau_aretes(Array_base&, RESIZE_OPTIONS opt = RESIZE_OPTIONS::COPY_INIT) const;
  virtual void creer_tableau_faces_bord(Array_base&, RESIZE_OPTIONS opt = RESIZE_OPTIONS::COPY_INIT) const;
  virtual const MD_Vector& md_vector_faces_bord() const { return md_vector_faces_front_; }
  virtual const MD_Vector& md_vector_faces() const { return md_vector_faces_; }
  // Attention, si les aretes ne sont pas remplies, le md_vector_ est nul
  virtual const MD_Vector& md_vector_aretes() const { return md_vector_aretes_; }

  virtual const DoubleTab& xv_bord() const;
  DoubleTab calculer_xgr() const;

  //produit scalaire (a - ma).(b - mb)
  inline double dot (const double *a, const double *b, const double *ma = nullptr, const double *mb = nullptr) const;

  //produit vectoriel
  inline std::array<double, 3> cross(int dima, int dimb, const double *a, const double *b, const double *ma = nullptr, const double *mb = nullptr) const;

  inline virtual double dist_norm(int ) const { Cerr << __func__ << " method should be overrided in a derived class !! " << finl; throw; }
  inline virtual double dist_norm_bord(int ) const { Cerr << __func__ << " method should be overrided in a derived class !! " << finl; throw; }
  inline virtual double dist_face_elem0(int ,int ) const { Cerr << __func__ << " method should be overrided in a derived class !! " << finl; throw; }
  inline virtual double dist_face_elem1(int ,int ) const { Cerr << __func__ << " method should be overrided in a derived class !! " << finl; throw; }
  inline virtual double dist_face_elem0_period(int ,int ,double ) const { Cerr << __func__ << " method should be overrided in a derived class !! " << finl; throw; }
  inline virtual double dist_face_elem1_period(int ,int ,double ) const { Cerr << __func__ << " method should be overrided in a derived class !! " << finl; throw; }

// Methodes pour le calcul et l'appel de la distance au bord solide le plus proche ; en entree on met le tableau des CL de la QDM
  void init_dist_paroi_globale(const Conds_lim& conds_lim) override;
  const DoubleTab& normale_paroi_elem()  const {return n_y_elem_;} ;
  const DoubleTab& normale_paroi_faces() const {return n_y_faces_;} ;

private:
  DoubleVect face_surfaces_;                // surface des faces

protected:
  DoubleVect volumes_;                          // volumes des elements
  DoubleVect inverse_volumes_;                  // inverse du volumes des elements
  DoubleVect volumes_entrelaces_;            // volumes entrelaces pour l'integration des Qdm
  DoubleTab volumes_entrelaces_dir_;        // volumes entrelaces par cote
  DoubleTab face_normales_;             // normales aux faces

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

  IntTab num_fac_loc_;     // renvoie pour une face son numero local dans l'element
  //ArrOfInt faces_perio_;   // faces periodiques (utile si on boucle de 0 a nb_faces_tot)
  ArrOfInt faces_doubles_; // faces a double contribution (faces periodiques et items communs). Utile si on boucle de 0 a nb_faces pour une reduction ensuite
  ArrOfInt est_face_bord_; // renvoie pour une face reelle ou virtuelle: 0 si interne, 1 si face de bord non periodique, 2 si face de bord periodique

  // Pour chaque face virtuelle i avec nb_faces_<=i<nb_faces_tot on a :
  // face_virt_pe_num_(i-nb_faces_,0) = numero du PE qui possede la face
  // face_virt_pe_num_(i-nb_faces_,1) = numero local de cette face sur le PE qui le possede
  IntTab face_virt_pe_num_;

  DoubleTab n_y_elem_ ; // vecteur normal entre le bord le plus proche et l'element
  DoubleTab n_y_faces_; // vecteur normal entre le bord le plus proche et la face

  int nb_elem_std_=-10;                     // nombre d'elements standard
  int nb_faces_std_=-10;                    // nombre de faces standard
  IntVect rang_elem_non_std_;    // rang_elem_non_std_= -1 si l'element est standard
  // rang_elem_non_std_= rang de l'element dans les tableaux
  // relatifs aux elements non standards
};

// Renvoie le numero local de face a partir d'un numero de face global et de elem local (0 ou 1)
// Methode beaucoup plus rapide que Domaine_VF::numero_face_local(face,elem)
inline int Domaine_VF::get_num_fac_loc(int face,int voisin) const
{
  assert(voisin==0 || voisin==1);
  return num_fac_loc_(face,voisin);
}

inline int Domaine_VF::numero_sommet_local(int som, int elem) const
{
  int nse=domaine().nb_som_elem();
  const IntTab& les_elems=domaine().les_elems();
  for(int som_loc=0; som_loc<nse; som_loc++)
    {
      if(les_elems(elem, som_loc)==som)
        return som_loc;
    }
  return -1;
}

/*! @brief renvoie l'element voisin de numface dans la direction i.
 *
 * i=0 : dans le sens oppose de l'axe orthogonal a la face numface.
 *  i=1 : dans le sens de l'axe orthogonal a la face numface.
 *  exemple :
 *
 *                 |   0     |     1    |
 *                       numface
 *  renvoie -1 si l'element n'existe pas (au bord).
 *
 */
inline int Domaine_VF::face_voisins(int num_face,int i) const
{
  return face_voisins_(num_face,i);
}

/*! @brief renvoie le tableaux des volumes des connectivites face elements cf au dessus.
 *
 */
inline IntTab& Domaine_VF::face_voisins()
{
  return face_voisins_;
}

/*! @brief cf au dessus.
 *
 */
inline const IntTab& Domaine_VF::face_voisins() const
{
  return face_voisins_;
}

/*! @brief renvoie dans el0 le numero de l'elt a l'interieur renvoie dans elf le numero de l'elt  fictif (-1 si il n'existe pas)
 *
 *
 *
 */
inline void  Domaine_VF::face_voisins_reel_fictif(int face,int& el0,int& elf) const
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

/*! @brief une face est interne ssi elle separe deux elements.
 *
 * renvoie le numero de la premiere face interne.
 *
 */
inline int Domaine_VF::premiere_face_int() const
{
  return nb_faces_bord();
}

/*! @brief renvoie le nombre global de faces.
 *
 */
inline int Domaine_VF::nb_faces() const
{
  return face_sommets_.dimension(0);
}

/*! @brief renvoie le nombre total de faces.
 *
 * C'est-a-dire faces reelles + faces virtuelles
 *
 */
inline int Domaine_VF::nb_faces_tot() const
{
  return face_sommets_.dimension_tot(0);
}


/*! @brief renvoie le nombre de sommets par face.
 *
 * Rque : on a suppose que toutes les faces avait
 *  le meme nombre de sommet ce qui exclue des elements
 *  comme le prisme.
 *
 */
inline int Domaine_VF::nb_som_face() const
{
  return face_sommets_.dimension(1);
}

/*! @brief renvoie le nombre de faces sur lesquelles sont appliquees les conditions limites :
 *
 *  bords, raccords, plaques.
 *
 */
inline int Domaine_VF::nb_faces_bord() const
{
  return domaine().nb_faces_frontiere();
}

/*! @brief renvoie le numero de la premiere des faces sur lesquelles sont appliquees les conditions limites :
 *
 *  bords, raccords, plaques.
 *
 */
inline int Domaine_VF::premiere_face_bord() const
{
  return 0;
}

/*! @brief une face est interne ssi elle separe deux elements.
 *
 * renvoie le nombre de faces internes.
 *
 */
inline int Domaine_VF::nb_faces_internes() const
{
  return nb_faces()- nb_faces_bord();
}

/*! @brief renvoie le numero de le ieme face de la maille num_elem la facon dont ces faces sont numerotees est
 *
 *  laisse a la responsabilite des classes derivees
 *
 */
inline int Domaine_VF::elem_faces(int num_elem, int i) const
{
  return elem_faces_(num_elem, i);
}

/*! @brief renvoie le tableau de connectivite element/faces
 *
 */
inline IntTab& Domaine_VF::elem_faces()
{
  return elem_faces_;
}

/*! @brief cf au dessus.
 *
 */
inline const IntTab& Domaine_VF::elem_faces() const
{
  return elem_faces_;
}

/*! @brief renvoie 1 pour les faces appartenant a un bord perio ou un item commun, 0 par defaut
 *
 */
inline ArrOfInt& Domaine_VF::faces_doubles()
{
  return faces_doubles_;
}

/*! @brief cf au dessus
 *
 */
inline const ArrOfInt& Domaine_VF::faces_doubles() const
{
  return faces_doubles_;
}

/*! @brief renvoie le numero du ieme sommet de la face num_face.
 *
 */
inline int Domaine_VF::face_sommets(int num_face, int i) const
{
  return face_sommets_(num_face, i);
}

/*! @brief renvoie le tableau de connectivite faces/sommets.
 *
 */
inline IntTab& Domaine_VF::face_sommets()
{
  return face_sommets_;
}

/*! @brief cf au dessus.
 *
 */
inline const IntTab& Domaine_VF::face_sommets() const
{
  return face_sommets_;
}

/*! @brief renvoie la ieme frontiere_discrete.
 *
 */
inline const Frontiere_dis_base& Domaine_VF::frontiere_dis(int i) const
{
  return les_bords_[i];
}

/*! @brief renvoie la ieme frontiere_discrete.
 *
 */
inline Frontiere_dis_base& Domaine_VF::frontiere_dis(int i)
{
  return les_bords_[i];
}


inline const IntVect& Domaine_VF::orientation() const
{
  Cerr<<"Try using orientation(num_face) instead of orientation() if the mesh is axis oriented."<< finl;
  exit();
  throw;
  //return orientation();
}

inline int Domaine_VF::orientation_si_definie(int num_face) const
{
  for (int dir=0; dir<dimension; dir++)
    {
      if (est_egal(std::fabs(face_normales(num_face, dir)), face_surfaces(num_face)))
        return dir;
    }
  return -1;
}

inline int Domaine_VF::orientation(int num_face) const
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

/*! @brief renvoie 1 si face est une face virtuelle de bord, 0 sinon
 *
 */
inline int Domaine_VF::est_une_face_virt_bord(int face) const
{
  if (face<nb_faces() || ind_faces_virt_bord()[face-nb_faces()]==-1)
    return 0;
  else
    return 1;
}

inline int Domaine_VF::face_numero_bord(int num_face) const
{
  assert(num_face < nb_faces());
  return face_numero_bord_(num_face);
}

/* produit scalaire de deux vecteurs */
inline double Domaine_VF::dot(const double *a, const double *b, const double *ma, const double *mb) const
{
  double res = 0;
  for (int i = 0; i < dimension; i++) res += (a[i] - (ma ? ma[i] : 0)) * (b[i] - (mb ? mb[i] : 0));
  return res;
}

/* produit vectoriel de deux vecteurs (toujours 3D, meme en 2D) */
inline std::array<double, 3> Domaine_VF::cross(int dima, int dimb, const double *a, const double *b, const double *ma, const double *mb) const
{
  std::array<double, 3> va = {{ 0, 0, 0 }}, vb = {{ 0, 0, 0 }}, res;
  for (int i = 0; i < dima; i++) va[i] = a[i] - (ma ? ma[i] : 0);
  for (int i = 0; i < dimb; i++) vb[i] = b[i] - (mb ? mb[i] : 0);
  for (int i = 0; i < 3; i++) res[i] = va[(i + 1) % 3] * vb[(i + 2) % 3] - va[(i + 2) % 3] * vb[(i + 1) % 3];
  return res;
}


#endif /* Domaine_VF_included */
