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
// File:        Champ_front_lu.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_front_lu_included
#define Champ_front_lu_included

#include <Ch_front_var_stationnaire.h>


#include <Ref_Domaine.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front_lu
//     Classe derivee de Champ_front_var qui represente les
//     champs aux frontieres dont les valeurs sont lues dans un fichier de format :
//        nval
//        xi yi zi val_i
// .SECTION voir aussi
//     Champ_front_base Champ_front_var
//////////////////////////////////////////////////////////////////////////////
class Champ_front_lu : public Ch_front_var_stationnaire
{
  Declare_instanciable(Champ_front_lu);

public:

  virtual int initialiser(double temps, const Champ_Inc_base& inco);
  int is_bin(Nom name);

protected :
  REF(Domaine) mon_domaine;
  Nom nom_fic;
};

#endif

