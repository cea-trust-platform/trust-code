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
// File:        Ch_front_var_stationnaire.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#include <Ch_front_var_stationnaire.h>

Implemente_base(Ch_front_var_stationnaire,"Ch_front_var_stationnaire",Champ_front_var);

Sortie& Ch_front_var_stationnaire::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Ch_front_var_stationnaire::readOn(Entree& s)
{
  return s ;
}


Champ_front_base& Ch_front_var_stationnaire::affecter_(const Champ_front_base& ch)
{
  return *this;
}

// Description:
//    Renvoie les valeurs sans s'occuper du temps puisque le champ
//    est stationnaire.
DoubleTab& Ch_front_var_stationnaire::valeurs_au_temps(double temps)
{
  return les_valeurs->valeurs();
}

// Description:
//    Renvoie les valeurs sans s'occuper du temps puisque le champ
//    est stationnaire.
const DoubleTab& Ch_front_var_stationnaire::valeurs_au_temps(double temps) const
{
  return les_valeurs->valeurs();
}

// Description:
//    Avance en temps : rien a faire pour un champ stationnaire !
int Ch_front_var_stationnaire::avancer(double temps)
{
  return 1;
}

// Description:
//    Recule en temps : rien a faire pour un champ stationnaire !
int Ch_front_var_stationnaire::reculer(double temps)
{
  return 1;
}

// Description :
// rien a faire pour un champ stationnaire !
void Ch_front_var_stationnaire::changer_temps_futur(double temps,int i)
{
}
