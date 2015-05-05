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
// File:        Mod_Turb_scal_diffturb_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Mod_Turb_scal_diffturb_base.h>
#include <Mod_turb_hyd_base.h>
#include <Convection_Diffusion_std.h>

Implemente_base(Mod_Turb_scal_diffturb_base,"Mod_Turb_scal_diffturb_base",Modele_turbulence_scal_base);


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
Sortie& Mod_Turb_scal_diffturb_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


// Description:
//    Lit les specifications d'un modele de turbulence
//    a partir d'un flot d'entree.
//    Format:
//      {
//      }
//    (il n'y a rien a lire sauf les accolades)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: accolade fermante attendue
// Effets de bord:
// Postcondition:
Entree& Mod_Turb_scal_diffturb_base::readOn(Entree& is )
{
  return Modele_turbulence_scal_base::readOn(is);
}

// Description:
//    Associe une viscosite turbulente au modele
//    de turbulence.
// Precondition:
// Parametre: Champ_Fonc& visc_turb
//    Signification: le champ fonction representant la viscosite
//                   turbulente a associer.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le modele de turbulence a une viscosite associee
void Mod_Turb_scal_diffturb_base::associer_viscosite_turbulente(const Champ_Fonc& visc_turb)
{
  la_viscosite_turbulente = visc_turb;
}

// Description:
//    Complete le modele de turbulence:
//    Appelle Modele_turbulence_scal_base::completer()
//    associe la viscosite turbulente du probleme
//    au modele de turbulence.
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
void Mod_Turb_scal_diffturb_base::completer()
{
  Modele_turbulence_scal_base::completer();
  const Probleme_base& mon_pb = equation().probleme();
  const RefObjU& modele_turbulence = mon_pb.equation(0).get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  const Champ_Fonc& visc_turb = mod_turb_hydr.viscosite_turbulente();
  associer_viscosite_turbulente(visc_turb);
}

// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Entree&
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: NON ACCEDE
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Mod_Turb_scal_diffturb_base::reprendre(Entree& )
{
  return 1;
}
