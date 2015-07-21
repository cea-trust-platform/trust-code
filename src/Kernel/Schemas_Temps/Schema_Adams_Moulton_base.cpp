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
// File:        Schema_Adams_Moulton_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////


//
// .DESCRIPTION class Schema_Adams_Moulton_base
//
// .SECTION voir aussi
//

#include <Schema_Adams_Moulton_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Probleme_Couple.h>
#include <Milieu_base.h>
#include <Param.h>
#include <Debog.h>
#include <communications.h>
#include <Matrice_Morse.h>
#include <string>

Implemente_base(Schema_Adams_Moulton_base,"Schema_Adams_Moulton_base",Schema_Implicite_Multi_TimeStep_base);


//     printOn()
/////

Sortie& Schema_Adams_Moulton_base::printOn(Sortie& s) const
{
  return  Schema_Implicite_base::printOn(s);
}

//// readOn
//

Entree& Schema_Adams_Moulton_base::readOn(Entree& s)
{
  return Schema_Implicite_Multi_TimeStep_base::readOn(s);
}

void Schema_Adams_Moulton_base::associer_pb(const Probleme_base& un_probleme)
{
  Schema_Implicite_Multi_TimeStep_base::associer_pb(un_probleme);
  // Loop on equations
  for (int i=0; i<pb_base().nombre_d_equations(); i++)
    pb_base().equation(i).set_calculate_time_derivative(1);
}

double Schema_Adams_Moulton_base::changer_temps(Equation_base& eqn, const double& temps)
{
  eqn.inconnue().valeur().Champ_base::changer_temps(temps);
  eqn.derivee_en_temps().valeur().Champ_base::changer_temps(temps);
  return temps;
}

void Schema_Adams_Moulton_base::update_time_derivative(Equation_base& eqn, const DoubleTab& data)
{
  DoubleTab& time_derivative = eqn.derivee_en_temps().futur();
  time_derivative = data;                                //here "data" is supposed to contain "vpoint" table
  modifier_second_membre_full_impl(eqn,time_derivative);
}

void Schema_Adams_Moulton_base::mettre_a_jour_equation(Equation_base& eqn, const double& temps)
{
  eqn.inconnue().mettre_a_jour(temps);
  eqn.derivee_en_temps().mettre_a_jour(temps);
}

void Schema_Adams_Moulton_base::compute_coefficients(double time_step, const DoubleTab& times) const
{
  compute_adams_moulton_coefficients(time_step,times);
}

void Schema_Adams_Moulton_base::add_multi_timestep_data(const Equation_base& eqn, Matrice_Base& mat_morse, DoubleTab& secmem) const
{
  Equation_base& eqn_bis         = ref_cast_non_const(Equation_base,eqn);
  double time_step               = pas_de_temps();
  double effective_time_step     = -1.;
  double scaling                 = -1.;

  // Remark for "pen" interger : we do not penalize matrices and right members even in the case
  // of Dirichlet or Symetrie boundary conditions
  int pen    = 0;
  int offset = 0;
  int i      = 0;

  effective_time_step     = coefficients()[nb_valeurs_passees()+1] * time_step;
  scaling                 = 1./coefficients()[nb_valeurs_passees()+1];

  //Modification due to pressure gradient
  DoubleTab correction(eqn.inconnue().valeurs());

  correction = 0.;
  eqn_bis.corriger_derivee_expl(correction);
  correction *= (scaling-1.);
  secmem     += correction;

  //Modification due to present time and past times
  DoubleTab dudt(eqn.inconnue().passe(1)); //due to Initialiser_Champs() offset
  DoubleTab tmp(dudt);

  for (i=0; i<nb_valeurs_passees()+1; ++i)
    {
      offset  = nb_valeurs_passees()-i; //present time if offset==0 else past times
      tmp     = eqn.derivee_en_temps().passe(offset);
      tmp    *= coefficients()[i] * time_step;
      dudt   += tmp;
    }

  eqn.solv_masse().ajouter_masse(effective_time_step,secmem,dudt,pen);
  eqn.solv_masse().ajouter_masse(effective_time_step,mat_morse,pen);
}

//To get du/dt table without pressure gradient and influences of past and present times
void Schema_Adams_Moulton_base::modifier_second_membre_full_impl(const Equation_base& eqn, DoubleTab& secmem)
{
  Equation_base& eqn_bis = ref_cast_non_const(Equation_base,eqn);

  DoubleTab correction(secmem);
  correction=0.;
  eqn_bis.corriger_derivee_expl(correction);
  eqn.solv_masse().appliquer(correction);
  secmem-=correction; //on enleve le gradient de pression

  if (nb_pas_dt()>nb_pas_dt_seuil())
    {
      int i = 0;
      int offset = 0;
      double scaling = 1./coefficients()[nb_valeurs_passees()+1];

      for (i=0; i<nb_valeurs_passees()+1; ++i )
        {
          offset      = nb_valeurs_passees() - i;
          correction  = eqn.derivee_en_temps().passe(offset);//present time if i==nb_valeurs_passees() else past times
          correction *= coefficients()[i];
          secmem     -= correction;
        }

      secmem *= scaling;
    }
}

//To take into account pressure constraint
void Schema_Adams_Moulton_base::store_equation_parameters(Equation_base& eqn, DoubleTab& stored_parameters)
{
  //Nothing to do
}

//To take into account pressure constraint
void Schema_Adams_Moulton_base::modify_equation_parameters(Equation_base& eqn, DoubleTab& stored_parameters)
{
  //Nothing to do
}
