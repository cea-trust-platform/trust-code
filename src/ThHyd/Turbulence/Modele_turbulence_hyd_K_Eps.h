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
// File:        Modele_turbulence_hyd_K_Eps.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_turbulence_hyd_K_Eps_included
#define Modele_turbulence_hyd_K_Eps_included

#include <Transport_K_Eps.h>
#include <Modele_Fonc_Bas_Reynolds.h>
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Modele_turbulence_hyd_K_Eps
//    Cette classe represente le modele de turbulence (k,eps) pour les
//    equations de Navier-Stokes.
// .SECTION voir aussi
//    Mod_turb_hyd_base Mod_turb_hyd_ss_maille
//////////////////////////////////////////////////////////////////////////////

class Modele_turbulence_hyd_K_Eps : public Mod_turb_hyd_RANS
{

  Declare_instanciable(Modele_turbulence_hyd_K_Eps);

public:

  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  int preparer_calcul();
  void verifie_loi_paroi();
  virtual bool initTimeStep(double dt);
  void mettre_a_jour(double );
  virtual inline Champ_Inc& K_Eps();
  virtual inline const Champ_Inc& K_Eps() const;

  inline int nombre_d_equations() const;
  inline Transport_K_Eps_base& eqn_transp_K_Eps();
  inline const Transport_K_Eps_base& eqn_transp_K_Eps() const;
  const Equation_base& equation_k_eps(int) const ;


  inline Modele_Fonc_Bas_Reynolds& associe_modele_fonction();
  inline const Modele_Fonc_Bas_Reynolds& associe_modele_fonction() const;

  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
protected:
  Modele_Fonc_Bas_Reynolds mon_modele_fonc;
  Transport_K_Eps  eqn_transport_K_Eps;
  virtual Champ_Fonc& calculer_viscosite_turbulente(double temps);

};


// Description:
//    Renvoie le champ inconnue du modele de turbulence
//    i.e. : (K,Epsilon). Cette inconnue est portee
//    par l'equation de transport K-eps porte par le modele.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue (K,epsilon)
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Inc& Modele_turbulence_hyd_K_Eps::K_Eps() const
{
  return eqn_transport_K_Eps.inconnue();
}


// Description:
//    Renvoie le champ inconnue du modele de turbulence
//    i.e. : (K,Epsilon). Cette inconnue est portee
//    par l'equation de transport K-eps porte par le modele.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue (K,epsilon)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Champ_Inc& Modele_turbulence_hyd_K_Eps::K_Eps()
{
  return eqn_transport_K_Eps.inconnue();
}

// Description:
//    Renvoie l'equation du modele de turbulence
//    i.e. : (K,Epsilon).
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Transport_K_Eps&
//    Signification: equation (K,epsilon)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Transport_K_Eps_base& Modele_turbulence_hyd_K_Eps::eqn_transp_K_Eps()
{
  return eqn_transport_K_Eps;
}

// Description:
//    Renvoie l'equation du modele de turbulence
//    i.e. : (K,Epsilon).
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Transport_K_Eps&
//    Signification: equation (K,epsilon)
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Transport_K_Eps_base& Modele_turbulence_hyd_K_Eps::eqn_transp_K_Eps() const
{
  return eqn_transport_K_Eps;
}
inline Modele_Fonc_Bas_Reynolds& Modele_turbulence_hyd_K_Eps::associe_modele_fonction()
{
  return mon_modele_fonc;
}

inline const Modele_Fonc_Bas_Reynolds& Modele_turbulence_hyd_K_Eps::associe_modele_fonction() const
{
  return  mon_modele_fonc;
}
inline int Modele_turbulence_hyd_K_Eps::nombre_d_equations() const
{
  return 1;
}
#endif
