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
// File:        Eval_Diff_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Diff
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Diff_VDF_Elem_TPP_included
#define Eval_Diff_VDF_Elem_TPP_included


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

// See generic impl in the class Eval_Diff_VDF. They will be overloaded for Dift ops
template <typename DERIVED_T>
inline int Eval_Diff_VDF_Elem<DERIVED_T>::ind_Fluctu_Term() const
{
  return static_cast<const DERIVED_T *>(this)->get_ind_Fluctu_Term();
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::equivalent_distance (int boundary_index, int local_face) const
{
  return static_cast<const DERIVED_T *>(this)->get_equivalent_distance(boundary_index,local_face);
}

//************************
// Replaced the MACROS
//************************

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::Dist_norm_bord (int face) const
{
  double val = DERIVED_T::IS_AXI ? la_zone->dist_norm_bord_axi(face) : la_zone->dist_norm_bord(face);
  return DERIVED_T::IS_MULTD ? val : 2*val;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::Dist_face_elem0(int face, int n0) const
{
  double val = DERIVED_T::IS_AXI ? la_zone->dist_face_elem0_axi(face,n0): la_zone->dist_face_elem0(face,n0);
  return val;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::Dist_face_elem1(int face, int n1) const
{
  double val = DERIVED_T::IS_AXI ? la_zone->dist_face_elem1_axi(face,n1): la_zone->dist_face_elem1(face,n1);
  return val;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::Dist_norm_bord_externe_VEC(int boundary_index, int global_face, int local_face) const
{
  if (DERIVED_T::IS_DEQUIV)
    return equivalent_distance(boundary_index,local_face);
  else
    return DERIVED_T::IS_AXI ? la_zone->dist_norm_bord_axi(global_face) : la_zone->dist_norm_bord(global_face);
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::Dist_norm_bord_externe_(int global_face) const
{
  assert (!DERIVED_T::IS_DEQUIV);
  return DERIVED_T::IS_AXI ? la_zone->dist_norm_bord_axi(global_face) : la_zone->dist_norm_bord(global_face);
}

//************************
// CAS SCALAIRE
//************************

/* Function templates specialization for BC Dirichlet_entree_fluide */
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face, const Dirichlet_entree_fluide& la_cl, int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  const int n0 = elem_(face,0), n1 = elem_(face,1), ori = DERIVED_T::IS_ANISO ? orientation(face) : 0;
  // Olga avait mis : double dist = 2*Dist_norm_bord(face);
  // Pierre dit que :
  double dist = Dist_norm_bord(face), flux;
  if (n0 != -1)
    {
      flux = (T_imp-inco[n0])*surface(face)/dist;
      flux *= (porosite(face)*nu_1(n0,ori));
    }
  else   // n1 != -1
    {
      flux = (inco[n1]-T_imp)*surface(face)/dist;
      flux*= (porosite(face)*nu_1(n1,ori));
    }
  return flux;
}

/* Function templates specialization for BC Dirichlet_entree_fluide */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face, int, const Dirichlet_entree_fluide& la_cl, double& aii, double& ajj) const
{
  const int i = elem_(face,0), ori = DERIVED_T::IS_ANISO ? orientation(face) : 0;
  double dist = Dist_norm_bord(face);
  if (i != -1)
    {
      aii =  porosite(face)*nu_1(i,ori)*surface(face)/dist;
      ajj = 0;
    }
  else   // j != -1
    {
      ajj =  porosite(face)*nu_1(elem_(face,1),ori)*surface(face)/dist;
      aii = 0;
    }
}

/* Function templates specialization for BC Dirichlet_entree_fluide */
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Dirichlet_entree_fluide& la_cl, int num1) const
{
  double T_imp = la_cl.val_imp(face-num1);
  const int i = elem_(face,0), ori = DERIVED_T::IS_ANISO ? orientation(face) : 0;
  double dist = Dist_norm_bord(face), flux;
  if (i != -1)
    {
      flux = T_imp*surface(face)/dist;
      flux *= porosite(face)*nu_1(i,ori);
    }
  else   // j != -1
    {
      flux = -T_imp*surface(face)/dist;
      flux *= (porosite(face)*nu_1(elem_(face,1),ori));
    }

  return flux;
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                       const Echange_externe_impose& la_cl, int num1) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite
  double h_total_inv, e,  h_imp = la_cl.h_imp(face-num1), T_ext = la_cl.T_ext(face-num1);
  const int n0 = elem_(face,0), n1 = elem_(face,1);
  //  Dist_norm_bord_externe(boundary_index,face,local_face);
  if (DERIVED_T::IS_MODIF_DEQ)
    e = ind_Fluctu_Term()==1 ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else
    e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  double flux, heq;
  const int ori = DERIVED_T::IS_ANISO ? orientation(face) : 0;

  // XXX : E. Saikali 08/03/2021
  // The test of a zero diffusion was not done before. I think it should be like that
  // See always when we use int ori = DERIVED_T::IS_ANISO....
  if (n0 != -1)
    {
      // XXX : Attention for DIFT OPERATORS : nu_2 and not nu_1 (only laminar part)
      if (nu_2(n0,ori) == 0.0) heq = h_imp; // Should be 0 ??
      else
        {
          h_total_inv = 1.0/h_imp + e/nu_2(n0,ori);
          heq = 1.0 / h_total_inv;
        }
      flux = heq*(T_ext - inco[n0])*surface(face);
    }
  else
    {
      if (nu_2(n1,ori) == 0.0) heq = h_imp; // Should be 0 ??
      else
        {
          h_total_inv = 1.0/h_imp + e/nu_2(n1,ori);
          heq = 1.0 / h_total_inv;
        }
      flux = heq*(inco[n1] - T_ext)*surface(face);
    }
  return flux;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                       const Echange_externe_impose& la_cl, double& aii, double& ajj) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite
  double h_total_inv,e,  h_imp = la_cl.h_imp(face-num1);
  const int i = elem_(face,0);

  if (DERIVED_T::IS_MODIF_DEQ)
    e = ind_Fluctu_Term()==1 ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else
    e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  const int ori = DERIVED_T::IS_ANISO ? orientation(face) : 0;
  double heq;

  if (i != -1)
    {
      if (nu_2(i,ori) == 0.0) heq = h_imp; // Should be 0 ??
      else
        {
          h_total_inv =  1.0/h_imp + e/nu_2(i,ori);
          heq = 1.0 / h_total_inv;
        }

      aii = heq*surface(face);
      ajj = 0;
    }
  else
    {
      if (nu_2(elem_(face,1),ori) == 0.0) heq = h_imp; // Should be 0 ??
      else
        {
          h_total_inv =  1.0/h_imp + e/nu_2(elem_(face,1),ori);
          heq = 1.0 / h_total_inv;
        }

      ajj = heq*surface(face);
      aii = 0;
    }
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int boundary_index,int face,int local_face,
                                                         const Echange_externe_impose& la_cl, int num1) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite
  double h_total_inv,e,  h_imp = la_cl.h_imp(face-num1), T_ext = la_cl.T_ext(face-num1);
  const int i = elem_(face,0);

  if (DERIVED_T::IS_MODIF_DEQ)
    e = ind_Fluctu_Term()==1 ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else
    e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  double flux, heq;
  const int ori = DERIVED_T::IS_ANISO ? orientation(face) : 0;

  if (i != -1)
    {
      if (nu_2(i,ori) == 0.0) heq = h_imp; // Should be 0 ??
      else
        {
          h_total_inv = 1.0/h_imp + e/nu_2(i,ori);
          heq = 1.0 / h_total_inv;
        }
      flux = heq*T_ext*surface(face);
    }
  else
    {
      if (nu_2(elem_(face,1),ori) == 0.0) heq = h_imp; // Should be 0 ??
      else
        {
          h_total_inv = 1.0/h_imp + e/nu_2(elem_(face,1),ori);
          heq = 1.0 / h_total_inv;
        }
      flux = - heq*T_ext*surface(face);
    }
  return flux;
}

/* Function templates specialization for BC Echange_global_impose */
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face ,const Echange_global_impose& la_cl, int num1) const
{
  double flux, h = la_cl.h_imp(face-num1), T_ext = la_cl.T_ext(face-num1);
  const int n0 = elem_(face,0), n1 = elem_(face,1);
  flux = (n0 != -1) ? h*(T_ext - inco[n0])*surface(face) : h*(inco[n1] - T_ext)*surface(face);
  return flux;
}

/* Function templates specialization for BC Echange_global_impose */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face, int num1, const Echange_global_impose& la_cl, double& aii, double& ajj) const
{
  double h = la_cl.h_imp(face-num1);
  const int i = elem_(face,0);
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

/* Function templates specialization for BC Echange_global_impose */
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Echange_global_impose& la_cl, int num1) const
{
  double flux, h = la_cl.h_imp(face-num1), T_ext = la_cl.T_ext(face-num1);
  const int i = elem_(face,0);
  flux = (i != -1) ? h*T_ext*surface(face) : -h*T_ext*surface(face);
  return flux;
}

/* Function templates specialization for BC Neumann_paroi */
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , int face, const Neumann_paroi& la_cl, int num1) const
{
  const int i = elem_(face,0);
  double flux =  (i != -1) ? la_cl.flux_impose(face-num1)*surface(face) : -la_cl.flux_impose(face-num1)*surface(face);
  return flux;
}

/* Function templates specialization for BC Neumann_paroi */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int , int, const Neumann_paroi& , double& aii, double& ajj) const
{
  aii = ajj = 0;
}

/* Function templates specialization for BC Neumann_paroi */
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Neumann_paroi& la_cl, int num1) const
{
  const int i = elem_(face,0);
  double flux = (i != -1) ? la_cl.flux_impose(face-num1)*surface(face) : -la_cl.flux_impose(face-num1)*surface(face);
  return flux;
}

/* Function templates specialization for BC Periodique */
template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face, const Periodique& la_cl, int ) const
{
  double flux, heq;
  const int n0 = elem_(face,0), n1 = elem_(face,1), ori = DERIVED_T::IS_ANISO ? orientation(face) : 0;
  double d0 = la_zone->dist_face_elem0_period(face,n0,la_cl.distance()), d1 = la_zone->dist_face_elem1_period(face,n1,la_cl.distance());
  heq = compute_heq(d0,n0,d1,n1,ori);
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

/* Function templates specialization for BC Periodique */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int, const Periodique& la_cl,double& aii, double& ajj) const
{
  const int i = elem_(face,0), j = elem_(face,1), ori = DERIVED_T::IS_ANISO ? orientation(face) : 0;
  double heq, d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance()), d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  heq=  compute_heq(d0,i,d1,j,ori);
  aii = ajj = heq*surface(face)*porosite(face);
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::flux_faces_interne(const DoubleTab& inco, int face) const
{
  double flux, heq;
  const int n0 = elem_(face,0), n1 = elem_(face,1), ori = DERIVED_T::IS_ANISO ? orientation(face) : 0;
  double d0 = Dist_face_elem0(face,n0), d1 = Dist_face_elem1(face,n1);

  if (nu_1(n0,ori)==0.0 && nu_1(n1,ori)==0.0) heq = 0.;
  else
    {
      assert(nu_1(n0,ori)!=0.0 && nu_1(n1,ori)!=0.0);
      heq = compute_heq(d0,n0,d1,n1,ori);
    }
  flux = heq*(inco[n1] - inco[n0])*surface(face)*porosite(face);
  return flux;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_faces_interne(int face, double& aii, double& ajj) const
{
  const int i = elem_(face,0), j = elem_(face,1), ori = DERIVED_T::IS_ANISO ? orientation(face) : 0;
  double heq, d0 = Dist_face_elem0(face,i), d1 = Dist_face_elem1(face,j);
  if (nu_1(i,ori)==0.0 && nu_2(j,ori)==0.0) heq = 0.;
  else
    {
      assert(nu_1(i,ori)!=0.0 && nu_2(j,ori)!=0.0);
      heq = compute_heq(d0,i,d1,j,ori);
    }
  aii = ajj = heq*surface(face)*porosite(face);
}

template <typename DERIVED_T>
inline double Eval_Diff_VDF_Elem<DERIVED_T>::secmem_faces_interne( int ) const
{
  return 0;
}

//************************
// CAS VECTORIEL
//************************

/* Function templates specialization for BC Dirichlet_entree_fluide */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face, const Dirichlet_entree_fluide& la_cl, int num1,DoubleVect& flux) const
{
  const int n0 = elem_(face,0), n1 = elem_(face,1);
  double dist = Dist_norm_bord(face);
  if (n0 != -1)
    {
      if (DERIVED_T::IS_QUASI)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //        for (k=0; k<flux.size(); k++)
          //          flux(k) = (la_cl.val_imp(face-num1,k)-inco(n0,k))/dv_mvol(n0)
          //                    *surface(face)*porosite(face)*nu_1(n0,k)/dist;
        }
      else
        for (int k=0; k<flux.size(); k++) flux(k) = (la_cl.val_imp(face-num1,k)-inco(n0,k))*surface(face)*porosite(face)*nu_1(n0,k)/dist;
    }
  else  // n1 != -1
    {
      if (DERIVED_T::IS_QUASI)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //        for (k=0; k<flux.size(); k++)
          //          flux(k) = (inco(n1,k)-la_cl.val_imp(face-num1,k))/dv_mvol(n1)
          //                    *surface(face)*porosite(face)*nu_1(n1,k)/dist;
        }
      else
        for (int k=0; k<flux.size(); k++) flux(k) = (inco(n1,k)-la_cl.val_imp(face-num1,k))*surface(face)*porosite(face)*nu_1(n1,k)/dist;
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int, const Dirichlet_entree_fluide& la_cl,DoubleVect& aii, DoubleVect& ajj) const
{
  const int i = elem_(face,0);
  double dist = Dist_norm_bord(face);
  if (i != -1)
    {
      for (int k=0; k<aii.size(); k++) aii(k) = surface(face)*porosite(face)*nu_1(i,k)/dist;
      for (int k=0; k<ajj.size(); k++) ajj(k) = 0;
    }
  else  // j != -1
    {
      for (int k=0; k<ajj.size(); k++) ajj(k) = surface(face)*(porosite(face)*nu_1(elem_(face,1),k))/dist;
      for (int k=0; k<aii.size(); k++) aii(k) = 0;
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,  int num1,DoubleVect& flux) const
{
  const int i = elem_(face,0);
  double dist = Dist_norm_bord(face);
  if (i != -1)
    for (int k=0; k<flux.size(); k++) flux(k) = la_cl.val_imp(face-num1,k)*surface(face)*porosite(face)*nu_1(i,k)/dist;
  else // j != -1
    for (int k=0; k<flux.size(); k++) flux(k) = -la_cl.val_imp(face-num1,k)*surface(face)*(porosite(face)*nu_1(elem_(face,1),k))/dist;
}

/* Function templates specialization for BC Dirichlet_paroi_defilante */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int, DoubleVect&  ) const
{
  // coder comme entree_fluide ????
  assert(0);
  Process::exit();
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int , int, const Dirichlet_paroi_defilante&, DoubleVect&, DoubleVect& ) const
{
  // coder comme entree_fluide ????
  assert(0);
  Process::exit();
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int, const Dirichlet_paroi_defilante&, int, DoubleVect& ) const
{
  // coder comme entree_fluide ????
  assert(0);
  Process::exit();
}

/* Function templates specialization for BC Dirichlet_paroi_fixe */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face, const Dirichlet_paroi_fixe&, int num1, DoubleVect& flux ) const
{
  flux=0;
  const int n0 = elem_(face,0), n1 = elem_(face,1);
  double dist = Dist_norm_bord(face);
  if (n0 != -1)
    {
      if (DERIVED_T::IS_QUASI)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //                for (k=0; k<flux.size(); k++)
          //                  flux(k) = (-inco(n0,k))
          //                            *surface(face)*porosite(face)*nu_1(n0,k)/dv_mvol(n0)/dist;
        }
      else
        for (int k=0; k<flux.size(); k++) flux(k) = (-inco(n0,k))*surface(face)*porosite(face)*nu_1(n0,k)/dist;
    }
  else  // n1 != -1
    {
      if (DERIVED_T::IS_QUASI)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //                for (k=0; k<flux.size(); k++)
          //                  flux(k) = (inco(n1,k))
          //                            *surface(face)*porosite(face)*nu_1(n1,k)/dv_mvol(n1)/dist;
        }
      else
        for (int k=0; k<flux.size(); k++) flux(k) = (inco(n1,k))*surface(face)*porosite(face)*nu_1(n1,k)/dist;
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int num1, const Dirichlet_paroi_fixe&, DoubleVect& aii , DoubleVect& ajj) const
{
  const int n0 = elem_(face,0), n1 = elem_(face,1);
  double dist = Dist_norm_bord(face);
  if (n0 != -1)
    {
      if (DERIVED_T::IS_QUASI)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //          for (k=0; k<aii.size(); k++)
          //            aii(k) =
          //              surface(face)*porosite(face)*nu_1(n0,k)/dv_mvol(n0)/dist;
        }
      else
        for (int k=0; k<aii.size(); k++) aii(k) = surface(face)*porosite(face)*nu_1(n0,k)/dist;

      for (int k=0; k<aii.size(); k++) ajj(k) =0;
    }
  else  // n1 != -1
    {
      if (n1==-1) Process::exit();
      if (DERIVED_T::IS_QUASI)
        {
          //TODO : implement when Diff_K_Eps_QC is templatized
          Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
          Process::exit();
          //          for (k=0; k<ajj.size(); k++)
          //            ajj(k) =
          //              surface(face)*porosite(face)*nu_1(n1,k)/dv_mvol(n1)/dist;
        }
      else
        for (int k=0; k<ajj.size(); k++) ajj(k) = surface(face)*porosite(face)*nu_1(n1,k)/dist;

      for (int k=0; k<ajj.size(); k++) aii(k) =0.;
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Dirichlet_paroi_fixe&, int, DoubleVect& flux) const
{
  // coder comme entree_fluide ????
  flux=0  ;
}

/* Function templates specialization for BC Echange_externe_impose */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face,
                                                     const Echange_externe_impose& la_cl, int num1,DoubleVect& flux) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite
  if (DERIVED_T::IS_QUASI)
    {
      Cerr<<__FILE__<< " QC BORD non code ligne "<<(int)__LINE__<<finl;
      Process::exit();
    }

  double h_total_inv,e;
  const int n0 = elem_(face,0), n1 = elem_(face,1);
  if (DERIVED_T::IS_MODIF_DEQ)
    e = ind_Fluctu_Term()==1 ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else
    e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  if (n0 != -1)
    {
      for (int k=0; k<flux.size(); k++)
        {
          const int l = DERIVED_T::IS_ANISO ? orientation(face) : k;
          h_total_inv = 1/la_cl.h_imp(face-num1,k) + e/nu_2(n0,l);
          flux(k) = (la_cl.T_ext(face-num1,k) - inco(n0,k))*surface(face)/h_total_inv;
        }
    }
  else
    {
      for (int k=0; k<flux.size(); k++)
        {
          const int l = DERIVED_T::IS_ANISO ? orientation(face) : k;
          h_total_inv = 1/la_cl.h_imp(face-num1,k) + e/nu_2(n1,l);
          flux(k) = (inco(n1,k) - la_cl.T_ext(face-num1,k))*surface(face)/h_total_inv;
        }
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int boundary_index, int face, int local_face, int num1,
                                                       const Echange_externe_impose& la_cl, DoubleVect& aii, DoubleVect& ajj) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite
  double e,h_total_inv;
  const int i = elem_(face,0);
  if (DERIVED_T::IS_MODIF_DEQ)
    e = ind_Fluctu_Term()==1 ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else
    e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  if (i != -1)
    {
      for (int k=0; k<aii.size(); k++)
        {
          const int l = DERIVED_T::IS_ANISO ? orientation(face) : k;
          h_total_inv =  1/la_cl.h_imp(face-num1,k) + e/nu_2(i,l);
          aii(k) = surface(face)/h_total_inv;
        }
      for (int k=0; k<ajj.size(); k++) ajj(k) = 0;
    }
  else
    {
      for (int k=0; k<ajj.size(); k++)
        {
          const int l = DERIVED_T::IS_ANISO ? orientation(face) : k;
          h_total_inv = 1/la_cl.h_imp(face-num1,k) + e/nu_2(elem_(face,1),l);
          ajj(k) = surface(face)/h_total_inv;
        }
      for (int k=0; k<aii.size(); k++) aii(k) = 0;
    }
}

//// secmem
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl,
                                                       int num1,DoubleVect& flux) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite)
  // La C.L fournit h_imp ; il faut calculer e/diffusivite
  double e,h_total_inv, h_imp;
  const int i = elem_(face,0);
  if (DERIVED_T::IS_MODIF_DEQ)
    e = ind_Fluctu_Term()==1 ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else
    e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  if (i != -1)
    {
      for (int k=0; k<flux.size(); k++)
        {
          h_imp = la_cl.h_imp(face-num1,k);
          h_total_inv = 1/h_imp + e/nu_2(i,k);
          flux(k) = la_cl.T_ext(face-num1,k) *surface(face)/h_total_inv;
        }
    }
  else
    {
      for (int k=0; k<flux.size(); k++)
        {
          h_imp = la_cl.h_imp(face-num1,k);
          h_total_inv = 1/h_imp + e/nu_2(elem_(face,1),k);
          flux(k) = -la_cl.T_ext(face-num1,k)*surface(face)/h_total_inv;
        }
    }
}

/* Function templates specialization for BC Echange_global_impose */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face , const Echange_global_impose& la_cl, int num1,DoubleVect& flux) const
{
  if (DERIVED_T::IS_QUASI)
    {
      Cerr<<__FILE__<< " QC BORD non code ligne "<<(int)__LINE__<<finl;
      Process::exit();
    }

  const int n0 = elem_(face,0), n1 = elem_(face,1);

  if (n0 != -1)
    for (int k=0; k<flux.size(); k++) flux(k) = la_cl.h_imp(face-num1,k)*(la_cl.T_ext(face-num1,k) - inco(n0,k))*surface(face);
  else
    for (int k=0; k<flux.size(); k++) flux(k) = la_cl.h_imp(face-num1,k)*(inco(n1,k) - la_cl.T_ext(face-num1,k))*surface(face);
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int num1,  const Echange_global_impose& la_cl, DoubleVect& aii, DoubleVect& ajj ) const
{
  const int i = elem_(face,0);
  if (i != -1)
    {
      for (int k=0; k<aii.size(); k++) aii(k) = la_cl.h_imp(face-num1,k)*surface(face);
      for (int k=0; k<ajj.size(); k++) ajj(k) = 0;
    }
  else
    {
      for (int k=0; k<ajj.size(); k++) ajj(k) = la_cl.h_imp(face-num1,k)*surface(face);
      for (int k=0; k<aii.size(); k++) aii(k) = 0;
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Echange_global_impose& la_cl, int num1,DoubleVect& flux) const
{
  double h;
  const int i = elem_(face,0);
  if (i != -1)
    for (int k=0; k<flux.size(); k++)
      {
        h = la_cl.h_imp(face-num1,k);
        flux(k) = h*la_cl.T_ext(face-num1,k)*surface(face);
      }
  else
    for (int k=0; k<flux.size(); k++)
      {
        h = la_cl.h_imp(face-num1,k);
        flux(k) = -h*la_cl.T_ext(face-num1,k)*surface(face);
      }
}

/* Function templates specialization for BC Neumann_paroi */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , int face, const Neumann_paroi& la_cl, int num1,DoubleVect& flux) const
{
  const int i = elem_(face,0);
  for (int k=0; k<flux.size(); k++) flux(k) = ((i != -1) ? 1 : -1) * la_cl.flux_impose(face-num1,k)*surface(face);
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int num1, const Neumann_paroi&, DoubleVect& aii, DoubleVect& ajj ) const
{
  aii=0.;
  ajj=0.;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Neumann_paroi& la_cl, int num1, DoubleVect& flux) const
{
  const int i = elem_(face,0);
  for (int k=0; k<flux.size(); k++) flux(k) = ((i != -1) ? 1 : -1) * la_cl.flux_impose(face-num1,k)*surface(face);
}

/* Function templates specialization for BC Neumann_sortie_libre */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , int , const Neumann_sortie_libre& , int ,DoubleVect& flux ) const
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

/* Function templates specialization for BC Periodique */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face, const Periodique& la_cl, int , DoubleVect& flux) const
{
  const int n0 = elem_(face,0), n1 = elem_(face,1);
  double heq, d0 = la_zone->dist_face_elem0_period(face,n0,la_cl.distance()), d1 = la_zone->dist_face_elem1_period(face,n1,la_cl.distance());

  if (DERIVED_T::IS_QUASI)
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

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(int face,int, const Periodique& la_cl, DoubleVect& aii, DoubleVect& ajj ) const
{
  const int i = elem_(face,0), j = elem_(face,1);
  double heq, d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance()), d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  for (int k=0; k<aii.size(); k++)
    {
      heq = compute_heq(d0,i,d1,j,k);
      aii(k) =  heq*surface(face)*porosite(face);
    }
  for (int k=0; k<ajj.size(); k++)
    {
      heq = compute_heq(d0,i,d1,j,k);
      ajj(k)=  heq*surface(face)*porosite(face);
    }
}

/* Function templates specialization for BC flux_faces_interne */
template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_faces_interne(const DoubleTab& inco, int face,DoubleVect& flux) const
{
  if (DERIVED_T::IS_QUASI)
    {
      //TODO : implement when Diff_K_Eps_QC is templatized
      Cerr << "Diff_K_Eps_QC are not templatized yet !" << finl;
      Process::exit();
    }
  const int n0 = elem_(face,0), n1 = elem_(face,1);
  double heq, d0 = Dist_face_elem0(face,n0), d1 = Dist_face_elem1(face,n1);
  for (int k=0; k<flux.size(); k++)
    {
      const int l = DERIVED_T::IS_ANISO ? orientation(face) : k;
      if (nu_1(n0, l) == 0.0 && nu_1(n1, l) == 0.0) heq = 0.;
      else
        {
          assert(nu_1(n0, l)!=0.0);
          assert(nu_1(n1, l)!=0.0);
          heq = compute_heq(d0, n0, d1, n1, l);
        }
      flux(k) = heq*(inco(n1,k) - inco(n0,k))*surface(face)*porosite(face);
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_faces_interne(int face, DoubleVect& aii, DoubleVect& ajj ) const
{
  const int i = elem_(face,0), j = elem_(face,1);
  double heq, d0 = Dist_face_elem0(face,i), d1 = Dist_face_elem1(face,j);
  for (int k=0; k<aii.size(); k++)
    {
      const int l = DERIVED_T::IS_ANISO ? orientation(face) : k;
      if (nu_1(i,l)==0.0 && nu_2(j,l)==0.0) heq = 0.;
      else
        {
          assert(nu_1(i,l)!=0.0 && nu_2(j,l)!=0.0);
          heq = compute_heq(d0,i,d1,j,l);
        }
      aii(k) = heq*surface(face)*porosite(face);
      ajj(k) = heq*surface(face)*porosite(face);
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_faces_interne( int, DoubleVect& flux ) const
{
  for (int k=0; k<flux.size(); k++) flux(k)=0.;
}

#endif /* Eval_Diff_VDF_Elem_TPP_included */
