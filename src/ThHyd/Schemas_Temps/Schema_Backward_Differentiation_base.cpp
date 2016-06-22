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
// File:        Schema_Backward_Differentiation_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_Backward_Differentiation_base.h>
#include <Equation.h>
#include <Equation_base.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Probleme_Couple.h>
#include <Milieu_base.h>
#include <Param.h>
#include <Debog.h>
#include <communications.h>
#include <Matrice_Morse.h>

Implemente_base(Schema_Backward_Differentiation_base,"Schema_Backward_Differentiation_base",Schema_Implicite_Multi_TimeStep_base);


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
Sortie& Schema_Backward_Differentiation_base::printOn(Sortie& s) const
{
  return  Schema_Implicite_Multi_TimeStep_base::printOn(s);
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
Entree& Schema_Backward_Differentiation_base::readOn(Entree& s)
{
  return Schema_Implicite_Multi_TimeStep_base::readOn(s);
}

double Schema_Backward_Differentiation_base::changer_temps(Equation_base& eqn, const double& temps)
{
  eqn.inconnue().valeur().Champ_base::changer_temps(temps);
  return temps;
}

void Schema_Backward_Differentiation_base::update_time_derivative(Equation_base& eqn, const DoubleTab& data)
{
  //Nothing to do
}

void Schema_Backward_Differentiation_base::mettre_a_jour_equation(Equation_base& eqn, const double& temps)
{
  eqn.inconnue().mettre_a_jour(temps);
}

void Schema_Backward_Differentiation_base::compute_coefficients(double time_step, const DoubleTab& times) const
{
  compute_backward_differentiation_coefficients(time_step,times);
}

//To take into account pressure constraint
void Schema_Backward_Differentiation_base::store_equation_parameters(Equation_base& eqn, DoubleTab& stored_parameters)
{
  if (nb_pas_dt()>nb_pas_dt_seuil())
    {
      if (sub_type(Navier_Stokes_std,eqn))
        {
          Navier_Stokes_std& navier_stokes = ref_cast(Navier_Stokes_std,eqn);
          stored_parameters = navier_stokes.pression().valeurs(); //store pressure at time tn
        }
    }
}

//To take into account pressure constraint
void Schema_Backward_Differentiation_base::modify_equation_parameters(Equation_base& eqn, DoubleTab& stored_parameters)
{
  if (nb_pas_dt()>nb_pas_dt_seuil())
    {
      if (sub_type(Navier_Stokes_std,eqn))
        {
          Navier_Stokes_std& navier_stokes = ref_cast(Navier_Stokes_std,eqn);

          DoubleTab& pressure = navier_stokes.pression().valeurs(); //pressure at time tnplus1 that is to say P(n+1)
          DoubleTab  delta_pressure(pressure);

          delta_pressure -= stored_parameters; //contains P(n+1) - P(n)
          delta_pressure /= coefficients()[nb_valeurs_passees()+1]; //take into account time scheme coefficient
          pressure        = stored_parameters;
          pressure       += delta_pressure;
        }
    }
}

void Schema_Backward_Differentiation_base::add_multi_timestep_data(const Equation_base& eqn, Matrice_Base& mat_morse, DoubleTab& secmem) const
{
  double time_step               =  pas_de_temps();
  double effective_time_step     = -1.;

  // Remark for "pen" interger : we do not penalize matrices and right members even in the case
  // of Dirichlet or Symetrie boundary conditions
  int pen    = 0;
  int offset = 0;
  int i      = 0;

  effective_time_step     = coefficients()[nb_valeurs_passees()+1] * time_step;

  //Modification due to present time and past times
  DoubleTab dudt(eqn.inconnue().valeurs());
  DoubleTab tmp(dudt);

  dudt = 0.;
  for (i=0; i<nb_valeurs_passees()+1; ++i)
    {
      offset  = nb_valeurs_passees()-i + 1;//due to Initialiser_Champs() offset
      tmp     = eqn.inconnue().passe(offset);
      tmp    *= coefficients()[i] ;
      dudt   += tmp; //past times and present time if i==nb_valeurs_passees()
    }

  eqn.solv_masse().ajouter_masse(effective_time_step,secmem,dudt,pen);
  eqn.solv_masse().ajouter_masse(effective_time_step,mat_morse,pen);
}


