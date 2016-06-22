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
// File:        Sources.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////



#include <Sources.h>
#include <DoubleTab.h>

Implemente_liste(Source);
Implemente_instanciable(Sources,"Sources",LIST(Source));


// Description:
//    Imprime la liste des Sources sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Sources::printOn(Sortie& os) const
{
  return LIST(Source)::printOn(os);
}


// Description:
//    Lecture d'une liste de sources sur un flot d'entree.
//    Lit une liste de sources separees par des virgules
//    et le sajoute a la liste.
//    format:
//    {
//     bloc de lecture d'une source
//     [, bloc de lecture d'une source]
//     ...
//    }
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: accolade fermante ou virgule attendue
// Exception:
// Effets de bord: le flot d'entree est modifie
// Postcondition:
Entree& Sources::readOn(Entree& is)
{
  Nom accouverte="{";
  Nom accfermee="}";
  Nom virgule=",";
  Nom typ;
  is >> typ;
  if (typ!=accouverte)
    {
      Cerr << "We were waiting for { before " << typ << finl;
      exit();
    }
  Source t;
  is >> typ;
  if(typ==accfermee)
    return is;
  while(1)
    {
      Source& so=add(t);
      Cerr << "Reading of term " << typ << finl;
      Cerr << "and typing: ";
      // Cout << ">>>>>>>>>>>>>>>>>>>> Type de Source = " << typ << finl;
      so.typer(typ,mon_equation.valeur());
      so->associer_eqn(mon_equation.valeur());
      is >> so.valeur();
      is >> typ;
      if(typ==accfermee)
        return is;
      if(typ!=virgule)
        {
          Cerr << typ << " : we expected a ',' or a '}'" << finl;
          exit();
        }
      assert (typ==virgule);
      is >> typ;
    }
}


// Description:
//    Ajoute la contribution de toutes les sources de la
//    liste au tableau passe en parametre, et renvoie ce tableau.
// Precondition:
// Parametre: DoubleTab& xx
//    Signification: le tableau dans lequel on doit accumuler la contribution
//                   des termes sources
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le parametre xx modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Sources::ajouter(DoubleTab& xx) const
{
  CONST_LIST_CURSEUR(Source) curseur(*this);
  while(curseur)
    {
      curseur->ajouter(xx);
      ++curseur;
    }
  return xx;
}

DoubleTab& Sources::ajouter_derivee(DoubleTab& xx) const
{
  CONST_LIST_CURSEUR(Source) curseur(*this);
  while(curseur)
    {
      curseur->ajouter_derivee(xx);
      ++curseur;
    }
  return xx;
}

// Description:
//    Calcule la contribution de toutes les sources de la
//    liste stocke le resultat dans le tableau passe en parametre,
//    et renvoie  ce tableau.
// Precondition:
// Parametre: DoubleTab& xx
//    Signification: le tableau dans lequel on doit stocker la somme
//                   des contributions des sources
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: DoubleTab&
//    Signification: le parametre xx modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:  la methode ne modifie pas l'objet
DoubleTab& Sources::calculer(DoubleTab& xx) const
{
  xx=0;
  CONST_LIST_CURSEUR(Source) curseur(*this);
  while(curseur)
    {
      curseur->ajouter(xx);
      ++curseur;
    }
  return xx;
}

DoubleTab& Sources::calculer_derivee(DoubleTab& xx) const
{
  xx=0;
  CONST_LIST_CURSEUR(Source) curseur(*this);
  while(curseur)
    {
      curseur->ajouter_derivee(xx);
      ++curseur;
    }
  return xx;
}

// Description:
//    Mise a jour en temps, de toute les sources de la liste
// Precondition:
// Parametre: double temps
//    Signification: le pas de temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Sources::mettre_a_jour(double temps)
{
  LIST_CURSEUR(Source) curseur(*this);
  while(curseur)
    {
      curseur->mettre_a_jour(temps);
      ++curseur;
    }
}


// Description:
//    Appelle Source::completer() sur toutes les sources de la
//    liste. voir Source_base::completer().
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
void Sources::completer()
{
  LIST_CURSEUR(Source) curseur(*this);
  while(curseur)
    {
      curseur->completer();
      ++curseur;
    }
}

// Description:
//  Pour chaque source de la liste, appel a associer_champ_rho
//  de la source.
//  Si la masse volumique est variable, il faut declarer le
//  champ de rho aux sources avec cette methode (front-tracking)
//  Sinon, par defaut, les calculs sont faits avec rho=1
void Sources::associer_champ_rho(const Champ_base& champ_rho)
{
  LIST_CURSEUR(Source) curseur(*this);
  while(curseur)
    {
      Source& src = curseur.valeur();
      Source_base& src_base = src.valeur();
      src_base.associer_champ_rho(champ_rho);
      ++curseur;
    }
}

// Description:
//  Pour chaque source de la liste, appel a a_pour_Champ_Fonc(mot,ch_ref).
//  Cette methode est appelee par Equation_base::a_pour_Champ_Fonc.
int Sources::a_pour_Champ_Fonc(const Motcle& mot,
                               REF(Champ_base)& ch_ref) const
{
  int ok = 0;
  CONST_LIST_CURSEUR(Source) curseur(*this);
  while(curseur)
    {
      const Source& src = curseur.valeur();
      const Source_base& src_base = src.valeur();
      if (src_base.a_pour_Champ_Fonc(mot, ch_ref))
        {
          ok = 1;
          break;
        }
      ++curseur;
    }
  return ok;
}

// Description:
//    Appelle Source::impr() sur toutes les sources de la
//    liste. voir Source_base::impr().
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
int Sources::impr(Sortie& os) const
{
  CONST_LIST_CURSEUR(Source) curseur(*this);
  while(curseur)
    {
      curseur->impr(os);
      ++curseur;
    }
  return 1;
}

// Description:
// contribution a la matrice implicite des termes sources
// par defaut pas de contribution
void Sources::contribuer_a_avec(const DoubleTab& a, Matrice_Morse& matrice) const
{
  CONST_LIST_CURSEUR(Source) curseur(*this);
  while(curseur)
    {
      const Source& src = curseur.valeur();
      const Source_base& src_base = src.valeur();
      src_base.contribuer_a_avec(a,matrice);
      ++curseur;
    }
}

// Description:
//    Appelle Source::initialiser(temps) sur toutes les sources de la
//    liste. voir Source_base::initialiser(double temps).
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
int Sources::initialiser(double temps)
{
  LIST_CURSEUR(Source) curseur(*this);
  int ok=1;
  while(curseur)
    {
      ok = ok && curseur->initialiser(temps);
      ++curseur;
    }
  return ok;
}
