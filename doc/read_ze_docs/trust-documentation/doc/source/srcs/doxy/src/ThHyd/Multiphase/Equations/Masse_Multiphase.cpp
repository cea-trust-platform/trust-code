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

#include <EcritureLectureSpecial.h>
#include <Pb_Multiphase_HEM.h>
#include <Masse_Multiphase.h>
#include <Champ_Uniforme.h>
#include <Matrice_Morse.h>
#include <Discret_Thyd.h>
#include <Fluide_base.h>
#include <Domaine_VF.h>
#include <TRUSTTrav.h>
#include <Domaine.h>
#include <EChaine.h>
#include <Param.h>

Implemente_instanciable(Masse_Multiphase, "Masse_Multiphase", Convection_Diffusion_std);
// XD Masse_Multiphase eqn_base Masse_Multiphase -1 Mass consevation equation for a multi-phase problem where the unknown is the alpha (void fraction)

Sortie& Masse_Multiphase::printOn(Sortie& is) const { return Equation_base::printOn(is); }

Entree& Masse_Multiphase::readOn(Entree& is)
{
  assert(l_inco_ch.non_nul());
  assert(le_fluide.non_nul());
  terme_convectif.associer_eqn(*this), evanescence.associer_eqn(*this);

  Equation_base::readOn(is);

  terme_convectif.set_fichier("Debit");
  terme_convectif.set_description((Nom)"Mass flow rate=Integral(-alpha*rho*u*ndS) [kg/s] if SI units used");

  champs_compris_.ajoute_champ(l_inco_ch);

  // Special treatment for Pb_Multiphase_HEM
  // We enforce the presence of a source term related to the interfacial flux automatically.
  if (sub_type(Pb_Multiphase_HEM, probleme()))
    {
      int check_source_FICC(0);
      for (int ii = 0; ii < sources().size(); ii++)
        if (sources()(ii).valeur().que_suis_je().debute_par("Flux_interfacial"))
          check_source_FICC = 1;
      if (check_source_FICC == 0)
        {
          EChaine source_FI("{ flux_interfacial }");
          lire_sources(source_FI);
        }
    }

  return is;
}

void Masse_Multiphase::set_param(Param& param)
{
  Equation_base::set_param(param);
  param.ajouter_non_std("convection",(this));
  param.ajouter_non_std("evanescence|vanishing",(this));
  param.ajouter_condition("is_read_convection","The convection operator must be read, select negligeable type if you want to neglect it.");
}

int Masse_Multiphase::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="evanescence") is >> evanescence;
  else return Convection_Diffusion_std::lire_motcle_non_standard(mot, is);
  return 1;
}

const Operateur& Masse_Multiphase::operateur(int i) const
{
  if (i)
    {
      Cerr << "Masse_Multiphase : wrong operator number " << i << finl;
      Process::exit();
    }
  return terme_convectif;
}

Operateur& Masse_Multiphase::operateur(int i)
{
  if (i)
    {
      Cerr << "Masse_Multiphase : wrong operator number " << i << finl;
      Process::exit();
    }
  return terme_convectif;
}

void Masse_Multiphase::dimensionner_matrice_sans_mem(Matrice_Morse& matrice)
{
  Equation_base::dimensionner_matrice_sans_mem(matrice);
  evanescence.valeur().dimensionner(matrice);
}

int Masse_Multiphase::has_interface_blocs() const
{
  int ok = Convection_Diffusion_std::has_interface_blocs();
  if (evanescence.non_nul()) ok &= evanescence.valeur().has_interface_blocs();
  return ok;
}

/* l'evanescence passe en dernier */
void Masse_Multiphase::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  Equation_base::dimensionner_blocs(matrices, semi_impl);
  if (evanescence.non_nul()) evanescence.valeur().dimensionner_blocs(matrices, semi_impl);
}

void Masse_Multiphase::assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl)
{
  Equation_base::assembler_blocs_avec_inertie(matrices, secmem, semi_impl);
  if (evanescence.non_nul()) evanescence.valeur().ajouter_blocs(matrices, secmem, semi_impl);
}

/*! @brief Associe un milieu physique a l'equation, le milieu est en fait caste en Fluide_base
 *
 * @param (Milieu_base& un_milieu)
 * @throws les proprietes physiques du fluide ne sont pas toutes specifiees
 */
void Masse_Multiphase::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Fluide_base& un_fluide = ref_cast(Fluide_base,un_milieu);
  associer_fluide(un_fluide);
}

/*! @brief Associe l inconnue de l equation a la loi d etat,
 *
 */
void Masse_Multiphase::completer()
{
  Equation_base::completer();
  terme_convectif.valeur().check_multiphase_compatibility();

  const Domaine_dis& zdis = domaine_dis();
  if (discretisation().is_vdf())
    {
      // initialiser l'operateur grad SI VDF
      Op_Grad_.associer_eqn(*this);
      Op_Grad_.typer();
      Op_Grad_.l_op_base().associer_eqn(*this);
      const Domaine_Cl_dis& zcl = domaine_Cl_dis();
      const Champ_Inc& inco = inconnue();
      Op_Grad_->associer(zdis, zcl, inco);
    }
}

/*! @brief Discretise l'equation.
 *
 */
void Masse_Multiphase::discretiser()
{
  int nb_valeurs_temp = schema_temps().nb_valeurs_temporelles();
  double temps = schema_temps().temps_courant();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  Cerr << "Volume fraction discretization" << finl;
  //On utilise temperature pour la directive car discretisation identique
  const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, probleme());
  dis.discretiser_champ("temperature",domaine_dis(),"alpha","sans_dimension", pb.nb_phases(),nb_valeurs_temp,temps,l_inco_ch);
  l_inco_ch.valeur().fixer_nature_du_champ(pb.nb_phases() == 1 ? scalaire : pb.nb_phases() == dimension ? vectoriel : multi_scalaire); //pfft
  Equation_base::discretiser();
  for (int i = 0; i < pb.nb_phases(); i++)
    l_inco_ch.valeur().fixer_nom_compo(i, Nom("alpha_") + pb.nom_phase(i));

  Cerr << "Masse_Multiphase::discretiser() ok" << finl;
}

/*! @brief Renvoie le milieu physique de l'equation.
 *
 * (un Fluide_base upcaste en Milieu_base)
 *     (version const)
 *
 * @return (Milieu_base&) le Fluide_base upcaste en Milieu_base
 */
const Milieu_base& Masse_Multiphase::milieu() const
{
  return fluide();
}


/*! @brief Renvoie le milieu physique de l'equation.
 *
 * (un Fluide_base upcaste en Milieu_base)
 *
 * @return (Milieu_base&) le Fluide_base upcaste en Milieu_base
 */
Milieu_base& Masse_Multiphase::milieu()
{
  return fluide();
}


/*! @brief Renvoie le fluide incompressible associe a l'equation.
 *
 * (version const)
 *
 * @return (Fluide_base&) le fluide incompressible associe a l'equation
 * @throws pas de fluide associe a l'eqaution
 */
const Fluide_base& Masse_Multiphase::fluide() const
{
  if (le_fluide.est_nul())
    {
      Cerr << "You forgot to associate the fluid to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_fluide.valeur();
}


/*! @brief Renvoie le fluide incompressible associe a l'equation.
 *
 * @return (Fluide_base&) le fluide incompressible associe a l'equation
 * @throws pas de fluide associe a l'eqaution
 */
Fluide_base& Masse_Multiphase::fluide()
{
  assert(le_fluide.non_nul());
  return le_fluide.valeur();
}

/*! @brief Renvoie le nom du domaine d'application de l'equation.
 *
 * Ici "Thermique".
 *
 * @return (Motcle&) le nom du domaine d'application de l'equation
 */
const Motcle& Masse_Multiphase::domaine_application() const
{
  static Motcle domaine ="Fraction_volumique";
  return domaine;
}

/*! @brief Associe un fluide de type Fluide_base a l'equation.
 *
 * @param (Fluide_base& un_fluide) le milieu a associer a l'equation
 */
void Masse_Multiphase::associer_fluide(const Fluide_base& un_fluide)
{
  assert(sub_type(Fluide_base,un_fluide));
  le_fluide = ref_cast(Fluide_base,un_fluide);
}

void Masse_Multiphase::calculer_alpha_rho(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Equation_base& eqn = ref_cast(Equation_base, obj);
  const Champ_base& ch_rho = eqn.milieu().masse_volumique().valeur();
  const Champ_Inc_base& ch_alpha = eqn.inconnue(), *pch_rho = sub_type(Champ_Inc_base, ch_rho) ? &ref_cast(Champ_Inc_base, ch_rho) : NULL;
  const DoubleTab& alpha = ch_alpha.valeurs(), &rho = ch_rho.valeurs();
  int i, nl = val.dimension_tot(0), n, N = val.line_size(), cR = sub_type(Champ_Uniforme, ch_rho);

  /* valeurs du champ */
  for (i = 0; i < nl; i++)
    for (n = 0; n < N; n++) val(i, n) = alpha(i, n) * rho(!cR * i, n);

  /* valeur aux bords */
  /* on ne peut utiliser valeur_aux_bords que si ch_rho a un domaine_dis_base */
  ch_rho.a_un_domaine_dis_base() ? bval = ch_rho.valeur_aux_bords() : ch_rho.valeur_aux(ref_cast(Domaine_VF, eqn.domaine_dis().valeur()).xv_bord(), bval);
  tab_multiply_any_shape(bval, ch_alpha.valeur_aux_bords(), VECT_ALL_ITEMS);

  /* derivees */
  DoubleTab& d_a = deriv["alpha"]; //derivee en alpha : rho
  for (d_a.resize(nl, N), i = 0; i < nl; i++)
    for (n = 0; n < N; n++) d_a(i, n) = rho(!cR * i , n);
  if (pch_rho)
    for (auto &&d_c : pch_rho->derivees()) //derivees en les dependances de rho
      {
        DoubleTab& der = deriv[d_c.first];
        for (der.resize(nl, N), i = 0; i < nl; i++)
          for (n = 0; n < N; n++) der(i, n) = d_c.second(i, n) * alpha(i, n);
      }
}
