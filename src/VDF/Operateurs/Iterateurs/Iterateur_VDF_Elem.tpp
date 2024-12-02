/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Iterateur_VDF_Elem_TPP_included
#define Iterateur_VDF_Elem_TPP_included

#include <Operateur_Diff_base.h>
#include <Champ_Uniforme.h>
#include <communications.h>
#include <TRUSTSingle.h>

template<class _TYPE_>
void Iterateur_VDF_Elem<_TYPE_>::ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>& f2e) const
{
  const int ncomp = inco.line_size();
  ArrOfDouble aii(ncomp), ajj(ncomp);
  const Front_VF& frontiere_dis = ref_cast(Front_VF, la_cl->frontiere_dis());
  const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
  if (_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP)
    {
      const Echange_global_impose& cl = (const Echange_global_impose&) (la_cl.valeur());
      for (int f = ndeb; f < nfin; f++)
        {
          const int e1 = f2e[f].first, e2 = f2e[f].second;
          flux_evaluateur.coeffs_face(f, ndeb, cl, aii, ajj);
          for (int i = 0; i < ncomp; i++)
            matrice(e1 * ncomp + i, e2 * ncomp + i) = -(elem(f, 0) > -1 ? aii[i] : ajj[i]);
        }
    }
}

/* ************************************** *
 * *********  INTERFACE  BLOCS ********** *
 * ************************************** */

template<class _TYPE_>
void Iterateur_VDF_Elem<_TYPE_>::ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(op_base->equation().inconnue().valeurs().nb_dim() < 3 && la_zcl.non_nul() && le_dom.non_nul());
  const int ncomp = op_base->equation().inconnue().valeurs().line_size();
  DoubleTab& flux_bords = op_base->flux_bords();
  flux_bords.resize(le_dom->nb_faces_bord(), ncomp);
  flux_bords = 0.;
  // modif b.m.: on va faire += sur des items virtuels, initialiser les cases : sinon risque que les cases soient invalides ou non initialisees
  {
    int n = secmem.size_array() - secmem.size();
    double *data = secmem.addr() + secmem.size();
    for (; n; n--, data++) *data = 0.;
  }
  if (ncomp == 1)
    {
      ajouter_blocs_bords < SingleDouble > (ncomp, mats, secmem, semi_impl);
      ajouter_blocs_interne < SingleDouble > (ncomp, mats, secmem, semi_impl);
    }
  else
    {
      ajouter_blocs_bords < ArrOfDouble > (ncomp, mats, secmem, semi_impl);
      ajouter_blocs_interne < ArrOfDouble > (ncomp, mats, secmem, semi_impl);
    }

  if (!is_pb_multi) modifier_flux(); /* deja rho*cp car champ convecte */
}

template<class _TYPE_> template<typename Type_Double>
void Iterateur_VDF_Elem<_TYPE_>::ajouter_blocs_bords(const int ncomp, matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const
{
  for (int num_cl = 0; num_cl < le_dom->nb_front_Cl(); num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF, la_cl->frontiere_dis());
      const int ndeb = frontiere_dis.num_premiere_face(), nfin = ndeb + frontiere_dis.nb_faces();
      /* Test en bidim axi */
      if (bidim_axi && !sub_type(Symetrie, la_cl.valeur()))
        {
          if (nfin > ndeb && est_egal(le_dom->face_surfaces()[ndeb], 0))
            {
              Cerr << "Error in the definition of the boundary conditions. The axis of revolution for this 2D calculation is along Y." << finl;
              Cerr << "So you must specify symmetry boundary condition (symetrie keyword) for the boundary " << frontiere_dis.le_nom() << finl;
              Process::exit();
            }
        }

      switch(type_cl(la_cl))
        {
        case navier:
          ajouter_blocs_bords_<_TYPE_::CALC_FLUX_FACES_SYMM, Type_Double>((const Symetrie&) la_cl.valeur(), ndeb, nfin, ncomp, mats, resu, semi_impl);
          break;
        case sortie_libre:
          ajouter_blocs_bords_<_TYPE_::CALC_FLUX_FACES_SORTIE_LIB, Type_Double>((const Neumann_sortie_libre&) la_cl.valeur(), ndeb, nfin, ncomp, mats, resu, semi_impl);
          break;
        case entree_fluide:
          ajouter_blocs_bords_<_TYPE_::CALC_FLUX_FACES_ENTREE_FL, Type_Double>((const Dirichlet_entree_fluide&) la_cl.valeur(), ndeb, nfin, ncomp, mats, resu, semi_impl);
          break;
        case paroi_fixe:
          ajouter_blocs_bords_<_TYPE_::CALC_FLUX_FACES_PAR_FIXE, Type_Double>((const Dirichlet_paroi_fixe&) la_cl.valeur(), ndeb, nfin, ncomp, mats, resu, semi_impl);
          break;
        case paroi_defilante:
          ajouter_blocs_bords_<_TYPE_::CALC_FLUX_FACES_PAR_DEFIL, Type_Double>((const Dirichlet_paroi_defilante&) la_cl.valeur(), ndeb, nfin, ncomp, mats, resu, semi_impl);
          break;
        case paroi_scalaire_impose:
          ajouter_blocs_bords_<_TYPE_::CALC_FLUX_FACES_SCAL_IMPOSEE, Type_Double>((const Scalaire_impose_paroi&) la_cl.valeur(), ndeb, nfin, ncomp, mats, resu, semi_impl);
          break;
        case paroi_dirichlet_loi_paroi:
          ajouter_blocs_bords_<_TYPE_::CALC_FLUX_FACES_SCAL_IMPOSEE /* true */, Type_Double>((const Dirichlet_loi_paroi&) la_cl.valeur(), ndeb, nfin, ncomp, mats, resu, semi_impl);
          break;
        case paroi:
          ajouter_blocs_bords_<_TYPE_::CALC_FLUX_FACES_PAR, Type_Double>((const Neumann_paroi&) la_cl.valeur(), ndeb, nfin, ncomp, mats, resu, semi_impl);
          break;
        case echange_global_impose:
          ajouter_blocs_bords_<_TYPE_::CALC_FLUX_FACES_ECH_GLOB_IMP, Type_Double>((const Echange_global_impose&) la_cl.valeur(), ndeb, nfin, ncomp, mats, resu, semi_impl);
          break;
        case paroi_adiabatique:
          ajouter_blocs_bords_<_TYPE_::CALC_FLUX_FACES_PAR_ADIAB, Type_Double>((const Neumann_paroi_adiabatique&) la_cl.valeur(), ndeb, nfin, ncomp, mats, resu, semi_impl);
          break;
        case echange_externe_impose:
          ajouter_blocs_bords_ < Type_Double > ((const Echange_externe_impose&) la_cl.valeur(), ndeb, nfin, num_cl, ncomp, frontiere_dis, mats, resu, semi_impl);
          break;
        case periodique:
          ajouter_blocs_bords_ < Type_Double > ((const Periodique&) la_cl.valeur(), ndeb, nfin, ncomp, frontiere_dis, mats, resu, semi_impl);
          break;
        default:
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur() << " , dans Iterateur_VDF_Elem<_TYPE_>::ajouter_bords" << finl;
          Process::exit();
          break;
        }
    }
}

template<class _TYPE_> template<typename Type_Double>
void Iterateur_VDF_Elem<_TYPE_>::ajouter_blocs_interne(const int N, matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const
{
  const DoubleTab& donnee = semi_impl.count(nom_ch_inco_) ? semi_impl.at(nom_ch_inco_) : le_champ_convecte_ou_inc->valeurs();
  const int multi = multiscalar_;
  Type_Double flux(N), aii(N * (multi ? N : 1)), ajj(N * (multi ? N : 1)), aef(N);
  const int ndeb = le_dom->premiere_face_int(), nfin = le_dom->nb_faces(), Mv = le_ch_v.non_nul() ? le_ch_v->valeurs().line_size() : N;
  for (int face = ndeb; face < nfin; face++)
    {
      flux_evaluateur.flux_faces_interne(donnee, face, flux);
      const int e0 = elem(face, 0), e1 = elem(face, 1);
      // second membre
      for (int k = 0; k < N; k++)
        {
          resu(e0, k) += flux[k];
          resu(e1, k) -= flux[k];
        }
    }

  Matrice_Morse *m_vit = (mats.count("vitesse") && is_convective_op()) ? mats.at("vitesse") : nullptr, *mat = (!is_pb_multiphase() && mats.count(nom_ch_inco_)) ? mats.at(nom_ch_inco_) : nullptr;
  VectorDeriv d_cc;
  fill_derivee_cc(mats, semi_impl, d_cc);

  //derivees : vitesse
  if (m_vit)
    for (int face = ndeb; face < nfin; face++)
      {
        flux_evaluateur.coeffs_faces_interne_bloc_vitesse(donnee, face, aef);
        for (int i = 0; i < 2; i++)
          for (int n = 0, m = 0; n < N; n++, m += (Mv > 1))
            (*m_vit)(N * elem(face, i) + n, Mv * face + m) += (i ? -1.0 : 1.0) * aef(n);
      }

  //derivees : champ convecte
  if (mat || d_cc.size() > 0)
    for (int face = ndeb; face < nfin; face++)
      {
        flux_evaluateur.coeffs_faces_interne(face, aii, ajj);
        fill_coeffs_matrices(face, 1.0 /* coeff */, aii, ajj, mat, d_cc);
      }
}

template<class _TYPE_> template<bool should_calc_flux, typename Type_Double, typename BC>
void Iterateur_VDF_Elem<_TYPE_>::ajouter_blocs_bords_(const BC& cl, const int ndeb, const int nfin, const int N, matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const
{
  constexpr bool is_Neum_paroi_adiab = std::is_same<BC, Neumann_paroi_adiabatique>::value;
  const int multi = multiscalar_;
  if (should_calc_flux)
    {
      if (is_Neum_paroi_adiab)
        Process::exit(); // On bloque ici :-)

      const DoubleTab& donnee = semi_impl.count(nom_ch_inco_) ? semi_impl.at(nom_ch_inco_) : le_champ_convecte_ou_inc->valeurs(),
                       val_b = sub_type(Champ_Face_base, le_champ_convecte_ou_inc.valeur()) ? DoubleTab() : (use_base_val_b_ ? le_champ_convecte_ou_inc->Champ_base::valeur_aux_bords() : le_champ_convecte_ou_inc->valeur_aux_bords()); // si le champ associe est un champ_face, alors on est dans un operateur de div

      int e, Mv = le_ch_v.non_nul() ? le_ch_v->valeurs().line_size() : N;
      Type_Double flux(N), aii(N * (multi ? N : 1)), ajj(N * (multi ? N : 1)), aef(N);
      for (int face = ndeb; face < nfin; face++)
        {
          flux_evaluateur.flux_face(donnee, val_b, face, cl, ndeb, flux); // Generic code
          fill_flux_tables_(face, N, 1.0 /* coeff */, flux, resu);
        }

      Matrice_Morse *m_vit = (mats.count("vitesse") && is_convective_op()) ? mats.at("vitesse") : nullptr, *mat = (!is_pb_multiphase() && mats.count(nom_ch_inco_)) ? mats.at(nom_ch_inco_) : nullptr;
      VectorDeriv d_cc;
      fill_derivee_cc(mats, semi_impl, d_cc);

      //derivees : vitesse
      if (m_vit)
        {
          const IntTab* fcl_v = le_ch_v.non_nul() ? &ref_cast(Champ_Face_base, le_ch_v.valeur()).fcl() : nullptr;
          for (int f = ndeb; f < nfin; f++)
            if ((*fcl_v)(f, 0) < 2)
              {
                flux_evaluateur.coeffs_face_bloc_vitesse(donnee, val_b, f, cl, ndeb, aef);
                for (int i = 0; i < 2; i++)
                  if ((e = elem(f, i)) >= 0)
                    for (int n = 0, m = 0; n < N; n++, m += (Mv > 1))
                      (*m_vit)(N * e + n, Mv * f + m) += (i ? -1.0 : 1.0) * aef(n);
              }
        }

      //derivees : champ convecte
      if (mat || d_cc.size() > 0)
        for (int face = ndeb; face < nfin; face++)
          {
            flux_evaluateur.coeffs_face(face, ndeb, cl, aii, ajj); // Generic code
            fill_coeffs_matrices(face, aii, ajj, mat, d_cc);
          }
    }
}

template<class _TYPE_> template<typename Type_Double>
void Iterateur_VDF_Elem<_TYPE_>::ajouter_blocs_bords_(const Periodique& cl, const int ndeb, const int nfin, const int N, const Front_VF& frontiere_dis,
                                                      matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const
{
  DoubleTab& flux_bords = op_base->flux_bords();
  if (_TYPE_::CALC_FLUX_FACES_PERIO)
    {
      const DoubleTab& donnee = semi_impl.count(nom_ch_inco_) ? semi_impl.at(nom_ch_inco_) : le_champ_convecte_ou_inc->valeurs();

      Type_Double flux(N), aii(N), ajj(N), aef(N);
      for (int face = ndeb; face < nfin; face++)
        {
          const int e0 = elem(face, 0), e1 = elem(face, 1);
          flux_evaluateur.flux_face(donnee, donnee, face, cl, ndeb, flux); // attention 2 fois donnee

          for (int n = 0; n < N; n++)
            {
              if (e0 > -1)
                {
                  resu(e0, n) += 0.5 * flux[n];
                  if (face < (ndeb + frontiere_dis.nb_faces() / 2)) flux_bords(face, n) += flux[n];
                }
              if (e1 > -1)
                {
                  resu(e1, n) -= 0.5 * flux[n];
                  if ((ndeb + frontiere_dis.nb_faces() / 2) <= face) flux_bords(face, n) -= flux[n];
                }
            }
        }

      Matrice_Morse *m_vit = mats.count("vitesse") ? mats.at("vitesse") : nullptr, *mat = (!is_pb_multiphase() && mats.count(nom_ch_inco_)) ? mats.at(nom_ch_inco_) : nullptr;
      VectorDeriv d_cc;
      fill_derivee_cc(mats, semi_impl, d_cc);

      //derivees : vitesse
      if (m_vit)
        for (int face = ndeb; face < nfin; face++)
          {
            const int e0 = elem(face, 0), e1 = elem(face, 1);
            flux_evaluateur.coeffs_face_bloc_vitesse(donnee, DoubleTab(), face, cl, ndeb, aef);
            if (e0 > -1)
              for (int i = 0; i < N; i++)
                if (face < (ndeb + frontiere_dis.nb_faces() / 2)) (*m_vit)(e0 * N + i, face * N + i) += aef[i];
            if (e1 > -1)
              for (int i = 0; i < N; i++)
                if ((ndeb + frontiere_dis.nb_faces() / 2) <= face) (*m_vit)(e1 * N + i, face * N + i) -= aef[i];
          }

      //derivees : champ convecte
      if (mat || d_cc.size() > 0)
        for (int face = ndeb; face < nfin; face++)
          {
            flux_evaluateur.coeffs_face(face, ndeb, cl, aii, ajj);
            fill_coeffs_matrices(face, 0.5 /* coeff */, aii, ajj, mat, d_cc);
          }
    }
}

template<class _TYPE_> template<typename Type_Double>
void Iterateur_VDF_Elem<_TYPE_>::ajouter_blocs_bords_(const Echange_externe_impose& cl, const int ndeb, const int nfin, const int num_cl, const int N, const Front_VF& frontiere_dis,
                                                      matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const
{
  if (_TYPE_::CALC_FLUX_FACES_ECH_EXT_IMP)
    {
      // Si la method est faite dans FT, on sort
      const bool calculated_in_FT = ajouter_blocs_bords_echange_ext_FT_TCL<Type_Double>(cl, ndeb, nfin, num_cl, N, frontiere_dis, mats, resu, semi_impl);
      if ( calculated_in_FT ) return;

      // Sinon, GO !
      const DoubleTab& donnee = semi_impl.count(nom_ch_inco_) ? semi_impl.at(nom_ch_inco_) : le_champ_convecte_ou_inc->valeurs();

      const int multi = multiscalar_;
      Type_Double flux(N), aii(N * (multi ? N : 1)), ajj(N * (multi ? N : 1)), aef(N);
      int boundary_index = -1;
      if (le_dom->front_VF(num_cl).le_nom() == frontiere_dis.le_nom())
        boundary_index = num_cl;

      int e, Mv = le_ch_v.non_nul() ? le_ch_v->valeurs().line_size() : N;
      for (int face = ndeb; face < nfin; face++)
        {
          const int local_face = le_dom->front_VF(boundary_index).num_local_face(face);
          flux_evaluateur.flux_face(donnee, boundary_index, face, local_face, cl, ndeb, flux);
          fill_flux_tables_(face, N, 1.0 /* coeff */, flux, resu);
        }

      Matrice_Morse *m_vit = (mats.count("vitesse") && is_convective_op()) ? mats.at("vitesse") : nullptr, *mat = (!is_pb_multiphase() && mats.count(nom_ch_inco_)) ? mats.at(nom_ch_inco_) : nullptr;
      VectorDeriv d_cc;
      fill_derivee_cc(mats, semi_impl, d_cc);

      //derivees : vitesse
      if (m_vit)
        {
          DoubleTab val_b = use_base_val_b_ ? le_champ_convecte_ou_inc->Champ_base::valeur_aux_bords() : le_champ_convecte_ou_inc->valeur_aux_bords();
          for (int face = ndeb; face < nfin; face++)
            {
              const int local_face = le_dom->front_VF(boundary_index).num_local_face(face);
              flux_evaluateur.coeffs_face_bloc_vitesse(donnee, val_b, boundary_index, face, local_face, cl, ndeb, aef);

              for (int i = 0; i < 2; i++)
                if ((e = elem(face, i)) >= 0)
                  for (int n = 0, m = 0; n < N; n++, m += (Mv > 1)) (*m_vit)(N * e + n, Mv * face + m) += (i ? -1.0 : 1.0) * aef(n);
            }
        }

      //derivees : champ convecte
      if (mat || d_cc.size() > 0)
        for (int face = ndeb; face < nfin; face++)
          {
            const int local_face = le_dom->front_VF(boundary_index).num_local_face(face);
            flux_evaluateur.coeffs_face(boundary_index, face, local_face, ndeb, cl, aii, ajj);
            fill_coeffs_matrices(face, aii, ajj, mat, d_cc); // XXX : Attention Yannick pour d_cc c'est pas tout a fait comme avant ... N et M ...
          }
    }
}

template<class _TYPE_>
inline void Iterateur_VDF_Elem<_TYPE_>::fill_derivee_cc(matrices_t mats, const tabs_t& semi_impl, VectorDeriv& d_cc) const
{
  //liste des derivees de cc a renseigner : couples (derivee de cc, matrice, nb de compos de la variable)
  if (is_pb_multiphase() && is_convective_op() && !semi_impl.count(le_champ_convecte_ou_inc->le_nom().getString()))
    {
      for (auto &i_m : mats)
        if (le_champ_convecte_ou_inc->derivees().count(i_m.first))
          d_cc.push_back(std::make_tuple(&le_champ_convecte_ou_inc->derivees().at(i_m.first), i_m.second, op_base->equation().probleme().get_champ(i_m.first.c_str()).valeurs().line_size()));
    }
  else assert(d_cc.size() == 0);
}

template<class _TYPE_> template<typename Type_Double>
inline void Iterateur_VDF_Elem<_TYPE_>::fill_coeffs_matrices(const int f, const double coeff, Type_Double& aii, Type_Double& ajj, Matrice_Morse *mat, VectorDeriv& d_cc) const
{
  const int multi = multiscalar_, N = multi ? int(sqrt(aii.size_array())) : aii.size_array();
  if (mat)
    {
      for (int i = 0; i < 2; i++)
        for (int j = 0, e = elem(f, i); j < 2; j++)
          for (int n = 0, eb = elem(f, j); n < N; n++)
            for (int m = (multi ? 0 : n); m < (multi ? N : n + 1); m++)
              (*mat)(N * e + n, N * eb + m) += (i == j ? 1.0 : -1.0) * coeff * (j ? ajj[multi ? N * n + m : n] : aii[multi ? N * n + m : n]);
    }
  else
    for (auto &&d_m_i : d_cc)
      for (int i = 0; i < 2; i++)
        for (int j = 0, e = elem(f, i); j < 2; j++)
          {
            const int M = std::get<2> (d_m_i);
            const DoubleTab& d_var_cc = *std::get<0> (d_m_i);
            Matrice_Morse& d_var_operateur = *std::get<1> (d_m_i);

            for (int n = 0, m = 0, eb = elem(f, j); n < N; n++, m += (M > 1))
              d_var_operateur(N * e + n, M * eb + m) += (i == j ? 1.0 : -1.0) * coeff * (j ? ajj[n] : aii[n]) * d_var_cc(eb, m);
          }
}

template<class _TYPE_> template<typename Type_Double>
inline void Iterateur_VDF_Elem<_TYPE_>::fill_coeffs_matrices(const int face, Type_Double& aii, Type_Double& ajj, Matrice_Morse *mat, VectorDeriv& d_cc) const
{
  const int e0 = elem(face, 0), e1 = elem(face, 1);
  const int multi = multiscalar_, N = multi ? int(sqrt(aii.size_array())) : aii.size_array();

  if (mat)
    {
      if (e0 > -1)
        for (int n = 0; n < N; n++)
          for (int m = (multi ? 0 : n); m < (multi ? N : n + 1); m++)
            (*mat)(e0 * N + n, e0 * N + m) += aii[multi ? N * n + m : n];
      if (e1 > -1)
        for (int n = 0; n < N; n++)
          for (int m = (multi ? 0 : n); m < (multi ? N : n + 1); m++)
            (*mat)(e1 * N + n, e1 * N + m) += ajj[multi ? N * n + m : n];
    }
  else
    for (auto &&d_m_i : d_cc)
      {
        const int M = std::get<2> (d_m_i);
        const DoubleTab& d_var_cc = *std::get<0> (d_m_i);
        Matrice_Morse& d_var_operateur = *std::get<1> (d_m_i);

        if (e0 > -1)
          for (int n = 0, m = 0; n < N; n++, m += (M > 1))
            {
              const int i0 = N * e0 + n;
              d_var_operateur(i0, i0) += aii[n] * d_var_cc(e0, m);
            }
        if (e1 > -1)
          for (int n = 0, m = 0; n < N; n++, m += (M > 1))
            {
              const int j0 = M * e1 + m;
              d_var_operateur(j0, j0) += ajj[n] * d_var_cc(e1, m);
            }
      }
}

#include <Iterateur_VDF_Elem_bis.tpp>
#include <Iterateur_VDF_Elem_FT_TCL.tpp> // pour FT ...

#endif /* Iterateur_VDF_Elem_TPP_included */
