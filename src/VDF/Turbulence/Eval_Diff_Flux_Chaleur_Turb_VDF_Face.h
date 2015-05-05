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
// File:        Eval_Diff_Flux_Chaleur_Turb_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_Flux_Chaleur_Turb_VDF_Face_included
#define Eval_Diff_Flux_Chaleur_Turb_VDF_Face_included

#include <Eval_Diff_Flux_Chaleur_Turb_VDF.h>
#include <Champ_Fonc.h>
#include <Eval_VDF_Face.h>
#include <Ref_Modele_turbulence_scal_base.h>
#include <Zone_VDF.h>


////////////////////////////////////////////////////////////////////////////////
//
// CLASS: Eval_Diff_Flux_Chaleur_Turb_VDF_Face
//
//////////////////////////////////////////////////////////////////////////////

class Eval_Diff_Flux_Chaleur_Turb_VDF_Face : public Eval_Diff_Flux_Chaleur_Turb_VDF, public Eval_VDF_Face
{

public:
  inline Eval_Diff_Flux_Chaleur_Turb_VDF_Face();
  inline int calculer_fa7_sortie_libre() const ;
  inline int calculer_arete_fluide() const ;
  inline int calculer_arete_paroi() const ;
  inline int calculer_arete_paroi_fluide() const ;
  inline int calculer_arete_symetrie() const ;
  inline int calculer_arete_interne() const ;
  inline int calculer_arete_mixte() const ;
  inline int calculer_arete_periodicite() const;
  inline int calculer_arete_symetrie_paroi() const;
  inline int calculer_arete_symetrie_fluide() const;

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles renvoient le flux calcule

  inline double flux_fa7_sortie_libre(const DoubleTab&, int ,
                                      const Neumann_sortie_libre&, int ) const;
  inline double flux_fa7_elem(const DoubleTab&, int, int, int) const ;
  inline double flux_arete_paroi(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_symetrie(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_interne(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_mixte(const DoubleTab&, int, int, int, int) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int,
                                double&, double&) const;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int,
                                      double&, double&) const;
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int, int,
                                         double&, double&) const ;
  inline double flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int) const ;

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux

  inline void flux_fa7_elem(const DoubleTab&, int, int, int, DoubleVect& flux) const;
  inline void flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&,
                                    int, DoubleVect& flux) const;
  inline void flux_arete_interne(const DoubleTab&, int, int, int,
                                 int, DoubleVect& flux) const ;
  inline void flux_arete_mixte(const DoubleTab&, int, int, int,
                               int, DoubleVect& flux) const ;
  inline void flux_arete_symetrie(const DoubleTab&, int, int, int,
                                  int, DoubleVect& flux) const ;
  inline void flux_arete_paroi(const DoubleTab&, int, int, int,
                               int, DoubleVect& flux) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int,
                                int, DoubleVect&, DoubleVect&) const ;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int,
                                      int, int, DoubleVect&, DoubleVect&) const;
  inline void flux_arete_periodicite(const DoubleTab&, int, int,
                                     int, int, DoubleVect&, DoubleVect& ) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int,
                                         int, int, DoubleVect&, DoubleVect&) const;
  inline void flux_arete_symetrie_paroi(const DoubleTab&, int, int, int,
                                        int, DoubleVect& flux) const ;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  inline void coeffs_fa7_elem(int, int, int, double& aii, double& ajj) const;
  inline void coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&, double& aii, double& ajj ) const;
  inline void coeffs_arete_interne(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_paroi(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_paroi_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_periodicite(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, double& aii1_2,
                                          double& aii3_4, double& ajj1_2) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite dans le cas scalaire.

  inline double secmem_fa7_elem( int, int, int) const;
  inline double secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&, int ) const;
  inline double secmem_arete_interne(int, int, int, int) const;
  inline double secmem_arete_mixte(int, int, int, int) const;
  inline double secmem_arete_symetrie(int, int, int, int) const;
  inline double secmem_arete_paroi(int, int, int, int ) const;
  inline void secmem_arete_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_paroi_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_periodicite(int, int, int, int, double&, double&) const;
  inline void secmem_arete_symetrie_fluide(int, int, int, int, double&, double&) const;
  inline double secmem_arete_symetrie_paroi(int, int, int, int ) const;



  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  inline void coeffs_fa7_elem(int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_interne(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_symetrie(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_paroi_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_periodicite(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_symetrie_fluide(int, int, int, int,
                                           DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite dans le cas vectoriel.

  inline void secmem_fa7_elem(int, int, int, DoubleVect& flux) const;
  inline void secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&, int, DoubleVect& flux) const;
  inline void secmem_arete_interne(int, int, int, int, DoubleVect& flux) const;
  inline void secmem_arete_mixte(int, int, int, int, DoubleVect& flux) const;
  inline void secmem_arete_symetrie(int, int, int, int, DoubleVect& ) const;
  inline void secmem_arete_paroi(int, int, int, int, DoubleVect& ) const;
  inline void secmem_arete_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_paroi_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_periodicite(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline  void secmem_arete_symetrie_paroi(int, int, int, int, DoubleVect& ) const;
  inline void secmem_arete_symetrie_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
private:

  REF(Modele_turbulence_scal_base) le_modele_turbulence;
  DoubleTab Flux_Chaleur_Turb;
};

//
// Fonctions inline de la classe Eval_Diff_Flux_Chaleur_Turb_VDF_Face
//

inline Eval_Diff_Flux_Chaleur_Turb_VDF_Face::Eval_Diff_Flux_Chaleur_Turb_VDF_Face()
  : Eval_Diff_Flux_Chaleur_Turb_VDF() {}


//// calculer_fa7_sortie_libre
//

inline int Eval_Diff_Flux_Chaleur_Turb_VDF_Face::calculer_fa7_sortie_libre() const
{
  return 1;
}

//// calculer_arete_fluide
//

inline int Eval_Diff_Flux_Chaleur_Turb_VDF_Face::calculer_arete_fluide() const
{
  return 1;
}


//// calculer_arete_paroi
//

inline int Eval_Diff_Flux_Chaleur_Turb_VDF_Face::calculer_arete_paroi() const
{
  return 0;
}


//// calculer_arete_paroi_fluide
//

inline int Eval_Diff_Flux_Chaleur_Turb_VDF_Face::calculer_arete_paroi_fluide() const
{
  return 1;
}

//// calculer_arete_periodicite
//

inline int Eval_Diff_Flux_Chaleur_Turb_VDF_Face::calculer_arete_periodicite() const
{
  return 1;
}


//// calculer_arete_symetrie
//

inline int Eval_Diff_Flux_Chaleur_Turb_VDF_Face::calculer_arete_symetrie() const
{
  return 0;
}

//// calculer_arete_interne
//

inline int Eval_Diff_Flux_Chaleur_Turb_VDF_Face::calculer_arete_interne() const
{
  return 1;
}

//// calculer_arete_mixte
//

inline int Eval_Diff_Flux_Chaleur_Turb_VDF_Face::calculer_arete_mixte() const
{
  return 0;
}

//// calculer_arete_symetrie_paroi
//

inline int Eval_Diff_Flux_Chaleur_Turb_VDF_Face::calculer_arete_symetrie_paroi() const
{
  return 1;
}

//// calculer_arete_symetrie_fluide
//

inline int Eval_Diff_Flux_Chaleur_Turb_VDF_Face::calculer_arete_symetrie_fluide() const
{
  return 1;
}

//////////////////////////////////////////////////////////////////
// Fonctions de calcul des flux pour une inconnue scalaire
/////////////////////////////////////////////////////////////////

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_fa7_sortie_libre(const DoubleTab&, int face,
                                                                          const Neumann_sortie_libre&, int ) const
{
  double flux=0;
  return flux;
}

//// coeffs_fa7_sortie_libre
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&,
                                                                          double& aii, double& ajj ) const
{
  aii = ajj = 0;
}

//// secmem_fa7_sortie_libre
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&,
                                                                            int ) const
{
  return 0;
}

//// flux_arete_interne
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_interne(const DoubleTab& inco, int fac1,
                                                                       int fac2, int fac3, int fac4) const
{
  double flux;
  int ori1 = orientation(fac1);
  int ori3 = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int elem3 = elem_(fac4,0);
  int elem4 = elem_(fac4,1);

  double visc_turb = 0.25*(dv_diffusivite_turbulente(elem1) + dv_diffusivite_turbulente(elem2)
                           +dv_diffusivite_turbulente(elem3) + dv_diffusivite_turbulente(elem4));
  double dist34 = dist_face(fac3,fac4,ori1);
  double dist12 = dist_face(fac1,fac2,ori3);
  double tau = (inco[fac4]-inco[fac3])/(dist34*Prdt_UTETA);
  double tau_tr = (inco[fac2]-inco[fac1])/(dist12*Prdt_UTETA);
  double reyn = (tau + tau_tr)*visc_turb;

  flux = reyn*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
  return flux;
}

//// coeffs_arete_interne
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_interne(int fac1, int fac2, int fac3, int fac4,
                                                                       double& aii, double& ajj) const
{

  int ori1 = orientation(fac1);
  int ori3 = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int elem3 = elem_(fac4,0);
  int elem4 = elem_(fac4,1);

  double visc_turb = 0.25*(dv_diffusivite_turbulente(elem1) + dv_diffusivite_turbulente(elem2)
                           +dv_diffusivite_turbulente(elem3) + dv_diffusivite_turbulente(elem4));

  double dist34 = dist_face(fac3,fac4,ori1);
  double dist12 = dist_face(fac1,fac2,ori3);
  double tau = 1/(dist34*Prdt_UTETA);
  double tau_tr = 1/(dist12*Prdt_UTETA);
  double reyn = (tau + tau_tr)*visc_turb;

  aii = ajj = reyn*(surface(fac1)+surface(fac2))
              *(porosite(fac1)+porosite(fac2));
}

//// secmem_arete_interne
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_interne(int fac1, int fac2, int fac3, int fac4) const
{
  return 0;
}

//// flux_arete_mixte
//

// Sur les aretes mixtes les termes croises du tenseur de Reynolds
// sont nuls: il ne reste donc que la diffusion laminaire

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_mixte(const DoubleTab& inco, int fac1,
                                                                     int fac2, int fac3, int fac4) const
{
  return 0;
}

//// coeffs_arete_mixte
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_mixte(int fac1, int fac2, int fac3, int fac4,
                                                                     double& aii, double& ajj) const
{
  ;
}

//// secmem_arete_mixte
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_mixte(int fac1, int fac2, int fac3, int fac4) const
{
  return 0;
}

//// flux_arete_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_fluide(const DoubleTab& inco, int fac1,
                                                                    int fac2, int fac3, int signe,
                                                                    double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);
  double vit_imp = 0.5*(inconnue->val_imp_face_bord(rang1,ori)+
                        inconnue->val_imp_face_bord(rang2,ori));

  double dist = dist_norm_bord(fac1);
  double dist12 = dist_face(fac1,fac2,ori);
  double tau = signe * (vit_imp - inco[fac3])/(dist*Prdt_UTETA);
  double tau_tr = (inco[fac2] - inco[fac1])/(dist12*Prdt_UTETA);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;

  flux3 = reyn*surf*poros;
  flux1_2 = (reyn)*surface(fac3)*porosite(fac3);

}

//// coeffs_arete_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                                      double& aii1_2, double& aii3_4,
                                                                      double& ajj1_2) const
{
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);

  double dist = dist_norm_bord(fac1);
  double dist12 = dist_face(fac1,fac2,ori);
  double tau = signe/(dist*Prdt_UTETA);
  double tau_tr = 1/(dist12*Prdt_UTETA);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;
  // Calcul de aii3_4
  aii3_4 = reyn*surf*poros;

  // Calcul de aii1_2 et ajj1_2
  aii1_2 = ajj1_2  = (reyn)*surface(fac3)*porosite(fac3);
}

//// secmem_arete_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                                      double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);
  double vit_imp = 0.5*(inconnue->val_imp_face_bord(rang1,ori)+
                        inconnue->val_imp_face_bord(rang2,ori));

  double dist = dist_norm_bord(fac1);
  double tau = signe*vit_imp/(dist*Prdt_UTETA);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = tau*visc_turb;

  flux3 = reyn*surf*poros;
  flux1_2 = 0;
}


//// flux_arete_paroi
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_paroi(const DoubleTab& inco, int fac1,
                                                                     int fac2, int fac3, int signe ) const
{
  return 0;
}

//// coeffs_arete_paroi
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_paroi(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4, double& ajj1_2) const
{
  ;
}


//// secmem_arete_paroi
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_paroi(int fac1, int fac2, int fac3, int signe) const
{
  return 0;
}

//// flux_arete_paroi_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_paroi_fluide(const DoubleTab& inco , int fac1,
                                                                          int fac2 , int fac3, int signe,
                                                                          double& flux3, double& flux1_2 ) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);

  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
  double vit_imp;
  if (est_egal(inco[fac1],0)) // fac1 est la face de paroi
    vit_imp = inconnue->val_imp_face_bord(rang2,ori);
  else  // fac2 est la face de paroi
    vit_imp = inconnue->val_imp_face_bord(rang1,ori);

  double dist = dist_norm_bord(fac1);
  double dist12 = dist_face(fac1,fac2,ori);
  double tau = signe * (vit_imp - inco[fac3])/(dist*Prdt_UTETA);
  double tau_tr = (inco[fac2] - inco[fac1])/(dist12*Prdt_UTETA);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;
  flux3 = reyn*surf*poros;
  flux1_2 = reyn*surface(fac3)*porosite(fac3);
}

//// coeffs_arete_paroi_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                                            double& aii1_2, double& aii3_4,
                                                                            double& ajj1_2) const
{
  double dist;
  int ori= orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));

  //Calcul des aii et ajj 3_4
  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi

  dist = dist_norm_bord(fac1);
  double dist12 = dist_face(fac1,fac2,ori);
  double tau = signe/(dist*Prdt_UTETA);
  double tau_tr = 1/(dist12*Prdt_UTETA);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;

  aii3_4 = reyn*surf*poros;
  aii1_2 = ajj1_2 =reyn*surface(fac3)*porosite(fac3);
}


//// secmem_arete_paroi_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                                            double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);

  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi

  double vit_imp;
  if (est_egal(inconnue->valeurs()[fac1],0)) // fac1 est la face de paroi
    vit_imp = inconnue->val_imp_face_bord(rang2,ori);
  else  // fac2 est la face de paroi
    vit_imp = inconnue->val_imp_face_bord(rang1,ori);

  double dist = dist_norm_bord(fac1);
  double tau = signe*vit_imp/(dist*Prdt_UTETA);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = tau*visc_turb;

  flux3 = reyn*surf*poros;
  flux1_2 = 0;
}

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_periodicite(const DoubleTab& inco,
                                                                         int fac1, int fac2, int fac3, int fac4,
                                                                         double& flux3_4, double& flux1_2) const
{
  ;
}

// coeffs_arete_periodicite
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_periodicite(int fac1, int fac2, int fac3, int fac4,
                                                                           double& aii, double& ajj) const
{
  ;
}

//// secmem_arete_periodicite
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_periodicite(int fac1, int fac2, int fac3, int fac4,
                                                                           double& flux3_4, double& flux1_2) const
{
  ;
}
//// flux_arete_symetrie
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_symetrie(const DoubleTab&, int,
                                                                        int, int, int) const
{
  return 0;
}

//// coeffs_arete_symetrie
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_symetrie(int, int, int, int, double&, double&, double&) const
{
  ;
}

//// secmem_arete_symetrie
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_symetrie(int, int, int, int) const
{
  return 0;
}

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_fa7_elem(const DoubleTab& inco, int elem,
                                                                  int fac1, int fac2) const
{
  return 0;
}

//// coeffs_fa7_elem
//

inline void  Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_fa7_elem(int elem ,int fac1, int fac2, double& aii, double& ajj) const
{
  ;
}

//// secmem_fa7_elem
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_fa7_elem(int elem ,int fac1, int fac2) const
{
  return 0;
}

//// flux_arete_symetrie_fluide
//
inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_symetrie_fluide(const DoubleTab& inco, int fac1,
                                                                             int fac2, int fac3, int signe,
                                                                             double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);
  double vit_imp = 0.5*(inconnue->val_imp_face_bord(rang1,ori)+
                        inconnue->val_imp_face_bord(rang2,ori));

  double dist = dist_norm_bord(fac1);
  double dist12 = dist_face(fac1,fac2,ori);
  double tau = signe * (vit_imp - inco[fac3])/(dist*Prdt_UTETA);
  double tau_tr = (inco[fac2] - inco[fac1])/(dist12*Prdt_UTETA);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;

  flux3 = reyn*surf*poros;
  flux1_2 = (reyn)*surface(fac3)*porosite(fac3);

}

//// coeffs_arete_symetrie_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_symetrie_fluide(int fac1, int fac2, int fac3,
                                                                               int signe,
                                                                               double& aii1_2, double& aii3_4,
                                                                               double& ajj1_2) const
{
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);

  double dist = dist_norm_bord(fac1);
  double dist12 = dist_face(fac1,fac2,ori);
  double tau = signe/(dist*Prdt_UTETA);
  double tau_tr = 1/(dist12*Prdt_UTETA);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;
  // Calcul de aii3_4
  aii3_4 = reyn*surf*poros;

  // Calcul de aii1_2 et ajj1_2
  aii1_2 = ajj1_2  = (reyn)*surface(fac3)*porosite(fac3);
}

//// secmem_arete_symetrie_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_symetrie_fluide(int fac1, int fac2, int fac3, int signe,
                                                                               double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);
  double vit_imp = 0.5*(inconnue->val_imp_face_bord(rang1,ori)+
                        inconnue->val_imp_face_bord(rang2,ori));

  double dist = dist_norm_bord(fac1);
  double tau = signe*vit_imp/(dist*Prdt_UTETA);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = tau*visc_turb;

  flux3 = reyn*surf*poros;
  flux1_2 = 0;
}

//// flux_arete_symetrie_paroi
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_symetrie_paroi(const DoubleTab& inco, int fac1,
                                                                              int fac2, int fac3, int signe ) const
{
  return 0;
}

//// coeffs_arete_symetrie_paroi
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_symetrie_paroi(int fac1, int fac2, int fac3,
                                                                              int signe, double& aii1_2,
                                                                              double& aii3_4, double& ajj1_2) const
{
  ;
}


//// secmem_arete_symetrie_paroi
//

inline double Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_symetrie_paroi(int fac1, int fac2, int fac3, int signe) const
{
  return 0;
}

// Fonctions de calcul des flux pour une inconnue vectorielle

//// flux_fa7_sortie_libre
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_fa7_sortie_libre(const DoubleTab&, int,
                                                                        const Neumann_sortie_libre&,
                                                                        int, DoubleVect&) const
{
  ;
}

////coeffs_fa7_sortie_libre
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&,
                                                                          DoubleVect&, DoubleVect& ) const
{
  // A Coder!
}

////secmem_fa7_sortie_libre
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&,
                                                                          int, DoubleVect&  ) const
{
  // A Coder!
}


//// flux_arete_interne
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_interne(const DoubleTab&, int, int,
                                                                     int, int, DoubleVect& ) const
{
  // EMPTY //
  ;
}

//// coeffs_arete_interne
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_interne(int , int , int , int ,
                                                                       DoubleVect&, DoubleVect& ) const
{
  // A coder!
}

//// secmem_arete_interne
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_interne(int ,
                                                                       int , int , int ,
                                                                       DoubleVect& ) const
{
  // A coder!
}


//// flux_arete_mixte
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_mixte(const DoubleTab&, int, int,
                                                                   int, int, DoubleVect& ) const
{
  // EMPTY //
  ;
}

//// coeffs_arete_mixte
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_mixte(int , int , int , int ,
                                                                     DoubleVect& , DoubleVect& ) const
{
  // A coder!
}

//// secmem_arete_mixte
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_mixte(int, int , int , int ,
                                                                     DoubleVect& ) const
{
  // A coder!
}


//// flux_arete_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_fluide(const DoubleTab&, int, int,
                                                                    int, int, DoubleVect& , DoubleVect& ) const
{
  // EMPTY //
  ;
}

//// coeffs_arete_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_fluide(int ,int ,
                                                                      int , int , DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}

//// secmem_arete_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_fluide(int ,int ,int , int ,
                                                                      DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}


//// flux_arete_paroi
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_paroi(const DoubleTab&, int, int,
                                                                   int, int, DoubleVect& ) const
{
  // EMPTY //
  ;
}

//// coeffs_arete_paroi
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_paroi(int , int , int , int ,
                                                                     DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A coder!
}

//// secmem_arete_paroi
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_paroi(int, int , int , int ,
                                                                     DoubleVect& ) const
{
  // A coder!
}


//// flux_arete_paroi_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_paroi_fluide(const DoubleTab&, int, int,
                                                                          int, int, DoubleVect& ,
                                                                          DoubleVect& ) const
{
  // EMPTY //
  ;
}

//// coeffs_arete_paroi_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_paroi_fluide(int, int , int , int ,
                                                                            DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}


//// secmem_arete_paroi_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_paroi_fluide(int, int , int , int ,
                                                                            DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}


//// flux_arete_periodicite
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_periodicite(const DoubleTab& , int ,
                                                                         int , int , int ,
                                                                         DoubleVect& , DoubleVect& ) const
{
  ;
}

//// coeffs_arete_periodicite
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_periodicite(int , int , int , int ,
                                                                           DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// secmem_arete_periodicite
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_periodicite(int , int , int , int ,
                                                                           DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}


//// flux_arete_symetrie
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_symetrie(const DoubleTab&, int, int,
                                                                      int, int, DoubleVect& ) const
{
  ;
}

//// coeffs_arete_symetrie
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_symetrie(int, int, int, int, DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}

//// secmem_arete_symetrie
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_symetrie(int, int,
                                                                        int, int, DoubleVect& ) const
{
  // A Coder!
}


//// flux_fa7_elem
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_fa7_elem(const DoubleTab&, int, int,
                                                                int, DoubleVect& ) const
{
  // EMPTY //
  ;
}

//// coeffs_fa7_elem
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_fa7_elem(int , int ,int , DoubleVect&, DoubleVect& ) const
{
  // A Coder!
}

//// secmem_fa7_elem
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_fa7_elem(int , int ,
                                                                  int , DoubleVect& ) const
{
  // A Coder!
}

//// flux_arete_symetrie_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_symetrie_fluide(const DoubleTab&, int, int,
                                                                             int, int, DoubleVect& , DoubleVect& ) const
{
  // EMPTY //
  ;
}

//// coeffs_arete_symetrie_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_symetrie_fluide(int ,int ,
                                                                               int , int ,
                                                                               DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}

//// secmem_arete_symetrie_fluide
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_symetrie_fluide(int ,int ,int , int ,
                                                                               DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}


//// flux_arete_symetrie_paroi
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::flux_arete_symetrie_paroi(const DoubleTab&, int, int,
                                                                            int, int, DoubleVect& ) const
{
  // EMPTY //
  ;
}

//// coeffs_arete_symetrie_paroi
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::coeffs_arete_symetrie_paroi(int , int , int , int ,
                                                                              DoubleVect&, DoubleVect&,
                                                                              DoubleVect&) const
{
  // A coder!
}

//// secmem_arete_symetrie_paroi
//

inline void Eval_Diff_Flux_Chaleur_Turb_VDF_Face::secmem_arete_symetrie_paroi(int, int , int , int ,
                                                                              DoubleVect& ) const
{
  // A coder!
}



#endif

