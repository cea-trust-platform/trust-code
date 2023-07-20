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
#include <Op_Diff_VEF_base.h>
#include <Periodique.h>

template <typename DERIVED_T> template <Type_Champ _TYPE_>
void Op_Dift_VEF_Face_Gen<DERIVED_T>::ajouter_contribution_bord_gen(const DoubleTab& transporte, Matrice_Morse& matrice, const DoubleTab& nu,
                                                                    const DoubleTab& nu_turb, const DoubleVect& porosite_eventuelle) const
{
  constexpr bool is_VECT = (_TYPE_ == Type_Champ::VECTORIEL);

  // On traite les faces bord
  const Domaine_Cl_VEF& domaine_Cl_VEF = zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = dom_vef.valeur();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const DoubleVect& volumes = domaine_VEF.volumes();
  const DoubleTab& face_normale = domaine_VEF.face_normales();
  const int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), nb_faces = domaine_VEF.nb_faces();
  const int nb_bords = domaine_VEF.nb_front_Cl(), nb_comp = transporte.line_size();

  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
      int num1 = 0, num2 = le_bord.nb_faces_tot();
      int nb_faces_bord_reel = le_bord.nb_faces();

      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          // on ne parcourt que la moitie des faces volontairement...
          // GF il ne faut pas s'occuper des faces virtuelles
          num2 = nb_faces_bord_reel / 2; // XXX : attention si ecarts car version multicompo, num2 /= 2 ....

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
                                double valA = static_cast<const DERIVED_T*>(this)->viscA(num_face0, j, elem0, d_nu);
                                int n0 = num_face0 * nb_comp + nc;
                                int n0perio = fac_asso * nb_comp + nc;
                                int j0 = j * nb_comp + nc;

                                matrice(n0, n0) += valA * porosite_eventuelle(num_face0);
                                matrice(n0, j0) -= valA * porosite_eventuelle(j);

                                if (j < nb_faces) // On traite les faces reelles
                                  {
                                    if (ok == 1)
                                      matrice(j0, n0) -= valA * porosite_eventuelle(num_face0);
                                    else
                                      matrice(j0, n0perio) -= valA * porosite_eventuelle(num_face0);

                                    matrice(j0, j0) += valA * porosite_eventuelle(j);
                                  }

                                // XXX : On a l'equation QDM et donc on ajoute grad_U transpose
                                if (is_VECT)
                                  for (int nc2 = 0; nc2 < nb_comp; nc2++)
                                    {
                                      int n1 = num_face0 * nb_comp + nc2;
                                      int j1 = j * nb_comp + nc2;
                                      double coeff_s = orientation * nu_turb(elem0) / volumes(elem0) * face_normale(num_face0, nc2) * face_normale(j, nc);
                                      matrice(n0, n1) += coeff_s * porosite_eventuelle(num_face0);
                                      matrice(n0, j1) -= coeff_s * porosite_eventuelle(j);

                                      if (j < nb_faces) // On traite les faces reelles
                                        {
                                          double coeff_s2 = orientation * nu_turb(elem0) / volumes(elem0) * face_normale(num_face0, nc) * face_normale(j, nc2);

                                          if (ok == 1)
                                            matrice(j0, n1) -= coeff_s2 * porosite_eventuelle(num_face0);
                                          else
                                            matrice(j0, fac_asso * nb_comp + nc2) -= coeff_s2 * porosite_eventuelle(num_face0);

                                          matrice(j0, j1) += coeff_s2 * porosite_eventuelle(j);
                                        }
                                    }
                              }
                        }
                    }
                }

            }
        }
      else // pas perio
        {
          // correction dans le cas dirichlet sur paroi temperature
          if (sub_type(Scalaire_impose_paroi, la_cl.valeur())) // CL Temperature imposee
            {
              const Equation_base& my_eqn = domaine_Cl_VEF.equation();
              const RefObjU& modele_turbulence = my_eqn.get_modele(TURBULENCE);
              if (sub_type(Modele_turbulence_scal_base, modele_turbulence.valeur()))
                {
                  const Modele_turbulence_scal_base& mod_turb_scal = ref_cast(Modele_turbulence_scal_base, modele_turbulence.valeur());
                  const Turbulence_paroi_scal& loiparth = mod_turb_scal.loi_paroi();
                  if (loiparth->use_equivalent_distance())
                    {
                      const DoubleVect& d_equiv = loiparth->equivalent_distance(n_bord);
                      int nb_dim_pb = Objet_U::dimension;

                      DoubleVect le_mauvais_gradient(nb_dim_pb);
                      for (int ind_face = num1; ind_face < num2; ind_face++)
                        for (int nc = 0; nc < nb_comp; nc++)
                          {
                            int num_face = le_bord.num_face(ind_face);
                            // Tf est la temperature fluide moyenne dans le premier element sans tenir compte de la temperature de paroi.
                            double Tf = 0.;
                            double bon_gradient = 0.; // c'est la norme du gradient de temperature normal a la paroi
                            // calculee a l'aide de la loi de paroi.

                            int elem1 = face_voisins(num_face, 0);
                            if (elem1 == -1) elem1 = face_voisins(num_face, 1);

                            // inconnue(num_face) est la temperature de paroi : Tw.
                            // On se fiche du signe de bon gradient car c'est la norme du gradient de temperature dans l'element.
                            // Ensuite ce sera multiplie par le vecteur normal a la face de paroi qui lui a les bons signes.
                            bon_gradient = 1. / d_equiv(ind_face) * (-domaine_VEF.oriente_normale(num_face, elem1));

                            double surface_face = domaine_VEF.face_surfaces(num_face), nutotal = nu(elem1, nc) + nu_turb(elem1);

                            for (int i0 = 0; i0 < nb_faces_elem; i0++)
                              {
                                int j = elem_faces(elem1, i0);

                                for (int ii = 0; ii < nb_faces_elem; ii++)
                                  {
                                    le_mauvais_gradient = 0;
                                    int jj = elem_faces(elem1, ii);
                                    double surface_pond = 0;
                                    for (int kk = 0; kk < nb_dim_pb; kk++)
                                      surface_pond -= (face_normale(jj, kk) * domaine_VEF.oriente_normale(jj, elem1) * face_normale(num_face, kk) * domaine_VEF.oriente_normale(num_face, elem1))
                                                      / (surface_face * surface_face);

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
                                    if (jj == num_face) resu2 *= -1;
                                    else resu2 *= Tf;

                                    int j0 = j * nb_comp + nc, jj0 = jj * nb_comp + nc;

                                    matrice(j0, jj0) += (resu1 - resu2) * porosite_eventuelle(jj0);
                                  }
                              }
                          }
                    }
                }
            }
          else if (sub_type(Echange_externe_impose, la_cl.valeur()) && nb_comp < 2) // XXX : plus tard pour multi inco aussi ...
            {
              const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
              const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
              for (int face = ndeb; face < nfin; face++)
                matrice(face, face) += la_cl_paroi.h_imp(face - ndeb) * domaine_VEF.face_surfaces(face);
            }

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
                          double d_nu = nu(elem, is_VECT ? 0 : nc) + nu_turb(elem);
                          double valA = static_cast<const DERIVED_T*>(this)->viscA(num_face, j, elem, d_nu);
                          const int n0 = num_face * nb_comp + nc, j0 = j * nb_comp + nc;

                          if (ind_face < nb_faces_bord_reel)
                            {
                              matrice(n0, n0) += valA * porosite_eventuelle(num_face);
                              matrice(n0, j0) -= valA * porosite_eventuelle(j);
                            }

                          if (j < nb_faces)
                            {
                              matrice(j0, n0) -= valA * porosite_eventuelle(num_face);
                              matrice(j0, j0) += valA * porosite_eventuelle(j);
                            }

                          // XXX : On a l'equation QDM et donc on ajoute grad_U transpose
                          if (is_VECT)
                            for (int nc2 = 0; nc2 < nb_comp; nc2++)
                              {
                                const int n1 = num_face * nb_comp + nc2, j1 = j * nb_comp + nc2;
                                if (ind_face < nb_faces_bord_reel)
                                  {
                                    double coeff_s = orientation * nu_turb(elem) / volumes(elem) * face_normale(num_face, nc2) * face_normale(j, nc);
                                    matrice(n0, n1) += coeff_s * porosite_eventuelle(num_face);
                                    matrice(n0, j1) -= coeff_s * porosite_eventuelle(j);
                                  }

                                if (j < nb_faces)
                                  {
                                    double coeff_s = orientation * nu_turb(elem) / volumes(elem) * face_normale(num_face, nc) * face_normale(j, nc2);
                                    matrice(j0, n1) -= coeff_s * porosite_eventuelle(num_face);
                                    matrice(j0, j1) += coeff_s * porosite_eventuelle(j);
                                  }
                              }
                        }
                    }
                }
            }
        }
    }
}

template <typename DERIVED_T> template <Type_Champ _TYPE_>
void Op_Dift_VEF_Face_Gen<DERIVED_T>::ajouter_contribution_interne_gen(const DoubleTab& transporte, Matrice_Morse& matrice, const DoubleTab& nu,
                                                                       const DoubleTab& nu_turb, const DoubleVect& porosite_eventuelle) const
{
  constexpr bool is_VECT = (_TYPE_ == Type_Champ::VECTORIEL);

  // On traite les faces internes
  const Domaine_VEF& domaine_VEF = dom_vef.valeur();
  const int nb_faces = domaine_VEF.nb_faces(), nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  const int nb_comp = transporte.line_size();
  const IntTab& face_voisins = domaine_VEF.face_voisins(), &elem_faces = domaine_VEF.elem_faces();
  const DoubleVect& volumes = domaine_VEF.volumes();
  const DoubleTab& face_normale = domaine_VEF.face_normales();

  int rumpremiereface = domaine_VEF.premiere_face_int();
  for (int num_face0 = rumpremiereface; num_face0 < nb_faces; num_face0++)
    for (int l = 0; l < 2; l++)
      {
        int elem0 = face_voisins(num_face0, l);
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
                        if ((elem0 == face_voisins(j, l)) || (face_voisins(num_face0, 1 - l) == face_voisins(j, 1 - l)))
                          orientation = -1;

                        tmp = orientation * nu_turb(elem0) / volumes(elem0);
                      }

                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        double d_nu = nu(elem0, is_VECT ? 0 : nc) + nu_turb(elem0);
                        double valA = static_cast<const DERIVED_T*>(this)->viscA(num_face0, j, elem0, d_nu);
                        const int n0 = num_face0 * nb_comp + nc, j0 = j * nb_comp + nc;

                        double contrib_num_face = valA * porosite_eventuelle(num_face0);
                        double contrib_j = valA * porosite_eventuelle(j);

                        matrice(n0, n0) += contrib_num_face;
                        matrice(n0, j0) -= contrib_j;
                        if (j < nb_faces) // On traite les faces reelles
                          {
                            matrice(j0, n0) -= contrib_num_face;
                            matrice(j0, j0) += contrib_j;
                          }

                        // XXX : On a l'equation QDM et donc on ajoute grad_U transpose
                        if (is_VECT)
                          for (int nc2 = 0; nc2 < nb_comp; nc2++)
                            {
                              const int n1 = num_face0 * nb_comp + nc2, j1 = j * nb_comp + nc2;
                              double coeff_s = tmp * face_normale(num_face0, nc2) * face_normale(j, nc);

                              matrice(n0, n1) += coeff_s * porosite_eventuelle(num_face0);
                              matrice(n0, j1) -= coeff_s * porosite_eventuelle(j);

                              if (j < nb_faces) // On traite les faces reelles
                                {
                                  double coeff_s2 = tmp * face_normale(num_face0, nc) * face_normale(j, nc2);
                                  matrice(j0, n1) -= coeff_s2 * porosite_eventuelle(num_face0);
                                  matrice(j0, j1) += coeff_s2 * porosite_eventuelle(j);
                                }
                            }
                      }
                  }
              }
          }
      }
}

#endif /* Op_Dift_VEF_Face_Gen_TPP_included */
