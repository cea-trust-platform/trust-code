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
// File:        Modele_turbulence_hyd_K_Eps_V2.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_turbulence_hyd_K_Eps_V2_included
#define Modele_turbulence_hyd_K_Eps_V2_included

#include <EqnF22.h>
#include <Modele_turbulence_hyd_K_Eps.h>
#include <Transport_K_Eps_V2.h>
#include <Transport_V2.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Modele_turbulence_hyd_K_Eps_V2
//    Cette classe represente le modele de turbulence (k,eps,v2) pour les
//    equations de Navier-Stokes.
// .SECTION voir aussi
//    Mod_turb_hyd_base Mod_turb_hyd_ss_maille
//////////////////////////////////////////////////////////////////////////////
class Modele_turbulence_hyd_K_Eps_V2 : public Mod_turb_hyd_RANS
{
  Declare_base(Modele_turbulence_hyd_K_Eps_V2);

public:

  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void completer();
  int preparer_calcul();
  virtual bool initTimeStep(double dt);
  int sauvegarder(Sortie& os) const;
  int reprendre(Entree& is);

  inline int nombre_d_equations() const;
  inline Transport_K_Eps_base& eqn_transp_K_Eps();
  inline const Transport_K_Eps_base& eqn_transp_K_Eps() const;
  const Equation_base& equation_k_eps(int) const ;


  void verifie_loi_paroi();
  inline  const Transport_V2& eqn_V2() const;
  inline Transport_V2& eqn_V2();

  inline EqnF22base& eqn_F22();
  inline const EqnF22base& eqn_F22() const;

  inline Champ_Inc& V2();
  inline const Champ_Inc& V2() const;

  virtual inline Champ_Inc& K_Eps();
  virtual inline const Champ_Inc& K_Eps() const;

protected:

  Transport_K_Eps_V2 eqn_transport_K_Eps_V2;
  Transport_V2 eqn_transport_V2;
  EqnF22 eqn_f22;

};

inline const Transport_K_Eps_base& Modele_turbulence_hyd_K_Eps_V2::eqn_transp_K_Eps() const
{
  return eqn_transport_K_Eps_V2;
}

inline Transport_K_Eps_base& Modele_turbulence_hyd_K_Eps_V2::eqn_transp_K_Eps()
{
  return eqn_transport_K_Eps_V2;
}

inline const Champ_Inc& Modele_turbulence_hyd_K_Eps_V2::K_Eps() const
{
  return eqn_transport_K_Eps_V2.inconnue();
}

inline Champ_Inc& Modele_turbulence_hyd_K_Eps_V2::K_Eps()
{
  return eqn_transport_K_Eps_V2.inconnue();
}


// Description:
//    Renvoie l'equation du modele de turbulence
//    i.e. : (V2).
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Transport_V2&
//    Signification: equation V2
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline  Transport_V2& Modele_turbulence_hyd_K_Eps_V2::eqn_V2()
{
  return eqn_transport_V2;
}

// Description:
//    Renvoie l'equation du modele de turbulence
//    i.e. : (V2).
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Transport_V2&
//    Signification: equation V2
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Transport_V2& Modele_turbulence_hyd_K_Eps_V2::eqn_V2() const
{
  return eqn_transport_V2;
}
// Description:
//    Renvoie l'equation en F22 du modele de turbulence
//    i.e. : (eqnF22).
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: EqnF22base&
//    Signification: equation F22
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline  EqnF22base& Modele_turbulence_hyd_K_Eps_V2::eqn_F22()
{
  return eqn_f22.valeur();
}
// Description:
//    Renvoie l'equation en F22 du modele de turbulence
//    i.e. : (eqnF22).
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: EqnF22base&
//    Signification: equation F22
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline  const EqnF22base& Modele_turbulence_hyd_K_Eps_V2::eqn_F22() const
{
  return eqn_f22.valeur();
}

inline Champ_Inc& Modele_turbulence_hyd_K_Eps_V2::V2()
{
  return eqn_transport_V2.inconnue();
}

inline const Champ_Inc& Modele_turbulence_hyd_K_Eps_V2::V2() const
{
  return eqn_transport_V2.inconnue();
}


inline int Modele_turbulence_hyd_K_Eps_V2::nombre_d_equations() const
{
  return 3;
}
/* inline Champ_Inc& Modele_turbulence_hyd_K_Eps_V2::F22() */
/* { */
/*   return eqn_f22.inconnue(); */
/* } */

/* inline const Champ_Inc& Modele_turbulence_hyd_K_Eps_V2::F22() const */
/* { */
/*   return eqn_f22.inconnue(); */
/* } */

#endif
