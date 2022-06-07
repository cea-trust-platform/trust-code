/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <EcrFicCollecte.h>
#include <communications.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(EcrFicCollecte,"EcrFicCollecte",SFichier);
Entree& EcrFicCollecte::readOn(Entree& s)
{
  throw;
  return s;
}

Sortie& EcrFicCollecte::printOn(Sortie& s) const
{
  throw;
  return s;
}
EcrFicCollecte::~EcrFicCollecte()
{
  SFichier::close();
}
// Description:
//    Constructeur
//    Ouvre le fichier avec les parametres mode et prot donnes
//    Ces parametres sont les parametres de la methode open standard
// Precondition:
// Parametre: const char* name
//    Signification: nom du fichier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int mode
//    Signification: parametre passe a open
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int prot
//    Signification: parametre passe a open
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
EcrFicCollecte::EcrFicCollecte(const char* name,IOS_OPEN_MODE mode)
{
  EcrFicCollecte::ouvrir(name, mode);
}


// Description:
//    Ouvre le fichier avec les parametres mode et prot donnes
//    Ces parametres sont les parametres de la methode open standard
// Precondition:
// Parametre: const char* name
//    Signification: nom du fichier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int mode
//    Signification: parametre passe a open
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int prot
//    Signification: parametre passe a open
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int EcrFicCollecte::ouvrir(const char* name,IOS_OPEN_MODE mode)
{
  Nom nom_fic(name);
  if(Process::nproc()>1)
    nom_fic=nom_fic.nom_me(me());

#ifdef FILESYSTEM_NON_GLOBAL
  Nom localisation;
  if (je_suis_maitre())
    localisation = pwd();
  envoyer_broadcast(localisation, 0);
  nom_fic = localisation + "/" + nom_fic;
#endif

  if (je_suis_maitre())
    Cerr << "Opening the files of type EcrFicColl : " << name <<finl;
  return SFichier::ouvrir((const char *)nom_fic,mode);
}
