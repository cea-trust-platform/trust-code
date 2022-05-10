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
// File:        Champ_front_vide.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_front_vide_included
#define Champ_front_vide_included

#include <Champ_front_base.h>



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front_vide
//     Classe derivee de Champ_front_base qui permet d'avoir un objet champ_front defini pour que le calcul tourne car il y a beaucoup d'appels a cond_lim_base.champ_front() dans les classes du dessus mais qui pese pas lourd
//     Classe inspiree de Champ_front_uniforme
// .SECTION voir aussi
//     Champ_front_base Champ_Uniforme
//////////////////////////////////////////////////////////////////////////////
class Champ_front_vide : public Champ_front_base
{
  Declare_instanciable(Champ_front_vide);

public:

  DoubleTab& valeurs_au_temps(double temps) override { Process::exit("Impossible d'appeler les valeurs d'un champ_fronc_vide"); return les_valeurs->valeurs();};
  const DoubleTab& valeurs_au_temps(double temps) const override { Process::exit("Impossible d'appeler les valeurs d'un champ_fronc_vide"); return les_valeurs->valeurs();};
  int avancer(double temps) override {return 1;};
  int reculer(double temps) override {return 1;};
  Champ_front_base& affecter_(const Champ_front_base& ch) override {return *this;};
  void valeurs_face(int,DoubleVect&) const override {};
  void changer_temps_futur(double temps,int i) override {};
};

#endif
