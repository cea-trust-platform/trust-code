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

#include <Op_VEF_Face.h>
#include <Matrice_Morse.h>
#include <Equation_base.h>
#include <Sortie.h>
#include <Probleme_base.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Schema_Temps_base.h>
#include <Milieu_base.h>
#include <Operateur_base.h>
#include <Operateur_Diff_base.h>
#include <Op_Conv_VEF_base.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <communications.h>
#include <Matrice_Morse_Diag.h>
#include <TRUSTTrav.h>
#include <Dirichlet_homogene.h>
#include <Periodique.h>
#include <Symetrie.h>

/*! @brief Dimensionnement de la matrice qui devra recevoir les coefficients provenant de la convection, de la diffusion pour le cas des faces.
 *
 *  Cette matrice a une structure de matrice morse.
 *  Nous commencons par calculer les tailles des tableaux tab1 et tab2.
 *
 */

void Op_VEF_Face::dimensionner(const Domaine_VEF& le_dom, const Domaine_Cl_VEF& le_dom_cl, Matrice_Morse& la_matrice) const
{
  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de
  // la convection, de la diffusion pour le cas des faces.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.
  // Pour ce faire il faut chercher les faces voisines de la face consideree.

  int num_face;
  int ndeb = 0;
  int nfin = le_dom.nb_faces();
  int nnnn = le_dom.nb_faces_tot();
  nfin = nnnn;
  int i, j, k, kk;
  int elem1, elem2;
  int nb_faces_elem = le_dom.domaine().nb_faces_elem();
  //const Conds_lim& les_cl = le_dom_cl.les_conditions_limites();
  const int nb_comp = le_dom_cl.equation().inconnue().valeurs().line_size();
  la_matrice.dimensionner(nfin * nb_comp, nfin * nb_comp, 0);

  IntVect& tab1 = la_matrice.get_set_tab1();
  IntVect& tab2 = la_matrice.get_set_tab2();
  DoubleVect& coeff = la_matrice.get_set_coeff();
  coeff = 0;

  const IntTab& elem_faces = le_dom.elem_faces();
  const IntTab& face_voisins = le_dom.face_voisins();

  // A chaque face on associe un tableau d'entiers et une liste de reels:
  // voisines[i] = {j t.q j>i et M(i,j) est non nul }

  //  IntVect rang_voisin(nfin*nb_comp);
  IntVect rang_voisin(nnnn * nb_comp);
  rang_voisin = nb_comp;

  // On traite toutes les faces

  for (num_face = 0; num_face < nfin; num_face++)
    {
      elem1 = face_voisins(num_face, 0);
      elem2 = face_voisins(num_face, 1);

      for (i = 0; i < nb_faces_elem; i++)
        {
          if ((j = elem_faces(elem1, i)) != num_face)
            {
              for (k = 0; k < nb_comp; k++)
                {
                  rang_voisin(num_face * nb_comp + k) += nb_comp;
                }
            }
          if (elem2 != -1)
            if ((j = elem_faces(elem2, i)) != num_face)
              {
                for (k = 0; k < nb_comp; k++)
                  {
                    rang_voisin(num_face * nb_comp + k) += nb_comp;
                  }
              }
        }
    }

  // les faces voisines de num_face etant desormais comtabilisees
  // nous dimensionnons tab1 et tab2 au nombre de faces

  tab1(0) = 1;
  for (num_face = ndeb; num_face < nfin; num_face++)
    {
      for (k = 0; k < nb_comp; k++)
        {
          tab1(num_face * nb_comp + 1 + k) = rang_voisin(num_face * nb_comp + k) + tab1(num_face * nb_comp + k);
        }
    }
  la_matrice.dimensionner(nfin * nb_comp, tab1(nfin * nb_comp) - 1);

  for (num_face = 0; num_face < nfin; num_face++)
    {
      for (k = 0; k < nb_comp; k++)
        {
          for (kk = 0; kk < nb_comp; kk++)
            {
              int modulo = (k + kk) % nb_comp;
              tab2[tab1[num_face * nb_comp + k] - 1 + kk] = num_face * nb_comp + 1 + modulo;
            }
          rang_voisin[num_face * nb_comp + k] = tab1[num_face * nb_comp + k] + nb_comp - 1;
        }
    }

  // On traite toutes les faces
  for (num_face = 0; num_face < nfin; num_face++)
    {
      elem1 = face_voisins(num_face, 0);
      elem2 = face_voisins(num_face, 1);

      for (i = 0; i < nb_faces_elem; i++)
        {
          if ((j = elem_faces(elem1, i)) != num_face)
            {
              for (k = 0; k < nb_comp; k++)
                {
                  for (kk = 0; kk < nb_comp; kk++)
                    {
                      int modulo = (k + kk) % nb_comp;
                      tab2[rang_voisin[num_face * nb_comp + k] + kk] = j * nb_comp + 1 + modulo;
                    }
                  rang_voisin[num_face * nb_comp + k] += nb_comp;
                }
            }
          if (elem2 != -1)
            if ((j = elem_faces(elem2, i)) != num_face)
              {
                for (k = 0; k < nb_comp; k++)
                  {
                    for (kk = 0; kk < nb_comp; kk++)
                      {
                        int modulo = (k + kk) % nb_comp;
                        tab2[rang_voisin[num_face * nb_comp + k] + kk] = j * nb_comp + 1 + modulo;
                      }
                    rang_voisin[num_face * nb_comp + k] += nb_comp;
                  }
              }
        }
    }
}

/*! @brief Modification des coef de la matrice et du second membre pour les conditions de Dirichlet
 *
 */

void Op_VEF_Face::modifier_pour_Cl(const Domaine_VEF& le_dom, const Domaine_Cl_VEF& le_dom_cl, Matrice_Morse& la_matrice, DoubleTab& secmem) const
{
  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de
  // la convection, de la diffusion pour le cas des faces.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.
  const Conds_lim& les_cl = le_dom_cl.les_conditions_limites();
  const IntVect& tab1 = la_matrice.get_tab1();
  DoubleVect& coeff = la_matrice.get_set_coeff();
  const DoubleTab& champ_inconnue = le_dom_cl.equation().inconnue().valeurs();
  const int nb_comp = champ_inconnue.line_size();
  ArrOfDouble normale(nb_comp);
  for (const auto &itr : les_cl)
    {
      const Cond_lim& la_cl = itr;
      if (sub_type(Dirichlet, la_cl.valeur()))
        {
          const Dirichlet& la_cl_Dirichlet = ref_cast(Dirichlet, la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF, la_cl.frontiere_dis());
          int nfaces = la_front_dis.nb_faces();
          for (int ind_face = 0; ind_face < nfaces; ind_face++)
            {
              int face = la_front_dis.num_face(ind_face);
              for (int comp = 0; comp < nb_comp; comp++)
                {
                  int idiag = tab1[face * nb_comp + comp] - 1;
                  coeff[idiag] = 1;
                  // pour les voisins
                  int nbvois = tab1[face * nb_comp + 1 + comp] - tab1[face * nb_comp + comp];
                  for (int k = 1; k < nbvois; k++)
                    {
                      coeff[idiag + k] = 0;
                    }
                  // pour le second membre
                  if (nb_comp == 1)
                    secmem(face) = la_cl_Dirichlet.val_imp(ind_face, 0);
                  else
                    secmem(face, comp) = la_cl_Dirichlet.val_imp(ind_face, comp);
                }
            }
        }
      if (sub_type(Dirichlet_homogene, la_cl.valeur()))
        {
          const Dirichlet_homogene& la_cl_Dirichlet_homogene = ref_cast(Dirichlet_homogene, la_cl.valeur());
          const Front_VF& la_front_dis = ref_cast(Front_VF, la_cl.frontiere_dis());
          int nfaces = la_front_dis.nb_faces_tot();
          for (int ind_face = 0; ind_face < nfaces; ind_face++)
            {
              int face = la_front_dis.num_face(ind_face);
              for (int comp = 0; comp < nb_comp; comp++)
                {
                  int idiag = tab1[face * nb_comp + comp] - 1;
                  coeff[idiag] = 1;
                  // pour les voisins
                  int nbvois = tab1[face * nb_comp + 1 + comp] - tab1[face * nb_comp + comp];
                  for (int k = 1; k < nbvois; k++)
                    {
                      coeff[idiag + k] = 0;
                    }
                  // pour le second membre
                  if (nb_comp == 1)
                    secmem(face) = la_cl_Dirichlet_homogene.val_imp(ind_face, 0);
                  else
                    secmem(face, comp) = la_cl_Dirichlet_homogene.val_imp(ind_face, comp);
                }
            }
        }
      if (sub_type(Symetrie, la_cl.valeur()))
        if (le_dom_cl.equation().inconnue().valeur().nature_du_champ() == vectoriel)
          {
            const IntVect& tab2 = la_matrice.get_tab2();
            const Front_VF& la_front_dis = ref_cast(Front_VF, la_cl.frontiere_dis());
            const DoubleTab& face_normales = le_dom.face_normales();
            int nfaces = la_front_dis.nb_faces_tot();
            ArrOfDouble somme(la_matrice.nb_colonnes()); // On dimensionne au plus grand
            for (int ind_face = 0; ind_face < nfaces; ind_face++)
              {
                int face = la_front_dis.num_face(ind_face);
                double max_coef = 0;
                int ind_max = -1;
                double n2 = 0;
                for (int comp = 0; comp < nb_comp; comp++)
                  {
                    normale[comp] = face_normales(face, comp);
                    if (std::fabs(normale[comp]) > std::fabs(max_coef))
                      {
                        max_coef = normale[comp];
                        ind_max = comp;
                      }
                    n2 += normale[comp] * normale[comp];
                  }
                normale /= sqrt(n2);
                max_coef = normale[ind_max];

                // On commence par recalculer secmem=secmem-A *present pour pouvoir modifier A (on en profite pour projeter)
                int nb_coeff_ligne = tab1[face * nb_comp + 1] - tab1[face * nb_comp];
                for (int k = 0; k < nb_coeff_ligne; k++)
                  {
                    for (int comp = 0; comp < nb_comp; comp++)
                      {
                        int j = tab2[tab1[face * nb_comp + comp] - 1 + k] - 1;
                        //assert(j!=(face*nb_comp+comp));
                        //if ((j>=(face*nb_comp))&&(j<(face*nb_comp+nb_comp)))
                        const double coef_ij = la_matrice(face * nb_comp + comp, j);
                        int face2 = j / nb_comp;
                        int comp2 = j - face2 * nb_comp;
                        secmem(face, comp) -= coef_ij * champ_inconnue(face2, comp2);
                      }
                  }
                double somme_b = 0;

                for (int comp = 0; comp < nb_comp; comp++)
                  somme_b += secmem(face, comp) * normale[comp];

                // on retire secmem.n n
                for (int comp = 0; comp < nb_comp; comp++)
                  secmem(face, comp) -= somme_b * normale[comp];

                // on doit remettre la meme diagonale partout on prend la moyenne
                double ref = 0;
                for (int comp = 0; comp < nb_comp; comp++)
                  {
                    int j0 = face * nb_comp + comp;
                    ref += la_matrice(j0, j0);
                  }
                ref /= nb_comp;

                for (int comp = 0; comp < nb_comp; comp++)
                  {
                    int j0 = face * nb_comp + comp;
                    double rap = ref / la_matrice(j0, j0);
                    for (int k = 0; k < nb_coeff_ligne; k++)
                      {
                        int j = tab2[tab1[j0] - 1 + k] - 1;
                        la_matrice(j0, j) *= rap;
                      }
                    assert(est_egal(la_matrice(j0, j0), ref));
                  }
                // on annule tous les coef extra diagonaux du bloc
                //
                for (int k = 1; k < nb_coeff_ligne; k++)
                  {
                    for (int comp = 0; comp < nb_comp; comp++)
                      {
                        int j = tab2[tab1[face * nb_comp + comp] - 1 + k] - 1;
                        assert(j != (face * nb_comp + comp));
                        if ((j >= (face * nb_comp)) && (j < (face * nb_comp + nb_comp)))
                          la_matrice(face * nb_comp + comp, j) = 0;
                      }
                  }

                // pour les blocs extra diagonaux on assure que Aij.ni=0
                //ArrOfDouble somme(nb_coeff_ligne);
                for (int k = 0; k < nb_coeff_ligne; k++)
                  {
                    somme[k] = 0;
                    int j = tab2[tab1[face * nb_comp] - 1 + k] - 1;

                    // le coeff j doit exister sur les nb_comp lignes
                    double dsomme = 0;
                    for (int comp = 0; comp < nb_comp; comp++)
                      dsomme += la_matrice(face * nb_comp + comp, j) * normale[comp];

                    // on retire somme ni

                    for (int comp = 0; comp < nb_comp; comp++)
                      // on modifie que les coefficients ne faisant pas intervenir u(face,comp)
                      if ((j < (face * nb_comp)) || (j >= (face * nb_comp + nb_comp)))
                        la_matrice(face * nb_comp + comp, j) -= (dsomme) * normale[comp];
                  }
                // Finalement on recalcule secmem=secmem+A*champ_inconnue (A a ete beaucoup modiife)
                for (int k = 0; k < nb_coeff_ligne; k++)
                  {
                    for (int comp = 0; comp < nb_comp; comp++)
                      {
                        int j = tab2[tab1[face * nb_comp + comp] - 1 + k] - 1;
                        int face2 = j / nb_comp;
                        int comp2 = j - face2 * nb_comp;
                        const double coef_ij = la_matrice(face * nb_comp + comp, j);
                        secmem(face, comp) += coef_ij * champ_inconnue(face2, comp2);
                      }
                  }
                {
                  // verification
                  double somme_c = 0;
                  for (int comp = 0; comp < nb_comp; comp++)
                    somme_c += secmem(face, comp) * normale[comp];
                  // on retire secmem.n n
                  for (int comp = 0; comp < nb_comp; comp++)
                    secmem(face, comp) -= somme_c * normale[comp];
                }
              }
          }
    }
}

void Op_VEF_Face::modifier_flux(const Operateur_base& op) const
{
  controle_modifier_flux_ = 1;
  DoubleTab& flux_bords_ = op.flux_bords();
  if (flux_bords_.nb_dim() != 2)
    return;
  const Probleme_base& pb = op.equation().probleme();

  const Domaine_VEF& le_dom_vef = ref_cast(Domaine_VEF, op.equation().domaine_dis().valeur());
  int nb_compo = flux_bords_.dimension(1);
  // On multiplie le flux au bord par rho*Cp sauf si c'est un operateur de diffusion avec la conductivite comme champ
  if (op.equation().inconnue().le_nom() == "temperature" && !( sub_type(Operateur_Diff_base,op) && ref_cast(Operateur_Diff_base,op).diffusivite().le_nom() == "conductivite"))
    {
      const Champ_base& rho = (op.equation()).milieu().masse_volumique().valeur();
      const Champ_Don& Cp = (op.equation()).milieu().capacite_calorifique();
      int rho_uniforme = (sub_type(Champ_Uniforme,rho) ? 1 : 0);
      int cp_uniforme = (sub_type(Champ_Uniforme,Cp.valeur()) ? 1 : 0);
      double Cp_ = 0, rho_ = 0;
      int is_rho_u = pb.is_dilatable();
      if (is_rho_u)
        {
          is_rho_u = 0;
          if (sub_type(Op_Conv_VEF_base, op))
            {
              if (ref_cast(Op_Conv_VEF_base,op).vitesse().le_nom() == "rho_u")
                is_rho_u = 1;
            }
        }
      const int nb_faces_bords = le_dom_vef.nb_faces_bord();
      for (int face = 0; face < nb_faces_bords; face++)
        {
          if (cp_uniforme)
            Cp_ = Cp(0, 0);
          else
            {
              if (Cp.nb_comp() == 1)
                Cp_ = Cp(face);
              else
                Cp_ = Cp(face, 0);
            }
          if (rho_uniforme)
            rho_ = rho(0, 0);
          else
            {
              if (rho.nb_comp() == 1)
                rho_ = rho(face);
              else
                rho_ = rho(face, 0);
            }
          // si on est en QC temperature et si on a calcule div(rhou * T)
          // il ne faut pas remultiplier par rho
          if (is_rho_u)
            rho_ = 1;
          flux_bords_(face, 0) *= (rho_ * Cp_);
        }
    }

  // On multiplie par rho si Navier Stokes incompressible
  Nom nom_eqn = op.equation().que_suis_je();
  if (nom_eqn.debute_par("Navier_Stokes") && pb.milieu().que_suis_je() == "Fluide_Incompressible")
    {
      const Champ_base& rho = op.equation().milieu().masse_volumique().valeur();
      if (sub_type(Champ_Uniforme, rho))
        {
          double coef = rho(0, 0);
          int nb_faces_bord = le_dom_vef.nb_faces_bord();
          double *flux_bords_addr = computeOnTheDevice(flux_bords_);
          #pragma omp target teams distribute parallel for if (Objet_U::computeOnDevice)
          for (int face = 0; face < nb_faces_bord; face++)
            for (int k = 0; k < nb_compo; k++)
              flux_bords_addr[face * nb_compo + k] *= coef;
        }
    }

}

/*! @brief Impression des flux d'un operateur VEF aux faces (ie: diffusion, convection)
 *
 */
int Op_VEF_Face::impr(Sortie& os, const Operateur_base& op) const
{
  const Domaine_VEF& le_dom_vef = ref_cast(Domaine_VEF, op.equation().domaine_dis().valeur());
  DoubleTab& flux_bords_ = op.flux_bords();
  if (flux_bords_.nb_dim() != 2)
    {
      Cout << "L'impression des flux n'est pas codee pour l'operateur " << op.que_suis_je() << finl;
      return 1;
    }
  if (controle_modifier_flux_ == 0)
    if (max_abs_array(flux_bords_) != 0)
      {
        Cerr << op.que_suis_je() << " appelle  Op_VEF_Face::impr sans avoir appeler  Op_VEF_Face::modifier_flux, on arrete tout " << finl;
        Process::exit();
      }
  int nb_compo = flux_bords_.dimension(1);
  const Probleme_base& pb = op.equation().probleme();
  const Schema_Temps_base& sch = pb.schema_temps();
  // On n'imprime les moments que si demande et si on traite l'operateur de diffusion de la vitesse
  int impr_mom = 0;
  if (le_dom_vef.domaine().moments_a_imprimer() && sub_type(Operateur_Diff_base, op) && op.equation().inconnue().le_nom() == "vitesse")
    impr_mom = 1;

  const int impr_sum = (le_dom_vef.domaine().bords_a_imprimer_sum().est_vide() ? 0 : 1);
  const int impr_bord = (le_dom_vef.domaine().bords_a_imprimer().est_vide() ? 0 : 1);

  // Calcul des moments
  DoubleTab xgr;
  if (impr_mom)
    xgr = le_dom_vef.calculer_xgr();

  // On parcours les frontieres pour sommer les flux par frontiere dans le tableau flux_bord
  DoubleVect bilan(nb_compo);
  bilan = 0;
  int nb_cl = le_dom_vef.nb_front_Cl();
  DoubleTrav flux_bords(4, nb_cl, nb_compo);
  flux_bords = 0.;
  /*
   flux_bord(k)             ->     flux_bords(0,num_cl,k)
   flux_bord_perio1(k)      ->     flux_bords(1,num_cl,k)
   flux_bord_perio2(k)      ->     flux_bords(2,num_cl,k)
   moment(k)                ->     flux_bords(3,num_cl,k)
   */
  for (int num_cl = 0; num_cl < nb_cl; num_cl++)
    {
      const Cond_lim& la_cl = op.equation().domaine_Cl_dis().les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF, la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      int perio = (sub_type(Periodique,la_cl.valeur()) ? 1 : 0);
      for (int face = ndeb; face < nfin; face++)
        {
          for (int k = 0; k < nb_compo; k++)
            {
              flux_bords(0, num_cl, k) += flux_bords_(face, k);
              if (perio)
                {
                  if (face < (ndeb + frontiere_dis.nb_faces() / 2))
                    flux_bords(1, num_cl, k) += flux_bords_(face, k);
                  else
                    flux_bords(2, num_cl, k) += flux_bords_(face, k);
                }
            }
          if (impr_mom)
            {
              // Calcul du moment exerce par le fluide sur le bord (OM/\F)
              if (Objet_U::dimension == 2)
                flux_bords(3, num_cl, 0) += flux_bords_(face, 1) * xgr(face, 0) - flux_bords_(face, 0) * xgr(face, 1);
              else
                {
                  flux_bords(3, num_cl, 0) += flux_bords_(face, 2) * xgr(face, 1) - flux_bords_(face, 1) * xgr(face, 2);
                  flux_bords(3, num_cl, 1) += flux_bords_(face, 0) * xgr(face, 2) - flux_bords_(face, 2) * xgr(face, 0);
                  flux_bords(3, num_cl, 2) += flux_bords_(face, 1) * xgr(face, 0) - flux_bords_(face, 0) * xgr(face, 1);
                }
            }
        } // fin for face
    } // fin for num_cl

  // On somme les contributions de chaque processeur
  mp_sum_for_each_item(flux_bords);

  // Ecriture dans les fichiers
  if (Process::je_suis_maitre())
    {
      // Open files if needed
      //SFichier Flux;
      if (!Flux.is_open())
        op.ouvrir_fichier(Flux, "", 1);
      //SFichier Flux_moment;
      if (!Flux_moment.is_open())
        op.ouvrir_fichier(Flux_moment, "moment", impr_mom);
      //SFichier Flux_sum;
      if (!Flux_sum.is_open())
        op.ouvrir_fichier(Flux_sum, "sum", impr_sum);

      // Write time
      Flux.add_col(sch.temps_courant());
      if (impr_mom)
        Flux_moment.add_col(sch.temps_courant());
      if (impr_sum)
        Flux_sum.add_col(sch.temps_courant());

      // Write flux on boundaries
      for (int num_cl = 0; num_cl < nb_cl; num_cl++)
        {
          const Frontiere_dis_base& la_fr = op.equation().domaine_Cl_dis().les_conditions_limites(num_cl).frontiere_dis();
          const Cond_lim& la_cl = op.equation().domaine_Cl_dis().les_conditions_limites(num_cl);
          int perio = (sub_type(Periodique,la_cl.valeur()) ? 1 : 0);
          for (int k = 0; k < nb_compo; k++)
            {
              if (perio)
                {
                  Flux.add_col(flux_bords(1, num_cl, k));
                  Flux.add_col(flux_bords(2, num_cl, k));
                }
              else
                Flux.add_col(flux_bords(0, num_cl, k));
              if (impr_mom)
                Flux_moment.add_col(flux_bords(3, num_cl, k));
              if (le_dom_vef.domaine().bords_a_imprimer_sum().contient(la_fr.le_nom()))
                Flux_sum.add_col(flux_bords(0, num_cl, k));

              // On somme les flux de toutes les frontieres pour mettre dans le tableau bilan
              bilan(k) += flux_bords(0, num_cl, k);
            }
        }

      // On imprime les bilans et on va a la ligne
      for (int k = 0; k < nb_compo; k++)
        Flux.add_col(bilan(k));
      Flux << finl;
      if (impr_mom)
        Flux_moment << finl;
      if (impr_sum)
        Flux_sum << finl;
    }

  const LIST(Nom) &Liste_bords_a_imprimer = le_dom_vef.domaine().bords_a_imprimer();
  if (!Liste_bords_a_imprimer.est_vide())
    {
      EcrFicPartage Flux_face;
      op.ouvrir_fichier_partage(Flux_face, "", impr_bord);
      // Impression sur chaque face si demande
      for (int num_cl = 0; num_cl < nb_cl; num_cl++)
        {
          const Frontiere_dis_base& la_fr = op.equation().domaine_Cl_dis().les_conditions_limites(num_cl).frontiere_dis();
          const Cond_lim& la_cl = op.equation().domaine_Cl_dis().les_conditions_limites(num_cl);
          const Front_VF& frontiere_dis = ref_cast(Front_VF, la_cl.frontiere_dis());
          int ndeb = frontiere_dis.num_premiere_face();
          int nfin = ndeb + frontiere_dis.nb_faces();
          // Impression sur chaque face
          if (Liste_bords_a_imprimer.contient(la_fr.le_nom()))
            {
              Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps ";
              sch.imprimer_temps_courant(Flux_face);
              Flux_face << " : " << finl;
              const DoubleTab& xv = le_dom_vef.xv();
              for (int face = ndeb; face < nfin; face++)
                {
                  if (Objet_U::dimension == 2)
                    Flux_face << "# Face a x= " << xv(face, 0) << " y= " << xv(face, 1);
                  else if (Objet_U::dimension == 3)
                    Flux_face << "# Face a x= " << xv(face, 0) << " y= " << xv(face, 1) << " z= " << xv(face, 2);
                  for (int k = 0; k < nb_compo; k++)
                    Flux_face << " surface_face(m2)= " << le_dom_vef.face_surfaces(face) << " flux_par_surface(W/m2)= " << flux_bords_(face, k) / le_dom_vef.face_surfaces(face) << " flux(W)= "
                              << flux_bords_(face, k);
                  Flux_face << finl;
                }
              Flux_face.syncfile();
            }
        }
    }
  return 1;
}

/////////////////////////////////////////
// Methode pour l'implicite
/////////////////////////////////////////
void modif_matrice_pour_periodique_avant_contribuer(Matrice_Morse& matrice, const Equation_base& eqn)
{
  const int nb_comp = eqn.inconnue().valeurs().line_size();
  const Domaine_Cl_dis_base& domaine_Cl_VEF = eqn.domaine_Cl_dis().valeur();
  const Domaine_VF& domaine_VEF = ref_cast(Domaine_VF, eqn.domaine_dis().valeur());
  int nb_bords = domaine_VEF.nb_front_Cl();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  int nb_faces_elem = elem_faces.dimension(1);
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          // on ne parcourt que la moitie des faces periodiques
          // on copiera a la fin le resultat dans la face associe..
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces() / 2;
          for (int dir = 0; dir < 2; dir++)
            for (int num_face = num1; num_face < num2; num_face++)
              {
                int elem1 = face_voisins(num_face, dir);
                int fac_asso = la_cl_perio.face_associee(num_face - num1) + num1;
                for (int i = 0; i < nb_faces_elem; i++)
                  {
                    int j = elem_faces(elem1, i);
                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        int n0 = num_face * nb_comp + nc;
                        int n0perio = fac_asso * nb_comp + nc;
                        if (((j == num_face) || (j == fac_asso)))
                          {
                            if (dir == 0)
                              {
                                assert(matrice(n0, n0perio) == 0);
                                assert(matrice(n0perio, n0) == 0);
                                assert(matrice(n0, n0) == matrice(n0perio, n0perio));
                                double titi = (matrice(n0, n0)) / 2.;
                                matrice(n0, n0) = titi;
                                matrice(n0perio, n0perio) = titi;
                              }
                          }
                        else
                          {
                            for (int nc2 = 0; nc2 < nb_comp; nc2++)
                              {
                                int j20 = j * nb_comp + nc2;
                                assert(matrice(n0, j20) == matrice(n0perio, j20));
                                double titi = (matrice(n0, j20) / 2.);
                                matrice(n0, j20) = titi;
                                matrice(n0perio, j20) = titi;
                              }
                          }

                      }
                  }
              }
        }
    }
  //  matrice.imprimer(Cerr);
}

void modif_matrice_pour_periodique_apres_contribuer(Matrice_Morse& matrice, const Equation_base& eqn)
{
  const int nb_comp = eqn.inconnue().valeurs().line_size();
  const Domaine_Cl_dis_base& domaine_Cl_VEF = eqn.domaine_Cl_dis().valeur();
  const Domaine_VF& domaine_VEF = ref_cast(Domaine_VF, eqn.domaine_dis().valeur());
  int nb_bords = domaine_VEF.nb_front_Cl();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  int nb_faces_elem = elem_faces.dimension(1);

  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          // on ne parcourt que la moitie des faces periodiques
          // on copiera a la fin le resultat dans la face associe..
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces() / 2;
          for (int dir = 0; dir < 2; dir++)
            for (int num_face = num1; num_face < num2; num_face++)
              {
                int elem1 = face_voisins(num_face, dir);
                int fac_asso = la_cl_perio.face_associee(num_face - num1) + num1;
                for (int i = 0; i < nb_faces_elem; i++)
                  {
                    int j = elem_faces(elem1, i);
                    //if (j!=num_face)
                    for (int nc = 0; nc < nb_comp; nc++)
                      {
                        int n0 = num_face * nb_comp + nc;
                        //    int j0=j*nb_comp+nc;
                        int n0perio = fac_asso * nb_comp + nc;
                        if (((j == num_face) || (j == fac_asso)))
                          {
                            if (dir == 0)
                              {
                                for (int nc2 = 0; nc2 < nb_comp; nc2++)
                                  {
                                    int j0 = num_face * nb_comp + nc2;
                                    int j0perio = fac_asso * nb_comp + nc2;
                                    matrice(n0, j0) += matrice(n0, j0perio);
                                    matrice(n0, j0perio) = 0;
                                    matrice(n0perio, j0perio) += matrice(n0perio, j0);
                                    matrice(n0perio, j0) = 0;
                                    double titi = (matrice(n0, j0) + matrice(n0perio, j0perio));
                                    matrice(n0, j0) = titi;
                                    if (nc != nc2)
                                      {
                                        matrice(n0perio, j0) = titi;
                                        matrice(n0perio, j0perio) = 0;
                                      }
                                    else
                                      matrice(n0perio, j0perio) = titi;
                                  }
                              }
                          }
                        else
                          {
                            for (int nc2 = 0; nc2 < nb_comp; nc2++)
                              {
                                int j20 = j * nb_comp + nc2;
                                double titi = (matrice(n0, j20) + matrice(n0perio, j20));
                                matrice(n0, j20) = titi;
                                matrice(n0perio, j20) = titi;
                              }
                          }
                      }
                  }
              }
        }
    }
  //  matrice.imprimer(Cerr);
}

/*! @brief divise les coefficients sur les ligne des faces periodiques par 2 en prevision de l'application modifier_matrice_pour_periodique_apres_contribuer qui va sommer les 2 lignes des faces periodiques associees
 *
 */

void Op_VEF_Face::modifier_matrice_pour_periodique_avant_contribuer(Matrice_Morse& matrice, const Equation_base& eqn) const
{
  // si matrice_morse_diag pas de contribution n0 n0perio
  if (sub_type(Matrice_Morse_Diag, matrice))
    return;
  modif_matrice_pour_periodique_avant_contribuer(matrice, eqn);
}
/*! @brief Somme  les 2 lignes des faces periodiques associees permet de calculer dans le code sans se poser de question pour retrouver la face_associee
 *
 *  on ne parcourt que la moitiee des faces periodiques dans contribuer_a_avec (en general).
 *
 */

void Op_VEF_Face::modifier_matrice_pour_periodique_apres_contribuer(Matrice_Morse& matrice, const Equation_base& eqn) const
{
  // si matrice_morse_diag pas de contribution n0 n0perio
  if (sub_type(Matrice_Morse_Diag, matrice))
    return;

  modif_matrice_pour_periodique_apres_contribuer(matrice, eqn);

  // verification que la matrice est bien periodique
#ifndef  NDEBUG

  const int nb_comp = eqn.inconnue().valeurs().line_size();

  const Domaine_Cl_dis_base& domaine_Cl_VEF = eqn.domaine_Cl_dis().valeur();
  const Domaine_VF& domaine_VEF = ref_cast(Domaine_VF, eqn.domaine_dis().valeur());
  int nb_bords = domaine_VEF.nb_front_Cl();

  const IntVect& tab1 = matrice.get_tab1();
  const IntVect& tab2 = matrice.get_tab2();

  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      //      int num2 = num1 + le_bord.nb_faces();

      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          int fac_asso;
          // on ne parcourt que la moitie des faces periodiques
          // on copiera a la fin le resultat dans la face associe..
          int num2 = num1 + le_bord.nb_faces() / 2;
          for (int num_face = num1; num_face < num2; num_face++)
            for (int nc = 0; nc < nb_comp; nc++)
              {
                fac_asso = la_cl_perio.face_associee(num_face - num1) + num1;
                int n0 = num_face * nb_comp + nc;
                int n0perio = fac_asso * nb_comp + nc;
                // on verifie que les 2 lignes sont identiques ( sauf la case diagonale qui n'est pas au meme endroit)
                for (int j = tab1[n0] - 1; j < tab1[n0 + 1] - 1; j++)
                  {
                    int c = tab2[j] - 1;
                    if ((c != n0) && (c != n0perio))
                      {
                        double test = matrice(n0, c) - matrice(n0perio, c);
                        if (test != 0)
                          {
                            Cerr << "Pb matrice non periodique face" << num_face << " composante " << nc << " colonne " << c << finl;
                            Cerr << " diff " << test << " coef1 " << matrice(n0, c) << " coef2 " << matrice(n0perio, c) << finl;
                            Process::exit();
                          }
                      }
                  }
                if ((matrice(n0, n0perio) != 0) || (matrice(n0perio, n0) != 0))
                  {
                    Cerr << "Pb matrice non periodique face" << num_face << " composante " << nc << finl;
                    Cerr << " coef  non nul" << matrice(n0, n0perio) << " " << matrice(n0perio, n0) << finl;
                    Process::exit();
                  }
                if (matrice(n0, n0) != matrice(n0perio, n0perio))
                  {
                    double test = matrice(n0, n0perio) - matrice(n0perio, n0);
                    Cerr << "Pb matrice non periodique face" << num_face << " composante " << nc << finl;
                    Cerr << " diff " << test << " coef  different " << matrice(n0, n0) << " " << matrice(n0perio, n0perio) << finl;
                    Process::exit();
                  }
              }
        }
    }

  /*
   Matrice_Morse es(matrice);
   modif_matrice_pour_periodique_avant_contribuer(es,eqn);
   modif_matrice_pour_periodique_apres_contribuer(es,eqn);
   es.coeff_-=(matrice.coeff_);
   Cerr<<" erreur apres modifier_matrice_pour_periodique_apres_contribuer"<< mp_max_abs_vect(es.coeff_)<<finl;
   assert(mp_max_abs_vect(es.coeff_)<1e-9);
   */

#endif
}
