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
// File:        Paroi_TBLE_QDM.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Paroi_TBLE_QDM_included
#define Paroi_TBLE_QDM_included

#include <MuLambda_TBLE.h>
#include <Paroi_TBLE_QDM_Scal.h>

class Motcle;
class Zone_dis_base;
class Zone_Cl_dis_base;
class Zone_VF;
class Probleme_base;
class Param;

//.DESCRIPTION
//
// CLASS: Paroi_TBLE_Impl
//
//.SECTION  voir aussi
// Turbulence_paroi_base

class Paroi_TBLE_QDM
{
public:

  Paroi_TBLE_QDM();
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);


  /**
   * Initialise les tableaux (dimensionnement) : a appeler dans init_lois_paroi() des classes filles
   */
  int init_lois_paroi(const Zone_VF&, const Zone_Cl_dis_base&);



  /**
   * Methodes d'arret/reprise a appeler pour les quantites TBLE
   */
  int reprendre(Entree&, const Zone_dis_base&, const Zone_Cl_dis_base&, double tps);
  int sauvegarder(Sortie&, const Zone_dis_base&, const Zone_Cl_dis_base&, double tps) const ;


  /**
   * Remplit les tableaux des quantites statistiques
   * indice_post : indice de tableau du numero de postraitement a calculer (entre 0 et nb_post_pts)
   * indice_maillage : indice correspondant au numero de maille dans le maillage 1D
   * Fx, Fy, Fz : les composantes du second membre sur la maille 1D "indice_maillage" de la face correspondante a "indice_post"
   * u, v, w : les composantes de la vitesse correspondant sur la maille 1D "indice_maillage" de la face correspondante a "indice_post"
   * dt : pas de temps courant
   */
  void  calculer_stat(int indice_post, int indice_maillage, double Fx, double Fy, double Fz, double u, double v, double w, double dt);

  /**
   * Impression des stats dans les fichiers de sortie
   */
  void imprimer_stat(Sortie&, double) const ;

  inline Eq_couch_lim& get_eq_couch_lim(int i)
  {
    return eq_vit[i];
  };

  virtual   ~Paroi_TBLE_QDM() {};
  MuLambda_TBLE_base& getMuLambda()
  {
    return mu_lambda.valeur();
  }
  virtual const Probleme_base& getPbBase() const = 0 ;

protected:


  // il faudrait mettre des methodes d'acces a ces attributs pour les classes filles
  // ce serait plus clair
  int nb_pts, nb_comp, oui_stats, max_it;

  int statio; //flag pour savoir si l'on desire mener a CV en temps les equations TBLE a chaque pas de temps grossier
  int max_it_statio; // nb maximum de pas de temps pour arriver a CV de TBLE dans le cas
  // ou l'on choisit de mener a CV en temps les equations TBLE a chaque pas de temps grossier
  double eps_statio; // Critere d'arret dans le cas ou l'on choisit de mener a CV en temps les equations TBLE a chaque pas de temps grossier

  double fac, epsilon;
  double tps_deb_stats, tps_fin_stats;

  double tps_start_stat_nu_t_dyn;
  double tps_nu_t_dyn;

  Motcle modele_visco;
  VECT(Eq_couch_lim) eq_vit ;


  DoubleTab visco_turb_moy;
  int nu_t_dyn;

  int nb_post_pts;
  DoubleTab sonde_tble;
  Noms nom_pts;
  IntTab num_faces_post, num_global_faces_post;

  int reprise_ok;
  DoubleTab valeurs_reprises;
  int restart;

  //Tableau de Stats
  DoubleTab Fxmean_sum; //Integrale en temps du second membre Fx.
  DoubleTab Fymean_sum; //Integrale en temps du second membre Fy.
  DoubleTab Fzmean_sum; //Integrale en temps du second membre Fz.
  DoubleTab Umean_sum; //Integrale en temps de U.
  DoubleTab Umean_2_sum; //Integrale en temps de U.
  DoubleTab Vmean_sum; //Integrale en temps de V.
  DoubleTab Vmean_2_sum; //Integrale en temps de VV.
  DoubleTab Wmean_sum; //Integrale en temps de W.
  DoubleTab Wmean_2_sum; //Integrale en temps de WW.
  DoubleTab UVmean_sum; //Integrale en temps de UV.
  DoubleTab WVmean_sum; //Integrale en temps de WV.
  DoubleTab WUmean_sum; //Integrale en temps de WU.


  // Mu fonction de T
  int mu_fonction;
  Nom mu_chaine;
  int lambda_fonction;
  Nom lambda_chaine;
  MuLambda_TBLE mu_lambda;

  //Flag pour le terme source de Boussinesq dans les equations TBLE :
  // le terme source de Boussi est automatiquement mis dans TBLE si il est present dans le 3D. Dans ce cas, il est possible de le supprimer malgre tout
  // avec l'option "sans_source_boussinesq" (alors source_boussinesq=0)
  int source_boussinesq;
  double T0;
  double beta_t;
};


#endif
