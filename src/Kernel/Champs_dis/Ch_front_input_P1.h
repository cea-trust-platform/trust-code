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
// File:        Ch_front_input_P1.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Ch_front_input_P1_included
#define Ch_front_input_P1_included


#include <Ch_front_input.h>
#include <TRUSTTab.h>

//////////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     class Ch_front_input
//
//     Cette classe represente un champ accessible par setInputField
//     defini sur une frontiere avec une valeur par face.
//
// .SECTION voir aussi
//   Champ_Input_Proto
/////////////////////////////////////////////////////////////////////////////////


class Ch_front_input_P1 : public Ch_front_input
{
  Declare_instanciable(Ch_front_input_P1);

public:

  double valeur_au_temps_et_au_point(double temps,int som, double x,double y, double z,int comp) const override;
  inline int valeur_au_temps_et_au_point_disponible() const override
  {
    return 1;
  };
  void setValue(const TrioField& afield) override;
  void getTemplate(TrioField& afield) const override ;
  int initialiser(double temps, const Champ_Inc_base& inco) override;
protected:

  Roue_ptr les_valeurs_som;
};

#endif
