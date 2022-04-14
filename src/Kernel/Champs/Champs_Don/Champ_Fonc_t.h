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
// File:        Champ_Fonc_t.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs/Champs_Don
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Fonc_t_included
#define Champ_Fonc_t_included

#include <Champ_Uniforme_inst.h>


#include <Vect_Parser_U.h>
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Champ_Fonc_t
//    Cette classe derivee de Champ_Uniforme_inst qui represente
//    un champ constant dans l'espace et fonction de la variable temps t.
// .SECTION voir aussi
//     Champ_Fonc_t
//////////////////////////////////////////////////////////////////////////////
class Champ_Fonc_t : public Champ_Uniforme_inst
{

  Declare_instanciable(Champ_Fonc_t);

public:

  void me_calculer(double t) override;
private:
  VECT(Parser_U) ft;
};

#endif
