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
// File:        ModifyDomaineAxi1D.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/33
//
//////////////////////////////////////////////////////////////////////////////
#include <ModifyDomaineAxi1D.h>
#include <DomaineAxi1d.h>

Implemente_instanciable(ModifyDomaineAxi1D,"ModifyDomaineAxi1D|Convert_1D_to_1Daxi",Interprete);
// XD modifydomaineAxi1d interprete modifydomaineAxi1d 0 Convert a 1D mesh to 1D axisymmetric mesh
// XD attr dom chaine dom 0 not_set
// XD attr bloc bloc_lecture bloc 0 not_set
//
// printOn et readOn

Sortie& ModifyDomaineAxi1D::printOn(Sortie& s ) const
{
  return s;
}

Entree& ModifyDomaineAxi1D::readOn(Entree& is )
{
  return is;
}
Entree& ModifyDomaineAxi1D::interpreter(Entree& is )
{
  Nom dom;
  is >> dom;
  is >> ref_cast(DomaineAxi1d, objet(dom));
  return is;
}



