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

#ifndef LecFicDiffuseBin_included
#define LecFicDiffuseBin_included

#include <LecFicDiffuse.h>

class Objet_U;


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Read a file in binary format.
//    This class implements the operators and virtual methods of the EFichier class in the following way:
//    The file to read is physically locate on the disk of the machine hosting the master task of the Trio-U application (the process of rank 0 in the "tous" group)
//    and each item read from this file is published to all other processes of the "tous" group.
//    It is the same for the methods of inspection of the state of a file.
// .SECTION see also
//////////////////////////////////////////////////////////////////////////////
class LecFicDiffuseBin : public LecFicDiffuse
{
  Declare_instanciable_sans_constructeur(LecFicDiffuseBin);
  // the master reads the file et propagates the information
public:
  LecFicDiffuseBin()
  {
    set_bin(1);
  };
  LecFicDiffuseBin(const char* name,IOS_OPEN_MODE mode=ios::in)
  {
    set_bin(1);
    ouvrir(name,mode);
  };
};

#endif
