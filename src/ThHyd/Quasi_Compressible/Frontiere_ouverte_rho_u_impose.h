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
// File:        Frontiere_ouverte_rho_u_impose.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Frontiere_ouverte_rho_u_impose_included
#define Frontiere_ouverte_rho_u_impose_included

#include <Entree_fluide_vitesse_imposee_libre.h>
#include <Ref_Fluide_Quasi_Compressible.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Frontiere ouverte sur laquelle on impose le flux massique rho.U
//    et non la vitesse U. La vitesse est calculee par division par
//    rho(n+1) trouve dans le fluide au moment de l'appel a
//    val_imp.
// .SECTION voir aussi
//    Dirichlet_entree_fluide
//////////////////////////////////////////////////////////////////////////////
class Frontiere_ouverte_rho_u_impose  : public Entree_fluide_vitesse_imposee_libre
{

  Declare_instanciable(Frontiere_ouverte_rho_u_impose);

public :
  int compatible_avec_eqn(const Equation_base&) const;
  void completer();

  double val_imp_au_temps(double temps, int i) const;
  double val_imp_au_temps(double temps, int i, int j) const;

protected :
  REF(Fluide_Quasi_Compressible) le_fluide;

};

#endif
