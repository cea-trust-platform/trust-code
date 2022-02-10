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
// File:        Sortie_libre_Gradient_Pression_libre_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Cond_Lim
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sortie_libre_Gradient_Pression_libre_VEF_included
#define Sortie_libre_Gradient_Pression_libre_VEF_included


//
// .DESCRIPTION Sortie_libre_Gradient_Pression_libre_VEF
//
// Cette classe derive de la classe Neumann_sortie_libre
//
//    Elle represente une frontiere ouverte avec condition
//    de gradient de pression calculer.
//    La fonction flux_impose() renvoie une valeur de pression
//    a l'exterieur calculee a partir du gradient de pression impose
//    et de la pression a l'interieur du domaine.
//
//
// .SECTION voir aussi
// Milieu_base


#include <Neumann_sortie_libre.h>
#include <Ref_Champ_P0_VEF.h>
#include <Ref_Zone_VEF.h>

class Sortie_libre_Gradient_Pression_libre_VEF : public Neumann_sortie_libre
{

  Declare_instanciable(Sortie_libre_Gradient_Pression_libre_VEF);

public:

  void completer() override;
  int initialiser(double temps) override;
  void mettre_a_jour(double temps) override;
  double flux_impose(int ) const override;
  double flux_impose(int , int ) const override;
  virtual double Grad_P_lib_VEF(int ) const;
  int compatible_avec_eqn(const Equation_base&) const override;
  int compatible_avec_discr(const Discretisation_base& ) const override;

protected:

  REF(Zone_VEF)  la_zone_VEF;
  REF(Champ_P0_VEF)  pression_interne;
  DoubleVect trace_pression_int;   // pression interne sur les mailles de bord
  DoubleVect coeff;

};

#endif
