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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Echange_couplage_thermique.cpp
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_couplage_thermique.h>
#include <Discretisation_base.h>
#include <EChaine.h>
#include <Milieu_base.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Equation_base.h>

Implemente_instanciable_sans_constructeur( Echange_couplage_thermique, "Echange_couplage_thermique", Echange_global_impose ) ;
// XD Echange_couplage_thermique paroi_echange_global_impose Echange_couplage_thermique 1 Thermal coupling boundary condition
// XD attr text  suppress_param text  1 suppress
// XD attr h_imp suppress_param h_imp 1 suppress
// XD attr himpc suppress_param himpc 1 suppress
// XD attr ch    suppress_param ch    1 suppress
// XD attr temperature_paroi field_base temperature_paroi 1 Temperature
// XD attr flux_paroi  field_base flux_paroi  1 Wall heat flux

Echange_couplage_thermique::Echange_couplage_thermique()
{
  couplage_=-1.;
  reprise_ = false;
  phi_ext_lu_ = true;
}

Sortie& Echange_couplage_thermique::printOn( Sortie& os ) const
{
  Echange_global_impose::printOn( os );
  return os;
}

Entree& Echange_couplage_thermique::readOn( Entree& is )
{
  Cerr << "Reading of coupled boundary\n";

  EChaine ech1("Ch_front_var_instationnaire_dep 1");
  EChaine ech2("Ch_front_var_instationnaire_dep 1");

  ech1 >>  h_imp_;
  ech2 >>  T_ext();

  List_Nom noms;
  noms.add("temperature_paroi");
  noms.add("flux_paroi");
  lec_champs.lire_champs(is,noms);
  reprise_ = lec_champs.champs_lus();

  return is;
}

int Echange_couplage_thermique::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app = eqn.domaine_application();

  if ( (dom_app=="Thermique"))
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}

int Echange_couplage_thermique::compatible_avec_discr(const Discretisation_base& discr) const
{
  if (discr.que_suis_je() == "EF" || discr.que_suis_je() == "EF_axi" || discr.que_suis_je() == "VDF" || discr.que_suis_je() == "VEFPreP1B")
    return 1;
  else
    {
      err_pas_compatible(discr);
      return 0;
    }
}

void Echange_couplage_thermique::completer()
{
  Echange_global_impose::completer();

  coeff_ap.typer("Ch_front_var_instationnaire_dep");
  coeff_ap->fixer_nb_comp(1);
  coeff_ap->associer_fr_dis_base(frontiere_dis());
  coeff_ap->fixer_nb_valeurs_temporelles(3);

  coeff_sp.typer("Ch_front_var_instationnaire_dep");
  coeff_sp->fixer_nb_comp(1);
  coeff_sp->associer_fr_dis_base(frontiere_dis());
  coeff_sp->fixer_nb_valeurs_temporelles(3);

  coeff_heff.typer("Ch_front_var_instationnaire_dep");
  coeff_heff->fixer_nb_comp(1);
  coeff_heff->associer_fr_dis_base(frontiere_dis());
  coeff_heff->fixer_nb_valeurs_temporelles(3);

  temperature_Teff.typer("Ch_front_var_instationnaire_dep");
  temperature_Teff->fixer_nb_comp(1);
  temperature_Teff->associer_fr_dis_base(frontiere_dis());
  temperature_Teff->fixer_nb_valeurs_temporelles(3);
}

void Echange_couplage_thermique::get_dRho_dCp(int i, double& d_rho, double& d_Cp) const
{
  const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
  Nom nom_pb=ma_zone_cl_dis->equation().probleme().que_suis_je();
  const Champ_base& rho=mil.masse_volumique();
  if ((nom_pb.debute_par("Probleme_Interface")|| nom_pb==Nom("Probleme_Thermo_Front_Tracking"))||!sub_type(Champ_Uniforme,rho)
      ||nom_pb==Nom("Pb_Conduction") || nom_pb.debute_par("Pb_Conduction_Combustible"))
    {
      // Pour le front tracking, QC, et conduc_variable, on ne divise pas par Rho*Cp:
      d_rho=1.;
      d_Cp=1.;
    }
  else
    {
      const Champ_Don& Cp =mil.capacite_calorifique();

      if (sub_type(Champ_Uniforme,rho))
        {
          d_rho= rho(0,0);
        }
      else
        {
          d_rho= rho(i);
        }
      if (sub_type(Champ_Uniforme,Cp.valeur()))
        {
          d_Cp= Cp(0,0);
        }
      else
        {
          d_Cp= Cp.valeur()(i);
        }
    }
}

double Echange_couplage_thermique::champ_exterieur(int i, const Champ_front& champ_ext) const
{
  if (ma_zone_cl_dis->equation().que_suis_je()!="Convection_Diffusion_Concentration")
    {
      double d_rho, d_Cp;
      get_dRho_dCp(i, d_rho, d_Cp);

      if (champ_ext.valeurs().size()==1)
        return champ_ext(0,0)/(d_rho*d_Cp);
      else if (champ_ext.valeurs().dimension(1)==1)
        return champ_ext(i,0)/(d_rho*d_Cp);
      else
        Cerr << "Echange_couplage_thermique::flux_impose erreur" << finl;
      exit();
      return 0.;
    }
  else
    {
      if (champ_ext.valeurs().size()==1)
        return champ_ext(0,0);
      else if (champ_ext.valeurs().dimension(1)==1)
        return champ_ext(i,0);
      else
        Cerr << "Echange_couplage_thermique::flux_impose erreur" << finl;
      exit();
      return 0.;

    }
}

double Echange_couplage_thermique::champ_exterieur(int i,int j, const Champ_front& champ_ext) const
{
  if (ma_zone_cl_dis->equation().que_suis_je()!="Convection_Diffusion_Concentration")
    {
      double d_rho, d_Cp;
      get_dRho_dCp(i, d_rho, d_Cp);
      if (champ_ext.valeurs().dimension(0)==1)
        return champ_ext(0,j)/(d_rho*d_Cp);
      else
        return champ_ext(i,j)/(d_rho*d_Cp);
    }
  else
    {
      if (champ_ext.valeurs().dimension(0)==1)
        return champ_ext(0,j);
      else
        return champ_ext(i,j);
    }
}

double Echange_couplage_thermique::flux_exterieur_impose(int i,int j) const
{
  return couplage_*champ_exterieur(i,j,phi_ext());
}

double Echange_couplage_thermique::flux_exterieur_impose(int i) const
{
  return couplage_*champ_exterieur(i,phi_ext());
}

void Echange_couplage_thermique::changer_temps_futur(double temps, int i)
{
  Echange_global_impose::changer_temps_futur(temps,i);
  coeff_ap->changer_temps_futur(temps,i);
  coeff_sp->changer_temps_futur(temps,i);
  coeff_heff->changer_temps_futur(temps,i);
  temperature_Teff->changer_temps_futur(temps,i);
}

int Echange_couplage_thermique::avancer(double temps)
{
  coeff_ap->avancer(temps);
  coeff_sp->avancer(temps);
  coeff_heff->avancer(temps);
  temperature_Teff->avancer(temps);
  return  Echange_global_impose::avancer(temps);
}

int Echange_couplage_thermique::reculer(double temps)
{
  coeff_ap->reculer(temps);
  coeff_sp->reculer(temps);
  coeff_heff->reculer(temps);
  temperature_Teff->reculer(temps);
  return  Echange_global_impose::reculer(temps);
}

void Echange_couplage_thermique::set_temps_defaut(double temps)
{
  Echange_global_impose::set_temps_defaut(temps);
  coeff_ap->set_temps_defaut(temps);
  coeff_sp->set_temps_defaut(temps);
  coeff_heff->set_temps_defaut(temps);
  temperature_Teff->set_temps_defaut(temps);
}

void Echange_couplage_thermique::mettre_a_jour(double temps)
{
  Echange_global_impose::mettre_a_jour(temps);
  coeff_ap->mettre_a_jour(temps);
  coeff_sp->mettre_a_jour(temps);
  coeff_heff->mettre_a_jour(temps);
  temperature_Teff->mettre_a_jour(temps);
}

int Echange_couplage_thermique::initialiser(double temps)
{
  Echange_global_impose::initialiser(temps);

  if (reprise_)
    {
      Champ_Inc ch = zone_Cl_dis().equation().inconnue();
      ch->affecter(lec_champs.champ_lu("temperature_paroi"));
      DoubleTab& vals1 =le_champ_front->valeurs();
      ch->trace(frontiere_dis(),vals1,temps,0);

      ch->affecter(lec_champs.champ_lu("flux_paroi"));
      DoubleTab& vals = phi_ext_->valeurs();
      ch->trace(frontiere_dis(),vals,temps,0);
    }
  coeff_ap->initialiser(temps,zone_Cl_dis().inconnue());
  coeff_sp->initialiser(temps,zone_Cl_dis().inconnue());
  coeff_heff->initialiser(temps,zone_Cl_dis().inconnue());
  temperature_Teff->initialiser(temps,zone_Cl_dis().inconnue());

  return 1;
}
