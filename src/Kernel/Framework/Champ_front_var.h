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


#ifndef Champ_front_var_included
#define Champ_front_var_included

#include <Champ_front_base.h>

////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front_var
//     Classe derivee de Champ_front_base qui represente un champ sur
//     une frontiere variable en espace (non uniforme).
//     Le DoubleTab est dimensionne au nombre de faces de la frontiere
//     par la methode initialiser et possede un espace virtuel.
//     Chaque modification du tableau de valeurs doit donc etre suivie
//     d'un appel a echange_espace_virtuel().
//     Les Champ_front_var sont classes suivant qu'ils sont
//     stationnaires (Champ_front_var_stationnaire) ou non
//     (Champ_front_var_instationnaire).
//     Dans le premier cas, le tableau de valeurs est rempli une fois
//     pour toutes par la methode initialiser. Dans le deuxieme, il
//     est aussi recalcule a chaque appel a mettre_a_jour.
// .SECTION voir aussi
//     Champ_front_base Champ_Var
//////////////////////////////////////////////////////////////////////////////
class Champ_front_var : public Champ_front_base
{
  Declare_base(Champ_front_var);
public:
  int initialiser(double temps, const Champ_Inc_base& inco) override;
};

#endif
