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

#include <Multi_Sch_ThHyd.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Param.h>

Implemente_instanciable(Multi_Sch_ThHyd,"Multi_Schema_ThHyd",Schema_Temps_base);


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
Sortie& Multi_Sch_ThHyd::printOn(Sortie& s) const
{
  return  Schema_Temps_base::printOn(s);
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
Entree& Multi_Sch_ThHyd::readOn(Entree& s)
{
  nb_ss_pas_dt=0;
  return Schema_Temps_base::readOn(s) ;
}


////////////////////////////////
//                            //
// Caracteristiques du schema //
//                            //
////////////////////////////////


// Description:
//    Renvoie le nombre de valeurs temporelles a conserver.
//    Ici : le max des deux schemas utilises.
int Multi_Sch_ThHyd::nb_valeurs_temporelles() const
{
  return std::max(sch_ns.valeur().nb_valeurs_temporelles(),sch_scalaires.valeur().nb_valeurs_temporelles());
}


// Description:
//    Renvoie le nombre de valeurs temporelles futures.
//    Ici : la valeur commune aux deux schemas utilises.
int Multi_Sch_ThHyd::nb_valeurs_futures() const
{
  int n=sch_ns.valeur().nb_valeurs_futures();
  assert (n==sch_scalaires.valeur().nb_valeurs_futures());
  return n;
}

// Description:
//    Renvoie le le temps a la i-eme valeur future.
//    Ici : la valeur commune aux deux schemas utilises.
double Multi_Sch_ThHyd::temps_futur(int i) const
{
  double t=sch_ns.valeur().temps_futur(i);
  assert(t==sch_scalaires.valeur().temps_futur(i));
  return t;
}

// Description:
//    Renvoie le le temps le temps que doivent rendre les champs a
//    l'appel de valeurs()
//    Ici : la valeur commune aux deux schemas utilises.
double Multi_Sch_ThHyd::temps_defaut() const
{
  double t=sch_ns.valeur().temps_defaut();
  assert(t==sch_scalaires.valeur().temps_defaut());
  return t;
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////


void Multi_Sch_ThHyd::completer()
{
  // OC : je comprends pas ces lignes :
  /*  double dts=dt_;
      dt_=0;
      mettre_a_jour();
      nb_pas_dt_--;
      dt_=dts;
  */

  // OC : je verrai plutot une methode  "completer" de ce genre :

  Schema_Temps_base& le_sch_ns = sch_ns.valeur();
  le_sch_ns.set_temps_init()=temps_init();
  le_sch_ns.set_temps_max()=temps_max();
  le_sch_ns.set_temps_courant()=temps_courant();
  le_sch_ns.set_nb_pas_dt()=nb_pas_dt();
  le_sch_ns.set_nb_pas_dt_max()=nb_pas_dt_max();
  le_sch_ns.set_dt_min()=pas_temps_min();
  le_sch_ns.set_dt_max()=pas_temps_max();
  le_sch_ns.set_dt_sauv()=temps_sauv();
  le_sch_ns.set_dt_impr()=temps_impr();
  le_sch_ns.set_facsec()=facteur_securite_pas();
  le_sch_ns.set_seuil_statio()=seuil_statio();
  le_sch_ns.set_stationnaire_atteint()=isStationary();
  le_sch_ns.set_diffusion_implicite()=diffusion_implicite();
  le_sch_ns.set_seuil_diffusion_implicite()=seuil_diffusion_implicite();
  le_sch_ns.set_niter_max_diffusion_implicite()=niter_max_diffusion_implicite();
  le_sch_ns.set_dt()=pas_de_temps();
  le_sch_ns.set_mode_dt_start()=mode_dt_start();
  le_sch_ns.set_indice_tps_final_atteint()=indice_tps_final_atteint();
  le_sch_ns.set_indice_nb_pas_dt_max_atteint()=indice_nb_pas_dt_max_atteint();
  le_sch_ns.set_lu()=lu();

  Schema_Temps_base& le_sch_scalaires = sch_scalaires.valeur();
  le_sch_scalaires.set_temps_init()=temps_init();
  le_sch_scalaires.set_temps_max()=temps_max();
  le_sch_scalaires.set_temps_courant()=temps_courant();
  le_sch_scalaires.set_nb_pas_dt()=nb_pas_dt();
  le_sch_scalaires.set_nb_pas_dt_max()=nb_pas_dt_max();
  le_sch_scalaires.set_dt_min()=pas_temps_min();
  le_sch_scalaires.set_dt_max()=pas_temps_max();
  le_sch_scalaires.set_dt_sauv()=temps_sauv();
  le_sch_scalaires.set_dt_impr()=temps_impr();
  le_sch_scalaires.set_facsec()=facteur_securite_pas();
  le_sch_scalaires.set_seuil_statio()=seuil_statio();
  le_sch_scalaires.set_stationnaire_atteint()=isStationary();
  le_sch_scalaires.set_diffusion_implicite()=diffusion_implicite();
  le_sch_scalaires.set_seuil_diffusion_implicite()=seuil_diffusion_implicite();
  le_sch_scalaires.set_niter_max_diffusion_implicite()=niter_max_diffusion_implicite();
  le_sch_scalaires.set_dt()=pas_de_temps();
  le_sch_scalaires.set_mode_dt_start()=mode_dt_start();
  le_sch_scalaires.set_indice_tps_final_atteint()=indice_tps_final_atteint();
  le_sch_scalaires.set_indice_nb_pas_dt_max_atteint()=indice_nb_pas_dt_max_atteint();
  le_sch_scalaires.set_lu()=lu();
}

int Multi_Sch_ThHyd::mettre_a_jour()
{
  Schema_Temps_base::mettre_a_jour();
  sch_ns.mettre_a_jour();
  sch_scalaires.mettre_a_jour();
  return 1;
}
void Multi_Sch_ThHyd::set_param(Param& param)
{
  param.ajouter("nb_ss_pas_dt",&nb_ss_pas_dt);
  param.ajouter("Schema_Temps_NS",&sch_ns);
  param.ajouter("Schema_Temps_scalaires",&sch_scalaires);
  Schema_Temps_base::set_param(param);
}


// Description:
//    Effectue un pas de temps d'Euler explicite
//    sur l'equation passee en parametre.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation que l'on veut faire avancer d'un
//                   pas de temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Multi_Sch_ThHyd::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{
  //  double dtok=dt_;
  if(sub_type(Navier_Stokes_std, eqn))
    {
      sch_ns->set_dt()=pas_de_temps();
      sch_ns.faire_un_pas_de_temps_eqn_base(eqn);
      facteur_securite_pas()=sch_ns.facteur_securite_pas();
    }
  else
    {
      //sch_scalaires.preparer_pas_temps();
      sch_scalaires->set_dt()=pas_de_temps();
      sch_scalaires.faire_un_pas_de_temps_eqn_base(eqn);
    }
  set_stationnaire_atteint()=sch_ns.valeur().isStationary() && sch_scalaires.valeur().isStationary() ;
  return 1;
}

bool Multi_Sch_ThHyd::iterateTimeStep(bool& converged)
{
  Probleme_base& prob=pb_base();
  double temps=temps_courant_+dt_;
  int nb_eqn=pb_base().nombre_d_equations();
  for(int i=nb_eqn-1; i>-1; i--)
    {
      Equation_base& eqn_i=prob.equation(i);
      if (eqn_i.equation_non_resolue())
        {
          Cout<< "====================================================" << finl;
          Cout<< eqn_i.que_suis_je()<<" equation is not solved."<<finl;
          Cout<< "====================================================" << finl;
          // On calcule une fois la derivee pour avoir les flux bord
          if (eqn_i.schema_temps().nb_pas_dt()==0)
            {
              DoubleTab inconnue_valeurs(eqn_i.inconnue().valeurs());
              eqn_i.derivee_en_temps_inco(inconnue_valeurs);
            }
        }
      else
        {
          eqn_i.zone_Cl_dis().mettre_a_jour(temps);
          faire_un_pas_de_temps_eqn_base(eqn_i);
        }
    }
  converged=true;
  return true;

}

// Description:
//    Corrige le pas de temps dt_min <= dt <= dt_max
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: retourne corriger_pas_temps de la classe mere
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
bool Multi_Sch_ThHyd::corriger_dt_calcule(double& dt) const
{
  bool ok=sch_ns.valeur().corriger_dt_calcule(dt);
  ok = ok && sch_scalaires.valeur().corriger_dt_calcule(dt);
  ok = ok && Schema_Temps_base::corriger_dt_calcule(dt);
  return ok;
}


// Description:
//    Appel a l'objet sous-jacent
//    Change le temps courant
// Precondition:
// Parametre: double& t
//    Signification: la nouvelle valeur du temps courant
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Multi_Sch_ThHyd::changer_temps_courant(const double t)
{
  sch_ns.valeur().changer_temps_courant(t);
  sch_scalaires.valeur().changer_temps_courant(t);

  Schema_Temps_base::changer_temps_courant(t);
}

// Description:
//    Appel a l'objet sous-jacent
//    Renvoie 1 si il y lieu de stopper le calcul pour differente raisons:
//        - le temps final est atteint
//        - le nombre de pas de temps maximum est depasse
//        - l'etat stationnaire est atteint
//        - indicateur d'arret fichier
//    Renvoie 0 sinon
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:entier
//    Signification: 1 si il y a lieu de s'arreter 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Multi_Sch_ThHyd::stop() const
{
  int ls2 = sch_ns.valeur().stop();
  int ls3 = sch_scalaires.valeur().stop();

  return (ls2 | ls3 | Schema_Temps_base::stop());
}



// Description:
//    Appel a l'objet sous-jacent
//    Imprime le schema en temp sur un flot de sortie (si il y a lieu).
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Multi_Sch_ThHyd::imprimer(Sortie& os) const
{
  sch_ns.valeur().imprimer(os);
  sch_scalaires.valeur().imprimer(os);

  Schema_Temps_base::imprimer(os);
}


