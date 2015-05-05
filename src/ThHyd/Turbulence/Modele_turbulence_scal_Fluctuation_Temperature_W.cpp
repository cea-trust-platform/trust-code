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
// File:        Modele_turbulence_scal_Fluctuation_Temperature_W.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_turbulence_scal_Fluctuation_Temperature_W.h>
#include <Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <Probleme_base.h>
#include <DoubleTrav.h>
#include <Param.h>

Implemente_instanciable(Modele_turbulence_scal_Fluctuation_Temperature_W,"Modele_turbulence_scal_Fluctuation_Temperature_W",Modele_turbulence_scal_base);

//// printOn
//

Sortie& Modele_turbulence_scal_Fluctuation_Temperature_W::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


//// readOn
//

Entree& Modele_turbulence_scal_Fluctuation_Temperature_W::readOn(Entree& is )
{
  eqn.typer("Transport_Fluctuation_Temperature_W");
  eqn_transport_Fluctu_Temp = ref_cast(Transport_Fluctuation_Temperature_W, eqn.valeur());

  return Modele_turbulence_scal_base::readOn(is);
}
void Modele_turbulence_scal_Fluctuation_Temperature_W::set_param(Param& param)
{
  param.ajouter_non_std("Transport_Fluctuation_Temperature_W",this);
  // on ajoute pas les params de la classe mere car ce n'etait pas fait avt
}
int Modele_turbulence_scal_Fluctuation_Temperature_W::lire_motcle_non_standard(const Motcle& mot, Entree& s)
{
  Cerr << "Lecture des parametres du modele de fluctuation thermique. Il doit y avoir deux types d'equation." << finl;
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcles les_mots(1);
  {
    les_mots[0] = "Transport_Fluctuation_Temperature_W";
  }
  int rang=les_mots.search(mot);
  switch(rang)
    {
    case 0:
      {
        Cerr << "Lecture de l'equation Transport_Fluctuation_Temperature_W" << finl;
        eqn_transport_Fluctu_Temp->associer_modele_turbulence(*this);
        s >> eqn_transport_Fluctu_Temp.valeur();
        break;
      }
    default :
      {
        Cerr << "Erreur a la lecture des donnees du modele de fluctuation thermique" << finl;
        Cerr << "On attendait les motcles Transport_Fluctuation_Temperature_W au lieu de " << mot << finl;
        exit();
        break;
      }
    }
  return 1;
}

void Modele_turbulence_scal_Fluctuation_Temperature_W::associer_viscosite_turbulente(const Champ_Fonc& visc_turb)
{
  la_viscosite_turbulente = visc_turb;
}



int Modele_turbulence_scal_Fluctuation_Temperature_W::preparer_calcul()
{
  eqn_transport_Fluctu_Temp->preparer_calcul();
  Modele_turbulence_scal_base::preparer_calcul();
  calculer_diffusivite_turbulente();
  la_diffusivite_turbulente.valeurs().echange_espace_virtuel();
  return 1;
}

bool Modele_turbulence_scal_Fluctuation_Temperature_W::initTimeStep(double dt)
{
  return eqn->initTimeStep(dt);
}

Champ_Fonc& Modele_turbulence_scal_Fluctuation_Temperature_W::calculer_diffusivite_turbulente()
{
  DoubleTab& alpha_t = la_diffusivite_turbulente.valeurs();
  const DoubleTab& mu_t = la_viscosite_turbulente->valeurs();
  double temps = la_viscosite_turbulente->temps();
  const Champ_base& chFluctuTemp = eqn_transport_Fluctu_Temp->inconnue().valeur();

  const Probleme_base& mon_pb = mon_equation->probleme();
  const RefObjU& modele_turbulence = mon_pb.equation(0).get_modele(TURBULENCE);
  const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& mod_turb_hydr = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,modele_turbulence.valeur());
  const Transport_K_Eps_base& eqBasRe = mod_turb_hydr.eqn_transp_K_Eps();
  const DoubleTab& K_eps_Bas_Re = eqBasRe.inconnue().valeurs();

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
        if (  (K_eps_Bas_Re(i,1) > 1.e-3 ) && (chFluctuTemp(i,1) > 1.e-3)  && (K_eps_Bas_Re(i,0) > 1.e-3 ) && (chFluctuTemp(i,0) > 1.e-3))
          alpha_t[i] =C_Lambda*K_eps_Bas_Re(i,0)*sqrt((K_eps_Bas_Re(i,0)/K_eps_Bas_Re(i,1))*(chFluctuTemp(i,0)/(2*chFluctuTemp(i,1))));
        else
          {
            Cerr << " !!!! Eps ou EpsTeta est nul !!!! " << finl;
            Cerr << " K = " << K_eps_Bas_Re(i,0) << finl;
            alpha_t[i] = 0.0000001;
          }

      la_diffusivite_turbulente.changer_temps(temps);
    }

  return la_diffusivite_turbulente;
}

void Modele_turbulence_scal_Fluctuation_Temperature_W::mettre_a_jour(double temps)
{
  Schema_Temps_base& sch1 = eqn_transport_Fluctu_Temp->schema_temps();
  // Voir Schema_Temps_base::faire_un_pas_de_temps_pb_base
  eqn_transport_Fluctu_Temp->zone_Cl_dis().mettre_a_jour(temps);
  sch1.faire_un_pas_de_temps_eqn_base(eqn_transport_Fluctu_Temp.valeur());
  //eqn_transport_Fluctu_Temp->inconnue().mettre_a_jour(temps);
  eqn_transport_Fluctu_Temp->mettre_a_jour(temps);
  eqn_transport_Fluctu_Temp->controler_grandeur();
  calculer_diffusivite_turbulente();
  la_diffusivite_turbulente.valeurs().echange_espace_virtuel();
}

void Modele_turbulence_scal_Fluctuation_Temperature_W::completer()
{
  Cerr << "Dans Modele_turbulence_scal_Fluctuation_Temperature_W::completer()" << finl;
  eqn_transport_Fluctu_Temp->completer();
  const Probleme_base& mon_pb = mon_equation->probleme();
  const RefObjU& modele_turbulence = mon_pb.equation(0).get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  const Champ_Fonc& visc_turb = mod_turb_hydr.viscosite_turbulente();
  associer_viscosite_turbulente(visc_turb);
  Cerr << "Fin de Modele_turbulence_scal_Fluctuation_Temperature_W::completer()" << finl;

}

int Modele_turbulence_scal_Fluctuation_Temperature_W::sauvegarder(Sortie& os) const
{

  Modele_turbulence_scal_base::a_faire(os);
  return equation_Fluctu().sauvegarder(os);
}

int Modele_turbulence_scal_Fluctuation_Temperature_W::reprendre(Entree& is)
{
  Modele_turbulence_scal_base::reprendre(is);
  if (mon_equation.non_nul())
    {
      equation_Fluctu().reprendre(is);
      return 1;
    }
  else
    {
      double dbidon;
      Nom bidon;
      DoubleTrav tab_bidon;
      is >> bidon >> bidon;
      is >> dbidon;
      tab_bidon.jump(is);
      return 1;
    }
}
void Modele_turbulence_scal_Fluctuation_Temperature_W::imprimer(Sortie& os) const
{
}

const Champ_base& Modele_turbulence_scal_Fluctuation_Temperature_W::get_champ(const Motcle& nom) const
{
  REF(Champ_base) ref_champ;
  try
    {
      return Modele_turbulence_scal_base::get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }
  try
    {
      return eqn->get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }

  throw Champs_compris_erreur();
  return ref_champ;
}

void Modele_turbulence_scal_Fluctuation_Temperature_W::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Modele_turbulence_scal_base::get_noms_champs_postraitables(nom,opt);

  eqn->get_noms_champs_postraitables(nom,opt);
}
