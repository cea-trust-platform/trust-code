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

#ifndef Champ_Inc_base_included
#define Champ_Inc_base_included

#include <Interface_blocs.h>
#include <Champ_base.h>
#include <TRUSTTrav.h>
#include <TRUST_Ref.h>
#include <Domaine_VF.h>
#include <MorEqn.h>
#include <Roue.h>

class Frontiere_dis_base;
class Domaine_dis_base;
class Domaine_Cl_dis;
class MD_Vector;
class Domaine_dis;
class Domaine;

/*! @brief Classe Champ_Inc_base
 *
 *      Classe de base des champs inconnues qui sont les champs calcules par une equation.
 *      Un objet de type Roue est associe au Champ_Inc, cette roue permet de gerer le nombre de valeurs du temps pour lesquels le champ
 *      doit rester en memoire. C'est le schema en temps qui guide le nombre de valeurs a garder.
 *      Cette classe peut aussi servir a stocker des champs calcules a partir d'autres Champ_Inc. Dans ce cas, une fonction calculer_valeurs(...)
 *      est appellee lors de mettre_a_jour() et doit remplir :
 *       - les valeurs du champ a l'instant courant;
 *       - les derivees de ces valeurs par rapport aux inconnues;
 *       - ses valeurs aux bords (stockees dans un tableau, car le champ n'a pas
 *         de CL associee);
 *
 *      Champ_Inc est un morceaux d'equation car il herite de MorEqn.
 *
 * @sa MorEqn Champ_Inc Champ_base Ch_proto Equation_base, Classe abstraite, Methodes abstraites:, const Domaine_dis_base& associer_domaine_dis_base(const Domaine_dis_base&), const Domaine_dis_base& domaine_dis_base() const, DoubleTab& remplir_coord_noeuds(DoubleTab& ) const
 */

class Champ_Inc_base : public Champ_base, public MorEqn
{
  Declare_base_sans_constructeur(Champ_Inc_base);

public:
  Champ_Inc_base() : fonc_calc_(nullptr) { } // par defaut : pas de fonc_calc_

  // Methode reimplementees
  int fixer_nb_valeurs_nodales(int) override;
  double changer_temps(const double temps) override;
  void mettre_a_jour(double temps) override;
  int reprendre(Entree&) override;
  int sauvegarder(Sortie&) const override;
  Champ_base& affecter_compo(const Champ_base&, int compo) override;

  // Methodes viruelles pures implementees ici
  Champ_base& affecter_(const Champ_base&) override;
  virtual void verifie_valeurs_cl();
  DoubleTab& valeurs() override;
  const DoubleTab& valeurs() const override;

  DoubleVect& valeur_a(const DoubleVect& position, DoubleVect& valeurs) const override;
  DoubleTab& valeur_aux(const DoubleTab& positions, DoubleTab& valeurs) const override;
  DoubleVect& valeur_aux_compo(const DoubleTab& positions, DoubleVect& valeurs, int ncomp) const override;

  // Nouvelles methodes
  int nb_valeurs_nodales() const override;
  virtual int fixer_nb_valeurs_temporelles(int);
  virtual int nb_valeurs_temporelles() const;
  double changer_temps_futur(double, int i = 1);
  double changer_temps_passe(double, int i = 1);
  double recuperer_temps_futur(int i = 1) const;
  double recuperer_temps_passe(int i = 1) const;
  DoubleTab& valeurs(double temps) override;
  const DoubleTab& valeurs(double temps) const override;

  // Operateurs de conversion implicite
  operator DoubleTab& () = delete;
  operator const DoubleTab& () const = delete;

  DoubleTab& futur(int i = 1) override;
  const DoubleTab& futur(int i = 1) const override;
  DoubleTab& passe(int i = 1) override;
  const DoubleTab& passe(int i = 1) const override;
  Champ_Inc_base& avancer(int i = 1);
  Champ_Inc_base& reculer(int i = 1);

  int lire_donnees(Entree&);
  int imprime(Sortie&, int) const override;
  int a_un_domaine_dis_base() const override { return 1; }

  virtual void associer_eqn(const Equation_base&);
  virtual void associer_domaine_cl_dis(const Domaine_Cl_dis&);
  void associer_domaine_dis_base(const Domaine_dis_base&) override;

  const Domaine_Cl_dis& domaine_Cl_dis() const;
  Domaine_Cl_dis& domaine_Cl_dis();
  const Domaine_dis_base& domaine_dis_base() const override { return le_dom_VF.valeur(); }
  const Domaine_VF& domaine_vf() const { return le_dom_VF.valeur(); }

  DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double, int distant) const override;
  virtual int remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const;
  virtual int remplir_coord_noeuds_et_polys_compo(DoubleTab&, IntVect&, int) const;
  virtual DoubleTab& remplir_coord_noeuds(DoubleTab&) const =0;
  virtual DoubleTab& remplir_coord_noeuds_compo(DoubleTab&, int) const;

  virtual double integrale_espace(int ncomp) const;
  const Domaine& domaine() const;

  //derivees du champ en les inconnues :
  //renvoie les derivees calcules par fonc_calc_ si champ_calcule, deriv[nom de l'inco] = 1 si vraie inconnue
  const tabs_t& derivees() const { return deriv_; }
  tabs_t& derivees() { return deriv_; }
  DoubleTab& val_bord() { return val_bord_; }

  //champ dependant d'autres Champ_Inc : reglage de la fonciton de calcul, initialisation de val_bord_
  void init_champ_calcule(const Objet_U& obj, fonc_calc_t fonc);
  //pour forcer le calcul de toutes les cases au prochain mettre_a_jour() (normalement fait une seule fois)
  void reset_champ_calcule() { fonc_calc_init_ = 0; }

  void resize_val_bord();
  void set_val_bord_fluide_multiphase(const bool flag) { bord_fluide_multiphase_ = flag; }

  //utilise les conditions aux limites (au lieu de valeur_aux() dans Champ_base)
  //result n'est rempli que pour les faces de bord dont la CL impose une valeur (val_imp ou val_ext)
  DoubleTab valeur_aux_bords() const override;

  // Obsolete method: signature changed in order to generate a compiler error if old code is not removed
  virtual void creer_espace_distant(int dummy) { }

protected:
  // Par defaut on initialise les valeurs a zero
  virtual void creer_tableau_distribue(const MD_Vector&, Array_base::Resize_Options = Array_base::COPY_INIT);

  Roue_ptr les_valeurs;
  REF(Domaine_Cl_dis) mon_dom_cl_dis;
  REF(Domaine_VF) le_dom_VF;

  /* pour les champs dependant d'autres Champ_Inc */
  fonc_calc_t fonc_calc_;  //fonction de calcul
  int fonc_calc_init_ = 0; //1 une fois qu'on a calcule le champ au moins une fois
  RefObjU obj_calc_; //un objet a passer en argument
  DoubleTab val_bord_;   //valeurs aux bords au temps courant
  tabs_t deriv_;        //derivees au temps courant
  bool bord_fluide_multiphase_ = false;
};

#endif /* Champ_Inc_base_included */
