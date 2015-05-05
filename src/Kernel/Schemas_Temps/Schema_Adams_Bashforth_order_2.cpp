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
// File:        Schema_Adams_Bashforth_order_2.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_Adams_Bashforth_order_2.h>
#include <Equation.h>
#include <Debog.h>
#include <DoubleTrav.h>

Implemente_instanciable(Schema_Adams_Bashforth_order_2,"Schema_Adams_Bashforth_order_2",Schema_Adams_Bashforth_base);


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
Sortie& Schema_Adams_Bashforth_order_2::printOn(Sortie& s) const
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
Entree& Schema_Adams_Bashforth_order_2::readOn(Entree& s)
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
//    Ici : n-1, n et n+1 donc 3.
int Schema_Adams_Bashforth_order_2::nb_valeurs_temporelles() const
{
  return 3 ;
}

// Description:
//    Renvoie le nombre de pas de temps strictement au dela duquel on applique le schema d Adams-Bahshforth.
//    Ici : on a besoin d'au moins 1 temps du passe donc 0
inline int Schema_Adams_Bashforth_order_2::nb_pas_dt_seuil() const
{
  return 0 ;
}

// Description:
//    Renvoie le nombre de valeurs temporelles du passe.
//    Ici : n-1 donc 1.
inline int Schema_Adams_Bashforth_order_2::nb_valeurs_passees() const
{
  return 1 ;
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////

void Schema_Adams_Bashforth_order_2::compute_adams_bashforth_coefficients(double time_step, const DoubleTab& times)
{
  assert(times.size_array() == 3); //past, present and future times, stored in ascending order inside "times" table

  if (coefficients().size_array() != 2)
    {
      coefficients().resize(2,Array_base::NOCOPY_NOINIT);
    }

  double past_time          = times[0];
  double present_time       = times[1];
  double future_time        = times[2];
  double past_time_step     = present_time-past_time;

  coefficients()[1] = 1./past_time_step;
  coefficients()[1] *= (future_time+present_time-2*past_time);
  coefficients()[1] *= 0.5;
  assert(coefficients()[1] >= 0.);

  coefficients()[0] = 1-coefficients()[1];
  assert(coefficients()[0] <= 1.);
}
