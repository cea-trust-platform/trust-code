/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Eval_Diff_VDF_var_Elem_aniso.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Diff_VDF_var_Elem_aniso_included
#define Eval_Diff_VDF_var_Elem_aniso_included

#include <Eval_Diff_VDF_var_aniso.h>
#include <Eval_VDF_Elem.h>

//
// .DESCRIPTION class Eval_Diff_VDF_var_Elem_aniso
//
// Evaluateur VDF pour la diffusion
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite n'est pas constant.

//
// .SECTION voir aussi Eval_Diff_VDF_var_aniso

class Eval_Diff_VDF_var_Elem_aniso : public Eval_Diff_VDF_var_aniso, public Eval_VDF_Elem
{

public:

  inline Eval_Diff_VDF_var_Elem_aniso();

  inline int calculer_flux_faces_echange_externe_impose() const ;
  inline int calculer_flux_faces_echange_global_impose() const ;
  inline int calculer_flux_faces_entree_fluide() const ;
  inline int calculer_flux_faces_paroi() const ;
  inline int calculer_flux_faces_paroi_adiabatique() const ;
  inline int calculer_flux_faces_paroi_defilante() const ;
  inline int calculer_flux_faces_paroi_fixe() const ;
  inline int calculer_flux_faces_sortie_libre() const ;
  inline int calculer_flux_faces_symetrie() const ;
  inline int calculer_flux_faces_periodique() const ;
  inline int calculer_flux_faces_NSCBC() const ;

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles sont de type double et renvoient le flux

  inline double flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&, int ) const;
  inline double flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Echange_global_impose&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_paroi&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_paroi_adiabatique&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Symetrie&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Periodique&, int ) const;
  inline double flux_face(const DoubleTab&, int , const NSCBC&, int ) const;
  inline double flux_faces_interne(const DoubleTab&, int ) const;

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux

  inline void flux_face(const DoubleTab&, int , const Symetrie&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Periodique&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_sortie_libre&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_paroi_adiabatique&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_paroi&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&,
                        int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Echange_global_impose&,
                        int, DoubleVect& flux) const;
  //inline void flux_face(const DoubleTab&, int ,const Nouvelle_Cl_VDF&,
  //                      int, DoubleVect& flux) const=0;

  inline void flux_faces_interne(const DoubleTab&, int ,
                                 DoubleVect& flux) const;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  inline void coeffs_face(int,int, const Symetrie&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_sortie_libre&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_paroi_fixe&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_paroi_defilante&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi_adiabatique&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Echange_global_impose&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Periodique&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const NSCBC&, double& aii, double& ajj ) const;
  inline void coeffs_faces_interne(int, double& aii, double& ajj ) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l"implicite pour les grandeurs scalaires.

  inline double secmem_face(int, const Symetrie&, int ) const;
  inline double secmem_face(int, const Neumann_sortie_libre&, int ) const;
  inline double secmem_face(int, const Dirichlet_entree_fluide&, int ) const;
  inline double secmem_face(int, const Dirichlet_paroi_fixe&, int ) const;
  inline double secmem_face(int, const Dirichlet_paroi_defilante&, int ) const;
  inline double secmem_face(int, const Neumann_paroi_adiabatique&, int ) const;
  inline double secmem_face(int, const Neumann_paroi&, int ) const;
  inline double secmem_face(int, int, int, const Echange_externe_impose&, int ) const;
  inline double secmem_face(int, const Echange_global_impose&, int ) const;
  inline double secmem_face(int, const Periodique&, int ) const;
  inline double secmem_face(int, const NSCBC&, int ) const;
  inline double secmem_faces_interne(int ) const;


  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  inline void coeffs_face(int,int, const Symetrie&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int, int,const Dirichlet_paroi_fixe&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_paroi_defilante&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi_adiabatique&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Echange_global_impose&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Periodique&, DoubleVect& aii, DoubleVect& ajj ) const;

  //virtual void coeffs_face(const DoubleTab&, int , const Nouvelle_Cl_VDF&, int,
  //                           DoubleVect& aii, DoubleVect& ajj ) const;

  inline void coeffs_faces_interne(int, DoubleVect& aii, DoubleVect& ajj ) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l"implicite pour les grandeurs vectorielles.

  inline void secmem_face(int, const Symetrie&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Neumann_sortie_libre&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_entree_fluide&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_paroi_fixe&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_paroi_defilante&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Neumann_paroi_adiabatique&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Neumann_paroi&, int, DoubleVect& ) const;
  inline void secmem_face(int, int, int, const Echange_externe_impose&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Echange_global_impose&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Periodique&, int, DoubleVect& ) const;
  inline void secmem_faces_interne(int, DoubleVect& ) const;

  //inline double secmem_face(const DoubleTab&, int , const Nouvelle_Cl_VDF&, int, DoubleVect& ) const;

protected:

private:

};

//
// Fonctions inline de la classe Eval_Diff_VDF_var_Elem_aniso
//

inline Eval_Diff_VDF_var_Elem_aniso::Eval_Diff_VDF_var_Elem_aniso() {}




//#include <Cal_std.h>

//// calculer_flux_faces_echange_externe_impose
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_echange_externe_impose() const
{
  return 1;
}


//// calculer_flux_faces_echange_global_impose
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_echange_global_impose() const
{
  return 1;
}


//// calculer_flux_faces_entree_fluide
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_entree_fluide() const
{
  return 1;
}


//// calculer_flux_faces_paroi
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_paroi() const
{
  return 1;
}


//// calculer_flux_faces_paroi_adiabatique
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_paroi_adiabatique() const
{
  return 0;
}


//// calculer_flux_faces_paroi_defilante
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_paroi_defilante() const
{
  return 0;
}


//// calculer_flux_faces_paroi_fixe
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_paroi_fixe() const
{
  return 1;
}


//// calculer_flux_faces_sortie_libre
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_sortie_libre() const
{
  return 0;
}


//// calculer_flux_faces_symetrie
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_symetrie() const
{
  return 0;
}


//// calculer_flux_faces_periodique
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_periodique() const
{
  return 1;
}

//// calculer_flux_faces_NSCBC
//

inline int Eval_Diff_VDF_var_Elem_aniso::calculer_flux_faces_NSCBC() const
{
  return 0;
}





//#include <Scal_corps_base.h>

// Fonctions de calcul des flux pour une grandeur scalaire

//// flux_face avec Dirichlet_entree_fluide
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int face,
                                                      const Dirichlet_entree_fluide& la_cl,
                                                      int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int ori= orientation(face);
  // Olga avait mis :
  // double dist = 2*Dist_norm_bord(face);
  // Pierre dit que :
  double dist = la_zone->dist_norm_bord(face);
  double flux;
  if (n0 != -1)
    {
      flux = (T_imp-inco[n0])*surface(face)/dist;
      flux *= (porosite(face)*dt_diffusivite(n0,ori));
    }
  else   // n1 != -1
    {
      flux = (inco[n1]-T_imp)*surface(face)/dist;
      flux*= (porosite(face)*dt_diffusivite(n1,ori));
    }
  return flux;
}


//// coeffs_face avec Dirichlet_entree_fluide
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face, int,
                                                      const Dirichlet_entree_fluide& la_cl,
                                                      double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int ori= orientation(face);
  //  int j = elem(face,1);
  double dist = la_zone->dist_norm_bord(face);
  if (i != -1)
    {
      aii =  porosite(face)*dt_diffusivite(i,ori)*surface(face)/dist;
      ajj = 0;
    }
  else   // j != -1
    {
      ajj =  porosite(face)*dt_diffusivite(elem_(face,1),ori)*surface(face)/dist;
      aii = 0;
    }
}

//// secmem_face avec Dirichlet_entree_fluide
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                                        int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  int i = elem_(face,0);
  int ori= orientation(face);
  //  int j = elem(face,1);
  double dist = la_zone->dist_norm_bord(face);

  double flux;
  if (i != -1)
    {
      flux = T_imp*surface(face)/dist;
      flux *= porosite(face)*dt_diffusivite(i,ori);
    }
  else   // j != -1
    {
      flux = -T_imp*surface(face)/dist;
      flux *= (porosite(face)*dt_diffusivite(elem_(face,1),ori));
    }
  return flux;
}

//// flux_face avec Dirichlet_paroi_defilante
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab&, int ,
                                                      const Dirichlet_paroi_defilante&, int ) const
{
  return 0;
}

//// coeffs_face avec Dirichlet_paroi_defilante
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int , int,
                                                      const Dirichlet_paroi_defilante&,
                                                      double&, double& ) const
{
  ;
}

//// secmem_face avec Dirichlet_paroi_defilante
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Dirichlet_paroi_defilante&,
                                                        int ) const
{
  return 0;
}

//// flux_face avec Dirichlet_paroi_fixe
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab&, int ,
                                                      const Dirichlet_paroi_fixe&,
                                                      int ) const
{
  return 0;
}

//// coeffs_face avec Dirichlet_paroi_fixe
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int ,int,
                                                      const Dirichlet_paroi_fixe&,
                                                      double&, double& ) const
{
  ;
}

//// secmem_face avec Dirichlet_paroi_fixe
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Dirichlet_paroi_fixe&,
                                                        int ) const
{
  return 0;
}


//// flux_face avec Echange_externe_impose
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                      const Echange_externe_impose& la_cl,
                                                      int num1) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv, heq;
  double h_imp = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  double e;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  e=la_zone->dist_norm_bord(face);
  int ori = orientation(face);
  double flux;
  if (n0 != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(n0,ori);
      if (dt_diffusivite(n0,ori) == 0.0)
        {
//        h_total_inv = 1/h_imp;
          heq = 0.0;
        }
      else
        {
          h_total_inv = 1/h_imp + e/dt_diffusivite(n0,ori);
          heq = 1.0 / h_total_inv;
        }

//      flux = (T_ext - inco[n0])*surface(face)/h_total_inv;
      flux = heq*(T_ext - inco[n0])*surface(face);
    }
  else
    {
      //e = la_zone->xp(n1,ori) - la_zone->xv(face,ori);
      if (dt_diffusivite(n1,ori) == 0.0)
        {
//        h_total_inv = 1/h_imp;
          heq = 0.0;
        }
      else
        {
          h_total_inv = 1/h_imp + e/dt_diffusivite(n1,ori);
          heq = 1.0 / h_total_inv;
        }

//      flux = (inco[n1] - T_ext)*surface(face)/h_total_inv;
      flux = heq*(inco[n1] - T_ext)*surface(face);
    }
  return flux;
}

//// coeffs_face avec Echange_externe_impose
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                      const Echange_externe_impose& la_cl,
                                                      double& aii, double& ajj) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv, heq;
  double h_imp = la_cl.h_imp(face-num1);
  double e;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  int ori = orientation(face);
  e=la_zone->dist_norm_bord(face);
  if (i != -1)
    {
      // e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      if (dt_diffusivite(i,ori) == 0.0)
        {
//        h_total_inv =  1/h_imp;
          heq = 0.0;
        }
      else
        {
          h_total_inv =  1/h_imp + e/dt_diffusivite(i,ori);
          heq = 1.0 / h_total_inv;
        }

//      aii = surface(face)/h_total_inv;
      aii = heq*surface(face);
      ajj = 0;
    }
  else
    {
      // e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      if (dt_diffusivite(elem_(face,1),ori) == 0.0)
        {
//        h_total_inv =  1/h_imp;
          heq = 0.0;
        }
      else
        {
          h_total_inv =  1/h_imp + e/dt_diffusivite(elem_(face,1),ori);
          heq = 1.0 / h_total_inv;
        }

//      ajj = surface(face)/h_total_inv;
      ajj = heq*surface(face);
      aii = 0;
    }
}

//// secmem_face avec Echange_externe_impose
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int boundary_index,int face, int local_face, const Echange_externe_impose& la_cl,
                                                        int num1) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv, heq;
  double h_imp = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  double e;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  int ori = orientation(face);
  e=la_zone->dist_norm_bord(face);
  double flux;
  if (i != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      if (dt_diffusivite(i,ori) == 0.0)
        {
//        h_total_inv = 1/h_imp;
          heq = 0.0;
        }
      else
        {
          h_total_inv = 1/h_imp + e/dt_diffusivite(i,ori);
          heq = 1.0 / h_total_inv;
        }

//      flux = T_ext*surface(face)/h_total_inv;
      flux = heq*T_ext*surface(face);
    }
  else
    {
      //e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      if (dt_diffusivite(elem_(face,1),ori) == 0.0)
        {
//        h_total_inv = 1/h_imp;
          heq = 0.0;
        }
      else
        {
          h_total_inv = 1/h_imp + e/dt_diffusivite(elem_(face,1),ori);
          heq = 1.0 / h_total_inv;
        }

//      flux = - T_ext*surface(face)/h_total_inv;
      flux = - heq*T_ext*surface(face);
    }
  return flux;
}

//// flux_face avec Echange_global_impose
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int face ,
                                                      const Echange_global_impose& la_cl, int num1) const
{
  double h = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double flux;
  if (n0 != -1)
    flux = h*(T_ext - inco[n0])*surface(face);
  else
    flux = h*(inco[n1] - T_ext)*surface(face);
  return flux;
}

//// coeffs_face avec Echange_global_impose
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face, int num1,
                                                      const Echange_global_impose& la_cl,
                                                      double& aii, double& ajj) const
{
  double h = la_cl.h_imp(face-num1);
  int i = elem_(face,0);
  //  int j = elem(face,1);
  if (i!= -1)
    {
      aii = h*surface(face);
      ajj = 0;
    }
  else
    {
      ajj = h*surface(face);
      aii = 0;
    }
}

//// secmem_face avec Echange_global_impose
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int face, const Echange_global_impose& la_cl,
                                                        int num1) const
{
  double h = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  int i = elem_(face,0);
  //  int j = elem(face,1);
  double flux;
  if (i != -1)
    flux = h*T_ext*surface(face);
  else
    flux = -h*T_ext*surface(face);
  return flux;
}


//// flux_face avec Neumann_paroi
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& , int face,
                                                      const Neumann_paroi& la_cl,
                                                      int num1) const
{
  int i = elem_(face,0);
  //  int j = elem(face,1);
  double flux;
  if (i != -1)
    flux = la_cl.flux_impose(face-num1)*surface(face);
  else
    flux = -la_cl.flux_impose(face-num1)*surface(face);

  return flux;
}

//// coeffs_face avec Neumann_paroi
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int , int,
                                                      const Neumann_paroi& ,
                                                      double& aii, double& ajj) const
{
  aii = 0;
  ajj = 0;
}

//// secmem_face avec Neumann_paroi
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int face, const Neumann_paroi& la_cl,
                                                        int num1) const
{
  int i = elem_(face,0);
  //  int j = elem(face,1);
  double flux;
  if (i != -1)
    flux = la_cl.flux_impose(face-num1)*surface(face);
  else
    flux = -la_cl.flux_impose(face-num1)*surface(face);

  return flux;
}


//// flux_face avec Neumann_paroi_adiabatique
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& , int ,
                                                      const Neumann_paroi_adiabatique&, int ) const
{
  return 0;
}

//// coeffs_face avec Neumann_paroi_adiabatique
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int , int,
                                                      const Neumann_paroi_adiabatique&,
                                                      double&, double&) const
{
  ;
}

//// secmem_face avec Neumann_paroi_adiabatique
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Neumann_paroi_adiabatique&,
                                                        int ) const
{
  return 0;
}

//// flux_face avec Neumann_sortie_libre
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& , int  ,
                                                      const Neumann_sortie_libre& , int  ) const
{
  return 0;
}

//// coeffs_face avec Neumann_sortie_libre
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int ,int,const Neumann_sortie_libre& ,
                                                      double&, double&) const
{
  ;
}

//// secmem_face avec Neumann_sortie_libre
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Neumann_sortie_libre& ,
                                                        int ) const
{
  return 0;
}


//// flux_face avec Symetrie
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab&, int ,
                                                      const Symetrie&, int ) const
{
  return 0;
}

//// coeffs_face avec Symetrie
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int ,int, const Symetrie&, double&, double& ) const
{
  ;
}

//// secmem_face avec Symetrie
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Symetrie&, int ) const
{
  return 0;
}

//// flux_face avec Periodique
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int face,
                                                      const Periodique& la_cl, int ) const
{
  double flux;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,n0,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,n1,la_cl.distance());
  double heq;
  int ori = orientation(face);
  heq=1./(d0/dt_diffusivite(n0,ori) + d1/dt_diffusivite(n1,ori));
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

//// coeffs_face avec Periodique

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face,int,
                                                      const Periodique& la_cl,
                                                      double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  double heq;
  int ori = orientation(face);
  heq=1./(d0/dt_diffusivite(i,ori) + d1/dt_diffusivite(j,ori));
  aii = ajj = heq*surface(face)*porosite(face);
}

//// secmem_face avec Periodique
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Periodique&,
                                                        int ) const
{
  return 0;
}



//// flux_face avec NSCBC
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco , int faceD ,
                                                      const NSCBC& la_cl , int ndeb ) const
{
  // On evalue le flux au bord a partir des premieres mailles internes
  const Zone_VDF& zvdf = la_zone;
  int oriD = zvdf.orientation(faceD);
  int faceC;
  int elem3;

  int elem0 = zvdf.face_voisins(faceD,0);
  //double signe = -1.;
  if (elem0 == -1)
    {
      elem0 = zvdf.face_voisins(faceD,1);
      faceC = zvdf.elem_faces(elem0,oriD+dimension);
      elem3 = zvdf.face_voisins(faceC,1);
      //  signe = 1.;
    }
  else
    {
      faceC = zvdf.elem_faces(elem0,oriD);
      elem3 = zvdf.face_voisins(faceC,0);
    }

  assert(faceC != faceD);
  assert(elem3 != elem0);


  double d0 = zvdf.xp(elem0,orientation(faceD)) - zvdf.xp(elem3,orientation(faceD));
  double heq = (dt_diffusivite(elem0,oriD)/d0);
  double flux = heq*(inco(elem0) - inco(elem3))*surface(faceD)*porosite(faceD);

  return flux;
}


//// coeffs_face avec NSCBC
//
inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face, int num1,const NSCBC& la_cl,
                                                      double& aii, double& ajj) const
{
  /*  Cerr<<"Eval_Diff_VDF_var_Elem::coeffs_face n'est pas codee"<<finl;
      Cerr<<"pour la condition NSCBC"<<finl;
      exit();*/
  aii = ajj = 0.;
}

//// secmem_face avec NSCBC
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const NSCBC& ,
                                                        int ) const
{
  /*  Cerr<<"Eval_Diff_VDF_var_Elem::coeffs_face n'est pas codee"<<finl;
      Cerr<<"pour la condition NSCBC"<<finl;
      exit();*/
  return 0;
}



//// flux_faces_interne
//

inline double Eval_Diff_VDF_var_Elem_aniso::flux_faces_interne(const DoubleTab& inco, int face) const
{
  double flux;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->dist_face_elem0(face,n0);
  double d1 = la_zone->dist_face_elem1(face,n1);
  int ori = orientation(face);
  double heq;
  if (dt_diffusivite(n0,ori)==0.0 && dt_diffusivite(n1,ori)==0.0)
    {
      heq = 0.;
    }
  else
    {
      assert(dt_diffusivite(n0,ori)!=0.0);
      assert(dt_diffusivite(n1,ori)!=0.0);
      heq=1./(d0/dt_diffusivite(n0,ori) + d1/dt_diffusivite(n1,ori));
    }
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

//// coeffs_faces_interne

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_faces_interne(int face, double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->dist_face_elem0(face,i);
  double d1 = la_zone->dist_face_elem1(face,j);
  int ori = orientation(face);
  double heq;
  if (dt_diffusivite(i,ori)==0.0 && dt_diffusivite(j,ori)==0.0)
    {
      heq = 0.;
    }
  else
    {
      assert(dt_diffusivite(i,ori)!=0.0);
      assert(dt_diffusivite(j,ori)!=0.0);
      heq=1./(d0/dt_diffusivite(i,ori) + d1/dt_diffusivite(j,ori));
    }
  aii = ajj = heq*surface(face)*porosite(face);
}


//// secmem_faces_interne
//

inline double Eval_Diff_VDF_var_Elem_aniso::secmem_faces_interne( int ) const
{
  return 0;
}




//#include <Vect_corps_base_inut.h>


//// flux_face avec Dirichlet_entree_fluide
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int face,
                                                    const Dirichlet_entree_fluide& la_cl,
                                                    int num1,DoubleVect& flux) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Dirichlet_entree_fluide
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face,int, const Dirichlet_entree_fluide& la_cl,DoubleVect& aii, DoubleVect& ajj) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Dirichlet_entree_fluide
//

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,  int num1,DoubleVect& flux) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// flux_face avec Dirichlet_paroi_defilante
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab&, int ,
                                                    const Dirichlet_paroi_defilante&,
                                                    int, DoubleVect&  ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Dirichlet_paroi_defilante
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int , int,
                                                      const Dirichlet_paroi_defilante&,
                                                      DoubleVect&, DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Dirichlet_paroi_defilante
//

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Dirichlet_paroi_defilante&,
                                                      int, DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}


//// flux_face avec Dirichlet_paroi_fixe
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab&, int ,
                                                    const Dirichlet_paroi_fixe&,
                                                    int, DoubleVect&  ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Dirichlet_paroi_fixe
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int,int, const Dirichlet_paroi_fixe&,
                                                      DoubleVect&, DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Dirichlet_paroi_fixe
//

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Dirichlet_paroi_fixe&,
                                                      int, DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}


//// flux_face avec Echange_externe_impose
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                    const Echange_externe_impose& la_cl,
                                                    int num1,DoubleVect& flux) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Echange_externe_impose
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                      const Echange_externe_impose& la_cl,
                                                      DoubleVect& aii, DoubleVect& ajj) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                                      int num1,DoubleVect& flux) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// flux_face avec Echange_global_impose
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int face ,
                                                    const Echange_global_impose& la_cl,
                                                    int num1,DoubleVect& flux) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Echange_global_impose
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face,int num1,  const Echange_global_impose& la_cl, DoubleVect& aii, DoubleVect& ajj ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();


}

//// secmem_face avec Echange_global_impose
//

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_face(int face, const Echange_global_impose& la_cl,
                                                      int num1,DoubleVect& flux) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}


//// flux_face avec Neumann_paroi
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& , int face,
                                                    const Neumann_paroi& la_cl,
                                                    int num1,DoubleVect& flux) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Neumann_paroi
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int ,int,
                                                      const Neumann_paroi& ,
                                                      DoubleVect& , DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Neumann_paroi
//

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_face(int face, const Neumann_paroi& la_cl,
                                                      int num1, DoubleVect& flux) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();
}


//// flux_face avec Neumann_paroi_adiabatique
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& , int ,
                                                    const Neumann_paroi_adiabatique&,
                                                    int ,DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Neumann_paroi_adiabatique
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int ,int,
                                                      const Neumann_paroi_adiabatique&,
                                                      DoubleVect&, DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// secmem_face avec Neumann_paroi_adiabatique
//

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Neumann_paroi_adiabatique&,
                                                      int, DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// flux_face avec Neumann_sortie_libre
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& , int ,
                                                    const Neumann_sortie_libre& ,
                                                    int ,DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// coeffs_face avec Neumann_sortie_libre
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int,int, const Neumann_sortie_libre& ,
                                                      DoubleVect&, DoubleVect&) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Neumann_sortie_libre
//

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Neumann_sortie_libre& ,
                                                      int , DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// flux_face avec Symetrie
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab&, int ,
                                                    const Symetrie&,
                                                    int ,DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// coeffs_face avec Symetrie
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int, int,const Symetrie&,
                                                      DoubleVect&, DoubleVect&) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// secmem_face avec Symetrie
//

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Symetrie&,
                                                      int, DoubleVect&) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// flux_face avec Periodique
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_face(const DoubleTab& inco, int face,
                                                    const Periodique& la_cl,
                                                    int , DoubleVect& flux) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_face avec Periodique
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_face(int face,int, const Periodique& la_cl,
                                                      DoubleVect& aii, DoubleVect& ajj ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}


//// secmem_face avec Periodique
//

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_face(int, const Periodique&,
                                                      int, DoubleVect&) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

//// flux_faces_interne
//

inline void Eval_Diff_VDF_var_Elem_aniso::flux_faces_interne(const DoubleTab& inco,
                                                             int face,DoubleVect& flux) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}

//// coeffs_faces_interne
//

inline void Eval_Diff_VDF_var_Elem_aniso::coeffs_faces_interne(int face, DoubleVect& aii, DoubleVect& ajj ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();

}


//// secmem_faces_interne
//

inline void Eval_Diff_VDF_var_Elem_aniso::secmem_faces_interne( int, DoubleVect& ) const
{
  Cerr<<" Eval_Diff_VDF_var_Elem_aniso vectorielle "<<finl;
  assert(0);
  Process::exit();
  ;
}

#endif
