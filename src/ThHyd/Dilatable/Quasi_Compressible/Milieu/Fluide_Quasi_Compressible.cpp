/****************************************************************************
* Copyright (c) 2021, CEA
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
#include <Probleme_base.h>
#include <Discretisation_base.h>
#include <Loi_Etat_Multi_GP_QC.h>
#include <Param.h>
#include <Champ_Fonc_Fonction.h>
#include <Neumann_sortie_libre.h>

Implemente_instanciable_sans_constructeur(Fluide_Quasi_Compressible,"Fluide_Quasi_Compressible",Fluide_Dilatable_base);
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

Fluide_Quasi_Compressible::Fluide_Quasi_Compressible() : traitement_rho_gravite_(0),
  temps_debut_prise_en_compte_drho_dt_(-DMAXFLOAT),omega_drho_dt_(1.) { }

Sortie& Fluide_Quasi_Compressible::printOn(Sortie& os) const
{
  os << que_suis_je() << finl;
  Fluide_Dilatable_base::ecrire(os);
  return os;
}

Entree& Fluide_Quasi_Compressible::readOn(Entree& is)
{
  Fluide_Dilatable_base::readOn(is);
  return is;
}

void Fluide_Quasi_Compressible::checkTraitementPth(const Zone_Cl_dis& zone_cl)
{
  /*
   * traitement_PTh=0 => resolution classique de l'edo
   * traitement_PTh=1 => pression calculee pour conserver la masse
   * traitement_PTh=2 => pression laissee cste.
   */

  if (traitement_PTh==0)
    {
      /* Do nothing*/
    }
  else
    {
      int pression_imposee=0;
      int size=zone_cl.les_conditions_limites().size();
      assert(size!=0);
      for (int n=0; n<size; n++)
        {
          const Cond_lim& la_cl = zone_cl.les_conditions_limites(n);
          if (sub_type(Neumann_sortie_libre, la_cl.valeur())) pression_imposee=1;
        }

      if (pression_imposee && traitement_PTh!=2)
        {
          if (Process::je_suis_maitre())
            {
              Cerr << "The Traitement_Pth option selected is not coherent with the boundaries conditions." << finl;
              Cerr << "Traitement_Pth constant must be used for the case of free outlet." << finl;
            }
          Process::exit();
        }

      if (!pression_imposee && traitement_PTh!=1)
        {
          if (Process::je_suis_maitre())
            {
              Cerr << "The Traitement_Pth option selected is not coherent with the boundaries conditions." << finl;
              Cerr << "Traitement_Pth conservation_masse must be used for the case without free outlet." << finl;
            }
          Process::exit();
        }
    }
}

void Fluide_Quasi_Compressible::set_param(Param& param)
{
  Fluide_Dilatable_base::set_param(param);
  param.ajouter("temps_debut_prise_en_compte_drho_dt",&temps_debut_prise_en_compte_drho_dt_);
  param.ajouter("omega_relaxation_drho_dt",&omega_drho_dt_);
  param.ajouter_non_std("pression",(this),Param::REQUIRED);
  param.ajouter_non_std("Traitement_rho_gravite",(this));
}

int Fluide_Quasi_Compressible::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="pression")
    {
      is>>Pth_;
      Pth_n = Pth_;
      rho.typer("Champ_Uniforme");
      DoubleTab& tab_rho=rho.valeurs();
      tab_rho.resize(1,1);
      tab_rho(0,0) = 1.;
      return 1;
    }
  else if (mot=="Traitement_PTh")
    {
      Motcle trait;
      is >> trait;
      Motcles les_options(3);
      {
        les_options[0] = "edo";
        les_options[1] = "conservation_masse";
        les_options[2] = "constant";
      }
      traitement_PTh=les_options.search(trait);
      if (traitement_PTh == -1)
        {
          Cerr<< trait << " is not understood as an option of the keyword " << mot <<finl;
          Cerr<< "One of the following options was expected : " << les_options << finl;
          Process::exit();
        }
      return 1;
    }
  else if (mot=="Traitement_rho_gravite")
    {
      Motcle trait;
      is >> trait;
      Motcles les_options(2);
      {
        les_options[0] = "standard";
        les_options[1] = "moins_rho_moyen";
      }
      traitement_rho_gravite_=les_options.search(trait);
      if (traitement_rho_gravite_ == -1)
        {
          Cerr<< trait << " is not understood as an option of the keyword " << mot <<finl;
          Cerr<< "One of the following options was expected : " << les_options << finl;
          Process::exit();
        }
      return 1;
    }
  else return Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);
}

// Description:
//    Complete le fluide avec les champs inconnus associes au probleme
// Precondition:
// Parametre: Pb_Thermohydraulique& pb
//    Signification: le probleme a resoudre
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Fluide_Quasi_Compressible::completer(const Probleme_base& pb)
{
  Cerr<<"Fluide_Quasi_Compressible::completer Pth = " << Pth_ << finl;
  if ((loi_etat_->que_suis_je() == "Loi_Etat_rhoT_Gaz_Parfait_QC" || loi_etat_->que_suis_je() == "Loi_Etat_Binaire_Gaz_Parfait_QC" )
      && traitement_PTh == 0)
    {
      Cerr << "The option Traitement_PTh EDO is not allowed with the state law " << loi_etat_->que_suis_je() << finl;
      Cerr << "Set **traitement_pth** constant or conservation_masse in the Fluide_Quasi_Compressible bloc definition." << finl;
      Process::exit();
    }

  Fluide_Dilatable_base::completer(pb);
  if (traitement_PTh != 2) completer_edo(pb);
}

// Description:
//    Prepare le pas de temps
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Fluide_Quasi_Compressible::preparer_pas_temps()
{
  Fluide_Dilatable_base::preparer_pas_temps();
  if (traitement_PTh != 2 ) EDO_Pth_->mettre_a_jour_CL(Pth_);
}

void Fluide_Quasi_Compressible::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  const Zone_dis_base& zone_dis=pb.equation(0).zone_dis();
  double temps=pb.schema_temps().temps_courant();

  // In *_Melange_Binaire_QC we do not even have a temperature variable ...
  // it is the species mass fraction Y1... Although named ch_temperature
  Champ_Don& ch_TK = ch_temperature();
  if (pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_QC" || pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_Turbulent_QC")
    dis.discretiser_champ("temperature",zone_dis,"fraction_massique","neant",1,temps,ch_TK);
  else
    dis.discretiser_champ("temperature",zone_dis,"temperature","K",1,temps,ch_TK);

  if (type_fluide()!="Gaz_Parfait")
    loi_etat().valeur().champs_compris().ajoute_champ(ch_TK);

  Fluide_Dilatable_base::discretiser(pb,dis);
}

void Fluide_Quasi_Compressible::prepare_pressure_edo()
{
  if (traitement_PTh != 2) EDO_Pth_->completer();

  eos_tools_->mettre_a_jour(le_probleme_->schema_temps().temps_courant());
}

void Fluide_Quasi_Compressible::write_mean_edo(double temps)
{
  double Ch_m = eos_tools_->moyenne_vol(inco_chaleur_->valeurs());
  double rhom = eos_tools_->moyenne_vol(rho.valeurs());

  if(je_suis_maitre() && traitement_PTh != 2)
    {
      SFichier fic (output_file_,ios::app);
      fic<<temps <<" "<<Ch_m<<" "<<rhom<<" "<<Pth_<<finl;
    }
}

void Fluide_Quasi_Compressible::completer_edo(const Probleme_base& pb)
{
  assert(traitement_PTh != 2);
  Nom typ = pb.equation(0).discretisation().que_suis_je();
  if (typ=="VEFPreP1B") typ = "VEF";
  typ += "_";
  // EDO_Pression_th_VDF/VEF_Melange_Binaire not implemented yet
  // typer Gaz_Parfait instead to use when traitement_PTh=1...
  if (pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_QC" || pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_Turbulent_QC")
    typ +="Gaz_Parfait";
  else
    typ += loi_etat_->type_fluide();

  EDO_Pth_.typer(typ);
  EDO_Pth_->associer_zones(pb.equation(0).zone_dis(),pb.equation(0).zone_Cl_dis());
  EDO_Pth_->associer_fluide(*this);
  EDO_Pth_->mettre_a_jour_CL(Pth_);

  // Write in file
  output_file_ = Objet_U::nom_du_cas();
  output_file_ += "_";
  output_file_ += pb.le_nom();
  output_file_ += ".evol_glob";

  Cerr << "Warning! evol_glob file renamed " << output_file_ << finl;
  if(je_suis_maitre())
    {
      SFichier fic (output_file_);
      fic<<"# Time sum(T*dv)/sum(dv)[K] sum(rho*dv)/sum(dv)[kg/m3] Pth[Pa]"<<finl;
    }
}

void Fluide_Quasi_Compressible::remplir_champ_pression_tot(int n, const DoubleTab& PHydro, DoubleTab& PTot)
{
  for (int i=0 ; i<n ; i++) PTot(i,0) = PHydro(i,0) + Pth_;
}
