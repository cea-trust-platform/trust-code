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

#ifndef Turbulence_paroi_scal_included
#define Turbulence_paroi_scal_included


#include <Turbulence_paroi_scal_base.h>
class Pb_Hydraulique;
class Probleme_base;



Declare_deriv(Turbulence_paroi_scal_base);

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Turbulence_paroi_scal
//    Classe generique de la hierarchie des turbulences au niveau de la
//    paroi, un objet Turbulence_paroi peut referencer n'importe quel
//    objet derivant Turbulence_paroi_base.
//    La plupart des methodes appellent les methodes de l'objet Probleme
//    sous-jacent via la methode valeur() declaree grace a la macro
//Declare_deriv().;
// .SECTION voir aussi
//    Turbulence_paroi_scal_base
//////////////////////////////////////////////////////////////////////////////
class Turbulence_paroi_scal : public DERIV(Turbulence_paroi_scal_base)
{

  Declare_instanciable(Turbulence_paroi_scal);

public:

  //  Turbulence_paroi_scal(const Turbulence_paroi_scal_base& x):DERIV(Turbulence_paroi_scal_base)(x) {}
  inline Turbulence_paroi_scal& operator=(const Turbulence_paroi_scal_base& paroi_base);
  void associer_modele(const Modele_turbulence_scal_base& );
  inline int init_lois_paroi();
  inline int calculer_scal(Champ_Fonc& );
  inline int calculer_scal(Champ_Fonc_base& );
  inline void imprimer_nusselt(Sortie& ) const;

  inline int  tab_equivalent_distance_size();
  inline const DoubleVect& tab_equivalent_distance(int bord) const;
protected:

  REF(Modele_turbulence_scal_base) mon_modele_turb_scal;
};


// Description:
//    Operateur d'affectation d'un objet Turbulence_paroi_scal_base
//    dans un objet Turbulence_paroi_scal.
// Precondition:
// Parametre: Turbulence_paroi_scal_base& paroi_base
//    Signification: la partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Turbulence_paroi_scal&
//    Signification: le resultat de l'affectation (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Turbulence_paroi_scal& Turbulence_paroi_scal::operator=(const Turbulence_paroi_scal_base& paroi_base)
{
  DERIV(Turbulence_paroi_scal_base)::operator=(paroi_base);
  return *this;
}

// Description:
//    Appel a l'objet sous-jacent.
//    Imprime le nusselt de la loi de paroi
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Turbulence_paroi_scal::imprimer_nusselt(Sortie& os) const
{
  valeur().imprimer_nusselt(os);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Initialise les lois de parois.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Turbulence_paroi_scal::init_lois_paroi()
{
  return valeur().init_lois_paroi();
}

// Description:
//    Appel a l'objet sous-jacent.
// Precondition:
// Parametre: Champ_Fonc& ch
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Turbulence_paroi_scal::calculer_scal(Champ_Fonc& ch)
{
  return valeur().calculer_scal(ch);
}

// Description:
//    Appel a l'objet sous-jacent.
// Precondition:
// Parametre: Champ_Fonc_base& ch
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Turbulence_paroi_scal::calculer_scal(Champ_Fonc_base& ch)
{
  return valeur().calculer_scal(ch);
}

// Description:
//    Appel a l'objet sous-jacent.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: return size of tabular containing equivalent distances associated to all boundaries, so it is equivalent to number of boundaries
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Turbulence_paroi_scal::tab_equivalent_distance_size()
{
  return valeur().tab_equivalent_distance_size();
}

// Description:
//    Appel a l'objet sous-jacent.
// Precondition:
// Parametre: boundary index
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Doublevect
//    Signification: return equivalent distances associated to boundary index
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline const DoubleVect& Turbulence_paroi_scal::tab_equivalent_distance(int boundary_index) const
{
  return valeur().tab_equivalent_distance(boundary_index);
}

#endif
