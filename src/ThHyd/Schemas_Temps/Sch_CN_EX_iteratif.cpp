/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Sch_CN_EX_iteratif.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Sch_CN_EX_iteratif.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Param.h>
#include <Process.h>
#include <SFichier.h>

Implemente_instanciable(Sch_CN_EX_iteratif,"Sch_CN_EX_iteratif",Sch_CN_iteratif);


Sortie& Sch_CN_EX_iteratif::printOn(Sortie& s) const
{
  return  Sch_CN_iteratif::printOn(s);
}


Entree& Sch_CN_EX_iteratif::readOn(Entree& s)
{
  seuil=0.05;
  niter_min=2;
  niter_max=6;
  niter_avg=3;
  facsec_max=20;
  omega=0.1;

  Schema_Temps_base::readOn(s) ;

  return s;
}

void Sch_CN_EX_iteratif::set_param(Param& param)
{
  param.ajouter("omega",&omega);
  Sch_CN_iteratif::set_param(param);
}

// Description:
// Ne tient compte que de l'equation de Navier Stokes du probleme (s'il y en a une)
// Les autres ne sont pas limitantes : elles font autant de sous-pas de temps que necessaire.
// Code honteusement recopie de Probleme_base::calculer_pas_de_temps()
//
void Sch_CN_EX_iteratif::mettre_a_jour_dt_stab()
{
  imprimer(Cout);
  //  dt_=dt_max_; La majoration par dt_max_ est faite dans corriger_dt_calcule
  dt_stab_=DMAXFLOAT;
  const Probleme_base& prob=pb_base();

  for(int i=0; i<prob.nombre_d_equations(); i++)
    {
      const Equation_base& eqn=prob.equation(i);
      if (sub_type(Navier_Stokes_std,eqn))
        {
          dt_stab_=std::min(dt_stab_,eqn.calculer_pas_de_temps());
          break; // Max une equation de Navier Stokes par probleme
        }
    }
}
bool Sch_CN_EX_iteratif::iterateTimeStepOnEquation(int i,bool& converged)
{
  Probleme_base& pb = pb_base();
  Equation_base& eqn = pb.equation(i);

  // Choix selon que l'equation est de type Navier_Stokes ou non
  if (sub_type(Navier_Stokes_std,eqn))
    return iterateTimeStepOnNS(i,converged);
  else
    return iterateTimeStepOnOther(i,converged);
}

// Tres similaire a Sch_CN_iteratif::iterateTimeStepOnEquation mais on applique
// un facteur d'attenuation omega entre les iterations.

bool Sch_CN_EX_iteratif::iterateTimeStepOnNS(int i,bool& converged)
{

  Probleme_base& pb = pb_base();
  Equation_base& eqn = pb.equation(i);

  double temps_intermediaire=temps_futur(1);
  double temps_final=temps_futur(2);
  double dt_intermediaire=temps_intermediaire-temps_courant();
  double dt_final=temps_final-temps_courant();

  DoubleTab& present = eqn.inconnue().valeurs();
  DoubleTab& intermediaire = eqn.inconnue()->valeurs(temps_intermediaire);
  DoubleTab& final = eqn.inconnue()->valeurs(temps_final);

  DoubleTab dudt(present);

  DoubleTab delta(intermediaire);
  DoubleTab old(intermediaire);

  // On impose les CLs Dirichlet au temps intermediaire.
  // En effet, les operateurs de diffusion n'utilisent que
  // l'inconnue et ne vont pas lire les CLs.
  // Cela permet en particulier d'avoir l'egalite des flux en pb
  // couple thermique VEF avec Chap_front_contact_VEF, meme avant convergence.
  // WEC :  /!\ la vitesse au temps intermediaire
  // n'est pas forcement a divergence nulle.
  eqn.zone_Cl_dis()->imposer_cond_lim(eqn.inconnue(),temps_intermediaire);

  // Calcul de la derivee dudt pour la valeur intermediaire de l'inconnue.
  // Bidouille : Comme les operateurs prennent par defaut le present,
  // on avance temporairement l'inconnue.

  eqn.inconnue().avancer();
  eqn.derivee_en_temps_inco(dudt);
  eqn.inconnue().reculer();

  // Mise a jour des valeurs de l'inconnue aux temps intermediaire et final
  // intermediaire = present + dt_intermediaire * dudt;
  // intermediaire = (1-omega)*new_intermediaire + omega*old_intermediaire
  // final =  present + dt_final * dudt;
  intermediaire = dudt;
  intermediaire*= dt_intermediaire;
  intermediaire+= present;
  intermediaire*= (1-omega);
  old*= omega;
  intermediaire+= old;
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(),temps_intermediaire);
  intermediaire.echange_espace_virtuel();
  final = dudt;
  final*= dt_final;
  final+= present;

  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(),temps_final);
  final.echange_espace_virtuel();

  delta*=-1;
  delta+=intermediaire; // delta = Contient u(n+1/2,p+1) - u(n+1/2,p)

  // Si l'equation a diverge
  if (divergence(present,intermediaire,delta,iteration))
    {
      converged=false;
      return false;
    }

  // Si l'equation a converge
  if (convergence(present,intermediaire,delta,iteration))
    {
      converged=true;
      delta=final;
      delta-=present;
      delta/=dt_final;
      update_critere_statio(delta, eqn);
    }
  else
    {
      set_stationnaire_atteint()=0;
      converged=false;
    }
  return true;

}

// Faire plusieurs pas de temps d'Euler explicite a facsec<=1
// On impose qu'un pas de temps tombe pile sur le temps intermediaire
// et un autre pile sur le temps final.

bool Sch_CN_EX_iteratif::iterateTimeStepOnOther(int i,bool& converged)
{
  Probleme_base& pb = pb_base();
  Equation_base& eqn = pb.equation(i);

  double dt_eq=eqn.calculer_pas_de_temps();

  double temps_intermediaire=temps_futur(1);
  double temps_final=temps_futur(2);
  double dt_1=temps_intermediaire-temps_courant();
  double dt_2=temps_final-temps_intermediaire;

  DoubleTab& present = eqn.inconnue().valeurs();
  DoubleTab& intermediaire = eqn.inconnue()->valeurs(temps_intermediaire);
  DoubleTab& final = eqn.inconnue()->valeurs(temps_final);

  DoubleTab dIdt(present); // Pour dimensionner.

  // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
  double nb_dt_1, nb_dt_2;
  modf(ceil(dt_1/dt_eq), &nb_dt_1); // Nombre de pas de temps dans le premier, le deuxieme intervalle.
  modf(ceil(dt_2/dt_eq), &nb_dt_2);

  double dt_eq_1=dt_1/nb_dt_1; // Duree des pas de temps dans le premier, le deuxieme intervalle.
  double dt_eq_2=dt_2/nb_dt_2;

  // Calculs sur le premier pas de temps
  intermediaire=present;
  for (double k=0.; k<nb_dt_1; k=k+1.)
    {

      // Calcul de la derivee dIdt sur la valeur intermediaire de l'inconnue.
      // Bidouille : Comme les operateurs prennent par defaut le present,
      // on avance temporairement l'inconnue.
      eqn.inconnue().avancer();
      eqn.derivee_en_temps_inco(dIdt);
      eqn.inconnue().reculer();

      // intermediaire = intermediaire + dt_eq_1 * dIdt
      dIdt*=dt_eq_1;
      intermediaire+= dIdt;
      intermediaire.echange_espace_virtuel();

    }

  // On impose les CLs au temps intermediaire
  eqn.zone_Cl_dis()->imposer_cond_lim(eqn.inconnue(),temps_intermediaire);


  // Calculs (idem) sur le deuxieme pas de temps
  final=intermediaire;
  for (double k=0.; k<nb_dt_2; k=k+1.)
    {

      // Calcul de la derivee dIdt sur la valeur finale de l'inconnue.
      eqn.inconnue().avancer();
      eqn.inconnue().avancer();
      eqn.derivee_en_temps_inco(dIdt);
      eqn.inconnue().reculer();
      eqn.inconnue().reculer();

      // intermediaire = intermediaire + dt_eq_2 * dIdt
      dIdt*=dt_eq_2;
      final+= dIdt;
      final.echange_espace_virtuel();

    }
  dIdt=final;
  dIdt-=present;
  dIdt/=dt_2;
  update_critere_statio(dIdt, eqn);
  // On impose les CLs au temps final
  eqn.zone_Cl_dis()->imposer_cond_lim(eqn.inconnue(),temps_final);

  converged=true;
  return true;

}


void Sch_CN_EX_iteratif::ajuster_facsec(type_convergence cv)
{

  if (facsec_!=last_facsec) // On n'ajuste qu'une fois pour un initTimeStep.
    return;

  switch (cv)
    {
    case DIVERGENCE:
      // le critere de divergence a ete atteint
      facsec_=last_facsec/1.5;
      omega=0.5*(0.5+omega); // fait tendre omega vers 0.5
      break;
    case NON_CONVERGENCE:
      // ni le critere de divergence ni le critere de convergence
      // n'ont ete atteints en niter_max iterations
      facsec_=last_facsec/1.5;
      omega=0.5*(0.5+omega); // (0.5+3*omega)/4; // fait tendre omega vers 0.5, plus lentement
      break;
    case CONVERGENCE_LENTE:
      // le critere de convergence a ete atteint en plus que niter_avg
      // iterations
      facsec_=last_facsec*0.9;
      if(omega>0.1)
        omega*=0.95;
      break;
    case CONVERGENCE_RAPIDE:
      // le critere de convergence a ete atteint en moins que niter_avg
      // iterations
      facsec_=last_facsec*1.1;
      if(omega>0.1)
        omega*=0.75;
      break;
    case CONVERGENCE_OK:
      // le critere de convergence a ete atteint en niter_avg iterations
      facsec_=last_facsec*1.01;
      break;
    default:
      break;
    }
  facsec_=std::min(facsec_,facsec_max);
}
