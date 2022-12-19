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

#ifndef Champ_Fonc_included
#define Champ_Fonc_included

#include <Champ_Fonc_base.h>
#include <TRUST_Deriv.h>

/*! @brief classe Champ_Fonc Classe generique de la hierarchie des champs fontions du temps,
 *
 *      un objet Champ_Fonc peut referencer n'importe quel objet derivant de
 *      Champ_Fonc_base.
 *      La plupart des methodes appellent les methodes de l'objet Champ_Fonc
 *      sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Champ_Fonc_base
 */
class Champ_Fonc : public DERIV(Champ_Fonc_base), public Champ_Proto
{
  Declare_instanciable(Champ_Fonc);
public :
  Champ_Fonc(const Champ_Fonc&) = default;
  inline Champ_Fonc& operator=(const Champ_Fonc_base& ) ;
  inline Champ_Fonc& operator=(const Champ_Fonc& ) ;
  inline void fixer_nb_valeurs_nodales(int ) ;
  inline int nb_valeurs_nodales() const ;
  using Champ_Proto::valeurs;
  inline DoubleTab& valeurs() override ;
  inline const DoubleTab& valeurs() const override ;
  inline operator DoubleTab& () = delete ;
  inline operator const DoubleTab& () const  = delete;
  inline double temps() const ;
  inline double changer_temps(const double t) ;
  inline void mettre_a_jour(double un_temps) ;
  inline int initialiser(const double un_temps);
  inline void associer_zone_dis_base(const Zone_dis_base&) ;
  inline const Zone_dis_base& zone_dis_base() const ;
  inline int reprendre(Entree& ) override ;
  inline int sauvegarder(Sortie&) const override;
  inline DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& les_valeurs, int le_poly) const;
  inline virtual double valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const;
  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& les_valeurs) const;
  inline DoubleTab& valeur_aux_sommets(const Zone&, DoubleTab&) const;
  inline DoubleVect& valeur_aux_sommets_compo(const Zone&,
                                              DoubleVect&, int) const;
  inline void nommer(const Nom& nom) override
  {
    valeur().nommer(nom);
  };
};

/*! @brief Appel a l'objet sous-jacent Fixe le nombre de degres de liberte par composante
 *
 * @param (int n) le nombre de degres de liberte par comp
 */
inline void Champ_Fonc::fixer_nb_valeurs_nodales(int n)
{
  valeur().fixer_nb_valeurs_nodales(n);
}

/*! @brief Appel a l'objet sous-jacent Renvoie le nombre de degres de liberte par composante
 *
 * @return (int) le nombre de degres de liberte par composante
 */
inline int Champ_Fonc::nb_valeurs_nodales() const
{
  return valeur().nb_valeurs_nodales();
}

/*! @brief Appel a l'objet sous-jacent Renvoie le tableau des valeurs du champ
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
inline DoubleTab& Champ_Fonc::valeurs()
{
  return valeur().valeurs();
}

/*! @brief Appel a l'objet sous-jacent Renvoie le tableau des valeurs du champ
 *
 *     (version const)
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
inline const DoubleTab& Champ_Fonc::valeurs() const
{
  return valeur().valeurs();
}

/*! @brief Appel a l'objet sous-jacent Renvoie le temps courant du champ
 *
 * @return (double) le temps courant du champ
 */
inline double Champ_Fonc::temps() const
{
  return valeur().temps();
}

/*! @brief Appel a l'objet sous-jacent Change le temps courant du champ, et le renvoie.
 *
 * @param (double& t) le nouveau temps courant du champ
 * @return (double) le nouveau temps courant du champ
 */
inline double Champ_Fonc::changer_temps(const double t)
{
  return valeur().changer_temps(t);
}

/*! @brief Appel a l'objet sous-jacent Associe une zone discretisee au champ.
 *
 * @param (Zone_dis_base& zone_dis) la zone discretisee a associer au champ
 */
inline void Champ_Fonc::associer_zone_dis_base(const Zone_dis_base& zone_dis)
{
  valeur().associer_zone_dis_base(zone_dis);
}

/*! @brief Appel a l'objet sous-jacent Renvoe la zone discretisee associee
 *
 * @return (Zone_dis_base&) la zone discretisee associee
 */
inline const Zone_dis_base& Champ_Fonc::zone_dis_base() const
{
  return valeur().zone_dis_base();
}

/*! @brief Appel a l'objet sous-jacent Effectue une mise a jour en temps
 *
 * @param (double temps) le temps de mise a jour
 */
inline void Champ_Fonc::mettre_a_jour(double un_temps)
{
  valeur().mettre_a_jour(un_temps);
}

/*! @brief Appel a l'objet sous-jacent Initilialise le champ (si il y lieu)
 *
 */
inline int Champ_Fonc::initialiser(const double un_temps)
{
  return valeur().initialiser(un_temps);
}

/*! @brief Operateur d'affectation d'un Champ_Fonc_base dans un  Champ_Fonc.
 *
 * @param (Champ_Fonc_base& ch_fonc_base) la partie droite de l'affectation
 * @return (Champ_Fonc&) le resultat de l'affectation (*this)
 */
inline Champ_Fonc& Champ_Fonc::operator=(const Champ_Fonc_base& ch_fonc_base)
{
  DERIV(Champ_Fonc_base)::operator=(ch_fonc_base);
  return *this;
}

/*! @brief Operateur d'affectation d'un Champ_Fonc_base dans un  Champ_Fonc.
 *
 * @param (Champ_Fonc_base& ch_fonc_base) la partie droite de l'affectation
 * @return (Champ_Fonc&) le resultat de l'affectation (*this)
 */
inline Champ_Fonc& Champ_Fonc::operator=(const Champ_Fonc& ch_fonc)
{
  if (ch_fonc.non_nul()) DERIV(Champ_Fonc_base)::operator=(ch_fonc.valeur());
  return *this;
}

/*! @brief Appel a l'objet sous-jacent Postraite au format lml.
 *
 * @param (Sortie& os) un flot de sortie
 * @param (Motcle& loc_post) la localisation du postraitement, "sommet" ou "constant par element"
 * @return (int) code de retour propage
 */
inline int Champ_Fonc::reprendre(Entree& fich)
{
  return valeur().reprendre(fich);
}

/*! @brief Appel a l'objet sous-jacent Effectue une sauvegarde sur un flot de sortie
 *
 * @param (Sortie& fich) un flot de sortie
 * @return (int) code de retour propage
 */
inline int Champ_Fonc::sauvegarder(Sortie& fich) const
{
  return valeur().sauvegarder(fich);
}

inline DoubleVect& Champ_Fonc::valeur_a_elem(const DoubleVect& position, DoubleVect& les_valeurs, int le_poly) const
{
  return valeur().valeur_a_elem(position, les_valeurs, le_poly);
}
inline double Champ_Fonc::valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const
{
  return valeur().valeur_a_elem_compo(position, le_poly, ncomp);
}
inline DoubleTab& Champ_Fonc::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& les_valeurs) const
{
  return valeur().valeur_aux_elems(positions, les_polys, les_valeurs);
}
inline DoubleTab& Champ_Fonc::valeur_aux_sommets(const Zone& dom, DoubleTab& les_valeurs) const
{
  return valeur().valeur_aux_sommets(dom, les_valeurs);
}
inline DoubleVect& Champ_Fonc::valeur_aux_sommets_compo(const Zone& dom, DoubleVect& les_valeurs, int compo) const
{
  return valeur().valeur_aux_sommets_compo(dom, les_valeurs, compo);
}

#endif
