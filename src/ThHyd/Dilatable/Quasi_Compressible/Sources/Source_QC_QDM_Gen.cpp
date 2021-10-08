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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Source_QC_QDM_Gen.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/Sources
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_QC_QDM_Gen.h>
#include <Equation_base.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Champ_Uniforme.h>
#include <Modifier_pour_fluide_dilatable.h>
#include <Fluide_Quasi_Compressible.h>
#include <DoubleTrav.h>
#include <Matrice_Morse.h>

Implemente_instanciable(Source_QC_QDM_Gen,"Source_QC_QDM_Gen_QC",Source_base);

Sortie& Source_QC_QDM_Gen::printOn(Sortie& os) const { return os; }

Entree& Source_QC_QDM_Gen::readOn(Entree& is)
{
  Nom typ_complet;
  is >> typ_complet;
  readOn_spec( is,typ_complet);
  Cerr<<" Source_QC_QDM_Gen Pas teste encore.... on s'arrete "<<finl;
  Process::exit();
  return is;
}

// Desciption: methode appele par les classes filles
// pour typer le terme source
Entree& Source_QC_QDM_Gen::readOn_spec(Entree& is,Nom& typ)
{
  source_incompressible.typer_direct(typ);
  source_incompressible->associer_eqn(mon_equation.valeur());
  is >> source_incompressible.valeur();
  return is;
}

DoubleTab& Source_QC_QDM_Gen::ajouter(DoubleTab& resu) const
{
  DoubleTrav trav(resu);
  source_incompressible.ajouter(trav);
  // multiplication par rho de trav
  multiplier_par_rho_si_dilatable(trav,equation().milieu());
  resu+=trav;
  return resu;
}
// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Mise a jour en temps du terme source.
// Precondition:
// Parametre: double
//    Signification: le pas de temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Source_QC_QDM_Gen::mettre_a_jour(double t)
{
  source_incompressible->mettre_a_jour(t);
}

// Description:
//    Met a jour les references internes a l'objet Source_QC_QDM_Gen.
//    Appelle 2 methodes virtuelles pures protegees:
//       Source_QC_QDM_Gen::associer_zones(const Zone_dis& ,const Zone_Cl_dis&)
//       Source_QC_QDM_Gen::associer_pb(const Probleme_base&)
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
// Postcondition: la source est liee a tous les objets concernes
void Source_QC_QDM_Gen::completer()
{
  Source_base::completer();
  source_incompressible->completer();
}

// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Mise a jour en temps du terme source.
// Precondition:
// Parametre: double
//    Signification: le pas de temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Source_QC_QDM_Gen::impr(Sortie& os) const
{
  return source_incompressible->impr(os);
}
DoubleTab&  Source_QC_QDM_Gen::calculer(DoubleTab& resu) const
{
  resu=0.;
  ajouter(resu);
  return resu;
}
void  Source_QC_QDM_Gen::associer_zones(const Zone_dis& z ,const Zone_Cl_dis& zcl )
{
  source_incompressible->associer_zones_public(z,zcl);
}
void  Source_QC_QDM_Gen::associer_pb(const Probleme_base& pb )
{
  if (!sub_type(Fluide_Quasi_Compressible,pb.equation(0).milieu()))
    {
      Cerr<<que_suis_je() <<" n'est a utiliser qu'en Quasi Compressible" <<finl;
      Process::exit();
    }
}

void Source_QC_QDM_Gen::creer_champ(const Motcle& motlu)
{
  source_incompressible->creer_champ(motlu);
}
const Champ_base& Source_QC_QDM_Gen::get_champ(const Motcle& nom) const
{
  return source_incompressible->get_champ(nom);
}
void Source_QC_QDM_Gen::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  source_incompressible->get_noms_champs_postraitables(nom,opt);
}
