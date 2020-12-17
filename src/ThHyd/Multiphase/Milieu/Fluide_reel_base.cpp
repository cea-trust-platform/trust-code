/****************************************************************************
* Copyright (c) 2020, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Fluide_reel_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_reel_base.h>
#include <Equation_base.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <EChaine.h>
#include <Zone_VF.h>
#include <Pb_Multiphase.h>

Implemente_base(Fluide_reel_base, "Fluide_reel_base", Fluide_base);

Entree& Fluide_reel_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  if (isotherme_)
    {
      if (T_ref_ == -1 || P_ref_ == -1) Process::exit("Fluide_reel : set T_ref and P_ref if you ant to use isotherme option.");
      else
        {
          Nom n_ch("champ_uniforme 1 "), n_rho(n_ch), n_cp(n_ch), n_beta(n_ch), n_lambda(n_ch), n_mu(n_ch);
          n_rho += Nom(rho_(T_ref_, P_ref_));
          n_cp += Nom(cp_(T_ref_, P_ref_));
          n_beta += Nom(beta_(T_ref_));
          n_lambda += Nom(lambda_(T_ref_));
          n_mu += Nom(mu_(T_ref_));
          EChaine ch_rho(n_rho), ch_cp(n_cp), ch_beta(n_beta), ch_lambda(n_rho), ch_mu(n_cp);
          ch_rho >> rho, ch_cp >> Cp, ch_beta >> beta_th, ch_lambda >> lambda, ch_mu >> mu;
        }
    }
  return is;
}

void Fluide_reel_base::set_param(Param& param)
{
  param.ajouter("isotherme",&isotherme_);
  param.ajouter("T_ref",&T_ref_);
  param.ajouter("P_ref",&P_ref_);
}

Sortie& Fluide_reel_base::printOn(Sortie& os) const
{
  return os;
}

void Fluide_reel_base::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  if (isotherme_)
    {
      Fluide_base::discretiser(pb, dis);
      return;
    }
  Cerr << "Medium discretization." << finl;

  const Zone_dis_base& zone_dis = pb.equation(0).zone_dis();
  const double temps = pb.schema_temps().temps_courant();

  Champ_Fonc rho_fonc;
  dis.discretiser_champ("champ_elem", zone_dis,       "masse_volumique", "kg/m^3", 1, temps, rho_fonc);
  rho = rho_fonc;
  dis.discretiser_champ("champ_elem", zone_dis,   "viscosite_dynamique", "kg/m/s", 1, temps,       mu);
  dis.discretiser_champ("champ_elem", zone_dis, "viscosite_cinematique",   "m2/s", 1, temps,       nu);
  dis.discretiser_champ("champ_elem", zone_dis,           "diffusivite",   "m2/s", 1, temps,    alpha);
  dis.discretiser_champ("champ_elem", zone_dis,          "conductivite",  "W/m/K", 1, temps,   lambda);
  dis.discretiser_champ("champ_elem", zone_dis,  "capacite_calorifique", "J/kg/K", 1, temps,       Cp);
}

int Fluide_reel_base::initialiser(const double& temps)
{
  if (isotherme_) return Fluide_base::initialiser(temps);

  if (sub_type(Champ_Don_base, rho.valeur())) ref_cast(Champ_Don_base, rho.valeur()).initialiser(temps);
  else rho.mettre_a_jour(temps);
  Cp.initialiser(temps);
  mu.initialiser(temps);
  nu.initialiser(temps);
  alpha.initialiser(temps);
  lambda.initialiser(temps);
  mettre_a_jour_tabs(temps);
  return 1;
}

void Fluide_reel_base::mettre_a_jour(double temps)
{
  if (isotherme_) return Fluide_base::mettre_a_jour(temps);
  Milieu_base::mettre_a_jour(temps);
  mettre_a_jour_tabs(temps);
}

void Fluide_reel_base::mettre_a_jour_tabs(const double t)
{
  if (isotherme_) Process::exit("should not be there!");

  const Equation_base& eqn = *equation.at("temperature");
  const Champ_base& ch_T = eqn.probleme().get_champ("Temperature");
  const Champ_base& ch_P = eqn.probleme().get_champ("Pression");
  const DoubleTab& temp = ch_T.valeurs(t);
  const DoubleTab& pres = ch_P.valeurs(t);
  const int Nl = mu.valeurs().dimension_tot(0);

  DoubleTab& tab_rho = rho.valeurs();
  DoubleTab& tab_Cp = Cp.valeurs();
  DoubleTab& tab_mu = mu.valeurs();
  DoubleTab& tab_nu = nu.valeurs();
  DoubleTab& tab_alpha = alpha.valeurs();
  DoubleTab& tab_lambda = lambda.valeurs();
  for (int i = 0; i < Nl; i++)
    {
      const double T = temp(i, id_composite), P = pres(i);
      tab_rho(i) = rho_(T, P);
      tab_Cp(i) = cp_(T, P);
      tab_mu(i) = mu_(T);
      tab_lambda(i) = lambda_(T);
      tab_nu(i) = tab_mu(i) / tab_rho(i);
      tab_alpha(i) = tab_lambda(i) / tab_rho(i) / tab_Cp(i);
    }
}

// void Fluide_reel_base::calculer_masse_volumique(const Champ_Inc_base& ch, double t, DoubleTab& val, DoubleTab& bval, tabs_t& deriv, int val_only)
// {
//   const Champ_base& ch_T = ch.equation().probleme().get_champ("Temperature");
//   const Champ_base& ch_P = ch.equation().probleme().get_champ("Pression");
//   const DoubleTab& temp = ch_T.valeurs(t);
//   const DoubleTab& pres = ch_P.valeurs(t);
//   const Fluide_reel_base& mil = ref_cast(Fluide_reel_base, ch.equation().milieu());
//   const int Nl = temp.dimension_tot(0), N = temp.line_size();
//   const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, ch.equation().probleme());

//   for (int i = 0; i < Nl; i++) val(i) = mil.rho(temp(i, id_composite), pres(i));
//   // if (val_only) return;

//   // bval = ch_T.valeur_aux_bords();
//   // DoubleTab b_P;
//   // b_P.resize(bval.dimension_tot(0));
//   // b_P = ch_P.valeur_aux(ref_cast(Zone_VF, ch.equation().zone_dis().valeur()).xv_bord(), b_P);
//   // for (i = j = 0; i < bval.dimension_tot(0); i++) for (n = 0; n < N; n++, j++)
//   //     {
//   //       if (pb.nom_phase(n).debute_par("liq"))
//   //         bval.addr()[j] = mil.rho_l(bval.addr()[j], b_P.addr()[i]);
//   //       else if (pb.nom_phase(n).debute_par("gaz"))
//   //         bval.addr()[j] = mil.rho_v(bval.addr()[j], b_P.addr()[i]);
//   //       else Process::exit("Nom phase doit commencer par liq ou gaz");
//   //     }

//   // /* derivees : Cp */
//   // DoubleTab& derT = deriv["temperature"];
//   // DoubleTab& derP = deriv["pression"];
//   // for (derT.resize(Nl, N), derP.resize(Nl, N), i = j = 0; i < Nl; i++) for (n = 0; n < N; n++, j++)
//   //     {
//   //       derT.addr()[j] = mil.dT_rho_l(temp.addr()[j], pres.addr()[i]);
//   //       derP.addr()[j] = mil.dT_rho_l(temp.addr()[j], pres.addr()[i]);
//   //     }
// }

// void Fluide_reel_base::creer_energie_interne() const
// {
//   const Equation_base& eqn = *equation.at("temperature");
//   eqn.discretisation().discretiser_champ("temperature", eqn.zone_dis(),"energie_interne", "J/m^3",
//                                          eqn.inconnue()->nb_comp(),
//                                          eqn.inconnue()->nb_valeurs_temporelles(),
//                                          eqn.schema_temps().temps_courant(), e_int);
//   e_int->associer_eqn(eqn);
//   e_int->init_champ_calcule(calculer_energie_interne_reel);
// }

// void Fluide_reel_base::calculer_energie_interne_reel(const Champ_Inc_base& ch, double t, DoubleTab& val, DoubleTab& bval, tabs_t& deriv, int val_only)
// {
//   const Champ_base& ch_T = ch.equation().probleme().get_champ("Temperature");
//   const Champ_base& ch_P = ch.equation().probleme().get_champ("Pression");
//   const DoubleTab& temp = ch_T.valeurs(t);
//   const DoubleTab& pres = ch_P.valeurs(t);
//   const Fluide_reel_base& mil = ref_cast(Fluide_reel_base, ch.equation().milieu());
//   const int Nl = temp.dimension_tot(0), N = temp.line_size();
//   const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, ch.equation().probleme());
//   int i, j, n;

//   for (i = j = 0; i < Nl; i++) for (n = 0; n < N; n++, j++)
//       if (pb.nom_phase(n).debute_par("liq"))
//         val.addr()[j] = mil.ei_l(temp.addr()[j], pres.addr()[i]);
//       else if (pb.nom_phase(n).debute_par("gaz"))
//         val.addr()[j] = mil.ei_v(temp.addr()[j], pres.addr()[i]);
//       else Process::exit("Nom phase doit commencer par liq ou gaz");
//   if (val_only) return;

//   bval = ch_T.valeur_aux_bords();
//   DoubleTab b_P;
//   b_P.resize(bval.dimension_tot(0));
//   b_P = ch_P.valeur_aux(ref_cast(Zone_VF, ch.equation().zone_dis().valeur()).xv_bord(), b_P);
//   for (i = j = 0; i < bval.dimension_tot(0); i++) for (n = 0; n < N; n++, j++)
//       bval.addr()[j] = mil.ei_l(bval.addr()[j], b_P.addr()[i]);

//   /* derivees */
//   DoubleTab& derT = deriv["temperature"];
//   // DoubleTab& derP = deriv["pression"];
//   // for (derT.resize(Nl, N), derP.resize(Nl, N), i = j = 0; i < Nl; i++) for (n = 0; n < N; n++, j++)
//   for (derT.resize(Nl, N), i = j = 0; i < Nl; i++) for (n = 0; n < N; n++, j++)
//       {
//         derT.addr()[j] = mil.dT_ei_l(temp.addr()[j], pres.addr()[i]);
//         // derP.addr()[j] = mil.dP_ei_l(temp.addr()[j], pres.addr()[i]);
//       }
// }
