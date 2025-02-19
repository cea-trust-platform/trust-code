/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <EcritureLectureSpecial.h>
#include <Scalaire_impose_paroi.h>
#include <Echange_global_impose.h>
#include <Op_Conv_negligeable.h>
#include <Energie_Multiphase.h>
#include <Pb_Multiphase_HEM.h>
#include <TRUSTTab_parts.h>
#include <Champ_Uniforme.h>
#include <Matrice_Morse.h>
#include <Neumann_paroi.h>
#include <Discret_Thyd.h>
#include <Domaine_VF.h>
#include <TRUSTTrav.h>
#include <EChaine.h>
#include <Domaine.h>
#include <Param.h>
#include <SETS.h>

Implemente_instanciable(Energie_Multiphase, "Energie_Multiphase", Convection_Diffusion_Temperature_base);
// XD Energie_Multiphase eqn_base Energie_Multiphase -1 Internal energy conservation equation for a multi-phase problem where the unknown is the temperature

Sortie& Energie_Multiphase::printOn(Sortie& is) const { return Convection_Diffusion_Temperature_base::printOn(is); }

Entree& Energie_Multiphase::readOn(Entree& is)
{
  assert(l_inco_ch_.non_nul());
  assert(le_fluide.non_nul());
  evanescence_.associer_eqn(*this);
  Convection_Diffusion_Temperature_base::readOn(is);

  terme_convectif.set_fichier("Convection_chaleur");
  terme_convectif.set_description((Nom)"Convective heat transfer rate=Integral(-h*u*ndS) [W] if SI units used");
  terme_diffusif.set_fichier("Diffusion_chaleur");
  //terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) "+unite);
  terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) [W] if SI units used");

  // Special treatment for Pb_Multiphase_HEM
  // We enforce the presence of a source term related to the interfacial flux automatically.
  if (sub_type(Pb_Multiphase_HEM, probleme()))
    {
      int check_source_FICC(0);
      for (int ii = 0; ii < sources().size(); ii++)
        if (sources()(ii)->que_suis_je().debute_par("Flux_interfacial"))
          check_source_FICC = 1;
      if (check_source_FICC == 0)
        {
          EChaine source_FI("{ flux_interfacial }");
          lire_sources(source_FI);
        }
    }

  return is;
}

void Energie_Multiphase::set_param(Param& param)
{
  Convection_Diffusion_Temperature_base::set_param(param);
  param.ajouter_non_std("evanescence|vanishing",(this));
}

int Energie_Multiphase::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="evanescence") is >> evanescence_;
  else return Convection_Diffusion_Temperature_base::lire_motcle_non_standard(mot, is);
  return 1;
}

/*! @brief Discretise l'equation.
 *
 */
void Energie_Multiphase::discretiser()
{
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  Cerr << "Energy equation discretization " << finl;
  const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, probleme());
  dis.temperature(schema_temps(), domaine_dis(), l_inco_ch_, pb.nb_phases());
  l_inco_ch_->fixer_nature_du_champ(pb.nb_phases() == 1 ? scalaire : pb.nb_phases() == dimension ? vectoriel : multi_scalaire); //pfft
  for (int i = 0; i < pb.nb_phases(); i++)
    l_inco_ch_->fixer_nom_compo(i, Nom("temperature_") + pb.nom_phase(i));
  champs_compris_.ajoute_champ(l_inco_ch_);
  Equation_base::discretiser();
  Cerr << "Energie_Multiphase::discretiser() ok" << finl;
}

/*! @brief Impression des flux sur les bords sur un flot de sortie.
 *
 * Appelle Equation_base::impr(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (int) code de retour propage
 */
int Energie_Multiphase::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}

/*! @brief Verification du nombre de composantes lues pour la specification d un champ.
 *
 * Dans Energie_Multiphase, les conditions aux limites de type "paroi" ne prennent
 *  qu'une compposante lorsqu'une correlation "flux_parietal" est definie au niveau du probleme.
 *
 * @param (ch_ref  : un champ inconnu de l equation consideree)
 */
void Energie_Multiphase::verifie_ch_init_nb_comp_cl(const Champ_Inc_base& ch_ref, const int nb_comp, const Cond_lim_base& cl) const
{
  //si on verifie une CL de type
  if (probleme().has_correlation("flux_parietal")
      && (sub_type(Neumann_paroi, cl) || sub_type(Scalaire_impose_paroi, cl) || sub_type(Echange_global_impose, cl)))
    {
      if (nb_comp == 1) return; //OK
      Cerr << "Energie_Multiphase : when using a Flux_parietal correlation, only one wall temperature/heat flux "
           << "can be specified at the boundary " << cl.le_nom() << " . Please provide 1 component instead of " << nb_comp << "!" << finl;
      Process::exit();
    }
  else Convection_Diffusion_Temperature_base::verifie_ch_init_nb_comp(ch_ref, nb_comp); //traitement normal
}

/*! @brief Renvoie le nom du domaine d'application de l'equation.
 *
 * Ici "Thermique".
 *
 * @return (Motcle&) le nom du domaine d'application de l'equation
 */
const Motcle& Energie_Multiphase::domaine_application() const
{
  static Motcle mot("Thermique");
  return mot;
}

void Energie_Multiphase::dimensionner_matrice_sans_mem(Matrice_Morse& matrice)
{
  Convection_Diffusion_Temperature_base::dimensionner_matrice_sans_mem(matrice);
  if (evanescence_.non_nul()) evanescence_->dimensionner(matrice);
}

int Energie_Multiphase::has_interface_blocs() const
{
  int ok = Convection_Diffusion_Temperature_base::has_interface_blocs();
  if (evanescence_.non_nul()) ok &= evanescence_->has_interface_blocs();
  return ok;
}

/* l'evanescence passe en dernier */
void Energie_Multiphase::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  Convection_Diffusion_Temperature_base::dimensionner_blocs(matrices, semi_impl);
  if (evanescence_.non_nul()) evanescence_->dimensionner_blocs(matrices, semi_impl);
}

void Energie_Multiphase::assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl)
{
  Convection_Diffusion_Temperature_base::assembler_blocs_avec_inertie(matrices, secmem, semi_impl);
  if (evanescence_.non_nul()) evanescence_->ajouter_blocs(matrices, secmem, semi_impl);
}


void Energie_Multiphase::calculer_alpha_rho_e(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Equation_base& eqn = ref_cast(Equation_base, obj);
  const Fluide_base& fl = ref_cast(Fluide_base, eqn.milieu());
  const Champ_base& ch_rho = fl.masse_volumique();
  const Champ_Inc_base& ch_alpha = ref_cast(Pb_Multiphase, eqn.probleme()).equation_masse().inconnue(),
                        &ch_en = ref_cast(Champ_Inc_base, fl.energie_interne()), //toujours un Champ_Inc
                         *pch_rho = sub_type(Champ_Inc_base, ch_rho) ? &ref_cast(Champ_Inc_base, ch_rho) : nullptr; //pas toujours un Champ_Inc
  const DoubleTab& alpha = ch_alpha.valeurs(), &rho = ch_rho.valeurs(), &en = ch_en.valeurs();

  /* valeurs du champ */
  int i, n, N = val.line_size(), Nl = val.dimension_tot(0), cR = sub_type(Champ_Uniforme, ch_rho);
  for (i = 0; i < Nl; i++)
    for (n = 0; n < N; n++) val(i, n) = alpha(i, n) * rho(!cR * i, n) * en(i, n);

  /* on ne peut utiliser valeur_aux_bords que si ch_rho a un domaine_dis_base */
  DoubleTab b_al = ch_alpha.valeur_aux_bords(), b_rho, b_en = ch_en.valeur_aux_bords();
  int Nb = b_al.dimension_tot(0);
  if (ch_rho.a_un_domaine_dis_base()) b_rho = ch_rho.valeur_aux_bords();
  else b_rho.resize(Nb, N), ch_rho.valeur_aux(ref_cast(Domaine_VF, eqn.domaine_dis()).xv_bord(), b_rho);
  for (i = 0; i < Nb; i++)
    for (n = 0; n < N; n++) bval(i, n) = b_al(i, n) * b_rho(i, n) * b_en(i, n);

  DoubleTab& d_a = deriv["alpha"];//derivee en alpha : rho * en
  for (d_a.resize(Nl, N), i = 0; i < Nl; i++)
    for (n = 0; n < N; n++) d_a(i, n) = rho(!cR * i, n) * en(i, n);

  /* derivees a travers rho et en */
  const tabs_t d_vide = {}, &d_rho = pch_rho ? pch_rho->derivees() : d_vide, &d_en = ch_en.derivees();
  std::set<std::string> vars; //liste de toutes les derivees possibles
  for (auto && d_c : d_rho) vars.insert(d_c.first);
  for (auto && d_c : d_en) vars.insert(d_c.first);

  for (auto && var : vars)
    {
      const DoubleTab *dr = d_rho.count(var) ? &d_rho.at(var) : nullptr, *de = d_en.count(var) ? &d_en.at(var) : nullptr;
      DoubleTab& d_v = deriv[var];
      for (d_v.resize(Nl, N), i = 0; i < Nl; i++)
        for (n = 0; n < N; n++)
          d_v(i, n) = alpha(i, n) * ((dr ? (*dr)(i, n) * en(i, n) : 0) + (de ? rho(!cR * i, n) * (*de)(i, n) : 0));
    }
}

void Energie_Multiphase::calculer_alpha_rho_h(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Equation_base& eqn = ref_cast(Equation_base, obj);
  const Fluide_base& fl = ref_cast(Fluide_base, eqn.milieu());
  const Champ_base& ch_rho = fl.masse_volumique();
  const Champ_Inc_base& ch_alpha = ref_cast(Pb_Multiphase, eqn.probleme()).equation_masse().inconnue(),
                        &ch_h = ref_cast(Champ_Inc_base, fl.enthalpie()), //toujours un Champ_Inc
                         *pch_rho = sub_type(Champ_Inc_base, ch_rho) ? &ref_cast(Champ_Inc_base, ch_rho) : nullptr; //pas toujours un Champ_Inc
  const DoubleTab& alpha = ch_alpha.valeurs(), &rho = ch_rho.valeurs(), &h = ch_h.valeurs();

  /* valeurs du champ */
  int i, n, N = val.line_size(), Nl = val.dimension_tot(0), cR = sub_type(Champ_Uniforme, ch_rho);
  for (i = 0; i < Nl; i++)
    for (n = 0; n < N; n++) val(i, n) = alpha(i, n) * rho(!cR * i, n) * h(i, n);

  /* on ne peut utiliser valeur_aux_bords que si ch_rho a un domaine_dis_base */
  DoubleTab b_al = ch_alpha.valeur_aux_bords(), b_rho, b_h = ch_h.valeur_aux_bords();
  int Nb = b_al.dimension_tot(0);
  if (ch_rho.a_un_domaine_dis_base()) b_rho = ch_rho.valeur_aux_bords();
  else b_rho.resize(Nb, N), ch_rho.valeur_aux(ref_cast(Domaine_VF, eqn.domaine_dis()).xv_bord(), b_rho);
  for (i = 0; i < Nb; i++)
    for (n = 0; n < N; n++) bval(i, n) = b_al(i, n) * b_rho(i, n) * b_h(i, n);

  DoubleTab& d_a = deriv["alpha"];//derivee en alpha : rho * h
  for (d_a.resize(Nl, N), i = 0; i < Nl; i++)
    for (n = 0; n < N; n++) d_a(i, n) = rho(!cR * i, n) * h(i, n);

  /* derivees a travers rho et en */
  const tabs_t d_vide = {}, &d_rho = pch_rho ? pch_rho->derivees() : d_vide, &d_h = ch_h.derivees();
  std::set<std::string> vars; //liste de toutes les derivees possibles
  for (auto && d_c : d_rho) vars.insert(d_c.first);
  for (auto && d_c : d_h) vars.insert(d_c.first);

  for (auto && var : vars)
    {
      const DoubleTab *dr = d_rho.count(var) ? &d_rho.at(var) : nullptr, *dh = d_h.count(var) ? &d_h.at(var) : nullptr;
      DoubleTab& d_v = deriv[var];
      for (d_v.resize(Nl, N), i = 0; i < Nl; i++)
        for (n = 0; n < N; n++)
          d_v(i, n) = alpha(i, n) * ((dr ? (*dr)(i, n) * h(i, n) : 0) + (dh ? rho(!cR * i, n) * (*dh)(i, n) : 0));
    }
}

void Energie_Multiphase::init_champ_convecte() const
{
  if (champ_convecte_.non_nul()) return; //deja fait
  int Nt = inconnue().nb_valeurs_temporelles(), Nl = inconnue().valeurs().size_reelle_ok() ? inconnue().valeurs().dimension(0) : -1, Nc = inconnue().valeurs().line_size();
  //champ_convecte_ : meme type / support que l'inconnue
  discretisation().creer_champ(champ_convecte_, domaine_dis(), inconnue().que_suis_je(), "N/A", "N/A", Nc, Nl, Nt, schema_temps().temps_courant());
  champ_convecte_->associer_eqn(*this);
  auto nom_fonc = get_fonc_champ_convecte();
  champ_convecte_->nommer(nom_fonc.first);
  champ_convecte_->init_champ_calcule(*this, nom_fonc.second);
}
