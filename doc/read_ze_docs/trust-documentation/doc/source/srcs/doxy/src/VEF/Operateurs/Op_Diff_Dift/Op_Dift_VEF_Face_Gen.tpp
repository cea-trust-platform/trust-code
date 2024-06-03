/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Op_Dift_VEF_Face_Gen_TPP_included
#define Op_Dift_VEF_Face_Gen_TPP_included

#include <Modele_turbulence_scal_base.h>
#include <Echange_externe_impose.h>
#include <Scalaire_impose_paroi.h>
#include <Neumann_sortie_libre.h>
#include <Op_Diff_VEF_base.h>
#include <Neumann_homogene.h>
#include <Neumann_paroi.h>
#include <Periodique.h>
#include <Champ_P1NC.h>
#include <Symetrie.h>

template <typename DERIVED_T> template <Type_Champ _TYPE_>
void Op_Dift_VEF_Face_Gen<DERIVED_T>::fill_grad_Re(const DoubleTab& inconnue, const DoubleTab& resu, const DoubleTab& nu, const DoubleTab& nu_turb) const
{
  constexpr bool is_VECT = (_TYPE_ == Type_Champ::VECTORIEL);
  if (is_VECT)
    {
      const auto *z_class = static_cast<const DERIVED_T*>(this); // CRTP --> I love you :*

      const Domaine_Cl_VEF& domaine_Cl_VEF = z_class->domaine_cl_vef();
      const Domaine_VEF& domaine_VEF = z_class->domaine_vef();
      const int nbr_comp = resu.line_size(), nb_elem = domaine_VEF.nb_elem();
      assert(nbr_comp > 1);

      // Construction du tableau grad_ si necessaire
      if (!grad_.get_md_vector().non_nul())
        {
          grad_.resize(0, Objet_U::dimension, Objet_U::dimension);
          domaine_VEF.domaine().creer_tableau_elements(grad_);
        }
      grad_ = 0.;

      Champ_P1NC::calcul_gradient(inconnue, grad_, domaine_Cl_VEF);

      if (z_class->get_modele_turbulence().utiliser_loi_paroi())
        Champ_P1NC::calcul_duidxj_paroi(grad_, nu, nu_turb, z_class->get_tau_tan(), domaine_Cl_VEF);

      grad_.echange_espace_virtuel();

      if (!Re_.get_md_vector().non_nul())
        {
          Re_.resize(0, Objet_U::dimension, Objet_U::dimension);
          domaine_VEF.domaine().creer_tableau_elements(Re_);
        }
      Re_ = 0.;

      if (z_class->get_modele_turbulence().calcul_tenseur_Re(nu_turb, grad_, Re_))
        {
          Cerr << "On utilise une diffusion turbulente non linaire dans NS" << finl;
          for (int elem = 0; elem < nb_elem; elem++)
            for (int i = 0; i < nbr_comp; i++)
              for (int j = 0; j < nbr_comp; j++)
                Re_(elem, i, j) *= nu_turb[elem];
        }
      else
        for (int elem = 0; elem < nb_elem; elem++)
          for (int i = 0; i < nbr_comp; i++)
            for (int j = 0; j < nbr_comp; j++)
              Re_(elem, i, j) = nu_turb[elem] * (grad_(elem, i, j) + grad_(elem, j, i));

      Re_.echange_espace_virtuel();
    }
}

/*
 * ***************************
 *  METHODES POUR L'EXPLICITE
 * ***************************
 */

template <typename DERIVED_T> template<Type_Champ _TYPE_>
std::enable_if_t<_TYPE_ == Type_Champ::VECTORIEL, void>
Op_Dift_VEF_Face_Gen<DERIVED_T>::ajouter_bord_gen(const DoubleTab& inconnue, DoubleTab& resu, DoubleTab& tab_flux_bords, const DoubleTab& nu, const DoubleTab& nu_turb) const
{
  const auto *z_class = static_cast<const DERIVED_T*>(this); // CRTP --> I love you :*

  const Domaine_Cl_VEF& domaine_Cl_VEF = z_class->domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = z_class->domaine_vef();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleTab& face_normale = domaine_VEF.face_normales();
  const int nbr_comp = resu.line_size();

  // boucle sur les CL
  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  const int nb_cl = les_cl.size();
  for (int n_bord = 0; n_bord < nb_cl; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
      const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Periodique, la_cl.valeur()))
        for (int num_face = ndeb; num_face < nfin; num_face++)
          for (int kk = 0; kk < 2; kk++)
            {
              const int elem = face_voisins(num_face, kk), ori = 1 - 2 * kk;
              for (int i = 0; i < nbr_comp; i++)
                for (int j = 0; j < nbr_comp; j++)
                  resu(num_face, i) -= ori * face_normale(num_face, j) * (nu[elem] * grad_(elem, i, j) + Re_(elem, i, j));
            }
      else // CL pas periodique
        for (int num_face = ndeb; num_face < nfin; num_face++)
          {
            const int elem = face_voisins(num_face, 0);
            for (int i = 0; i < nbr_comp; i++)
              for (int j = 0; j < nbr_comp; j++)
                {
                  double flux = face_normale(num_face, j) * (nu[elem] * grad_(elem, i, j) + Re_(elem, i, j));
                  resu(num_face, i) -= flux;
                  tab_flux_bords(num_face, i) -= flux;
                }

            // Correction tab_flux_bords si symetrie
            if (sub_type(Symetrie, la_cl.valeur()))
              tab_flux_bords(num_face, 0) = 0.;
          }
    }
}

template <typename DERIVED_T> template<Type_Champ _TYPE_>
std::enable_if_t<_TYPE_ == Type_Champ::VECTORIEL, void>
Op_Dift_VEF_Face_Gen<DERIVED_T>::ajouter_interne_gen(const DoubleTab& inconnue, DoubleTab& resu, DoubleTab& flux_bords, const DoubleTab& nu, const DoubleTab& nu_turb) const
{
  const Domaine_VEF& domaine_VEF = static_cast<const DERIVED_T*>(this)->domaine_vef();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleTab& face_normale = domaine_VEF.face_normales();
  const int nb_faces = domaine_VEF.nb_faces(), nbr_comp = resu.line_size();

  // Boucle sur les faces internes
  const int nint = domaine_VEF.premiere_face_int();
  for (int num_face = nint; num_face < nb_faces; num_face++)
    for (int kk = 0; kk < 2; kk++)
      {
        const int elem = face_voisins(num_face, kk), ori = 1 - 2 * kk;
        for (int i = 0; i < nbr_comp; i++)
          for (int j = 0; j < nbr_comp; j++)
            resu(num_face, i) -= ori * face_normale(num_face, j) * (nu[elem] * grad_(elem, i, j) + Re_(elem, i, j));
      }
}

template <typename DERIVED_T> template<Type_Champ _TYPE_>
std::enable_if_t<_TYPE_ == Type_Champ::SCALAIRE, void>
Op_Dift_VEF_Face_Gen<DERIVED_T>::ajouter_bord_gen(const DoubleTab& inconnue, DoubleTab& resu, DoubleTab& tab_flux_bords, const DoubleTab& nu, const DoubleTab& nu_turb) const
{
  // On traite les faces bord
  const auto *z_class = static_cast<const DERIVED_T*>(this); // CRTP --> I love you :*

  const Domaine_Cl_VEF& domaine_Cl_VEF = z_class->domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = z_class->domaine_vef();
  const int nb_front = domaine_VEF.nb_front_Cl();

  for (int n_bord = 0; n_bord < nb_front; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique, la_cl.valeur()))
        ajouter_bord_perio_gen__<_TYPE_, Type_Schema::EXPLICITE, false>(n_bord, inconnue, &resu, nullptr, nu, nu_turb, nu_turb /* poubelle */);
      else // CL pas periodique
        {
          if (sub_type(Scalaire_impose_paroi, la_cl.valeur()) || sub_type(Neumann_paroi, la_cl.valeur()) || sub_type(Neumann_homogene, la_cl.valeur())) // CL Temperature imposee
            ajouter_bord_scalaire_impose_gen__<_TYPE_, Type_Schema::EXPLICITE, false>(n_bord, inconnue, &resu, nullptr, nu, nu_turb, nu_turb /* poubelle */, &tab_flux_bords);

          // A pas oublier !
          ajouter_bord_gen__<_TYPE_, Type_Schema::EXPLICITE, false>(n_bord, inconnue, &resu, nullptr, nu, nu_turb, nu_turb /* poubelle */, &tab_flux_bords);
        }
    }
  modifie_pour_cl_gen<false>(inconnue, resu, tab_flux_bords);
}

template <typename DERIVED_T> template <bool _IS_STAB_>
void Op_Dift_VEF_Face_Gen<DERIVED_T>::modifie_pour_cl_gen(const DoubleTab& inconnue, DoubleTab& resu, DoubleTab& tab_flux_bords) const
{
  // On traite les faces bord
  const auto *z_class = static_cast<const DERIVED_T*>(this); // CRTP --> I love you :*
  constexpr bool is_STAB = _IS_STAB_;

  const Domaine_Cl_VEF& domaine_Cl_VEF = z_class->domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = z_class->domaine_vef();
  const int nb_front = domaine_VEF.nb_front_Cl(), nb_comp = resu.line_size();

  for (int n_bord = 0; n_bord < nb_front; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
      const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

      if (is_STAB && sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());

          for (int ind_face = 0; ind_face < le_bord.nb_faces(); ind_face++)
            {
              const int face = le_bord.num_face(ind_face), face_associee = le_bord.num_face(la_cl_perio.face_associee(ind_face));
              if (face < face_associee)
                for (int nc = 0; nc < nb_comp; nc++)
                  {
                    resu(face, nc) += resu(face_associee, nc);
                    resu(face_associee, nc) = resu(face, nc);
                  }
            }
        }

      if (sub_type(Neumann_paroi, la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          for (int face = ndeb; face < nfin; face++)
            for (int nc = 0; nc < nb_comp; nc++)
              {
                const double flux = la_cl_paroi.flux_impose(face - ndeb, nc) * domaine_VEF.face_surfaces(face);
                if (is_STAB) resu(face, nc) -= flux; // XXX -= car regarde dans Op_Dift_Stab_VEF_Face::ajouter
                else resu(face, nc) += flux;
                tab_flux_bords(face, nc) = flux;
              }
        }

      if (sub_type(Echange_externe_impose, la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          for (int face = ndeb; face < nfin; face++)
            for (int nc = 0; nc < nb_comp; nc++)
              {
                const double flux = la_cl_paroi.h_imp(face - ndeb, nc) * (la_cl_paroi.T_ext(face - ndeb, nc) - inconnue(face, nc)) * domaine_VEF.face_surfaces(face);
                if (is_STAB) resu(face, nc) -= flux;
                else resu(face, nc) += flux;
                tab_flux_bords(face, nc) = flux;
              }
        }

      if (sub_type(Neumann_homogene,la_cl.valeur()) || sub_type(Symetrie, la_cl.valeur()) || sub_type(Neumann_sortie_libre, la_cl.valeur()))
        for (int face = ndeb; face < nfin; face++)
          for (int nc = 0; nc < nb_comp; nc++)
            tab_flux_bords(face, nc) = 0.;
    }
}

/*
 * ***************************
 *  METHODES POUR L'IMPLICITE
 * ***************************
 */
template <typename DERIVED_T> template <Type_Champ _TYPE_, bool _IS_STAB_>
void Op_Dift_VEF_Face_Gen<DERIVED_T>::ajouter_contribution_bord_gen(const DoubleTab& transporte, Matrice_Morse& matrice, const DoubleTab& nu,
                                                                    const DoubleTab& nu_turb, const DoubleVect& porosite_eventuelle) const
{
  // On traite les faces bord
  const auto *z_class = static_cast<const DERIVED_T*>(this); // CRTP --> I love you :*

  const Domaine_Cl_VEF& domaine_Cl_VEF = z_class->domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = z_class->domaine_vef();
  const int nb_bords = domaine_VEF.nb_front_Cl(), nb_comp = transporte.line_size();

  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());

      if (sub_type(Periodique, la_cl.valeur()))
        ajouter_bord_perio_gen__<_TYPE_, Type_Schema::IMPLICITE, _IS_STAB_>(n_bord, transporte, nullptr, &matrice, nu, nu_turb, porosite_eventuelle);
      else // pas perio
        {
          if (sub_type(Scalaire_impose_paroi, la_cl.valeur())) // CL Temperature imposee
            ajouter_bord_scalaire_impose_gen__<_TYPE_, Type_Schema::IMPLICITE, _IS_STAB_>(n_bord, transporte, nullptr, &matrice, nu, nu_turb, porosite_eventuelle);
          else if (sub_type(Echange_externe_impose, la_cl.valeur()) && nb_comp < 2) // XXX : plus tard pour multi inco aussi ...
            {
              const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
              const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
              for (int face = ndeb; face < nfin; face++)
                matrice(face, face) += la_cl_paroi.h_imp(face - ndeb) * domaine_VEF.face_surfaces(face);
            }

          // A pas oublier !
          ajouter_bord_gen__<_TYPE_, Type_Schema::IMPLICITE, _IS_STAB_>(n_bord, transporte, nullptr, &matrice, nu, nu_turb, porosite_eventuelle);
        }
    }
}

// METHODES GENERIQUES
template <typename DERIVED_T> template <Type_Champ _TYPE_, Type_Schema _SCHEMA_, bool _IS_STAB_>
void Op_Dift_VEF_Face_Gen<DERIVED_T>::ajouter_bord_perio_gen__(const int n_bord, const DoubleTab& inconnue, DoubleTab* resu /* Si explicite */, Matrice_Morse* matrice /* Si implicite */,
                                                               const DoubleTab& nu, const DoubleTab& nu_turb, const DoubleVect& porosite_eventuelle, DoubleTab* flux_bord) const
{
  constexpr bool is_VECT = (_TYPE_ == Type_Champ::VECTORIEL), is_EXPLICIT = (_SCHEMA_ == Type_Schema::EXPLICITE), is_STAB = _IS_STAB_;

  const auto *z_class = static_cast<const DERIVED_T*>(this); // CRTP --> I love you :*

  const Domaine_Cl_VEF& domaine_Cl_VEF = z_class->domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = z_class->domaine_vef();
  const IntTab& elem_faces = domaine_VEF.elem_faces(), &face_voisins = domaine_VEF.face_voisins();
  const DoubleVect& volumes = domaine_VEF.volumes();
  const DoubleTab& face_normale = domaine_VEF.face_normales();

  const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
  const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
  const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
  const int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), nb_faces = domaine_VEF.nb_faces(), nb_comp = inconnue.line_size();
  int num1 = 0, num2 = le_bord.nb_faces_tot(), nb_faces_bord_reel = le_bord.nb_faces();

  // on ne parcourt que la moitie des faces volontairement ... GF il ne faut pas s'occuper des faces virtuelles
  num2 = is_EXPLICIT ? num2 : nb_faces_bord_reel / 2; // XXX : attention si ecarts car version multicompo, num2 /= 2 .... et aussi je garde l'explicite comme num2

  for (int ind_face = num1; ind_face < num2; ind_face++)
    {
      int fac_asso = la_cl_perio.face_associee(ind_face);
      fac_asso = le_bord.num_face(fac_asso);
      int num_face0 = le_bord.num_face(ind_face);

      for (int l = 0; l < 2; l++)
        {
          int elem0 = face_voisins(num_face0, l);
          for (int i0 = 0; i0 < nb_faces_elem; i0++)
            {
              int j = elem_faces(elem0, i0);

              if (is_EXPLICIT)
                {
                  assert(resu != nullptr);
                  assert (!is_VECT && !_IS_STAB_);
                  if ((j > num_face0) && (j != fac_asso))
                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        const double d_nu = nu(elem0, nc) + nu_turb(elem0);
                        const double valA = z_class->viscA(num_face0, j, elem0, d_nu);
                        const double flux = valA * inconnue(j, nc) - valA * inconnue(num_face0, nc);
                        (*resu)(num_face0, nc) += flux;
                        if (j < nb_faces) // face reelle
                          (*resu)(j, nc) -= 0.5 * flux;
                      }
                }
              else // pour l'implicite
                {
                  assert(matrice != nullptr);
                  if (j > num_face0)
                    {
                      int orientation = 1, fac_loc = 0, ok = 1, contrib = 1;

                      if ((elem0 == face_voisins(j, l)) || (face_voisins(num_face0, (l + 1) % 2) == face_voisins(j, (l + 1) % 2)))
                        orientation = -1;

                      while ((fac_loc < nb_faces_elem) && (elem_faces(elem0, fac_loc) != num_face0))
                        fac_loc++;

                      if (fac_loc == nb_faces_elem)
                        ok = 0;

                      if (j >= nb_faces) // C'est une face virtuelle
                        {
                          int el1 = face_voisins(j, 0), el2 = face_voisins(j, 1);
                          if ((el1 == -1) || (el2 == -1))
                            contrib = 0;
                        }

                      if (contrib)
                        for (int nc = 0; nc < nb_comp; nc++)
                          {
                            double d_nu = nu(elem0, is_VECT ? 0 : nc) + nu_turb(elem0);
                            double valA = z_class->viscA(num_face0, j, elem0, d_nu);
                            if (is_STAB && valA < 0.)
                              valA = 0.;

                            int n0 = num_face0 * nb_comp + nc;
                            int n0perio = fac_asso * nb_comp + nc;
                            int j0 = j * nb_comp + nc;

                            (*matrice)(n0, n0) += valA * porosite_eventuelle(num_face0);
                            (*matrice)(n0, j0) -= valA * porosite_eventuelle(j);

                            if (j < nb_faces) // On traite les faces reelles
                              {
                                if (ok == 1)
                                  (*matrice)(j0, n0) -= valA * porosite_eventuelle(num_face0);
                                else
                                  (*matrice)(j0, n0perio) -= valA * porosite_eventuelle(num_face0);

                                (*matrice)(j0, j0) += valA * porosite_eventuelle(j);
                              }

                            // XXX : On a l'equation QDM et donc on ajoute grad_U transpose
                            if (is_VECT)
                              for (int nc2 = 0; nc2 < nb_comp; nc2++)
                                {
                                  int n1 = num_face0 * nb_comp + nc2;
                                  int j1 = j * nb_comp + nc2;
                                  double coeff_s = orientation * nu_turb(elem0) / volumes(elem0) * face_normale(num_face0, nc2) * face_normale(j, nc);
                                  (*matrice)(n0, n1) += coeff_s * porosite_eventuelle(num_face0);
                                  (*matrice)(n0, j1) -= coeff_s * porosite_eventuelle(j);

                                  if (j < nb_faces) // On traite les faces reelles
                                    {
                                      double coeff_s2 = orientation * nu_turb(elem0) / volumes(elem0) * face_normale(num_face0, nc) * face_normale(j, nc2);

                                      if (ok == 1)
                                        (*matrice)(j0, n1) -= coeff_s2 * porosite_eventuelle(num_face0);
                                      else
                                        (*matrice)(j0, fac_asso * nb_comp + nc2) -= coeff_s2 * porosite_eventuelle(num_face0);

                                      (*matrice)(j0, j1) += coeff_s2 * porosite_eventuelle(j);
                                    }
                                }
                          }
                    }
                }
            }
        }

    }
}

template <typename DERIVED_T> template <Type_Champ _TYPE_, Type_Schema _SCHEMA_, bool _IS_STAB_>
void Op_Dift_VEF_Face_Gen<DERIVED_T>::ajouter_bord_scalaire_impose_gen__(const int n_bord, const DoubleTab& inconnue, DoubleTab* resu /* Si explicite */, Matrice_Morse* matrice /* Si implicite */,
                                                                         const DoubleTab& nu, const DoubleTab& nu_turb, const DoubleVect& porosite_eventuelle, DoubleTab* tab_flux_bords) const
{
  constexpr bool is_EXPLICIT = (_SCHEMA_ == Type_Schema::EXPLICITE);

  const auto *z_class = static_cast<const DERIVED_T*>(this); // CRTP --> I love you :*

  const Domaine_Cl_VEF& domaine_Cl_VEF = z_class->domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = z_class->domaine_vef();
  const Front_VF& le_bord = ref_cast(Front_VF, domaine_Cl_VEF.les_conditions_limites(n_bord).frontiere_dis());
  const RefObjU& modele_turbulence = domaine_Cl_VEF.equation().get_modele(TURBULENCE);

  const IntTab& elem_faces = domaine_VEF.elem_faces(), &face_voisins = domaine_VEF.face_voisins();
  const DoubleVect& volumes = domaine_VEF.volumes();
  const DoubleTab& face_normale = domaine_VEF.face_normales();

  const int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), nb_comp = inconnue.line_size(), size_flux_bords = domaine_VEF.nb_faces_bord();
  int num1 = 0, num2 = le_bord.nb_faces_tot();

  if (sub_type(Modele_turbulence_scal_base, modele_turbulence.valeur()))
    {
      const Modele_turbulence_scal_base& mod_turb_scal = ref_cast(Modele_turbulence_scal_base, modele_turbulence.valeur());
      const Turbulence_paroi_scal& loiparth = mod_turb_scal.loi_paroi();
      if (loiparth->use_equivalent_distance())
        {
          const DoubleVect& d_equiv = loiparth->equivalent_distance(n_bord);
          // d_equiv contient la distance equivalente pour le bord
          // Dans d_equiv, pour les faces qui ne sont pas paroi_fixe (eg periodique, symetrie, etc...)
          // il y a la distance geometrique grace a l'initialisation du tableau dans la loi de paroi.

          // Les lois de parois ne s'appliquent qu'aux cas ou la CL est de type temperature imposee, car dans les autres cas
          // (flux impose et adiabatique) le flux a la paroi est connu et fixe.
          int nb_dim_pb = Objet_U::dimension;

          const Cond_lim_base& cl_base = domaine_Cl_VEF.les_conditions_limites(n_bord).valeur();

          int ldp_appli = 0;

          if (sub_type(Scalaire_impose_paroi, cl_base))
            ldp_appli = 1;
          else if (loiparth->get_flag_calcul_ldp_en_flux_impose())
            if ((sub_type(Neumann_paroi, cl_base)) || (sub_type(Neumann_homogene, cl_base)))
              ldp_appli = 1;

          if (ldp_appli)
            {
              DoubleVect le_mauvais_gradient(nb_dim_pb);
              for (int ind_face = num1; ind_face < num2; ind_face++)
                for (int nc = 0; nc < nb_comp; nc++)
                  {
                    int num_face = le_bord.num_face(ind_face);
                    // Tf est la temperature fluide moyenne dans le premier element sans tenir compte de la temperature de paroi.
                    double Tf = 0.;
                    double bon_gradient = 0.; // c'est la norme du gradient de temperature normal a la paroi
                    le_mauvais_gradient = 0.;

                    int elem1 = face_voisins(num_face, 0);
                    if (elem1 == -1) elem1 = face_voisins(num_face, 1);

                    // inconnue(num_face) est la temperature de paroi : Tw.
                    // On se fiche du signe de bon gradient car c'est la norme du gradient de temperature dans l'element.
                    // Ensuite ce sera multiplie par le vecteur normal a la face de paroi qui lui a les bons signes.

                    if (!is_EXPLICIT)
                      bon_gradient = 1. / d_equiv(ind_face) * (-domaine_VEF.oriente_normale(num_face, elem1));

                    double surface_face = domaine_VEF.face_surfaces(num_face),
                           nutotal = nu(elem1, nc) + nu_turb(elem1);

                    if (is_EXPLICIT)
                      {
                        assert(resu != nullptr);
                        for (int i = 0; i < nb_faces_elem; i++)
                          {
                            const int j = elem_faces(elem1, i);
                            if (j != num_face)
                              {
                                double surface_pond = 0.;
                                for (int kk = 0; kk < nb_dim_pb; kk++)
                                  surface_pond -= (face_normale(j, kk) * domaine_VEF.oriente_normale(j, elem1) * face_normale(num_face, kk) * domaine_VEF.oriente_normale(num_face, elem1)) / (surface_face * surface_face);

                                Tf += inconnue(j, nc) * surface_pond;
                              }

                            for (int kk = 0; kk < nb_dim_pb; kk++)
                              le_mauvais_gradient(kk) += inconnue(j, nc) * face_normale(j, kk) * domaine_VEF.oriente_normale(j, elem1);
                          }
                        le_mauvais_gradient /= volumes(elem1);

                        double mauvais_gradient = 0;
                        for (int kk = 0; kk < nb_dim_pb; kk++)
                          mauvais_gradient += le_mauvais_gradient(kk) * face_normale(num_face, kk) / surface_face;

                        // inconnue(num_face) est la temperature de paroi : Tw.
                        // On se fiche du signe de bon gradient car c'est la norme du gradient de temperature dans l'element.
                        // Ensuite ce sera multiplie par le vecteur normal a la face de paroi qui lui a les bons signes.
                        bon_gradient = (Tf - inconnue(num_face, nc)) / d_equiv(ind_face) * (-domaine_VEF.oriente_normale(num_face, elem1));

                        for (int i = 0; i < nb_faces_elem; i++)
                          {
                            const int j = elem_faces(elem1, i);
                            double correction = 0.;
                            for (int kk = 0; kk < nb_dim_pb; kk++)
                              {
                                double resu2 = nutotal * (bon_gradient - mauvais_gradient) * face_normale(num_face, kk) * face_normale(j, kk) * (-domaine_VEF.oriente_normale(j, elem1)) / surface_face;
                                correction += resu2;
                              }

                            (*resu)(j, nc) += correction;

                            if (j == num_face && j < size_flux_bords)
                              (*tab_flux_bords)(j, nc) -= correction;
                          }
                      }
                    else // implicite
                      {
                        assert(matrice != nullptr);
                        for (int i0 = 0; i0 < nb_faces_elem; i0++)
                          {
                            int j = elem_faces(elem1, i0);
                            for (int ii = 0; ii < nb_faces_elem; ii++)
                              {
                                le_mauvais_gradient = 0;
                                int jj = elem_faces(elem1, ii);
                                double surface_pond = 0;
                                for (int kk = 0; kk < nb_dim_pb; kk++)
                                  surface_pond -= (face_normale(jj, kk) * domaine_VEF.oriente_normale(jj, elem1) * face_normale(num_face, kk) * domaine_VEF.oriente_normale(num_face, elem1)) / (surface_face * surface_face);

                                Tf = surface_pond;
                                for (int kk = 0; kk < nb_dim_pb; kk++)
                                  le_mauvais_gradient(kk) += face_normale(jj, kk) * domaine_VEF.oriente_normale(jj, elem1);

                                le_mauvais_gradient /= volumes(elem1);
                                double mauvais_gradient = 0;

                                for (int kk = 0; kk < nb_dim_pb; kk++)
                                  mauvais_gradient += le_mauvais_gradient(kk) * face_normale(num_face, kk) / surface_face;

                                double resu1 = 0, resu2 = 0;
                                for (int kk = 0; kk < nb_dim_pb; kk++)
                                  {
                                    resu1 += nutotal * mauvais_gradient * face_normale(num_face, kk) * face_normale(j, kk) * (-domaine_VEF.oriente_normale(j, elem1)) / surface_face;
                                    resu2 += nutotal * bon_gradient * face_normale(num_face, kk) * face_normale(j, kk) * (-domaine_VEF.oriente_normale(j, elem1)) / surface_face;
                                  }
                                // bon gradient_reel = bongradient*(Tf-T_face) d'ou les derivees... & mauvais gradient_reel=mauvai_gradient_j*Tj
                                if (jj == num_face)
                                  resu2 *= -1;
                                else
                                  resu2 *= Tf;

                                int j0 = j * nb_comp + nc, jj0 = jj * nb_comp + nc;

                                (*matrice)(j0, jj0) += (resu1 - resu2) * porosite_eventuelle(jj0);
                              }
                          }
                      }
                  }
            }
        }
    }
}

template <typename DERIVED_T> template <Type_Champ _TYPE_, Type_Schema _SCHEMA_, bool _IS_STAB_>
void Op_Dift_VEF_Face_Gen<DERIVED_T>::ajouter_bord_gen__(const int n_bord, const DoubleTab& inconnue, DoubleTab* resu /* Si explicite */, Matrice_Morse* matrice /* Si implicite */,
                                                         const DoubleTab& nu, const DoubleTab& nu_turb, const DoubleVect& porosite_eventuelle, DoubleTab* tab_flux_bords) const
{
  constexpr bool is_VECT = (_TYPE_ == Type_Champ::VECTORIEL), is_EXPLICIT = (_SCHEMA_ == Type_Schema::EXPLICITE), is_STAB = _IS_STAB_;

  const auto *z_class = static_cast<const DERIVED_T*>(this); // CRTP --> I love you :*

  const Domaine_Cl_VEF& domaine_Cl_VEF = z_class->domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = z_class->domaine_vef();
  const IntTab& elem_faces = domaine_VEF.elem_faces(), &face_voisins = domaine_VEF.face_voisins();
  const DoubleVect& volumes = domaine_VEF.volumes();
  const DoubleTab& face_normale = domaine_VEF.face_normales();
  const int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), nb_faces = domaine_VEF.nb_faces(), nb_comp = inconnue.line_size(), premiere_face_int = domaine_VEF.premiere_face_int();;

  const Front_VF& le_bord = ref_cast(Front_VF, domaine_Cl_VEF.les_conditions_limites(n_bord).frontiere_dis());
  const int num1 = 0, num2 = le_bord.nb_faces_tot(), nb_faces_bord_reel = le_bord.nb_faces();

  for (int ind_face = num1; ind_face < num2; ind_face++)
    {
      const int num_face = le_bord.num_face(ind_face), elem = face_voisins(num_face, 0);
      for (int i = 0; i < nb_faces_elem; i++)
        {
          int j = elem_faces(elem, i);
          if ((j > num_face) || (ind_face >= nb_faces_bord_reel))
            {
              int orientation = 1;
              if ((elem == face_voisins(j, 0)) || (face_voisins(num_face, (0 + 1) % 2) == face_voisins(j, (0 + 1) % 2)))
                orientation = -1;

              for (int nc = 0; nc < nb_comp; nc++)
                {
                  const double d_nu = nu(elem, is_VECT ? 0 : nc) + nu_turb(elem);
                  double valA = z_class->viscA(num_face, j, elem, d_nu);

                  if (is_STAB && valA < 0.) valA = 0.;

                  if (is_EXPLICIT)
                    {
                      assert(resu != nullptr);
                      if (ind_face < nb_faces_bord_reel)
                        {
                          double flux = valA * (inconnue(j, nc) - inconnue(num_face, nc));
                          (*resu)(num_face, nc) += flux;
                          (*tab_flux_bords)(num_face, nc) -= flux;
                        }
                      if (j < nb_faces) // face reelle
                        {
                          double flux = valA * (inconnue(num_face, nc) - inconnue(j, nc));
                          (*resu)(j, nc) += flux;
                          if (j < premiere_face_int)
                            (*tab_flux_bords)(j, nc) -= flux;
                        }
                    }
                  else // implicite
                    {
                      assert(matrice != nullptr);
                      const int n0 = num_face * nb_comp + nc, j0 = j * nb_comp + nc;
                      if (ind_face < nb_faces_bord_reel)
                        {
                          (*matrice)(n0, n0) += valA * porosite_eventuelle(num_face);
                          (*matrice)(n0, j0) -= valA * porosite_eventuelle(j);
                        }

                      if (j < nb_faces) // face reelle
                        {
                          (*matrice)(j0, n0) -= valA * porosite_eventuelle(num_face);
                          (*matrice)(j0, j0) += valA * porosite_eventuelle(j);
                        }

                      // XXX : On a l'equation QDM et donc on ajoute grad_U transpose
                      if (is_VECT)
                        for (int nc2 = 0; nc2 < nb_comp; nc2++)
                          {
                            const int n1 = num_face * nb_comp + nc2, j1 = j * nb_comp + nc2;
                            if (ind_face < nb_faces_bord_reel)
                              {
                                double coeff_s = orientation * nu_turb(elem) / volumes(elem) * face_normale(num_face, nc2) * face_normale(j, nc);
                                (*matrice)(n0, n1) += coeff_s * porosite_eventuelle(num_face);
                                (*matrice)(n0, j1) -= coeff_s * porosite_eventuelle(j);
                              }

                            if (j < nb_faces) // face reelle
                              {
                                double coeff_s = orientation * nu_turb(elem) / volumes(elem) * face_normale(num_face, nc) * face_normale(j, nc2);
                                (*matrice)(j0, n1) -= coeff_s * porosite_eventuelle(num_face);
                                (*matrice)(j0, j1) += coeff_s * porosite_eventuelle(j);
                              }
                          }
                    }
                }
            }
        }
    }
}

template <typename DERIVED_T> template <Type_Champ _TYPE_, Type_Schema _SCHEMA_, bool _IS_STAB_>
void Op_Dift_VEF_Face_Gen<DERIVED_T>::ajouter_interne_gen__(const DoubleTab& inconnue, DoubleTab* resu /* Si explicite */, Matrice_Morse* matrice /* Si implicite */,
                                                            const DoubleTab& nu, const DoubleTab& nu_turb, const DoubleVect& porosite_eventuelle) const
{
  constexpr bool is_VECT = (_TYPE_ == Type_Champ::VECTORIEL), is_EXPLICIT = (_SCHEMA_ == Type_Schema::EXPLICITE), is_STAB = _IS_STAB_;

  const auto *z_class = static_cast<const DERIVED_T*>(this); // CRTP --> I love you :*

  const Domaine_VEF& domaine_VEF = z_class->domaine_vef();
  const IntTab& face_voisins = domaine_VEF.face_voisins(), &elem_faces = domaine_VEF.elem_faces();
  const DoubleVect& volumes = domaine_VEF.volumes();
  const DoubleTab& face_normale = domaine_VEF.face_normales();
  const int premiere_face_int = domaine_VEF.premiere_face_int(), nb_faces = domaine_VEF.nb_faces(), nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), nb_comp = inconnue.line_size();

  for (int num_face0 = premiere_face_int; num_face0 < nb_faces; num_face0++)
    for (int kk = 0; kk < 2; kk++)
      {
        int elem0 = face_voisins(num_face0, kk);
        for (int i0 = 0; i0 < nb_faces_elem; i0++)
          {
            int j = elem_faces(elem0, i0);
            if (j > num_face0)
              {
                int contrib = 1;
                if (j >= nb_faces) // C'est une face virtuelle
                  {
                    const int el1 = face_voisins(j, 0), el2 = face_voisins(j, 1);
                    if ((el1 == -1) || (el2 == -1))
                      contrib = 0;
                  }

                if (contrib)
                  {
                    double tmp = 0.;

                    // XXX : On a l'equation QDM et donc on ajoute grad_U transpose
                    if (is_VECT)
                      {
                        int orientation = 1;
                        if ((elem0 == face_voisins(j, kk)) || (face_voisins(num_face0, 1 - kk) == face_voisins(j, 1 - kk)))
                          orientation = -1;

                        tmp = orientation * nu_turb(elem0) / volumes(elem0);
                      }

                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        double d_nu = nu(elem0, is_VECT ? 0 : nc) + nu_turb(elem0);
                        double valA = z_class->viscA(num_face0, j, elem0, d_nu);

                        if (is_STAB && valA < 0.) valA = 0.;

                        if (is_EXPLICIT)
                          {
                            assert(resu != nullptr);
                            const double flux = valA * inconnue(j, nc) - valA * inconnue(num_face0, nc);
                            (*resu)(num_face0, nc) += flux;
                            if (j < nb_faces) // On traite les faces reelles
                              (*resu)(j, nc) -= flux;
                          }
                        else // METHODE IMPLICITE
                          {
                            assert(matrice != nullptr);
                            const int n0 = num_face0 * nb_comp + nc, j0 = j * nb_comp + nc;
                            double contrib_num_face = valA * porosite_eventuelle(num_face0);
                            double contrib_j = valA * porosite_eventuelle(j);

                            (*matrice)(n0, n0) += contrib_num_face;
                            (*matrice)(n0, j0) -= contrib_j;

                            if (j < nb_faces) // On traite les faces reelles
                              {
                                (*matrice)(j0, n0) -= contrib_num_face;
                                (*matrice)(j0, j0) += contrib_j;
                              }

                            // XXX : On a l'equation QDM et donc on ajoute grad_U transpose
                            if (is_VECT)
                              for (int nc2 = 0; nc2 < nb_comp; nc2++)
                                {
                                  const int n1 = num_face0 * nb_comp + nc2, j1 = j * nb_comp + nc2;
                                  double coeff_s = is_STAB ? 0. : tmp * face_normale(num_face0, nc2) * face_normale(j, nc);

                                  (*matrice)(n0, n1) += coeff_s * porosite_eventuelle(num_face0);
                                  (*matrice)(n0, j1) -= coeff_s * porosite_eventuelle(j);

                                  if (j < nb_faces) // On traite les faces reelles
                                    {
                                      double coeff_s2 = is_STAB ? 0. : tmp * face_normale(num_face0, nc) * face_normale(j, nc2);
                                      (*matrice)(j0, n1) -= coeff_s2 * porosite_eventuelle(num_face0);
                                      (*matrice)(j0, j1) += coeff_s2 * porosite_eventuelle(j);
                                    }

                                }
                          }
                      }
                  }
              }
          }
      }
}

#endif /* Op_Dift_VEF_Face_Gen_TPP_included */
