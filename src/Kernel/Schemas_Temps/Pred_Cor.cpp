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
// File:        Pred_Cor.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Pred_Cor.h>
#include <Operateur_Conv.h>

Implemente_instanciable(Pred_Cor,"Schema_Predictor_Corrector",Schema_Temps_base);


//     printOn()
/////

Sortie& Pred_Cor::printOn(Sortie& s) const
{
  return  Schema_Temps_base::printOn(s);
}

//// readOn
//

Entree& Pred_Cor::readOn(Entree& s)
{
  return Schema_Temps_base::readOn(s) ;
}


////////////////////////////////
//                            //
// Caracteristiques du schema //
//                            //
////////////////////////////////


// Description:
//    Renvoie le nombre de valeurs temporelles a conserver.
//    Ici : n et n+1, donc 2.
int Pred_Cor::nb_valeurs_temporelles() const
{
  return 2 ;
}

// Description:
//    Renvoie le nombre de valeurs temporelles futures.
//    Ici : n+1, donc 1.
int Pred_Cor::nb_valeurs_futures() const
{
  return 1 ;
}

// Description:
//    Renvoie le le temps a la i-eme valeur future.
//    Ici : t(n+1)
double Pred_Cor::temps_futur(int i) const
{
  assert(i==1);
  return temps_courant()+pas_de_temps();
}

// Description:
//    Renvoie le le temps le temps que doivent rendre les champs a
//    l'appel de valeurs()
//    Ici : t(n+1)
double Pred_Cor::temps_defaut() const
{
  return temps_courant()+pas_de_temps();
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////


int Pred_Cor::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{
  DoubleTab& xi=eqn.inconnue().valeurs();
  DoubleTab& xipls1=eqn.inconnue().futur();

  DoubleTab dudt(xi);

  // Etape PREDICTOR

  eqn.zone_Cl_dis()->imposer_cond_lim(eqn.inconnue(),temps_courant()+pas_de_temps());

  // On tourne la roue pour que les operateurs utilisent les champs au temps futur
  eqn.inconnue().avancer();
  eqn.derivee_en_temps_inco(dudt);

  // Xi+1=Xi+dt_*dX/dt(Xi)
  xipls1 = dudt;
  xipls1 *= dt_;
  xipls1 += xi;

  xipls1.echange_espace_virtuel();

  // Etape CORRECTOR
  DoubleTab dudtpls1(xipls1);
  eqn.derivee_en_temps_inco(dudtpls1);
  eqn.inconnue().reculer();

  dudtpls1 += dudt;
  dudtpls1 *= 0.5;

  // Xi+1=Xi+dt_*0.5*(dX/dt(Xi)+dX/dt(Xi+1))
  xipls1  = dudtpls1 ;
  xipls1  *= dt_;
  xipls1  += xi ;

  eqn.zone_Cl_dis()->imposer_cond_lim(eqn.inconnue(),temps_courant()+pas_de_temps());
  update_critere_statio(dudtpls1, eqn);

  return 1;
}
