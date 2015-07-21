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
// File:        Schema_Adams_Moulton_order_2.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////


//
// .DESCRIPTION class Schema_Adams_Moulton_order_2
//
// .SECTION voir aussi
//

#include <Schema_Adams_Moulton_order_2.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Probleme_Couple.h>
#include <Milieu_base.h>
#include <Param.h>
#include <Debog.h>
#include <communications.h>
#include <Matrice_Morse.h>
#include <string>

Implemente_instanciable(Schema_Adams_Moulton_order_2,"Schema_Adams_Moulton_order_2",Schema_Adams_Moulton_base);


//     printOn()
/////

Sortie& Schema_Adams_Moulton_order_2::printOn(Sortie& s) const
{
  return  Schema_Adams_Moulton_base::printOn(s);
}

//// readOn
//

Entree& Schema_Adams_Moulton_order_2::readOn(Entree& s)
{
  return Schema_Adams_Moulton_base::readOn(s);
}

// Description:
//    Renvoie le nombre de valeurs temporelles a conserver.
int Schema_Adams_Moulton_order_2::nb_valeurs_temporelles() const
{
  return 3 ;
}

int Schema_Adams_Moulton_order_2::nb_valeurs_passees() const
{
  return 0;
}

int Schema_Adams_Moulton_order_2::nb_pas_dt_seuil() const
{
  return 0;
}

void Schema_Adams_Moulton_order_2::compute_adams_moulton_coefficients(double time_step, const DoubleTab& times) const
{
  assert(times.size_array() == 2); //past, present and future times, stored in ascending order inside "times" table

  if ( adams_moulton_coefficients_.size_array() != 2)
    {
      adams_moulton_coefficients_.resize(2,Array_base::NOCOPY_NOINIT);
    }

  adams_moulton_coefficients_[0] = 0.5;
  adams_moulton_coefficients_[1] = 0.5;
}
