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

#include <Echange_Thermique_Volumique_Elem.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Domaine_VF.h>
#include <Param.h>
#include <Synonyme_info.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Navier_Stokes_std.h>
#include <Pb_Multiphase.h>
#include <Flux_parietal_base.h>

Implemente_instanciable(Echange_Thermique_Volumique_Elem, "Echange_Thermique_Volumique_Elem", Source_base);
Add_synonym(Echange_Thermique_Volumique_Elem, "Echange_Thermique_Volumique_Elem_VDF_P0_VDF");
Add_synonym(Echange_Thermique_Volumique_Elem, "Echange_Thermique_Volumique_Elem_PolyMAC_P0");
Add_synonym(Echange_Thermique_Volumique_Elem, "Echange_Thermique_Volumique_Elem_PolyMAC_P0P1NC");
Add_synonym(Echange_Thermique_Volumique_Elem, "Echange_Thermique_Volumique_Elem_PolyVEF_P0");
Add_synonym(Echange_Thermique_Volumique_Elem, "Echange_Thermique_Volumique_Elem_PolyVEF_P0P1");
Add_synonym(Echange_Thermique_Volumique_Elem, "Echange_Thermique_Volumique_Elem_PolyVEF_P0P1NC");



Sortie& Echange_Thermique_Volumique_Elem::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Echange_Thermique_Volumique_Elem::readOn(Entree& s)
{
  Param param(que_suis_je());
  param.ajouter("nom|name", &tag_, Param::REQUIRED);
  param.ajouter("aire_interfaciale|interfacial_area", &Ai_, Param::REQUIRED);
  param.ajouter("conduction_length|epaisseur_conduction", &ep_cond_);
  param.ajouter("conductivity|conductivite", &cond_);
  param.ajouter_non_std("flux_parietal|heat_flux", (this));
  param.lire_avec_accolades_depuis(s);
  set_fichier("Echange_Thermique_Volumique");
  set_description("Power (W)");
  Noms col_names;
  col_names.add("Power");
  set_col_names(col_names);
  return s;
}

int Echange_Thermique_Volumique_Elem::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "flux_parietal" || mot == "heat_flux")
    {
      Correlation_base::typer_lire_correlation(flux_par_, equation().probleme(), "flux_parietal", is);
      return 1;
    }
  return 1;
}

int Echange_Thermique_Volumique_Elem::initialiser(double temps)
{
  Ai_->initialiser(temps);
  if (ep_cond_.non_nul()) ep_cond_->initialiser(temps);
  /* recherche du terme source de l'autre cote */
  if (!equation().probleme().is_coupled())
    Process::exit(que_suis_je() + " can only be used in a coupled problem!");
  for (int i = 0; i < equation().probleme().get_pb_couple().nb_problemes(); i++)
    {
      Probleme_base& o_pb = ref_cast(Probleme_base, equation().probleme().get_pb_couple().probleme(i));
      if (&o_pb != &equation().probleme())
        for(int j = 0; j < o_pb.nombre_d_equations(); j++)
          for (auto &so : o_pb.equation(j).sources())
            if (sub_type(Echange_Thermique_Volumique_Elem, so.valeur()) && ref_cast(Echange_Thermique_Volumique_Elem, so.valeur()).tag_ == tag_)
              o_ech_ = ref_cast(Echange_Thermique_Volumique_Elem, so.valeur());
    }
  if (!o_ech_.non_nul()) //pas trouve
    Process::exit(que_suis_je() + " : could not find matching term for name " + tag_ + " in problem " + equation().probleme().le_nom() + " !");

  return Source_base::initialiser(temps);
}

void Echange_Thermique_Volumique_Elem::mettre_a_jour(double temps)
{
  Ai_->mettre_a_jour(temps);
  if (ep_cond_.non_nul()) ep_cond_->mettre_a_jour(temps);
  if (cond_.non_nul()) cond_->mettre_a_jour(temps);
}

void Echange_Thermique_Volumique_Elem::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  std::string nom_inc = equation().inconnue().le_nom().getString(), o_nom_inc = nom_inc + "/" + o_ech_->equation().probleme().le_nom().getString();
  if (semi_impl.count(nom_inc)) return; //semi-implicite -> pas de derivees
  Matrice_Morse *mat[2] = { matrices.count(nom_inc) ? matrices.at(nom_inc) : nullptr, matrices.count(o_nom_inc) ? matrices.at(o_nom_inc) : nullptr };
  const DoubleTab *vals[2] = { &equation().inconnue().valeurs(), &o_ech_->equation().inconnue().valeurs() };
  const Domaine_VF& dom = ref_cast(Domaine_VF, equation().domaine_dis());
  int i, e, o_e, ne_tot = dom.nb_elem_tot(), n, N = vals[0]->line_size(), m, M = vals[1]->line_size();

  /* aire interfaciale par maille */
  DoubleTab Ai(ne_tot, 1);
  IntVect polys(ne_tot);
  for (e = 0; e < ne_tot; e++) polys(e) = e;
  Ai_->valeur_aux_elems(dom.xp(), polys, Ai);
  /* matrice du remapper */
  const std::vector<std::map<mcIdType,double>>& interp = equation().probleme().domaine().get_remapper(o_ech_->equation().probleme().domaine())->getCrudeMatrix();

  /* derivees : aux mailles ou Ai > 0 */
  IntTab sten[2];
  for (sten[0].resize(0, 2), sten[1].resize(0, 2), e = 0; e < dom.nb_elem(); e++)
    if (Ai(e) > 0)
      {
        //partie locale
        for (n = 0; n < N; n++)
          for (m = 0; m < N; m++)
            sten[0].append_line(N * e + n, N * e + m);
        //partie distante
        for (auto &&kv : interp[e])
          if (kv.second)
            for (o_e = kv.first, n = 0; n < N; n++)
              for (m = 0; m < M; m++)
                sten[1].append_line(N * e + n, M * o_e + m);
      }
  for (i = 0; i < 2; i++)
    if (mat[i])
      {
        tableau_trier_retirer_doublons(sten[i]);
        Matrice_Morse mat2;
        Matrix_tools::allocate_morse_matrix(vals[0]->size_totale(), vals[i]->size_totale(), sten[i], mat2);
        mat[i]->nb_colonnes() ? *mat[i] += mat2 : *mat[i] = mat2;
      }
}

void Echange_Thermique_Volumique_Elem::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Probleme_base *pb[2] = { &equation().probleme(), &o_ech_->equation().probleme() };
  std::string nom_inc = equation().inconnue().le_nom().getString(), o_nom_inc = nom_inc + "/" + pb[1]->le_nom().getString();
  const DoubleTab& vals = semi_impl.count(nom_inc) ? semi_impl.at(nom_inc) : equation().inconnue().valeurs(),
                   &o_vals = semi_impl.count(o_nom_inc) ? semi_impl.at(o_nom_inc) : o_ech_->equation().inconnue().valeurs(), *pvals[2] = { &vals, &o_vals },
                       &lambda = equation().milieu().conductivite().passe(), &o_lambda = o_ech_->equation().milieu().conductivite().passe();
  const Domaine_VF *dom[2] = { &ref_cast(Domaine_VF, equation().domaine_dis()), &ref_cast(Domaine_VF, o_ech_->equation().domaine_dis()) };
  int i, e, o_e, ne_tot = dom[0]->nb_elem_tot(), o_ne_tot = dom[1]->nb_elem_tot(), n, m, N[2] = { vals.line_size(), o_vals.line_size() }, semi = semi_impl.count(nom_inc), d, D = dimension,
                                                                                                cL = lambda.dimension(0) == 1, o_cL = o_lambda.dimension(0) == 1;
  Matrice_Morse *mat = !semi && matrices.count(nom_inc) ? matrices.at(nom_inc) : nullptr, *o_mat = !semi && matrices.count(o_nom_inc) ? matrices.at(o_nom_inc) : nullptr;

  /* aire interfaciale par maille */
  DoubleTrav Ai(ne_tot, 1), ep, o_ep, cond, o_cond, o_Ai(o_ne_tot, 1), v_e[2];
  IntVect polys(ne_tot), o_polys(o_ne_tot);
  for (e = 0; e < ne_tot; e++) polys(e) = e;
  for (o_e = 0; o_e < o_ne_tot; o_e++) o_polys(o_e) = o_e;
  Ai_->valeur_aux_elems(dom[0]->xp(), polys, Ai), o_ech_->Ai_->valeur_aux_elems(dom[1]->xp(), o_polys, o_Ai);
  if (ep_cond_.non_nul())
    ep.resize(ne_tot, 1), ep_cond_->valeur_aux_elems(dom[0]->xp(), polys, ep);
  if (o_ech_->ep_cond_.non_nul())
    o_ep.resize(o_ne_tot, 1), o_ech_->ep_cond_->valeur_aux_elems(dom[1]->xp(), o_polys, o_ep);
  if (cond_.non_nul())
    cond.resize(ne_tot, 1), cond_->valeur_aux_elems(dom[0]->xp(), polys, cond);
  if (o_ech_->cond_.non_nul())
    o_cond.resize(o_ne_tot, 1), o_ech_->cond_->valeur_aux_elems(dom[1]->xp(), o_polys, o_cond);
  for (i = 0; i < 2; i++)
    if ((i ? &o_ech_->flux_par_ : &flux_par_)->non_nul())
      v_e[i].resize(i ? o_ne_tot : ne_tot, D * N[i]), ref_cast(Navier_Stokes_std, pb[i]->equation(0)).inconnue().valeur_aux_elems(dom[i]->xp(), i ? o_polys : polys, v_e[i]);

  /* matrice du remapper */
  const std::vector<std::map<mcIdType,double>>& interp = equation().probleme().domaine().get_remapper(o_ech_->equation().probleme().domaine())->getCrudeMatrix();
  bilan().resize(N[0]), bilan() = 0;

  for (e = 0; e < dom[0]->nb_elem(); e++)
    if (Ai(e) > 0)
      for (auto &&kv : interp[e])
        if (kv.second)
          {
            o_e = kv.first;
            // if (std::abs(Ai(e) - o_Ai(o_e)) > 1e-6)
            //   {
            //     Cerr << que_suis_je() + " : interfacial area inconsistency between " + dom[0]->domaine().le_nom() + "(" + Nom(Ai(e)) + ") and " + dom[1]->domaine().le_nom() + " (" + Nom(o_Ai(o_e)) + ")!";
            //     Cerr << "Positions : (" << dom[0]->xp(e, 0) << ", " << dom[0]->xp(e, 1) << ", " << dom[0]->xp(e, 2) << "), (" << dom[1]->xp(o_e, 0) << ", " << dom[1]->xp(o_e, 1) << ", " << dom[1]->xp(o_e, 2) << ")" << finl;
            //     Cerr << "Intersection : " << kv.second << finl;
            //     Process::exit();
            //   }
            //resistivite thermique totale : on commence par la partie conductrice
            double surf = std::min(Ai(e), o_Ai(o_e)) * kv.second, hf[2] = { 0, };
            double invh = (ep.size() ? ep(e) / (cond.size() ? cond(e) : lambda(!cL * e, 0)) : 0) + (o_ep.size() ? o_ep(o_e) / (o_cond.size() ? o_cond(o_e) : o_lambda(!o_cL * o_e, 0)) : 0);
            //flux parietaux de chaque cote
            DoubleTrav f_h(2, std::max(N[0], N[1])); // f_h(i, j) : fraction de l'echange avec la phase j du cote i
            for (i = 0; i < 2; i++)
              if ((i ? &o_ech_->flux_par_ : &flux_par_)->non_nul())
                {
                  const Flux_parietal_base& corr = ref_cast(Flux_parietal_base, (i ? &o_ech_->flux_par_ : &flux_par_)->valeur());
                  const DoubleTab* alpha = sub_type(Pb_Multiphase, *pb[i]) ? &ref_cast(Pb_Multiphase, *pb[i]).equation_masse().inconnue().passe() : nullptr, &dh = pb[i]->milieu().diametre_hydraulique_elem(),
                                   &press = ref_cast(Navier_Stokes_std, pb[i]->equation(0)).pression().passe(),
                                    &lamb = ref_cast(Fluide_base, pb[i]->milieu()).conductivite().passe(), &mu = ref_cast(Fluide_base, pb[i]->milieu()).viscosite_dynamique().passe(),
                                     &rho = pb[i]->milieu().masse_volumique().passe(), &Cp = pb[i]->milieu().capacite_calorifique().passe();
                  int Clamb = lamb.dimension(0) == 1, Cmu = mu.dimension(0) == 1, Crho = rho.dimension(0) == 1, Ccp = Cp.dimension(0) == 1, el = i ? o_e : e, nonlinear = 0;

                  Flux_parietal_base::input_t in;
                  Flux_parietal_base::output_t out;
                  DoubleTrav qpk(N[i]), dTf_qpk(N[i], N[i]), dTp_qpk(N[i]), qpi(N[i], N[i]), dTf_qpi(N[i], N[i], N[i]), dTp_qpi(N[i], N[i]), nv(N[i]);
                  in.N = N[i], in.D_h = dh(el), in.D_ch = dh(el), in.alpha = alpha ? &(*alpha)(el, 0) : nullptr, in.T = &(*pvals[i])(el, 0), in.p = press(el), in.v = nv.addr();
                  in.lambda = &lamb(!Clamb * el, 0), in.mu = &mu(!Cmu * el, 0), in.rho = &rho(!Crho * el, 0), in.Cp = &Cp(!Ccp * el, 0), in.Tp = 0;
                  out.qpk = &qpk, out.dTf_qpk = &dTf_qpk, out.dTp_qpk = &dTp_qpk, out.qpi = &qpi, out.dTf_qpi = &dTf_qpi, out.dTp_qpi = &dTp_qpi, out.nonlinear = &nonlinear;
                  for (d = 0; d < D; d++)
                    for (n = 0; n < N[i]; n++)
                      nv(n) += v_e[i](el, N[i] * d + n) * v_e[i](el, N[i] * d + n);
                  for (n = 0; n < N[0]; n++) nv(n) = sqrt(nv[n]);
                  //appel!
                  corr.qp(in, out);
                  if (nonlinear)
                    Process::exit(que_suis_je() + " : nonlinear heat flux such as " + corr.que_suis_je() + " are not implemented yet!");
                  //on n'est interesse que par les coeffs d'echange
                  for (n = 0; n < N[i]; n++) hf[i] += -dTf_qpk(n, n);
                  for (n = 0; n < N[i]; n++) f_h(i, n) = -dTf_qpk(n, n) / hf[i];
                  invh += 1. / hf[i];
                }
              else if (N[i] > 1)
                Process::exit(que_suis_je() + " : multi-component heat flux with " + pb[i]->le_nom() + ", but no heat_flux has been defined!");
              else f_h(i, 0) = 1;
            //contributions!
            for (n = 0; n < N[0]; n++)
              {
                for (m = 0; m < N[0]; m++) //locales
                  {
                    double fac = surf * f_h(0, n) * (hf[0] * (f_h(0, m) - (m == n)) - f_h(0, m) / invh);
                    secmem(e, n) += fac * vals(e, m), bilan()(n) += fac * vals(e, m);
                    if (mat) (*mat)(N[0] * e + n, N[0] * e + m) -= fac;
                  }
                for (m = 0; m < N[1]; m++) //distantes
                  {
                    double fac = surf / invh * f_h(0, n) * f_h(1, m);
                    secmem(e, n) += fac * o_vals(o_e, m), bilan()(n) += fac * o_vals(o_e, m);
                    if (o_mat) (*o_mat)(N[0] * e + n, N[1] * o_e + m) -= fac;
                  }
              }
          }
}
