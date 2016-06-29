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
// File:        Operateur_Conv_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Operateurs
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Operateur_Conv_base_included
#define Operateur_Conv_base_included

#include <Operateur_base.h>
#include <Ref_Champ_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Operateur_Conv_base
//     Cette classe est la base de la hierarchie des operateurs representant
//     un terme de convection dans une equation.
// .SECTION voir aussi
//     Operateur_base
//     Classe abstraite
//     Methode abstraite
//       void associer_vitesse(const Champ_Inc& vit )
//////////////////////////////////////////////////////////////////////////////

class Operateur_Conv_base  : public Operateur_base
{
  Declare_base(Operateur_Conv_base);

public :

  virtual void associer_vitesse(const Champ_base& vit ) =0;
  inline double dt_stab_conv() const;
  inline void fixer_dt_stab_conv(double dt);
  virtual void associer_norme_vitesse(const Champ_base& norme_vitesse);
  virtual void associer_vitesse_pour_pas_de_temps(const Champ_base& vitesse);

protected :
  REF(Champ_base) la_norme_vitesse;
  REF(Champ_base) vitesse_pour_pas_de_temps_;
  double dt_stab_conv_;
};

//Descrition
// renvoie la valeur du pas de temps de stabilite en convection
inline double Operateur_Conv_base::dt_stab_conv() const
{
  assert(!est_egal(dt_stab_conv_,123.));
  return dt_stab_conv_;
}

//Descrition
// permet de fixer la valeur du pas de temps de stabilite en convection
inline void Operateur_Conv_base::fixer_dt_stab_conv(double dt)
{
  dt_stab_conv_ = dt;
}

#endif
