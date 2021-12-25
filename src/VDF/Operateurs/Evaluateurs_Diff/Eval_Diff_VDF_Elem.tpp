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

/* ************************************** *
 * *********  POUR L'EXPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const int face, const Dirichlet_entree_fluide& la_cl, const int num1, DoubleVect& flux) const
{
  if (DERIVED_T::IS_QUASI) not_implemented_k_eps(__func__);

  // Olga avait mis : double dist = 2*Dist_norm_bord(face);
  // Pierre dit que :
  const double dist = Dist_norm_bord(face);
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size();

  for (int k = 0; k < ncomp; k++)
    {
      const double T_imp = la_cl.val_imp(face-num1,k);
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      flux(k) = (i != -1) ? (T_imp-inco(i,k))*surface(face)*porosite(face)*nu_1(i,ori)/dist : (inco(j,k)-T_imp)*surface(face)*porosite(face)*nu_1(j,ori)/dist;
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , const int face, const Neumann_paroi& la_cl, const int num1, DoubleVect& flux) const
{
  const int i = elem_(face,0), ncomp = flux.size();
  for (int k=0; k < ncomp; k++) flux(k) = ((i != -1) ? 1 : -1) * la_cl.flux_impose(face-num1,k)*surface(face);
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const int face, const Periodique& la_cl, const int , DoubleVect& flux) const
{
  if (DERIVED_T::IS_QUASI) not_implemented_k_eps(__func__);

  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size();
  const double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance()), d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());

  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      const double heq = compute_heq(d0,i,d1,j,ori);
      flux(k) = heq*(inco(j,k) - inco(i,k))*surface(face)*porosite(face);
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const int face , const Echange_global_impose& la_cl, const int num1, DoubleVect& flux) const
{
  if (DERIVED_T::IS_QUASI)  not_implemented_k_eps(__func__);

  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size();

  for (int k = 0; k < ncomp; k++)
    {
      const double h = la_cl.h_imp(face-num1,k), T_ext = la_cl.T_ext(face-num1,k);
      flux(k) = (i != -1) ? h*(T_ext-inco(i,k))*surface(face) : h*(inco(j,k)-T_ext)*surface(face);
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const int face, const Dirichlet_paroi_fixe&, const int num1, DoubleVect& flux ) const
{
  if (DERIVED_T::IS_QUASI) not_implemented_k_eps(__func__);

  // TODO : FIXME : Yannick help :/ methode pas code a la base pour cas scalaire ... mais on a 0 cas test dans trust et on entre jamais ici :-)
  flux = 0.;
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size();
  const double dist = Dist_norm_bord(face);

  for (int k = 0; k < ncomp; k++) flux(k) = (i != -1) ? -inco(i,k)*surface(face)*porosite(face)*nu_1(i,k)/dist : inco(j,k)*surface(face)*porosite(face)*nu_1(j,k)/dist;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const int boundary_index, const int face, const int local_face, const Echange_externe_impose& la_cl, const int num1, DoubleVect& flux) const
{
  if (DERIVED_T::IS_QUASI) not_implemented_k_eps(__func__);

  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite) : La C.L fournit h_imp ; il faut calculer e/diffusivite
  double heq, h_total_inv, e;
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size();

  if (DERIVED_T::IS_MODIF_DEQ) e = ind_Fluctu_Term()==1 ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  // XXX : E. Saikali 08/03/2021 : The test of a zero diffusion was not done before. I think it should be like that
  // Attention for DIFT OPERATORS : nu_2 and not nu_1 (only laminar part)
  if ( i != -1 )
    {
      for (int k = 0; k < ncomp; k++)
        {
          const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
          const double h_imp = la_cl.h_imp(face-num1,k), T_ext = la_cl.T_ext(face-num1,k);
          if ( nu_2(i,ori) == 0.0 ) heq = h_imp; // Should be 0 ??
          else
            {
              h_total_inv =  1.0/h_imp + e/nu_2(i,ori);
              heq = 1.0 / h_total_inv;
            }
          flux(k) = heq * (T_ext-inco(i,k))*surface(face);
        }
    }
  else // j != -1
    {
      for (int k = 0; k < ncomp; k++)
        {
          const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
          const double h_imp = la_cl.h_imp(face-num1,k), T_ext = la_cl.T_ext(face-num1,k);
          if ( nu_2(j,ori) == 0.0 ) heq = h_imp; // Should be 0 ??
          else
            {
              h_total_inv = 1.0/h_imp + e/nu_2(j,ori);
              heq = 1.0 / h_total_inv;
            }
          flux(k) = heq*(inco(j,k)-T_ext)*surface(face);
        }
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_faces_interne(const DoubleTab& inco, const int face, DoubleVect& flux) const
{
  if (DERIVED_T::IS_QUASI) not_implemented_k_eps(__func__);

  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size();
  double heq, d0 = Dist_face_elem0(face,i), d1 = Dist_face_elem1(face,j);
  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      if (nu_1(i,ori) == 0.0 && nu_1(j,ori) == 0.0) heq = 0.;
      else
        {
          assert(nu_1(i,ori) != 0.0 && nu_1(j,ori) != 0.0);
          heq = compute_heq(d0,i, d1,j,ori);
        }
      flux(k) = heq*(inco(j,k)-inco(i,k))*surface(face)*porosite(face);
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int, const Dirichlet_entree_fluide& la_cl, DoubleVect& aii, DoubleVect& ajj) const
{
  assert (aii.size() == ajj.size());
  const int i = elem_(face,0), j = elem_(face,1), ncomp = aii.size();
  const double dist = Dist_norm_bord(face);
  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      aii(k) = (i != -1) ? porosite(face)*nu_1(i,ori)*surface(face) / dist : 0.;
      ajj(k) = (i != -1) ? 0. : porosite(face)*nu_1(j,ori)*surface(face) / dist;
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int num1, const Neumann_paroi&, DoubleVect& aii, DoubleVect& ajj ) const
{
  aii = ajj = 0.; // On peut faire ca !
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int, const Periodique& la_cl, DoubleVect& aii, DoubleVect& ajj ) const
{
  assert (aii.size() == ajj.size());
  const int i = elem_(face,0), j = elem_(face,1), ncomp = aii.size();
  const double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance()), d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      const double heq = compute_heq(d0,i,d1,j,ori);
      aii(k) = ajj(k) = heq*surface(face)*porosite(face); // On peut faire ca !
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int num1, const Echange_global_impose& la_cl, DoubleVect& aii, DoubleVect& ajj ) const
{
  assert (aii.size() == ajj.size());
  const int i = elem_(face,0), ncomp = aii.size();
  for (int k = 0; k < ncomp; k++)
    {
      aii(k) = (i != -1) ? la_cl.h_imp(face-num1,k)*surface(face) : 0.;
      ajj(k) = (i != -1) ? 0. : la_cl.h_imp(face-num1,k)*surface(face);
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int num1, const Dirichlet_paroi_fixe&, DoubleVect& aii , DoubleVect& ajj) const
{
  if (DERIVED_T::IS_QUASI) not_implemented_k_eps(__func__);

  assert (aii.size() == ajj.size());
  const int i = elem_(face,0), j = elem_(face,1), ncomp = aii.size(); // TODO : FIXME : Yannick help :/ methode pas code a la base pour cas scalaire ... mais on a 0 cas test dans trust et on entre jamais ici :-)
  const double dist = Dist_norm_bord(face);
  for (int k = 0; k < ncomp; k++)
    {
      aii(k) = (i != -1) ? surface(face)*porosite(face)*nu_1(i,k)/dist : 0.;
      ajj(k) = (i != -1) ? 0. : surface(face)*porosite(face)*nu_1(j,k)/dist;
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int boundary_index, const int face, const int local_face, const int num1, const Echange_externe_impose& la_cl, DoubleVect& aii, DoubleVect& ajj) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite) : La C.L fournit h_imp ; il faut calculer e/diffusivite
  assert (aii.size() == ajj.size());
  const int i = elem_(face,0), j = elem_(face,1), ncomp = aii.size();
  double e, heq, h_total_inv;

  if (DERIVED_T::IS_MODIF_DEQ) e = ind_Fluctu_Term() == 1 ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  if (i != -1)
    for (int k = 0; k < ncomp; k++)
      {
        const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
        const double h_imp = la_cl.h_imp(face-num1,k);
        if (nu_2(i,ori) == 0.0) heq = h_imp; // Should be 0 ??
        else
          {
            h_total_inv =  1.0/h_imp + e/nu_2(i,ori);
            heq = 1.0 / h_total_inv;
          }
        aii(k) = heq*surface(face);
        ajj(k) = 0.;
      }
  else
    for (int k = 0; k < ncomp; k++)
      {
        const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
        const double h_imp = la_cl.h_imp(face-num1,k);
        if (nu_2(j,ori) == 0.0) heq = h_imp; // Should be 0 ??
        else
          {
            h_total_inv =  1.0/h_imp + e/nu_2(j,ori);
            heq = 1.0 / h_total_inv;
          }
        ajj(k) = heq*surface(face);
        aii(k) = 0.;
      }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_faces_interne(const int face, DoubleVect& aii, DoubleVect& ajj ) const
{
  assert (aii.size() == ajj.size());
  const int i = elem_(face,0), j = elem_(face,1), ncomp = aii.size();
  double heq, d0 = Dist_face_elem0(face,i), d1 = Dist_face_elem1(face,j);
  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      if (nu_1(i,ori) == 0.0 && nu_2(j,ori) == 0.0) heq = 0.;
      else
        {
          assert(nu_1(i,ori) != 0.0 && nu_2(j,ori) != 0.0);
          heq = compute_heq(d0,i,d1,j,ori);
        }
      aii(k) = ajj(k) = heq*surface(face)*porosite(face); // On peut faire ca !
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Dirichlet_entree_fluide& la_cl, int num1,DoubleVect& flux) const
{
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size();
  double dist = Dist_norm_bord(face);
  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      const double T_imp = la_cl.val_imp(face-num1,k);
      flux(k) = (i != -1) ? T_imp*surface(face)*porosite(face)*nu_1(i,ori) / dist : -T_imp*surface(face)*(porosite(face)*nu_1(j,ori)) / dist;
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Neumann_paroi& la_cl, int num1, DoubleVect& flux) const
{
  const int i = elem_(face,0), ncomp = flux.size();
  for (int k = 0; k < ncomp; k++) flux(k) = ((i != -1) ? 1 : -1) * la_cl.flux_impose(face-num1,k)*surface(face);
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Echange_global_impose& la_cl, int num1, DoubleVect& flux) const
{
  const int i = elem_(face,0), ncomp = flux.size();
  for (int k = 0; k < ncomp; k++)
    {
      const double h = la_cl.h_imp(face-num1,k), T_ext = la_cl.T_ext(face-num1,k);
      flux(k) = (i != -1) ? h*T_ext*surface(face) : -h*T_ext*surface(face);
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int face, const Dirichlet_paroi_fixe&, int, DoubleVect& flux) const
{
  flux = 0.;
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(int boundary_index, int face, int local_face, const Echange_externe_impose& la_cl, int num1,DoubleVect& flux) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite) : La C.L fournit h_imp ; il faut calculer e/diffusivite
  double e, h_total_inv, heq;
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size();

  if (DERIVED_T::IS_MODIF_DEQ) e = (ind_Fluctu_Term() == 1) ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  if (i != -1)
    {
      for (int k = 0; k < ncomp; k++)
        {
          const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
          const double h_imp = la_cl.h_imp(face-num1,k), T_ext = la_cl.T_ext(face-num1);
          if (nu_2(i,ori) == 0.0) heq = h_imp; // Should be 0 ??
          else
            {
              h_total_inv = 1.0/h_imp + e/nu_2(i,ori);
              heq = 1.0 / h_total_inv;
            }
          flux(k) = heq*T_ext*surface(face);
        }
    }
  else
    {
      for (int k = 0; k < ncomp; k++)
        {
          const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
          const double h_imp = la_cl.h_imp(face-num1,k), T_ext = la_cl.T_ext(face-num1);
          if (nu_2(j,ori) == 0.0) heq = h_imp; // Should be 0 ??
          else
            {
              h_total_inv = 1.0/h_imp + e/nu_2(j,ori);
              heq = 1.0 / h_total_inv;
            }
          flux(k) = -heq*T_ext*surface(face);
        }
    }
}

template <typename DERIVED_T>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_faces_interne( int, DoubleVect& flux ) const
{
  flux = 0.;
}

#endif /* Eval_Diff_VDF_Elem_TPP_included */
