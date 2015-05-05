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
// File:        Mod_turb_hyd_ss_maille.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Mod_turb_hyd_ss_maille.h>
#include <Equation_base.h>
#include <Param.h>
#include <Modifier_nut_pour_QC.h>
#include <Debog.h>
#include <stat_counters.h>

Implemente_base_sans_constructeur(Mod_turb_hyd_ss_maille,"Mod_turb_hyd_ss_maille",Mod_turb_hyd_base);

Mod_turb_hyd_ss_maille::Mod_turb_hyd_ss_maille()
{
  methode="volume"; // Parametre par defaut pour calculer la longueur caracteristique
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
Sortie& Mod_turb_hyd_ss_maille::printOn(Sortie& is) const
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
Entree& Mod_turb_hyd_ss_maille::readOn(Entree& is)
{
  Mod_turb_hyd_base::readOn(is);
  return is;
}

void Mod_turb_hyd_ss_maille::set_param(Param& param)
{
  Mod_turb_hyd_base::set_param(param);
  param.ajouter("longueur_maille",&methode);
}

int Mod_turb_hyd_ss_maille::preparer_calcul()
{
  Mod_turb_hyd_base::preparer_calcul();
  if (methode!="??")
    Cerr << "The characteristic length of the subgrid scale model is calculated with the method : " << methode << finl;
  calculer_longueurs_caracteristiques();
  mettre_a_jour(0.);
  return 1;
}

// Description:
//    Discretise le modele de turbulence.
//    Appelle Mod_turb_hyd_base::discretiser()
//    Initialise les integrales statistiques.
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
void Mod_turb_hyd_ss_maille::discretiser()
{
  Mod_turb_hyd_base::discretiser();
  discretiser_K(mon_equation->schema_temps(),mon_equation->zone_dis(),energie_cinetique_turb_);
  champs_compris_.ajoute_champ(energie_cinetique_turb_);
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
void Mod_turb_hyd_ss_maille::completer()
{
}


void Mod_turb_hyd_ss_maille::mettre_a_jour(double )
{
  statistiques().begin_count(nut_counter_);
  calculer_viscosite_turbulente();
  calculer_energie_cinetique_turb();
  loipar->calculer_hyd(la_viscosite_turbulente,energie_cinetique_turbulente());
  limiter_viscosite_turbulente();
  Correction_nut_et_cisaillement_paroi_si_qc(*this);
  energie_cinetique_turb_.valeurs().echange_espace_virtuel();
  la_viscosite_turbulente->valeurs().echange_espace_virtuel();
  statistiques().end_count(nut_counter_);
}

void Mod_turb_hyd_ss_maille::calculer_energie_cinetique_turb()
{
  DoubleVect& k = energie_cinetique_turb_.valeurs();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  double Cq = 0.094;

  // PQ : 10/08/06 : on utilise ici la formule de Schuman : q_sm = (nu_t)^2 / (Cq.l)^2

  const int nb_elem = visco_turb.size();
  if (k.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent kinetic energy." << finl;
      exit();
    }

  for (int elem=0 ; elem<nb_elem; elem++)
    k(elem)=visco_turb[elem]*visco_turb[elem]/(Cq*Cq*l_(elem)*l_(elem));

  double temps = mon_equation->inconnue().temps();
  energie_cinetique_turb_.changer_temps(temps);
}

