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
// File:        Discretiser.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Discretiser_included
#define Discretiser_included




#include <Interprete.h>
#include <Domaine.h>
#include <Schema_Temps.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Discretiser
//     Discretise un probleme:
//     {DANS LE JEU DE DONNEES] Discretiser pb dis
//     Discretise le probleme "pb" avec la discretisation "dis"
//     ou "dis" est une Discretisation_base VDF ou VEF pour le moment.
// .SECTION voir aussi
//     Interprete Discretisation_base
//////////////////////////////////////////////////////////////////////////////
class Discretiser : public Interprete
{
  Declare_instanciable(Discretiser);
public :
  Entree& interpreter(Entree&);
protected :
  static int is_deja_appele;
};

#endif
