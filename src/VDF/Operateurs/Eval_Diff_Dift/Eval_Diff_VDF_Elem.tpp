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

#ifndef Eval_Diff_VDF_Elem_TPP_included
#define Eval_Diff_VDF_Elem_TPP_included

/* ************************************** *
 * *********  POUR L'EXPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const int face, const Dirichlet_entree_fluide& la_cl, const int num1, Type_Double& flux) const
{
  // Olga avait mis : double dist = 2*Dist_norm_bord(face);
  // Pierre dit que :
  const double dist = Dist_norm_bord(face);
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size_array();

  for (int k = 0; k < ncomp; k++)
    {
      const double T_imp = la_cl.val_imp(face-num1,k);
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      if (DERIVED_T::IS_QUASI)
        flux[k] = (i != -1) ? (T_imp-inco(i,k))/dv_mvol(i)*surface(face)*porosite(face)*nu_1(i,ori)/dist : (inco(j,k)-T_imp)/dv_mvol(j)*surface(face)*porosite(face)*nu_1(j,ori)/dist;
      else
        flux[k] = (i != -1) ? (T_imp-inco(i,k))*surface(face)*porosite(face)*nu_1(i,ori)/dist : (inco(j,k)-T_imp)*surface(face)*porosite(face)*nu_1(j,ori)/dist;
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& , const int face, const Neumann_paroi& la_cl, const int num1, Type_Double& flux) const
{
  const int i = elem_(face,0), ncomp = flux.size_array();
  for (int k=0; k < ncomp; k++) flux[k] = ((i != -1) ? 1 : -1) * la_cl.flux_impose(face-num1,k)*surface(face);
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const int face, const Periodique& la_cl, const int , Type_Double& flux) const
{
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size_array();
  const double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance()), d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());

  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      const double heq = compute_heq(d0,i,d1,j,ori);

      flux[k] = DERIVED_T::IS_QUASI ? heq*(inco(j,k)/dv_mvol(j) - inco(i,k)/dv_mvol(i))*surface(face)*porosite(face) : heq*(inco(j,k) - inco(i,k))*surface(face)*porosite(face);
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const int face, const Dirichlet_paroi_fixe&, const int num1, Type_Double& flux ) const
{
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size_array();
  const double dist = Dist_norm_bord(face);

  if (DERIVED_T::IS_QUASI)
    for (int k = 0; k < ncomp; k++) flux[k] = (i != -1) ? -inco(i,k)*surface(face)*porosite(face)*nu_1(i,k)/dv_mvol(i)/dist : inco(j,k)*surface(face)*porosite(face)*nu_1(j,k)/dv_mvol(j)/dist;
  else
    for (int k = 0; k < ncomp; k++) flux[k] = (i != -1) ? -inco(i,k)*surface(face)*porosite(face)*nu_1(i,k)/dist : inco(j,k)*surface(face)*porosite(face)*nu_1(j,k)/dist;
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const int face , const Echange_global_impose& la_cl, const int num1, Type_Double& flux) const
{
  if (DERIVED_T::IS_QUASI)  not_implemented_k_eps(__func__);

  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size_array();

  for (int k = 0; k < ncomp; k++)
    {
      const double h = la_cl.h_imp(face-num1,k), T_ext = la_cl.T_ext(face-num1,k);
      const double phi_ext = la_cl.has_phi_ext() ? la_cl.flux_exterieur_impose(face-num1,k) : 0;
      flux[k] = (i != -1) ? (phi_ext+h*(T_ext-inco(i,k)))*surface(face) : (-phi_ext+h*(inco(j,k)-T_ext))*surface(face);
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const int boundary_index, const int face, const int local_face, const Echange_externe_impose& la_cl, const int num1, Type_Double& flux) const
{
  if (DERIVED_T::IS_QUASI) not_implemented_k_eps(__func__);

  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite) : La C.L fournit h_imp ; il faut calculer e/diffusivite
  double heq, h_total_inv, e;
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size_array();

  if (DERIVED_T::IS_MODIF_DEQ) e = ind_Fluctu_Term()==1 ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  int elem_opp = -1;
  if (sub_type(Echange_interne_impose, la_cl))
    {
      // In this case, T_ext might change within the timestep (e.g. Newton's algorithm used for the
      // variable gap model). Make sure we get the most up-to-date value.
      const Echange_interne_impose& cl = ref_cast(Echange_interne_impose, la_cl);
      const Champ_front_calc_interne& Text = ref_cast(Champ_front_calc_interne, cl.T_ext().valeur());
      const IntTab& fmap = Text.face_map();
      int opp_face = fmap[face-num1]+num1;  // num1 is the index of the first face
      int elem1 = la_zone->face_voisins(opp_face, 0);
      elem_opp = (elem1 != -1) ? elem1 : la_zone->face_voisins(opp_face, 1);
    }

  // XXX : E. Saikali 08/03/2021 : The test of a zero diffusion was not done before. I think it should be like that
  // Attention for DIFT OPERATORS : nu_2 and not nu_1 (only laminar part)
  if ( i != -1 )
    {
      for (int k = 0; k < ncomp; k++)
        {
          const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
          const double h_imp = la_cl.h_imp(face-num1,k), T_ext = (elem_opp == -1) ? la_cl.T_ext(face-num1,k) : inco(elem_opp,k);
          if ( nu_2(i,ori) == 0.0 ) heq = 0.0;
          else
            {
              h_total_inv =  1.0/h_imp + e/nu_2(i,ori);
              heq = 1.0 / h_total_inv;
            }
          flux[k] = heq * (T_ext-inco(i,k))*surface(face);
        }
    }
  else // j != -1
    {
      for (int k = 0; k < ncomp; k++)
        {
          const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
          const double h_imp = la_cl.h_imp(face-num1,k), T_ext = (elem_opp == -1) ? la_cl.T_ext(face-num1,k) : inco(elem_opp,k);
          if ( nu_2(j,ori) == 0.0 ) heq = 0.0;
          else
            {
              h_total_inv = 1.0/h_imp + e/nu_2(j,ori);
              heq = 1.0 / h_total_inv;
            }
          flux[k] = heq*(inco(j,k)-T_ext)*surface(face);
        }
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::flux_faces_interne(const DoubleTab& inco, const int face, Type_Double& flux) const
{
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size_array();
  double heq, d0 = Dist_face_elem0(face,i), d1 = Dist_face_elem1(face,j);
  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      if (DERIVED_T::IS_RANS)
        {
          heq = compute_heq(d0,i, d1,j,ori); // pas d'assert pour k-eps !
          flux[k] = DERIVED_T::IS_QUASI ? heq*(inco(j,k)/dv_mvol(j) - inco(i,k)/dv_mvol(i))*surface(face)*porosite(face) : heq*(inco(j,k)-inco(i,k))*surface(face)*porosite(face);
        }
      else
        {
          if (nu_1(i,ori) == 0.0 && nu_1(j,ori) == 0.0) heq = 0.;
          else
            {
              assert(nu_1(i,ori) != 0.0 && nu_1(j,ori) != 0.0);
              heq = compute_heq(d0,i, d1,j,ori);
            }
          flux[k] = heq*(inco(j,k)-inco(i,k))*surface(face)*porosite(face);
        }
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int, const Dirichlet_entree_fluide& la_cl, Type_Double& aii, Type_Double& ajj) const
{
  assert (aii.size_array() == ajj.size_array());
  const int i = elem_(face,0), j = elem_(face,1), ncomp = aii.size_array();
  const double dist = Dist_norm_bord(face);
  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      aii[k] = (i != -1) ? porosite(face)*nu_1(i,ori)*surface(face) / dist : 0.;
      ajj[k] = (i != -1) ? 0. : porosite(face)*nu_1(j,ori)*surface(face) / dist;
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int, const Periodique& la_cl, Type_Double& aii, Type_Double& ajj ) const
{
  assert (aii.size_array() == ajj.size_array());
  const int i = elem_(face,0), j = elem_(face,1), ncomp = aii.size_array();
  const double d0 = la_zone->dist_face_elem0_period(face,i,la_cl.distance()), d1 = la_zone->dist_face_elem1_period(face,j,la_cl.distance());
  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      const double heq = compute_heq(d0,i,d1,j,ori);
      aii[k] = ajj[k] = heq*surface(face)*porosite(face); // On peut faire ca !
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int num1, const Dirichlet_paroi_fixe&, Type_Double& aii , Type_Double& ajj) const
{
  assert (aii.size_array() == ajj.size_array());
  const int i = elem_(face,0), j = elem_(face,1), ncomp = aii.size_array();
  const double dist = Dist_norm_bord(face);
  if (DERIVED_T::IS_QUASI)
    {
      for (int k = 0; k < ncomp; k++)
        {
          aii[k] = (i != -1) ? surface(face)*porosite(face)*nu_1(i,k)/dv_mvol(i)/dist : 0.;
          ajj[k] = (i != -1) ? 0. : surface(face)*porosite(face)*nu_1(j,k)/dv_mvol(j)/dist;
        }
    }
  else
    for (int k = 0; k < ncomp; k++)
      {
        aii[k] = (i != -1) ? surface(face)*porosite(face)*nu_1(i,k)/dist : 0.;
        ajj[k] = (i != -1) ? 0. : surface(face)*porosite(face)*nu_1(j,k)/dist;
      }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int num1, const Echange_global_impose& la_cl, Type_Double& aii, Type_Double& ajj ) const
{
  assert (aii.size_array() == ajj.size_array());
  const int i = elem_(face,0), ncomp = aii.size_array();
  for (int k = 0; k < ncomp; k++)
    {
      const double h = la_cl.h_imp(face-num1,k);
      const double dphi_dT = la_cl.has_phi_ext() ? la_cl.derivee_flux_exterieur_imposee(face-num1,k) : 0;
      aii[k] = (i != -1) ? (dphi_dT + h)*surface(face) : 0.;
      ajj[k] = (i != -1) ? 0. : (dphi_dT + h)*surface(face);
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_face(const int boundary_index, const int face, const int local_face, const int num1, const Echange_externe_impose& la_cl, Type_Double& aii, Type_Double& ajj) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite) : La C.L fournit h_imp ; il faut calculer e/diffusivite
  assert (aii.size_array() == ajj.size_array());
  const int i = elem_(face,0), j = elem_(face,1), ncomp = aii.size_array();
  double e, heq, h_total_inv;

  if (DERIVED_T::IS_MODIF_DEQ) e = ind_Fluctu_Term() == 1 ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  const bool is_internal = sub_type(Echange_interne_impose, la_cl) ? true : false;

  if (i != -1)
    for (int k = 0; k < ncomp; k++)
      {
        const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
        const double h_imp = la_cl.h_imp(face-num1,k);
        if (nu_2(i,ori) == 0.0) heq = 0.0;
        else
          {
            h_total_inv =  1.0/h_imp + e/nu_2(i,ori);
            heq = 1.0 / h_total_inv;
          }
        aii[k] = heq*surface(face);
        ajj[k] = is_internal ? heq*surface(face) : 0.;
      }
  else
    for (int k = 0; k < ncomp; k++)
      {
        const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
        const double h_imp = la_cl.h_imp(face-num1,k);
        if (nu_2(j,ori) == 0.0) heq = 0.0;
        else
          {
            h_total_inv =  1.0/h_imp + e/nu_2(j,ori);
            heq = 1.0 / h_total_inv;
          }
        ajj[k] = heq*surface(face);
        aii[k] = is_internal ? heq*surface(face) : 0.;
      }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::coeffs_faces_interne(const int face, Type_Double& aii, Type_Double& ajj ) const
{
  assert (aii.size_array() == ajj.size_array());
  const int i = elem_(face,0), j = elem_(face,1), ncomp = aii.size_array();
  double heq, d0 = Dist_face_elem0(face,i), d1 = Dist_face_elem1(face,j);
  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      if (DERIVED_T::IS_RANS)
        {
          heq = compute_heq(d0,i,d1,j,ori);
          aii[k] = ajj[k] = heq*surface(face)*porosite(face); // On peut faire ca !
        }
      else
        {
          if (nu_1(i,ori) == 0.0 && nu_2(j,ori) == 0.0) heq = 0.;
          else
            {
              assert(nu_1(i,ori) != 0.0 && nu_2(j,ori) != 0.0);
              heq = compute_heq(d0,i,d1,j,ori);
            }
          aii[k] = ajj[k] = heq*surface(face)*porosite(face); // On peut faire ca !
        }
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(const int face, const Dirichlet_entree_fluide& la_cl, const int num1, Type_Double& flux) const
{
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size_array();
  double dist = Dist_norm_bord(face);
  for (int k = 0; k < ncomp; k++)
    {
      const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
      const double T_imp = la_cl.val_imp(face-num1,k);
      flux[k] = (i != -1) ? T_imp*surface(face)*porosite(face)*nu_1(i,ori) / dist : -T_imp*surface(face)*(porosite(face)*nu_1(j,ori)) / dist;
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(const int face, const Neumann_paroi& la_cl, const int num1, Type_Double& flux) const
{
  const int i = elem_(face,0), ncomp = flux.size_array();
  for (int k = 0; k < ncomp; k++) flux[k] = ((i != -1) ? 1 : -1) * la_cl.flux_impose(face-num1,k)*surface(face);
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(const int face, const Echange_global_impose& la_cl, const int num1, Type_Double& flux) const
{
  const int i = elem_(face,0), ncomp = flux.size_array();
  for (int k = 0; k < ncomp; k++)
    {
      const double h = la_cl.h_imp(face-num1,k), T_ext = la_cl.T_ext(face-num1,k);
      const double phi_ext = la_cl.has_phi_ext() ? la_cl.flux_exterieur_impose(face-num1, k) : 0.;
      flux[k] = (i != -1) ? (phi_ext+h*T_ext)*surface(face) : (-phi_ext-h*T_ext)*surface(face);
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Diff_VDF_Elem<DERIVED_T>::secmem_face(const int boundary_index, const int face, const int local_face, const Echange_externe_impose& la_cl, const int num1, Type_Double& flux) const
{
  // C.L de type Echange_externe_impose : 1/h_total = (1/h_imp) + (e/diffusivite) : La C.L fournit h_imp ; il faut calculer e/diffusivite
  double e, h_total_inv, heq;
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size_array();

  if (DERIVED_T::IS_MODIF_DEQ) e = (ind_Fluctu_Term() == 1) ? Dist_norm_bord_externe_(face) : equivalent_distance(boundary_index,local_face);
  else e = DERIVED_T::IS_DEQUIV ? equivalent_distance(boundary_index,local_face) : Dist_norm_bord_externe_(face);

  if (i != -1)
    {
      for (int k = 0; k < ncomp; k++)
        {
          const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
          const double h_imp = la_cl.h_imp(face-num1,k), T_ext = la_cl.T_ext(face-num1);
          if (nu_2(i,ori) == 0.0) heq = 0.0;
          else
            {
              h_total_inv = 1.0/h_imp + e/nu_2(i,ori);
              heq = 1.0 / h_total_inv;
            }
          flux[k] = heq*T_ext*surface(face);
        }
    }
  else
    {
      for (int k = 0; k < ncomp; k++)
        {
          const int ori = DERIVED_T::IS_ANISO ? orientation(face) : k;
          const double h_imp = la_cl.h_imp(face-num1,k), T_ext = la_cl.T_ext(face-num1);
          if (nu_2(j,ori) == 0.0) heq = 0.0;
          else
            {
              h_total_inv = 1.0/h_imp + e/nu_2(j,ori);
              heq = 1.0 / h_total_inv;
            }
          flux[k] = -heq*T_ext*surface(face);
        }
    }
}

#endif /* Eval_Diff_VDF_Elem_TPP_included */
