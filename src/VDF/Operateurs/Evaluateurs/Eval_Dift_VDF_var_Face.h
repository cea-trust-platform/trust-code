/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Eval_Dift_VDF_var_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Dift_VDF_var_Face_included
#define Eval_Dift_VDF_var_Face_included

#include <Eval_Dift_VDF_var.h>
#include <Eval_VDF_Face.h>
#include <Ref_Turbulence_paroi_base.h>
#include <Mod_turb_hyd_base.h>

//
// .DESCRIPTION class Eval_Dift_VDF_var_Face
//
// Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
// Le champ diffuse est un Champ_Face
// Le champ de diffusivite n'est pas constant.

//
// .SECTION voir aussi Eval_Dift_VDF_var

class Eval_Dift_VDF_var_Face : public Eval_Dift_VDF_var, public Eval_VDF_Face
{

public:

  inline Eval_Dift_VDF_var_Face();
  void associer_modele_turbulence(const Mod_turb_hyd_base& );
  void mettre_a_jour( );

  inline int calculer_arete_fluide() const ;
  inline int calculer_arete_paroi() const ;
  inline int calculer_arete_paroi_fluide() const ;
  inline int calculer_arete_symetrie() const ;
  inline int calculer_arete_interne() const ;
  inline int calculer_arete_mixte() const ;
  inline int calculer_fa7_sortie_libre() const ;
  inline int calculer_arete_periodicite() const;
  inline int calculer_arete_symetrie_paroi() const;
  inline int calculer_arete_symetrie_fluide() const;

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles renvoient le flux calcule

  inline double flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const;
  inline double flux_fa7_elem(const DoubleTab&, int, int, int) const ;
  inline double flux_arete_interne(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_mixte(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_symetrie(const DoubleTab&, int, int, int, int) const ;
  inline double flux_arete_paroi(const DoubleTab&, int, int, int, int) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int,
                                int, int, double& , double& ) const ;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int,
                                      int, int, double& , double& ) const ;
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int,
                                     double&, double&) const ;
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
                                      int, int, DoubleVect& , DoubleVect&) const;
  inline void flux_arete_periodicite(const DoubleTab&, int, int,
                                     int, int, DoubleVect&, DoubleVect& ) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int,
                                         int, int, DoubleVect&, DoubleVect&) const;
  inline void flux_arete_symetrie_paroi(const DoubleTab&, int, int, int,
                                        int, DoubleVect& flux) const ;

  inline double tau_tan(int face,int k) const;
  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  inline void coeffs_fa7_elem(int, int, int, double& aii, double& ajj) const;
  inline void coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&, double& aii, double& ajj) const;
  inline void coeffs_arete_interne(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_paroi(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_paroi_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_periodicite(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie_fluide(int, int, int, int,
                                           double& aii1_2, double& aii3_4, double& ajj1_2) const;
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
  inline void secmem_arete_symetrie_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_symetrie_paroi(int, int, int, int, DoubleVect& ) const;

private:

  REF(Mod_turb_hyd_base) le_modele_turbulence;
  REF(Turbulence_paroi_base) loipar;
  DoubleTab tau_tan_;
};

//
// Fonctions inline de la classe Eval_Dift_VDF_var_Face
//
inline double Eval_Dift_VDF_var_Face::tau_tan(int face, int k) const
{
  int nb_faces = la_zone.valeur().nb_faces();
  const ArrOfInt& ind_faces_virt_bord = la_zone.valeur().ind_faces_virt_bord();
  int f;
  if(face>=tau_tan_.dimension(0))
    f = ind_faces_virt_bord[face-nb_faces];
  else
    f=face;
  if(f>=tau_tan_.dimension_tot(0))
    {
      Cerr << "Erreur dans tau_tan " << finl;
      Cerr << "dimension : " << tau_tan_.dimension(0) << finl;
      Cerr << "dimension_tot : " << tau_tan_.dimension_tot(0) << finl;
      Cerr << "face : " << face << finl;
      Process::exit();
    }
  return tau_tan_(f,k);
}

inline Eval_Dift_VDF_var_Face::Eval_Dift_VDF_var_Face()
{}


//// calculer_arete_fluide
//

inline int Eval_Dift_VDF_var_Face::calculer_arete_fluide() const
{
  return 1;
}


//// calculer_arete_paroi
//

inline int Eval_Dift_VDF_var_Face::calculer_arete_paroi() const
{
  return 1;
}


//// calculer_arete_paroi_fluide
//

inline int Eval_Dift_VDF_var_Face::calculer_arete_paroi_fluide() const
{
  return 0;
}

//// calculer_arete_periodicite
//

inline int Eval_Dift_VDF_var_Face::calculer_arete_periodicite() const
{
  return 1;
}

//// calculer_arete_symetrie
//

inline int Eval_Dift_VDF_var_Face::calculer_arete_symetrie() const
{
  return 0;
}

//// calculer_arete_interne
//

inline int Eval_Dift_VDF_var_Face::calculer_arete_interne() const
{
  return 1;
}

////  calculer_arete_mixte
//

inline int Eval_Dift_VDF_var_Face::calculer_arete_mixte() const
{
  return 1;
}

////   calculer_fa7_sortie_libre
//

inline int Eval_Dift_VDF_var_Face::calculer_fa7_sortie_libre() const
{
  return 1;
}

//// calculer_arete_symetrie_paroi
//

inline int Eval_Dift_VDF_var_Face::calculer_arete_symetrie_paroi() const
{
  return 1;
}

//// calculer_arete_symetrie_fluide
//

inline int Eval_Dift_VDF_var_Face::calculer_arete_symetrie_fluide() const
{
  return 1;
}

// Fonctions de calcul des flux pour une inconnue scalaire

//// flux_fa7_sortie_libre
//

inline double Eval_Dift_VDF_var_Face::flux_fa7_sortie_libre(const DoubleTab&, int face,
                                                            const Neumann_sortie_libre&, int ) const
{
  double flux=0;
  return flux;
}

//// coeffs_fa7_sortie_libre
//

inline void Eval_Dift_VDF_var_Face::coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&,
                                                            double& aii, double& ajj ) const
{
  aii = ajj = 0;
}

//// secmem_fa7_sortie_libre
//

inline double Eval_Dift_VDF_var_Face::secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&,
                                                              int ) const
{
  return 0;
}


//// flux_arete_interne
//

inline double Eval_Dift_VDF_var_Face::flux_arete_interne(const DoubleTab& inco, int fac1,
                                                         int fac2, int fac3, int fac4) const
{
  double flux;
  int ori1 = orientation(fac1);
  int ori3 = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int elem3 = elem_(fac4,0);
  int elem4 = elem_(fac4,1);

  double visc_lam = 0.25*(dv_diffusivite(elem1) + dv_diffusivite(elem2)
                          +dv_diffusivite(elem3) + dv_diffusivite(elem4));
  double visc_turb = 0.25*(dv_diffusivite_turbulente(elem1) + dv_diffusivite_turbulente(elem2)
                           +dv_diffusivite_turbulente(elem3) + dv_diffusivite_turbulente(elem4));

  double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori1);
  double tau_tr = (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori3);
  double reyn = (tau + tau_tr)*visc_turb;

  flux = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))
         *(porosite(fac1)+porosite(fac2));
  return flux;
}

//// coeffs_arete_interne
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_interne(int fac1, int fac2, int fac3, int fac4,
                                                         double& aii, double& ajj) const
{

  int ori1 = orientation(fac1);
  int ori3 = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int elem3 = elem_(fac4,0);
  int elem4 = elem_(fac4,1);

  double visc_lam = 0.25*(dv_diffusivite(elem1) + dv_diffusivite(elem2)
                          +dv_diffusivite(elem3) + dv_diffusivite(elem4));
  double visc_turb = 0.25*(dv_diffusivite_turbulente(elem1) + dv_diffusivite_turbulente(elem2)
                           +dv_diffusivite_turbulente(elem3) + dv_diffusivite_turbulente(elem4));

  double tau = 1/dist_face(fac3,fac4,ori1);
  double tau_tr = 1/dist_face(fac1,fac2,ori3);
  double reyn = (tau + tau_tr)*visc_turb;

  aii = ajj = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))
              *(porosite(fac1)+porosite(fac2));
}

//// secmem_arete_interne
//

inline double Eval_Dift_VDF_var_Face::secmem_arete_interne(int fac1, int fac2, int fac3, int fac4) const
{
  return 0;
}


//// flux_arete_mixte
//

// Sur les aretes mixtes les termes croises du tenseur de Reynolds
// sont nuls: il ne reste donc que la diffusion laminaire

inline double Eval_Dift_VDF_var_Face::flux_arete_mixte(const DoubleTab& inco, int fac1,
                                                       int fac2, int fac3, int fac4) const
{
  double flux=0;
  if (inco[fac4]*inco[fac3] != 0)
    {
      int ori1 = orientation(fac1);
      int ori3 = orientation(fac3);
      int elem[4];
      elem[0] = elem_(fac3,0);
      elem[1] = elem_(fac3,1);
      elem[2] = elem_(fac4,0);
      elem[3] = elem_(fac4,1);

      double visc_lam=0;
      double visc_turb=0;
      for (int i=0; i<4; i++)
        if (elem[i] != -1)
          {
            visc_lam += dv_diffusivite(elem[i]);
            visc_turb += dv_diffusivite_turbulente(elem[i]);
          }
      visc_lam/=3.0;
      visc_turb/=3.0;

      double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori1);
      double tau_tr = (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori3);
      double reyn = (tau + tau_tr)*visc_turb;
      flux = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))
             *(porosite(fac1)+porosite(fac2));
    }
  return flux;
}

//// coeffs_arete_mixte
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_mixte(int fac1, int fac2, int fac3, int fac4,
                                                       double& aii, double& ajj) const
{

  if (inconnue->valeurs()[fac4]*inconnue->valeurs()[fac3] != 0)
    {
      int ori1 = orientation(fac1);
      int ori3 = orientation(fac3);
      int elem[4];
      elem[0] = elem_(fac3,0);
      elem[1] = elem_(fac3,1);
      elem[2] = elem_(fac4,0);
      elem[3] = elem_(fac4,1);

      double visc_lam=0;
      double visc_turb=0;
      for (int i=0; i<4; i++)
        if (elem[i] != -1)
          {
            visc_lam += dv_diffusivite(elem[i]);
            visc_turb += dv_diffusivite_turbulente(elem[i]);
          }
      visc_lam/=3.0;
      visc_turb/=3.0;

      double tau = 1/dist_face(fac3,fac4,ori1);
      double tau_tr = 1/dist_face(fac1,fac2,ori3);
      double reyn = (tau + tau_tr)*visc_turb;

      aii = ajj = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))
                  *(porosite(fac1)+porosite(fac2));
    }
  else
    {
      aii=ajj=0;
    }
}

//// secmem_arete_mixte
//

inline double Eval_Dift_VDF_var_Face::secmem_arete_mixte(int fac1, int fac2, int fac3, int fac4) const
{
  return 0;
}


//// flux_arete_fluide
//

inline void Eval_Dift_VDF_var_Face::flux_arete_fluide(const DoubleTab& inco, int fac1,
                                                      int fac2, int fac3, int signe,
                                                      double& flux3,double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  double dist = dist_norm_bord(fac1);
  double tau = signe * (vit_imp - inco[fac3])/dist;
  double tau_tr = (inco[fac2] - inco[fac1])/dist_face(fac1,fac2,ori);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;
  double coef = ((tau+tau_tr)*visc_lam + reyn);
  flux3 = coef*surf*poros;
  flux1_2 = ((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);

}

//// coeffs_arete_fluide
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                        double& aii1_2, double& aii3_4,
                                                        double& ajj1_2) const
{
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);

  double dist = dist_norm_bord(fac1);
  double tau = signe/dist;
  double tau_tr = 1/dist_face(fac1,fac2,ori);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;
  double coef = ((tau+tau_tr)*visc_lam + reyn);

  // Calcul de aii3_4
  aii3_4 = coef*surf*poros;

  // Calcul de aii1_2 et ajj1_2
  aii1_2 = ajj1_2  = ((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);
}

//// secmem_arete_fluide
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                        double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  double dist = dist_norm_bord(fac1);
  double tau = signe*vit_imp/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = tau*visc_turb;
  double coef = (tau*visc_lam + reyn);

  flux3 = coef*surf*poros;
  flux1_2 = 0;
}


//// flux_arete_paroi
//

inline double Eval_Dift_VDF_var_Face::flux_arete_paroi(const DoubleTab& inco, int fac1,
                                                       int fac2, int fac3, int signe ) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int ori = orientation(fac3);
  double vit = inco(fac3);
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  if ( !le_modele_turbulence.valeur().utiliser_loi_paroi() )
    {
      int elem1 = elem_(fac3,0);
      int elem2 = elem_(fac3,1);
      if (elem1==-1)
        elem1 = elem2;
      else if (elem2==-1)
        elem2 = elem1;
      double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
      double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                              + dv_diffusivite_turbulente(elem2));

      double dist = dist_norm_bord(fac1);
      double tau  = signe*(vit_imp - inco[fac3])/dist;
      double surf = 0.5*(surface(fac1)+surface(fac2));
      flux = tau*surf*(visc_lam+visc_turb);
    }
  else
    {
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      int signe_terme;
      if ( vit < vit_imp )
        signe_terme = -1;
      else
        signe_terme = 1;

      //30/09/2003  YB : influence de signe terme eliminee, signe pris en compte dans la loi de paroi
      signe_terme = 1;

      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
      double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      double coef = tau1+tau2;
      flux = signe_terme*coef;
    }
  return flux;

}

//// coeffs_arete_paroi
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_paroi(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4, double& ajj1_2) const
{
  if ( !le_modele_turbulence.valeur().utiliser_loi_paroi() )
    {
      int elem1 = elem_(fac3,0);
      int elem2 = elem_(fac3,1);
      if (elem1==-1)
        elem1 = elem2;
      else if (elem2==-1)
        elem2 = elem1;
      double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
      double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                              + dv_diffusivite_turbulente(elem2));

      double dist = dist_norm_bord(fac1);
      double surf = 0.5*(surface(fac1)+surface(fac2));
      aii3_4 = signe*surf*(visc_lam+visc_turb)/dist;
      aii1_2 = 0;
      ajj1_2 = 0;
    }
  else
    {
      aii3_4 = 0;
      aii1_2 = 0;
      ajj1_2 = 0;
    }
}


//// secmem_arete_paroi
//

inline double Eval_Dift_VDF_var_Face::secmem_arete_paroi(int fac1, int fac2, int fac3, int signe) const
{
  double flux;
  int ori = orientation(fac3);
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);
  const DoubleTab& inco = inconnue->valeurs();
  double vit = inco(fac3);
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));

  if ( !le_modele_turbulence.valeur().utiliser_loi_paroi() )
    {
      double dist = dist_norm_bord(fac1);
      double tau  = signe*(vit_imp - inco[fac3])/dist;
      double surf = 0.5*(surface(fac1)+surface(fac2));
      flux = tau*surf*(visc_lam+visc_turb);
    }
  else
    {
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      int signe_terme;
      if ( vit < vit_imp )
        signe_terme = -1;
      else
        signe_terme = 1;

      //30/09/2003  YB : influence de signe terme eliminee, signe pris en compte dans la loi de paroi
      signe_terme = 1;

      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
      double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      double coef = tau1+tau2;
      flux = signe_terme*coef;
    }
  return flux;
}

//// flux_arete_paroi_fluide
//

inline void Eval_Dift_VDF_var_Face::flux_arete_paroi_fluide(const DoubleTab& inco, int fac1,
                                                            int fac2 , int fac3, int signe,
                                                            double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);

  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi
  double vit_imp;
  if (est_egal(inco[fac1],0)) // fac1 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl);
  else  // fac2 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl);

  double dist = dist_norm_bord(fac1);
  double tau = signe * (vit_imp - inco[fac3])/dist;
  double tau_tr = (inco[fac2] - inco[fac1])/dist_face(fac1,fac2,ori);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;
  double coef = ((tau+tau_tr)*visc_lam + reyn);
  flux3 = coef*surf*poros;
  flux1_2 = ((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);

}

//// coeffs_arete_paroi_fluide
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                              double& aii1_2, double& aii3_4,
                                                              double& ajj1_2) const
{
  double dist;
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int ori= orientation(fac3);
  double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));

  //Calcul des aii et ajj 3_4
  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi

  dist = dist_norm_bord(fac1);
  double tau = signe/dist;
  double tau_tr = 1/dist_face(fac1,fac2,ori);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;
  double coef = ((tau+tau_tr)*visc_lam + reyn);

  aii3_4 = coef*surf*poros;
  aii1_2 = ajj1_2 =((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);
}


//// secmem_arete_paroi_fluide
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                              double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam =  0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);

  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi

  double vit_imp;
  if (est_egal(inconnue->valeurs()[fac1],0)) // fac1 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl);
  else  // fac2 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl);

  double dist = dist_norm_bord(fac1);
  double tau = signe*vit_imp/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = tau*visc_turb;
  double coef = (tau*visc_lam + reyn);

  flux3 = coef*surf*poros;
  flux1_2 = 0;
}

//// flux_arete_periodicite
//

inline void Eval_Dift_VDF_var_Face::flux_arete_periodicite(const DoubleTab& inco,
                                                           int fac1, int fac2, int fac3, int fac4,
                                                           double& flux3_4, double& flux1_2) const
{
  double flux;
  int ori1 = orientation(fac1);
  int ori3 = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int elem3 = elem_(fac4,0);
  int elem4 = elem_(fac4,1);
  double dist3_4 = dist_face_period(fac3,fac4,ori1);
  double dist1_2 = dist_face_period(fac1,fac2,ori3);

  double visc_lam = 0.25*(dv_diffusivite(elem1) + dv_diffusivite(elem2)
                          +dv_diffusivite(elem3) + dv_diffusivite(elem4));
  double visc_turb = 0.25*(dv_diffusivite_turbulente(elem1) + dv_diffusivite_turbulente(elem2)
                           +dv_diffusivite_turbulente(elem3) + dv_diffusivite_turbulente(elem4));

  double tau = (inco[fac4]-inco[fac3])/dist3_4;
  double tau_tr = (inco[fac2]-inco[fac1])/dist1_2;
  double reyn = (tau + tau_tr)*visc_turb;

  flux = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))
         *(porosite(fac1)+porosite(fac2));

  flux3_4 = flux;

  flux = 0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac3)+surface(fac4))
         *(porosite(fac3)+porosite(fac4));

  flux1_2 = flux;
}

//// coeffs_arete_periodicite
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_periodicite(int fac1, int fac2, int fac3, int fac4,
                                                             double& aii, double& ajj) const
{
  int ori1 = orientation(fac1);
  int ori3 = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  int elem3 = elem_(fac4,0);
  int elem4 = elem_(fac4,1);
  double dist3_4 = dist_face_period(fac3,fac4,ori1);
  double dist1_2 = dist_face_period(fac1,fac2,ori3);

  double visc_lam = 0.25*(dv_diffusivite(elem1) + dv_diffusivite(elem2)
                          +dv_diffusivite(elem3) + dv_diffusivite(elem4));
  double visc_turb = 0.25*(dv_diffusivite_turbulente(elem1) + dv_diffusivite_turbulente(elem2)
                           +dv_diffusivite_turbulente(elem3) + dv_diffusivite_turbulente(elem4));

  double tau = 1/dist3_4;
  double tau_tr = 1/dist1_2;
  double reyn = (tau + tau_tr)*visc_turb;

  aii = ajj =0.25*(reyn + visc_lam*(tau+tau_tr))*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
}


//// secmem_arete_periodicite
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_periodicite(int fac1, int fac2, int fac3, int fac4,
                                                             double& flux3_4, double& flux1_2) const
{
  ;
}

//// flux_arete_symetrie
//

inline double Eval_Dift_VDF_var_Face::flux_arete_symetrie(const DoubleTab&, int,
                                                          int, int, int) const
{
  return 0;
}

//// coeffs_arete_symetrie
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_symetrie(int, int, int, int, double&, double&, double&) const
{
  ;
}

//// secmem_arete_symetrie
//

inline double Eval_Dift_VDF_var_Face::secmem_arete_symetrie(int, int, int, int) const
{
  return 0;
}


//// flux_fa7_elem
//

inline double Eval_Dift_VDF_var_Face::flux_fa7_elem(const DoubleTab& inco, int elem,
                                                    int fac1, int fac2) const
{
  double flux;
  int ori=orientation(fac1);
  double tau = (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori);
  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  double reyn = - 2.*dv_diffusivite_turbulente(elem)*tau ;
  flux = (-reyn + 2.*dv_diffusivite(elem)*tau) * surf ;
  return flux;
}


//// coeffs_fa7_elem
//

inline void Eval_Dift_VDF_var_Face::coeffs_fa7_elem(int elem ,int fac1, int fac2, double& aii, double& ajj) const
{
  int ori=orientation(fac1);
  double tau = 1/dist_face(fac1,fac2,ori);
  double surf = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2));
  double reyn = - 2.*dv_diffusivite_turbulente(elem)*tau ;
  aii = ajj =(-reyn +2.*dv_diffusivite(elem)*tau) * surf;
}


//// secmem_fa7_elem
//

inline double Eval_Dift_VDF_var_Face::secmem_fa7_elem(int elem ,int fac1, int fac2) const
{
  return 0;
}

//// flux_arete_symetrie_fluide
//

inline void Eval_Dift_VDF_var_Face::flux_arete_symetrie_fluide(const DoubleTab& inco, int fac1,
                                                               int fac2, int fac3, int signe,
                                                               double& flux3,double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl));

  double dist = dist_norm_bord(fac1);
  double tau = signe * (vit_imp - inco[fac3])/dist;
  double tau_tr = (inco[fac2] - inco[fac1])/dist_face(fac1,fac2,ori);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;
  double coef = ((tau+tau_tr)*visc_lam + reyn);
  flux3 = coef*surf*poros;
  flux1_2 = ((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);
}

//// coeffs_arete_symetrie_fluide
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_symetrie_fluide(int fac1, int fac2, int fac3, int signe,
                                                                 double& aii1_2, double& aii3_4,
                                                                 double& ajj1_2) const
{
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);

  double dist = dist_norm_bord(fac1);
  double tau = signe/dist;
  double tau_tr = 1/dist_face(fac1,fac2,ori);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = (tau + tau_tr)*visc_turb;
  double coef = ((tau+tau_tr)*visc_lam + reyn);

  // Calcul de aii3_4
  aii3_4 = coef*surf*poros;

  // Calcul de aii1_2 et ajj1_2
  aii1_2 = ajj1_2  = ((tau+tau_tr)*visc_lam + reyn)*surface(fac3)*porosite(fac3);
}

//// secmem_arete_symetrie_fluide
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_symetrie_fluide(int fac1, int fac2, int fac3, int signe,
                                                                 double& flux3, double& flux1_2) const
{
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam = 0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  int ori= orientation(fac3);
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));

  double dist = dist_norm_bord(fac1);
  double tau = signe*vit_imp/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double reyn = tau*visc_turb;
  double coef = (tau*visc_lam + reyn);

  flux3 = coef*surf*poros;
  flux1_2 = 0;
}

//// flux_arete_symetrie_paroi
//

inline double Eval_Dift_VDF_var_Face::flux_arete_symetrie_paroi(const DoubleTab& inco, int fac1,
                                                                int fac2, int fac3, int signe ) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam =  0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));

  int ori = orientation(fac3);
  //  double vit = inco(fac3);
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl));
  if ( !le_modele_turbulence.valeur().utiliser_loi_paroi() )
    {
      double dist = dist_norm_bord(fac1);
      double tau  = signe*(vit_imp - inco[fac3])/dist;
      double surf = 0.5*(surface(fac1)+surface(fac2));
      flux = tau*surf*(visc_lam+visc_turb);
    }
  else
    {
      // solution retenue pour le calcul du frottement sachant que sur l'une des faces
      // tau_tan vaut 0 car c'est une face qui porte une condition de symetrie
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
      double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      double coef = tau1+tau2;
      flux = coef;
    }
  return flux;
}

//// coeffs_arete_symetrie_paroi
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_symetrie_paroi(int fac1, int fac2,
                                                                int fac3, int signe,
                                                                double& aii1_2, double& aii3_4, double& ajj1_2) const
{
  if ( !le_modele_turbulence.valeur().utiliser_loi_paroi() )
    {
      int elem1 = elem_(fac3,0);
      int elem2 = elem_(fac3,1);
      double visc_lam =  0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
      double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                              + dv_diffusivite_turbulente(elem2));

      double dist = dist_norm_bord(fac1);
      double surf = 0.5*(surface(fac1)+surface(fac2));
      aii3_4 = signe*surf*(visc_lam+visc_turb)/dist;
      aii1_2 = 0;
      ajj1_2 = 0;
    }
  else
    {
      aii3_4 = 0;
      aii1_2 = 0;
      ajj1_2 = 0;
    }
}


//// secmem_arete_symetrie_paroi
//

inline double Eval_Dift_VDF_var_Face::secmem_arete_symetrie_paroi(int fac1, int fac2, int fac3, int signe) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int ori = orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  double visc_lam =  0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double visc_turb = 0.5*(dv_diffusivite_turbulente(elem1)
                          + dv_diffusivite_turbulente(elem2));
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl));
  if ( !le_modele_turbulence.valeur().utiliser_loi_paroi() )
    {
      const DoubleTab& inco = inconnue->valeurs();
      double dist = dist_norm_bord(fac1);
      double tau  = signe*(vit_imp - inco[fac3])/dist;
      double surf = 0.5*(surface(fac1)+surface(fac2));
      flux = tau*surf*(visc_lam+visc_turb);
    }
  else
    {
      // solution retenue pour le calcul du frottement sachant que sur l'une des faces
      // tau_tan vaut 0 car c'est une face qui porte une condition de symetrie
      // On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
      double tau1 = tau_tan(rang1,ori)*0.5*surface(fac1);
      double tau2 = tau_tan(rang2,ori)*0.5*surface(fac2);
      double coef = tau1+tau2;
      flux = coef;
    }
  return flux;
}

// Fonctions de calcul des flux pour une inconnue vectorielle
// Elles ne sont pas codees pour l'instant

//// flux_fa7_sortie_libre
//

inline void Eval_Dift_VDF_var_Face::flux_fa7_sortie_libre(const DoubleTab&, int ,
                                                          const Neumann_sortie_libre&,
                                                          int, DoubleVect& ) const
{
  // A coder !
}

////coeffs_fa7_sortie_libre
//

inline void Eval_Dift_VDF_var_Face::coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&,
                                                            DoubleVect&, DoubleVect& ) const
{
  // A Coder!
}

////secmem_fa7_sortie_libre
//

inline void Eval_Dift_VDF_var_Face::secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&,
                                                            int, DoubleVect&  ) const
{
  // A Coder!
}


//// flux_arete_interne
//

inline void Eval_Dift_VDF_var_Face::flux_arete_interne(const DoubleTab&, int, int,
                                                       int, int, DoubleVect& ) const
{
  // A coder !
}

//// coeffs_arete_interne
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_interne(int , int , int , int ,
                                                         DoubleVect&, DoubleVect& ) const
{
  // A coder!
}

//// secmem_arete_interne
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_interne(int ,
                                                         int , int , int ,
                                                         DoubleVect& ) const
{
  // A coder!
}

//// flux_arete_mixte
//

inline void Eval_Dift_VDF_var_Face::flux_arete_mixte(const DoubleTab&, int, int,
                                                     int, int, DoubleVect& ) const
{
  // A coder !
}

//// coeffs_arete_mixte
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_mixte(int , int , int , int ,
                                                       DoubleVect& , DoubleVect& ) const
{
  // A coder!
}

//// secmem_arete_mixte
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_mixte(int, int , int , int ,
                                                       DoubleVect& ) const
{
  // A coder!
}

//// flux_arete_fluide
//

inline void Eval_Dift_VDF_var_Face::flux_arete_fluide(const DoubleTab&, int, int,
                                                      int, int, DoubleVect&, DoubleVect& ) const
{
  // A coder !
}

//// coeffs_arete_fluide
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_fluide(int ,int ,
                                                        int , int , DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}

//// secmem_arete_fluide
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_fluide(int ,int ,int , int ,
                                                        DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// flux_arete_paroi
//

inline void Eval_Dift_VDF_var_Face::flux_arete_paroi(const DoubleTab&, int, int,
                                                     int, int, DoubleVect& ) const
{
  // A coder !
}

//// coeffs_arete_paroi
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_paroi(int , int , int , int ,
                                                       DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A coder!
}

//// secmem_arete_paroi
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_paroi(int, int , int , int ,
                                                       DoubleVect& ) const
{
  // A coder!
}

//// flux_arete_paroi_fluide
//

inline void Eval_Dift_VDF_var_Face::flux_arete_paroi_fluide(const DoubleTab&, int, int,
                                                            int, int, DoubleVect&, DoubleVect&) const
{
  // A coder !
}

//// coeffs_arete_paroi_fluide
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_paroi_fluide(int, int , int , int ,
                                                              DoubleVect&, DoubleVect&, DoubleVect& ) const
{
  // A Coder!
}


//// secmem_arete_paroi_fluide
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_paroi_fluide(int, int , int , int ,
                                                              DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// flux_arete_periodicite
//

inline void Eval_Dift_VDF_var_Face::flux_arete_periodicite(const DoubleTab& , int ,
                                                           int , int , int ,
                                                           DoubleVect& , DoubleVect& ) const
{
  ;
}

//// coeffs_arete_periodicite
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_periodicite(int , int , int , int ,
                                                             DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// secmem_arete_periodicite
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_periodicite(int , int , int , int ,
                                                             DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// flux_arete_symetrie
//

inline void Eval_Dift_VDF_var_Face::flux_arete_symetrie(const DoubleTab&, int, int,
                                                        int, int, DoubleVect& ) const
{
  // A coder !
}

//// coeffs_arete_symetrie
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_symetrie(int, int, int, int, DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}

//// secmem_arete_symetrie
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_symetrie(int, int,
                                                          int, int, DoubleVect& ) const
{
  // A Coder!
}


//// flux_fa7_elem
//

inline void Eval_Dift_VDF_var_Face::flux_fa7_elem(const DoubleTab&, int, int,
                                                  int, DoubleVect& ) const
{
  // A coder !
}

//// coeffs_fa7_elem
//

inline void Eval_Dift_VDF_var_Face::coeffs_fa7_elem(int , int ,int , DoubleVect&, DoubleVect& ) const
{
  // A Coder!
}

//// secmem_fa7_elem
//

inline void Eval_Dift_VDF_var_Face::secmem_fa7_elem(int , int ,
                                                    int , DoubleVect& ) const
{
  // A Coder!
}

//// flux_arete_symetrie_fluide
//

inline void Eval_Dift_VDF_var_Face::flux_arete_symetrie_fluide(const DoubleTab&, int, int,
                                                               int, int, DoubleVect&, DoubleVect& ) const
{
  // A coder !
}

inline void Eval_Dift_VDF_var_Face::coeffs_arete_symetrie_fluide(int ,int ,
                                                                 int , int , DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}

//// secmem_arete_symetrie_fluide
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_symetrie_fluide(int ,int ,int , int ,
                                                                 DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// flux_arete_symetrie_paroi
//

inline void Eval_Dift_VDF_var_Face::flux_arete_symetrie_paroi(const DoubleTab&, int, int,
                                                              int, int, DoubleVect& ) const
{
  // EMPTY //
  ;
}

//// coeffs_arete_symetrie_paroi
//

inline void Eval_Dift_VDF_var_Face::coeffs_arete_symetrie_paroi(int , int , int , int ,
                                                                DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A coder!
}

//// secmem_arete_symetrie_paroi
//

inline void Eval_Dift_VDF_var_Face::secmem_arete_symetrie_paroi(int, int , int , int ,
                                                                DoubleVect& ) const
{
  // A coder!
}

#endif
