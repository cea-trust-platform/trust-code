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

#include <Op_Dift_standard_VEF_Face.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_homogene.h>
#include <Champ_P1NC.h>
#include <Periodique.h>
#include <Dirichlet.h>
#include <TRUSTTrav.h>
#include <Debog.h>

Implemente_instanciable(Op_Dift_standard_VEF_Face, "Op_Dift_VEF_P1NC_standard", Op_Dift_VEF_base);

Sortie& Op_Dift_standard_VEF_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Dift_standard_VEF_Face::readOn(Entree& is)
{
  Motcle accfermee = "}", motlu;
  is >> motlu;
  while (motlu != accfermee)
    {
      if (motlu == "defaut_bar")
        {
          is >> motlu;
          break;
        }
      else if (motlu == "grad_Ubar") is >> grad_Ubar;
      else if (motlu == "nu") is >> nu_lu;
      else if (motlu == "nut") is >> nut_lu;
      else if (motlu == "nu_transp") is >> nu_transp_lu;
      else if (motlu == "nut_transp") is >> nut_transp_lu;
      else if (motlu == "filtrer_resu") is >> filtrer_resu;
      else
        {
          Cerr << motlu << " n'est pas compris par " << que_suis_je() << finl;
          Process::exit();
        }
      is >> motlu;
    }
  return is;
}

void Op_Dift_standard_VEF_Face::ajouter_cas_vectoriel(const DoubleTab& vitesse, DoubleTab& resu, const DoubleTab& nu, const DoubleTab& nu_turb) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const int nbr_comp = resu.line_size();

  // on cast grad et grad_transp pour pouvoir les modifier : on utilise plus les static car pb avec plusieurs pbs
  DoubleTab& grad = ref_cast_non_const(DoubleTab, grad_);
  Debog::verifier("Op_Dift_standard_VEF_Face::ajouter_cas_vectoriel : resu 0 ", resu);

  //DoubleVect n(Objet_U::dimension), t(Objet_U::dimension);
  //DoubleTrav Tgrad(Objet_U::dimension, Objet_U::dimension);

  assert(nbr_comp > 1);

  // On dimensionne et initialise le tableau des bilans de flux:
  flux_bords_.resize(domaine_VEF.nb_faces_bord(), nbr_comp);
  flux_bords_ = 0.;

  // Construction du tableau grad_
  if (!grad.get_md_vector().non_nul())
    {
      grad.resize(0, Objet_U::dimension, Objet_U::dimension);
      domaine_VEF.domaine().creer_tableau_elements(grad);
    }

  // *** CALCUL DU GRADIENT ***
  DoubleTab ubar(vitesse);

  if (grad_Ubar) ref_cast(Champ_P1NC,inconnue_.valeur()).filtrer_L2(ubar);

  Champ_P1NC::calcul_gradient(ubar, grad, domaine_Cl_VEF);

  if (le_modele_turbulence.valeur().utiliser_loi_paroi())
    Champ_P1NC::calcul_duidxj_paroi(grad, nu, nu_turb, tau_tan_, domaine_Cl_VEF);

  grad.echange_espace_virtuel();

  // *** CALCUL DE LA DIFFUSION ***
  Debog::verifier("Op_Dift_standard_VEF_Face::ajouter_cas_vectoriel : grad 1 ", grad);
  calcul_divergence(resu, grad, grad, nu, nu_turb);

  Debog::verifier("Op_Dift_standard_VEF_Face::ajouter_cas_vectoriel : resu 1 ", resu);

  if (filtrer_resu) ref_cast(Champ_P1NC,inconnue_.valeur()).filtrer_resu(resu);
}

void Op_Dift_standard_VEF_Face::calcul_divergence(DoubleTab& dif, const DoubleTab& grad, const DoubleTab& gradt, const DoubleTab& nu, const DoubleTab& nu_turb) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleTab& face_normale = domaine_VEF.face_normales();
  const int nb_faces = domaine_VEF.nb_faces(), nbr_comp = dif.line_size();
  double nu1 = -123., nu2 = -123., nu1t = -123., nu2t = -123., flux = -123.;
  double c1 = 0., c2 = 0., c3 = 0., c4 = 0.;

  if (nu_lu) c1 = 1.;
  if (nut_lu) c2 = 1.;
  if (nu_transp_lu) c3 = 1.;
  if (nut_transp_lu) c4 = 1.;

  // Traitement des bords
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence dif 0 ", dif);

  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  int nb_cl = les_cl.size();
  for (int num_cl = 0; num_cl < nb_cl; num_cl++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
      const int nb_faces_b = le_bord.nb_faces();
      const int num1 = le_bord.num_premiere_face(), num2 = num1 + nb_faces_b;

      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          IntVect fait(nb_faces_b);
          fait = 0;
          for (int num_face0 = num1; num_face0 < num2; num_face0++)
            {
              int face_associee = la_cl_perio.face_associee(num_face0 - num1);
              if (fait(num_face0 - num1) == 0)
                {
                  fait(num_face0 - num1) = 1;
                  fait(face_associee) = 1;
                  const int elem10 = face_voisins(num_face0, 0), elem2 = face_voisins(num_face0, 1);
                  nu1 = c1 * nu[elem10] + c2 * nu_turb[elem10];
                  nu2 = c1 * nu[elem2] + c2 * nu_turb[elem2];
                  nu1t = c3 * nu[elem10] + c4 * nu_turb[elem10];
                  nu2t = c3 * nu[elem2] + c4 * nu_turb[elem2];

                  for (int i = 0; i < nbr_comp; i++)
                    for (int j = 0; j < nbr_comp; j++)
                      dif(num_face0, i) -= face_normale(num_face0, j) * (nu1 * grad(elem10, i, j) + nu1t * gradt(elem10, j, i) - (nu2 * grad(elem2, i, j) + nu2t * gradt(elem2, j, i)));

                  for (int i = 0; i < nbr_comp; i++)
                    dif(face_associee + num1, i) = dif(num_face0, i);
                }
            }
        }
      else if ((sub_type(Dirichlet, la_cl.valeur())) || (sub_type(Dirichlet_homogene, la_cl.valeur())))
        {
          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante, la_cl.valeur()))
            {
              for (int num_face0 = num1; num_face0 < num2; num_face0++)
                {
                  int elem1 = face_voisins(num_face0, 0);
                  nu1 = c1 * nu[elem1] + c2 * nu_turb[elem1];
                  nu1t = c3 * nu[elem1] + c4 * nu_turb[elem1];

                  for (int i = 0; i < nbr_comp; i++)
                    {
                      flux = 0.;
                      for (int j = 0; j < nbr_comp; j++)
                        flux += face_normale(num_face0, j) * (nu1 * grad(elem1, i, j) + nu1t * gradt(elem1, j, i));

                      dif(num_face0, i) = 0.; // PQ : valable en regime turbulent d'apres profil lineaire de la vitesse dans la sous couche visqueuse
                      flux_bords_(num_face0, i) -= flux;
                    }
                }
            }
        }
      else // Pour les autres conditions aux limites
        {
          for (int num_face = num1; num_face < num2; num_face++)
            {
              int elem1 = face_voisins(num_face, 0);
              nu1 = c1 * nu[elem1] + c2 * nu_turb[elem1];
              nu1t = c3 * nu[elem1] + c4 * nu_turb[elem1];

              for (int i = 0; i < nbr_comp; i++)
                {
                  flux = 0.;
                  for (int j = 0; j < nbr_comp; j++)
                    flux += face_normale(num_face, j) * (nu1 * grad(elem1, i, j) + nu1t * gradt(elem1, j, i));

                  dif(num_face, i) = 0.; // PQ : en attendant de faire mieux
                  flux_bords_(num_face, i) -= flux;
                }

            }
        }
    }

  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence dif 1 ", dif);
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence nu 1 ", nu);
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence nu_turb 1 ", nu_turb);
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence grad 1 ", grad);
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence gradt 1 ", gradt);

  for (int num_face0 = domaine_VEF.premiere_face_int(); num_face0 < nb_faces; num_face0++)
    {
      const int elem10 = face_voisins(num_face0, 0), elem2 = face_voisins(num_face0, 1);
      nu1 = c1 * nu[elem10] + c2 * nu_turb[elem10];
      nu2 = c1 * nu[elem2] + c2 * nu_turb[elem2];
      nu1t = c3 * nu[elem10] + c4 * nu_turb[elem10];
      nu2t = c3 * nu[elem2] + c4 * nu_turb[elem2];

      for (int i = 0; i < nbr_comp; i++)
        for (int j = 0; j < nbr_comp; j++)
          dif(num_face0, i) -= face_normale(num_face0, j) * (nu1 * grad(elem10, i, j) + nu1t * gradt(elem10, j, i) - (nu2 * grad(elem2, i, j) + nu2t * gradt(elem2, j, i)));
    }

  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence dif 2 ", dif);

  dif.echange_espace_virtuel();

  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence dif 3 ", dif);
}

DoubleTab& Op_Dift_standard_VEF_Face::ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  const DoubleTab& nu_turb = diffusivite_turbulente()->valeurs();
  DoubleTab nu(nu_turb);
  remplir_nu(nu);
  ajouter_cas_vectoriel(inconnue, resu, nu, nu_turb);
  modifier_flux(*this);
  return resu;
}
