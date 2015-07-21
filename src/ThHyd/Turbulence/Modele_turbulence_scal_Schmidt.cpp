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
// File:        Modele_turbulence_scal_Schmidt.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_turbulence_scal_Schmidt.h>
#include <Modifier_pour_QC.h>
#include <Param.h>
#include <Convection_Diffusion_std.h>

Implemente_instanciable_sans_constructeur(Modele_turbulence_scal_Schmidt,"Modele_turbulence_scal_Schmidt",Mod_Turb_scal_diffturb_base);

Modele_turbulence_scal_Schmidt::Modele_turbulence_scal_Schmidt() : LeScturb(0.7) {}

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
Sortie& Modele_turbulence_scal_Schmidt::printOn(Sortie& s ) const
{

  return Mod_Turb_scal_diffturb_base::printOn(s);
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
Entree& Modele_turbulence_scal_Schmidt::readOn(Entree& is )
{
  return Mod_Turb_scal_diffturb_base::readOn(is);
}

// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Motcle&
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Modele_turbulence_scal_Schmidt::set_param(Param& param)
{
  param.ajouter("ScTurb",&LeScturb);
  Cerr << "La valeur du nombre de Schmidt turbulent est de " << LeScturb << finl;
  Modele_turbulence_scal_base::set_param(param);
}


// Description:
//    Renvoie 1 si le mot cle passe en parametre
//    est un nom de champ de l'objet
// Precondition:
// Parametre: Motcle& mot
//    Signification: le mot cle a comparer aux noms de champs connus
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: 0 si le mot n'est pas un nom de champ
//                   1 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Modele_turbulence_scal_Schmidt::comprend_champ(const Motcle& mot) const
{
  if (mot == Motcle("diffusion_turbulente"))
    return 1;
  else
    return 0;
}


// Description:
//    Renvoie 1 si un champ fonction (Champ_Fonc) du nom specifie
//    est porte par le modele de turbulence.
//    Renvoie 0 sinon.
// Precondition:
// Parametre: Motcle& mot
//    Signification: le nom d'un champ fonction du modele de turbulence
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: REF(Champ_base)& ch_ref
//    Signification: la reference sur le champ recherche (si il a ete trouve)
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: int
//    Signification: 1 si un champ fonction du nom specifie a ete trouve
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Modele_turbulence_scal_Schmidt::a_pour_Champ_Fonc(const Motcle& mot,
                                                      REF(Champ_base)& ch_ref) const
{
  if (mot == Motcle("diffusion_turbulente"))
    {
      ch_ref = la_diffusivite_turbulente.valeur();
      return 1;
    }
  return 0;
}


// Description:
//    Calcule le coefficient turbulent utilise dans l equation et
//    la loi de paroi.
// Precondition:
// Parametre: double
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: NON ACCEDE
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Modele_turbulence_scal_Schmidt::mettre_a_jour(double )
{
  calculer_diffusion_turbulente();
  const Milieu_base& mil=equation().probleme().milieu();
  diviser_par_rho_si_qc(la_diffusivite_turbulente.valeurs(),mil);
  const Turbulence_paroi_scal& lp = loi_paroi();
  if (lp.non_nul())
    {
      loipar->calculer_scal(la_diffusivite_turbulente);
    }
  DoubleTab& alpha_t = la_diffusivite_turbulente.valeurs();
  multiplier_par_rho_si_qc(alpha_t,mil);
  la_diffusivite_turbulente->valeurs().echange_espace_virtuel();
}

// Description:
//    Calcule la diffusion turbulente.
//    diffusion_turbulente = viscosite_turbulente / Sc_turbulent
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Fonc&
//    Signification: la diffusion turbulente nouvellement calculee
//    Contraintes:
// Exception: les champs diffusivite_turbulente et viscosite_turbulente
//            doivent avoir le meme nombre de valeurs nodales
// Effets de bord:
// Postcondition:
Champ_Fonc& Modele_turbulence_scal_Schmidt::calculer_diffusion_turbulente()
{
  DoubleTab& alpha_t = la_diffusivite_turbulente.valeurs();
  const DoubleTab& mu_t = la_viscosite_turbulente->valeurs();
  double temps = la_viscosite_turbulente->temps();
  int n= alpha_t.size();
  if (mu_t.size() != n)
    {
      Cerr << "Les DoubleTab des champs diffusivite_turbulente et viscosite_turbulente" << finl;
      Cerr << "doivent avoir le meme nombre de valeurs nodales" << finl;
      exit();
    }

  for (int i=0; i<n; i++)
    alpha_t[i] = mu_t[i]/LeScturb;
  la_diffusivite_turbulente.changer_temps(temps);
  return la_diffusivite_turbulente;
}
