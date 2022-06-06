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

#ifndef MorEqn_included
#define MorEqn_included

#include <Ref_Equation_base.h>

class Champ;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe MorEqn
//     Classe qui regroupe les fonctionnalites de liaison avec une
//     Equation. Le membre MorEqn::mon_equation contient une reference
//     vers un objet Equation_base. Les classes qui sont des "Morceaux d'equation"
//     sont associe par une liaison 1-1 a leur equation en heritant de MorEqn,
//     par exemple: Operateur_base, Solveur_Masse, Source_base ...
// .SECTION voir aussi
//     Equation_base
//     Ce n'est pas une classe de l'arbre TRUST a elle seule.
//     Cette classe est faite etre une classe mere d'une classe
//     qui heritera par ailleurs d'Objet_U
//////////////////////////////////////////////////////////////////////////////
class MorEqn
{

public:
  void associer_eqn(const Equation_base&);
  virtual void calculer_pour_post(Champ& espace_stockage,const Nom& option, int comp) const;
  virtual Motcle get_localisation_pour_post(const Nom& option) const;
  inline const Equation_base& equation() const;
  inline Equation_base& equation();

  inline int mon_equation_non_nul() const;

  /* compatibilite avec les equations multiphase : par defaut, message d'erreur */
  virtual void check_multiphase_compatibility() const;
protected :
  REF(Equation_base) mon_equation;
  inline virtual ~MorEqn();
};

MorEqn::~MorEqn()
{}
// Description:
//    Renvoie la reference sur l'equation pointe par
//    MorEqn::mon_equation.
//    (version const)
// Precondition: on doit avoir associe une equation
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Equation_base&
//    Signification: l'equation associee a l'objet
//    Contraintes: reference constante
// Exception: pas d'equation associee
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Equation_base& MorEqn::equation() const
{
  if (mon_equation.non_nul()==0)
    {
      Cerr << "\nError in MorEqn::equation() : The equation is unknown !" << finl;
      Process::exit();
    }
  return mon_equation.valeur();
}

// Description:
//    Renvoie la reference sur l'equation pointe par
//    MorEqn::mon_equation.
// Precondition: on doit avoir associe une equation
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Equation_base&
//    Signification: l'equation associee a l'objet
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline  Equation_base& MorEqn::equation()
{
  if (mon_equation.non_nul()==0)
    {
      Cerr << "\nError in MorEqn::equation() : The equation is unknown !" << finl;
      Process::exit();
    }
  return mon_equation.valeur();
}
int MorEqn::mon_equation_non_nul() const
{
  //Pour pouvoir tester si la reference est
  if (mon_equation.non_nul())
    return 1;
  else
    return 0;
}
#endif

