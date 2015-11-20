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
// File:        Schema_Adams_Moulton_order_3.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////


//
// .DESCRIPTION class Schema_Adams_Moulton_order_3
//
// .SECTION voir aussi
//

#include <Schema_Adams_Moulton_order_3.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Probleme_Couple.h>
#include <Milieu_base.h>
#include <Param.h>
#include <Debog.h>
#include <communications.h>
#include <Matrice_Morse.h>

Implemente_instanciable(Schema_Adams_Moulton_order_3,"Schema_Adams_Moulton_order_3",Schema_Adams_Moulton_base);


//     printOn()
/////

Sortie& Schema_Adams_Moulton_order_3::printOn(Sortie& s) const
{
  return  Schema_Adams_Moulton_base::printOn(s);
}

//// readOn
//

Entree& Schema_Adams_Moulton_order_3::readOn(Entree& s)
{
  return Schema_Adams_Moulton_base::readOn(s);
}

//On calcule la valeur moyenne entre borne_inf et borne_sup du polynome de lagrange de degre 2 :
//- qui vaut 0 en a et en b
//- qui vaut 1 en c
static double integrate_lagrangian_basis(double a, double b, double c, double borne_inf, double borne_sup)
{
  double result  = 0.;
  double scaling = (c-a)*(c-b);
  double dt      = borne_sup-borne_inf;

  double prod = a*b;
  double sum  = -0.5*(a+b);

  double tmp_sup = borne_sup*(prod+borne_sup*(borne_sup/3.+sum));
  double tmp_inf = borne_inf*(prod+borne_inf*(borne_inf/3.+sum));

  result = tmp_sup-tmp_inf;
  result/=(scaling*dt);

  return result;
}


// Description:
//    Renvoie le nombre de valeurs temporelles a conserver.
// 3 valeurs temporelles : tn-1, tn, tn+1
// 1 valeur de plus pour le fonctionnement des algorithmes de l'implicite
int Schema_Adams_Moulton_order_3::nb_valeurs_temporelles() const
{
  return 4 ;
}

int Schema_Adams_Moulton_order_3::nb_valeurs_passees() const
{
  return 1;
}

int Schema_Adams_Moulton_order_3::nb_pas_dt_seuil() const
{
  return 1;
}

void Schema_Adams_Moulton_order_3::compute_adams_moulton_coefficients(double time_step, const DoubleTab& times) const
{
  if (nb_pas_dt()<100 && limpr() && (facteur_securite_pas()>1.0 || facsec_max_>1.0))
    {
      Cerr << finl;
      Cerr << "********************* Warning (printed only on the first 100 time steps) ************************"<< finl;
      Cerr << "The Adams Moulton order 3 scheme is recommended with a facsec and facsec_max set to 1.0 or lower."<< finl;
      Cerr << "*************************************************************************************************"<< finl;
      // See $TRUST_ROOT/tests/Reference/Schema_Adams_Moulton_order_3 test case.
    }
  assert(times.size_array() == 3); //past, present and future times, stored in ascending order inside "times" table

  if ( adams_moulton_coefficients_.size_array() != 3)
    {
      adams_moulton_coefficients_.resize(3,Array_base::NOCOPY_NOINIT);
    }

  double past_time    = times[0];
  double present_time = times[1];
  double future_time  = times[2];

  adams_moulton_coefficients_[0] = integrate_lagrangian_basis(present_time,future_time,past_time,present_time,future_time);
  adams_moulton_coefficients_[1] = integrate_lagrangian_basis(past_time,future_time,present_time,present_time,future_time);
  adams_moulton_coefficients_[2] = 1 -  adams_moulton_coefficients_[1] -  adams_moulton_coefficients_[0];
}
