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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Operateur_negligeable.h
// Directory:   $TRUST_ROOT/src/Kernel/Operateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Operateur_negligeable_included
#define Operateur_negligeable_included

#include <TRUSTTab.h>

class Zone_dis;
class Zone_Cl_dis;
class Champ_Inc;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Opnegligeable
//    Cette classe definit l'interface d'un operateur negligeable
//    en definissant des methodes ajouter,calculer et mettre_a_jour
//    qui ne font aucun calcul.
//    Cette classe est utilisee lors de la definition d'operateurs
//    negligeable par heritage multiple.
// .SECTION voir aussi
//    Op_Diff_negligeable Op_Diff_K_Eps_negligeable Op_Conv_negligeable
//    Classe interface, hors hierarchie operateurs TrioU.
//    Doit faire partie d'un heritage multiple pour etre
//    utile a la definition d'un operateur negligeable instanciable.
//////////////////////////////////////////////////////////////////////////////
class Operateur_negligeable
{
public :

  inline DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const;
  inline DoubleTab& calculer(const DoubleTab&, DoubleTab& ) const;
  inline void mettre_a_jour(double );

protected :
  inline void associer(const Zone_dis&, const Zone_Cl_dis&, const Champ_Inc&);
};



// Description:
//    Ajout de la contribution d'un operateur negligeable a
//    un tableau.
//    NE FAIT RIEN, renvoie le parametre tableau sans le modifie.
// Precondition:
// Parametre: DoubleTab&
//    Signification: le tableau sur lequel on applique l'operateur
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Parametre: DoubleTab& x
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: DoubleTab&
//    Signification: le parametre d'entree x non modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline DoubleTab& Operateur_negligeable::ajouter(const DoubleTab&, DoubleTab& x) const
{
  return x;
}


// Description:
//    Initialise le parametre tableau avec la contribution
//    de l'operateur negligeable: initialise le tableau a ZERO.
// Precondition:
// Parametre: DoubleTab&
//    Signification: le tableau sur lequel on applique l'operateur
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Parametre: DoubleTab& x
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: DoubleTab&
//    Signification: le tableau d'entree mis a zero
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline DoubleTab& Operateur_negligeable::calculer(const DoubleTab&,
                                                  DoubleTab& x) const
{
  return x=0.0;
}


// Description:
//    Mise a jour en temps d'un operateur negligeable: NE FAIT RIEN
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
inline void Operateur_negligeable::mettre_a_jour(double )
{
}


// Description:
//    Associe divers objets a un operateurs negligeable: NE FAIT RIEN
// Precondition:
// Parametre: Zone_dis&
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Parametre: Zone_Cl_dis&
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Parametre: Champ_Inc&
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Operateur_negligeable::associer(const Zone_dis&, const Zone_Cl_dis&, const Champ_Inc&)
{
}
#endif
