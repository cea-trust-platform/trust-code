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
// File:        Champ_front_calc_interne.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_front_calc_interne_included
#define Champ_front_calc_interne_included

#include <Champ_front_calc.h>
#include <IntTab.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front_calc_interne
//     Classe derivee de Champ_front_calc qui represente
//     le transfert d'un champ_inc d'un cote a l'autre d'une paroi interne.
// .SECTION voir aussi
//     Champ_front_calc Echange_interne_impose
//////////////////////////////////////////////////////////////////////////////
class Champ_front_calc_interne : public Champ_front_calc
{

  Declare_instanciable_sans_constructeur(Champ_front_calc_interne);

public:
  Champ_front_calc_interne();

  virtual void completer();
  void mettre_a_jour(double temps);
  const IntTab& face_map() const
  {
    return face_map_;
  }

protected:
  IntTab face_map_;
};

#endif
