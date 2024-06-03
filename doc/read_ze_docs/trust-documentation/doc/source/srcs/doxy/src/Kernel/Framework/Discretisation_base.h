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

#ifndef Discretisation_base_included
#define Discretisation_base_included

#include <Champ_base.h> // Pour Nature_du_champ
#include <TRUST_Ref.h>

class Champ_Fonc_Tabule;
class Schema_Temps_base;
class Domaine_dis_base;
class Champ_Fonc_base;
class Domaine_Cl_dis;
class Champ_Inc_base;
class Probleme_base;
class Equation_base;
class Domaine_dis;
class Champ_base;
class Champ_Fonc;
class Champ_Don;
class Champ_Inc;
class Domaine;
class Motcle;

/*! @brief classe Discretisation_base Cette classe represente un schema de discretisation en espace, qui
 *
 *      sera associe a un probleme. Discretisation_base est la classe
 *      abstraite qui est a la base de la hierarchie des discretisations
 *      en espace.
 *
 * @sa Probleme_base, Classe abstraite dont toutes les discretisations en espace doivent, deriver., Methode abstraite:, void domaine_Cl_dis(Domaine_dis& , Domaine_Cl_dis& ) const
 */
class Discretisation_base : public Objet_U
{
  Declare_base(Discretisation_base);

public :

  // MODIF ELI LAUCOIN (7/06/2007) :
  // J'ajoute une methode virtuelle pure qui precise le mode d'assemblage des contributions au residu des operateurs et des sources.
  //
  // Par defaut, dans TRUST, on passe par contribuer_a_avec, puis par ajouter. Dans les modules poreux MTMS (et EF ?), on passe plutot par contribuer_a_avec et
  // contribuer_au_second_membre, car on n'utilise pas a priori de schema de convection (comme Quick) pour lequel la jacobienne n'est pas exacte.
  //
  // La valeur retournee par defaut est VIA_AJOUTER, pour ne pas perturber le comportement normal des classes de Trio-U en dehors du noyau.
  //
  enum type_calcul_du_residu { VIA_CONTRIBUER_AU_SECOND_MEMBRE = 0, VIA_AJOUTER = 1 };
  inline virtual type_calcul_du_residu codage_du_calcul_du_residu(void) const { return VIA_AJOUTER; }

  void associer_domaine(const Domaine& dom);

  virtual void discretiser_variables() const;
  virtual void discretiser_Domaine_Cl_dis(const Domaine_dis&, Domaine_Cl_dis&) const;
  virtual void discretiser(REF(Domaine_dis)&) const;
  virtual void domaine_Cl_dis(Domaine_dis&, Domaine_Cl_dis&) const = 0;

  // Creation de champs scalaires ou vectoriels (essentiellement appel a la methode generale, ne pas surcharger ces methodes, elles ne sont la que par commodite)
  void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, const Nom& nom, const Nom& unite, int nb_comp, int nb_pas_dt, double temps, Champ_Inc& champ,
                         const Nom& sous_type=NOM_VIDE) const;
  void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, const Nom& nom, const Nom& unite, int nb_comp, double temps, Champ_Fonc& champ) const;
  void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, const Nom& nom, const Nom& unite, int nb_comp, double temps, Champ_Don& champ) const;

  // Creation de champs generaux (eventuellement multiscalaires) :
  // * Ces methodes doivent etre surchargees.
  // * Chaque methode comprend le motcle demande_description, qui provoque l'affichage de l'ensembldes des directives comprises (et appelle a l'ancetre avec le meme motcle).
  // * Si champ scalaire ou vectoriel, le premier nom et la premiere unite sont utilises
  virtual void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, int nb_pas_dt, double temps,
                                 Champ_Inc& champ, const Nom& sous_type=NOM_VIDE) const;
  virtual void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, double temps, Champ_Fonc& champ) const;
  virtual void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, double temps, Champ_Don& champ) const;

  void nommer_completer_champ_physique(const Domaine_dis_base& domaine_vdf, const Nom& nom_champ, const Nom& unite, Champ_base& champ, const Probleme_base& pbi) const;
  int verifie_sous_type(Nom& type, const Nom& sous_type, const Motcle& directive) const;

  void volume_maille(const Schema_Temps_base& sch, const Domaine_dis& z, Champ_Fonc& ch) const;
  virtual void residu(const Domaine_dis&, const Champ_Inc&, Champ_Fonc&) const;

  static void creer_champ(Champ_Inc& ch, const Domaine_dis_base& z, const Nom& type, const Nom& nom, const Nom& unite, int nb_comp, int nb_ddl, int nb_pas_dt, double temps,
                          const Nom& directive=NOM_VIDE, const Nom& nom_discretisation=NOM_VIDE);
  static void creer_champ(Champ_Fonc& ch, const Domaine_dis_base& z, const Nom& type, const Nom& nom, const Nom& unite, int nb_comp, int nb_ddl, double temps, const Nom& directive = NOM_VIDE,
                          const Nom& nom_discretisation=NOM_VIDE);
  static void creer_champ(Champ_Don& ch, const Domaine_dis_base& z, const Nom& type, const Nom& nom, const Nom& unite, int nb_comp, int nb_ddl, double temps, const Nom& directive = NOM_VIDE,
                          const Nom& nom_discretisation=NOM_VIDE);

  virtual Nom get_name_of_type_for(const Nom& class_operateur, const Nom& type_operteur,const Equation_base& eqn, const REF(Champ_base)& champ_supp =REF(Champ_base)()) const;

  // usefull methods to detect discretization
  virtual bool is_ef() const { return false; }
  virtual bool is_vdf() const { return false; }
  virtual bool is_vef() const { return false; }
  virtual bool is_polymac() const { return false; }
  virtual bool is_polymac_p0() const { return false; }
  virtual bool is_polymac_p0p1nc() const { return false; }
  virtual bool is_polymac_family() const { return false; }

protected:
  static const Motcle DEMANDE_DESCRIPTION;
  static const Nom NOM_VIDE;
  REF(Domaine) le_domaine_;

private:
  void test_demande_description(const Motcle& , const Nom&) const;
  static void champ_fixer_membres_communs(Champ_base& ch, const Domaine_dis_base& z, const Nom& type, const Nom& nom, const Nom& unite, int nb_comp, int nb_ddl, double temps);

  virtual void modifier_champ_tabule(const Domaine_dis_base& domaine_dis,Champ_Fonc_Tabule& ch_tab,const VECT(REF(Champ_base))& ch_inc) const ;
};

using Discretisation = TRUST_Deriv<Discretisation_base>;

#endif /* Discretisation_base_included */
