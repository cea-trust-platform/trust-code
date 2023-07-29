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

#include <Check_espace_virtuel.h>
#include <Discretisation_base.h>
#include <Op_Dift_VEF_Face.h>
#include <Milieu_base.h>
#include <Debog.h>

Implemente_instanciable_sans_constructeur(Op_Dift_VEF_Face, "Op_Dift_VEF_P1NC|Op_Dift_VEF_var_P1NC", Op_Dift_VEF_base);

Op_Dift_VEF_Face::Op_Dift_VEF_Face()
{
  declare_support_masse_volumique(1); // pour FT ... BOF
}

Sortie& Op_Dift_VEF_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Dift_VEF_Face::readOn(Entree& s) { return s; }

DoubleTab& Op_Dift_VEF_Face::ajouter(const DoubleTab& inconnue_org, DoubleTab& resu) const
{
  remplir_nu(nu_); // On remplit le tableau nu car ajouter peut se faire avant le premier pas de temps

  const DoubleTab& nu_turb = diffusivite_turbulente()->valeurs();
  DoubleTab nu, nu_turb_m, tab_inconnue;
  const int nb_comp = resu.line_size();

  // On dimensionne et initialise le tableau des bilans de flux:
  flux_bords_.resize(le_dom_vef->nb_faces_bord(), nb_comp);
  flux_bords_ = 0.;

  int marq = phi_psi_diffuse(equation());
  const DoubleVect& porosite_face = equation().milieu().porosite_face(), &porosite_elem = equation().milieu().porosite_elem();

  // soit on a div(phi nu grad inco) OU on a div(nu grad phi inco) : cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_, nu, !marq, porosite_elem);
  modif_par_porosite_si_flag(nu_turb, nu_turb_m, !marq, porosite_elem);
  const DoubleTab& inconnue = modif_par_porosite_si_flag(inconnue_org, tab_inconnue, marq, porosite_face);

  assert_espace_virtuel_vect(nu);
  assert_espace_virtuel_vect(inconnue);
  assert_espace_virtuel_vect(nu_turb_m);

  Debog::verifier("Op_Dift_VEF_Face::ajouter nu", nu);
  Debog::verifier("Op_Dift_VEF_Face::ajouter nu_turb", nu_turb_m);
  Debog::verifier("Op_Dift_VEF_Face::ajouter inconnue_org", inconnue_org);
  Debog::verifier("Op_Dift_VEF_Face::ajouter inconnue", inconnue);

  if (equation().inconnue()->nature_du_champ() == vectoriel)
    {
      fill_grad_Re<Type_Champ::VECTORIEL>(inconnue, resu, nu, nu_turb);
      ajouter_bord_gen<Type_Champ::VECTORIEL>(inconnue, resu, flux_bords_, nu, nu_turb);
      ajouter_interne_gen<Type_Champ::VECTORIEL>(inconnue, resu, flux_bords_, nu, nu_turb);
    }
  else
    {
      ajouter_bord_gen<Type_Champ::SCALAIRE>(inconnue, resu, flux_bords_, nu, nu_turb);
      ajouter_interne_gen<Type_Champ::SCALAIRE>(inconnue, resu, flux_bords_, nu, nu_turb);
    }

  modifier_flux(*this);

  return resu;
}

void Op_Dift_VEF_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  modifier_matrice_pour_periodique_avant_contribuer(matrice, equation());
  remplir_nu(nu_); // On remplit le tableau nu car l'assemblage d'une matrice avec ajouter_contribution peut se faire avant le premier pas de temps

  const DoubleTab& nu_turb_ = diffusivite_turbulente()->valeurs();
  DoubleTab nu, nu_turb;

  int marq = phi_psi_diffuse(equation());
  const DoubleVect& porosite_elem = equation().milieu().porosite_elem();

  // soit on a div(phi nu grad inco) OU on a div(nu grad phi inco) : cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_, nu, !marq, porosite_elem);
  modif_par_porosite_si_flag(nu_turb_, nu_turb, !marq, porosite_elem);

  DoubleVect porosite_eventuelle(equation().milieu().porosite_face());
  if (!marq) porosite_eventuelle = 1;

  if (equation().inconnue()->nature_du_champ() == vectoriel)
    {
      ajouter_contribution_bord_gen<Type_Champ::VECTORIEL>(inco, matrice, nu, nu_turb, porosite_eventuelle);
      ajouter_contribution_interne_gen<Type_Champ::VECTORIEL>(inco, matrice, nu, nu_turb, porosite_eventuelle);
    }
  else
    {
      ajouter_contribution_bord_gen<Type_Champ::SCALAIRE>(inco, matrice, nu, nu_turb, porosite_eventuelle);
      ajouter_contribution_interne_gen<Type_Champ::SCALAIRE>(inco, matrice, nu, nu_turb, porosite_eventuelle);
    }

  modifier_matrice_pour_periodique_apres_contribuer(matrice, equation());
}

// bientot a la poubelle// bientot a la poubelle
void Op_Dift_VEF_Face::contribuer_au_second_membre(DoubleTab& resu) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleTab& face_normale = domaine_VEF.face_normales();
  const int nb_faces = domaine_VEF.nb_faces(), nb_comp = resu.line_size();

  // On traite les faces bord
  if (equation().inconnue()->nature_du_champ() == vectoriel)
    {
      const DoubleTab& nu_turb = diffusivite_turbulente()->valeurs(), &inconnue_org = equation().inconnue().valeurs();
      DoubleTab nu, nu_turb_m, tab_inconnue;

      int marq = phi_psi_diffuse(equation());

      const DoubleVect& porosite_face = equation().milieu().porosite_face(), &porosite_elem = equation().milieu().porosite_elem();

      // soit on a div(phi nu grad inco) OU on a div(nu grad phi inco) : cela depend si on diffuse phi_psi ou psi
      modif_par_porosite_si_flag(nu_, nu, !marq, porosite_elem);
      modif_par_porosite_si_flag(nu_turb, nu_turb_m, !marq, porosite_elem);

      const DoubleTab& inconnue = modif_par_porosite_si_flag(inconnue_org, tab_inconnue, marq, porosite_face);

      DoubleTab& grad = grad_;
      grad = 0.;

      Champ_P1NC::calcul_gradient(inconnue, grad, domaine_Cl_VEF);
      DoubleTab gradsa(grad);

      if (le_modele_turbulence.valeur().utiliser_loi_paroi())
        Champ_P1NC::calcul_duidxj_paroi(grad, nu, nu_turb, tau_tan_, domaine_Cl_VEF);

      grad -= gradsa;
      grad.echange_espace_virtuel();

      for (int num_face = 0; num_face < nb_faces; num_face++)
        for (int kk = 0; kk < 2; kk++)
          {
            int elem = face_voisins(num_face, kk);
            if (elem != -1)
              {
                int ori = 1 - 2 * kk;
                for (int i = 0; i < nb_comp; i++)
                  for (int j = 0; j < nb_comp; j++)
                    resu(num_face, i) -= ori * face_normale(num_face, j) * ((nu[elem] + nu_turb[elem]) * grad(elem, i, j) + (nu_turb[elem]) * grad(elem, j, i) /* grad transpose */);
              }
          }
    }

  for (int n_bord = 0; n_bord < domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_paroi, la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
          const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
          for (int face = ndeb; face < nfin; face++)
            for (int comp = 0; comp < nb_comp; comp++)
              resu(face, comp) += la_cl_paroi.flux_impose(face - ndeb, comp) * domaine_VEF.face_surfaces(face);
        }
      else if (sub_type(Echange_externe_impose, la_cl.valeur()))
        {
          if (resu.line_size() == 1)
            {
              const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
              const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
              for (int face = ndeb; face < nfin; face++)
                resu[face] += la_cl_paroi.h_imp(face - ndeb) * (la_cl_paroi.T_ext(face - ndeb)) * domaine_VEF.face_surfaces(face);
            }
          else
            Process::exit("Op_Dift_VEF_Face::contribuer_au_second_membre : Non code pour Echange_externe_impose !");
        }
    }
}
