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

#include <Schema_Euler_Implicite.h>
#include <Discretisation_base.h>
#include <Loi_Fermeture_base.h>
#include <Milieu_composite.h>
#include <Interprete_bloc.h>
#include <Pb_Multiphase.h>
#include <Domaine.h>
#include <EChaine.h>
#include <Debog.h>
#include <SETS.h>

Implemente_instanciable(Pb_Multiphase, "Pb_Multiphase", Pb_Fluide_base);
// XD Pb_Multiphase Pb_base Pb_Multiphase -1 A problem that allows the resolution of N-phases with 3*N equations
// XD attr milieu_composite bloc_lecture milieu_composite 1 The composite medium associated with the problem.
// XD attr Milieu_MUSIG bloc_lecture Milieu_MUSIG 1 The composite medium associated with the problem.
// XD attr correlations bloc_lecture correlations 1 List of correlations used in specific source terms (i.e. interfacial flux,  interfacial friction, ...)
// XD attr QDM_Multiphase QDM_Multiphase QDM_Multiphase 0 Momentum conservation equation for a multi-phase problem where the unknown is the velocity
// XD attr Masse_Multiphase Masse_Multiphase Masse_Multiphase 0 Mass consevation equation for a multi-phase problem where the unknown is the alpha (void fraction)
// XD attr Energie_Multiphase Energie_Multiphase Energie_Multiphase 0 Internal energy conservation equation for a multi-phase problem where the unknown is the temperature
// XD attr Echelle_temporelle_turbulente Echelle_temporelle_turbulente Echelle_temporelle_turbulente 1 Turbulent Dissipation time scale equation for a turbulent mono/multi-phase problem (available in TrioCFD)
// XD attr Energie_cinetique_turbulente Energie_cinetique_turbulente Energie_cinetique_turbulente 1 Turbulent kinetic Energy conservation equation for a turbulent mono/multi-phase problem (available in TrioCFD)
// XD attr Energie_cinetique_turbulente_WIT Energie_cinetique_turbulente_WIT Energie_cinetique_turbulente_WIT 1 Bubble Induced Turbulent kinetic Energy equation for a turbulent multi-phase problem (available in TrioCFD)
// XD attr Taux_dissipation_turbulent Taux_dissipation_turbulent Taux_dissipation_turbulent 1 Turbulent Dissipation frequency equation for a turbulent mono/multi-phase problem (available in TrioCFD)

// XD Energie_cinetique_turbulente eqn_base Energie_cinetique_turbulente 1 Turbulent kinetic Energy conservation equation for a turbulent mono/multi-phase problem (available in TrioCFD)
// XD Echelle_temporelle_turbulente eqn_base Echelle_temporelle_turbulente -1 Turbulent Dissipation time scale equation for a turbulent mono/multi-phase problem (available in TrioCFD)
// XD Energie_cinetique_turbulente_WIT eqn_base Energie_cinetique_turbulente_WIT -1 Bubble Induced Turbulent kinetic Energy equation for a turbulent multi-phase problem (available in TrioCFD)
// XD Taux_dissipation_turbulent eqn_base Taux_dissipation_turbulent -1 Turbulent Dissipation frequency equation for a turbulent mono/multi-phase problem (available in TrioCFD)

Sortie& Pb_Multiphase::printOn(Sortie& os) const
{
  return Pb_Fluide_base::printOn(os);
}

Entree& Pb_Multiphase::readOn(Entree& is)
{
  if (discretisation().is_vef())
    {
      Cerr << "Error: Problem of type " << que_suis_je() << " is not available for VEF discretization" << finl;
      Cerr << "It is only available for VDF, PolyMAC_P0P1NC and PolyMAC_P0 discretizations." << finl;
      Process::exit();
    }
  return Pb_Fluide_base::readOn(is);
}

Entree& Pb_Multiphase::lire_equations(Entree& is, Motcle& mot)
{
  // Verify that user choosed adapted time scheme/solver
  if (!sub_type(Schema_Euler_Implicite, le_schema_en_temps_.valeur()))
    {
      Cerr << "Error: for Pb_Multiphase, you can only use Scheme_euler_implicit time scheme with sets/ice solver" << finl;
      Process::exit();
    }
  else if (sub_type(Schema_Euler_Implicite, le_schema_en_temps_.valeur()))
    {
      Schema_Euler_Implicite& schm_imp = ref_cast(Schema_Euler_Implicite, le_schema_en_temps_.valeur());
      if (!sub_type(SETS, schm_imp.solveur().valeur()))
        {
          Cerr << "Error: for Pb_Multiphase, you can only use Scheme_euler_implicit time scheme with sets/ice solver" << finl;
          Process::exit();
        }
    }

  bool already_read;

  is >> mot;
  if (mot == "correlations")
    lire_correlations(is), already_read = false;
  else
    already_read = true;

  typer_lire_correlation_hem(); // enforce a interfacial flux correlation with constant coefficient si HEM

  Cerr << "Reading of the equations" << finl;
  for (int i = 0; i < nombre_d_equations(); i++, already_read = false)
    {
      if (!already_read)
        is >> mot;
      is >> getset_equation_by_name(mot);
    }

  /* lecture d'equations optionnelles */
  Noms noms_eq, noms_eq_maj; //noms de toutes les equations possibles!
  Type_info::les_sous_types(Nom("Equation_base"), noms_eq);
  for (auto &itr : noms_eq)
    noms_eq_maj.add(Motcle(itr)); //ha ha ha
  for (is >> mot; noms_eq_maj.rang(mot) >= 0; is >> mot)
    {
      eq_opt_.add(OWN_PTR(Equation_base)()); //une autre equation optionelle
      eq_opt_.dernier().typer(mot); //on lui donne le bon type
      Equation_base& eq = eq_opt_.dernier().valeur();
      //memes associations que pour les autres equations : probleme, milieu, schema en temps
      eq.associer_pb_base(*this);
      eq.associer_milieu_base(milieu());
      eq.associer_sch_tps_base(le_schema_en_temps_);
      eq.associer_domaine_dis(domaine_dis());
      eq.discretiser(); //a faire avant de lire l'equation
      is >> eq; //et c'est parti!
      eq.associer_milieu_equation(); //remontee vers le milieu
    }
  return is;
}

void Pb_Multiphase::typer_lire_milieu(Entree& is)
{
  le_milieu_.resize(1); /* Un milieu .. mais composite !! */
  is >> le_milieu_[0]; // On commence par la lecture du milieu
  if (!sub_type(Milieu_composite, le_milieu_[0].valeur()))
    {
      Cerr << "Error: Fluid of type " << le_milieu_[0]->le_type() << " is not compatible with " << que_suis_je() << " problem which accepts only Milieu_composite medium" << finl;
      Cerr << "Check your datafile!" << finl;
      Process::exit();
    }
  noms_phases_ = ref_cast(Milieu_composite,le_milieu_[0].valeur()).noms_phases();
  associer_milieu_base(le_milieu_[0].valeur());

  // On discretise les equations maintenant ! voir avec Elie si t'es pas d'accord
  Probleme_base::discretiser_equations();
  // remontee de l'inconnue vers le milieu
  for (int i = 0; i < nombre_d_equations(); i++)
    equation(i).associer_milieu_equation();
  // On discretise le milieu composite
  equation(0).milieu().discretiser((*this), la_discretisation_.valeur());
}

/*! @brief Renvoie le nombre d'equation, Renvoie 2 car il y a 2 equations a un probleme de
 *
 *     thermo-hydraulique standard:
 *         l'equation de Navier Stokes
 *         l' equation de la thermique de type Convection_Diffusion_Temperature
 *
 * @return (int) le nombre d'equation
 */
int Pb_Multiphase::nombre_d_equations() const
{
  return 3 + eq_opt_.size();
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature si i=1
 *     (version const)
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
const Equation_base& Pb_Multiphase::equation(int i) const
{
  if (i == 0)
    return eq_qdm_;
  else if (i == 1)
    return eq_masse_;
  else if (i == 2)
    return eq_energie_;
  else if (i < 3 + eq_opt_.size())
    return eq_opt_[i - 3].valeur();
  else
    {
      Cerr << "Pb_Multiphase::equation() : Wrong equation number" << i << "!" << finl;
      Process::exit();
    }
  return eq_qdm_; //pour renvoyer quelque chose
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature si i=1
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
Equation_base& Pb_Multiphase::equation(int i)
{
  if (i == 0)
    return eq_qdm_;
  else if (i == 1)
    return eq_masse_;
  else if (i == 2)
    return eq_energie_;
  else if (i < 3 + eq_opt_.size())
    return eq_opt_[i - 3].valeur();
  else
    {
      Cerr << "Pb_Multiphase::equation() : Wrong equation number" << i << "!" << finl;
      Process::exit();
    }
  return eq_qdm_; //pour renvoyer quelque chose
}

/*! @brief Associe le milieu au probleme Le milieu doit etre de type fluide incompressible
 *
 * @param (Milieu_base& mil) le milieu physique a associer au probleme
 * @throws mauvais type de milieu physique
 */
void Pb_Multiphase::associer_milieu_base(const Milieu_base& mil)
{
  /* controler le type de milieu ici */
  equation_qdm().associer_milieu_base(mil);
  equation_energie().associer_milieu_base(mil);
  equation_masse().associer_milieu_base(mil);
}

/*! @brief Teste la compatibilite des equations de la thermique et de l'hydraulique.
 *
 * Le test se fait sur les conditions
 *     aux limites discretisees de chaque equation.
 *     Appel la fonction de librairie hors classe:
 *       tester_compatibilite_hydr_thermique(const Domaine_Cl_dis&,const Domaine_Cl_dis&)
 *
 * @return (int) code de retour propage
 */
int Pb_Multiphase::verifier()
{
  const Domaine_Cl_dis& domaine_Cl_hydr = equation_qdm().domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_th = equation_energie().domaine_Cl_dis();
  return tester_compatibilite_hydr_thermique(domaine_Cl_hydr, domaine_Cl_th);
}

void Pb_Multiphase::preparer_calcul()
{
  Pb_Fluide_base::preparer_calcul();
  const double temps = schema_temps().temps_courant();
  mettre_a_jour(temps);
}

double Pb_Multiphase::calculer_pas_de_temps() const
{
  if (sub_type(ICE, ref_cast(Schema_Euler_Implicite, le_schema_en_temps_.valeur()).solveur().valeur()))
    return Pb_Fluide_base::calculer_pas_de_temps();
  else if (ref_cast(SETS, ref_cast(Schema_Euler_Implicite, le_schema_en_temps_.valeur()).solveur().valeur()).facsec_diffusion_for_sets() < 0.)
    return Pb_Fluide_base::calculer_pas_de_temps();

  // Case where we calculate the time step with higher facsec for diffusion

  double dt = schema_temps().pas_temps_max();
  for (int i = 0; i < nombre_d_equations(); i++)
    {
      double dt_op;

      int nb_op = equation(i).nombre_d_operateurs();
      for (int j = 0; j < nb_op; j++)
        {
          dt_op = equation(i).operateur(j).calculer_pas_de_temps();

          const Operateur_base& op = equation(i).operateur(j).l_op_base();

          if (le_schema_en_temps_->limpr())
            {
              if (j == 0)
                {
                  Cout << " " << finl;
                  Cout << "Printing of the next provisional time steps for the equation: " << equation(i).que_suis_je() << finl;
                }
              if (sub_type(Operateur_Conv_base, op))
                Cout << "   convective";
              else if (sub_type(Operateur_Diff_base, op))
                Cout << "   diffusive";
              else
                Cout << "   operator ";
              Cout << " time step : " << dt_op << finl;
            }
          if (sub_type(Operateur_Diff_base, op))
            dt_op *= ref_cast(SETS, ref_cast(Schema_Euler_Implicite, le_schema_en_temps_.valeur()).solveur().valeur()).facsec_diffusion_for_sets();
          dt = std::min(dt, dt_op);
        }
    }
  return dt;
}
