/****************************************************************************
* Copyright (c) 2024, CEA
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


/*! @brief class Schema_Adams_Moulton_order_2
 *
 */

#include <Schema_Adams_Moulton_order_2.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Probleme_Couple.h>
#include <Milieu_base.h>
#include <Param.h>
#include <communications.h>
#include <Matrice_Morse.h>
#include <string>

Implemente_instanciable(Schema_Adams_Moulton_order_2,"Schema_Adams_Moulton_order_2",Schema_Adams_Moulton_base);
// XD schema_adams_moulton_order_2 schema_implicite_base schema_adams_moulton_order_2 -1 not_set
// XD attr facsec_max floattant facsec_max 1 Maximum ratio allowed between time step and stability time returned by CFL condition. The initial ratio given by facsec keyword is changed during the calculation with the implicit scheme but it couldn\'t be higher than facsec_max value.NL2 Warning: Some implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3 needs facsec=facsec_max=1. NL2 Advice:NL2 The calculation may start with a facsec specified by the user and increased by the algorithm up to the facsec_max limit. But the user can also choose to specify a constant facsec (facsec_max will be set to facsec value then). Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic only or thermal hydraulic with forced convection and low coupling between velocity and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic with forced convection and strong coupling between velocity and temperature (Boussinesq value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection, facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8) as if the scheme was unconditionally stableNL2These values can also be used as rule of thumb for initial facsec with a facsec_max limit higher.



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

/*! @brief Renvoie le nombre de valeurs temporelles a conserver.
 *
 */
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
      adams_moulton_coefficients_.resize(2,RESIZE_OPTIONS::NOCOPY_NOINIT);
    }

  adams_moulton_coefficients_[0] = 0.5;
  adams_moulton_coefficients_[1] = 0.5;
}
