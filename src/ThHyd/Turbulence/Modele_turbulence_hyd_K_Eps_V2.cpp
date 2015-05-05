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
// File:        Modele_turbulence_hyd_K_Eps_V2.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/30
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_turbulence_hyd_K_Eps_V2.h>
#include <EChaine.h>
#include <Discretisation_base.h>
#include <DoubleTrav.h>
#include <Param.h>

Implemente_base(Modele_turbulence_hyd_K_Eps_V2,"Modele_turbulence_hyd_K_Epsilon_V2",Mod_turb_hyd_RANS);

// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
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
Sortie& Modele_turbulence_hyd_K_Eps_V2::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


// Description:
//   Appel a Mod_turb_hyd_RANS::readOn(Entree&)
//   et lecture imposee pour loi de paroi
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
Entree& Modele_turbulence_hyd_K_Eps_V2::readOn(Entree& is )
{
  if (Process::nproc()>1)
    {
      Cerr << "The K_epsilon_V2 model is not parallelized." << finl;
      exit();
    }
  Mod_turb_hyd_RANS::readOn(is);
  EChaine cha("loi_standard_hydr");
  loipar.associer_modele(*this);
  cha >> loipar;
  return is;
}

void Modele_turbulence_hyd_K_Eps_V2::set_param(Param& param)
{
  Mod_turb_hyd_RANS::set_param(param);
  param.ajouter_non_std("Transport_K_Epsilon_V2",(this),Param::REQUIRED);
  param.ajouter_non_std("Transport_V2",(this),Param::REQUIRED);
  param.ajouter_non_std("EqnF22",(this),Param::REQUIRED);
}

int Modele_turbulence_hyd_K_Eps_V2::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="Transport_K_Epsilon_V2")
    {
      eqn_transp_K_Eps().associer_modele_turbulence(*this);
      is >> eqn_transp_K_Eps();
      Cerr << "K_Epsilon equation type " << eqn_transp_K_Eps().que_suis_je() << finl;
      return 1;
    }
  else if (mot=="Transport_V2")
    {
      eqn_transport_V2.associer_modele_turbulence(*this);
      is >> eqn_transport_V2;
      Cerr << "V2 equation type " << eqn_transport_V2.que_suis_je() << finl;
      return 1;
    }
  else if (mot=="EqnF22")
    {
      Motcle typ;
      Motcle nom1("EqnF22");
      Nom discr=equation().discretisation().que_suis_je();
      nom1+=discr;
      eqn_f22.typer(nom1);
      eqn_f22->associer_modele_turbulence(*this);
      is >> eqn_f22.valeur();
      Cerr << "F22 equation type " << eqn_f22.que_suis_je() << finl;
      return 1;
    }
  else
    return Mod_turb_hyd_RANS::lire_motcle_non_standard(mot,is);
  return 1;
}

int Modele_turbulence_hyd_K_Eps_V2::preparer_calcul()
{
  Cerr << " Method Modele_turbulence_hyd_K_Eps_V2::preparer_calcul() " << finl;
  eqn_f22.preparer_calcul();
  eqn_transport_V2.preparer_calcul();
  return eqn_transp_K_Eps().preparer_calcul();
}

bool Modele_turbulence_hyd_K_Eps_V2::initTimeStep(double dt)
{
  bool ok= eqn_transport_V2.initTimeStep(dt);
  ok = ok && eqn_transport_K_Eps_V2.initTimeStep(dt);
  ok = ok && eqn_f22.valeur().initTimeStep(dt);
  return ok;
}

// Description:
//    Simple appel a Transport_K_Eps::completer()
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
void Modele_turbulence_hyd_K_Eps_V2::completer()
{
  eqn_transp_K_Eps().completer();
  eqn_transport_V2.completer();
  eqn_f22.valeur().completer();
  verifie_loi_paroi();
}


// Description:
//    Sauvegarde le modele de turbulence sur un flot de sortie.
//    (en vue d'une reprise)
//    Sauvegarde le type de l'objet et
//    l'equation de transport K-epsilon associee.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage de:
//                   Transport_K_Eps::sauvegarder(Sortie&)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Modele_turbulence_hyd_K_Eps_V2::sauvegarder(Sortie& os) const
{

  Mod_turb_hyd_base::a_faire(os);
  int bytes=0;
  bytes += eqn_f22.sauvegarder(os);
  bytes += eqn_transport_V2.sauvegarder(os);
  bytes += eqn_transp_K_Eps().sauvegarder(os);
  return bytes;
}


// Description:
//    Reprise du modele a partir d'un flot d'entree.
//    Si l'equation portee par l'objet est non nulle
//    on effectue une reprise "bidon".
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage de:
//                   Transport_K_Eps::sauvegarder(Sortie&)
//                   ou 1 si la reprise est bidon.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Modele_turbulence_hyd_K_Eps_V2::reprendre(Entree& is)
{
  Mod_turb_hyd_base::reprendre(is);
  if (mon_equation.non_nul())
    {
      eqn_f22.reprendre(is);
      eqn_transport_V2.reprendre(is);
      return eqn_transp_K_Eps().reprendre(is);
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


const Equation_base& Modele_turbulence_hyd_K_Eps_V2::equation_k_eps(int i) const
{
  assert ((i==0) || (i==1) || (i==2));
  if (i==0)
    return eqn_transport_K_Eps_V2;
  else if (i==1)
    return  eqn_transport_V2;
  else
    return  eqn_f22.valeur();

}
