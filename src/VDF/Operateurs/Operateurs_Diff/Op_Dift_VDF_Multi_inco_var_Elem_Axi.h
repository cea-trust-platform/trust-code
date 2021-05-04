/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Op_Dift_VDF_Multi_inco_var_Elem_Axi.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Operateurs_Diff
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Dift_VDF_Multi_inco_var_Elem_Axi_included
#define Op_Dift_VDF_Multi_inco_var_Elem_Axi_included

#include <Op_Dift_VDF_base.h>
#include <ItVDFEl.h>
#include <Eval_Dift_VDF_leaves.h>
#include <Op_VDF_Elem.h>

declare_It_VDF_Elem(Eval_Dift_VDF_Multi_inco_var_Elem_Axi)

class Champ_Fonc;
class Turbulence_paroi_scal;

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Dift_VDF_Multi_inco_var_Elem_Axi
//
//////////////////////////////////////////////////////////////////////////////

class Op_Dift_VDF_Multi_inco_var_Elem_Axi : public Op_Dift_VDF_base, public Op_VDF_Elem
{
  Declare_instanciable_sans_constructeur(Op_Dift_VDF_Multi_inco_var_Elem_Axi);

public:
  Op_Dift_VDF_Multi_inco_var_Elem_Axi();
  void associer(const Zone_dis& , const Zone_Cl_dis& , const Champ_Inc& );
  void associer_diffusivite(const Champ_base& );
  void associer_diffusivite_turbulente(const Champ_Fonc& );
  inline  void dimensionner(Matrice_Morse& ) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  void associer_loipar(const Turbulence_paroi_scal& );
  void completer();
  const Champ_base& diffusivite() const;
  double calculer_dt_stab() const;
};

// Description:
// on dimensionne notre matrice.
inline  void Op_Dift_VDF_Multi_inco_var_Elem_Axi::dimensionner(Matrice_Morse& matrice) const
{
  Op_VDF_Elem::dimensionner(iter.zone(), iter.zone_Cl(), matrice);
}

inline void Op_Dift_VDF_Multi_inco_var_Elem_Axi::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_VDF_Elem::modifier_pour_Cl(iter.zone(), iter.zone_Cl(), matrice, secmem);
}

#endif /* Op_Dift_VDF_Multi_inco_var_Elem_Axi_included */
