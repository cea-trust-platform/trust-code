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
// File:        Schema_Backward_Differentiation_order_2.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_Backward_Differentiation_order_2.h>
#include <Equation.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Probleme_Couple.h>
#include <Milieu_base.h>
#include <Param.h>
#include <Debog.h>
#include <communications.h>
#include <Matrice_Morse.h>

Implemente_instanciable(Schema_Backward_Differentiation_order_2,"Schema_Backward_Differentiation_order_2",Schema_Backward_Differentiation_base);


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
Sortie& Schema_Backward_Differentiation_order_2::printOn(Sortie& s) const
{
  return  Schema_Backward_Differentiation_base::printOn(s);
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
Entree& Schema_Backward_Differentiation_order_2::readOn(Entree& s)
{
  return Schema_Backward_Differentiation_base::readOn(s);
}

////////////////////////////////
//                            //
// Caracteristiques du schema //
//                            //
////////////////////////////////


// Description:
//    Renvoie le nombre de valeurs temporelles a conserver.
//    Ici : n-1, n et n+1 donc 3.
int Schema_Backward_Differentiation_order_2::nb_valeurs_temporelles() const
{
  return 4 ;
}

int Schema_Backward_Differentiation_order_2::nb_valeurs_passees() const
{
  return 1;
}

int Schema_Backward_Differentiation_order_2::nb_pas_dt_seuil() const
{
  return 1;
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////

void Schema_Backward_Differentiation_order_2::compute_backward_differentiation_coefficients(double time_step, const DoubleTab& times) const
{
  assert(times.size_array() == 3); //past, present and future times, stored in ascending order inside "times" table

  if (backward_differentiation_coefficients_.size_array() != 3)
    {
      backward_differentiation_coefficients_.resize(3,Array_base::NOCOPY_NOINIT);
    }

  double alpha_0 = time_step/(times[2]-times[0]); // for times[0] which means past time
  double alpha_1 = 1.;                            // for times[1] which means present time

  backward_differentiation_coefficients_[2] = 1./(alpha_0+alpha_1);                                                    //for times[2] which means future time
  backward_differentiation_coefficients_[1] = alpha_1*backward_differentiation_coefficients_[2]/(1.-alpha_0/alpha_1); //for times[1] which means present time
  backward_differentiation_coefficients_[0] = alpha_0*backward_differentiation_coefficients_[2]/(1.-alpha_1/alpha_0); //for times[0] which means past time
}

