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
// File:        EqnF22.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EqnF22_included
#define EqnF22_included


#include <EqnF22base.h>
class Motcle;


Declare_deriv(EqnF22base);


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe EqnF22base
//    Cette classe represente l'equation de  la quantite F22
//    associee au modele de turbulence (k,eps,v2).
//    On instanciera un objet de cette classe uniquement si
//    on utilise le modele (k,eps,v2) pour traiter la turbulence
// .SECTION voir aussi
//    Equation_base
//////////////////////////////////////////////////////////////////////////////
class EqnF22 :  public DERIV(EqnF22base)
{

  Declare_instanciable(EqnF22);

public:

  inline void associer_milieu_base(const Milieu_base&);
  inline void associer_modele_turbulence(const Modele_turbulence_hyd_K_Eps_V2& );
  inline const Milieu_base& milieu() const ;
  inline Milieu_base& milieu() ;
  inline const Modele_turbulence_hyd_K_Eps_V2& modele_turbulence() const;
  inline Modele_turbulence_hyd_K_Eps_V2& modele_turbulence();
  inline int nombre_d_operateurs() const;
  inline const Operateur& operateur(int) const;
  inline Operateur& operateur(int);
  inline const Champ_Inc& inconnue() const;
  inline Champ_Inc& inconnue();
  inline void  discretiser();
  inline void resoudre();
  inline int preparer_calcul();
  inline int sauvegarder( Sortie&) const;
  inline int reprendre( Entree&);
  /*
    inline int comprend_mot(const Motcle& ) const;
    inline int comprend_champ(const Motcle& ) const;
    inline int a_pour_Champ_Inc(const Motcle&, REF(Champ_base)& ) const;
  */
  inline const Motcle& domaine_application() const;


};

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
inline void EqnF22::associer_milieu_base(const Milieu_base& m)
{
  valeur().associer_milieu_base(m);
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
inline void EqnF22::associer_modele_turbulence(const Modele_turbulence_hyd_K_Eps_V2& mod)
{
  valeur().associer_modele_turbulence(mod);
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
inline const Milieu_base& EqnF22::milieu() const
{
  return valeur().milieu();
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
inline Milieu_base& EqnF22::milieu()
{
  return valeur().milieu();
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
inline const Modele_turbulence_hyd_K_Eps_V2& EqnF22::modele_turbulence() const
{
  return valeur().modele_turbulence();
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
inline  Modele_turbulence_hyd_K_Eps_V2& EqnF22::modele_turbulence()
{
  return valeur().modele_turbulence();
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
inline int EqnF22::nombre_d_operateurs() const
{
  return valeur().nombre_d_operateurs();
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
inline const Operateur& EqnF22::operateur(int i) const
{
  return valeur().operateur(i);
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
inline  Operateur& EqnF22::operateur(int i)
{
  return valeur().operateur(i);
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
inline const Champ_Inc& EqnF22::inconnue() const
{
  return valeur().inconnue();
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
inline Champ_Inc& EqnF22::inconnue()
{
  return valeur().inconnue();
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
inline void EqnF22::discretiser()
{
  valeur().discretiser();
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
inline void EqnF22::resoudre()
{
  valeur().resoudre();
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
inline const Motcle& EqnF22::domaine_application() const
{
  return valeur().domaine_application();
}

inline int EqnF22::preparer_calcul()
{
  return valeur().preparer_calcul();
}

inline int EqnF22::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}

inline int EqnF22::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

#endif


