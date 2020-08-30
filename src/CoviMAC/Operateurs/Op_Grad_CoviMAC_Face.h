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
// File:        Op_Grad_CoviMAC_Face.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Grad_CoviMAC_Face_included
#define Op_Grad_CoviMAC_Face_included

#include <Operateur_Grad.h>
#include <Ref_Zone_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Ref_Zone_Cl_CoviMAC.h>
#include <Ref_Champ_Face_CoviMAC.h>

//
// .DESCRIPTION class Op_Grad_CoviMAC_Face
//
//  Cette classe represente l'operateur de gradient
//  La discretisation est CoviMAC
//  On calcule le gradient d'un champ_P0_CoviMAC (la pression)
//

// .SECTION voir aussi
// Operateur_Grad_base
//

class Op_Grad_CoviMAC_Face : public Operateur_Grad_base
{

  Declare_instanciable(Op_Grad_CoviMAC_Face);

public:

  void associer(const Zone_dis& , const Zone_Cl_dis& , const Champ_Inc& );
  void completer();
  void dimensionner(Matrice_Morse& ) const;
  void dimensionner_NS(Matrice_Morse& ) const;
  virtual DoubleTab& ajouter(const DoubleTab& ,  DoubleTab&) const;
  virtual DoubleTab& ajouter_NS(const DoubleTab& ,  DoubleTab&, Matrice_Morse*, const DoubleTab*, const DoubleTab* ) const;
  virtual DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  virtual DoubleTab& calculer_NS(const DoubleTab& ,  DoubleTab&, Matrice_Morse*, const DoubleTab*, const DoubleTab* ) const;
  void contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const;
  int impr(Sortie& os) const;
  /* variation de fgrad du fait d'une correction en pression -> action differente de ajouter() !!! */
  virtual DoubleVect& multvect(const DoubleTab&, DoubleTab&) const;

  /* interaction exterieure */
  mutable IntTab fgrad_d, fgrad_j;
  mutable DoubleTab fgrad_c; // coefficients de |f| phi_f * [grad p]_f, calcules par Zone_CoviMAC::flux() a partir de Masse_CoviMAC_Face::W_e
  mutable int grad_a_jour;  // 0 si fgrad_c doit etre recalcule (fait par Masse_CoviMAC_Face)
  void update_grad() const; // pour recalculer fgrad_c

private:

  REF(Zone_CoviMAC) ref_zone;
  REF(Zone_Cl_CoviMAC) ref_zcl;

protected:
};

#endif
