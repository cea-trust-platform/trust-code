/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Modele_turbulence_hyd_included
#define Modele_turbulence_hyd_included

#include <Modele_turbulence_hyd_base.h>
#include <TRUST_Deriv.h>

/*! @brief Classe Modele_turbulence_hyd Classe generique de la hierarchie des modeles de turbulence utilises
 *
 *     par une equation_base
 *     un objet Modele_turbulence_hyd peut referencer n'importe quel objet derivant de
 *     Modele_turbulence_hyd_base.
 *     La plupart des methodes appellent les methodes de l'objet Modele_turbulence_hyd
 *     sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Modele_turbulence_hyd_base MorEqn
 */
class Modele_turbulence_hyd: public MorEqn, public DERIV(Modele_turbulence_hyd_base)
{
  Declare_instanciable(Modele_turbulence_hyd);

public:

  inline Modele_turbulence_hyd& operator=(const Modele_turbulence_hyd_base&);
  inline const Champ_Fonc& viscosite_turbulente() const;
  /*Modification pour utilisation des fonctions de la classe MorEqn*/
  inline const Turbulence_paroi& loi_paroi() const;

  inline int preparer_calcul();
  inline void mettre_a_jour(double);
  inline void discretiser();
  inline void completer();
  inline int sauvegarder(Sortie &os) const override;
  inline int reprendre(Entree &is) override;
  inline void imprimer(Sortie &os) const;
};

inline Modele_turbulence_hyd& Modele_turbulence_hyd::operator=(const Modele_turbulence_hyd_base& x)
{
  DERIV(Modele_turbulence_hyd_base)::operator=(x);
  return *this;
}

inline const Champ_Fonc& Modele_turbulence_hyd::viscosite_turbulente() const
{
  return valeur().viscosite_turbulente();
}

inline const Turbulence_paroi& Modele_turbulence_hyd::loi_paroi() const
{
  return valeur().loi_paroi();
}

inline int Modele_turbulence_hyd::preparer_calcul()
{
  return valeur().preparer_calcul();
}

inline void Modele_turbulence_hyd::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}

inline int Modele_turbulence_hyd::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}

inline int Modele_turbulence_hyd::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

inline void Modele_turbulence_hyd::discretiser()
{
  valeur().discretiser();
}

inline void Modele_turbulence_hyd::completer()
{
  valeur().completer();
  valeur().loi_paroi()->completer();
}

inline void Modele_turbulence_hyd::imprimer(Sortie& os) const
{
  valeur().imprimer(os);
}

#endif

