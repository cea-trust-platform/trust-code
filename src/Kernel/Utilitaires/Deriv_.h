/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Deriv_.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Deriv__included
#define Deriv__included
#include <O_U_Ptr.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//   Type de base de tous les types DERIV(__TYPE__).
//   La classe est vide, elle sert de couche intermediaire pour
//   verifier si un objet est de type DERIV.
//
//   30/06/2004, B.Mathieu : la template version n'est plus a jour.
//
// .SECTION voir aussi
//    TpltDeriv O_U_Ptr
//////////////////////////////////////////////////////////////////////////////
class Deriv_ : public O_U_Ptr
{
  Declare_base(Deriv_);
public:
  void detach();
  Objet_U * typer(const char * nom_type);
  int associer_(Objet_U&);
};
#endif // DERIV__H
