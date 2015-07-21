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
// File:        Sous_Zone.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sous_Zone_included
#define Sous_Zone_included


#include <List.h>
#include <IntVect.h>
#include <Ref_Zone.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Sous_Zone
//    Represente une sous zone volumique i.e un sous ensemble
//    de polyedres d'un objet de type Zone.
//    Un objet Sous_Zone porte une reference vers la zone qu'il
//    subdivise.
// .SECTION voir aussi
//    Zone Sous_Zones
//////////////////////////////////////////////////////////////////////////////
class Sous_Zone : public Objet_U
{
  Declare_instanciable(Sous_Zone);

public :

  virtual int lire_motcle_non_standard(const Motcle&, Entree&);
  inline Sous_Zone(const Zone& );
  inline const Nom& le_nom() const;
  inline int operator()(int) const;
  inline int operator[](int) const;
  inline int nb_elem_tot() const; // Returns the subzone number of elements (real+virtual elements in parallel)
  void associer_zone(const Zone&);
  int associer_(Objet_U&);
  virtual void nommer(const Nom&);
  inline Zone& zone()
  {
    return la_zone_.valeur();
  }
  inline const Zone& zone() const
  {
    return la_zone_.valeur();
  }
  int add_poly(const int& poly);
  int remove_poly(const int& poly);

protected :

  IntVect les_polys_;
  REF(Zone) la_zone_;
  Nom nom_;
};

// Description:
//    Constructeur d'une sous-zone a partir d'une zone.
// Precondition:
// Parametre: Zone& zone
//    Signification: la zone dont la sous-zone est une partie
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Sous_Zone::Sous_Zone(const Zone& azone) : la_zone_(azone)
{}


// Description:
//    Renvoie le nom de la sous-zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom de la sous-zone
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Nom& Sous_Zone::le_nom() const
{
  return nom_;
}


// Description:
//    Renvoie le numero du i-ieme polyedre de la sous-zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme polyedre de la sous-zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Sous_Zone::operator()(int i) const
{
  return les_polys_[i];
}


// Description:
//    Renvoie le numero du i-ieme polyedre de la sous-zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le numero du i-ieme polyedre de la sous-zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Sous_Zone::operator[](int i) const
{
  return les_polys_[i];
}


// Description:
//    Renvoie le nombre de polyedre de la sous-zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de polyedre de la sous-zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Sous_Zone::nb_elem_tot() const
{
  return les_polys_.size();
}

#endif

