/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Op_Diff_CoviMAC_Elem.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Diff_CoviMAC_Elem_included
#define Op_Diff_CoviMAC_Elem_included

#include <Op_Diff_CoviMAC_base.h>
#include <Matrice_Morse.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Op_Diff_CoviMAC_Elem
//
// <Description of class Op_Diff_CoviMAC_Elem>
//
/////////////////////////////////////////////////////////////////////////////

class Op_Diff_CoviMAC_Elem : public Op_Diff_CoviMAC_base
{

  Declare_instanciable( Op_Diff_CoviMAC_Elem ) ;

public :

  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  virtual void calculer_flux_bord(const DoubleTab& inco) const;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  void contribuer_au_second_membre(DoubleTab& ) const;
  void modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const;
  void completer();
  void dimensionner(Matrice_Morse& mat) const;

  virtual void update_nu(IntTab *tpfa = NULL) const; //appelle update_nu() de Op_Diff_CoviMAC_base et met a jour les interpolations
};

/* comme des synonymes, mais avec l'info de ce qu'on est dans que_suis_je() */
class Op_Diff_Nonlinear_CoviMAC_Elem : public Op_Diff_CoviMAC_Elem
{
  Declare_instanciable( Op_Diff_Nonlinear_CoviMAC_Elem ) ;
};

class Op_Dift_CoviMAC_Elem : public Op_Diff_CoviMAC_Elem
{
  Declare_instanciable( Op_Dift_CoviMAC_Elem ) ;
};

class Op_Dift_Nonlinear_CoviMAC_Elem : public Op_Diff_CoviMAC_Elem
{
  Declare_instanciable( Op_Dift_Nonlinear_CoviMAC_Elem ) ;
};

#endif /* Op_Diff_CoviMAC_Elem_included */
