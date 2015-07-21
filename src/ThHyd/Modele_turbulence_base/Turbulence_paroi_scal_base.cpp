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
// File:        Turbulence_paroi_scal_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Modele_turbulence_base
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_paroi_scal_base.h>
#include <Modele_turbulence_scal_base.h>
#include <Probleme_base.h>
#include <Convection_Diffusion_std.h>
#include <Schema_Temps_base.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <Zone_VF.h>
#include <Champ_Uniforme.h>
#include <Champ_Don.h>

Implemente_base_sans_constructeur(Turbulence_paroi_scal_base,"Turbulence_paroi_scal_base",Objet_U);


// Description:
//    Ecrit le type de l'objet sur un flot de sortie
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
Sortie& Turbulence_paroi_scal_base::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree& s
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Turbulence_paroi_scal_base::readOn(Entree& is)
{

  return is ;
}
void Turbulence_paroi_scal_base::creer_champ(const Motcle& motlu)
{
}

const Champ_base& Turbulence_paroi_scal_base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);
}

void Turbulence_paroi_scal_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<"Turbulence_paroi_scal_base : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());

}
// Description:
//    Ouverture/creation d'un fichier d'impression de
//    Face, d_eq, Nu local, h
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
// Postcondition: la methode ne modifie pas l'objet
void Turbulence_paroi_scal_base::ouvrir_fichier_partage(EcrFicPartage& Nusselt,const Nom& extension) const
{
  const Probleme_base& pb=mon_modele_turb_scal->equation().probleme();
  const Schema_Temps_base& sch=pb.schema_temps();

  Nom fichier=Objet_U::nom_du_cas()+"_"+pb.le_nom()+"_"+extension+".face";

  // On cree le fichier au premier pas de temps si il n'y a pas reprise
  if (nb_impr_==0 && !pb.reprise_effectuee())
    {
      Nusselt.ouvrir(fichier);
    }
  // Sinon on l'ouvre
  else
    {
      Nusselt.ouvrir(fichier,ios::app);
    }

  if(je_suis_maitre())
    {
      EcrFicPartage& fic=Nusselt;
      fic << "Temps : " << sch.temps_courant();
    }
  nb_impr_++;
}



// Description:
//    Give a boolean indicating if we need to use equivant distance
//    by default we consider that we use the equivalent distance
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: boolean
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
//
bool Turbulence_paroi_scal_base::use_equivalent_distance() const
{
  return true;
}
