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
// File:        Schema_Adams_Bashforth_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_Adams_Bashforth_base.h>
#include <Equation.h>
#include <Debog.h>
#include <DoubleTrav.h>
#include <Probleme_base.h>

Implemente_base(Schema_Adams_Bashforth_base,"Schema_Adams_Bashforth_base",Schema_Explicite_Multi_TimeStep_base);


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
Sortie& Schema_Adams_Bashforth_base::printOn(Sortie& s) const
{
  return Schema_Explicite_Multi_TimeStep_base::printOn(s);
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
Entree& Schema_Adams_Bashforth_base::readOn(Entree& s)
{
  return Schema_Explicite_Multi_TimeStep_base::readOn(s) ;
}

void Schema_Adams_Bashforth_base::associer_pb(const Probleme_base& un_probleme)
{
  Schema_Explicite_Multi_TimeStep_base::associer_pb(un_probleme);
  // Loop on equations
  for (int i=0; i<pb_base().nombre_d_equations(); i++)
    pb_base().equation(i).set_calculate_time_derivative(1);
}

void Schema_Adams_Bashforth_base::compute_coefficients(double time_step, const DoubleTab& times)
{
  compute_adams_bashforth_coefficients(time_step,times);
}

void Schema_Adams_Bashforth_base::modifier_second_membre(const Equation_base& eqn, DoubleTab& secmem)
{
  if (nb_pas_dt()> nb_pas_dt_seuil())
    {

      const Equation_base& eqn_bis   = ref_cast(Equation_base,eqn);
      int            i         = 0;
      int            offset    = 0;
      double         time_step = pas_de_temps(); //tn+1-tn

      DoubleTab times(nb_valeurs_temporelles());

      //Building of a time-in-ascending-order table
      assert(nb_valeurs_passees()+2 == nb_valeurs_temporelles());

      for (i=0; i<nb_valeurs_passees(); ++i)
        {
          offset   =  nb_valeurs_passees()-i;
          times[i] =  eqn_bis.inconnue().valeur().recuperer_temps_passe(offset); //past
        }
      times[nb_valeurs_passees()]   = eqn_bis.inconnue().valeur().recuperer_temps_futur(0); //present
      times[nb_valeurs_passees()+1] = eqn_bis.inconnue().valeur().recuperer_temps_futur(1); //future

      compute_coefficients(time_step,times);

      //Modification due to present time and past times
      secmem*=coefficients()[nb_valeurs_passees()]; //present time

      DoubleTrav dudt(eqn.inconnue().valeurs());
      DoubleTrav tmp(dudt);
      // Compute sum(coeff*dudt)
      for (i=0; i<nb_valeurs_passees(); ++i)
        {
          offset = nb_valeurs_passees()-i;
          tmp = eqn.derivee_en_temps().passe(offset);
          tmp *= coefficients()[i];
          dudt += tmp; //past time
        }
      // secmem+=Mdu/dt
      eqn.solv_masse().ajouter_masse(1.,secmem,dudt,0);
    }
}

