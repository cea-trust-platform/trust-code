/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Convection_Diffusion_Turbulent.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Turbulent.h>
#include <Champ_Uniforme.h>
#include <Operateur_Diff_base.h>
#include <Operateur_Diff.h>
#include <Discretisation_base.h>
#include <Champ_Don.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Schema_Temps_base.h>
#include <Avanc.h>

// Description:
//    Associe le modele de turbulence a l'equation passee
//    en parametre
//    Construit le modele de turbulence a partir d'un flot d'entree,
//    et le discretise.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: Equation_base& eqn
//    Signification: l'equation auquel le modele de turbulence doit
//                   etre associe
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Convection_Diffusion_Turbulent::lire_modele(Entree& is, const Equation_base& eqn)
{
  Cerr << "Reading and typing of the turbulence model : " << finl;
  le_modele_turbulence.associer_eqn(eqn);
  is >> le_modele_turbulence;
  le_modele_turbulence.discretiser();
  return is;
}

Entree& Convection_Diffusion_Turbulent::lire_op_diff_turbulent(Entree& is, const Equation_base& eqn, Operateur_Diff& terme_diffusif)
{
  Motcle accouverte = "{" , accfermee = "}" ;
  Nom type="Op_Dift_";

  Motcle motbidon;
  is >>  motbidon;
  assert(motbidon == accouverte);
  is >>  motbidon;
  if (motbidon=="negligeable")
    {
      type+=motbidon;
      is>>motbidon;

      terme_diffusif.typer(type);
      terme_diffusif.l_op_base().associer_eqn(eqn);
      Cerr << terme_diffusif.valeur().que_suis_je() << finl;
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());

      assert(motbidon == accfermee);
    }
  else if (motbidon=="stab")
    {
      Nom disc = eqn.discretisation().que_suis_je();
      if(disc=="VEFPreP1B") disc="VEF";

      Nom nb_inc;
      nb_inc ="_";
      if ((terme_diffusif.diffusivite().nb_comp() != 1) && ((disc=="VDF")))
        nb_inc = "_Multi_inco_";

      Nom type_diff;
      if(sub_type(Champ_Uniforme,terme_diffusif.diffusivite()))
        type_diff="";
      else
        type_diff="var_";

      Nom type_inco=eqn.inconnue()->que_suis_je();

      type+=disc;
      type+=nb_inc ;
      type+=type_diff;
      type+=(type_inco.suffix("Champ_"));
      if (Objet_U::axi) type+="_Axi";
      type+="_stab";

      terme_diffusif.typer(type);
      terme_diffusif.l_op_base().associer_eqn(eqn);
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());
      is>>terme_diffusif.valeur();
    }
  else
    {
      Nom disc = eqn.discretisation().que_suis_je();
      // les operateurs C_D_Turb_T sont communs aux discretisations VEF et VEFP1B
      if(disc=="VEFPreP1B")
        disc="VEF";
      type+=disc;

      Nom nb_inc;
      nb_inc = "_";
      if ((terme_diffusif.diffusivite().nb_comp() != 1) && ((disc=="VDF")))
        nb_inc = "_Multi_inco_";

      type+= nb_inc ;

      Nom type_diff;
      if(sub_type(Champ_Uniforme,terme_diffusif.diffusivite()))
        type_diff="";
      else
        type_diff="var_";
      type+= type_diff;

      Nom type_inco=eqn.inconnue()->que_suis_je();
      type+=(type_inco.suffix("Champ_"));

      if (Objet_U::axi)
        type+="_Axi";

      terme_diffusif.typer(type);
      terme_diffusif.l_op_base().associer_eqn(eqn);
      Cerr << terme_diffusif.valeur().que_suis_je() << finl;
      terme_diffusif->associer_diffusivite(terme_diffusif.diffusivite());

      assert(motbidon == accfermee);
    }
  return is;
}

// Description:
//    Complete le modele de turbulence.
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
void Convection_Diffusion_Turbulent::completer()
{
  le_modele_turbulence.completer();
  le_modele_turbulence->loi_paroi()->completer();

}

bool Convection_Diffusion_Turbulent::initTimeStep(double dt)
{
  return le_modele_turbulence->initTimeStep(dt);
}


// Description:
//    Prepare le calcul.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Convection_Diffusion_Turbulent::preparer_calcul()
{
  le_modele_turbulence.preparer_calcul();
  return 1;
}


// Description:
//    Simple appel a Modele_turbulence_scal::sauvegarder(Sortie&)
//    sur le membre concerne.
//    Sauvegarde le modele de turbulence sur un flot
//    de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Convection_Diffusion_Turbulent::sauvegarder(Sortie& os) const
{
  return le_modele_turbulence.sauvegarder(os);
}


// Description:
//    Reprise (apres une sauvegarde) a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception: fin de fichier atteinte pendant la reprise
// Effets de bord:
// Postcondition:
int Convection_Diffusion_Turbulent::reprendre(Entree& is)
{
  le_modele_turbulence.reprendre(is);
  return 1;
}


// Description:
//    Mise a jour en temps du modele de turbulence.
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Convection_Diffusion_Turbulent::mettre_a_jour(double temps)
{
  le_modele_turbulence.mettre_a_jour(temps);
}

