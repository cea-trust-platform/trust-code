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
// File:        Modele_turbulence_scal.h
// Directory:   $TRUST_ROOT/src/ThHyd/Modele_turbulence_base
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_turbulence_scal_included
#define Modele_turbulence_scal_included


//
// .DESCRIPTION class Modele_turbulence_scal
// Cette classe represente n'importe quelle classe
// derivee de Modele_turbulence_scal_base

#include <Modele_turbulence_scal_base.h>

Declare_deriv(Modele_turbulence_scal_base);

class Modele_turbulence_scal : public MorEqn, public DERIV(Modele_turbulence_scal_base)
{

  Declare_instanciable(Modele_turbulence_scal);

public:

  //  inline Modele_turbulence_scal(Modele_turbulence_scal_base& );
  inline Modele_turbulence_scal& operator=(const Modele_turbulence_scal_base& );
  inline int preparer_calcul();
  inline void completer();
  inline void mettre_a_jour(double );
  inline const Champ_Fonc& conductivite_turbulente() const;
  inline const Turbulence_paroi_scal& loi_paroi() const;
  inline int loi_paroi_non_nulle() const;
  inline int sauvegarde(Sortie& ) const;
  inline int reprise(Entree& );

  inline void discretiser();
  inline int sauvegarder(Sortie& ) const override;
  inline int reprendre(Entree& ) override;
  inline void imprimer(Sortie& os) const;

};


//
//  Fonctions inline de la classe Modele_turbulence_scal
//

/*
inline Modele_turbulence_scal::Modele_turbulence_scal(Modele_turbulence_scal_base& x)
  : DERIV(Modele_turbulence_scal_base)(x) {}

inline Modele_turbulence_scal& Modele_turbulence_scal::operator=(const Modele_turbulence_scal_base& x)
{
  DERIV(Modele_turbulence_scal_base)::operator=(x);
  return *this;
}
*/
inline void Modele_turbulence_scal::completer()
{
  valeur().completer();
}

inline void Modele_turbulence_scal::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}

inline const Champ_Fonc& Modele_turbulence_scal::conductivite_turbulente() const
{
  return valeur().conductivite_turbulente();
}

inline const Turbulence_paroi_scal& Modele_turbulence_scal::loi_paroi() const
{
  return valeur().loi_paroi();
}

inline int Modele_turbulence_scal::loi_paroi_non_nulle() const
{
  return valeur().loi_paroi_non_nulle();
}

inline int Modele_turbulence_scal::preparer_calcul()
{
  return valeur().preparer_calcul();
}

inline void Modele_turbulence_scal::discretiser()
{
  valeur().discretiser();
}

inline int Modele_turbulence_scal::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}

inline int Modele_turbulence_scal::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

// Description:
//    Appel a l'objet sous-jacent
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
inline void Modele_turbulence_scal::imprimer(Sortie& os) const
{
  valeur().imprimer(os);
}
#endif
