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

#include <Fluide_Quasi_Compressible.h>
#include <Neumann_sortie_libre.h>
#include <Loi_Etat_Multi_GP_QC.h>
#include <Discretisation_base.h>
#include <Champ_Fonc_Fonction.h>
#include <Probleme_base.h>
#include <Param.h>

Implemente_instanciable(Fluide_Quasi_Compressible,"Fluide_Quasi_Compressible",Fluide_Dilatable_base);
// XD fluide_quasi_compressible fluide_dilatable_base fluide_quasi_compressible -1 Quasi-compressible flow with a low mach number assumption; this means that the thermo-dynamic pressure (used in state law) is uniform in space.
// XD attr sutherland bloc_sutherland sutherland 1 Sutherland law for viscosity and for conductivity.
// XD attr pression double pression 1 Initial thermo-dynamic pressure used in the assosciated state law.
// XD attr loi_etat loi_etat_base loi_etat 1 The state law that will be associated to the Quasi-compressible fluid.
// XD attr traitement_pth chaine(into=["edo","constant","conservation_masse"]) traitement_pth 1 Particular treatment for the thermodynamic pressure Pth ; there are three possibilities: NL2 1) with the keyword \'edo\' the code computes Pth solving an O.D.E. ; in this case, the mass is not strictly conserved (it is the default case for quasi compressible computation): NL2 2) the keyword \'conservation_masse\' forces the conservation of the mass (closed geometry or with periodic boundaries condition) NL2 3) the keyword \'constant\' makes it possible to have a constant Pth ; it\'s the good choice when the flow is open (e.g. with pressure boundary conditions). NL2 It is possible to monitor the volume averaged value for temperature and density, plus Pth evolution in the .evol_glob file.
// XD attr traitement_rho_gravite chaine(into=["standard","moins_rho_moyen"]) traitement_rho_gravite 1 It may be :1) \`standard\` : the gravity term is evaluted with rho*g (It is the default). 2) \`moins_rho_moyen\` : the gravity term is evaluated with (rho-rhomoy) *g. Unknown pressure is then P*=P+rhomoy*g*z. It is useful when you apply uniforme pressure boundary condition like P*=0.
// XD attr temps_debut_prise_en_compte_drho_dt double temps_debut_prise_en_compte_drho_dt 1 While time<value, dRho/dt is set to zero (Rho, volumic mass). Useful for some calculation during the first time steps with big variation of temperature and volumic mass.
// XD attr omega_relaxation_drho_dt double omega_relaxation_drho_dt 1 Optional option to have a relaxed algorithm to solve the mass equation. value is used (1 per default) to specify omega.
// XD attr lambda field_base lambda_u 1 Conductivity (W.m-1.K-1).
// XD attr mu field_base mu 1 Dynamic viscosity (kg.m-1.s-1).

// XD bloc_sutherland objet_lecture nul 0 Sutherland law for viscosity mu(T)=mu0*((T0+C)/(T+C))*(T/T0)**1.5 and (optional) for conductivity lambda(T)=mu0*Cp/Prandtl*((T0+Slambda)/(T+Slambda))*(T/T0)**1.5
// XD attr problem_name ref_Pb_base problem_name 0 Name of problem.
// XD attr mu0 chaine(into=["mu0"]) mu0 0 not_set
// XD attr mu0_val double mu0_val 0 not_set
// XD attr t0 chaine(into=["T0"]) t0 0 not_set
// XD attr t0_val double t0_val 0 not_set
// XD attr Slambda chaine(into=["Slambda"]) Slambda 1 not_set
// XD attr s double s 1 not_set
// XD attr C chaine(into=["C"]) C 0 not_set
// XD attr c_val double c_val 0 not_set

Sortie& Fluide_Quasi_Compressible::printOn(Sortie& os) const { return Fluide_Dilatable_base::printOn(os); }

Entree& Fluide_Quasi_Compressible::readOn(Entree& is) { return Fluide_Dilatable_base::readOn(is); }

void Fluide_Quasi_Compressible::set_param(Param& param)
{
  Fluide_Dilatable_base::set_param(param);
  param.ajouter("temps_debut_prise_en_compte_drho_dt", &temps_debut_prise_en_compte_drho_dt_);
  param.ajouter("omega_relaxation_drho_dt", &omega_drho_dt_);
  param.ajouter_non_std("pression", (this), Param::REQUIRED);
  param.ajouter_non_std("Traitement_rho_gravite", (this));
}

int Fluide_Quasi_Compressible::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "pression")
    {
      is >> Pth_;
      Pth_n_ = Pth_;
      ch_rho_.typer("Champ_Uniforme");
      DoubleTab& tab_rho = ch_rho_->valeurs();
      tab_rho.resize(1, 1);
      tab_rho(0, 0) = 1.;
      return 1;
    }
  else if (mot == "Traitement_rho_gravite")
    {
      Motcle trait;
      is >> trait;
      Motcles les_options(2);
      {
        les_options[0] = "standard";
        les_options[1] = "moins_rho_moyen";
      }
      traitement_rho_gravite_ = les_options.search(trait);
      if (traitement_rho_gravite_ == -1)
        {
          Cerr << trait << " is not understood as an option of the keyword " << mot << finl;
          Cerr << "One of the following options was expected : " << les_options << finl;
          Process::exit();
        }
      return 1;
    }
  else
    return Fluide_Dilatable_base::lire_motcle_non_standard(mot, is);
}

/*! @brief Complete le fluide avec les champs inconnus associes au probleme
 *
 * @param (Pb_Thermohydraulique& pb) le probleme a resoudre
 */
void Fluide_Quasi_Compressible::completer(const Probleme_base& pb)
{
  Cerr << "Fluide_Quasi_Compressible::completer Pth = " << Pth_ << finl;
  if ((loi_etat_->que_suis_je() == "Loi_Etat_rhoT_Gaz_Parfait_QC" || loi_etat_->que_suis_je() == "Loi_Etat_Binaire_Gaz_Parfait_QC") && traitement_PTh_ == 0)
    {
      Cerr << "The option Traitement_PTh EDO is not allowed with the state law " << loi_etat_->que_suis_je() << finl;
      Cerr << "Set **traitement_pth** constant or conservation_masse in the Fluide_Quasi_Compressible bloc definition." << finl;
      Process::exit();
    }

  Fluide_Dilatable_base::completer(pb);
}

void Fluide_Quasi_Compressible::discretiser(const Probleme_base& pb, const Discretisation_base& dis)
{
  const Domaine_dis_base& domaine_dis = pb.equation(0).domaine_dis();
  double temps = pb.schema_temps().temps_courant();

  // In *_Melange_Binaire_QC we do not even have a temperature variable ...
  // it is the species mass fraction Y1... Although named ch_temperature
  if (pb.que_suis_je() == "Pb_Hydraulique_Melange_Binaire_QC" || pb.que_suis_je() == "Pb_Hydraulique_Melange_Binaire_Turbulent_QC")
    dis.discretiser_champ("temperature", domaine_dis, "fraction_massique", "neant", 1, temps, loi_etat_->temperature_);
  else
    dis.discretiser_champ("temperature", domaine_dis, "temperature", "K", 1, temps, loi_etat_->temperature_);

  if (type_fluide() != "Gaz_Parfait")
    loi_etat()->champs_compris().ajoute_champ(ch_temperature());

  Fluide_Dilatable_base::discretiser(pb, dis);
}

void Fluide_Quasi_Compressible::remplir_champ_pression_tot(int n, const DoubleTab& tab_PHydro, DoubleTab& tab_PTot)
{
  double Pth = Pth_;
  CDoubleTabView PHydro = tab_PHydro.view_ro();
  DoubleTabView PTot = tab_PTot.view_wo();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), n, KOKKOS_LAMBDA(const int i)
  {
    PTot(i,0) = PHydro(i,0) + Pth;
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
}
