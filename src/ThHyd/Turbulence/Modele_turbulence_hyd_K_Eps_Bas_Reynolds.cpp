/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Modele_turbulence_hyd_K_Eps_Bas_Reynolds.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <Schema_Temps_base.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Schema_Temps.h>
#include <Debog.h>
#include <stat_counters.h>
#include <Param.h>

Implemente_instanciable(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,"Modele_turbulence_hyd_K_Epsilon_Bas_Reynolds",Mod_turb_hyd_RANS);

//
// printOn et readOn

Sortie& Modele_turbulence_hyd_K_Eps_Bas_Reynolds::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

// Description:
// Simple appel a Mod_turb_hyd_RANS::readOn(Entree&)
Entree& Modele_turbulence_hyd_K_Eps_Bas_Reynolds::readOn(Entree& is)
{
  return Mod_turb_hyd_RANS::readOn(is);
}

void Modele_turbulence_hyd_K_Eps_Bas_Reynolds::set_param(Param& param)
{
  Mod_turb_hyd_RANS::set_param(param);
  param.ajouter_non_std("Transport_K_Epsilon_Bas_Reynolds",(this),Param::REQUIRED);
  param.ajouter_non_std("Modele_Fonc_Bas_Reynolds",(this),Param::REQUIRED);
}

int Modele_turbulence_hyd_K_Eps_Bas_Reynolds::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="Transport_K_Epsilon_Bas_Reynolds")
    {
      eqn_transp_K_Eps().associer_modele_turbulence(*this);
      is >> eqn_transp_K_Eps();
      Cerr << "K_Epsilon equation type " <<  eqn_transp_K_Eps().que_suis_je() << finl;
      return 1;
    }
  else if (mot=="Modele_Fonc_Bas_Reynolds")
    {
      mon_modele_fonc.associer_eqn(eqn_transp_K_Eps());
      is >> mon_modele_fonc;
      mon_modele_fonc.valeur().discretiser();
      Cerr << "Low Reynolds number model type " << mon_modele_fonc.valeur().que_suis_je() << finl;
      return 1;
    }
  else
    return Mod_turb_hyd_RANS::lire_motcle_non_standard(mot,is);
  return 1;
}

Champ_Fonc& Modele_turbulence_hyd_K_Eps_Bas_Reynolds::calculer_viscosite_turbulente(double temps)
{

  const Champ_base& chK_Eps=eqn_transp_K_Eps().inconnue().valeur();
  const Zone_dis& la_zone_dis = eqn_transp_K_Eps().zone_dis();
  Nom type=chK_Eps.que_suis_je();
  const DoubleTab& tab_K_Eps = chK_Eps.valeurs();
  Debog::verifier("Modele_turbulence_hyd_K_Eps_Bas_Reynolds::calculer_viscosite_turbulente K_Eps",tab_K_Eps);
  DoubleTab& visco_turb =  la_viscosite_turbulente.valeurs();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_transp_K_Eps().milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ch_visco_cin->valeurs();
  int n = tab_K_Eps.dimension(0);
  DoubleTab Fmu(n);
  double LeEPSMIN = get_LeEPS_MIN();
  // K_Eps(i,0) = K au noeud i
  // K_Eps(i,1) = Epsilon au noeud i

  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      double visco;
      visco = max(tab_visco(0,0),DMINFLOAT);
      mon_modele_fonc.Calcul_Fmu( Fmu,la_zone_dis,tab_K_Eps,visco);
    }
  else
    {
      mon_modele_fonc.Calcul_Fmu( Fmu,la_zone_dis,tab_K_Eps,tab_visco);
    }

  Debog::verifier("Modele_turbulence_hyd_K_Eps_Bas_Reynolds::calculer_viscosite_turbulente Fmu",Fmu);

  //
  //  limiteur Durbin
  //
  //        double T_durbin, T_kolmo, T_ke;
  // dans le cas d'une zone nulle on doit effectuer le dimensionnement
  double non_prepare=1;
  if (visco_turb.size() == n)
    non_prepare=0.;
  non_prepare=mp_max(non_prepare);

  if (non_prepare==1)
    {
      Champ_Inc visco_turb_au_format_K_eps_Bas_Re;
      visco_turb_au_format_K_eps_Bas_Re.typer(type);
      Champ_Inc_base& ch_visco_turb_K_eps_Bas_Re=visco_turb_au_format_K_eps_Bas_Re.valeur();
      ch_visco_turb_K_eps_Bas_Re.associer_zone_dis_base(eqn_transp_K_Eps().zone_dis().valeur());
      ch_visco_turb_K_eps_Bas_Re.nommer("diffusivite_turbulente");
      ch_visco_turb_K_eps_Bas_Re.fixer_nb_comp(1);
      ch_visco_turb_K_eps_Bas_Re.fixer_nb_valeurs_nodales(n);
      ch_visco_turb_K_eps_Bas_Re.fixer_unite("inconnue");
      ch_visco_turb_K_eps_Bas_Re.changer_temps(0.);

      DoubleTab& visco_turb_K_eps_Bas_Re =  ch_visco_turb_K_eps_Bas_Re.valeurs();
      if(visco_turb_K_eps_Bas_Re.size() != n)
        {
          Cerr << "visco_turb_K_eps_Bas_Re size is " << visco_turb_K_eps_Bas_Re.size()
               << " instead of " << n << finl;
          exit();
        }

      for (int i=0; i<n; i++)
        {
          if (tab_K_Eps(i,1) <= LeEPSMIN)
            visco_turb_K_eps_Bas_Re[i] = 0;
          else
            visco_turb_K_eps_Bas_Re[i] = CMU*Fmu(i)*tab_K_Eps(i,0)*tab_K_Eps(i,0)/tab_K_Eps(i,1);
        }

      la_viscosite_turbulente->affecter(visco_turb_au_format_K_eps_Bas_Re.valeur());

    }
  else
    {
      for (int i=0; i<n; i++)
        {
          if (inf_ou_egal(tab_K_Eps(i,1),LeEPSMIN))
            visco_turb[i] = 0;
          else
            visco_turb[i] = CMU*Fmu(i)*tab_K_Eps(i,0)*tab_K_Eps(i,0)/tab_K_Eps(i,1);
        }
    }
  la_viscosite_turbulente.changer_temps(temps);
  return la_viscosite_turbulente;
}

int Modele_turbulence_hyd_K_Eps_Bas_Reynolds::preparer_calcul()
{
  return eqn_transp_K_Eps().preparer_calcul();
}

void Modele_turbulence_hyd_K_Eps_Bas_Reynolds::mettre_a_jour(double temps)
{
  Champ_Inc& ch_K_Eps = K_Eps();
  Schema_Temps_base& sch = eqn_transp_K_Eps().schema_temps();
  // Voir Schema_Temps_base::faire_un_pas_de_temps_pb_base
  eqn_transp_K_Eps().zone_Cl_dis().mettre_a_jour(temps);
  sch.faire_un_pas_de_temps_eqn_base(eqn_transp_K_Eps());
  eqn_transp_K_Eps().mettre_a_jour(temps);

  statistiques().begin_count(nut_counter_);
  eqn_transp_K_Eps().controler_K_Eps();
  calculer_viscosite_turbulente(ch_K_Eps.temps());
  limiter_viscosite_turbulente();
  la_viscosite_turbulente.valeurs().echange_espace_virtuel();
  Debog::verifier("Modele_turbulence_hyd_K_Eps_Bas_Reynolds::mettre_a_jour apres calculer_viscosite_turbulente la_viscosite_turbulente",la_viscosite_turbulente.valeurs());
  statistiques().end_count(nut_counter_);
}

bool Modele_turbulence_hyd_K_Eps_Bas_Reynolds::initTimeStep(double dt)
{
  return eqn_transport_K_Eps_Bas_Re.initTimeStep(dt);
}



void Modele_turbulence_hyd_K_Eps_Bas_Reynolds::imprimer(Sortie& os) const
{
}
const Equation_base& Modele_turbulence_hyd_K_Eps_Bas_Reynolds::equation_k_eps(int i) const
{
  assert ((i==0));
  return eqn_transport_K_Eps_Bas_Re;
}

const Champ_base& Modele_turbulence_hyd_K_Eps_Bas_Reynolds::get_champ(const Motcle& nom) const
{
  REF(Champ_base) ref_champ;
  try
    {
      return Mod_turb_hyd_RANS::get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }

  if (mon_modele_fonc.non_nul())
    {
      try
        {
          return mon_modele_fonc->get_champ(nom);
        }
      catch (Champs_compris_erreur)
        {
        }
    }

  throw Champs_compris_erreur();
  return ref_champ;
}

void Modele_turbulence_hyd_K_Eps_Bas_Reynolds::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Mod_turb_hyd_RANS::get_noms_champs_postraitables(nom,opt);

  if (mon_modele_fonc.non_nul())
    mon_modele_fonc->get_noms_champs_postraitables(nom,opt);

}

void Modele_turbulence_hyd_K_Eps_Bas_Reynolds::completer()
{
  eqn_transp_K_Eps().completer();
}
