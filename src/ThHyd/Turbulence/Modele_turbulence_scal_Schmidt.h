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
// File:        Modele_turbulence_scal_Schmidt.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_turbulence_scal_Schmidt_included
#define Modele_turbulence_scal_Schmidt_included

#include <Mod_Turb_scal_diffturb_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Modele_turbulence_scal_Schmidt
//    Cette classe represente le modele de calcul suivant
//    pour la diffusion turbulente:
//                  diffu_turb = visco_turb / Sc_turb;
// .SECTION voir aussi
//    Mod_Turb_scal_diffuturb_base
//////////////////////////////////////////////////////////////////////////////
class Modele_turbulence_scal_Schmidt : public Mod_Turb_scal_diffturb_base
{

  Declare_instanciable_sans_constructeur(Modele_turbulence_scal_Schmidt);

public:
  Modele_turbulence_scal_Schmidt();
  virtual int comprend_champ(const Motcle& ) const;
  virtual int a_pour_Champ_Fonc(const Motcle&, REF(Champ_base)& ) const;
  virtual void mettre_a_jour(double );
  void set_param(Param&);
  inline double get_Scturb() const;
protected:
  double LeScturb;
  Champ_Fonc& calculer_diffusion_turbulente();
};

// Description:
//    Renvoie de la valeur du Prandtl
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: la valeur Prdt
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double Modele_turbulence_scal_Schmidt::get_Scturb() const
{
  return LeScturb;
}

#endif
