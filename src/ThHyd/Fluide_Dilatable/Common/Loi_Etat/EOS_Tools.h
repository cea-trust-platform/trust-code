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
// File:        EOS_Tools.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Common/Loi_Etat
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EOS_Tools_included
#define EOS_Tools_included

#include <EOS_Tools_base.h>
#include <Deriv.h>

////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
// class EOS_Tools
//    Cette classe est la derivee de la classe EOS_Tools_base
//
// .SECTION voir aussi
// EOS_Tools_base
////////////////////////////////////////////////////////////////

Declare_deriv(EOS_Tools_base);

class EOS_Tools : public DERIV(EOS_Tools_base)
{
  Declare_instanciable(EOS_Tools);
public:
  void typer(const Nom&);
};

#endif /* EOS_Tools_included */
