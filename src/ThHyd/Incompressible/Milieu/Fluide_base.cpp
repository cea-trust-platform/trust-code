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

#include <Fluide_Incompressible.h>
#include <Discretisation_base.h>
#include <Champ_Fonc_Tabule.h>
#include <Schema_Temps_base.h>
#include <Navier_Stokes_std.h>
#include <Champ_Fonc_MED.h>
#include <Champ_Uniforme.h>
#include <Champ_Inc_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Fluide_base.h>
#include <EChaine.h>
#include <Motcle.h>
#include <Param.h>


Implemente_instanciable(Fluide_base, "Fluide_base", Milieu_base);
// XD fluide_base milieu_base fluide_base -3 Basic class for fluids.
// XD attr indice field_base indice 1 Refractivity of fluid.
// XD attr kappa field_base kappa 1 Absorptivity of fluid (m-1).

Sortie& Fluide_base::printOn(Sortie& os) const
{
  os << "{" << finl;
  os << "kappa " << coeff_absorption_ << finl;
  os << "indice " << indice_refraction_ << finl;
  os << "longueur_rayo " << longueur_rayo_ << finl;
  os << "mu " << ch_mu_ << finl;
  os << "beta_co " << ch_beta_co_ << finl;
  Milieu_base::ecrire(os);
  os << "}" << finl;
  return os;
}

/*! @brief Lit les caracteristiques du fluide a partir d'un flot d'entree.
 *
 *    Format:
 *      Fluide_base
 *      {
 *       Mu type_champ bloc de lecture de champ
 *       Rho Champ_Uniforme 1 vrel
 *       [Cp Champ_Uniforme 1 vrel]
 *       [Lambda type_champ bloc de lecture de champ]
 *       [Beta_th type_champ bloc de lecture de champ]
 *       [Beta_co type_champ bloc de lecture de champ]
 *      }
 *  cf Milieu_base::readOn
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 */
Entree& Fluide_base::readOn(Entree& is)
{
  Milieu_base::readOn(is);
  if (ch_mu_.non_nul()) champs_don_.add(ch_mu_.valeur());
  if (ch_nu_.non_nul()) champs_don_.add(ch_nu_.valeur());
  if (ch_beta_co_.non_nul()) champs_don_.add(ch_beta_co_.valeur());
  if (coeff_absorption_.non_nul()) champs_don_.add(coeff_absorption_.valeur());
  if (indice_refraction_.non_nul()) champs_don_.add(indice_refraction_.valeur());
  if (longueur_rayo_.non_nul()) champs_don_.add(longueur_rayo_.valeur());
  return is;
}

void Fluide_base::set_param(Param& param)
{
  Milieu_base::set_param(param);
  //La lecture de rho n est pas specifiee obligatoire ici car ce champ ne doit pas etre lu pour un fluide dilatable
  param.ajouter("mu", &ch_mu_, Param::REQUIRED);
  param.ajouter("beta_co", &ch_beta_co_);
  param.ajouter("kappa", &coeff_absorption_);
  param.ajouter("indice", &indice_refraction_);
}

void Fluide_base::creer_champs_non_lus()
{
  Milieu_base::creer_champs_non_lus();
  creer_nu();
}

void Fluide_base::discretiser(const Probleme_base& pb, const Discretisation_base& dis)
{
  const Domaine_dis_base& domaine_dis = pb.equation(0).domaine_dis();
  // mu rho nu  revoir
  double temps = pb.schema_temps().temps_courant();
  if (ch_mu_.non_nul())
    if (sub_type(Champ_Fonc_MED, ch_mu_.valeur()))
      {
        Cerr << " on convertit le champ_fonc_med en champ_don" << finl;
        OWN_PTR(Champ_Don_base) mu_prov;
        dis.discretiser_champ("champ_elem", domaine_dis, "neant", "neant", 1, temps, mu_prov);
        mu_prov->affecter(ch_mu_.valeur());
        ch_mu_.detach();
        ch_nu_.detach();
        dis.discretiser_champ("champ_elem", domaine_dis, "neant", "neant", 1, temps, ch_mu_);

        ch_mu_->valeurs() = mu_prov->valeurs();

      }
  if (ch_mu_.est_nul())
    {
      dis.discretiser_champ("champ_elem", domaine_dis, "neant", "neant", 1, temps, ch_mu_);
      dis.discretiser_champ("champ_elem", domaine_dis, "neant", "neant", 1, temps, ch_nu_);
    }
  if (ch_mu_.non_nul())
    {
      dis.nommer_completer_champ_physique(domaine_dis, "viscosite_dynamique", "kg/m/s", ch_mu_.valeur(), pb);
      champs_compris_.ajoute_champ(ch_mu_.valeur());
    }
  if (sub_type(Champ_Fonc_Tabule, ch_mu_.valeur()))
    {
      dis.discretiser_champ("champ_elem", domaine_dis, "neant", "neant", 1, temps, ch_nu_);
    }
  if (ch_nu_.est_nul())
    dis.discretiser_champ("champ_elem", domaine_dis, "neant", "neant", 1, temps, ch_nu_);

  if (ch_nu_.non_nul())
    {
      dis.nommer_completer_champ_physique(domaine_dis, "viscosite_cinematique", "m2/s", ch_nu_.valeur(), pb);
      champs_compris_.ajoute_champ(ch_nu_.valeur());
    }
  if (ch_beta_co_.non_nul())
    {
      dis.nommer_completer_champ_physique(domaine_dis, "dilatabilite_solutale", ".", ch_beta_co_.valeur(), pb);
      champs_compris_.ajoute_champ(ch_beta_co_.valeur());
    }

  Milieu_base::discretiser(pb, dis);
}
/*! @brief Verifie que les champs lus l'ont ete correctement.
 *
 * @throws la masse volumique (rho) n'est pas strictement positive
 * @throws la masse volumique (rho) n'est pas de type Champ_Uniforme
 * @throws la viscosite (mu) n'est pas strictement positive
 * @throws l'une des proprietes (rho ou mu) du fluide n'a pas ete definie
 * @throws la capacite calorifique (Cp) n'est pas strictement positive
 * @throws la capacite calorifique (Cp) n'est pas de type Champ_Uniforme
 * @throws la conductivite (lambda) n'est pas strictement positive
 * @throws toutes les proprietes du fluide anisotherme n'ont pas ete definies
 */
void Fluide_base::verifier_coherence_champs(int& err, Nom& msg)
{
  msg = "";
  if (ch_rho_.non_nul())
    {
      if (mp_min_vect(ch_rho_->valeurs()) <= 0)
        {
          msg += "The density rho is not striclty positive. \n";
          err = 1;
        }
    }
  else
    {
      msg += "The density rho has not been specified. \n";
      err = 1;
    }
  if ((ch_Cp_.non_nul()) && ((ch_lambda_.non_nul()) && (ch_beta_th_.non_nul()))) // Fluide anisotherme
    {
      if (sub_type(Champ_Uniforme, ch_Cp_.valeur()))
        {
          if (ch_Cp_->valeurs()(0, 0) <= 0)
            {
              msg += "The heat capacity Cp is not striclty positive. \n";
              err = 1;
            }
        }
      else
        {
          msg += "The heat capacity Cp is not of type Champ_Uniforme. \n";
          err = 1;
        }
      if (sub_type(Champ_Uniforme, ch_lambda_.valeur()))
        {
          if (ch_lambda_->valeurs()(0, 0) < 0)
            {
              msg += "The conductivity lambda is not positive. \n";
              err = 1;
            }
        }

    }
  if (((ch_Cp_.non_nul()) || (ch_beta_th_.non_nul())) && (ch_lambda_.est_nul()))
    {
      msg += " Physical properties for an anisotherm case : \n";
      msg += "the conductivity lambda has not been specified. \n";
      if (err == 0) err = 2; // if err=1 we keep it since err=1 exits while err=2 displays warning!
    }
  if (((ch_lambda_.non_nul()) || (ch_beta_th_.non_nul())) && (ch_Cp_.est_nul()))
    {
      msg += " Physical properties for an anisotherm case : \n";
      msg += "the heat capacity Cp has not been specified. \n";
      if (err == 0) err = 2;
    }
  if (((ch_lambda_.non_nul()) || (ch_Cp_.non_nul())) && (ch_beta_th_.est_nul()))
    {
      msg += " Physical properties for an anisotherm case : \n";
      msg += "the thermal expansion coefficient beta_th has not been specified. \n";
      if (err == 0) err = 2;
    }

  // Test de la coherence des proprietees radiatives du fluide incompressible
  // (pour un milieu semi transparent
  if ((coeff_absorption_.non_nul()) && (indice_refraction_.est_nul()))
    {
      msg += " Physical properties for semi tranparent radiation case : \n";
      msg += "Refraction index has not been specfied while it has been done for absorption coefficient. \n";
      err = 1;
    }
  if ((coeff_absorption_.est_nul()) && (indice_refraction_.non_nul()))
    {
      msg += " Physical properties for semi tranparent radiation case : \n";
      msg += "Absorption coefficient has not been specfied while it has been done for refraction index. \n";
      err = 1;
    }

  if ((coeff_absorption_.non_nul()) && indice_refraction_.non_nul())
    {
      if (sub_type(Champ_Uniforme, coeff_absorption_.valeur()))
        {
          if (coeff_absorption_->valeurs()(0, 0) <= 0)
            {
              msg += "The absorption coefficient kappa is not striclty positive. \n";
              err = 1;
            }
        }
    }
  Milieu_base::verifier_coherence_champs(err, msg);
}

/*! @brief Si l'objet reference par nu et du type Champ_Uniforme type nu en "Champ_Uniforme" et le remplit
 *
 *      avec mu(0,0)/rho(0,0).
 *     Sinon n efait rien.
 *
 */
void Fluide_base::creer_nu()
{
  assert(ch_mu_.non_nul());
  assert(ch_rho_.non_nul());
  ch_nu_ = ch_mu_;
  if (sub_type(Champ_Uniforme, ch_mu_.valeur()) && !sub_type(Champ_Uniforme, ch_rho_.valeur()))
    ch_nu_->valeurs().resize(ch_rho_->valeurs().dimension_tot(0), ch_rho_->valeurs().line_size());
  ch_nu_->nommer("nu");
}

void Fluide_base::calculer_nu()
{
  const DoubleTab& tabmu = ch_mu_->valeurs();
  const DoubleTab& tabrho = ch_rho_->valeurs();
  DoubleTab& tabnu = ch_nu_->valeurs();

  int cRho = sub_type(Champ_Uniforme, ch_rho_.valeur()),
      cMu = sub_type(Champ_Uniforme, ch_mu_.valeur());
  int i, j, n;
  int Nl = tabnu.dimension_tot(0), N = tabnu.line_size();

  /* valeurs : mu / rho */
  for (i = j = 0; i < Nl; i++)
    for (n = 0; n < N; n++, j++)
      tabnu.addr()[j] = tabmu.addr()[cMu ? n : j] / tabrho.addr()[cRho ? n : j];
}

bool Fluide_base::initTimeStep(double dt)
{
  if (!equation_.size() || !e_int_auto_)
    return true; //pas d'equation associee ou pas de e_int a gerer
  const Schema_Temps_base& sch = equation_.begin()->second->schema_temps(); //on recupere le schema en temps par la 1ere equation
  Champ_Inc_base& ch = ref_cast(Champ_Inc_base, ch_e_int_.valeur());
  for (int i = 1; i <= sch.nb_valeurs_futures(); i++)
    ch.changer_temps_futur(sch.temps_futur(i), i), ch.futur(i) = ch.valeurs();
  return true;
}

/*! @brief Effectue une mise a jour en temps du milieu, et donc de ses parametres caracteristiques.
 *
 *     Les champs uniformes sont recalcules pour le
 *     nouveau temps specifie, les autres sont mis a
 *     par un appel a CLASSE_DU_CHAMP::mettre_a_jour(double temps).
 *
 * @param (double temps) le temps de mise a jour
 */
void Fluide_base::mettre_a_jour(double temps)
{
  Milieu_base::mettre_a_jour(temps);
  if (ch_beta_co_.non_nul())
    ch_beta_co_->mettre_a_jour(temps);
  ch_mu_->mettre_a_jour(temps);
  calculer_nu();
  ch_nu_->changer_temps(temps);
  ch_nu_->valeurs().echange_espace_virtuel();
  if (e_int_auto_)
    ch_e_int_->mettre_a_jour(temps);

  // Mise a jour des proprietes radiatives du fluide incompressible
  // (Pour un fluide incompressible semi transparent).
  if (coeff_absorption_.non_nul() && indice_refraction_.non_nul())
    {
      coeff_absorption_->mettre_a_jour(temps);
      indice_refraction_->mettre_a_jour(temps);

      // Mise a jour de longueur_rayo
      longueur_rayo_->mettre_a_jour(temps);

      if (sub_type(Champ_Uniforme, kappa()))
        {
          longueur_rayo().valeurs()(0, 0) = 1 / (3 * kappa().valeurs()(0, 0));
          longueur_rayo().valeurs().echange_espace_virtuel();
        }
      else
        {
          DoubleTab& l_rayo = longueur_rayo_->valeurs();
          const DoubleTab& K = kappa().valeurs();
          for (int i = 0; i < kappa().nb_valeurs_nodales(); i++)
            l_rayo[i] = 1 / (3 * K[i]);
          l_rayo.echange_espace_virtuel();
        }
    }
}

/*! @brief Initialise les parametres du fluide.
 *
 */
int Fluide_base::initialiser(const double temps)
{
  Cerr << "Fluide_base::initialiser()" << finl;
  Milieu_base::initialiser(temps);
  ch_mu_->initialiser(temps);

  if (ch_beta_co_.non_nul())
    ch_beta_co_->initialiser(temps);

  calculer_nu();
  ch_nu_->valeurs().echange_espace_virtuel();
  ch_nu_->changer_temps(temps);

  // Initialisation des proprietes radiatives du fluide incompressible
  // (Pour un fluide incompressible semi transparent).
  if (coeff_absorption_.non_nul() && indice_refraction_.non_nul())
    {
      Cerr << "Semi transparent fluid properties initialization." << finl;
      coeff_absorption_->initialiser(temps);
      indice_refraction_->initialiser(temps);

      // Initialisation de longueur_rayo
      longueur_rayo_->initialiser(temps);
      if (sub_type(Champ_Uniforme, kappa()))
        longueur_rayo().valeurs()(0, 0) = 1 / (3 * kappa().valeurs()(0, 0));
      else
        {
          DoubleTab& l_rayo = longueur_rayo_->valeurs();
          const DoubleTab& K = kappa().valeurs();
          for (int i = 0; i < kappa().nb_valeurs_nodales(); i++)
            l_rayo[i] = 1 / (3 * K[i]);
        }
    }
  return 1;
}

int Fluide_base::is_rayo_semi_transp() const
{
  return int(indic_rayo_ == SEMITRANSP);
}

int Fluide_base::is_rayo_transp() const
{
  return int(indic_rayo_ == TRANSP);
}

void Fluide_base::reset_type_rayo()
{
  indic_rayo_ = NONRAYO;
}

void Fluide_base::fixer_type_rayo()
{
  if ((coeff_absorption_.non_nul()) && (indice_refraction_.non_nul()))
    indic_rayo_ = SEMITRANSP;
  else
    indic_rayo_ = TRANSP;
}

int Fluide_base::longueur_rayo_is_discretised()
{
  return longueur_rayo_.non_nul();
}

void Fluide_base::set_h0_T0(double h0, double T0)
{
  T0_ = T0;
  h0_ = h0;
}

void Fluide_base::creer_e_int() const
{
  OWN_PTR(Champ_Inc_base) e_int_inc;
  const Equation_base& eq = equation_.count("temperature") ? equation("temperature") : equation("enthalpie");
  eq.discretisation().discretiser_champ("champ_elem", eq.domaine_dis(), "energie_interne", "J/kg", 1, eq.inconnue().nb_valeurs_temporelles(), eq.inconnue().temps(), e_int_inc);
  e_int_inc->associer_eqn(eq), e_int_inc->init_champ_calcule(*this, calculer_e_int);
  ch_e_int_ = e_int_inc;
  ch_e_int_->mettre_a_jour(eq.inconnue().temps());
  e_int_auto_ = 1;
}

void Fluide_base::calculer_temperature_multiphase() const
{
  const bool res_en_T = equation_.count("temperature") ? true : false;
  if (res_en_T) return; /* Do nothing */

  const Equation_base& eq = equation("enthalpie");
  const Champ_base& ch_h = eq.inconnue(), &ch_Cp = capacite_calorifique();
  Champ_Inc_base& ch_T = ref_cast_non_const(Champ_Inc_base, ch_h_ou_T_.valeur());
  const DoubleTab& h = ch_h.valeurs(), &Cp_ = ch_Cp.valeurs();
  DoubleTab& T = ch_T.valeurs(), &bT = ch_T.val_bord();;

  DoubleTab bh = ch_h.valeur_aux_bords(), bCp;

  int i, zero = 0, Ni = h.dimension_tot(0), Nb = bh.dimension_tot(0), n, n0 = std::max(id_composite_, zero), cCp = Cp_.dimension_tot(0) == 1, N = id_composite_ >= 0 ? 1 : Cp_.dimension(1);

  // T = T0 + (h - h0) / cp
  for (i = 0; i < Ni; i++)
    for (n = 0; n < N; n++)
      T(i, n) = T0_ + (( h(i, n0 + n) - h0_) / Cp_(!cCp * i, n));

  if (ch_Cp.a_un_domaine_dis_base())
    bCp = ch_Cp.valeur_aux_bords();
  else
    {
      bCp.resize(bh.dimension_tot(0), N);
      ch_Cp.valeur_aux(ref_cast(Domaine_VF, ch_h.domaine_dis_base()).xv_bord(), bCp);
    }

  for (i = 0; i < Nb; i++)
    for (n = 0; n < N; n++)
      bT(i, n) = T0_ + (( bh(i, n0 + n) - h0_) / bCp(!cCp * i, n));
}

void Fluide_base::creer_temperature_multiphase() const
{
  const bool res_en_T = equation_.count("temperature") ? true : false;
  if (res_en_T) return; /* Do nothing */

  const Equation_base& eq = equation("enthalpie");
  if (ch_e_int_.est_nul()) creer_e_int();
  ch_h_ou_T_ = ch_e_int_; // on initialise
  ch_h_ou_T_->nommer("temperature");
  ch_h_ou_T_->mettre_a_jour(eq.inconnue().temps());
}

void Fluide_base::calculer_e_int(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Fluide_base& fl = ref_cast(Fluide_base, obj);
  const bool res_en_T = fl.equation_.count("temperature") ? true : false;

  const Equation_base& eq = res_en_T ? fl.equation("temperature") : fl.equation("enthalpie");
  const Champ_base& ch_T_ou_h = eq.inconnue(), &ch_Cp = fl.capacite_calorifique();
  const DoubleTab& T_ou_h = ch_T_ou_h.valeurs(), &Cp = ch_Cp.valeurs();
  int i, zero = 0, Ni = val.dimension_tot(0), Nb = bval.dimension_tot(0), n, n0 = std::max(fl.id_composite_, zero), cCp = Cp.dimension_tot(0) == 1, N = fl.id_composite_ >= 0 ? 1 : Cp.dimension(1);

  DoubleTab bT_ou_h = ch_T_ou_h.valeur_aux_bords();

  if (res_en_T)
    {
      // e = h0 + Cp * (T - T0)
      for (i = 0; i < Ni; i++)
        for (n = 0; n < N; n++)
          val(i, n) = fl.h0_ + Cp(!cCp * i, n) * (T_ou_h(i, n0 + n) - fl.T0_);

      DoubleTab bCp;

      if (ch_Cp.a_un_domaine_dis_base())
        bCp = ch_Cp.valeur_aux_bords();
      else
        {
          bCp.resize(bval.dimension_tot(0), N);
          ch_Cp.valeur_aux(ref_cast(Domaine_VF, ch_T_ou_h.domaine_dis_base()).xv_bord(), bCp);
        }

      for (i = 0; i < Nb; i++)
        for (n = 0; n < N; n++)
          bval(i, n) = fl.h0_ + bCp(i, n) * (bT_ou_h(i, n0 + n) - fl.T0_);

      DoubleTab& der_T = deriv[ch_T_ou_h.le_nom().getString()];
      for (der_T.resize(Ni, N), i = 0; i < Ni; i++)
        for (n = 0; n < N; n++)
          der_T(i, n) = Cp(!cCp * i, n);
    }
  else
    {
      // e = h - P / rho
      for (i = 0; i < Ni; i++)
        for (n = 0; n < N; n++)
          val(i, n) = T_ou_h(i, n0 + n);

      for (i = 0; i < Nb; i++)
        for (n = 0; n < N; n++)
          bval(i, n) = bT_ou_h(i, n0 + n);

      DoubleTab& der_T = deriv["enthalpie"];
      for (der_T.resize(Ni, N), i = 0; i < Ni; i++)
        for (n = 0; n < N; n++)
          der_T(i, n) = 1.;
    }
}

const Champ_base& Fluide_base::energie_interne() const
{
  if (ch_e_int_.est_nul()) creer_e_int();
  return ch_e_int_;
}

Champ_base& Fluide_base::energie_interne()
{
  if (ch_e_int_.est_nul()) creer_e_int();
  return ch_e_int_;
}

const Champ_base& Fluide_base::enthalpie() const
{
  if (ch_h_ou_T_.est_nul() && ch_e_int_.est_nul()) creer_e_int();
  return ch_h_ou_T_.non_nul() ? ch_h_ou_T_ : ch_e_int_;
}

Champ_base& Fluide_base::enthalpie()
{
  if (ch_h_ou_T_.est_nul() && ch_e_int_.est_nul()) creer_e_int();
  return ch_h_ou_T_.non_nul() ? ch_h_ou_T_ : ch_e_int_;
}

const Champ_base& Fluide_base::temperature_multiphase() const
{
  if (ch_h_ou_T_.est_nul())
    creer_temperature_multiphase();

  if (sub_type(Fluide_Incompressible, *this))
    calculer_temperature_multiphase(); /* sinon rempli dans Fluide_reel_base */

  return ch_h_ou_T_;
}

Champ_base& Fluide_base::temperature_multiphase()
{
  if (ch_h_ou_T_.est_nul())
    creer_temperature_multiphase();

  if (sub_type(Fluide_Incompressible, *this))
    calculer_temperature_multiphase(); /* sinon rempli dans Fluide_reel_base */

  return ch_h_ou_T_;
}
