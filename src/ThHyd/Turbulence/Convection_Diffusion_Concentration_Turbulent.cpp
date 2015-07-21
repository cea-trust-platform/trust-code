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
// File:        Convection_Diffusion_Concentration_Turbulent.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/30
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Concentration_Turbulent.h>
#include <Constituant.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Convection_Diffusion_Concentration_Turbulent,"Convection_Diffusion_Concentration_Turbulent",Convection_Diffusion_Concentration);

// Description:
//    Simple appel a Equation_base::printOn(Sortie&)
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
Sortie& Convection_Diffusion_Concentration_Turbulent::printOn(Sortie& is) const
{
  return Equation_base::printOn(is);
}


// Description:
//  cf Convection_Diffusion_Concentration::readOn(is)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: il faut specifier un modele de turbulence
// Effets de bord:
// Postcondition:
Entree& Convection_Diffusion_Concentration_Turbulent::readOn(Entree& is)
{
  //On conserve cette initialisation ici car si on la deplace dans le constructeur
  //elle est ecrasee par Convection_Diffusion_Concentration::discretiser()
  nb_constituants_=1;
  Convection_Diffusion_Concentration::readOn(is);
  return is;
}

void Convection_Diffusion_Concentration_Turbulent::set_param(Param& param)
{
  Convection_Diffusion_Concentration::set_param(param);
  param.ajouter("constituants",&nb_constituants_);
  param.ajouter_non_std("modele_turbulence",(this),Param::REQUIRED);
}

int Convection_Diffusion_Concentration_Turbulent::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      lire_op_diff_turbulent(is, *this, terme_diffusif);
      // GF pas sur que cela soit boin
      // le champ pour le dt_stab est le meme que celui de l'operateur
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(diffusivite_pour_pas_de_temps());
      return 1;
    }
  else if (mot=="modele_turbulence")
    {
      lire_modele(is,*this);
      RefObjU le_modele;
      le_modele = le_modele_turbulence.valeur();
      liste_modeles_.add_if_not(le_modele);
      return 1;
    }
  else
    return Convection_Diffusion_Concentration::lire_motcle_non_standard(mot,is);
  return 1;
}

// Description:
//    Sauvergarde de l'equation sur un flot de sortie.
//    Double appel a:
//      Convection_Diffusion_Concentration::sauvegarder(Sortie& );
//      Convection_Diffusion_Turbulent::sauvegarder(Sortie& );
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Convection_Diffusion_Concentration_Turbulent::sauvegarder(Sortie& os) const
{
  int bytes=0;
  bytes += Convection_Diffusion_Concentration::sauvegarder(os);
  bytes += Convection_Diffusion_Turbulent::sauvegarder(os);
  return bytes;
}


// Description:
//    Reprise a partir d'un flot d'entree, double appel a:
//      Convection_Diffusion_Concentration::reprendre(Entree& );
//      Convection_Diffusion_Turbulent::reprendre(Entree&);
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Convection_Diffusion_Concentration_Turbulent::reprendre(Entree& is)
{
  Convection_Diffusion_Concentration::reprendre(is);
  Convection_Diffusion_Turbulent::reprendre(is);
  return 1;
}


// Description:
//    Double appel a:
//     Convection_Diffusion_Turbulent::completer()
//     Convection_Diffusion_Concentration::completer()
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
void Convection_Diffusion_Concentration_Turbulent::completer()
{
  Convection_Diffusion_Turbulent::completer();
  Convection_Diffusion_Concentration::completer();
}


// Description:
//    Mise a jour en temps de l'equation, double appel a:
//      Convection_Diffusion_Concentration::mettre_a_jour(double );
//      Convection_Diffusion_Turbulent::mettre_a_jour(double );
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
void Convection_Diffusion_Concentration_Turbulent::mettre_a_jour(double temps)
{
  Convection_Diffusion_Concentration::mettre_a_jour(temps);
  Convection_Diffusion_Turbulent::mettre_a_jour(temps);
}

void Convection_Diffusion_Concentration_Turbulent::creer_champ(const Motcle& motlu)
{
  Convection_Diffusion_Concentration::creer_champ(motlu);

  if (le_modele_turbulence.non_nul())
    le_modele_turbulence->creer_champ(motlu);
}

const Champ_base& Convection_Diffusion_Concentration_Turbulent::get_champ(const Motcle& nom) const
{

  try
    {
      return Convection_Diffusion_Concentration::get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }

  if (le_modele_turbulence.non_nul())
    try
      {
        return le_modele_turbulence->get_champ(nom);
      }
    catch (Champs_compris_erreur)
      {
      }
  throw Champs_compris_erreur();
  REF(Champ_base) ref_champ;
  return ref_champ;
}

void Convection_Diffusion_Concentration_Turbulent::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Convection_Diffusion_Concentration::get_noms_champs_postraitables(nom,opt);
  if (le_modele_turbulence.non_nul())
    le_modele_turbulence->get_noms_champs_postraitables(nom,opt);
}

// Description:
//    Double appel a:
//      Convection_Diffusion_Turbulent::preparer_calcul()
//      Convection_Diffusion_Concentration::preparer_calcul()
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
int Convection_Diffusion_Concentration_Turbulent::preparer_calcul()
{
  Convection_Diffusion_Turbulent::preparer_calcul();
  Convection_Diffusion_Concentration::preparer_calcul();
  return 1;
}

bool Convection_Diffusion_Concentration_Turbulent::initTimeStep(double dt)
{
  bool ok = Convection_Diffusion_Turbulent::initTimeStep(dt);
  ok = ok && Convection_Diffusion_Concentration::initTimeStep(dt);
  return ok;
}

const RefObjU& Convection_Diffusion_Concentration_Turbulent::get_modele(Type_modele type) const
{
  CONST_LIST_CURSEUR(RefObjU) curseur = liste_modeles_;
  while(curseur)
    {
      const RefObjU&  mod = curseur.valeur();
      if (mod.non_nul())
        if ((sub_type(Modele_turbulence_scal_base,mod.valeur())) && (type==TURBULENCE))
          return mod;
      ++curseur;
    }
  return Equation_base::get_modele(type);
}
