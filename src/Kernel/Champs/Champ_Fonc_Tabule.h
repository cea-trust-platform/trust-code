/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Champ_Fonc_Tabule_included
#define Champ_Fonc_Tabule_included

#include <Champ_Fonc.h>
#include <Table.h>

class Zone_dis_base;

/*! @brief Classe Champ_Fonc_Tabule Classe derivee de Champ_Fonc_base qui represente les
 *
 *      champs fonctions d'un autre champ par tabulation
 *      L'objet porte un membre de type Champ_Fonc qui stocke
 *      les valeurs du champ tabule.
 *
 * @sa Champ_Fonc_base
 */

class Champ_Fonc_Tabule : public Champ_Fonc_base
{
  Declare_instanciable(Champ_Fonc_Tabule);

public:
  using Champ_Fonc_base::valeurs;

  inline int initialiser(const double un_temps) override;
  inline void mettre_a_jour(double un_temps) override;
  inline void associer_zone_dis_base(const Zone_dis_base&) override;
  inline const DoubleTab& valeurs() const override;
  inline const Zone_dis_base& zone_dis_base() const override;
  inline const Champ_Fonc& le_champ_tabule_discretise() const;
  inline Champ_Fonc& le_champ_tabule_discretise() { return le_champ_tabule_dis; }
  inline const Table& table() const { return la_table; }
  inline Table& table() { return la_table; }
  inline DoubleTab& valeurs() override;
  inline DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& valeurs, int le_poly) const override ;
  inline DoubleVect& valeur_aux_sommets_compo(const Domaine&, DoubleVect&, int) const override;
  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const override;
  inline DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const override;
  inline Noms& noms_champs_parametre() { return noms_champs_parametre_; };
  inline Noms& noms_problemes() { return noms_pbs_; };

  inline double valeur_a_elem_compo(const DoubleVect& position, int le_poly,int ncomp) const override ;
  Champ_base& affecter_(const Champ_base& ) override ;

  // Methodes utiles pour notifier l'utilisateur suite au changement du syntaxe
  static void Warn_old_chp_fonc_syntax(const char * nom_class, const Nom& val1, const Nom& val2, int& dim, Nom& param);
  static void Warn_old_chp_fonc_syntax_V_184(const char * nom_class, const Nom& val, int& dim, int& old_synt);
  static bool Check_if_int(const Nom& val);

protected:
  Noms noms_champs_parametre_, noms_pbs_;
  Nom nom_domaine;
  Table la_table;
  Champ_Fonc le_champ_tabule_dis;
};

/*! @brief Effectue une mise a jour en temps.
 *
 * @param (double temps) le temps de mise a jour
 */
inline void Champ_Fonc_Tabule::mettre_a_jour(double un_temps)
{
  le_champ_tabule_discretise().mettre_a_jour(un_temps);
  Champ_Fonc_base::mettre_a_jour(un_temps);
}

/*! @brief Initialisation du champ.
 *
 */
inline int Champ_Fonc_Tabule::initialiser(const double un_temps)
{
  le_champ_tabule_discretise().initialiser(un_temps);
  return 1;
}

/*! @brief Renvoie le tableau des valeurs du champ.
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
inline DoubleTab& Champ_Fonc_Tabule::valeurs()
{
  return le_champ_tabule_discretise().valeurs();
}

/*! @brief Renvoie le tableau des valeurs du champ.
 *
 * (version const)
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
inline const DoubleTab& Champ_Fonc_Tabule::valeurs() const
{
  return le_champ_tabule_discretise().valeurs();
}

/*! @brief Associe une Zone discretisee au champ.
 *
 * @param (Zone_dis_base& zone_dis) la zone discretisee a associe au champ
 */
inline void Champ_Fonc_Tabule::associer_zone_dis_base(const Zone_dis_base& zone_dis)
{
  le_champ_tabule_discretise().associer_zone_dis_base(zone_dis);
}

/*! @brief Renvoie la Zone discretisee associee.
 *
 * @return (Zone_dis_base&) la Zone discretisee associee
 */
inline const Zone_dis_base& Champ_Fonc_Tabule::zone_dis_base() const
{
  return le_champ_tabule_discretise().zone_dis_base();
}

inline DoubleVect& Champ_Fonc_Tabule::valeur_a_elem(const DoubleVect& position,
                                                    DoubleVect& les_valeurs,
                                                    int le_poly) const
/*! @brief Effectue le postraitement au format lml Simple appel a Champ_Fonc::postraiter_lml(Sortie&,const Motcle&) const
 *
 * @param (Sortie& os) un flot de sortie
 * @param (Motcle& loc_post) la localisation du postraitement, "sommet" ou "constant par element"
 * @return (int) code de retour propage
 */
{
  return le_champ_tabule_discretise().valeur_a_elem(position, les_valeurs, le_poly);
}
inline double Champ_Fonc_Tabule::valeur_a_elem_compo(const DoubleVect& position,
                                                     int le_poly,int ncomp) const
{
  return le_champ_tabule_discretise().valeur_a_elem_compo(position, le_poly, ncomp);
}
inline DoubleTab& Champ_Fonc_Tabule::valeur_aux_elems(const DoubleTab& positions,
                                                      const IntVect& les_polys,
                                                      DoubleTab& les_valeurs) const
{
  return le_champ_tabule_discretise().valeur_aux_elems(positions, les_polys, les_valeurs);
}
inline DoubleTab& Champ_Fonc_Tabule::valeur_aux_sommets(const Domaine& dom, DoubleTab& les_valeurs) const
{
  return le_champ_tabule_discretise().valeur_aux_sommets(dom, les_valeurs);
}
inline DoubleVect& Champ_Fonc_Tabule::valeur_aux_sommets_compo(const Domaine& dom,
                                                               DoubleVect& les_valeurs, int compo) const
{
  return le_champ_tabule_discretise().valeur_aux_sommets_compo(dom, les_valeurs, compo);
}

/*! @brief Renvoie le champ tabule calcule.
 *
 * (version const)
 *
 * @return (Champ_Fonc&) le champ tabule calcule
 */
inline const Champ_Fonc& Champ_Fonc_Tabule::le_champ_tabule_discretise() const
{
  if(!le_champ_tabule_dis.non_nul())
    {
      Cerr << "The attribute le_champ_tabule_dis of Champ_fonc_tabule " << le_nom() << " is not filled." << finl;
      exit();
    }
  return le_champ_tabule_dis;
}

#endif /* Champ_Fonc_Tabule_included */
