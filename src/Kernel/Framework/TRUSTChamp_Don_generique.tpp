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

#ifndef TRUSTChamp_Don_generique_TPP_included
#define TRUSTChamp_Don_generique_TPP_included

template <Champ_Don_Type _TYPE_>
Sortie& TRUSTChamp_Don_generique<_TYPE_>::printOn(Sortie& os) const
{
  const DoubleTab& tab = valeurs();
  os << tab.size() << " ";
  for (int i = 0; i < tab.size(); i++) os << tab(0, i);
  return os;
}

template <Champ_Don_Type _TYPE_>
void TRUSTChamp_Don_generique<_TYPE_>::mettre_a_jour(double t)
{
  static constexpr bool IS_TXYZ = (_TYPE_ == Champ_Don_Type::TXYZ);

  if (!IS_TXYZ) /* XYZ ou LU */
    Champ_Don_base::mettre_a_jour(t);
  else
    {
      changer_temps(t);
      int nb_elems = mon_domaine->nb_elem();
      DoubleTab& mes_val = valeurs();
      DoubleTab positions(nb_elems, dimension);
      mettre_a_jour_positions(positions);
      eval_fct(positions, temps(), mes_val);
      mes_val.echange_espace_virtuel();
    }
}

template <Champ_Don_Type _TYPE_>
double TRUSTChamp_Don_generique<_TYPE_>::valeur_a_compo(const DoubleVect& x, int ncomp) const
{
  static constexpr bool IS_XYZ = (_TYPE_ == Champ_Don_Type::XYZ);
  if (!IS_XYZ)
    return Champ_Don_base::valeur_a_compo(x, ncomp); // appel simple si TXYZ ou LU !
  else
    {
      if (ncomp > nb_compo_) erreur_champ_(__func__); // asking ncomp > nb_compo_ ?????

      DoubleTab positions(1, dimension), val_fct(1);
      for (int d = 0; d < dimension; d++) positions(0, d) = x(d);
      eval_fct(positions, val_fct, ncomp);
      return val_fct(0);
    }
}

template <Champ_Don_Type _TYPE_>
void TRUSTChamp_Don_generique<_TYPE_>::mettre_a_jour_positions(DoubleTab& positions)
{
  positions = 0.0;
  const int nb_elems = mon_domaine->nb_elem(), D = dimension;
  const IntTab& les_Polys = mon_domaine->les_elems();
  for (int num_elem = 0; num_elem < nb_elems; num_elem++)
    {
      int nb_som = 0;
      for (int s = 0, num_som; s < les_Polys.dimension(1); s++)
        if ((num_som = les_Polys(num_elem, s)) >= 0)
          {
            for (int d = 0; d < D; d++)
              positions(num_elem, d) += mon_domaine->coord(num_som, d);
            nb_som++;
          }
      for (int d = 0; d < D; d++)
        positions(num_elem, d) /= nb_som;
    }
}

/*! @brief Renvoie la valeur du champ au point specifie par ses coordonnees.
 *
 * @param (DoubleVect& x) les coordonnees du point de calcul
 * @param (DoubleVect& val) la valeur du champ au point specifie
 */
template <Champ_Don_Type _TYPE_> template<Champ_Don_Type T>
std::enable_if_t<T != Champ_Don_Type::LU, DoubleVect&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_a_(const DoubleVect& x, DoubleVect& val) const
{
  static constexpr bool IS_XYZ = (_TYPE_ == Champ_Don_Type::XYZ);
  if (val.size() != nb_compo_) erreur_champ_(__func__); // DoubleVect val doesn't have the correct size

  IS_XYZ ? eval_fct(x,val) : eval_fct(x,temps(),val);
  return val;
}

template <Champ_Don_Type _TYPE_> template<Champ_Don_Type T>
std::enable_if_t<T == Champ_Don_Type::LU, DoubleVect&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_a_(const DoubleVect& x, DoubleVect& val) const
{
  IntVect le_poly(1);
  mon_domaine->chercher_elements(x,le_poly);
  return valeur_a_elem(x,val,le_poly(0));
}

/*! @brief Renvoie la valeur du champ au point specifie par ses coordonnees, en indiquant que ce point est situe dans un element specifie.
 *
 * @param (DoubleVect&) les coordonnees du point de calcul
 * @param (DoubleVect& val) la valeur du champ au point specifie
 * @param (int le_poly) l'element dans lequel est situe le point de calcul
 */
template <Champ_Don_Type _TYPE_> template<Champ_Don_Type T>
std::enable_if_t<T != Champ_Don_Type::LU, DoubleVect&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_a_elem_(const DoubleVect& x, DoubleVect& val, int ) const
{
  return valeur_a(x,val);
}

template <Champ_Don_Type _TYPE_> template<Champ_Don_Type T>
std::enable_if_t<T == Champ_Don_Type::LU, DoubleVect&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_a_elem_(const DoubleVect& , DoubleVect& val, int le_poly) const
{
  if (val.size() != nb_compo_) erreur_champ_(__func__); // DoubleVect val doesn't have the correct size

  const DoubleTab& ch = valeurs();
  for (int k = 0; k < nb_compo_; k++) val(k) = ch(le_poly, k);

  return val;
}

/*! @brief Renvoie la valeur d'une composante du champ au point specifie par ses coordonnees, en indiquant que ce point est situe dans un element specifie.
 *
 * @param (DoubleVect&) les coordonnees du point de calcul
 * @param (int le_poly) l'element dans lequel est situe le point de calcul
 * @param (int ncomp) l'index de la composante du champ a calculer
 */
template <Champ_Don_Type _TYPE_> template <Champ_Don_Type T>
std::enable_if_t<T != Champ_Don_Type::TXYZ, double> /* XYZ ou LU */
TRUSTChamp_Don_generique<_TYPE_>::valeur_a_elem_compo_(const DoubleVect& x, int le_poly, int ncomp) const
{
  static constexpr bool IS_XYZ = (_TYPE_ == Champ_Don_Type::XYZ);
  if (ncomp > nb_compo_) erreur_champ_(__func__); // asking ncomp > nb_compo_ ?????

  return IS_XYZ ? valeur_a_compo(x,ncomp) : valeurs()(le_poly,ncomp) /* LU */;
}

template <Champ_Don_Type _TYPE_> template <Champ_Don_Type T>
std::enable_if_t<T == Champ_Don_Type::TXYZ, double>
TRUSTChamp_Don_generique<_TYPE_>::valeur_a_elem_compo_(const DoubleVect& x, int , int ncomp) const
{
  DoubleTab positions(1, dimension);
  for (int d = 0; d < dimension; d++) positions(0, d) = x(d);

  DoubleVect val_fct(1);
  eval_fct(positions, temps(), val_fct, ncomp);

  return val_fct(0);
}

/*! @brief Renvoie les valeurs du champ aux points specifies par leurs coordonnees.
 *
 * @param (DoubleTab& x) le tableau des coordonnees des points de calcul
 * @param (DoubleTab& val) le tableau des valeurs du champ aux points specifies
 */
template <Champ_Don_Type _TYPE_> template <Champ_Don_Type T>
std::enable_if_t<T != Champ_Don_Type::LU, DoubleTab&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_(const DoubleTab& x, DoubleTab& val) const
{
  static constexpr bool IS_XYZ = (_TYPE_ == Champ_Don_Type::XYZ);

  if (val.nb_dim() == 1) assert(nb_compo_ == 1);
  else if (val.nb_dim() == 2) assert(val.dimension(1) == nb_compo_);
  else erreur_champ_(__func__); // DoubleTab val has more than 2 entries !

  IS_XYZ ? eval_fct(x,val) : eval_fct(x,temps(),val);
  return val;
}

template <Champ_Don_Type _TYPE_> template <Champ_Don_Type T>
std::enable_if_t<T == Champ_Don_Type::LU, DoubleTab&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_(const DoubleTab& x, DoubleTab& val) const
{
  IntVect les_polys(mon_domaine->nb_elem());
  mon_domaine->chercher_elements(x,les_polys);
  return valeur_aux_elems(x,les_polys,val); // VTABLE pour Champ_som_lu
}

/*! @brief Renvoie les valeurs d'une composante du champ aux points specifies par leurs coordonnees.
 *
 * @param (DoubleVect& x) le tableau des coordonnees des points de calcul
 * @param (DoubleVect& valeurs) le tableau des valeurs de la composante du champ aux points specifies
 * @param (int ncomp) l'index de la composante du champ a calculer
 */
template <Champ_Don_Type _TYPE_> template<Champ_Don_Type T>
std::enable_if_t<T != Champ_Don_Type::LU, DoubleVect&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_compo_(const DoubleTab& x, DoubleVect& val, int ncomp) const
{
  static constexpr bool IS_XYZ = (_TYPE_ == Champ_Don_Type::XYZ);
  IS_XYZ ? eval_fct(x,val,ncomp) : eval_fct(x,temps(),val,ncomp);
  return val;
}

template <Champ_Don_Type _TYPE_> template<Champ_Don_Type T>
std::enable_if_t<T == Champ_Don_Type::LU, DoubleVect&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_compo_(const DoubleTab& x, DoubleVect& val, int ncomp) const
{
  IntVect les_polys(mon_domaine->nb_elem());
  mon_domaine->chercher_elements(x,les_polys);
  return valeur_aux_elems_compo(x,les_polys,val,ncomp); // VTABLE pour Champ_som_lu
}

/*! @brief Renvoie les valeurs du champ aux points specifies par leurs coordonnees, en indiquant que les points de calculs sont situes dans les elements indiques.
 *
 * @param (DoubleTab&) le tableau des coordonnees des points de calcul
 * @param (IntVect& les_polys) le tableau des elements dans lesquels sont situes les points de calcul (inutile)
 * @param (DoubleTab& val) le tableau des valeurs du champ aux points specifies
 */
template <Champ_Don_Type _TYPE_> template<Champ_Don_Type T>
std::enable_if_t<T != Champ_Don_Type::LU, DoubleTab&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_elems_(const DoubleTab& x, const IntVect&, DoubleTab& val) const
{
  return valeur_aux(x, val);
}

template <Champ_Don_Type _TYPE_> template<Champ_Don_Type T>
std::enable_if_t<T == Champ_Don_Type::LU, DoubleTab&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_elems_(const DoubleTab&, const IntVect& les_polys, DoubleTab& val) const
{
  if (val.nb_dim() > 2) erreur_champ_(__func__); // DoubleTab val don't have 2 entries

  int p;
  const DoubleTab& ch = valeurs();
  val = 0.;

  for (int rang_poly = 0; rang_poly < les_polys.size(); rang_poly++)
    if ((p = les_polys(rang_poly)) != -1)
      for (int n = 0; n < nb_compo_; n++) val(rang_poly, n) = ch(p, n);

  return val;
}

/*! @brief Renvoie les valeurs d'une composante du champ aux points specifies par leurs coordonnees, en indiquant que les points de calculs sont situes dans les elements indiques.
 *
 * @param (DoubleTab& x) le tableau des coordonnees des points de calcul
 * @param (IntVect& les_polys) le tableau des elements dans lesquels sont situes les points de calcul
 * @param (DoubleVect& val) le tableau des valeurs de la composante du champ aux points specifies
 * @param (int ncomp) l'index de la composante du champ a calculer
 */
template <Champ_Don_Type _TYPE_> template<Champ_Don_Type T>
std::enable_if_t<T != Champ_Don_Type::LU, DoubleVect&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_elems_compo_(const DoubleTab& x, const IntVect&, DoubleVect& val, int ncomp) const
{
  return valeur_aux_compo(x, val, ncomp);
}

template <Champ_Don_Type _TYPE_> template<Champ_Don_Type T>
std::enable_if_t<T == Champ_Don_Type::LU, DoubleVect&>
TRUSTChamp_Don_generique<_TYPE_>::valeur_aux_elems_compo_(const DoubleTab& , const IntVect& les_polys, DoubleVect& val, int ncomp) const
{
  assert(val.size() == les_polys.size());
  const DoubleTab& ch = valeurs();

  for (int rang_poly = 0; rang_poly < les_polys.size(); rang_poly++)
    {
      int le_poly = les_polys(rang_poly);
      if (le_poly == -1) val(rang_poly) = 0;
      else val(rang_poly) = ch(le_poly, ncomp);
    }
  return val;
}

#endif /* TRUSTChamp_Don_generique_TPP_included */
