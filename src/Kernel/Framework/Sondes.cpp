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
// File:        Sondes.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Sondes.h>

Implemente_liste(Sonde);
Implemente_instanciable(Sondes,"Sondes",LIST(Sonde));



// Description:
//    Lit une liste de sondes a partir d'un flot d'entree
//    Format:
//    {
//      [LIRE UNE SONDE AUTANT DE FOIS QUE NECESSAIRE]
//    }
// Precondition: la liste des sondes doit avoir un postraitement associe
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
Entree& Sondes::readOn(Entree& s )
{
  assert(mon_post.non_nul());

  Motcle motlu;
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");

  s >> motlu;

  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading the probes in the postprocessing" << finl;
      Cerr << "We expected { to start to read the probes" << finl;
      exit();
    }
  s >> motlu;

  if (motlu == accolade_fermee)
    {
      Cerr << "Error while reading the probes in the postprocessing" << finl;
      Cerr << "You have not defined any probe" << finl;
      exit();
    }
  while (motlu != accolade_fermee)
    {
      Sonde une_sonde(motlu);
      une_sonde.associer_post(mon_post.valeur());
      s >> une_sonde;
      add(une_sonde);
      s >> motlu;
    }
  Cerr << "End of reading probes " << finl;
  return s;
}


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Sondes::printOn(Sortie& s ) const
{
  return s ;
}


// Description:
//    Effectue le postraitement sur chacune des sondes de
//    la liste.
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
void Sondes::postraiter()
{
  LIST_CURSEUR(Sonde) curseur=*this;
  while(curseur)
    {
      curseur->postraiter();
      ++curseur;
    }
}


// Description:
//    Effectue une mise a jour en temps de chacune
//    des sondes de la liste.
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double tinit
//    Signification: le temps initial des sondes
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Sondes::mettre_a_jour(double temps, double tinit)
{
  LIST_CURSEUR(Sonde) curseur=*this;
  while(curseur)
    {
      curseur->mettre_a_jour(temps,tinit);
      ++curseur;
    }
}


// Description:
//    Associe un postraitement a la liste des sondes.
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
// Postcondition: la liste des sondes a un postraitement associe
void Sondes::associer_post(const Postraitement& post)
{
  mon_post = post;
}

