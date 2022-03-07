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
// File:        Perte_Charge_Singuliere.h
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Sources
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Perte_Charge_Singuliere_included
#define Perte_Charge_Singuliere_included


#include <Perte_Charge.h>
#include <Domaine_dis.h>
#include <Champ_Don.h>
#include <TRUSTVect.h>
#include <Parser_U.h>
class Domaine;
class Zone_dis_base;
class Equation_base;
class Source_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Perte_Charge_Singuliere
//    Cette classe derivee de Perte_Charge est utilisee lorsque l'on veut
//    modeliser une perte de charge sur une surface (passage d'une grille par exemple)
//    par exemple)
// .SECTION voir aussi
//     Perte_Charge Perte_Charge_Reguliere
//////////////////////////////////////////////////////////////////////////////
class Perte_Charge_Singuliere : public Perte_Charge
{

public :

  virtual ~Perte_Charge_Singuliere() {}
  Entree& lire_donnees(Entree&);
  virtual void lire_surfaces(Entree&, const Domaine&, const Zone_dis_base&, IntVect&, IntVect&);
  inline double K() const;
  double calculate_Q(const Equation_base& eqn, const IntVect& num_faces, const IntVect& sgn) const; //met a jour le debit a travers la surface et le renvoie
  void update_K(const Equation_base& eqn, double deb, DoubleVect& bilan);                  //regule K_ a partir du debit calcule par update_Q

protected :

  double K_;

  //pour la regulation de K;
  Nom identifiant_;          //nom pour le fichier de sortie : celui de la surface, ou celui de la sous-zone et de la coupe
  int regul_;                //1 si regulation activee
  Parser_U deb_cible_, eps_; //K_ peut varier entre [K_ * (1 - eps_(t)), K_ * (1 + eps_(t))] par seconde pour atteindre deb_cible_(t)
};


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Perte_Charge_Singuliere::K() const
{
  return K_;
}

#endif
