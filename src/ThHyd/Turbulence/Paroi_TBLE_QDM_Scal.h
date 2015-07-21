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
// File:        Paroi_TBLE_QDM_Scal.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Paroi_TBLE_QDM_Scal_included
#define Paroi_TBLE_QDM_Scal_included

#include <Vect_Eq_couch_lim.h>
#include <IntTab.h>
#include <Noms.h>
#include <DoubleVects.h>
class Motcle;
class Zone_dis_base;
class Zone_Cl_dis_base;
class Zone_VF;
class Probleme_base;
class Paroi_TBLE_QDM;

//.DESCRIPTION
//
// CLASS: Paroi_TBLE_QDM_Scal
//
//.SECTION  voir aussi
// Turbulence_paroi_scal_base

class Paroi_TBLE_QDM_Scal
{
public:

  virtual ~Paroi_TBLE_QDM_Scal() { };

  /**
   * Lecture des options de TBLE : a appeler dans readOn des classes filles
   */
  Entree& lire_donnees(const Motcle& motlu, Entree& s);


  /**
   * Initialise les tableaux (dimensionnement) : a appeler dans init_lois_paroi() des classes filles
   */
  int init_lois_paroi(const Zone_VF&, const Zone_Cl_dis_base&);


  /**
   * Initialise par defaut les attributs de la classe : a appeler dans readOn() des classes filles
   */
  void init_valeurs_defaut();


  /**
   * Methodes d'arret/reprise a appeler pour les quantites TBLE
   */
  int reprendre(Entree&, const Zone_dis_base&, const Zone_Cl_dis_base&, double tps);
  int sauvegarder(Sortie&, const Zone_dis_base&, const Zone_Cl_dis_base&, double tps) const ;


  /**
   * Remplit les tableaux des quantites statistiques
   * indice_post : indice de tableau du numero de postraitement a calculer (entre 0 et nb_post_pts)
   * indice_maillage : indice correspondant au numero de maille dans le maillage 1D
   * u, v, w, T: les composantes de la vitesse et de T sur la maille 1D "indice_maillage" de la face correspondante a "indice_post"
   * dt : pas de temps courant
   */
  void  calculer_stat(int j, int i, double u, double v, double w, double T, double dt);

  /**
   * Impression des stats et des sondes TBLE dans les fichiers de sortie
   */
  void imprimer_stat(Sortie&, double) const ;
  void imprimer_sondes(Sortie&, double, const VECT(DoubleVect)&) const ;
  void imprimer_sondes(Sortie&, double, const DoubleVect&) const ;

  Eq_couch_lim& get_eq_couche_lim_T(int i)
  {
    return eq_temp[i];
  };
  virtual const Probleme_base& getPbBase() const = 0 ;
  virtual Paroi_TBLE_QDM& getLoiParoiHydraulique() = 0;
  virtual MuLambda_TBLE_base& getMuLambda() = 0;
  virtual Eq_couch_lim& get_eq_couch_lim(int) = 0;

  int est_initialise() const
  {
    return is_init;  // ne devrait pas eervir a terme
  }
protected:

  int statio; //flag pour savoir si l'on desire mener a CV en temps les equations TBLE a chaque pas de temps grossier
  int max_it_statio; // nb maximum de pas de temps pour arriver a CV de TBLE dans le cas
  // ou l'on choisit de mener a CV en temps les equations TBLE a chaque pas de temps grossier
  double eps_statio; // Critere d'arret dans le cas ou l'on choisit de mener a CV en temps les equations TBLE a chaque pas de temps grossier


  // il faudrait mettre des methodes d'acces a ces attributs pour les classes filles
  // ce serait plus clair
  Motcle modele_visco;
  int nb_post_pts;
  DoubleTab sonde_tble;
  Noms nom_pts;
  IntTab num_faces_post, num_global_faces_post;

  int nb_pts, nb_comp, oui_stats, max_it;
  double fac, epsilon;
  double tps_deb_stats, tps_fin_stats;
  int nu_t_dyn;

  VECT(Eq_couch_lim) eq_temp ;


  int reprise_ok;
  DoubleTab valeurs_reprises;
  int restart;

  //Tableau de Stats
  DoubleTab Tmean_sum; //Integrale temporelle de T.
  DoubleTab UTmean_sum; //
  DoubleTab VTmean_sum; //
  DoubleTab WTmean_sum; //
  DoubleTab Tmean_2_sum; //Integrale temporelle de T^2.
  DoubleTab Alpha_mean_sum;//Integrale temporelle de la diffusivite turbulente


  int is_init;

  double Prandtl;
};


#endif
