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
// File:        Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re.h>
#include <Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Param.h>

Implemente_instanciable(Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re,"Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re",Modele_turbulence_scal_Fluctuation_Temperature_W);

//// printOn
//

Sortie& Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


//// readOn
//

Entree& Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::readOn(Entree& is )
{
  dt_impr_nusselt_=DMAXFLOAT;

  eqn.typer("Transport_Fluctuation_Temperature_W_Bas_Re");
  eqn_transport_Fluctu_Temp = ref_cast(Transport_Fluctuation_Temperature_W_Bas_Re, eqn.valeur());
  return Modele_turbulence_scal_base::readOn(is);
}
void  Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::set_param(Param& param)
{
  Modele_turbulence_scal_base::set_param(param);
  param.ajouter_non_std("Transport_Fluctuation_Temperature_W_Bas_Re",this);
  param.ajouter_non_std("Modele_Fonc_Bas_Reynolds_Thermique",this);
  // on ajoute pas les params de la classe mere car ce n'etait pas fait avt
}
int Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::lire_motcle_non_standard(const Motcle& mot, Entree& s)
{

  Cerr << "Lecture des parametres du modele de fluctuation thermique." << finl;
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcles les_mots(2);
  {
    les_mots[0] = "Transport_Fluctuation_Temperature_W_Bas_Re";
    les_mots[1] = "Modele_Fonc_Bas_Reynolds_Thermique";
  }
  int rang=les_mots.search(mot);
  switch(rang)
    {
    case 0:
      {
        Cerr << "Lecture de l'equation Transport_Fluctuation_Temperature_W_Bas_Re" << finl;
        eqn_transport_Fluctu_Temp->associer_modele_turbulence(*this);
        s >> eqn_transport_Fluctu_Temp.valeur();
        break;
      }
    case 1:
      {
        Cerr << "Lecture du modele bas reynolds associe " << finl;
        mon_modele_fonc.associer_eqn(eqn_transport_Fluctu_Temp.valeur());
        s >> mon_modele_fonc;
        mon_modele_fonc.valeur().discretiser();
        Cerr << "mon_modele_fonc.que_suis_je() " << mon_modele_fonc.valeur().que_suis_je() << finl;
        break;
      }

    default :
      {
        return  Modele_turbulence_scal_base::lire_motcle_non_standard(mot,s);
        break;
      }
    }
  return 1;
}

int Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::preparer_calcul()
{
  eqn_transport_Fluctu_Temp->preparer_calcul();
  Modele_turbulence_scal_base::preparer_calcul();
  calculer_diffusivite_turbulente();
  la_diffusivite_turbulente.valeurs().echange_espace_virtuel();
  return 1;
}

Champ_Fonc& Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::calculer_diffusivite_turbulente()
{

  DoubleTab& alpha_t = la_diffusivite_turbulente.valeurs();
  const DoubleTab& mu_t = la_viscosite_turbulente->valeurs();
  double temps = la_viscosite_turbulente->temps();
  const DoubleTab& chFluctuTemp = eqn_transport_Fluctu_Temp->inconnue().valeurs();
  const Zone_dis& la_zone_dis = eqn_transport_Fluctu_Temp->zone_dis();

  const Probleme_base& mon_pb = mon_equation->probleme();
  const Equation_base& eqn_hydr = mon_pb.equation(0);
  const RefObjU& modele_turbulence = eqn_hydr.get_modele(TURBULENCE);
  const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& mod_turb_hydr = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,modele_turbulence.valeur());
  const Transport_K_Eps_base& eqBasRe = mod_turb_hydr.eqn_transp_K_Eps();
  const DoubleTab& K_eps_Bas_Re = eqBasRe.inconnue().valeurs();

  //on recupere les proprietes physiques du fluide : viscosite cinematique et diffusivite
  const Fluide_Incompressible& fluide = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = fluide.viscosite_cinematique();
  const Champ_Don& ch_diffu = fluide.diffusivite();
  const DoubleTab& tab_visco = ch_visco_cin->valeurs();
  const DoubleTab& tab_diffu = ch_diffu->valeurs();
  double visco,diffu;

  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
    }
  else
    {
      visco=-1;
      Cerr << "La viscosite doit etre uniforme !!!!" << finl;
      exit();
    }

  if (sub_type(Champ_Uniforme,ch_diffu.valeur()))
    {
      diffu = max(tab_diffu(0,0),DMINFLOAT);
    }
  else
    {
      diffu=-1;
      Cerr << "La diffusivite doit etre uniforme !!!!" << finl;
      exit();
    }

  int nb_elem = K_eps_Bas_Re.dimension(0);
  DoubleTab Flambda(nb_elem);
  mon_modele_fonc.Calcul_Flambda( Flambda,la_zone_dis,K_eps_Bas_Re,chFluctuTemp,visco,diffu);

  if (temps != la_diffusivite_turbulente.temps())
    {
      static const double C_Lambda = 0.11;
      int n= alpha_t.size();
      if (mu_t.size() != n)
        {
          Cerr << "Les DoubleTab des champs diffusivite_turbulente et viscosite_turbulente" << finl;
          Cerr << "doivent avoir le meme nombre de valeurs nodales" << finl;
          exit();
        }

      for (int i=0; i<n; i++)
        {
          if (  (K_eps_Bas_Re(i,1) > 1.e-10 ) && (chFluctuTemp(i,1) > 1.e-10) && (K_eps_Bas_Re(i,0) > 1.e-10 ) && (chFluctuTemp(i,0) > 1.e-10))
            alpha_t[i] =C_Lambda*Flambda(i)*K_eps_Bas_Re(i,0)*sqrt((K_eps_Bas_Re(i,0)/K_eps_Bas_Re(i,1))*(chFluctuTemp(i,0)/(2*chFluctuTemp(i,1))));
          else
            {
              alpha_t[i] = 0.0000001;
            }
        }
      la_diffusivite_turbulente.changer_temps(temps);
    }

  return la_diffusivite_turbulente;
}



void Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::completer()
{
  eqn_transport_Fluctu_Temp->completer();
  const Probleme_base& mon_pb = mon_equation->probleme();
  const RefObjU& modele_turbulence = mon_pb.equation(0).get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  const Champ_Fonc& visc_turb = mod_turb_hydr.viscosite_turbulente();
  associer_viscosite_turbulente(visc_turb);
}

void Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::mettre_a_jour(double temps)
{
  Schema_Temps_base& sch1 = eqn_transport_Fluctu_Temp->schema_temps();
  // Voir Schema_Temps_base::faire_un_pas_de_temps_pb_base
  eqn_transport_Fluctu_Temp->zone_Cl_dis().mettre_a_jour(temps);
  sch1.faire_un_pas_de_temps_eqn_base(eqn_transport_Fluctu_Temp.valeur());
  eqn_transport_Fluctu_Temp->mettre_a_jour(temps);
  eqn_transport_Fluctu_Temp->controler_grandeur();
  calculer_diffusivite_turbulente();
  la_diffusivite_turbulente.valeurs().echange_espace_virtuel();
}

const Champ_base& Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::get_champ(const Motcle& nom) const
{
  REF(Champ_base) ref_champ;
  try
    {
      return Modele_turbulence_scal_Fluctuation_Temperature_W::get_champ(nom);
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

void Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Modele_turbulence_scal_Fluctuation_Temperature_W::get_noms_champs_postraitables(nom,opt);

  if (mon_modele_fonc.non_nul())
    mon_modele_fonc->get_noms_champs_postraitables(nom,opt);
}
