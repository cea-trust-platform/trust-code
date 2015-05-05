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
// File:        Schema_Adams_Bashforth_order_3.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_Adams_Bashforth_order_3.h>
#include <Equation.h>
#include <Debog.h>
#include <DoubleTrav.h>


Implemente_instanciable(Schema_Adams_Bashforth_order_3,"Schema_Adams_Bashforth_order_3",Schema_Adams_Bashforth_base);


// Description:
//    Simple appel a: Schema_Temps_base::printOn(Sortie& )
//    Ecrit le schema en temps sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Schema_Adams_Bashforth_order_3::printOn(Sortie& s) const
{
  return  Schema_Adams_Bashforth_base::printOn(s);
}


// Description:
//    Lit le schema en temps a partir d'un flot d'entree.
//    Simple appel a: Schema_Temps_base::readOn(Entree& )
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Schema_Adams_Bashforth_order_3::readOn(Entree& s)
{
  return Schema_Adams_Bashforth_base::readOn(s) ;
}


////////////////////////////////
//                            //
// Caracteristiques du schema //
//                            //
////////////////////////////////


// Description:
//    Renvoie le nombre de valeurs temporelles a conserver.
//    Ici : n-2, n-1, n et n+1 donc 4.
int Schema_Adams_Bashforth_order_3::nb_valeurs_temporelles() const
{
  return 4 ;
}

// Description:
//    Renvoie le nombre de pas de temps strictement au dela duquel on applique le schema d Adams-Bahshforth.
//    Ici : on a besoin d'au moins 2 temps du passe donc 1
inline int Schema_Adams_Bashforth_order_3::nb_pas_dt_seuil() const
{
  return 1 ;
}

// Description:
//    Renvoie le nombre de valeurs temporelles du passe.
//    Ici : n-2 et n-1 donc 1.
inline int Schema_Adams_Bashforth_order_3::nb_valeurs_passees() const
{
  return 2 ;
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////

static double Ln02(const double& time_n_plus_1, const double& time_n, const double& time_n_minus_1, const double& time_n_minus_2)
{
  //Integration of first degree-2 lagrangian polynomial with Simpson's formulae
  //Ln02(time_n_minus_2) = 0
  //Ln02(time_n_minus_1) = 0
  //Ln02(time_n)         = 1
  //Integration bound : time_n and time_n_plus_1
  double result = 0.;

  double midpoint = (time_n_plus_1+time_n);
  midpoint *= 0.5;

  double scaling_coeff = (time_n_plus_1-time_n);
  scaling_coeff /= 6.;

  double denominator = (time_n-time_n_minus_1);
  denominator *= (time_n-time_n_minus_2);
  denominator = 1./denominator;

  double numerator = (time_n-time_n_minus_1)*(time_n-time_n_minus_2);
  numerator += 4*(midpoint-time_n_minus_1)*(midpoint-time_n_minus_2);
  numerator += (time_n_plus_1-time_n_minus_1)*(time_n_plus_1-time_n_minus_2);

  result = scaling_coeff * numerator * denominator;
  return result;
}

static double Ln12(const double& time_n_plus_1, const double& time_n, const double& time_n_minus_1, const double& time_n_minus_2)
{
  //Integration of second degree-2 lagrangian polynomial with Simpson's formulae
  //Ln12(time_n_minus_2) = 0
  //Ln12(time_n_minus_1) = 1
  //Ln12(time_n)         = 0
  //Integration bound : time_n and time_n_plus_1
  double result = 0.;

  double midpoint = (time_n_plus_1+time_n);
  midpoint *= 0.5;

  double scaling_coeff = (time_n_plus_1-time_n);
  scaling_coeff /= 6.;

  double denominator = (time_n_minus_1-time_n);
  denominator *= (time_n_minus_1-time_n_minus_2);
  denominator = 1./denominator;

  double numerator = 0.;
  numerator += 4*(midpoint-time_n)*(midpoint-time_n_minus_2);
  numerator += (time_n_plus_1-time_n)*(time_n_plus_1-time_n_minus_2);

  result = scaling_coeff * numerator * denominator;
  return result;
}

void Schema_Adams_Bashforth_order_3::compute_adams_bashforth_coefficients(double time_step, const DoubleTab& times)
{
  if (nb_pas_dt()<100 && limpr() && facteur_securite_pas()==1.0)
    {
      Cerr << finl;
      Cerr << "********************* Warning (printed only on the first 100 time steps) **********************"<< finl;
      Cerr << "The Adams Bashforth order 3 scheme is recommended with a facsec lower than 1.0 for example 0.5."<< finl;
      Cerr << "***********************************************************************************************"<< finl;
      // See $TRUST_ROOT/tests/Reference/Schema_Adams_Bashforth_order_3 test case.
    }

  assert(times.size_array() == 4); //2 past times, present and future times, stored in ascending order inside "times" table

  if (coefficients().size_array() != 3)
    {
      coefficients().resize(3,Array_base::NOCOPY_NOINIT);
    }

  double inv_time_step  = 1./time_step;
  double time_n_minus_2 = times[0];
  double time_n_minus_1 = times[1];
  double time_n         = times[2];
  double time_n_plus_1  = times[3];

  coefficients()[2] = Ln02(time_n_plus_1,time_n,time_n_minus_1,time_n_minus_2) * inv_time_step;
  coefficients()[1] = Ln12(time_n_plus_1,time_n,time_n_minus_1,time_n_minus_2) * inv_time_step;
  coefficients()[0] = 1-coefficients()[2]-coefficients()[1];
}
