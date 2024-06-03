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

#include <Dirichlet_paroi_defilante.h>
#include <Echange_externe_impose.h>
#include <Dirichlet_paroi_fixe.h>
#include <Op_Dift_VEF_Face_Q1.h>
#include <Champ_Uniforme.h>
#include <Domaine_Cl_VEF.h>
#include <Neumann_paroi.h>
#include <Milieu_base.h>
#include <Champ_Q1NC.h>
#include <Periodique.h>
#include <TRUSTTrav.h>
#include <Champ_Don.h>
#include <Debog.h>

Implemente_instanciable(Op_Dift_VEF_Face_Q1, "Op_Dift_VEF_Q1NC", Op_Dift_VEF_base);

Sortie& Op_Dift_VEF_Face_Q1::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Dift_VEF_Face_Q1::readOn(Entree& s) { return s; }

static double viscA_Q1(const Domaine_VEF& le_dom, int num_face, int num2, int dimension, int num_elem, double diffu)
{
  double DSiSj = le_dom.face_normales(num_face, 0) * le_dom.face_normales(num2, 1) - le_dom.face_normales(num_face, 1) * le_dom.face_normales(num2, 0);

  if (dimension == 3)
    DSiSj += ((le_dom.face_normales(num_face, 1) * le_dom.face_normales(num2, 2) - le_dom.face_normales(num_face, 2) * le_dom.face_normales(num2, 1))
              + (le_dom.face_normales(num_face, 2) * le_dom.face_normales(num2, 0) - le_dom.face_normales(num_face, 0) * le_dom.face_normales(num2, 2)));

  DSiSj = diffu / le_dom.volumes(num_elem) * std::fabs(DSiSj);
  return DSiSj;
}

DoubleTab& Op_Dift_VEF_Face_Q1::ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const DoubleVect& porosite_face = equation().milieu().porosite_face();
  const IntTab& elem_faces = domaine_VEF.elem_faces(), &face_voisins = domaine_VEF.face_voisins();
  const IntVect& rang_elem_non_std = domaine_VEF.rang_elem_non_std();
  const int n1 = domaine_VEF.nb_faces(), nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), nb_comp = resu.line_size();
  const double mu = diffusivite(0);
  const DoubleTab& mu_turb = diffusivite_turbulente()->valeurs(), &face_normale = domaine_VEF.face_normales();
  DoubleVect n(dimension);
  DoubleTrav Tgrad(dimension, dimension);

  // On traite les faces bord
  if (nb_comp == 1)
    for (int n_bord = 0; n_bord < domaine_VEF.nb_front_Cl(); n_bord++)
      {
        Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres nb_compo= 1 nbords, resu", resu);

        const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
        int nb_faces_bord = le_bord.nb_faces();
        int num1 = 0;
        int num2 = nb_faces_bord;

        if (sub_type(Periodique, la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
            int fac_asso;
            for (int ind_face = num1; ind_face < num2; ind_face++)
              {
                const int num_face = le_bord.num_face(ind_face), elem1 = face_voisins(num_face, 0);
                fac_asso = le_bord.num_face(la_cl_perio.face_associee(ind_face));
                double d_mu = mu + mu_turb(elem1);
                for (int i = 0; i < nb_faces_elem; i++)
                  {
                    const int j = elem_faces(elem1, i);
                    if ((j > num_face) && (j != fac_asso))
                      {
                        const double valA = viscA_Q1(domaine_VEF, num_face, j, dimension, elem1, d_mu);
                        resu(num_face, 0) += valA * inconnue(j, 0) * porosite_face(num_face);
                        resu(num_face, 0) -= valA * inconnue(num_face, 0) * porosite_face(num_face);
                        resu(j, 0) += 0.5 * valA * inconnue(num_face, 0) * porosite_face(j);
                        resu(j, 0) -= 0.5 * valA * inconnue(j, 0) * porosite_face(j);
                      }
                  }
                const int elem2 = face_voisins(num_face, 1);
                d_mu = mu + mu_turb(elem2);
                for (int i = 0; i < nb_faces_elem; i++)
                  {
                    const int j = elem_faces(elem2, i);
                    if ((j > num_face) && (j != fac_asso))
                      {
                        const double valA = viscA_Q1(domaine_VEF, num_face, j, dimension, elem2, d_mu);
                        resu(num_face, 0) += valA * inconnue(j, 0) * porosite_face(num_face);
                        resu(num_face, 0) -= valA * inconnue(num_face, 0) * porosite_face(num_face);
                        resu(j, 0) += 0.5 * valA * inconnue(num_face, 0) * porosite_face(j);
                        resu(j, 0) -= 0.5 * valA * inconnue(j, 0) * porosite_face(j);
                      }
                  }
              }
          }
        else
          for (int ind_face = num1; ind_face < num2; ind_face++)
            {
              const int num_face = le_bord.num_face(ind_face), elem1 = face_voisins(num_face, 0);
              const double d_mu = mu + mu_turb(elem1);
              for (int i = 0; i < nb_faces_elem; i++)
                {
                  const int j = elem_faces(elem1, i);
                  if (j > num_face)
                    {
                      const double valA = viscA_Q1(domaine_VEF, num_face, j, dimension, elem1, d_mu);
                      resu(num_face, 0) += valA * inconnue(j, 0) * porosite_face(num_face);
                      resu(num_face, 0) -= valA * inconnue(num_face, 0) * porosite_face(num_face);
                      resu(j, 0) += valA * inconnue(num_face, 0) * porosite_face(j);
                      resu(j, 0) -= valA * inconnue(j, 0) * porosite_face(j);
                    }
                }
            }
      }
  else // nb_comp > 1
    for (int n_bord = 0; n_bord < domaine_VEF.nb_front_Cl(); n_bord++)
      {
        Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres nb_compo> 1 nbords, resu", resu);

        const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
        //const IntTab& elem_faces = domaine_VEF.elem_faces();
        int nb_faces_bord_tot = le_bord.nb_faces_tot();
        int nb_faces_bord = le_bord.nb_faces();
        int num1 = 0;
        int num2 = nb_faces_bord;

        if (sub_type(Periodique, la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
            int fac_asso;
            for (int ind_face = num1; ind_face < num2; ind_face++)
              {
                const int num_face = le_bord.num_face(ind_face), elem1 = face_voisins(num_face, 0);
                fac_asso = le_bord.num_face(la_cl_perio.face_associee(ind_face));
                double d_mu = mu + mu_turb(elem1);
                for (int i = 0; i < nb_faces_elem; i++)
                  {
                    const int j = elem_faces(elem1, i);
                    if ((j > num_face) && (j != fac_asso))
                      {
                        const double valA = viscA_Q1(domaine_VEF, num_face, j, dimension, elem1, d_mu);
                        for (int nc = 0; nc < nb_comp; nc++)
                          {
                            resu(num_face, nc) += valA * inconnue(j, nc) * porosite_face(num_face);
                            resu(num_face, nc) -= valA * inconnue(num_face, nc) * porosite_face(num_face);
                            resu(j, nc) += 0.5 * valA * inconnue(num_face, nc) * porosite_face(j);
                            resu(j, nc) -= 0.5 * valA * inconnue(j, nc) * porosite_face(j);
                          }
                      }
                  }
                const int elem2 = face_voisins(num_face, 1);
                d_mu = mu + mu_turb(elem2);
                for (int i = 0; i < nb_faces_elem; i++)
                  {
                    const int j = elem_faces(elem2, i);
                    if ((j > num_face) && (j != fac_asso))
                      {
                        const double valA = viscA_Q1(domaine_VEF, num_face, j, dimension, elem2, d_mu);
                        for (int nc = 0; nc < nb_comp; nc++)
                          {
                            resu(num_face, nc) += valA * inconnue(j, nc) * porosite_face(num_face);
                            resu(num_face, nc) -= valA * inconnue(num_face, nc) * porosite_face(num_face);
                            resu(j, nc) += 0.5 * valA * inconnue(num_face, nc) * porosite_face(j);
                            resu(j, nc) -= 0.5 * valA * inconnue(j, nc) * porosite_face(j);
                          }
                      }
                  }
              }
            Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres periodique , resu", resu);

          }
        else if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante, la_cl.valeur()))
          {
            if (le_modele_turbulence.valeur().utiliser_loi_paroi())
              {
                if (dimension == 2)
                  {
                    for (int ind_face = num1; ind_face < num2; ind_face++)
                      {
                        const int num_face = le_bord.num_face(ind_face);
                        int elem = face_voisins(num_face, 0);
                        // Calcul du gradient gradU=tau*Ny*N
                        // Au bord, n toujours oriente vers l'exterieur
                        n[0] = face_normale(num_face, 0);
                        n[1] = face_normale(num_face, 1);
                        n /= norme_array(n);
                        double signe = 0;
                        Tgrad(0, 0) = tau_tan_(num_face, 0) * n[1] * n[0];
                        Tgrad(0, 1) = tau_tan_(num_face, 0) * n[1] * n[1];
                        Tgrad(1, 0) = -tau_tan_(num_face, 0) * n[0] * n[0];
                        Tgrad(1, 1) = -tau_tan_(num_face, 0) * n[0] * n[1];
                        // On determine le sens du frottement parietal :
                        signe = 0;
                        for (int i = 0; i < nb_faces_elem; i++)
                          {
                            const int j = elem_faces(elem, i);
                            // Calcul du signe de (Utan-Uparoi)*tan :
                            signe += (inconnue(j, 1) - inconnue(num_face, 1)) * face_normale(num_face, 0) - (inconnue(j, 0) - inconnue(num_face, 0)) * face_normale(num_face, 1);
                          }
                        if (signe < 0)
                          signe = -1.;
                        else
                          signe = 1.;
                        // On calcule la contribution sur chaque face de l'element :
                        for (int i = 0; i < nb_faces_elem; i++)
                          {
                            const int j = elem_faces(elem, i);
                            double ori = 1.;
                            if (domaine_VEF.face_voisins(j, 0) != elem)
                              ori = -1;
                            if (j != num_face)
                              for (int nc = 0; nc < nb_comp; nc++)
                                resu(j, nc) -= ori * signe * (Tgrad(nc, 0) * face_normale(j, 0) + Tgrad(nc, 1) * face_normale(j, 1)) * porosite_face(j);
                          }
                      }
                    Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres dirichlet dim2, resu", resu);
                  }
                else if (dimension == 3)
                  {
                    for (int ind_face = num1; ind_face < num2; ind_face++)
                      {
                        const int num_face = le_bord.num_face(ind_face);
                        int elem = face_voisins(num_face, 0);
                        // Calcul du gradient gradU
                        // Au bord, n toujours oriente vers l'exterieur
                        n[0] = face_normale(num_face, 0);
                        n[1] = face_normale(num_face, 1);
                        n[2] = face_normale(num_face, 2);
                        n /= norme_array(n);
                        // Difference avec le 2D car ici tau_tan est signe !
                        // A tester malgre tout !
                        Tgrad(0, 0) = tau_tan_(num_face, 0) * n[0];
                        Tgrad(0, 1) = tau_tan_(num_face, 0) * n[1];
                        Tgrad(0, 2) = tau_tan_(num_face, 0) * n[2];
                        Tgrad(1, 0) = tau_tan_(num_face, 1) * n[0];
                        Tgrad(1, 1) = tau_tan_(num_face, 1) * n[1];
                        Tgrad(1, 2) = tau_tan_(num_face, 1) * n[2];
                        Tgrad(2, 0) = tau_tan_(num_face, 2) * n[0];
                        Tgrad(2, 1) = tau_tan_(num_face, 2) * n[1];
                        Tgrad(2, 2) = tau_tan_(num_face, 2) * n[2];
                        // On calcule la contribution sur chaque face de l'element :
                        for (int i = 0; i < nb_faces_elem; i++)
                          {
                            const int j = elem_faces(elem, i);
                            double ori = 1.;
                            if (domaine_VEF.face_voisins(j, 0) != elem)
                              ori = -1;
                            if (j != num_face)
                              for (int nc = 0; nc < nb_comp; nc++)
                                resu(j, nc) -= ori * (Tgrad(nc, 0) * face_normale(j, 0) + Tgrad(nc, 1) * face_normale(j, 1) + Tgrad(nc, 2) * face_normale(j, 2)) * porosite_face(j);
                          }
                      }
                    Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres dirichlet dim3, resu", resu);
                  }
              }
            Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres dirichlet (je crois), resu", resu);
          }
        else
          {
            Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter avant else des bords, resu", resu);
            num1 = 0;
            num2 = nb_faces_bord_tot;
            for (int ind_face = num1; ind_face < num2; ind_face++)
              {
                const int num_face = le_bord.num_face(ind_face);
                int elem = face_voisins(num_face, 0);
                const double d_mu = mu + mu_turb(elem);
                // Boucle sur les faces :
                for (int ii = 0; ii < nb_faces_elem; ii++)
                  {
                    const int j = elem_faces(elem, ii);
                    if (((j > num_face) && (ind_face < nb_faces_bord)) || ((j != num_face) && (ind_face >= nb_faces_bord)))
                      {
                        const double valA = viscA_Q1(domaine_VEF, num_face, j, dimension, elem, d_mu);
                        for (int nc = 0; nc < nb_comp; nc++)
                          {
                            resu(num_face, nc) += valA * inconnue(j, nc) * porosite_face(num_face);
                            resu(num_face, nc) -= valA * inconnue(num_face, nc) * porosite_face(num_face);
                            resu(j, nc) += valA * inconnue(num_face, nc) * porosite_face(j);
                            resu(j, nc) -= valA * inconnue(j, nc) * porosite_face(j);
                          }
                      }
                  }
              }
            Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres else des bords, resu", resu);
          }
      }

  Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres bords, resu", resu);

  // On traite les faces internes

  for (int num_face = domaine_VEF.premiere_face_int(); num_face < n1; num_face++)
    for (int kk = 0; kk < 2; kk++)
      {
        const int elem0 = face_voisins(num_face, kk);
        const double d_mu = mu + mu_turb(elem0);
        // On elimine les elements avec CL de paroi (rang>=1)
        int rang = rang_elem_non_std(elem0);
        if (rang < 1)
          {
            for (int i = 0; i < nb_faces_elem; i++)
              {
                const int j = elem_faces(elem0, i);
                if (j > num_face)
                  {
                    const double valA = viscA_Q1(domaine_VEF, num_face, j, dimension, elem0, d_mu);
                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        resu(num_face, nc) += valA * inconnue(j, nc) * porosite_face(num_face);
                        resu(num_face, nc) -= valA * inconnue(num_face, nc) * porosite_face(num_face);
                        resu(j, nc) += valA * inconnue(num_face, nc) * porosite_face(j);
                        resu(j, nc) -= valA * inconnue(j, nc) * porosite_face(j);
                      }
                  }
              }
          }
      }

  Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres interne, resu", resu);

  for (int n_bord = 0; n_bord < domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_paroi, la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face = ndeb; face < nfin; face++)
            for (int comp = 0; comp < nb_comp; comp++)
              resu(face, comp) += la_cl_paroi.flux_impose(face - ndeb, comp) * domaine_VEF.surface(face) * porosite_face(face);
        }
      else if (sub_type(Echange_externe_impose, la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face = ndeb; face < nfin; face++)
            for (int comp = 0; comp < nb_comp; comp++)
              resu(face, comp) += la_cl_paroi.h_imp(face - ndeb, comp) * (la_cl_paroi.T_ext(face - ndeb) - inconnue(face, comp)) * domaine_VEF.surface(face) * porosite_face(face);
        }
    }

  Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres fin, resu", resu);
  modifier_flux(*this);
  return resu;
}

void Op_Dift_VEF_Face_Q1::contribuer_a_avec(const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = domaine_cl_vef();
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const DoubleVect& porosite_face = equation().milieu().porosite_face();
  const IntTab& elem_faces = domaine_VEF.elem_faces(), &face_voisins = domaine_VEF.face_voisins();
  const IntVect& rang_elem_non_std = domaine_VEF.rang_elem_non_std();
  const int nb_comp = transporte.line_size(), nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), n1 = domaine_VEF.nb_faces();
  const DoubleTab& mu_turb = diffusivite_turbulente()->valeurs();
  const double mu = diffusivite(0);

  //DoubleVect n(dimension);
  //DoubleTrav Tgrad(dimension, dimension);
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();

  // On traite les faces bord
  for (int n_bord = 0; n_bord < domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
      const int num1 = le_bord.num_premiere_face(), num2 = num1 + le_bord.nb_faces();

      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          for (int num_face0 = num1; num_face0 < num2; num_face0++)
            {
              const int fac_asso = la_cl_perio.face_associee(num_face0 - num1) + num1, elem1 = face_voisins(num_face0, 0);
              double d_mu = mu + mu_turb(elem1);
              for (int i = 0; i < nb_faces_elem; i++)
                {
                  const int j = elem_faces(elem1, i);
                  if ((j > num_face0) && (j != fac_asso))
                    {
                      const double valA = viscA_Q1(domaine_VEF, num_face0, j, dimension, elem1, d_mu);
                      for (int nc = 0; nc < nb_comp; nc++)
                        {
                          for (int kk = tab1[num_face0 * nb_comp + nc] - 1; kk < tab1[num_face0 * nb_comp + nc + 1] - 1; kk++)
                            {
                              if (tab2[kk] - 1 == num_face0 * nb_comp + nc)
                                coeff(kk) += valA * porosite_face(num_face0 * nb_comp + nc);
                              if (tab2[kk] - 1 == j * nb_comp + nc)
                                coeff(kk) -= 0.5 * valA * porosite_face(j * nb_comp + nc);
                            }
                          for (int kk = tab1[j * nb_comp + nc] - 1; kk < tab1[j * nb_comp + nc + 1] - 1; kk++)
                            {
                              if (tab2[kk] - 1 == num_face0 * nb_comp + nc)
                                coeff(kk) -= valA * porosite_face(num_face0 * nb_comp + nc);
                              if (tab2[kk] - 1 == j * nb_comp + nc)
                                coeff(kk) += 0.5 * valA * porosite_face(j * nb_comp + nc);
                            }
                        }
                    }
                }
              const int elem2 = face_voisins(num_face0, 1);
              d_mu = mu + mu_turb(elem2);
              for (int i = 0; i < nb_faces_elem; i++)
                {
                  const int j = elem_faces(elem2, i);
                  if ((j > num_face0) && (j != fac_asso))
                    {
                      const double valA = viscA_Q1(domaine_VEF, num_face0, j, dimension, elem2, d_mu);
                      for (int nc = 0; nc < nb_comp; nc++)
                        {

                          for (int kk = tab1[num_face0 * nb_comp + nc] - 1; kk < tab1[num_face0 * nb_comp + nc + 1] - 1; kk++)
                            {
                              if (tab2[kk] - 1 == num_face0 * nb_comp + nc)
                                coeff(kk) += valA * porosite_face(num_face0 * nb_comp + nc);
                              if (tab2[kk] - 1 == j * nb_comp + nc)
                                coeff(kk) -= 0.5 * valA * porosite_face(j * nb_comp + nc);
                            }
                          for (int kk = tab1[j * nb_comp + nc] - 1; kk < tab1[j * nb_comp + nc + 1] - 1; kk++)
                            {
                              if (tab2[kk] - 1 == num_face0 * nb_comp + nc)
                                coeff(kk) -= valA * porosite_face(num_face0 * nb_comp + nc);
                              if (tab2[kk] - 1 == j * nb_comp + nc)
                                coeff(kk) += 0.5 * valA * porosite_face(j * nb_comp + nc);
                            }
                        }
                    }
                }
            }
        }
      else
        for (int num_face = num1; num_face < num2; num_face++)
          {
            int elembis = face_voisins(num_face, 0);
            const double d_mu = mu + mu_turb(elembis);
            // Boucle sur les faces :
            for (int ii = 0; ii < nb_faces_elem; ii++)
              {
                const int j = elem_faces(elembis, ii);
                if (j > num_face)
                  {
                    const double valA = viscA_Q1(domaine_VEF, num_face, j, dimension, elembis, d_mu);
                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        for (int kk = tab1[num_face * nb_comp + nc] - 1; kk < tab1[num_face * nb_comp + nc + 1] - 1; kk++)
                          {
                            if (tab2[kk] - 1 == num_face * nb_comp + nc)
                              coeff(kk) += valA * porosite_face(num_face * nb_comp + nc);
                            if (tab2[kk] - 1 == j * nb_comp + nc)
                              coeff(kk) -= valA * porosite_face(j * nb_comp + nc);
                          }
                        for (int kk = tab1[j * nb_comp + nc] - 1; kk < tab1[j * nb_comp + nc + 1] - 1; kk++)
                          {
                            if (tab2[kk] - 1 == num_face * nb_comp + nc)
                              coeff(kk) -= valA * porosite_face(num_face * nb_comp + nc);
                            if (tab2[kk] - 1 == j * nb_comp + nc)
                              coeff(kk) += valA * porosite_face(j * nb_comp + nc);
                          }
                      }
                  }
              }
          }
    }

  // On traite les faces internes
  for (int num_face0 = domaine_VEF.premiere_face_int(); num_face0 < n1; num_face0++)
    for (int ll = 0; ll < 2; ll++)
      {
        const int elem = face_voisins(num_face0, ll);
        const double d_mu = mu + mu_turb(elem);
        // On elimine les elements avec CL de paroi (rang>=1)
        int rang = rang_elem_non_std(elem);
        if (rang < 1)
          {
            for (int i = 0; i < nb_faces_elem; i++)
              {
                const int j = elem_faces(elem, i);
                if (j > num_face0)
                  {
                    const double valA = viscA_Q1(domaine_VEF, num_face0, j, dimension, elem, d_mu);
                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        for (int kk = tab1[num_face0 * nb_comp + nc] - 1; kk < tab1[num_face0 * nb_comp + nc + 1] - 1; kk++)
                          {
                            if (tab2[kk] - 1 == num_face0 * nb_comp + nc)
                              coeff(kk) += valA * porosite_face(num_face0 * nb_comp + nc);
                            if (tab2[kk] - 1 == j * nb_comp + nc)
                              coeff(kk) -= valA * porosite_face(j * nb_comp + nc);
                          }
                        for (int kk = tab1[j * nb_comp + nc] - 1; kk < tab1[j * nb_comp + nc + 1] - 1; kk++)
                          {
                            if (tab2[kk] - 1 == num_face0 * nb_comp + nc)
                              coeff(kk) -= valA * porosite_face(num_face0 * nb_comp + nc);
                            if (tab2[kk] - 1 == j * nb_comp + nc)
                              coeff(kk) += valA * porosite_face(j * nb_comp + nc);
                          }
                      }
                  }
              }
          }
      }
}
