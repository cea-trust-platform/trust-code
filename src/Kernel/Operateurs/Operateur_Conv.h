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
// File:        Operateur_Conv.h
// Directory:   $TRUST_ROOT/src/Kernel/Operateurs
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Operateur_Conv_included
#define Operateur_Conv_included

#include <Operateur_Conv_base.h>
#include <Operateur.h>
#include <Ref_Champ_base.h>

Declare_deriv(Operateur_Conv_base);


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Operateur_Conv
//    Classe generique de la hierarchie des operateurs representant un terme
//    de convection. Un objet Operateur_Conv peut referencer n'importe quel
//    objet derivant de Operateur_Conv_base.
// .SECTION voir aussi
//      Operateur_Conv_base Operateur
//////////////////////////////////////////////////////////////////////////////
class Operateur_Conv  : public Operateur, public DERIV(Operateur_Conv_base)
{
  Declare_instanciable(Operateur_Conv);
public :

  inline Operateur_base& l_op_base() override;
  inline const Operateur_base& l_op_base() const override;
  DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const override;
  DoubleTab& calculer(const DoubleTab&, DoubleTab& ) const override;
  inline void associer_vitesse(const Champ_base&) ;
  inline const Champ_base&  vitesse() const ;
  void typer() override;
  inline void typer(const Nom&);
  inline int op_non_nul() const override;
  void associer_norme_vitesse(const Champ_base&);
protected :

  REF(Champ_base) la_vitesse;
};

// Description:
//    Renvoie l'objet sous-jacent upcaste en Operateur_base
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Operateur_base&
//    Signification: l'objet sous-jacent upcaste en Operateur_base
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Operateur_base& Operateur_Conv::l_op_base()
{
  return valeur();
}
// Description:
//    Renvoie l'objet sous-jacent upcaste en Operateur_base
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Operateur_base&
//    Signification: l'objet sous-jacent upcaste en Operateur_base
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Operateur_base& Operateur_Conv::l_op_base() const
{
  return valeur();
}

// Description:
//    Associe la vitesse (en tant que vitesse transportante)
//    a l'operateur de convection.
// Precondition:
// Parametre: Champ_Inc& vit
//    Signification: le champ inconnue representant la vitesse
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification: le champ inconnue representant la vitesse transportante
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'operateur a une vitesse transportante asscoiee
inline void Operateur_Conv::associer_vitesse(const Champ_base& vit)
{
  la_vitesse = vit;
}

// Description:
//    Renvoie la vitesse transportante de l'operateur
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue representant la vitesse transportante
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_base& Operateur_Conv::vitesse() const
{
  return la_vitesse.valeur();
}



// Description:
//    Type l'operateur.
// Precondition:
// Parametre: Nom& typ
//    Signification: le nom representant le type de l'operateur
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Operateur_Conv::typer(const Nom& a_type)
{
  DERIV(Operateur_Conv_base)::typer(a_type);
}

inline int Operateur_Conv::op_non_nul() const
{
  if (non_nul())
    return 1;
  else
    return 0;
}
#endif
