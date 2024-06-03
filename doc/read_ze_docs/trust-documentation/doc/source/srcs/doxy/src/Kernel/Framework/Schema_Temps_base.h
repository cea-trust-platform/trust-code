/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Schema_Temps_base_included
#define Schema_Temps_base_included

#include <Interface_blocs.h>
#include <TRUST_Ref.h>
#include <TRUSTTab.h>
#include <Parser_U.h>
#include <SFichier.h>
#include <math.h>

class Probleme_base;
class Equation_base;
class Matrice_Base;
class SFichier;
class Equation;
class Motcle;
class Param;

/*! @brief classe Schema_Temps_base
 *
 */

//     Cette classe represente un schema en temps, c'est-a-dire un
//     algorithme de resolution particulier qui sera associe a un
//     Probleme_base (un probleme simple et non un couplage).
//     Schema_Temps_base est la classe abstraite qui est a la base de
//     la hierarchie des schemas en temps.
//
//     On note n le temps present, et n+1 le temps a l'issue du pas de temps.
//     Un schema en temps permet de calculer u(n+1) connaissant u jusqu'a u(n).
//     Il utilise u(n), mais peut aussi avoir besoin  de valeurs passees de u,
//     telles que u(n-1), ...
//     Il peut aussi en cours de calcul utiliser des valeurs de u a des temps
//     intermediaires entre n et n+1, par exemple n+1/2.
//     nb_valeurs_temporelles compte toutes les valeurs allouees :
//     n, n+1, les valeurs passees retenues etles valeurs intermediaires
//     entre n et n+1.
//     nb_valeurs_futures compte n+1 et les valeurs intermediaires entre n et n+1.
//     C'est donc le nombre de crans dont tournent les roues en avancant d'un pas de temps.
//     temps_futur(i) renvoie la i-eme valeur future du temps.
//     Enfin temps_defaut est le temps que doivent rendre les champs a
//     l'appel de valeurs() - donc notamment dans les operateurs.
//     Pour le moment n'est respecte que par les Champ_Front des CLs.
//
// .SECTION voir aussi
//     Equation Equation_base Probleme_base Algo_MG_base
//     Classe abstraite dont tous les schemas en temps doivent deriver.
//     Methodes abstraites:
//       int faire_un_pas_de_temps_eqn_base(Equation_base&)
//////////////////////////////////////////////////////////////////////////////
class Schema_Temps_base : public Objet_U
{
  Declare_base_sans_constructeur(Schema_Temps_base);

public :
  virtual void initialize();
  virtual void terminate() ;
  virtual double computeTimeStep(bool& stop) const;
  virtual bool initTimeStep(double dt);
  virtual void validateTimeStep();
  virtual bool isStationary() const;
  virtual void abortTimeStep();

  virtual bool iterateTimeStep(bool& converged);
  int limpr() const;

  ////////////////////////////////
  //                            //
  // Caracteristiques du schema //
  //                            //
  ////////////////////////////////

  virtual int nb_valeurs_temporelles() const =0;
  virtual int nb_valeurs_futures() const =0;
  virtual double temps_futur(int i) const =0;
  virtual double temps_defaut() const =0;

  /////////////////////////////////////////
  //                                     //
  // Fin des caracteristiques du schema  //
  //                                     //
  /////////////////////////////////////////

  Schema_Temps_base();
  inline void nommer(const Nom&) override;
  inline const Nom& le_nom() const override;
  virtual int faire_un_pas_de_temps_eqn_base(Equation_base&) =0;

  virtual void set_param(Param& titi);
  int sauvegarder(Sortie& ) const override;
  int reprendre(Entree& ) override ;
  virtual int mettre_a_jour();
  // pour empecher ancienne methode
  virtual void mettre_a_jour_dt(double toto)
  {
    abort();
  };
  virtual void mettre_a_jour_dt_stab();

  inline double pas_de_temps() const;
  inline const DoubleTab& pas_de_temps_locaux() const;

  virtual bool corriger_dt_calcule(double& dt) const;
  virtual void imprimer(Sortie& os) const;
  virtual int impr(Sortie& os) const;
  void imprimer(Sortie& os,Probleme_base& pb) const;
  void imprimer(Sortie& os,const Probleme_base& pb) const;
  virtual int impr(Sortie& os,Probleme_base& pb) const;
  virtual int impr(Sortie& os,const Probleme_base& pb) const;
  void imprimer_temps_courant(SFichier&) const;
  inline double pas_temps_min() const;
  inline double& pas_temps_min();
  inline double pas_temps_max() const;
  inline double& pas_temps_max();
  inline int nb_impr() const;
  inline double temps_courant() const;
  inline double temps_precedent() const;
  inline double temps_calcul() const;
  virtual inline void changer_temps_courant(const double );
  void update_critere_statio(const DoubleTab& tab_critere, Equation_base& equation);
  inline double facteur_securite_pas() const;
  inline double& facteur_securite_pas();
  virtual int stop() const;
  int lsauv() const;
  inline int temps_final_atteint() const;
  inline int nb_pas_dt_max_atteint() const;
  inline int temps_cpu_max_atteint() const;
  inline int stationnaire_atteint() const
  {
    assert(stationnaire_atteint_!=-1);
    return stationnaire_atteint_;
  };
  int stop_lu() const;
  inline int diffusion_implicite() const;
  inline double seuil_diffusion_implicite() const
  {
    return seuil_diff_impl_;
  };        // Seuil pour implicitation de la diffusion par GC
  inline int impr_diffusion_implicite() const
  {
    return impr_diff_impl_;
  };        // Impression ou non de GC implicitation
  inline int impr_extremums() const
  {
    return impr_extremums_;
  };
  inline int niter_max_diffusion_implicite() const
  {
    return niter_max_diff_impl_;
  };// Iterations maximale pour GC implicitation

  inline int no_conv_subiteration_diffusion_implicite() const
  {
    return no_conv_subiteration_diff_impl_;
  };
  inline int no_error_if_not_converged_diffusion_implicite() const
  {
    return no_error_if_not_converged_diff_impl_;
  };
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  virtual Entree& lire_nb_pas_dt_max(Entree&);
  virtual Entree& lire_periode_sauvegarde_securite_en_heures(Entree&);
  virtual Entree& lire_temps_cpu_max(Entree&);
  Entree& lire_residuals(Entree&);

  virtual void completer() =0;

  inline double temps_init() const ;
  inline double temps_max() const ;
  inline double temps_sauv() const ;
  inline double temps_impr() const ;
  inline int precision_impr() const
  {
    return precision_impr_;
  };
  inline int wcol() const
  {
    // largeur minimale des colonnes des fichiers .out
    // precision_impr_ + 9 car : -1.000e+150 on ajoute la longueur de "-1." et de "e+150" plus un espace
    return precision_impr_ + 9;
  };
  inline int gnuplot_header() const
  {
    return gnuplot_header_;
  };
  inline double seuil_statio() const ;
  inline int nb_pas_dt_max() const ;
  inline int nb_pas_dt() const ;
  inline double mode_dt_start() const { return mode_dt_start_; }
  inline int indice_tps_final_atteint() const { return ind_tps_final_atteint; }
  inline int indice_nb_pas_dt_max_atteint() const { return ind_nb_pas_dt_max_atteint; }
  inline int lu() const { return lu_; }
  inline int file_allocation() const { return file_allocation_; }

  inline double& set_temps_init() { return tinit_; }
  inline double& set_temps_max() { return tmax_; }
  inline double& set_temps_courant() { return temps_courant_; }
  inline double& set_temps_precedent() { return temps_precedent_; }
  inline int& set_nb_pas_dt() { return nb_pas_dt_; }
  inline int& set_nb_pas_dt_max() { return nb_pas_dt_max_; }
  inline double& set_dt_min() { return dt_min_; }
  inline double& set_dt_max()
  {
    dt_max_str_ = Nom(); //desactive la fonction dt_max = f(t)
    return dt_max_;
  } ;
  inline double& set_dt_sauv() { return dt_sauv_; }
  inline double& set_dt_impr() { return dt_impr_; }
  inline int& set_precision_impr() { return precision_impr_; }
  inline double& set_dt() { return dt_; }
  inline double& set_facsec() { return facsec_; }
  inline double& set_seuil_statio() { return seuil_statio_; }
  inline int& set_stationnaire_atteint()
  {
    if (stationnaire_atteint_==-1)
      stationnaire_atteint_=1;
    return stationnaire_atteint_;
  } ;
  inline void set_stationnaires_atteints(bool flag) { stationnaires_atteints_=flag; }
  inline int& set_diffusion_implicite() { return ind_diff_impl_; }
  inline double& set_seuil_diffusion_implicite() { return seuil_diff_impl_; }
  inline int& set_niter_max_diffusion_implicite() { return niter_max_diff_impl_; }
  inline double& set_mode_dt_start() { return mode_dt_start_; }
  inline int& set_indice_tps_final_atteint() { return ind_tps_final_atteint; }
  inline int& set_indice_nb_pas_dt_max_atteint() { return ind_nb_pas_dt_max_atteint; }
  inline int& set_lu() { return lu_; }
  inline const double& residu() const { return residu_ ; }
  inline double& residu() { return residu_ ; }
  inline const Nom& norm_residu() const { return norm_residu_ ; }

  inline int& schema_impr() { return schema_impr_ ; }
  inline const int& schema_impr() const { return schema_impr_ ; }

  virtual void associer_pb(const Probleme_base&);
  Probleme_base& pb_base();
  const Probleme_base& pb_base() const;

  //pour les schemas en temps a pas multiples
  inline virtual void modifier_second_membre(const Equation_base& eqn, DoubleTab& secmem) { };

  // pour implicite ajoute l'inertie a la matrice et au scd membre
  virtual void ajouter_inertie(Matrice_Base& mat_morse,DoubleTab& secmem,const Equation_base& eqn) const;

  //interface ajouter_blocs
  virtual void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const Equation_base& eqn, const tabs_t& semi_impl = {}) const;

  // Flag to disable the writing of the .progress file
  inline int disable_progress() const
  {
    return disable_progress_ ;
  };
  // Flag to disable the writing of the .dt_ev file
  inline int disable_dt_ev() const
  {
    return disable_dt_ev_ ;
  };
  void finir() const;
protected :
  REF(Probleme_base) mon_probleme;
  Nom nom_;
  double dt_;                                // Pas de temps de calcul
  DoubleTab dt_locaux_;                     // Local time steps: Vector of size nb faces of the mesh

  double temps_courant_ = -100.;
  double temps_precedent_ = -100.;
  double dt_failed_ = -100.;  //si on a rate un pas de temps, sa valeur
  double dt_gf_;
  double tinit_;
  double tmax_;
  double tcpumax_;
  int nb_pas_dt_;
  int nb_pas_dt_max_;
  mutable int nb_impr_;
  double dt_min_;                        // Pas de temps min fixe par l'utilisateur
  mutable double dt_max_;                // Pas de temps max fixe par l'utilisateur
  Nom dt_max_str_;                       //reglage de dt_max comme une fonction du temps
  mutable Parser_U dt_max_fn_;           //Parser_U associe
  double dt_stab_=-100.;                // Pas de temps de stabilite
  double facsec_;
  double seuil_statio_;
  int seuil_statio_relatif_deconseille_;                // Drapeau pour specifier si seuil_statio_ est une valeur absolue (defaut) ou relative
  Nom norm_residu_;
  double dt_sauv_;
  double limite_cpu_sans_sauvegarde_;
  double periode_cpu_sans_sauvegarde_;
  double temps_cpu_ecoule_;
  double dt_impr_;                        // Pas de temps d'impression
  int precision_impr_;                // Nombre de chiffres significatifs impression
  double mode_dt_start_;                // mode calcul du pas de temps de depart
  double residu_;        // Residu
  double residu_old_slope_;
  double cumul_slope_;
  int gnuplot_header_;

  int ind_tps_final_atteint;
  int ind_nb_pas_dt_max_atteint;
  int ind_temps_cpu_max_atteint;
  int lu_;
  int ind_diff_impl_ ;
  double seuil_diff_impl_;
  int impr_diff_impl_;
  int impr_extremums_;
  int niter_max_diff_impl_;
  int no_conv_subiteration_diff_impl_;
  int no_error_if_not_converged_diff_impl_;
  int schema_impr_;                  // 1 si le schema a le droit d'imprimer dans le .out et dt_ev
  int file_allocation_;                // 1 = allocation espace disque (par defaut), 0 sinon
  int max_length_cl_ = -10;
private:
  int stationnaire_atteint_;	// Stationary reached by the problem using this scheme
  bool stationnaires_atteints_;	// Stationary reached by the calculation (means all the problems reach stationary)
  SFichier progress_;
  int disable_progress_; // Flag to disable the writing of the .progress file
  int disable_dt_ev_; // Flag to disable the writing of the .dt_ev file
};


/*! @brief surcharge Objet_U::nommer(const Nom&) Donne un nom au shema en temps
 *
 * @param (Nom& name) le nom a donner au shema en temps
 */
inline void Schema_Temps_base::nommer(const Nom& name)
{
  nom_=name;
}


/*! @brief surcharge Objet_U::le_nom() Renvoie le nom du shema en temps
 *
 * @return (Nom&) le nom du shema en temps
 */
inline const Nom& Schema_Temps_base::le_nom() const
{
  return nom_;
}


/*! @brief Renvoie une reference sur le nombre de pas maxi
 *
 * @return (double&)
 */
inline int Schema_Temps_base::nb_pas_dt_max() const
{
  return nb_pas_dt_max_;
}


/*! @brief Renvoie une reference sur le seuil stationnaire
 *
 * @return (double&)
 */
inline double Schema_Temps_base::seuil_statio() const
{
  return seuil_statio_;
}


/*! @brief Renvoie une reference sur le temps d'impression
 *
 * @return (double&)
 */
inline double Schema_Temps_base::temps_impr() const
{
  return dt_impr_;
}


/*! @brief Renvoie une reference sur le temps de sauvegarde
 *
 * @return (double&)
 */
inline double Schema_Temps_base::temps_sauv() const
{
  return dt_sauv_;
}


/*! @brief Renvoie une reference sur le temps maximum
 *
 * @return (double&)
 */
inline double Schema_Temps_base::temps_max() const
{
  return tmax_;
}


/*! @brief Renvoie le pas de temps (delta_t) courant.
 *
 * @return (double) le pas de temps courant
 */
inline double Schema_Temps_base::pas_de_temps() const
{
  return dt_;
}
inline const DoubleTab& Schema_Temps_base::pas_de_temps_locaux() const
{
  return dt_locaux_;
}
/*! @brief Renvoie le pas de temps minimum.
 *
 * (version const)
 *
 * @return (double) le pas de temps minimum du schema en temps
 */
inline double Schema_Temps_base::pas_temps_min() const
{
  return dt_min_;
}

/*! @brief Renvoie une reference sur le pas de temps minimum.
 *
 * @return (double&) le pas de temps minimum du schema en temps
 */
inline double& Schema_Temps_base::pas_temps_min()
{
  return dt_min_;
}

/*! @brief Renvoie le pas de temps maximum.
 *
 * (version const)
 *
 * @return (double) le pas de temps maximum du schema en temps
 */
inline double Schema_Temps_base::pas_temps_max() const
{
  return dt_max_;
}

/*! @brief Renvoie une reference sur le pas de temps maximum.
 *
 * @return (double&) le pas de temps maximum du schema en temps
 */
inline double& Schema_Temps_base::pas_temps_max()
{
  return dt_max_;
}

/*! @brief Renvoie le temps courant.
 *
 * @return (double) le temps courant du schema en temps
 */
inline double Schema_Temps_base::temps_courant() const
{
  return temps_courant_;
}


/*! @brief Renvoie le temps courant.
 *
 * @return (double) le temps courant du schema en temps
 */
inline double Schema_Temps_base::temps_precedent() const
{
  return temps_precedent_;
}

/*! @brief Renvoie le temps de calcul ecoule i.
 *
 * e. (temps courant - temps initial).
 *
 * @return (double) le temps de calcul ecoule
 */
inline double Schema_Temps_base::temps_calcul() const
{
  return temps_courant_ - tinit_;
}

/*! @brief Renvoie le temps initial.
 *
 * @return (double) le temps de calcul ecoule
 */
inline double Schema_Temps_base::temps_init() const
{
  return tinit_;
}


/*! @brief Renvoie le nombre de pas de temps effectues.
 *
 * @return (int) le nombre de pas de temps effectues
 */
inline int Schema_Temps_base::nb_pas_dt() const
{
  return nb_pas_dt_;
}

/*! @brief Renvoie le nombre d'impressions effectuees.
 *
 * @return (int) le nombre d'impressions effectuees
 */
inline int Schema_Temps_base::nb_impr() const
{
  return nb_impr_;
}



/*! @brief Change le temps courant.
 *
 * @param (double& t) le nouveau temps courant
 */
inline void Schema_Temps_base::changer_temps_courant(const double t)
{
  temps_courant_ = t;
}

/*! @brief Renvoie le facteur de securite ou multiplicateur de delta_t.
 *
 * Ce facteur est utilise lors de la correction/verification du
 *     pas de temps. Voir Schema_Temps_base::corriger_dt_calcule(double&)
 *     (version const)
 *
 * @return (double) le facteur de securite du schema en temps
 */
inline double Schema_Temps_base::facteur_securite_pas() const
{
  return facsec_;
}

/*! @brief Renvoie une reference sur le facteur de securite ou multiplicateur de delta_t.
 *
 * Ce facteur est utilise lors de la correction/verification du
 *     pas de temps. Voir Schema_Temps_base::corriger_dt_calcule(double&)
 *
 * @return (double&) le facteur de securite du schema en temps
 */
inline double& Schema_Temps_base::facteur_securite_pas()
{
  return facsec_;
}

/*! @brief Renvoie 1 si le temps final est atteint (ou depasse).
 *
 * Renvoie 1 si temps_courant_ >= tmax
 *     Renvoie 0 sinon
 *
 * @return (int) 1 si le temps final est atteint 0 sinon
 * @throws temps final atteint
 */
inline int Schema_Temps_base::temps_final_atteint() const
{
  return ind_tps_final_atteint;
}

/*! @brief Renvoie 1 si (le nombre de pas de temps >= nombre de pas de temps maximum).
 *
 * Renvoie 0 sinon
 *
 * @return (int) 1 si le nombre de pas de temps maximum est depasse 0 sinon
 * @throws nombre de pas de temps maximum atteint
 */
inline int Schema_Temps_base::nb_pas_dt_max_atteint() const
{
  return ind_nb_pas_dt_max_atteint;
}

inline int Schema_Temps_base::temps_cpu_max_atteint() const
{
  return ind_temps_cpu_max_atteint;
}

/*! @brief Renvoie 1 si le schema en temps a ete lu diffusion_implicite.
 *
 * @return (int) 1 si le schema en temps a ete lu 0 sinon.
 */
inline int Schema_Temps_base::diffusion_implicite() const
{
  return ind_diff_impl_;
}

#endif

