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
// File:        Champ_som_lu.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_implementation_P1.h>
#include <LecFicDiffuse.h>
#include <Champ_som_lu.h>
#include <Domaine.h>

Implemente_instanciable(Champ_som_lu,"Champ_som_lu",Champ_Don_base);

Sortie& Champ_som_lu::printOn(Sortie& os) const { return not_implemented_champ_<Sortie&>(__func__); }

Entree& Champ_som_lu::readOn(Entree& is)
{
  int nbcomp;
  Nom nom;
  is >> nom;
  const Domaine& dom = ref_cast(Domaine, Interprete::objet(nom));
  is >> nbcomp; // nombre de composantes du champ
  is >> tolerance_; // tolerance en metres pour la recherche des coordonnees
  is >> nom; // nom du fichier a lire
  mon_domaine = dom;
  LecFicDiffuse file(nom);
  fixer_nb_comp(nbcomp);
  Champ_implementation_P1::init_from_file(valeurs_, dom, nbcomp, tolerance_, file);
  return is;
}

Champ_base& Champ_som_lu::affecter(const Champ_base& ch)
{
  Champ_base::affecter_erreur();
  return *this;
}

DoubleVect& Champ_som_lu::valeur_a(const DoubleVect& positions, DoubleTab& tab_valeurs) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect le_poly(1);
  la_zone.chercher_elements(positions, le_poly);
  DoubleTab positions2;
  positions2 = positions;
  return valeur_aux_elems(positions2, le_poly[0], tab_valeurs);
}

DoubleVect& Champ_som_lu::valeur_a(const DoubleVect& positions, DoubleVect& tab_valeurs) const
{
  return not_implemented_champ_<DoubleVect&>(__func__);
}

DoubleVect& Champ_som_lu::valeur_a(const DoubleVect& positions, DoubleVect& val, int ncomp) const
{
  return not_implemented_champ_<DoubleVect&>(__func__);
}

DoubleVect& Champ_som_lu::valeur_a(const DoubleVect& positions, DoubleVect& tab_valeurs, int le_poly, int ncomp) const
{
  return not_implemented_champ_<DoubleVect&>(__func__);
}

DoubleTab& Champ_som_lu::valeur_aux(const DoubleTab& positions, DoubleTab& tab_valeurs) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect les_polys(la_zone.nb_elem());
  la_zone.chercher_elements(positions, les_polys);
  return valeur_aux_elems(positions, les_polys, tab_valeurs);
}

DoubleVect& Champ_som_lu::valeur_aux_compo(const DoubleTab& positions, DoubleVect& tab_valeurs, int ncomp) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect les_polys(la_zone.nb_elem());
  la_zone.chercher_elements(positions, les_polys);
  return valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
}

DoubleTab& Champ_som_lu::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val) const
{
  if (val.nb_dim() == 2)
    {
      assert(val.dimension(0) == les_polys.size_array());
      assert(val.line_size() == nb_compo_);
    }
  else erreur_champ_(__func__);

  const DoubleTab& coord = mon_domaine->coord_sommets();
  const Zone& zone = mon_domaine->zone(0);
  const DoubleTab& ch = valeurs();
  int le_poly;

  for (int rang_poly = 0; rang_poly < les_polys.size_array(); rang_poly++)
    {
      le_poly = les_polys(rang_poly);
      if (le_poly == -1)
        for (int ncomp = 0; ncomp < nb_compo_; ncomp++) val(rang_poly, ncomp) = 0;
      else
        for (int ncomp = 0; ncomp < nb_compo_; ncomp++)
          {
            int som1 = zone.sommet_elem(le_poly, 0);
            int som2 = zone.sommet_elem(le_poly, 1);
            int som3 = zone.sommet_elem(le_poly, 2);
            int som4 = zone.sommet_elem(le_poly, 3);
            int som5 = zone.sommet_elem(le_poly, 4);
            int som6 = zone.sommet_elem(le_poly, 5);
            int som7 = zone.sommet_elem(le_poly, 6);
            int som8 = zone.sommet_elem(le_poly, 7);

            double xmin = coord(som1, 0);
            double xmax = coord(som2, 0);
            double ymin = coord(som1, 1);
            double ymax = coord(som3, 1);
            double zmin = coord(som1, 2);
            double zmax = coord(som5, 2);

            double hx = xmax - xmin;
            double hy = ymax - ymin;
            double hh = zmax - zmin;

            val(rang_poly, ncomp) = ch(som1, ncomp) * (xmax - positions(rang_poly, 0)) * (ymax - positions(rang_poly, 1)) * (zmax - positions(rang_poly, 2));
            val(rang_poly, ncomp) += ch(som2, ncomp) * (positions(rang_poly, 0) - xmin) * (ymax - positions(rang_poly, 1)) * (zmax - positions(rang_poly, 2));
            val(rang_poly, ncomp) += ch(som3, ncomp) * (xmax - positions(rang_poly, 0)) * (positions(rang_poly, 1) - ymin) * (zmax - positions(rang_poly, 2));
            val(rang_poly, ncomp) += ch(som4, ncomp) * (positions(rang_poly, 0) - xmin) * (positions(rang_poly, 1) - ymin) * (zmax - positions(rang_poly, 2));
            val(rang_poly, ncomp) += ch(som5, ncomp) * (xmax - positions(rang_poly, 0)) * (ymax - positions(rang_poly, 1)) * (positions(rang_poly, 2) - zmin);
            val(rang_poly, ncomp) += ch(som6, ncomp) * (positions(rang_poly, 0) - xmin) * (ymax - positions(rang_poly, 1)) * (positions(rang_poly, 2) - zmin);
            val(rang_poly, ncomp) += ch(som7, ncomp) * (xmax - positions(rang_poly, 0)) * (positions(rang_poly, 1) - ymin) * (positions(rang_poly, 2) - zmin);
            val(rang_poly, ncomp) += ch(som8, ncomp) * (positions(rang_poly, 0) - xmin) * (positions(rang_poly, 1) - ymin) * (positions(rang_poly, 2) - zmin);

            val(rang_poly, ncomp) /= (hx * hy * hh);
          }
    }
  return val;
}

DoubleVect& Champ_som_lu::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& val, int ncomp) const
{
  assert(val.size() == les_polys.size_array());
  const DoubleTab& coord = mon_domaine->coord_sommets();
  const Zone& zone = mon_domaine->zone(0);
  const DoubleTab& ch = valeurs();
  int le_poly;

  for (int rang_poly = 0; rang_poly < les_polys.size_array(); rang_poly++)
    {
      le_poly = les_polys(rang_poly);
      if (le_poly == -1) val(rang_poly) = 0;
      else
        {
          int som1 = zone.sommet_elem(le_poly, 0);
          int som2 = zone.sommet_elem(le_poly, 1);
          int som3 = zone.sommet_elem(le_poly, 2);
          int som4 = zone.sommet_elem(le_poly, 3);
          int som5 = zone.sommet_elem(le_poly, 4);
          int som6 = zone.sommet_elem(le_poly, 5);
          int som7 = zone.sommet_elem(le_poly, 6);
          int som8 = zone.sommet_elem(le_poly, 7);

          double xmin = coord(som1, 0);
          double xmax = coord(som2, 0);
          double ymin = coord(som1, 1);
          double ymax = coord(som3, 1);
          double zmin = coord(som1, 2);
          double zmax = coord(som5, 2);

          double hx = xmax - xmin;
          double hy = ymax - ymin;
          double hh = zmax - zmin;

          val(rang_poly) = ch(som1, ncomp) * (xmax - positions(rang_poly, 0)) * (ymax - positions(rang_poly, 1)) * (zmax - positions(rang_poly, 2));
          val(rang_poly) += ch(som2, ncomp) * (positions(rang_poly, 0) - xmin) * (ymax - positions(rang_poly, 1)) * (zmax - positions(rang_poly, 2));
          val(rang_poly) += ch(som3, ncomp) * (xmax - positions(rang_poly, 0)) * (positions(rang_poly, 1) - ymin) * (zmax - positions(rang_poly, 2));
          val(rang_poly) += ch(som4, ncomp) * (positions(rang_poly, 0) - xmin) * (positions(rang_poly, 1) - ymin) * (zmax - positions(rang_poly, 2));
          val(rang_poly) += ch(som5, ncomp) * (xmax - positions(rang_poly, 0)) * (ymax - positions(rang_poly, 1)) * (positions(rang_poly, 2) - zmin);
          val(rang_poly) += ch(som6, ncomp) * (positions(rang_poly, 0) - xmin) * (ymax - positions(rang_poly, 1)) * (positions(rang_poly, 2) - zmin);
          val(rang_poly) += ch(som7, ncomp) * (xmax - positions(rang_poly, 0)) * (positions(rang_poly, 1) - ymin) * (positions(rang_poly, 2) - zmin);
          val(rang_poly) += ch(som8, ncomp) * (positions(rang_poly, 0) - xmin) * (positions(rang_poly, 1) - ymin) * (positions(rang_poly, 2) - zmin);

          val(rang_poly) /= (hx * hy * hh);
        }
    }
  return val;
}
