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

#include <Echange_couplage_thermique.h>
#include <Discretisation_base.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <EChaine.h>

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
  phi_ext_lu_ = true;
}

Sortie& Echange_couplage_thermique::printOn( Sortie& os ) const { return Echange_global_impose::printOn( os ); }

Entree& Echange_couplage_thermique::readOn( Entree& is )
{
  if (app_domains.size() == 0) app_domains = { Motcle("Thermique") };
  if (supp_discs.size() == 0) supp_discs = { Nom("VDF"), Nom("VEFPreP1B"), Nom("EF_axi"), Nom("EF") };

  Cerr << "Reading of coupled boundary" << finl;

  EChaine ech1("Ch_front_var_instationnaire_dep 1");
  EChaine ech2("Ch_front_var_instationnaire_dep 1");

  ech1 >>  h_imp_;
  ech2 >>  T_ext();

  LIST(Nom) noms;
  noms.add("temperature_paroi");
  noms.add("flux_paroi");
  lec_champs.lire_champs(is,noms);
  reprise_ = lec_champs.champs_lus();

  return is;
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

  Nom nom_pb=ma_zone_cl_dis->equation().probleme().que_suis_je();
  const Champ_base& rho=ma_zone_cl_dis->equation().milieu().masse_volumique().valeur();
  if ((nom_pb.debute_par("Probleme_Interface")|| nom_pb==Nom("Probleme_Thermo_Front_Tracking"))||!sub_type(Champ_Uniforme,rho)
      ||nom_pb==Nom("Pb_Conduction") || nom_pb.debute_par("Pb_Conduction_Combustible")
      ||ma_zone_cl_dis->equation().que_suis_je()=="Convection_Diffusion_Concentration")
    {
      // Pour le front tracking, QC, et conduc_variable, on ne divise pas par Rho*Cp:
      divise_par_rho_cp_=false;
    }
}

double Echange_couplage_thermique::champ_exterieur(int i,int j, const Champ_front& champ_ext) const
{
  if (divise_par_rho_cp_)
    {
      const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
      const Champ_base& rho=mil.masse_volumique().valeur();
      const Champ_Don& Cp=mil.capacite_calorifique();
      double d_rho = sub_type(Champ_Uniforme,rho) ? rho(0,0) : rho(i);
      double d_Cp = sub_type(Champ_Uniforme,Cp.valeur()) ? Cp(0,0) : Cp.valeur()(i);
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
