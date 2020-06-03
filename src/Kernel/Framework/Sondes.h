/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Sondes.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sondes_included
#define Sondes_included




#include <List.h>
#include <Sonde.h>


Declare_liste(Sonde);

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Sondes
//     Cette classe represente une liste de sondes.
// .SECTION voir aussi
//     Sonde Postraitement
//////////////////////////////////////////////////////////////////////////////
class Sondes : public LIST(Sonde)
{

  Declare_instanciable(Sondes);

public:

  inline void ouvrir_fichiers();
  inline void fermer_fichiers();
  inline void completer();
  inline void init_bords();
  void associer_post(const Postraitement&);
  void postraiter();
  void mettre_a_jour(double temps, double tinit);
  void lire_fichier(const Nom& nom_fichier);

private:

  REF(Postraitement) mon_post;
};

// Description:
//   Complete la lecture des sondes
//
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
// Postcondition: les fichiers associes des sondes sont ouverts
inline void Sondes::completer()
{
  LIST_CURSEUR(Sonde) curseur=*this;
  while(curseur)
    {
      curseur->completer();
      ++curseur;
    }
}

// Description:
//   Complete la lecture des sondes
//
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
inline void Sondes::init_bords()
{
  LIST_CURSEUR(Sonde) curseur=*this;
  while(curseur)
    {
      curseur->init_bords();
      ++curseur;
    }
}

// Description:
//    Ouvre tous les fichiers associes a chacune des sondes
//    de la liste.
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
// Postcondition: les fichiers associes des sondes sont ouverts
inline void Sondes::ouvrir_fichiers()
{
  LIST_CURSEUR(Sonde) curseur=*this;
  while(curseur)
    {
      curseur->ouvrir_fichier();
      ++curseur;
    }
}


// Description:
//    Ferme tous les fichiers des sondes de la liste.
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
// Postcondition: les fichiers associes des sondes sont fermes
inline void Sondes::fermer_fichiers()
{
  LIST_CURSEUR(Sonde) curseur=*this;
  while(curseur)
    {
      curseur->fermer_fichier();
      ++curseur;
    }
}

#endif
