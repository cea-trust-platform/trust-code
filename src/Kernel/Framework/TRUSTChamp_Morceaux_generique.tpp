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
//////////////////////////////////////////////////////////////////////////////
//
// File:        TRUSTChamp_Morceaux_generique.tpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTChamp_Morceaux_generique_TPP_included
#define TRUSTChamp_Morceaux_generique_TPP_included

template<Champ_Morceaux_Type _TYPE_>
Champ_base& TRUSTChamp_Morceaux_generique<_TYPE_>::affecter_(const Champ_base& ch)
{
  static constexpr bool IS_FONC = (_TYPE_ == Champ_Morceaux_Type::FONC);

  if (!IS_FONC) not_implemented_champ_<void>(__func__);

  if (sub_type(Champ_Uniforme, ch)) valeurs() = ch.valeurs()(0, 0);
  else valeurs() = ch.valeurs();

  return *this;
}

// Description:
//    Renvoie la valeur du champ au point specifie par ses coordonnees.
// Parametre: DoubleVect& positions
//    Signification: les coordonnees du point de calcul
// Parametre: DoubleVect& valeurs
//    Signification: la valeur du champ au point specifie
template<Champ_Morceaux_Type _TYPE_>
DoubleVect& TRUSTChamp_Morceaux_generique<_TYPE_>::valeur_a(const DoubleVect& positions, DoubleVect& tab_valeurs) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect le_poly(1);
  la_zone.chercher_elements(positions, le_poly);
  return valeur_a_elem(positions, tab_valeurs, le_poly[0]);
}

// Description:
//    Renvoie la valeur du champ au point specifie par ses coordonnees, en indiquant que ce point est
//    situe dans un element specifie.
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
// Parametre: DoubleVect& val
//    Signification: la valeur du champ au point specifie
// Parametre: int le_poly
//    Signification: l'element dans lequel est situe le point de calcul
template<Champ_Morceaux_Type _TYPE_>
DoubleVect& TRUSTChamp_Morceaux_generique<_TYPE_>::valeur_a_elem(const DoubleVect&, DoubleVect& val, int le_poly) const
{
  if (val.size() != nb_compo_) erreur_champ_(__func__);

  const DoubleTab& ch = valeurs();
  for (int k = 0; k < nb_compo_; k++) val(k) = ch(le_poly, k);

  return val;
}

// Description:
//    Renvoie la valeur d'une composante du champ au point specifie
//    par ses coordonnees, en indiquant que ce point est situe dans un element specifie.
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
// Parametre: int le_poly
//    Signification: l'element dans lequel est situe le point de calcul
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
template<Champ_Morceaux_Type _TYPE_>
double TRUSTChamp_Morceaux_generique<_TYPE_>::valeur_a_elem_compo(const DoubleVect&, int le_poly, int ncomp) const
{
  if (ncomp > nb_compo_) erreur_champ_(__func__);

  const DoubleTab& ch = valeurs();
  double val = (nb_compo_ == 1) ? ch(le_poly, 0) : ch(le_poly, ncomp);

  return val;
}

// Description:
//    Renvoie les valeurs du champ aux points specifies par leurs coordonnees.
// Parametre: DoubleTab& positions
//    Signification: le tableau des coordonnees des points de calcul
// Parametre: DoubleTab& valeurs
//    Signification: le tableau des valeurs du champ aux points specifies
template<Champ_Morceaux_Type _TYPE_>
DoubleTab& TRUSTChamp_Morceaux_generique<_TYPE_>::valeur_aux(const DoubleTab& positions, DoubleTab& tab_valeurs) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect les_polys(la_zone.nb_elem());
  la_zone.chercher_elements(positions, les_polys);
  return valeur_aux_elems(positions, les_polys, tab_valeurs);
}

// Description:
//    Renvoie les valeurs d'une composante du champ aux points specifies par leurs coordonnees.
// Parametre: DoubleTab& positions
//    Signification: le tableau des coordonnees des points de calcul
// Parametre: DoubleVect& valeurs
//    Signification: le tableau des valeurs de la composante du champ aux points specifies
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
template<Champ_Morceaux_Type _TYPE_>
DoubleVect& TRUSTChamp_Morceaux_generique<_TYPE_>::valeur_aux_compo(const DoubleTab& positions, DoubleVect& tab_valeurs, int ncomp) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect les_polys(la_zone.nb_elem());
  la_zone.chercher_elements(positions, les_polys);
  return valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
}

// Description:
//    Renvoie les valeurs du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de calculs sont situes dans les elements indiques.
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
// Parametre: IntVect& les_polys
//    Signification: le tableau des elements dans lesquels sont situes les points de calcul
// Parametre: DoubleTab& val
//    Signification: le tableau des valeurs du champ aux points specifies
template<Champ_Morceaux_Type _TYPE_>
DoubleTab& TRUSTChamp_Morceaux_generique<_TYPE_>::valeur_aux_elems(const DoubleTab&, const IntVect& les_polys, DoubleTab& val) const
{
  if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size()) || (val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo_);
    }
  else erreur_champ_(__func__); // (val.nb_dim() > 2)

  const DoubleTab& ch = valeurs();
  val = 0.;
  int p;

  for (int rang_poly = 0; rang_poly < les_polys.size(); rang_poly++)
    if ((p = les_polys(rang_poly)) != -1)
      for (int n = 0; n < nb_compo_; n++) val(rang_poly, n) = ch(p, n);

  return val;
}

// Description:
//    Renvoie les valeurs d'une composante du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de calculs sont situes dans les elements indiques.
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
// Parametre: IntVect& les_polys
//    Signification: le tableau des elements dans lesquels sont situes les points de calcul
// Parametre: DoubleVect& val
//    Signification: le tableau des valeurs de la composante du champ aux points specifies
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
template<Champ_Morceaux_Type _TYPE_>
DoubleVect& TRUSTChamp_Morceaux_generique<_TYPE_>::valeur_aux_elems_compo(const DoubleTab&, const IntVect& les_polys, DoubleVect& val, int ncomp) const
{
  assert(val.size() == les_polys.size());
  int le_poly;
  const DoubleTab& ch = valeurs();

  for (int rang_poly = 0; rang_poly < les_polys.size(); rang_poly++)
    {
      le_poly = les_polys(rang_poly);
      val(rang_poly) = (le_poly == -1) ? 0 : ch(le_poly, ncomp);
    }

  return val;
}

template<Champ_Morceaux_Type _TYPE_>
void TRUSTChamp_Morceaux_generique<_TYPE_>::mettre_a_jour(double time)
{
  static constexpr bool IS_UNIFORME = (_TYPE_ == Champ_Morceaux_Type::UNIFORME);

  if (IS_UNIFORME)
    {
      Champ_Don_base::mettre_a_jour(time);
      return;
    }

  const IntTab& les_elems = mon_domaine->zone(0).les_elems();
  const int nb_som_elem = mon_domaine->zone(0).nb_som_elem();

  const Champ_base *ch = ref_pb.non_nul() ? &ref_pb->get_champ(nom_champ_parametre_) : nullptr;

  DoubleTab& tab = valeurs();

  for (int i = 0; i < mon_domaine->zone(0).nb_elem_tot(); i++)
    {
      /* xs : coordonnees du poly par barycentre des sommets -> pas top */
      double xs[3] = { 0, };
      int nb_som = 0, s, r;
      for (int j = 0; j < nb_som_elem && (s = les_elems(i, j)) >= 0; j++)
        for (r = 0, nb_som++; r < dimension; r++) xs[r] += mon_domaine->coord(s, r);

      for (r = 0; r < dimension; r++) xs[r] /= nb_som;

      /* calcul de chaque composante */
      double val = ch ? ch->valeurs()(i, 0) : 0;
      for (int k = 0; k < tab.dimension(1); k++)
        {
          Parser_U& psr = parser(parser_idx(i, k));
          psr.setVar("x", xs[0]);
          psr.setVar("y", xs[1]);
          psr.setVar("z", xs[2]);
          psr.setVar("t", time);

          if (ref_pb.non_nul()) psr.setVar("val", val);
          tab(i, k) = psr.eval();
        }
    }
}

template<Champ_Morceaux_Type _TYPE_>
int TRUSTChamp_Morceaux_generique<_TYPE_>::initialiser(const double time)
{
  return Champ_Don_base::initialiser(time);
}

template<Champ_Morceaux_Type _TYPE_>
void TRUSTChamp_Morceaux_generique<_TYPE_>::interprete_get_domaine(const Nom& nom)
{
  static constexpr bool IS_FONC_TXYZ = (_TYPE_ == Champ_Morceaux_Type::FONC_TXYZ);
  if (IS_FONC_TXYZ)
    {
      ref_pb = ref_cast(Probleme_base, Interprete::objet(nom));
      mon_domaine = ref_pb->domaine();
    }
  else mon_domaine = ref_cast(Domaine, Interprete::objet(nom));
}

template<Champ_Morceaux_Type _TYPE_>
void TRUSTChamp_Morceaux_generique<_TYPE_>::creer_tabs(const int dim)
{
  fixer_nb_comp(dim);
  parser_idx.resize(0, dim);
  mon_domaine->creer_tableau_elements(parser_idx);
  valeurs_.resize(0, dim);
  mon_domaine->creer_tableau_elements(valeurs_);
}

template<Champ_Morceaux_Type _TYPE_>
Entree& TRUSTChamp_Morceaux_generique<_TYPE_>::complete_readOn(const int dim, const Nom& qsj, Entree& is, Nom& nom)
{
  Motcle motlu;
  int k, poly;

  if (nom != "{")
    {
      Cerr << "Error while reading a " << qsj << " . We expected a { instead of " << nom << finl;
      Process::exit();
    }
  is >> motlu;
  if (motlu != Motcle("defaut"))
    {
      Cerr << "Error while reading a " << qsj << " . We expected defaut instead of " << nom << finl;
      Process::exit();
    }

  /* parsers par defaut */
  for (k = 0; k < dim; k++)
    {
      Parser_U psr;
      Nom tmp;
      is >> tmp;
      psr.setNbVar(5);
      psr.setString(tmp);
      psr.addVar("t"), psr.addVar("x"), psr.addVar("y"), psr.addVar("z");
      if (ref_pb.non_nul()) psr.addVar("val");
      psr.parseString();

      for (poly = 0; poly < mon_domaine->zone(0).nb_elem_tot(); poly++)
        parser_idx(poly, k) = parser.size();

      parser.add(psr);
    }

  is >> nom;
  while (nom != Nom("}"))
    {
      REF (Sous_Zone) refssz = les_sous_zones.add(mon_domaine->ss_zone(nom));
      Sous_Zone& ssz = refssz.valeur();
      for (k = 0; k < dim; k++)
        {
          Parser_U psr;
          Nom tmp;
          is >> tmp;
          psr.setNbVar(5);
          psr.setString(tmp);
          psr.addVar("t"), psr.addVar("x"), psr.addVar("y"), psr.addVar("z");
          if (ref_pb.non_nul()) psr.addVar("val");
          psr.parseString();

          for (poly = 0; poly < ssz.nb_elem_tot(); poly++)
            parser_idx(ssz(poly), k) = parser.size();

          parser.add(psr);
        }
      is >> nom;
    }
  return is;
}

#endif /* TRUSTChamp_Morceaux_generique_TPP_included */
