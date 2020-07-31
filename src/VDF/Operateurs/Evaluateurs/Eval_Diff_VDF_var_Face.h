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
// File:        Eval_Diff_VDF_var_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Diff_VDF_var_Face_included
#define Eval_Diff_VDF_var_Face_included

#include <Eval_Diff_VDF_var.h>
#include <Eval_VDF_Face.h>

//
// .DESCRIPTION class Eval_Diff_VDF_var_Face
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est un Champ_Face
// Le champ de diffusivite n"est pas constant.
// Dans le cas de la methode IMPLICITE les evaluateurs calculent la quantite qui figure
// dans le premier membre de l'equation, nous ne prenons pas par consequent l'oppose en
// ce qui concerne les termes pour la matrice, par contre pour le second membre nous
// procedons comme en explicite mais en ne fesant intervenir que les valeurs fournies
// par les conditions limites.
//
//
// .SECTION voir aussi Eval_Diff_VDF_var

class Eval_Diff_VDF_var_Face : public Eval_Diff_VDF_var, public Eval_VDF_Face
{

public:
  inline Eval_Diff_VDF_var_Face();

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
};

//
// Fonctions inline de la classe Eval_Diff_VDF_var_Face
//

inline Eval_Diff_VDF_var_Face::Eval_Diff_VDF_var_Face() {}


//// calculer_arete_fluide
//

inline int Eval_Diff_VDF_var_Face::calculer_arete_fluide() const
{
  return 1;
}


//// calculer_arete_paroi
//

inline int Eval_Diff_VDF_var_Face::calculer_arete_paroi() const
{
  return 1;
}


//// calculer_arete_paroi_fluide
//

inline int Eval_Diff_VDF_var_Face::calculer_arete_paroi_fluide() const
{
  return 1;
}

//// calculer_arete_symetrie
//

inline int Eval_Diff_VDF_var_Face::calculer_arete_symetrie() const
{
  return 0;
}

//// calculer_arete_interne
//

inline int Eval_Diff_VDF_var_Face::calculer_arete_interne() const
{
  return 1;
}

//// calculer_arete_mixte
//

inline int Eval_Diff_VDF_var_Face::calculer_arete_mixte() const
{
  return 1;
}

//// calculer_fa7_sortie_libre
//

inline int Eval_Diff_VDF_var_Face::calculer_fa7_sortie_libre() const
{
  return 0;
}

//// calculer_arete_periodicite
//

inline int Eval_Diff_VDF_var_Face::calculer_arete_periodicite() const
{
  return 1;
}

//// calculer_arete_symetrie_paroi
//

inline int Eval_Diff_VDF_var_Face::calculer_arete_symetrie_paroi() const
{
  return 1;
}

//// calculer_arete_symetrie_fluide
//

inline int Eval_Diff_VDF_var_Face::calculer_arete_symetrie_fluide() const
{
  return 1;
}

// Fonctions de calcul des flux pour une inconnue scalaire

//// flux_arete_fluide
//

inline void Eval_Diff_VDF_var_Face::flux_arete_fluide(const DoubleTab& inco, int fac1,
                                                      int fac2, int fac3, int signe,
                                                      double& flux3, double& flux1_2) const
{
  double tau,dist;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));

  // Calcul de flux3

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
  dist = dist_norm_bord(fac1);
  /*   dist = 2*dist_norm_bord(fac1);  */
  tau = signe * (vit_imp - inco[fac3])/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  flux3 = tau*surf*diffus*poros;

  // Calcul de flux1_2

  dist = dist_face(fac1,fac2,k);
  tau = (inco(fac2) - inco(fac1))/dist;
  flux1_2 = tau*diffus*porosite(fac3)*surface(fac3);
}


//// coeffs_arete_fluide
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                        double& aii1_2, double& aii3_4,
                                                        double& ajj1_2) const
{
  double dist;
  int k= orientation(fac3);
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));

  // Calcul de aii3_4

  /*   dist = 2*dist_norm_bord(fac1); */
  dist = dist_norm_bord(fac1);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  aii3_4 = signe*surf*diffus*poros/dist;

  // Calcul de aii1_2 et ajj1_2

  dist = dist_face(fac1,fac2,k);
  aii1_2 = ajj1_2  = (diffus*porosite(fac3)*surface(fac3))/dist;
}

//// secmem_arete_fluide
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_fluide(int fac1, int fac2, int fac3, int signe,
                                                        double& flux3, double& flux1_2) const
{
  double tau,dist;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);

  // Calcul de flux3

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
  //dist = 2*dist_norm_bord(fac1);
  dist = dist_norm_bord(fac1);
  tau = signe * vit_imp/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));

  flux3 = tau*surf*diffus*poros;
  flux1_2 = 0;
}


//// flux_arete_interne
//

inline double Eval_Diff_VDF_var_Face::flux_arete_interne(const DoubleTab& inco, int fac1,
                                                         int fac2, int fac3, int fac4) const
{
  double flux;
  int ori=orientation(fac1);
  double diffus=0.25* ( dv_diffusivite(elem_(fac3,0)) + dv_diffusivite(elem_(fac3,1))
                        +dv_diffusivite(elem_(fac4,0)) + dv_diffusivite(elem_(fac4,1)) );
  flux = 0.25 * (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori) *
         (surface(fac1)+surface(fac2))*diffus*(porosite(fac1)+porosite(fac2));
  return flux;
}


//// coeffs_arete_interne
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_interne(int fac1, int fac2, int fac3, int fac4,
                                                         double& aii, double& ajj) const
{
  int ori=orientation(fac1);
  double diffus=0.25* ( dv_diffusivite(elem_(fac3,0)) + dv_diffusivite(elem_(fac3,1))
                        +dv_diffusivite(elem_(fac4,0)) + dv_diffusivite(elem_(fac4,1)) );
  aii = ajj = 0.25*(surface(fac1)+surface(fac2))*diffus*
              (porosite(fac1)+porosite(fac2))/dist_face(fac3,fac4,ori);
}

//// secmem_arete_interne
//

inline double Eval_Diff_VDF_var_Face::secmem_arete_interne(int fac1, int fac2, int fac3, int fac4) const
{
  return 0;
}

//// flux_arete_mixte
//

inline double Eval_Diff_VDF_var_Face::flux_arete_mixte(const DoubleTab& inco, int fac1,
                                                       int fac2, int fac3, int fac4) const
{
  double flux=0;

  if (inco[fac4]*inco[fac3] != 0)
    {
      double diffus=0;
      int element;

      if ((element=elem_(fac3,0)) != -1)
        diffus+=dv_diffusivite(element);
      if ((element=elem_(fac3,1)) != -1)
        diffus+=dv_diffusivite(element);
      if ((element=elem_(fac4,0)) != -1)
        diffus+=dv_diffusivite(element);
      if ((element=elem_(fac4,1)) != -1)
        diffus+=dv_diffusivite(element);

      diffus/=3.0;

      int ori=orientation(fac1);
      double tau = (inco[fac4]-inco[fac3])/dist_face(fac3,fac4,ori);
      flux = 0.25*tau*(surface(fac1)+surface(fac2))*
             diffus*(porosite(fac1)+porosite(fac2));
    }
  return flux;
}


//// coeffs_arete_mixte
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_mixte(int fac1, int fac2, int fac3, int fac4,
                                                       double& aii, double& ajj) const
{

  if (inconnue->valeurs()[fac4]*inconnue->valeurs()[fac3] != 0)
    {
      double diffus=0;
      int element;

      if ((element=elem_(fac3,0)) != -1)
        diffus+=dv_diffusivite(element);
      if ((element=elem_(fac3,1)) != -1)
        diffus+=dv_diffusivite(element);
      if ((element=elem_(fac4,0)) != -1)
        diffus+=dv_diffusivite(element);
      if ((element=elem_(fac4,1)) != -1)
        diffus+=dv_diffusivite(element);

      diffus/=3.0;

      int ori=orientation(fac1);
      aii = ajj= 0.25*(surface(fac1)+surface(fac2))*diffus*
                 (porosite(fac1)+porosite(fac2))/dist_face(fac3,fac4,ori);
    }
  else
    {
      aii=ajj=0;
    }
}

//// secmem_arete_mixte
//

inline double Eval_Diff_VDF_var_Face::secmem_arete_mixte(int fac1, int fac2, int fac3, int fac4) const
{
  return 0;
}

//// flux_arete_paroi
//

inline double Eval_Diff_VDF_var_Face::flux_arete_paroi(const DoubleTab& inco, int fac1,
                                                       int fac2, int fac3, int signe) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);
  int elem1 = elem_(fac3,0);
  int elem2 = elem_(fac3,1);
  if (elem1==-1)
    elem1 = elem2;
  else if (elem2==-1)
    elem2 = elem1;

  double diffus=0.5*(dv_diffusivite(elem1)+dv_diffusivite(elem2));
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
  double dist = dist_norm_bord(fac1);
  double tau = signe * (vit_imp - inco[fac3])/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  flux = tau*surf*diffus;
  return flux;
}

//// coeffs_arete_paroi
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_paroi(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4,
                                                       double& ajj1_2) const
{
  double dist = dist_norm_bord(fac1);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));
  aii3_4 = signe*surf*diffus/dist;
  aii1_2 = 0;
  ajj1_2 = 0;
}


//// secmem_arete_paroi
//

inline double Eval_Diff_VDF_var_Face::secmem_arete_paroi(int fac1, int fac2, int fac3, int signe) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
  double dist = dist_norm_bord(fac1);
  double tau  = signe * vit_imp/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  flux = tau*surf*diffus;
  return flux;
}


//// flux_arete_paroi_fluide
//

inline void Eval_Diff_VDF_var_Face::flux_arete_paroi_fluide(const DoubleTab& inco, int fac1,
                                                            int fac2, int fac3, int signe,
                                                            double& flux3, double& flux1_2 ) const
{
  double tau,dist,vit_imp;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));

  // Calcul de flux3
  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi

  if (est_egal(inco[fac1],0)) // fac1 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl);
  else  // fac2 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl);

  dist = dist_norm_bord(fac1);

  tau = signe * (vit_imp - inco[fac3])/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  flux3 = tau*surf*diffus*poros;

  // Calcul de flux1_2

  dist = dist_face(fac1,fac2,k);
  tau = (inco[fac2] - inco[fac1])/dist;
  flux1_2 = tau*diffus*porosite(fac3)*surface(fac3);

}

//// coeffs_arete_paroi_fluide
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                              double& aii1_2, double& aii3_4,
                                                              double& ajj1_2) const
{
  double dist;
  int k= orientation(fac3);
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));

  //Calcul des aii 3_4
  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi

  dist = dist_norm_bord(fac1);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  aii3_4 = signe*surf*diffus*poros/dist;

  // Calcul des aii et ajj 1_2

  dist = dist_face(fac1,fac2,k);
  aii1_2 = ajj1_2 = diffus*porosite(fac3)*surface(fac3)/dist;
}


//// secmem_arete_paroi_fluide
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe,
                                                              double& flux3, double& flux1_2) const
{
  double tau,dist,vit_imp;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));

  // Calcul de flux
  // On ne sait pas qui de fac1 ou de fac2 est la face de paroi

  if (est_egal(inconnue->valeurs()[fac1],0)) // fac1 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl);
  else  // fac2 est la face de paroi
    vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl);

  dist = dist_norm_bord(fac1);

  tau = signe*vit_imp/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  flux3 = tau*surf*diffus*poros;
  flux1_2 = 0;
}


//// flux_arete_periodicite
//

inline void Eval_Diff_VDF_var_Face::flux_arete_periodicite(const DoubleTab& inco,
                                                           int fac1, int fac2, int fac3, int fac4,
                                                           double& flux3_4, double& flux1_2) const
{
  double flux,diffus;
  int ori;

  ori=orientation(fac1);
  diffus=0.25* ( dv_diffusivite(elem_(fac3,0)) + dv_diffusivite(elem_(fac3,1))
                 +dv_diffusivite(elem_(fac4,0)) + dv_diffusivite(elem_(fac4,1)) );
  flux = 0.25 * (inco[fac4]-inco[fac3])/dist_face_period(fac3,fac4,ori) *
         (surface(fac1)+surface(fac2))*diffus*(porosite(fac1)+porosite(fac2));
  flux3_4 = flux;

  ori=orientation(fac3);
  diffus =0.25* ( dv_diffusivite(elem_(fac1,0)) + dv_diffusivite(elem_(fac1,1))
                  +dv_diffusivite(elem_(fac2,0)) + dv_diffusivite(elem_(fac2,1)) );
  flux = 0.25 * (inco[fac2]-inco[fac1])/dist_face_period(fac1,fac2,ori) *
         (surface(fac3)+surface(fac4))*diffus*(porosite(fac3)+porosite(fac4));
  flux1_2 = flux;
}

//// coeffs_arete_periodicite
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_periodicite(int fac1, int fac2, int fac3, int fac4,
                                                             double& aii, double& ajj) const
{
  int ori;
  ori=orientation(fac1);
  double diffus=0.25* ( dv_diffusivite(elem_(fac3,0)) + dv_diffusivite(elem_(fac3,1))
                        +dv_diffusivite(elem_(fac4,0)) + dv_diffusivite(elem_(fac4,1)) );
  aii = ajj = 0.25*(surface(fac1)+surface(fac2))*diffus*(porosite(fac1)+porosite(fac2))/
              dist_face_period(fac3,fac4,ori);
}


//// secmem_arete_periodicite
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_periodicite(int fac1, int fac2, int fac3, int fac4,
                                                             double& flux3_4, double& flux1_2) const
{
  ;
}


//// flux_arete_symetrie
//

inline double Eval_Diff_VDF_var_Face::flux_arete_symetrie(const DoubleTab&, int, int, int, int) const
{
  return 0;
}

//// coeffs_arete_symetrie
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_symetrie(int, int, int, int, double&, double&, double&) const
{
  ;
}

//// secmem_arete_symetrie
//

inline double Eval_Diff_VDF_var_Face::secmem_arete_symetrie(int, int, int, int) const
{
  return 0;
}


//// flux_fa7_elem
//

inline double Eval_Diff_VDF_var_Face::flux_fa7_elem(const DoubleTab& inco, int element,
                                                    int fac1, int fac2) const
{
  double flux;
  int ori=orientation(fac1);
  flux = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)) * dv_diffusivite(element) *
         (inco[fac2]-inco[fac1])/dist_face(fac1,fac2,ori);
  return flux;
}

//// coeffs_fa7_elem
//

inline void Eval_Diff_VDF_var_Face::coeffs_fa7_elem(int element ,int fac1, int fac2, double& aii, double& ajj) const
{
  int ori=orientation(fac1);
  aii = ajj = 0.5*(surface(fac1)*porosite(fac1)+surface(fac2)*porosite(fac2)) * dv_diffusivite(element)/dist_face(fac1,fac2,ori);
}


//// secmem_fa7_elem
//

inline double Eval_Diff_VDF_var_Face::secmem_fa7_elem(int ,int fac1, int fac2) const
{
  return 0;
}


inline double Eval_Diff_VDF_var_Face::flux_fa7_sortie_libre(const DoubleTab&, int ,
                                                            const Neumann_sortie_libre&, int ) const
{
  return 0;
}

//// coeffs_fa7_sortie_libre
//

inline void Eval_Diff_VDF_var_Face::coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&,
                                                            double& aii, double& ajj ) const
{
  aii = ajj = 0;
}

//// secmem_fa7_sortie_libre
//

inline double Eval_Diff_VDF_var_Face::secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&,
                                                              int ) const
{
  return 0;
}

//// flux_arete_symetrie_fluide
//

inline void Eval_Diff_VDF_var_Face::flux_arete_symetrie_fluide(const DoubleTab& inco, int fac1,
                                                               int fac2, int fac3, int signe,
                                                               double& flux3, double& flux1_2) const
{
  double tau,dist;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));

  // Calcul de flux3

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,k,la_zcl));
  //dist = 2*dist_norm_bord(fac1);
  dist = dist_norm_bord(fac1);
  tau = signe * (vit_imp - inco[fac3])/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  flux3 = tau*surf*diffus*poros;

  // Calcul de flux1_2

  dist = dist_face(fac1,fac2,k);
  tau = (inco(fac2) - inco(fac1))/dist;
  // Modif pour l'element fictif
  //flux1_2 = tau*diffus*porosite(fac3)*surface(fac3);
  flux1_2 = 0.5*tau*diffus*porosite(fac3)*surface(fac3);
}

//// coeffs_arete_symetrie_fluide
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_symetrie_fluide(int fac1, int fac2, int fac3, int signe,
                                                                 double& aii1_2, double& aii3_4,
                                                                 double& ajj1_2) const
{
  double dist;
  int k= orientation(fac3);
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));

  // Calcul de aii3_4

  //dist = 2*dist_norm_bord(fac1);
  dist = dist_norm_bord(fac1);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  aii3_4 = (signe*surf*diffus*poros)/dist;

  // Calcul de aii1_2 et ajj1_2

  dist = dist_face(fac1,fac2,k);
  // Modif pour l'element fictif
  //  aii1_2 = ajj1_2  = (diffus*porosite(fac3)*surface(fac3))/dist;
  aii1_2 = ajj1_2  = (0.5*diffus*porosite(fac3)*surface(fac3))/dist;
}

//// secmem_arete_symetrie_fluide
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_symetrie_fluide(int fac1, int fac2, int fac3, int signe,
                                                                 double& flux3, double& flux1_2) const
{
  double tau,dist;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);

  // Calcul de flux3

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
  //dist = 2*dist_norm_bord(fac1);
  dist = dist_norm_bord(fac1);
  tau = signe * vit_imp/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double poros = 0.5*(porosite(fac1)+porosite(fac2));
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));

  flux3 = tau*surf*diffus*poros;
  flux1_2 = 0;
}

//// flux_arete_symetrie_paroi
//

inline double Eval_Diff_VDF_var_Face::flux_arete_symetrie_paroi(const DoubleTab& inco, int fac1,
                                                                int fac2, int fac3, int signe) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));

  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,k,la_zcl));
  double dist = dist_norm_bord(fac1);
  double tau = signe * (vit_imp - inco[fac3])/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  flux = tau*surf*diffus;
  return flux;
}

//// coeffs_arete_symetrie_paroi
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_symetrie_paroi(int fac1, int fac2, int fac3,
                                                                int signe, double& aii1_2, double& aii3_4,
                                                                double& ajj1_2) const
{
  double dist = dist_norm_bord(fac1);
  double surf = 0.5*(surface(fac1)+surface(fac2));
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));
  aii3_4 = signe*surf*diffus/dist;
  aii1_2 = 0;
  ajj1_2 = 0;
}


//// secmem_arete_symetrie_paroi
//

inline double Eval_Diff_VDF_var_Face::secmem_arete_symetrie_paroi(int fac1, int fac2, int fac3, int signe) const
{
  double flux;
  int rang1 = (fac1-premiere_face_bord);
  int rang2 = (fac2-premiere_face_bord);
  int k= orientation(fac3);
  double diffus=0.5*(dv_diffusivite(elem_(fac3,0))+dv_diffusivite(elem_(fac3,1)));
  double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,la_zcl)+
                        Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,la_zcl));
  double dist = dist_norm_bord(fac1);
  double tau  = signe * vit_imp/dist;
  double surf = 0.5*(surface(fac1)+surface(fac2));
  flux = tau*surf*diffus;
  return flux;
}


// Fonctions de calcul des flux pour une inconnue vectorielle
// Elles ne sont pas codees pour l'instant!

//// flux_arete_fluide
//

inline void Eval_Diff_VDF_var_Face::flux_arete_fluide(const DoubleTab& , int ,int ,
                                                      int , int ,
                                                      DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// coeffs_arete_fluide
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_fluide(int ,int ,
                                                        int , int , DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}

//// secmem_arete_fluide
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_fluide(int ,int ,int , int ,
                                                        DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// flux_arete_interne
//

inline void Eval_Diff_VDF_var_Face::flux_arete_interne(const DoubleTab& , int ,
                                                       int , int , int ,
                                                       DoubleVect& ) const
{
  // A coder!
}

//// coeffs_arete_interne
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_interne(int , int , int , int ,
                                                         DoubleVect&, DoubleVect& ) const
{
  // A coder!
}

//// secmem_arete_interne
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_interne(int ,
                                                         int , int , int ,
                                                         DoubleVect& ) const
{
  // A coder!
}


//// flux_arete_mixte
//

inline void Eval_Diff_VDF_var_Face::flux_arete_mixte(const DoubleTab& , int ,
                                                     int , int , int ,
                                                     DoubleVect& ) const
{
  // A coder!
}

//// coeffs_arete_mixte
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_mixte(int , int , int , int ,
                                                       DoubleVect& , DoubleVect& ) const
{
  // A coder!
}

//// secmem_arete_mixte
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_mixte(int, int , int , int ,
                                                       DoubleVect& ) const
{
  // A coder!
}


//// flux_arete_paroi
//

inline void Eval_Diff_VDF_var_Face::flux_arete_paroi(const DoubleTab& , int ,
                                                     int , int , int ,
                                                     DoubleVect& ) const
{
  // A coder!
}

//// coeffs_arete_paroi
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_paroi(int , int , int , int ,
                                                       DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A coder!
}

//// secmem_arete_paroi
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_paroi(int, int , int , int ,
                                                       DoubleVect& ) const
{
  // A coder!
}


//// flux_arete_paroi_fluide
//

inline void Eval_Diff_VDF_var_Face::flux_arete_paroi_fluide(const DoubleTab& , int ,
                                                            int , int , int ,
                                                            DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// coeffs_arete_paroi_fluide
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_paroi_fluide(int, int , int , int ,
                                                              DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}


//// secmem_arete_paroi_fluide
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_paroi_fluide(int, int , int , int ,
                                                              DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}


//// flux_arete_periodicite
//

inline void Eval_Diff_VDF_var_Face::flux_arete_periodicite(const DoubleTab& , int ,
                                                           int , int , int ,
                                                           DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// coeffs_arete_periodicite
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_periodicite(int , int , int , int ,
                                                             DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// secmem_arete_periodicite
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_periodicite(int , int , int , int ,
                                                             DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}


//// flux_arete_symetrie
//

inline void Eval_Diff_VDF_var_Face::flux_arete_symetrie(const DoubleTab&, int, int,
                                                        int, int, DoubleVect& ) const
{
  // A Coder!
}

//// coeffs_arete_symetrie
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_symetrie(int, int, int, int, DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}

//// secmem_arete_symetrie
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_symetrie(int, int,
                                                          int, int, DoubleVect& ) const
{
  // A Coder!
}


//// flux_fa7_elem
//

inline void Eval_Diff_VDF_var_Face::flux_fa7_elem(const DoubleTab& , int , int ,
                                                  int , DoubleVect& ) const
{
  // A Coder!
}

//// coeffs_fa7_elem
//

inline void Eval_Diff_VDF_var_Face::coeffs_fa7_elem(int , int ,int , DoubleVect&, DoubleVect& ) const
{
  // A Coder!
}

//// secmem_fa7_elem
//

inline void Eval_Diff_VDF_var_Face::secmem_fa7_elem(int , int ,
                                                    int , DoubleVect& ) const
{
  // A Coder!
}


inline void Eval_Diff_VDF_var_Face::flux_fa7_sortie_libre(const DoubleTab&, int ,
                                                          const Neumann_sortie_libre&,
                                                          int, DoubleVect&  ) const
{
  // A Coder!
}

////coeffs_fa7_sortie_libre
//

inline void Eval_Diff_VDF_var_Face::coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&,
                                                            DoubleVect&, DoubleVect& ) const
{
  // A Coder!
}

////secmem_fa7_sortie_libre
//

inline void Eval_Diff_VDF_var_Face::secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&,
                                                            int, DoubleVect&  ) const
{
  // A Coder!
}

//// flux_arete_symetrie_fluide
//

inline void Eval_Diff_VDF_var_Face::flux_arete_symetrie_fluide(const DoubleTab& , int ,int ,
                                                               int , int ,
                                                               DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// coeffs_arete_symetrie_fluide
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_symetrie_fluide(int ,int ,
                                                                 int , int ,
                                                                 DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A Coder!
}

//// secmem_arete_symetrie_fluide
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_symetrie_fluide(int ,int ,int , int ,
                                                                 DoubleVect& , DoubleVect& ) const
{
  // A Coder!
}

//// flux_arete_symetrie_paroi
//

inline void Eval_Diff_VDF_var_Face::flux_arete_symetrie_paroi(const DoubleTab& , int ,
                                                              int , int , int ,
                                                              DoubleVect& ) const
{
  // A coder!
}

//// coeffs_arete_symetrie_paroi
//

inline void Eval_Diff_VDF_var_Face::coeffs_arete_symetrie_paroi(int , int , int , int ,
                                                                DoubleVect&, DoubleVect&, DoubleVect&) const
{
  // A coder!
}

//// secmem_arete_symetrie_paroi
//

inline void Eval_Diff_VDF_var_Face::secmem_arete_symetrie_paroi(int, int , int , int ,
                                                                DoubleVect& ) const
{
  // A coder!
}


#endif
