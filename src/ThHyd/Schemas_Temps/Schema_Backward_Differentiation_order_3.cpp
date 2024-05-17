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

#include <Schema_Backward_Differentiation_order_3.h>

#include <Equation_base.h>
#include <Probleme_base.h>
#include <Probleme_Couple.h>
#include <Milieu_base.h>
#include <Param.h>

#include <communications.h>
#include <Matrice_Morse.h>

Implemente_instanciable(Schema_Backward_Differentiation_order_3,"Schema_Backward_Differentiation_order_3",Schema_Backward_Differentiation_base);
// XD schema_backward_differentiation_order_3 schema_implicite_base schema_backward_differentiation_order_3 -1 not_set
// XD attr facsec_max floattant facsec_max 1 Maximum ratio allowed between time step and stability time returned by CFL condition. The initial ratio given by facsec keyword is changed during the calculation with the implicit scheme but it couldn\'t be higher than facsec_max value.NL2 Warning: Some implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3 needs facsec=facsec_max=1. NL2 Advice:NL2 The calculation may start with a facsec specified by the user and increased by the algorithm up to the facsec_max limit. But the user can also choose to specify a constant facsec (facsec_max will be set to facsec value then). Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic only or thermal hydraulic with forced convection and low coupling between velocity and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic with forced convection and strong coupling between velocity and temperature (Boussinesq value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection, facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8) as if the scheme was unconditionally stableNL2These values can also be used as rule of thumb for initial facsec with a facsec_max limit higher.



/*! @brief Simple appel a: Schema_Temps_base::printOn(Sortie& ) Ecrit le schema en temps sur un flot de sortie.
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Schema_Backward_Differentiation_order_3::printOn(Sortie& s) const
{
  return  Schema_Backward_Differentiation_base::printOn(s);
}


/*! @brief Lit le schema en temps a partir d'un flot d'entree.
 *
 * Simple appel a: Schema_Temps_base::readOn(Entree& )
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Schema_Backward_Differentiation_order_3::readOn(Entree& s)
{
  return Schema_Backward_Differentiation_base::readOn(s);
}

////////////////////////////////
//                            //
// Caracteristiques du schema //
//                            //
////////////////////////////////


/*! @brief Renvoie le nombre de valeurs temporelles a conserver.
 *
 * Ici : n-1, n et n+1 donc 3.
 *
 */
int Schema_Backward_Differentiation_order_3::nb_valeurs_temporelles() const
{
  return 5 ;
}

int Schema_Backward_Differentiation_order_3::nb_valeurs_passees() const
{
  return 2;
}

int Schema_Backward_Differentiation_order_3::nb_pas_dt_seuil() const
{
  return 2;
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////

void Schema_Backward_Differentiation_order_3::compute_backward_differentiation_coefficients(double time_step, const DoubleTab& times) const
{
  assert(times.size_array() == 4); //past, present and future times, stored in ascending order inside "times" table

  if (backward_differentiation_coefficients_.size_array() != 4)
    {
      backward_differentiation_coefficients_.resize(4,RESIZE_OPTIONS::NOCOPY_NOINIT);
    }

  double alpha_1 = 1.;                            // for times[2] which means present time
  double alpha_2 = time_step/(times[3]-times[1]); // for times[1] which means past time that is to say t(n-1)
  double alpha_3 = time_step/(times[3]-times[0]); // for times[0] which means past past time that is to say t(n-2)

  backward_differentiation_coefficients_[3] = 1./(alpha_1+alpha_2+alpha_3);                                                                  //for times[3] which means future time
  backward_differentiation_coefficients_[2] = alpha_1*backward_differentiation_coefficients_[3]/((1.-alpha_2/alpha_1)*(1.-alpha_3/alpha_1)); //for times[2] which means past time
  backward_differentiation_coefficients_[1] = alpha_2*backward_differentiation_coefficients_[3]/((1.-alpha_1/alpha_2)*(1.-alpha_3/alpha_2)); //for times[1] which means past time
  backward_differentiation_coefficients_[0] = alpha_3*backward_differentiation_coefficients_[3]/((1.-alpha_1/alpha_3)*(1.-alpha_2/alpha_3)); //for times[0] which means past past time
}

