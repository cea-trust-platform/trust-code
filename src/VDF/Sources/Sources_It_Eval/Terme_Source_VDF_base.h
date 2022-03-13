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
// File:        Terme_Source_VDF_base.h
// Directory:   $TRUST_ROOT/src/VDF/Sources/Sources_It_Eval
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Terme_Source_VDF_base_included
#define Terme_Source_VDF_base_included

#include <Iterateur_Source_VDF.h>
#include <Source_base.h>

class Terme_Source_VDF_base : public Source_base
{
  Declare_base(Terme_Source_VDF_base);
public:
  Terme_Source_VDF_base(const Iterateur_Source_VDF_base& iter_base) : iter(iter_base) { }
  void completer() override;
  inline DoubleTab& calculer(DoubleTab& resu) const override { return iter->calculer(resu); }
  inline void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override {}
  inline void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override { iter->ajouter(secmem); }
  inline int has_interface_blocs() const override
  {
    return 1;
  };

protected:
  Iterateur_Source_VDF iter;
};

#endif /* Terme_Source_VDF_base_included */
