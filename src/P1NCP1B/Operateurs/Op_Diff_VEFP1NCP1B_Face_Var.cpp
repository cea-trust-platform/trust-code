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
// File:        Op_Diff_VEFP1NCP1B_Face_Var.cpp
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_VEFP1NCP1B_Face_Var.h>
#include <Champ_P1NC.h>
#include <Champ_Q1NC.h>
#include <Champ_Uniforme.h>
#include <Les_Cl.h>
#include <Ref_Champ_P1NC.h>
#include <Ref_Champ_Q1NC.h>
#include <Milieu_base.h>
#include <Debog.h>
#include <DoubleTrav.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>


Implemente_instanciable_sans_constructeur(Op_Diff_VEFP1NCP1B_Face_Var,"Op_Diff_VEFP1NCP1B_var_P1NC",Op_Diff_VEFP1NCP1B_Face);

Op_Diff_VEFP1NCP1B_Face_Var::Op_Diff_VEFP1NCP1B_Face_Var()
{
  declare_support_masse_volumique(1);
}
//// printOn
//

Sortie& Op_Diff_VEFP1NCP1B_Face_Var::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Diff_VEFP1NCP1B_Face_Var::readOn(Entree& s )
{
  return Op_Diff_VEFP1NCP1B_Face::readOn(s) ;
}
