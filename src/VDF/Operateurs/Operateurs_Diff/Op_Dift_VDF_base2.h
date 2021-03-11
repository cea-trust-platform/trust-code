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
// File:        Op_Dift_VDF_base2.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Operateurs_Diff
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Dift_VDF_base2_included
#define Op_Dift_VDF_base2_included

#include <Op_Diff_VDF_base.h>
#include <Op_Diff_Turbulent_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION class  Op_Dift_VDF_base2
//
//////////////////////////////////////////////////////////////////////////////

class Op_Dift_VDF_base2 : public Op_Diff_VDF_base, public Op_Diff_Turbulent_base
{

  Declare_base(Op_Dift_VDF_base2);

public:

  inline Op_Dift_VDF_base2(const Iterateur_VDF_base& );
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  void contribuer_au_second_membre(DoubleTab& ) const;
};

inline Op_Dift_VDF_base2::Op_Dift_VDF_base2(const Iterateur_VDF_base& iter_base)
  : Op_Diff_VDF_base(iter_base)
{}

#endif /* Op_Dift_VDF_base2_included */
