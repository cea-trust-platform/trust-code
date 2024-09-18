/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Neumann_sortie_libre.h>
#include <Assembleur_P_PolyMAC.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Face_PolyMAC.h>
#include <Matrice_Diagonale.h>
#include <Matrice_Morse_Sym.h>
#include <Matrice_Bloc_Sym.h>
#include <Static_Int_Lists.h>
#include <Domaine_PolyMAC.h>
#include <TRUSTTab_parts.h>
#include <Operateur_Grad.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Milieu_base.h>
#include <Array_tools.h>
#include <Dirichlet.h>
#include <Debog.h>

extern Stat_Counter_Id assemblage_sys_counter_;

Implemente_instanciable(Assembleur_P_PolyMAC,"Assembleur_P_PolyMAC",Assembleur_base);

Sortie& Assembleur_P_PolyMAC::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Assembleur_P_PolyMAC::readOn(Entree& s) { return Assembleur_base::readOn(s); }

int Assembleur_P_PolyMAC::assembler(Matrice& la_matrice)
{
  DoubleVect rien;
  return assembler_mat(la_matrice, rien, 1, 1);
}

int Assembleur_P_PolyMAC::assembler_rho_variable(Matrice& la_matrice, const Champ_Don_base& rho)
{
  abort();
  return 0;
}

int Assembleur_P_PolyMAC::assembler_mat(Matrice& la_matrice, const DoubleVect& diag, int incr_pression, int resoudre_en_u)
{
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);
  Cerr << "Assemblage de la matrice de pression ... ";
  statistiques().begin_count(assemblage_sys_counter_);
  la_matrice.typer("Matrice_Morse");
  Matrice_Morse& mat = ref_cast(Matrice_Morse, la_matrice.valeur());

  const Domaine_PolyMAC& domaine = ref_cast(Domaine_PolyMAC, le_dom_PolyMAC.valeur());
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins();
  const DoubleVect& fs = domaine.face_surfaces(), &pf = mon_equation->milieu().porosite_face(), &pe = mon_equation->milieu().porosite_elem(), &ve = domaine.volumes();
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, mon_equation->inconnue());
  int i, j, k, e, f, fb, n_f, ne = domaine.nb_elem(), ne_tot = domaine.nb_elem_tot(), nf = domaine.nb_faces(), nf_tot = domaine.nb_faces_tot(), na_tot =
                                                                                                                          dimension < 3 ? domaine.domaine().nb_som_tot() : domaine.domaine().nb_aretes_tot(), infoo;
  domaine.init_m2(), ch.fcl();

  DoubleTrav W(e_f.dimension(1), e_f.dimension(1)), W0(e_f.dimension(1), e_f.dimension(1));


  //en l'absence de CLs en pression, on ajoute P(0) = 0 sur le process 0
  has_P_ref = 0;
  for (int n_bord = 0; n_bord < le_dom_PolyMAC->nb_front_Cl(); n_bord++)
    if (sub_type(Neumann_sortie_libre, le_dom_Cl_PolyMAC->les_conditions_limites(n_bord).valeur()))
      has_P_ref = 1;

  /* 1. stencils de la matrice en pression et de rec : seulement au premier passage */
  if (!stencil_done)
    {
      IntTab stencil_M(0, 2), stencil_R(0, 2);

      for (e = 0; e < ne_tot; e++)
        {
          for (i = 0, j = domaine.m2d(e), n_f = domaine.m2d(e + 1) - domaine.m2d(e); i < n_f; i++, j++)
            {
              for (k = domaine.w2i(j), f = e_f(e, i); f < nf && k < domaine.w2i(j + 1); k++)
                {
                  fb = e_f(e, domaine.w2j(k));
                  stencil_M.append_line(ne_tot + f, ne_tot + fb);
                  if (e == f_e(f, 0))
                    stencil_R.append_line(f, ne_tot + fb);
                }
              if (ch.fcl()(f, 0) != 1 && e < ne)
                stencil_M.append_line(e, ne_tot + f);
              if (ch.fcl()(f, 0) != 1 && f < nf)
                stencil_M.append_line(ne_tot + f, e);
              if (e == f_e(f, 0) && f < nf)
                stencil_R.append_line(f, e);
            }
          if (e < ne)
            stencil_M.append_line(e, e);
          // if (!has_P_ref && !Process::me() && e < ne) stencil_M.append_line(0, e);
        }

      tableau_trier_retirer_doublons(stencil_M), tableau_trier_retirer_doublons(stencil_R);
      Matrix_tools::allocate_morse_matrix(ne_tot + nf_tot, ne_tot + nf_tot, stencil_M, mat);
      Matrix_tools::allocate_morse_matrix(nf_tot + na_tot, ne_tot + nf_tot, stencil_R, rec);
      tab1.ref_array(mat.get_set_tab1()), tab2.ref_array(mat.get_set_tab2());
      stencil_done = 1;
    }
  else //sinon, on recycle
    {
      mat.get_set_tab1().ref_array(tab1);
      mat.get_set_tab2().ref_array(tab2);
      mat.get_set_coeff().resize(tab2.size());
      mat.set_nb_columns(ne_tot + nf_tot);
      rec.get_set_coeff() = 0;
    }

  /* 2. remplissage des coefficients */
  for (e = 0; e < ne_tot; e++)
    {
      n_f = domaine.m2d(e + 1) - domaine.m2d(e), W0.resize(n_f, n_f), W.resize(n_f, n_f);
      for (i = 0, j = domaine.m2d(e), W0 = 0; i < n_f; i++, j++)
        for (k = domaine.w2i(j); k < domaine.w2i(j + 1); k++)
          W0(i, domaine.w2j(k)) = domaine.w2c(k);
      if (!diag.size())
        W = W0; //pas de correction diagonale -> on prend W telle quelle
      else //correction diagonale -> on re-inverse m2 + diag
        {
          //matrice m2 + correction diagonale
          for (i = 0, j = domaine.m2d(e), W = 0; i < n_f; i++, j++)
            for (k = domaine.m2i(j); k < domaine.m2i(j + 1); k++)
              W(i, domaine.m2j(k)) = domaine.m2c(k);
          for (i = 0; i < n_f; i++)
            f = e_f(e, i), W(i, i) += diag(f) * domaine.volumes_entrelaces_dir()(f, e != f_e(f, 0)) / domaine.volumes_entrelaces(f) / ve(e);
          //inversion par Cholesky (Lapack) + annulation des petits coeffs + remplissage a la main de la partir triangulaire inf
          char uplo = 'U';
          F77NAME(dpotrf)(&uplo, &n_f, W.addr(), &n_f, &infoo);
          F77NAME(dpotri)(&uplo, &n_f, W.addr(), &n_f, &infoo);
          for (i = 0; i < n_f; i++)
            for (j = i + 1; j < n_f; j++)
              W(i, j) = W(j, i);
          for (i = 0; i < n_f; i++)
            for (j = 0; j < n_f; j++)
              if (W0(i, j) == 0)
                W(i, j) = 0;
        }

      //remplissage de la matrice en (dPe, dPf)
      //sur les CLs de Neumann, on remplace l'equation sur dPf par dPf = 0 et on retire dPf des autres equations (pour symetrie)
      double mee, mef, mff, rfe, rff; //a ajouter a m[e][e], m[e][f] / m[f][e], m[f][f'], r[f][e], r[f][f']
      for (i = 0, mee = 0; i < n_f; mee += mef, i++)
        {
          for (f = e_f(e, i), mef = 0, rfe = 0, j = 0; f < nf && j < n_f; mef += mff, rfe += rff, j++, mff = 0, rff = 0)
            {
              fb = e_f(e, j), mff = fs(f) * fs(fb) * pe(e) * W(i, j) / ve(e), rff = e == f_e(f, 0) ? fs(fb) * pe(e) * W(i, j) / (ve(e) * pf(f)) : 0;
              if (mff && ch.fcl()(f, 0) != 1 && ch.fcl()(fb, 0) != 1)
                mat(ne_tot + f, ne_tot + fb) += mff;
              else if (ch.fcl()(f, 0) == 1 && f == fb)
                mat(ne_tot + f, ne_tot + fb) += 1;
              if (rff)
                rec(f, ne_tot + fb) += rff;
            }
          if (ch.fcl()(f, 0) != 1 && e < ne)
            mat(e, ne_tot + f) -= mef;
          if (ch.fcl()(f, 0) != 1 && f < nf)
            mat(ne_tot + f, e) -= mef;
          if (e == f_e(f, 0) && f < nf)
            rec(f, e) -= rfe;
        }
      if (e < ne)
        mat(e, e) += mee;
    }

  if (!has_P_ref && !Process::me())
    mat(0, 0) *= 2;
  // {
  //   double coeff = mat(0, 0) / ne;
  //   for (e = 0; e < ne; e++) mat(0, e) += coeff;
  // }

  statistiques().end_count(assemblage_sys_counter_);
  Cerr << statistiques().last_time(assemblage_sys_counter_) << " s" << finl;
  return 1;
}

/*! @brief Assemble la matrice de pression pour un fluide quasi compressible laplacein(P) est remplace par div(grad(P)/rho).
 *
 * @param (DoubleTab& tab_rho) mass volumique Valeurs par dPolyMACaut:
 * @return (int) renvoie toujours 1
 * @throws PolyMACfets de bord:
 */
int Assembleur_P_PolyMAC::assembler_QC(const DoubleTab& tab_rho, Matrice& matrice)
{
  Cerr << "Assemblage de la matrice de pression pour Quasi Compressible en cours..." << finl;
  assembler(matrice);
  set_resoudre_increment_pression(1);
  set_resoudre_en_u(0);
  abort();
  Matrice_Bloc& matrice_bloc = ref_cast(Matrice_Bloc, matrice.valeur());
  Matrice_Morse_Sym& la_matrice = ref_cast(Matrice_Morse_Sym, matrice_bloc.get_bloc(0, 0).valeur());
  if ((la_matrice.get_est_definie() != 1) && (1))
    {
      Cerr << "Pas de pression imposee  --> P(0)=0" << finl;
      if (je_suis_maitre())
        la_matrice(0, 0) *= 2;
      la_matrice.set_est_definie(1);
    }

  Cerr << "Fin de l'assemblage de la matrice de pression" << finl;
  return 1;
}

int Assembleur_P_PolyMAC::modifier_secmem(DoubleTab& secmem)
{
  Debog::verifier("secmem dans modifier secmem", secmem);

  const Domaine_PolyMAC& le_dom = le_dom_PolyMAC.valeur();
  const Domaine_Cl_PolyMAC& le_dom_cl = le_dom_Cl_PolyMAC.valeur();
  int nb_cond_lim = le_dom_cl.nb_cond_lim();
  const IntTab& face_voisins = le_dom.face_voisins();

  // Modification du second membre :
  int i;
  for (i = 0; i < nb_cond_lim; i++)
    {
      const Cond_lim_base& la_cl_base = le_dom_cl.les_conditions_limites(i).valeur();
      const Front_VF& la_front_dis = ref_cast(Front_VF, la_cl_base.frontiere_dis());
      const Champ_front_base& champ_front = la_cl_base.champ_front().valeur();
      int ndeb = la_front_dis.num_premiere_face();
      int nfin = ndeb + la_front_dis.nb_faces();

      // GF on est passe en increment de pression
      if ((sub_type(Neumann_sortie_libre, la_cl_base)) && (!get_resoudre_increment_pression()))
        {
          double Pimp, coPolyMAC;
          const Neumann_sortie_libre& la_cl_Neumann = ref_cast(Neumann_sortie_libre, la_cl_base);
          // const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl_base.frontiere_dis());
          //int ndeb = la_front_dis.num_premiere_face();
          //int nfin = ndeb + la_front_dis.nb_faces();
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              Pimp = la_cl_Neumann.flux_impose(num_face - ndeb);
              coPolyMAC = les_coeff_pression[num_face] * Pimp;
              secmem[face_voisins(num_face, 0)] += coPolyMAC;
            }
        }
      else if (sub_type(Dirichlet, la_cl_base) && champ_front.instationnaire() && get_resoudre_en_u() )
        {
          const DoubleTab& Gpt = champ_front.derivee_en_temps();
          bool ch_unif = (Gpt.nb_dim() == 1);
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              double Stt = 0.;
              for (int k = 0; k < dimension; k++)
                {
                  double Gpoint = ch_unif ? Gpt(k) : Gpt(num_face - ndeb, k);
                  Stt -= Gpoint * le_dom.face_normales(num_face, k);
                }
              secmem(face_voisins(num_face, 0)) += Stt;
            }
        }
    }

  secmem.echange_espace_virtuel();
  Debog::verifier("secmem dans modifier secmem fin", secmem);
  return 1;
}

int Assembleur_P_PolyMAC::modifier_solution(DoubleTab& pression)
{
  Debog::verifier("pression dans modifier solution in", pression);
  //on ne considere pas les pressions aux faces dans le min (solveur_U_P ne les met pas a jour)
  DoubleTab_parts ppart(pression);
  if (!has_P_ref) pression -= mp_min_vect(ppart[0]);
  return 1;
}

const Domaine_dis_base& Assembleur_P_PolyMAC::domaine_dis_base() const
{
  return le_dom_PolyMAC.valeur();
}

const Domaine_Cl_dis_base& Assembleur_P_PolyMAC::domaine_Cl_dis_base() const
{
  return le_dom_Cl_PolyMAC.valeur();
}

void Assembleur_P_PolyMAC::associer_domaine_dis_base(const Domaine_dis_base& le_dom_dis)
{
  le_dom_PolyMAC = ref_cast(Domaine_PolyMAC, le_dom_dis);
}

void Assembleur_P_PolyMAC::associer_domaine_cl_dis_base(const Domaine_Cl_dis_base& le_dom_Cl_dis)
{
  le_dom_Cl_PolyMAC = ref_cast(Domaine_Cl_PolyMAC, le_dom_Cl_dis);
}

void Assembleur_P_PolyMAC::completer(const Equation_base& Eqn)
{
  mon_equation = Eqn;
  stencil_done = 0;
}
