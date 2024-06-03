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

#ifndef Op_Dift_VDF_Elem_base_included
#define Op_Dift_VDF_Elem_base_included

#include <Iterateur_VDF_Elem.h>
#include <Op_Dift_VDF_base.h>
#include <Op_VDF_Elem.h>

class Turbulence_paroi_scal;

class Op_Dift_VDF_Elem_base : public Op_Dift_VDF_base, public Op_VDF_Elem
{
  Declare_base(Op_Dift_VDF_Elem_base);
public:
  inline Op_Dift_VDF_Elem_base(const Iterateur_VDF_base& iter_base) : Op_Dift_VDF_base(iter_base) { }
  double calculer_dt_stab_elem() const;
  double calculer_dt_stab_elem_axi() const;
  double calculer_dt_stab_elem_var_axi() const;
  inline void modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const override { Op_VDF_Elem::modifier_pour_Cl(iter->domaine(), iter->domaine_Cl(), matrice, secmem); }
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override;

protected:
  virtual double alpha_(const int ) const = 0;
};

#endif /* Op_Dift_VDF_Elem_base_included */
