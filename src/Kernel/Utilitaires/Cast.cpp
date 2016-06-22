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
// File:        Cast.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Cast.h>
#include <assert.h>
#include <Type_info.h>
#include <Process.h>
#include <Nom.h>

#include <iostream>
using std::cerr;
using std::endl;
using std::flush;

int* bad_ref_cast(const Type_info* t1, const Type_info* t2)
{
  cerr << "Type error during a ref_cast instruction !" << endl;
  cerr << "The class " << t2->name() << " is not a subclass of " << t1->name() << endl;
  if ((Nom)t2->name()=="Deriv")
    {
      cerr << "It seems somewhere in the code ref_cast(" << t1->name() << ",object)" << endl;
      cerr << "is used with object being an instance of a generic class (Deriv)." << endl;
      cerr << "It should be written ref_cast(" << t1->name() << ",object.valeur())" << endl;
      cerr << "Check your code, or contact TRUST support." << endl;
    }
  cerr << flush;
  Process::exit();
  return 0;
}
