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
// File:        Mod_turb_hyd_combin.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Mod_turb_hyd_combin.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Zone_VF.h>
#include <DoubleTabs.h>
#include <Modifier_nut_pour_QC.h>
#include <stat_counters.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Mod_turb_hyd_combin,"Modele_turbulence_hyd_combinaison",Mod_turb_hyd_base);

Mod_turb_hyd_combin::Mod_turb_hyd_combin()
{
  nb_var_=0;
  les_var.dimensionner(0);
}

// Description:
//    Simple appel a Mod_turb_hyd_base::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& is
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Mod_turb_hyd_combin::printOn(Sortie& is) const
{
  return Mod_turb_hyd_base::printOn(is);
}


// Description:
//    Simple appel a Mod_turb_hyd_base::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Mod_turb_hyd_combin::readOn(Entree& is)
{
  Mod_turb_hyd_base::readOn(is);
  return is;
}

void Mod_turb_hyd_combin::set_param(Param& param)
{
  Mod_turb_hyd_base::set_param(param);
  param.ajouter("nb_var",&les_var);
  param.ajouter("fonction",&la_fct,Param::REQUIRED);
}

void Mod_turb_hyd_combin::discretiser()
{
  Mod_turb_hyd_base::discretiser();
  discretiser_K(mon_equation->schema_temps(),mon_equation->zone_dis(),energie_cinetique_turb_);
}

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
void Mod_turb_hyd_combin::completer()
{
  nb_var_ = les_var.size();
  fxyz.dimensionner(1);
  fxyz[0].setNbVar(4+nb_var_);
  fxyz[0].setString(la_fct);
  fxyz[0].addVar("x");
  fxyz[0].addVar("y");
  fxyz[0].addVar("z");
  fxyz[0].addVar("t");
  for (int i=0; i<nb_var_; i++)
    fxyz[0].addVar(les_var[i]);

  fxyz[0].parseString();
}

int Mod_turb_hyd_combin::preparer_calcul()
{
  Mod_turb_hyd_base::preparer_calcul();
  mettre_a_jour(0);
  return 1;
}

void Mod_turb_hyd_combin::mettre_a_jour(double temps)
{
  statistiques().begin_count(nut_counter_);
  calculer_viscosite_turbulente();
  loipar->calculer_hyd(la_viscosite_turbulente,energie_cinetique_turbulente());
  limiter_viscosite_turbulente();
  Correction_nut_et_cisaillement_paroi_si_qc(*this);
  la_viscosite_turbulente->valeurs().echange_espace_virtuel();
  statistiques().end_count(nut_counter_);
}

Champ_Fonc& Mod_turb_hyd_combin::calculer_viscosite_turbulente()
{
  const Zone_VF& zone_VF = ref_cast(Zone_VF,equation().zone_dis().valeur());
  const DoubleTab& xp = zone_VF.xp();
  DoubleTab& viscosite_valeurs =  la_viscosite_turbulente.valeurs();
  const Probleme_base& mon_pb = equation().probleme();
  double temps = equation().inconnue().temps();
  int nb_ddl = zone_VF.nb_elem();
  double x=0;
  double y=0;
  double z=0;
  DoubleTabs sources_val(nb_var_);

  for (int so=0; so<nb_var_; so++)
    {
      REF(Champ_base) ch_ref;
      ch_ref = mon_pb.get_champ(les_var[so]);
      const DoubleTab& source_so_val = ch_ref->valeurs();
      sources_val[so] = source_so_val;
    }


  for (int i=0; i<nb_ddl; i++)
    {
      x=xp(i,0);
      y=xp(i,1);
      z=0;
      if (dimension >2 )
        z = xp(i,2);

      fxyz[0].setVar("x",x);
      fxyz[0].setVar("y",y);
      fxyz[0].setVar("z",z);
      fxyz[0].setVar("t",temps);

      for (int so=0; so<nb_var_; so++)
        {
          const Nom nom_source = les_var[so];;
          const DoubleTab& source_so_val = sources_val[so];
          fxyz[0].setVar(nom_source,source_so_val(i));
        }
      viscosite_valeurs(i) = fxyz[0].eval();
    }
  la_viscosite_turbulente.changer_temps(temps);

  return la_viscosite_turbulente;
}
