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
// File:        Champ_Fonc_Fonction.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_Fonction.h>
#include <EChaine.h>

Implemente_instanciable(Champ_Fonc_Fonction,"Champ_Fonc_Fonction",Champ_Fonc_Tabule);


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& os
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
Sortie& Champ_Fonc_Fonction::printOn(Sortie& os) const
{
  return os;
}

// Description:
//     Lecture du Champ a partir d'un flot d'entree,
//     (On ne sait traiter que les champs scalaires.)
//     exemple:
//     Champ_Fonc_Fonction ch
//     Lire ch
//     temperature  (ch est fonction de la temperature)
//     1 (nombre de composantes)
//     {
//     2
//     0 500 0 250  (ch(0)=0 && ch(500)=250
//     }
// Precondition: seul les champs scalaires sont traites
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade fermante attendue
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
Entree& Champ_Fonc_Fonction::readOn(Entree& is)
{
  int nbcomp;
  Nom val1, val2;
  is >> val1;
  is >> val2;
  Champ_Fonc_Tabule::Warn_old_chp_fonc_syntax("Champ_Fonc_Fonction", val1, val2, nbcomp, nom_champ_parametre_);
  nbcomp = lire_dimension(nbcomp,que_suis_je());
  fixer_nb_comp(nbcomp);
  Cerr<<"We read the analytic function "<<finl;
  la_table.lire_f(is, nbcomp);
  return is;
}
Implemente_instanciable_sans_constructeur(Sutherland,"Sutherland",Champ_Fonc_Fonction);

Sutherland::Sutherland()
{
  A_ = -1;
  C_ = -1;
  Tref_ = -1;
}

// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& os
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
Sortie& Sutherland::printOn(Sortie& os) const
{
  return os;
}

// Description:
//     Lecture du Champ a partir d'un flot d'entree,
//     (On ne sait traiter que les champs scalaires.)
//     exemple:
//     Sutherland ch
//     Lire ch
//     temperature  (ch est fonction de la temperature)
//     1 (nombre de composantes)
//     {
//     2
//     0 500 0 250  (ch(0)=0 && ch(500)=250
//     }
// Precondition: seul les champs scalaires sont traites
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade fermante attendue
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
Entree& Sutherland::readOn(Entree& is)
{

  Cerr<< "Sutherland::readOn usage { A a C c TREF Tref }"<<finl;
  Cerr<<"== Champ_Fonc_Fonction Temperature 1 a*(c+Tref)/(c+val)*(val/Tref)^1.5"<<finl;
  Motcle accferme="}";
  Motcle accouverte="{";

  Motcle motlu;
  is >> motlu;
  Cerr<<"Reading Sutherland"<<finl;
  if (motlu != accouverte)
    {
      Cerr<<" We expected "<<accouverte<<" instead of "<<motlu<<finl;
      abort();
    }

  //   Motcles les_mots(8);
  Motcles les_mots(3);
  {
    les_mots[0] = "A";
    les_mots[1] = "C";
    les_mots[2] = "Tref";

  }

  is >> motlu;
  while(motlu != accferme )
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          is >> A_;
          break;
        case 1 :
          is>> C_;
          break;
        case 2 :
          is>>Tref_;
          break;
        default :
          {
            Cerr<<"A "<<que_suis_je()<<" doesn't have property "<<motlu<<finl;
            Cerr<<"We expected a word in:"<<finl<<les_mots<<finl;
            abort();
          }
        }
      is >> motlu;
    }
  if ((C_==-1)||(A_==-1)||(Tref_==-1))
    {
      Cerr<<"Sutherland object "<<le_nom()<< " is incorrect"<<finl;
      if (C_==-1)
        Cerr<<"You must define C in "<<le_nom()<<finl;
      if (Tref_==-1)
        Cerr<<"You must define Tref in "<<le_nom()<<finl;
      if (A_==-1)
        Cerr<<"You must define A in "<<le_nom()<<finl;
      exit();
    }

  lire_expression();
  return is;
}

void Sutherland::lire_expression()
{
  // On cree une chaine correspondant a Sutherland
  // c.a.d Champ_Fonc_fonction 1 temperature A * (Tref+C)/(val+C) * pow(val/Tref,1.5);
  Nom chaine("temperature 1 ");
  Nom nA(A_);
  Nom nT(Tref_);
  Nom nC(C_);
  chaine+=nA;
  chaine+="*(";
  chaine+=nT;
  chaine+="+";
  chaine+=nC;
  chaine+=")/(val+";
  chaine+=nC;
  chaine+=")*(val/";
  chaine+=nT;
  chaine+=")^1.5";
  Cerr<<"Sutherland Ok ? Champ_Fonc_Fonction "<<chaine<<finl;
  EChaine echaine(chaine);
  Champ_Fonc_Fonction::readOn(echaine);
  Cerr<<table().val(2)<<finl;
}



