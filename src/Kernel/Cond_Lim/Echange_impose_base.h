/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Echange_impose_base_included
#define Echange_impose_base_included

#include <Cond_lim_base.h>
#include <TRUSTTab.h>

/*! @brief classe Echange_impose_base: Cette condition limite sert uniquement pour l'equation d'energie.
 *
 *     Elle correspond a imposer un echange de chaleur avec l'exterieur
 *     du domaine en imposant une temperature exterieure T_ext et un
 *     coefficient d'echange h_imp.
 *     Le terme de flux calcule a partir du couple(h_imp,T_ext) s'ecrit :
 *                            h_t(T_ext - T_entier)*Surf
 *                           avec h_t : coefficient d'echange global.
 *     Il figure au second membre de l'equation d'energie.
 *     Soit l'utilisateur donne un coefficient d'echange qui correspond
 *     uniquement a la paroi, auquel cas le programme calculera la diffusion
 *     sur la demi-maille pres de la paroi, soit il donne un coefficient
 *     d'echange global qui tient compte directement des deux precedents.
 *     Les deux classes derivees Echange_externe_impose et Echange_global_impose
 *     representent ces deux possibilites.
 *
 * @sa Cond_lim_base Echange_externe_impose Echange_global_impose
 */
class Echange_impose_base : public Cond_lim_base
{
  Declare_base_sans_constructeur(Echange_impose_base);
public:

  inline bool has_emissivite() const { return emissivite_.non_nul(); }
  inline bool has_h_imp() const { return h_imp_.non_nul(); }

  virtual double T_ext(int num) const;
  virtual double T_ext(int num,int k) const;
  virtual double h_imp(int num) const;
  virtual double h_imp(int num,int k) const;
  double emissivite(int num) const;
  double emissivite(int num,int k) const;

  /*! @brief Renvoie le champ T_ext de temperature imposee a la frontiere.
   *
   * @return (Champ_front_base&) le champ T_ext de temperature imposee a la frontiere
   */
  inline virtual Champ_front_base& T_ext() { return le_champ_front.valeur(); }
  inline virtual const Champ_front_base& T_ext() const { return le_champ_front.valeur(); }

  inline virtual Champ_front_base& h_imp() { assert (has_h_imp()); return h_imp_.valeur(); }
  inline virtual const Champ_front_base& h_imp() const { assert (has_h_imp()); return h_imp_.valeur(); }

  inline Champ_front_base& emissivite() {  assert (has_emissivite()); return emissivite_.valeur(); }
  inline const Champ_front_base& emissivite() const  {  assert (has_emissivite()); return emissivite_.valeur(); }

  // Utilise dans les CAL de calcul des flux pour les lois de paroi
  virtual void liste_faces_loi_paroi(IntTab&) { }

  void mettre_a_jour(double ) override;
  int initialiser(double temps) override;
  int a_mettre_a_jour_ss_pas_dt() override { return 1; }

  // ajout de methode pour ne pas operer directement sur champ_front
  void set_temps_defaut(double temps) override;
  void fixer_nb_valeurs_temporelles(int nb_cases) override;
  //
  void changer_temps_futur(double temps,int i) override;
  int avancer(double temps) override;
  int reculer(double temps) override;
  void associer_fr_dis_base(const Frontiere_dis_base& ) override ;

  virtual bool has_h_imp_grad() const { return false; }
  virtual double h_imp_grad(int num) const { Process::exit(que_suis_je()+ " : h_imp_grad must be overloaded !" ) ; return -1.e10 ;};
  virtual double h_imp_grad(int num,int k) const  { Process::exit(que_suis_je()+ " : h_imp_grad must be overloaded !") ; return -1.e10 ;};

protected :
  OWN_PTR(Champ_front_base) h_imp_, emissivite_ /* si Echange_externe_radiatif */;
};

#endif /* Echange_impose_base_included */
