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
// File:        Eval_Diff_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/New_eval
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Diff_VDF_Elem_included
#define Eval_Diff_VDF_Elem_included

#include <Evaluateur_VDF.h>
#include <Eval_VDF_Elem2.h>

//
// .DESCRIPTION class Eval_Diff_VDF_Elem
//

template <typename DERIVED_T>
class Eval_Diff_VDF_Elem : public Eval_VDF_Elem2, public Evaluateur_VDF
{
public:

  // These static constexpr bools & Methods that will generalize the implementation
  // almost for all operator classes
  // The previous MACRO methodology do not exist anymore
  // (previously MULTD /  D_AXI / DEQUIV / MODIF_DEQ / ISQUASI)...
  // See their generic declaration later & override them in the derived class if
  // needed to stay coherent with the previous macros
  static constexpr bool Is_Multd = true;
  static constexpr bool Is_Axi = false;
  static constexpr bool Is_Dequiv = false;
  static constexpr bool Is_Modif_Deq = false;
  static constexpr bool Is_Quasi = false;

  inline double Dist_norm_bord (int face) const;
  inline double Dist_face_elem0(int face, int n0) const;
  inline double Dist_face_elem1(int face, int n1) const;
  inline double Dist_norm_bord_externe_VEC(int boundary_index, int global_face, int local_face) const;
  inline double Dist_norm_bord_externe_(int global_face) const;

  // CRTP pattern to static_cast the appropriate class and get the implementation
  // This is magic !
  inline double nu_1(int i, int compo=0) const;
  inline double nu_2(int i, int compo=0) const;
  inline double compute_heq(double, int, double, int, int compo=0) const;

  inline int calculer_flux_faces_echange_externe_impose() const
  {
    return 1;
  }
  inline int calculer_flux_faces_echange_global_impose() const
  {
    return 1;
  }
  inline int calculer_flux_faces_entree_fluide() const
  {
    return 1;
  }
  inline int calculer_flux_faces_paroi() const
  {
    return 1;
  }
  inline int calculer_flux_faces_paroi_adiabatique() const
  {
    return 0;
  }
  inline int calculer_flux_faces_paroi_defilante() const
  {
    return 0;
  }
  inline int calculer_flux_faces_paroi_fixe() const
  {
    return 1;
  }
  inline int calculer_flux_faces_sortie_libre() const
  {
    return 0;
  }
  inline int calculer_flux_faces_symetrie() const
  {
    return 0;
  }
  inline int calculer_flux_faces_periodique() const
  {
    return 1;
  }
  inline int calculer_flux_faces_NSCBC() const
  {
    return 0;
  }

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles sont de type double et renvoient le flux

  inline double flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_paroi&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_paroi_adiabatique&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Symetrie&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Periodique&, int ) const;
  inline double flux_face(const DoubleTab&, int , const NSCBC&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Echange_global_impose&, int ) const;

  inline double flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const;

  inline double flux_faces_interne(const DoubleTab&, int ) const;

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&,      int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&,   int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_paroi&,             int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_paroi_adiabatique&, int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_sortie_libre&,  int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Symetrie&,              int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Periodique&,            int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Echange_global_impose&,     int, DoubleVect& flux) const;

  inline void flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&,   int, DoubleVect& flux) const;

  inline void flux_faces_interne(const DoubleTab&, int ,
                                 DoubleVect& flux) const;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  inline void coeffs_face(int,int, const Dirichlet_paroi_fixe&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_paroi_defilante&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi_adiabatique&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_sortie_libre&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Symetrie&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Periodique&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const NSCBC&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Echange_global_impose&, double& aii, double& ajj ) const;

  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, double& aii, double& ajj ) const;

  inline void coeffs_faces_interne(int, double& aii, double& ajj ) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l"implicite pour les grandeurs scalaires.

  inline double secmem_face(int, const Dirichlet_paroi_fixe&, int ) const;
  inline double secmem_face(int, const Dirichlet_entree_fluide&, int ) const;
  inline double secmem_face(int, const Dirichlet_paroi_defilante&, int ) const;
  inline double secmem_face(int, const Neumann_paroi&, int ) const;
  inline double secmem_face(int, const Neumann_paroi_adiabatique&, int ) const;
  inline double secmem_face(int, const Neumann_sortie_libre&, int ) const;
  inline double secmem_face(int, const Symetrie&, int ) const;
  inline double secmem_face(int, const Periodique&, int ) const;
  inline double secmem_face(int, const NSCBC&, int ) const;
  inline double secmem_face(int, const Echange_global_impose&, int ) const;

  inline double secmem_face(int, int, int, const Echange_externe_impose&, int ) const;

  inline double secmem_faces_interne(int ) const;


  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  inline void coeffs_face(int, int,const Dirichlet_paroi_fixe&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_paroi_defilante&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_paroi_adiabatique&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Symetrie&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Periodique&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Echange_global_impose&, DoubleVect& aii, DoubleVect& ajj ) const;

  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, DoubleVect& aii, DoubleVect& ajj ) const;

  inline void coeffs_faces_interne(int, DoubleVect& aii, DoubleVect& ajj ) const;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l"implicite pour les grandeurs vectorielles.

  inline void secmem_face(int, const Dirichlet_paroi_fixe&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_entree_fluide&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_paroi_defilante&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Neumann_paroi&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Neumann_paroi_adiabatique&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Neumann_sortie_libre&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Symetrie&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Periodique&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Echange_global_impose&, int, DoubleVect& ) const;

  inline void secmem_face(int, int, int, const Echange_externe_impose&, int, DoubleVect& ) const;

  inline void secmem_faces_interne(int, DoubleVect& ) const;
};

/*
 * XXX XXX XXX : Discussion
 *
 * Class template specializations are required for several classes because the implementation
 *  is not the same
 *
 * 2 approaches:
 * 	1- put the Class template specializations in this file with the generic ones
 * 	2- put the Class template specializations in the derived classes
 *
 * We follow the 2nd approach. It is much easier to read, to debug and to compile !
 *
 */

//************************
// CRTP pattern
//************************
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::nu_1(int i, int compo) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_1_impl(i, compo);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::nu_2(int i, int compo) const
{
  double nu = static_cast<const DERIVED_T *>(this)->nu_2_impl(i, compo);
  return nu;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::compute_heq(double d0, int i0, double d1, int i1, int compo) const
{
  double heq = static_cast<const DERIVED_T *>(this)->compute_heq_impl(d0, i0, d1, i1, compo);
  return heq;
}


//************************
// Replaced the MACROS
//************************

/*
 * Notes MACRO (just to understand when,where and why (not sure of why :) )
 *
 * - MULTD :
 *    TRUST : predefined for Diff/Dift const_Elem / var_Elem / var_Elem_aniso (not Multi-Inco)
 *    TrioCFD : predefined for Diff/Dift K_Eps_var_VDF_Elem & K_Eps_Bas_Re_VDF_var_Elem
 *
 * - D_AXI :
 *    TRUST : predefined for Diff/Dift Axi operators
 *    TrioCFD : predefined for Diff/Dift Axi operators K_Eps
 *
 * - DEQUIV
 *    TRUST : predefined for Dift (not Diff) Multi_inco_const_Elem / Multi_inco_var_Elem &&
 *        Axi operators const_Elem_Axi / var_Elem_Axi / Multi_inco_var_Elem_Axi / Multi_inco_const_Elem_Axi
 *
 * - MODIF_DEQ
 *    TRUST : predefined for Dift (not Diff) const_Elem / var_Elem (no Axi, no Multi_Inco)
 *
 * - ISQUASI
 *    TrioCFD : predefined Diff_K_Eps_QC_VDF_Elem && Diff_K_Eps_QC_var_VDF_Elem
 *
 */

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::Dist_norm_bord (int face) const
{
  double val = DERIVED_T::Is_Axi ? la_zone->dist_norm_bord_axi(face) : la_zone->dist_norm_bord(face);
  if (DERIVED_T::Is_Multd)
    return val;
  else
    return 2*val;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::Dist_face_elem0(int face, int n0) const
{
  double val = DERIVED_T::Is_Axi ? la_zone->dist_face_elem0_axi(face,n0): la_zone->dist_face_elem0(face,n0);
  return val;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::Dist_face_elem1(int face, int n1) const
{
  double val = DERIVED_T::Is_Axi ? la_zone->dist_face_elem1_axi(face,n1): la_zone->dist_face_elem1(face,n1);
  return val;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::Dist_norm_bord_externe_VEC(int boundary_index, int global_face, int local_face) const
{
  // TODO : FIXME : to use later when Dift follows the template procedure...
//	if (DERIVED_T::Is_Dequiv)
//		return la_zone->equivalent_distance[boundary_index](local_face);
//	else
//		return DERIVED_T::Is_Axi ? la_zone->dist_norm_bord_axi(global_face) : la_zone->dist_norm_bord(global_face);
  assert (!DERIVED_T::Is_Dequiv);
  return DERIVED_T::Is_Axi ? la_zone->dist_norm_bord_axi(global_face) : la_zone->dist_norm_bord(global_face);
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::Dist_norm_bord_externe_(int global_face) const
{
  assert (!DERIVED_T::Is_Dequiv);
  return DERIVED_T::Is_Axi ? la_zone->dist_norm_bord_axi(global_face) : la_zone->dist_norm_bord(global_face);
}

/**************************************************
 * SCALAIRE
 *************************************************/

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab&, int ,
                                                       const Dirichlet_paroi_defilante&, int ) const
{
  return 0;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int , int,
                                                       const Dirichlet_paroi_defilante&,
                                                       double&, double& ) const
{
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Dirichlet_paroi_defilante&,
                                                         int ) const
{
  return 0;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab&, int ,
                                                       const Dirichlet_paroi_fixe&,
                                                       int ) const
{
  return 0;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int ,int,
                                                       const Dirichlet_paroi_fixe&,
                                                       double&, double& ) const
{
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Dirichlet_paroi_fixe&,
                                                         int ) const
{
  return 0;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                       const Dirichlet_entree_fluide& la_cl,
                                                       int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  // Olga avait mis :
  // double dist = 2*Dist_norm_bord(face);
  // Pierre dit que :
  double dist = Dist_norm_bord(face);
  double flux;
  if (n0 != -1)
    {
      flux = (T_imp-inco[n0])*surface(face)/dist;
      flux *= (porosite(face)*nu_1(n0));
    }
  else   // n1 != -1
    {
      flux = (inco[n1]-T_imp)*surface(face)/dist;
      flux*= (porosite(face)*nu_1(n1));
    }
  return flux;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face, int,
                                                       const Dirichlet_entree_fluide& la_cl,
                                                       double& aii, double& ajj) const
{
  int i = elem_(face,0);
  //  int j = elem(face,1);
  double dist = Dist_norm_bord(face);
  if (i != -1)
    {
      aii =  porosite(face)*nu_1(i)*surface(face)/dist;
      ajj = 0;
    }
  else   // j != -1
    {
      ajj =  porosite(face)*nu_1(elem_(face,1))*surface(face)/dist;
      aii = 0;
    }
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                                         int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  int i = elem_(face,0);
  //  int j = elem(face,1);
  double dist = Dist_norm_bord(face);

  double flux;
  if (i != -1)
    {
      flux = T_imp*surface(face)/dist;
      flux *= porosite(face)*nu_1(i);
    }
  else   // j != -1
    {
      flux = -T_imp*surface(face)/dist;
      flux *= (porosite(face)*nu_1(elem_(face,1)));
    }
  return flux;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                       const Echange_externe_impose& la_cl,
                                                       int num1) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv;
  double h_imp = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  double e;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
//  Dist_norm_bord_externe(boundary_index,face,local_face);
  if (DERIVED_T::Is_Modif_Deq)
    {
      // TODO : FIXME : to implement when opp dift follow the procedure
      Cerr << "Op_Dift are not templatized yet !" << finl;
      Process::exit();
//      if (ind_Fluctu_Term==1)
//        e=Dist_norm_bord_externe_(face) ;
//      else
//        e=equivalent_distance[boundary_index](local_face);
//      //e=d_equiv(face-la_zone->premiere_face_bord());
    }
  else
    {
      if (DERIVED_T::Is_Dequiv)
        {
          // TODO : FIXME : to implement when opp dift follow the procedure
          Cerr << "Op_Dift is not templatized yet !" << finl;
          Process::exit();
//          e=equivalent_distance[boundary_index](local_face);
//          //  e=d_equiv(face-la_zone->premiere_face_bord());
        }
      else
        e=Dist_norm_bord_externe_(face);
    }


  //  int ori = orientation(face);
  double flux;
  if (n0 != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(n0,ori);
      h_total_inv = 1/h_imp + e/nu_2(n0);
      flux = (T_ext - inco[n0])*surface(face)/h_total_inv;
    }
  else
    {
      //e = la_zone->xp(n1,ori) - la_zone->xv(face,ori);
      h_total_inv = 1/h_imp + e/nu_2(n1);
      flux = (inco[n1] - T_ext)*surface(face)/h_total_inv;
    }
  return flux;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                       const Echange_externe_impose& la_cl,
                                                       double& aii, double& ajj) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv;
  double h_imp = la_cl.h_imp(face-num1);
  double e;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  //  int ori = orientation(face);
//  Dist_norm_bord_externe(boundary_index,face,local_face);

  if (DERIVED_T::Is_Modif_Deq)
    {
      // TODO : FIXME : to implement when opp dift follow the procedure
      Cerr << "Op_Dift are not templatized yet !" << finl;
      Process::exit();
//      if (ind_Fluctu_Term==1)
//        e=Dist_norm_bord_externe_(face) ;
//      else
//        e=equivalent_distance[boundary_index](local_face);
//      //e=d_equiv(face-la_zone->premiere_face_bord());
    }
  else
    {
      if (DERIVED_T::Is_Dequiv)
        {
          // TODO : FIXME : to implement when opp dift follow the procedure
          Cerr << "Op_Dift is not templatized yet !" << finl;
          Process::exit();
//          e=equivalent_distance[boundary_index](local_face);
//          //  e=d_equiv(face-la_zone->premiere_face_bord());
        }
      else
        e=Dist_norm_bord_externe_(face);
    }

  if (i != -1)
    {
      // e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      h_total_inv =  1/h_imp + e/nu_2(i);
      aii = surface(face)/h_total_inv;
      ajj = 0;
    }
  else
    {
      // e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      h_total_inv =  1/h_imp + e/nu_2(elem_(face,1));
      ajj = surface(face)/h_total_inv;
      aii = 0;
    }
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int boundary_index,int face, int local_face, const Echange_externe_impose& la_cl,
                                                         int num1) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double h_total_inv;
  double h_imp = la_cl.h_imp(face-num1);
  double T_ext = la_cl.T_ext(face-num1);
  double e;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  //  int ori = orientation(face);
//  Dist_norm_bord_externe(boundary_index,face,local_face);

  if (DERIVED_T::Is_Modif_Deq)
    {
      // TODO : FIXME : to implement when opp dift follow the procedure
      Cerr << "Op_Dift are not templatized yet !" << finl;
      Process::exit();
//      if (ind_Fluctu_Term==1)
//        e=Dist_norm_bord_externe_(face) ;
//      else
//        e=equivalent_distance[boundary_index](local_face);
//      //e=d_equiv(face-la_zone->premiere_face_bord());
    }
  else
    {
      if (DERIVED_T::Is_Dequiv)
        {
          // TODO : FIXME : to implement when opp dift follow the procedure
          Cerr << "Op_Dift is not templatized yet !" << finl;
          Process::exit();
//          e=equivalent_distance[boundary_index](local_face);
//          //  e=d_equiv(face-la_zone->premiere_face_bord());
        }
      else
        e=Dist_norm_bord_externe_(face);
    }

  double flux;
  if (i != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      h_total_inv = 1/h_imp + e/nu_2(i);
      flux = T_ext*surface(face)/h_total_inv;
    }
  else
    {
      //e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      h_total_inv = 1/h_imp + e/nu_2(elem_(face,1));
      flux = - T_ext*surface(face)/h_total_inv;
    }
  return flux;
}




template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face ,
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

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face, int num1,
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

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Echange_global_impose& la_cl,
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

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , int face,
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

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int , int,
                                                       const Neumann_paroi& ,
                                                       double& aii, double& ajj) const
{
  aii = ajj = 0;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Neumann_paroi& la_cl,
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

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , int ,
                                                       const Neumann_paroi_adiabatique&, int ) const
{
  return 0;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int , int,
                                                       const Neumann_paroi_adiabatique&,
                                                       double&, double&) const
{
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Neumann_paroi_adiabatique&,
                                                         int ) const
{
  return 0;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , int  ,
                                                       const Neumann_sortie_libre& , int  ) const
{
  return 0;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int ,int,const Neumann_sortie_libre& ,
                                                       double&, double&) const
{
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Neumann_sortie_libre& ,
                                                         int ) const
{
  return 0;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab&, int ,
                                                       const Symetrie&, int ) const
{
  return 0;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int ,int, const Symetrie&, double&, double& ) const
{
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Symetrie&, int ) const
{
  return 0;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                       const Periodique& la_cl, int ) const
{
  double flux;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,n0,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,n1,la_cl.distance());
  double heq = compute_heq(d0,n0,d1,n1);
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int,
                                                       const Periodique& la_cl,
                                                       double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  double heq = compute_heq(d0,i,d1,j);
  aii = ajj = heq*surface(face)*porosite(face);
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Periodique&,
                                                         int ) const
{
  return 0;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco , int faceD ,
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
  double heq = (nu_1(elem0)/d0);
  double flux = heq*(inco(elem0) - inco(elem3))*surface(faceD)*porosite(faceD);

  return flux;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face, int num1,const NSCBC& la_cl,
                                                       double& aii, double& ajj) const
{
  /*  Cerr<<"Eval_Diff_VDF_var_Elem::coeffs_face n'est pas codee"<<finl;
      Cerr<<"pour la condition NSCBC"<<finl;
      exit();*/
  aii = ajj = 0.;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const NSCBC& ,  int ) const
{
  /*  Cerr<<"Eval_Diff_VDF_var_Elem::coeffs_face n'est pas codee"<<finl;
      Cerr<<"pour la condition NSCBC"<<finl;
      exit();*/
  return 0;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_faces_interne(const DoubleTab& inco, int face) const
{
  double flux;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = Dist_face_elem0(face,n0);
  double d1 = Dist_face_elem1(face,n1);
  assert(nu_1(n0)!=0.0);
  assert(nu_1(n1)!=0.0);
  double heq = compute_heq(d0,n0,d1,n1);
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_faces_interne(int face, double& aii, double& ajj) const
{
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = Dist_face_elem0(face,i);
  double d1 = Dist_face_elem1(face,j);
  assert(nu_1(i)!=0.0);
  assert(nu_1(j)!=0.0);
  double heq = compute_heq(d0,i,d1,j);
  aii = ajj = heq*surface(face)*porosite(face);
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_faces_interne( int ) const
{
  return 0;
}

/**************************************************
 * VECTORIEL
 *************************************************/
// Fonctions de calcul des flux pour une grandeur vectorielle

//// flux_face avec Dirichlet_entree_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                     const Dirichlet_entree_fluide& la_cl,
                                                     int num1,DoubleVect& flux) const
{
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int k;
  double dist = Dist_norm_bord(face);
  if (n0 != -1)
    {
      if (DERIVED_T::Is_Quasi)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //        for (k=0; k<flux.size(); k++)
          //          flux(k) = (la_cl.val_imp(face-num1,k)-inco(n0,k))/dv_mvol(n0)
          //                    *surface(face)*porosite(face)*nu_1(n0,k)/dist;
        }
      else
        {
          for (k=0; k<flux.size(); k++)
            flux(k) = (la_cl.val_imp(face-num1,k)-inco(n0,k))
                      *surface(face)*porosite(face)*nu_1(n0,k)/dist;
        }
    }
  else  // n1 != -1
    {
      if (DERIVED_T::Is_Quasi)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //        for (k=0; k<flux.size(); k++)
          //          flux(k) = (inco(n1,k)-la_cl.val_imp(face-num1,k))/dv_mvol(n1)
          //                    *surface(face)*porosite(face)*nu_1(n1,k)/dist;
        }
      else
        {
          for (k=0; k<flux.size(); k++)
            flux(k) = (inco(n1,k)-la_cl.val_imp(face-num1,k))
                      *surface(face)*porosite(face)*nu_1(n1,k)/dist;
        }
    }
}

//// coeffs_face avec Dirichlet_entree_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int, const Dirichlet_entree_fluide& la_cl,DoubleVect& aii, DoubleVect& ajj) const
{
  int k;
  int i = elem_(face,0);
  // int j = elem(face,1);
  double dist = Dist_norm_bord(face);

  if (i != -1)
    {
      for (k=0; k<aii.size(); k++)
        aii(k) = surface(face)*porosite(face)*nu_1(i,k)/dist;
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else  // j != -1
    {
      for (k=0; k<ajj.size(); k++)
        ajj(k) = surface(face)*(porosite(face)*nu_1(elem_(face,1),k))/dist;
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
    }
}

//// secmem_face avec Dirichlet_entree_fluide
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,  int num1,DoubleVect& flux) const
{
  int i = elem_(face,0);
  //  int j = elem(face,1);
  int k;
  double dist = Dist_norm_bord(face);

  if (i != -1)
    for (k=0; k<flux.size(); k++)
      flux(k) = la_cl.val_imp(face-num1,k)
                *surface(face)*porosite(face)*nu_1(i,k)/dist;

  else // j != -1
    for (k=0; k<flux.size(); k++)
      flux(k) = -la_cl.val_imp(face-num1,k)
                *surface(face)*(porosite(face)*nu_1(elem_(face,1),k))/dist;
}

//// flux_face avec Dirichlet_paroi_defilante
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab&, int ,
                                                     const Dirichlet_paroi_defilante&,
                                                     int, DoubleVect&  ) const
{
  // coder dans evbasreyconst comme entree_fluide ????
  assert(0);
  Process::exit();
}

//// coeffs_face avec Dirichlet_paroi_defilante
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int , int,
                                                       const Dirichlet_paroi_defilante&,
                                                       DoubleVect&, DoubleVect& ) const
{
  // coder dans evbasreyconst comme entree_fluide ????
  assert(0);
  Process::exit();
}

//// secmem_face avec Dirichlet_paroi_defilante
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Dirichlet_paroi_defilante&,
                                                       int, DoubleVect& ) const
{
  // coder dans evbasreyconst comme entree_fluide ????
  assert(0);
  Process::exit();
}


//// flux_face avec Dirichlet_paroi_fixe
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                     const Dirichlet_paroi_fixe&,
                                                     int num1, DoubleVect& flux ) const
{
  // coder dans evbasreyconst comme entree_fluide ????
  //
  flux=0;;
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int k;
  double dist = Dist_norm_bord(face);
  if (n0 != -1)
    {
      if (DERIVED_T::Is_Quasi)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //                for (k=0; k<flux.size(); k++)
          //                  flux(k) = (-inco(n0,k))
          //                            *surface(face)*porosite(face)*nu_1(n0,k)/dv_mvol(n0)/dist;
        }
      else
        {
          for (k=0; k<flux.size(); k++)
            flux(k) = (-inco(n0,k))
                      *surface(face)*porosite(face)*nu_1(n0,k)/dist;
        }
    }
  else  // n1 != -1
    {
      if (DERIVED_T::Is_Quasi)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //                for (k=0; k<flux.size(); k++)
          //                  flux(k) = (inco(n1,k))
          //                            *surface(face)*porosite(face)*nu_1(n1,k)/dv_mvol(n1)/dist;
        }
      else
        {
          for (k=0; k<flux.size(); k++)
            flux(k) = (inco(n1,k))
                      *surface(face)*porosite(face)*nu_1(n1,k)/dist;
        }

    }
}
//// coeffs_face avec Dirichlet_paroi_fixe
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int num1, const Dirichlet_paroi_fixe&,
                                                       DoubleVect& aii , DoubleVect& ajj) const
{
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int k;
  double dist = Dist_norm_bord(face);
  if (n0 != -1)
    {

      if (DERIVED_T::Is_Quasi)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //          for (k=0; k<aii.size(); k++)
          //            aii(k) =
          //              surface(face)*porosite(face)*nu_1(n0,k)/dv_mvol(n0)/dist;
        }
      else
        {
          for (k=0; k<aii.size(); k++)
            aii(k) =
              surface(face)*porosite(face)*nu_1(n0,k)/dist;
        }


      for (k=0; k<aii.size(); k++)
        ajj(k) =0;
    }

  else  // n1 != -1
    {
      if (n1==-1) Process::exit();

      if (DERIVED_T::Is_Quasi)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //          for (k=0; k<ajj.size(); k++)
          //            ajj(k) =
          //              surface(face)*porosite(face)*nu_1(n1,k)/dv_mvol(n1)/dist;
        }
      else
        {
          for (k=0; k<ajj.size(); k++)
            ajj(k) =
              surface(face)*porosite(face)*nu_1(n1,k)/dist;
        }

      for (k=0; k<ajj.size(); k++)
        aii(k) =0.;

    }
}

//// secmem_face avec Dirichlet_paroi_fixe
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Dirichlet_paroi_fixe&,
                                                       int, DoubleVect& flux) const
{
  // coder dans evbasreyconst comme entree_fluide ????
  flux=0  ;
}


//// flux_face avec Echange_externe_impose
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                     const Echange_externe_impose& la_cl,
                                                     int num1,DoubleVect& flux) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  if (DERIVED_T::Is_Quasi)
    {
      Cerr<<__FILE__<< " QC BORD non code ligne "<<(int)__LINE__<<finl;
      Process::exit();
    }

  double h_total_inv,e;
  int k;
  //  int ori = orientation(face);
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  e=Dist_norm_bord_externe_VEC(boundary_index,face,local_face);
  if (n0 != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(n0,ori);
      for (k=0; k<flux.size(); k++)
        {
          h_total_inv = 1/la_cl.h_imp(face-num1,k) + e/nu_2(n0,k);
          flux(k) = (la_cl.T_ext(face-num1,k) - inco(n0,k))*surface(face)/h_total_inv;
        }
    }
  else
    {
      //    e = la_zone->xp(n1,ori) - la_zone->xv(face,ori);
      for (k=0; k<flux.size(); k++)
        {
          h_total_inv = 1/la_cl.h_imp(face-num1,k) + e/nu_2(n1,k);
          flux(k) = (inco(n1,k) - la_cl.T_ext(face-num1,k))*surface(face)/h_total_inv;
        }
    }
}

//// coeffs_face avec Echange_externe_impose
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                       const Echange_externe_impose& la_cl,
                                                       DoubleVect& aii, DoubleVect& ajj) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double e,h_total_inv;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  //  int ori = orientation(face);
  int k;
  e=Dist_norm_bord_externe_VEC(boundary_index, face, local_face);
  if (i != -1)
    {
      //      e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      for (k=0; k<aii.size(); k++)
        {
          h_total_inv =  1/la_cl.h_imp(face-num1,k) + e/nu_2(i,k);
          aii(k) = surface(face)/h_total_inv;
        }
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      //e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      for (k=0; k<ajj.size(); k++)
        {
          h_total_inv = 1/la_cl.h_imp(face-num1,k) + e/nu_2(elem_(face,1),k);
          ajj(k) = surface(face)/h_total_inv;
        }
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
    }
}

//// secmem
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                                       int num1,DoubleVect& flux) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite

  double e,h_total_inv;
  int k;
  double h_imp;
  int i = elem_(face,0);
  //  int j = elem(face,1);
  //  int ori = orientation(face);
  e=Dist_norm_bord_externe_VEC(boundary_index, face, local_face);
  if (i != -1)
    {
      //e = la_zone->xv(face,ori) - la_zone->xp(i,ori);
      for (k=0; k<flux.size(); k++)
        {
          h_imp = la_cl.h_imp(face-num1,k);
          h_total_inv = 1/h_imp + e/nu_2(i,k);
          flux(k) = la_cl.T_ext(face-num1,k) *surface(face)/h_total_inv;
        }
    }
  else
    {
      //e = la_zone->xp(j,ori) - la_zone->xv(face,ori);
      for (k=0; k<flux.size(); k++)
        {
          h_imp = la_cl.h_imp(face-num1,k);
          h_total_inv = 1/h_imp + e/nu_2(elem_(face,1),k);
          flux(k) =  - la_cl.T_ext(face-num1,k)*surface(face)/h_total_inv;
        }
    }
}

//// flux_face avec Echange_global_impose
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face ,
                                                     const Echange_global_impose& la_cl,
                                                     int num1,DoubleVect& flux) const
{

  if (DERIVED_T::Is_Quasi)
    {
      Cerr<<__FILE__<< " QC BORD non code ligne "<<(int)__LINE__<<finl;
      Process::exit();
    }

  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  int k;
  if (n0 != -1)
    for (k=0; k<flux.size(); k++)
      flux(k) = la_cl.h_imp(face-num1,k)*(la_cl.T_ext(face-num1,k) - inco(n0,k))
                *surface(face);
  else
    for (k=0; k<flux.size(); k++)
      flux(k) = la_cl.h_imp(face-num1,k)*(inco(n1,k) - la_cl.T_ext(face-num1,k))
                *surface(face);
}

//// coeffs_face avec Echange_global_impose
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int num1,  const Echange_global_impose& la_cl, DoubleVect& aii, DoubleVect& ajj ) const
{
  int i = elem_(face,0);
  //  int j = elem(face,1);
  int k;
  if (i != -1)
    {
      for (k=0; k<aii.size(); k++)
        aii(k) = la_cl.h_imp(face-num1,k)*surface(face);
      for (k=0; k<ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      for (k=0; k<ajj.size(); k++)
        ajj(k) = la_cl.h_imp(face-num1,k)*surface(face);
      for (k=0; k<aii.size(); k++)
        aii(k) = 0;
    }
}

//// secmem_face avec Echange_global_impose
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Echange_global_impose& la_cl,
                                                       int num1,DoubleVect& flux) const
{
  int k;
  double h;
  int i = elem_(face,0);
  //int j = elem(face,1);
  if (i != -1)
    for (k=0; k<flux.size(); k++)
      {
        h = la_cl.h_imp(face-num1,k);
        flux(k) = h*la_cl.T_ext(face-num1,k)*surface(face);
      }
  else
    for (k=0; k<flux.size(); k++)
      {
        h = la_cl.h_imp(face-num1,k);
        flux(k) = -h*la_cl.T_ext(face-num1,k)*surface(face);
      }
}


//// flux_face avec Neumann_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , int face,
                                                     const Neumann_paroi& la_cl,
                                                     int num1,DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = la_cl.flux_impose(face-num1,k)*surface(face);
}

//// coeffs_face avec Neumann_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int ,int,
                                                       const Neumann_paroi& ,
                                                       DoubleVect& , DoubleVect& ) const
{
  ;
}

//// secmem_face avec Neumann_paroi
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Neumann_paroi& la_cl,
                                                       int num1, DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++)
    flux(k) = la_cl.flux_impose(face-num1,k)*surface(face);
}


//// flux_face avec Neumann_paroi_adiabatique
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , int ,
                                                     const Neumann_paroi_adiabatique&,
                                                     int ,DoubleVect& ) const
{
  ;
}

//// coeffs_face avec Neumann_paroi_adiabatique
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int ,int,
                                                       const Neumann_paroi_adiabatique&,
                                                       DoubleVect&, DoubleVect& ) const
{
  ;
}

//// secmem_face avec Neumann_paroi_adiabatique
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Neumann_paroi_adiabatique&,
                                                       int, DoubleVect& ) const
{
  ;
}

//// flux_face avec Neumann_sortie_libre
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , int ,
                                                     const Neumann_sortie_libre& ,
                                                     int ,DoubleVect& flux ) const
{
  for (int k=0; k<flux.size(); k++)
    {
      if (flux(k)!=0)
        {
          Cerr<<" et OUI"<<finl;
          Process::exit();
        }
    }

}

//// coeffs_face avec Neumann_sortie_libre
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int,int, const Neumann_sortie_libre& ,
                                                       DoubleVect&, DoubleVect&) const
{
  ;
}

//// secmem_face avec Neumann_sortie_libre
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Neumann_sortie_libre& ,
                                                       int , DoubleVect& ) const
{
}

//// flux_face avec Symetrie
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab&, int ,
                                                     const Symetrie&,
                                                     int ,DoubleVect& ) const
{
}

//// coeffs_face avec Symetrie
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int, int,const Symetrie&,
                                                       DoubleVect&, DoubleVect&) const
{
}

//// secmem_face avec Symetrie
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Symetrie&,
                                                       int, DoubleVect&) const
{
}

//// flux_face avec Periodique
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                     const Periodique& la_cl,
                                                     int , DoubleVect& flux) const
{
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,n0,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,n1,la_cl.distance());
  double heq;

  if (DERIVED_T::Is_Quasi)
    {
      //TODO : implement when Diff_K_Eps_QC is templatized
      Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
      Process::exit();
      //            for (int k=0; k<flux.size(); k++)
      //              {
      //                heq = compute_heq(d0,n0,d1,n1,k);
      //                flux(k) = heq*(inco(n1,k)/dv_mvol(n1) - inco(n0,k)/dv_mvol(n0))*surface(face)*porosite(face);
      //              }
    }
  else
    {
      for (int k=0; k<flux.size(); k++)
        {
          heq = compute_heq(d0,n0,d1,n1,k);
          flux(k) = heq*(inco(n1,k) - inco(n0,k))*surface(face)*porosite(face);
        }
    }


}

//// coeffs_face avec Periodique
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int, const Periodique& la_cl,
                                                       DoubleVect& aii, DoubleVect& ajj ) const
{
  int k;
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance());
  double d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  double heq;

  for (k=0; k<aii.size(); k++)
    {
      heq = compute_heq(d0,i,d1,j,k);
      aii(k) =  heq*surface(face)*porosite(face);
    }
  for (k=0; k<ajj.size(); k++)
    {
      heq = compute_heq(d0,i,d1,j,k);
      ajj(k)=  heq*surface(face)*porosite(face);
    }
}


//// secmem_face avec Periodique
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Periodique&,
                                                       int, DoubleVect&) const
{
}


//// flux_faces_interne
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_faces_interne(const DoubleTab& inco,
                                                              int face,DoubleVect& flux) const
{
  int n0 = elem_(face,0);
  int n1 = elem_(face,1);
  double d0 = Dist_face_elem0(face,n0);
  double d1 = Dist_face_elem1(face,n1);
  double heq;
  for (int k=0; k<flux.size(); k++)
    {
      heq = compute_heq(d0,n0,d1,n1,k);

      if (DERIVED_T::Is_Quasi)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //          flux(k) = heq*(inco(n1,k)/dv_mvol(n1) - inco(n0,k)/dv_mvol(n0))*surface(face)*porosite(face);
        }
      else
        {
          flux(k) = heq*(inco(n1,k) - inco(n0,k))*surface(face)*porosite(face);
        }

    }
}

//// coeffs_faces_interne
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_faces_interne(int face, DoubleVect& aii, DoubleVect& ajj ) const
{
  int k;
  int i = elem_(face,0);
  int j = elem_(face,1);
  double d0 = Dist_face_elem0(face,i);
  double d1 = Dist_face_elem1(face,j);
  double heq;
  for ( k=0; k<aii.size(); k++)
    {
      heq = compute_heq(d0,i,d1,j,k);
      aii(k) = heq*surface(face)*porosite(face);
    }
  for ( k=0; k<ajj.size(); k++)
    {
      heq = compute_heq(d0,i,d1,j,k);
      ajj(k) = heq*surface(face)*porosite(face);
    }
}


//// secmem_faces_interne
//
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_faces_interne( int, DoubleVect& flux ) const
{
  int k;
  for ( k=0; k<flux.size(); k++)
    {
      flux(k)=0.;
    }
}

#endif /* Eval_Diff_VDF_Elem_included */
