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
// File:        Field_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#include <Field_base.h>

Implemente_base_sans_constructeur(Field_base,"Field_base",Objet_U);

Field_base::Field_base() :
  nb_compo_(0),
  nature_(scalaire)
{}

Sortie& Field_base::printOn(Sortie& s ) const
{
  return s;
}

Entree& Field_base::readOn(Entree& is )
{
  return is;
}

// Description:
//    Renvoie le nom du champ.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Field_base::le_nom() const
{
  return nom_;
}

// Description:
//    Donne un nom au champ
// Precondition:
// Parametre: Nom& name
//    Signification: le nom a donner au champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Field_base::nommer(const Nom& name)
{
  nom_ = name ;
}

// Description:
//    Fixe le nombre de composantes du champ.
//    Le champ est vectoriel s'il est de meme dimension
//    que l'espace.
// Precondition:
// Parametre: int i
//    Signification: le nombre de composantes du champs
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: void
// Exception:
// Effets de bord:
// Postcondition: la nature du champ peut changer, il peut devenir vectoriel
//                s'il a la bonne dimension
void Field_base::fixer_nb_comp(int i)
{
  // Interdiction de changer la nature du champ une fois
  // le nb_valeurs_nodales fixe.
  nb_compo_ = i ;
  noms_compo_.dimensionner(i);

  if(i==dimension)
    fixer_nature_du_champ(vectoriel);
  else if(i>dimension)
    fixer_nature_du_champ(multi_scalaire);
}

// Description:
//    Fixe le nom des composantes du champ
// Precondition:
// Parametre: Noms& noms
//    Signification: le tableau des noms a donner aux composantes du champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Noms&
//    Signification: le tableau des noms des composantes du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition:
const Noms& Field_base::fixer_noms_compo(const Noms& noms)
{
  return noms_compo_=noms;
}

// Description:
//    Renvoie le tableau des noms des composantes du champ
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Noms&
//    Signification: les noms des composantes du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Noms& Field_base::noms_compo() const
{
  return noms_compo_;
}

// Description:
//    Fixe le nom de la i-eme composante du champ
// Precondition:
// Parametre: int i
//    Signification: l'index de la composante du champ dont on veut
//                   specifier le nom
//    Valeurs par defaut:
//    Contraintes: i < nombre de composantes du champ
//    Acces:
// Parametre: Nom& nom
//    Signification: le nom a donner a la i-eme composante du champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Nom&
//    Signification: le nom de i-eme composante du champ
//    Contraintes: reference constante
// Exception: index de la composante du champ invalide
// Effets de bord:
// Postcondition:
const Nom& Field_base::fixer_nom_compo(int i, const Nom& nom)
{
  assert(i<nb_comp());
  return noms_compo_[i]=nom;
}

// Description:
//    Renvoie le nom de la ieme composante du champ
// Precondition:
// Parametre: int i
//    Signification: l'index de la composante du champ dont on veut
//                   specifier le nom
//    Valeurs par defaut:
//    Contraintes: i < nombre de composantes du champ
//    Acces:
// Retour: Nom& nom
//    Signification: le nom de i-eme composante du champ
//    Contraintes: reference constante
// Exception: index de la composante du champ invalide
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Field_base::nom_compo(int i) const
{
  assert(i<nb_comp());
  return noms_compo_[i];
}

// Description:
//    Fixe le nom d'un champ scalaire
// Precondition: le champ doit etre scalaire (avoir 1 seule composante)
// Parametre: Nom& nom
//    Signification: le nom a donner au champ (scalaire)
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Nom&
//    Signification: le nom du champ scalaire
//    Contraintes: reference constante
// Exception: le champ n'est pas scalaire
// Effets de bord:
// Postcondition:
const Nom& Field_base::fixer_nom_compo(const Nom& nom)
{
  assert(nb_comp()==1);
  nommer(nom);
  noms_compo_.dimensionner_force(1);
  return noms_compo_[0]=nom;
}

// Description:
//    Renvoie le nom d'un champ scalaire
// Precondition: le champ doit etre scalaire
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom du champ scalaire
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Field_base::nom_compo() const
{
  assert(nb_comp()==1);
  return le_nom();
}

// Description:
//    Specifie les unites des composantes du champ.
//    Ces unites sont specifiees grace a un tableau de Nom
//    et peuvent etre differentes pour chaque composante
//    du champ.
// Precondition:
// Parametre: Noms& noms
//    Signification: les noms des unites des composantes du champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Noms&
//    Signification: les noms des unites des composantes du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la nature du champ est changee: le champ est multiscalaire,
const Noms& Field_base::fixer_unites(const Noms& noms)
{
  unite_.dimensionner_force(nb_comp());
  fixer_nature_du_champ(multi_scalaire);
  return unite_=noms;
}

// Description:
//    Renvoie les unites des composantes du champ
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Noms&
//    Signification: les noms des unites des composantes du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Noms& Field_base::unites() const
{
  return unite_;
}

// Description:
//    Specifie l'unite de la i-eme composante du champ
// Precondition:
// Parametre: int i
//    Signification: l'index de la composante du champ dont on veut
//                   specifier l'unite
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: Nom& nom
//    Signification: le type de l'unite a specifier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Nom&
//    Signification: le type de l'unite de la i-eme composante du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: le champ est de nature multi-scalaire.
const Nom& Field_base::fixer_unite(int i, const Nom& nom)
{
  fixer_nature_du_champ(multi_scalaire);
  return unite_[i]=nom;
}

// Description:
//    Renvoie l'unite de la i-eme composante du champ
// Precondition:
// Parametre: int i
//    Signification: l'index de la composante du champ dont on veut
//                   connaitre l'unite
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: l'unite de la i-eme composante du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Field_base::unite(int i) const
{
  return unite_[i];
}

// Description:
//    Specifie l'unite d'un champ scalaire ou dont toutes les
//    composantes ont la meme unite
// Precondition:
// Parametre: Nom& nom
//    Signification: l'unite a specifier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Nom&
//    Signification: l'unite du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la dimension du tableau stockant les unites est force a 1
const Nom& Field_base::fixer_unite(const Nom& nom)
{
  unite_.dimensionner_force(1);
  return unite_[0]=nom;
}

// Description:
//    Renvoie l'unite d'un champ scalaire dont toutes les
//    composantes ont la meme unite
// Precondition: toutes les composantes du champ doivent avoir
//               la meme unite
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: l'unite (commune) des composantes du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Field_base::unite() const
{
  assert(unite_.size() == 1);
  return unite_[0];
}


// Description:
//    Fixer la nature d'un champ: scalaire, multiscalaire, vectoriel.
//    Le type (enumere) Nature_du_champ est defini dans Ch_base.h.
// Precondition:
// Parametre: Nature_du_champ n
//    Signification: la nature a donner au champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Nature_du_champ
//    Signification: la nature du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le membre Field_base::nature est modifie
Nature_du_champ Field_base::fixer_nature_du_champ(Nature_du_champ n)
{
  return nature_ = n;
}
