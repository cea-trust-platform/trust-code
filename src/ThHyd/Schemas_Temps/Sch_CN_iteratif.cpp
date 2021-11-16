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
// File:        Sch_CN_iteratif.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Sch_CN_iteratif.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Param.h>
#include <Navier_Stokes_std.h>
#include <communications.h>
#include <SFichier.h>

Implemente_instanciable(Sch_CN_iteratif,"Sch_CN_iteratif",Schema_Temps_base);


Sortie& Sch_CN_iteratif::printOn(Sortie& s) const
{
  return  Schema_Temps_base::printOn(s);
}


Entree& Sch_CN_iteratif::readOn(Entree& s)
{
  seuil=1.e-3;
  niter_min=2;
  niter_max=6;
  niter_avg=3;
  facsec_max=2;

  Schema_Temps_base::readOn(s) ;

  return s;
}

void Sch_CN_iteratif::ajuster_facsec(type_convergence cv)
{

  if (facsec_!=last_facsec) // On n'ajuste qu'une fois pour un initTimeStep.
    return;

  switch (cv)
    {
    case DIVERGENCE:
      // le critere de divergence a ete atteint
      facsec_=last_facsec*0.8;
      break;
    case NON_CONVERGENCE:
      // ni le critere de divergence ni le critere de convergence
      // n'ont ete atteints en niter_max iterations
      facsec_=last_facsec*0.9;
      break;
    case CONVERGENCE_LENTE:
      // le critere de convergence a ete atteint en plus que niter_avg
      // iterations
      facsec_=last_facsec*0.99;
      break;
    case CONVERGENCE_RAPIDE:
      // le critere de convergence a ete atteint en moins que niter_avg
      // iterations
      facsec_=last_facsec*1.01;
      break;
    case CONVERGENCE_OK:
      // le critere de convergence a ete atteint en niter_avg iterations
      break;
    default:
      break;
    }
  facsec_=std::min(facsec_,facsec_max);
}
void Sch_CN_iteratif::set_param(Param& param)
{
  param.ajouter("seuil",&seuil);
  param.ajouter("niter_min",&niter_min);
  param.ajouter("niter_max",&niter_max);
  param.ajouter("niter_avg",&niter_avg);
  param.ajouter("facsec_max",&facsec_max);
  Schema_Temps_base::set_param(param);
}




////////////////////////////////
//                            //
// Caracteristiques du schema //
//                            //
////////////////////////////////

// Description:
//    Renvoie le nombre de valeurs temporelles a conserver.
//    Ici : n, n+1/2 et n+1, donc 3
int Sch_CN_iteratif::nb_valeurs_temporelles() const
{
  return 3;
}

// Description:
//    Renvoie le nombre de valeurs temporelles futures.
//    Ici : n+1/2 et n+1 donc 2.
int Sch_CN_iteratif::nb_valeurs_futures() const
{
  return 2;
}

// Description:
//    Renvoie le le temps a la i-eme valeur future.
double Sch_CN_iteratif::temps_futur(int i) const
{
  assert(i>0 && i<=2);
  if (i==2)
    return temps_courant()+pas_de_temps();
  else
    return temps_courant()+pas_de_temps()/2;
}

// Description:
//    Renvoie le temps que doivent utiliser les champs a
//    l'appel de valeurs()
//    Ici : t(n+1/2)
double Sch_CN_iteratif::temps_defaut() const
{
  return temps_futur(1);
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////

bool Sch_CN_iteratif::initTimeStep(double dt)
{

  if (nb_pas_dt()==0)
    mettre_a_jour_dt_stab(); // sinon deja appele par validateTimeStep
  //Plus necessaire car desormais dt_ est mis a jour dans Schema_Temps_base::computeTimeStep(bool& stop)
  //facsec_=dt/dt_;
  last_facsec=facsec_;

  iteration=0;

  return Schema_Temps_base::initTimeStep(dt);
}

// Description:
bool Sch_CN_iteratif::iterateTimeStep(bool& converged)
{

  Probleme_base& pb = pb_base();
  const int nb_eqn=pb.nombre_d_equations();

  SFichier fic;
  fic.ouvrir("dt_CN",ios::app);

  if (je_suis_maitre())
    {
      fic << "Sch_CN_iteratif : pb= " <<  pb.le_nom()
          << " present= " << temps_courant()
          << " dt= " << dt_
          << " iteration= " << iteration
          << " last_facsec= " << last_facsec;
    }
  converged=true;
  bool diverged=false;

  for(int i=0; i<nb_eqn && !diverged; i++)
    {
      bool cv;
      diverged = diverged || !iterateTimeStepOnEquation(i,cv);
      converged = converged && cv;
    }

  iteration ++;
  if (iteration<niter_min)
    converged=false; // Continuer a iterer de toutes facons

  // Un peu de bon sens...
  assert(!(converged&&diverged));

  // Si convergence
  if (converged)
    {
      if (iteration<niter_avg) // Convergence trop rapide, augmenter facsec
        ajuster_facsec(CONVERGENCE_RAPIDE);
      else if (iteration>niter_avg) // Convergence trop lente, reduire facsec
        ajuster_facsec(CONVERGENCE_LENTE);
      else
        ajuster_facsec(CONVERGENCE_OK);
      if (je_suis_maitre())
        {
          fic << " facsec= " << facsec_
              << " result= CONVERGENCE" << finl;
        }
      fic.close();
      return true;
    }

  // Si divergence
  else if (diverged)
    {
      ajuster_facsec(DIVERGENCE);
      if (je_suis_maitre())
        {
          fic << " facsec= " << facsec_
              << " result= DIVERGENCE" << finl;
        }
      return false;
    }

  // Si pas converge assez vite
  else if (iteration==niter_max-1)
    {
      ajuster_facsec(NON_CONVERGENCE);
      if (je_suis_maitre())
        {
          fic << " facsec= " << facsec_
              << " result= NON_CONVERGENCE" << finl;
        }
      return false;
    }

  // Sinon, en cours de convergence
  if (je_suis_maitre())
    {
      fic << " facsec= " << facsec_
          << " result= CONTINUE" << finl;
    }
  return true;
}


// Description:
// Calcule une iteration de la resolution sur l'equation i.
// Calcule u(n+1/2,p+1)=u(n)+f(u(n+1/2,p))*dt/2
// et u(n+1,p+1)=u(n)+f(u(n+1/2,p))*dt
// ou f donne du/dt en fonction de u
// Retourne true dans converged si ca ne bouge plus d'une iteration a l'autre, false sinon
// Renvoie true si OK pour continuer a iterer, false sinon (diverge ou trop d'iterations)
bool Sch_CN_iteratif::iterateTimeStepOnEquation(int i,bool& converged)
{

  Probleme_base& pb = pb_base();
  Equation_base& eqn = pb.equation(i);
  if (eqn.equation_non_resolue())
    {
      Cout<< "====================================================" << finl;
      Cout<< eqn.que_suis_je()<<" equation is not solved."<<finl;
      Cout<< "====================================================" << finl;
      // On calcule une fois la derivee pour avoir les flux bord
      if (eqn.schema_temps().nb_pas_dt()==0)
        {
          DoubleTab inconnue_valeurs(eqn.inconnue().valeurs());
          eqn.derivee_en_temps_inco(inconnue_valeurs);
        }
      converged=true;
      return true;
    }
  double temps_intermediaire=temps_futur(1);
  double temps_final=temps_futur(2);
  double dt_intermediaire=temps_intermediaire-temps_courant();
  double dt_final=temps_final-temps_courant();

  DoubleTab& present = eqn.inconnue().valeurs();
  DoubleTab& intermediaire = eqn.inconnue()->valeurs(temps_intermediaire);
  DoubleTab& final = eqn.inconnue()->valeurs(temps_final);

  DoubleTab dudt(present);

  DoubleTab delta(intermediaire);
  delta*=-1;

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
  // final =  present + dt_final * dudt;
  intermediaire = dudt;
  intermediaire*= dt_intermediaire;
  intermediaire+= present;
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(),temps_intermediaire);
  intermediaire.echange_espace_virtuel();
  final = dudt;
  final*= dt_final;
  final+= present;
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(),temps_final);
  final.echange_espace_virtuel();

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


// WEC : on pourrait le coder, mais ca ne semble pas utile.
// Ce serait une boucle de IterateTimeStepOnEquation
int Sch_CN_iteratif::faire_un_pas_de_temps_eqn_base(Equation_base&)
{
  Cerr << "Sch_CN_iteratif::faire_un_pas_de_temps_eqn_base non code!" << finl;
  exit();
  return 0;
}


// Description:
//    Indique si le calcul iteratif a converge.
// Critere de convergence utilise :
//    || u(n+1,p+1) - u(n+1,p) ||  <  seuil * || u(n+1/2,p+1) ||
// C'est equivalent a
//    || u(n+1,p) - u(n+1) || < seuil * || (Id-(dt/2).(df/du))^-1 || * || u(n+1/2,p+1) ||
// ou u(n+1) est la solution exacte en n+1,
//    df/du est le lagrangien de f(u), pris en u(n+1/2)
//    et les normes sont compatibles entre matrices et vecteurs (ici norme infinie).
// Precondition:
// Parametre: DoubleTab& u0
//    Signification: L'inconnue au debut de l'intervalle de temps u(n).
//                   C'est aussi la premiere estimation u(n+1/2,0) de l'inconnue en tn+1/2.
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture seule
// Parametre: DoubleTab& up1
//    Signification: Estimation de l'inconnue en tn+1/2 a l'iteration p+1 : u(n+1/2,p+1)
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture seule
// Parametre: DoubleTab& delta
//    Signification: u(n+1/2,p+1) - u(n+1/2,p)
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture seule
// Retour: bool
//    Signification: true=converge, false=non converge
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
bool Sch_CN_iteratif::convergence(const DoubleTab& u0, const DoubleTab& up1, const DoubleTab& delta, int p) const
{
  double a = mp_max_abs_vect(delta);
  double b = mp_max_abs_vect(up1);
  int resu = (2. * a) < (seuil * b);
  envoyer_broadcast(resu, 0); // pour etre certain que tout le monde fait la meme chose
  return resu;
}

// Description:
//    Indique si le calcul iteratif a diverge.
// Precondition:
// Parametre: DoubleTab& u0
//    Signification: L'inconnue au debut de l'intervalle de temps u(n).
//                   C'est aussi la premiere estimation u(n+1/2,0) de l'inconnue en tn+1/2.
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: DoubleTab& up1
//    Signification: Estimation de l'inconnue en tn+1/2 a l'iteration p+1 : u(n+1/2,p+1)
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: DoubleTab& delta
//    Signification: u(n+1/2,p+1) - u(n+1/2,p)
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: bool
//    Signification: true=diverge, false=non diverge
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
bool Sch_CN_iteratif::divergence(const DoubleTab& u0, const DoubleTab& up1, const DoubleTab& delta, int p) const
{
  // WEC : ameliorable...
  return false;
}
