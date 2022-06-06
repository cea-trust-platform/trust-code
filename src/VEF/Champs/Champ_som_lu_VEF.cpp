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

#include <Champ_som_lu_VEF.h>

#include <EFichier.h>
#include <TRUSTTab.h>
#include <Domaine.h>

Implemente_instanciable(Champ_som_lu_VEF,"Champ_som_lu_VEF",Champ_som_lu);

Sortie& Champ_som_lu_VEF::printOn(Sortie& os) const { return Champ_som_lu::printOn(os); }
Entree& Champ_som_lu_VEF::readOn(Entree& is) { return Champ_som_lu::readOn(is); }

DoubleVect& Champ_som_lu_VEF::valeur_a_compo(const DoubleVect& positions, DoubleVect& tab_valeurs, int ncomp) const
{
  int nb_pos = positions.size();
  assert(nb_pos == 1);
  tab_valeurs.resize(nb_pos);
  assert(ncomp < 3);
  switch(dimension)
    {
    case 1:
      {
        Cerr << "Il y a un pbl !! Il faut etre en 2D ou 3D !!" << finl;
        break;
      }
    case 2:
      {
        break;
      }
    case 3:
      {
        break;
      }
    }
  return tab_valeurs;
}

DoubleVect& Champ_som_lu_VEF::valeur_a_elem_compo(const DoubleVect& positions, DoubleVect& tab_valeurs, int, int ncomp) const
{
  return valeur_a_compo(positions, tab_valeurs, ncomp);
}

double Champ_som_lu_VEF::valeur_a_compo(const DoubleVect&, int ) const
{
  return not_implemented_champ_<double>(__func__);
}

double Champ_som_lu_VEF::valeur_a_elem_compo(const DoubleVect&, int ,int ) const
{
  return not_implemented_champ_<double>(__func__);;
}

DoubleTab& Champ_som_lu_VEF::valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& val) const
{
  int som, le_poly;
  double xs, ys, zs;

  const Zone& la_zone = mon_domaine->zone(0);
  const DoubleTab& coord = la_zone.domaine().coord_sommets();
  const IntTab& sommet_poly = la_zone.les_elems();

  if (val.nb_dim() > 2) erreur_champ_(__func__);

  if (dimension == 2)
    {
      Cerr << "ATTENTION : Cela n a pas encore ete teste en 2D!!!" << finl;
      Cerr << "Il manque les fonctions de forme en 2D!! A vous de jouer!!" << finl;
    }

  const DoubleTab& ch = valeurs();

  for (int rang_poly = 0; rang_poly < les_polys.size(); rang_poly++)
    {
      le_poly = les_polys(rang_poly);
      if (le_poly == -1)
        for (int ncomp = 0; ncomp < nb_compo_; ncomp++) val(rang_poly, ncomp) = 0;
      else
        for (int ncomp = 0; ncomp < nb_compo_; ncomp++)
          {
            val(rang_poly, ncomp) = 0;
            xs = positions(rang_poly, 0);
            ys = positions(rang_poly, 1);
            zs = (dimension == 3) ? positions(rang_poly, 2) : 0.;
            for (int i = 0; i < dimension + 1; i++)
              {
                som = sommet_poly(le_poly, i);
                val(rang_poly, ncomp) += ch(som, ncomp)
                                         * ((dimension == 2) ? coord_barycentrique2D(sommet_poly, coord, xs, ys, le_poly, i) : coord_barycentrique3D(sommet_poly, coord, xs, ys, zs, le_poly, i));
              }
          }
    }
  return val;
}

DoubleVect& Champ_som_lu_VEF::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& val, int ncomp) const
{
  int som, le_poly;
  double xs, ys, zs;
  const Zone& la_zone = mon_domaine->zone(0);
  const DoubleTab& coord = la_zone.domaine().coord_sommets();
  const IntTab& sommet_poly = la_zone.les_elems();
  assert(val.size() == les_polys.size());
  const DoubleTab& ch = valeurs();

  if (dimension == 2)
    {
      Cerr << "ATTENTION : Cela n a pas encore ete teste en 2D!!!" << finl;
      Cerr << "Il manque les fonctions de forme en 2D!! A vous de jouer!!" << finl;
      Process::exit();
    }

  for (int rang_poly = 0; rang_poly < les_polys.size(); rang_poly++)
    {
      le_poly = les_polys(rang_poly);
      if (le_poly == -1) val(rang_poly) = 0;
      else
        {
          val(rang_poly) = 0;
          xs = positions(rang_poly, 0);
          ys = positions(rang_poly, 1);
          zs = (dimension == 3) ? positions(rang_poly, 2) : 0.;
          for (int i = 0; i < dimension + 1; i++)
            {
              som = sommet_poly(le_poly, i);
              val(rang_poly) += ch(som, ncomp) * ((dimension == 2) ? coord_barycentrique2D(sommet_poly, coord, xs, ys, le_poly, i) : coord_barycentrique3D(sommet_poly, coord, xs, ys, zs, le_poly, i));
            }
        }
    }
  return val;
}

double coord_barycentrique3D(const IntTab& polys, const DoubleTab& coord, double x, double y, double z, int le_poly, int i)
{
  int som0, som1, som2, som3;

  switch(i)
    {
    case 0:
      {
        som0 = polys(le_poly, 0);
        som1 = polys(le_poly, 1);
        som2 = polys(le_poly, 2);
        som3 = polys(le_poly, 3);
        break;
      }
    case 1:
      {
        som0 = polys(le_poly, 1);
        som1 = polys(le_poly, 2);
        som2 = polys(le_poly, 3);
        som3 = polys(le_poly, 0);
        break;
      }
    case 2:
      {
        som0 = polys(le_poly, 2);
        som1 = polys(le_poly, 3);
        som2 = polys(le_poly, 0);
        som3 = polys(le_poly, 1);
        break;
      }
    case 3:
      {
        som0 = polys(le_poly, 3);
        som1 = polys(le_poly, 0);
        som2 = polys(le_poly, 1);
        som3 = polys(le_poly, 2);
        break;
      }
    default:
      {
        som0 = som1 = som2 = som3 = -1;
        assert(0);
        Process::exit();
        break;
      }
    }

  double xp = (coord(som2, 1) - coord(som1, 1)) * (coord(som0, 2) - coord(som1, 2)) - (coord(som2, 2) - coord(som1, 2)) * (coord(som0, 1) - coord(som1, 1));
  double yp = (coord(som2, 2) - coord(som1, 2)) * (coord(som0, 0) - coord(som1, 0)) - (coord(som2, 0) - coord(som1, 0)) * (coord(som0, 2) - coord(som1, 2));
  double zp = (coord(som2, 0) - coord(som1, 0)) * (coord(som0, 1) - coord(som1, 1)) - (coord(som2, 1) - coord(som1, 1)) * (coord(som0, 0) - coord(som1, 0));
  double den = xp * (coord(som3, 0) - coord(som1, 0)) + yp * (coord(som3, 1) - coord(som1, 1)) + zp * (coord(som3, 2) - coord(som1, 2));

  xp = (coord(som2, 1) - coord(som1, 1)) * (z - coord(som1, 2)) - (coord(som2, 2) - coord(som1, 2)) * (y - coord(som1, 1));
  yp = (coord(som2, 2) - coord(som1, 2)) * (x - coord(som1, 0)) - (coord(som2, 0) - coord(som1, 0)) * (z - coord(som1, 2));
  zp = (coord(som2, 0) - coord(som1, 0)) * (y - coord(som1, 1)) - (coord(som2, 1) - coord(som1, 1)) * (x - coord(som1, 0));
  double num = xp * (coord(som3, 0) - coord(som1, 0)) + yp * (coord(som3, 1) - coord(som1, 1)) + zp * (coord(som3, 2) - coord(som1, 2));

  assert(den != 0.);
  double coord_bary = num / den;
  assert(sup_ou_egal(coord_bary, 0) && inf_ou_egal(coord_bary, 1));
  return coord_bary;
}

double coord_barycentrique2D(const IntTab& polys, const DoubleTab& coord, double x, double y, int le_poly, int i)
{
  int som0 = polys(le_poly, i), som1 = polys(le_poly, 1);

  if (i == 1) som1 = polys(le_poly, 0);

  double xref = coord(som1, 0) - coord(som0, 0);
  double yref = coord(som1, 1) - coord(som0, 1);

  double xp = x - coord(som0, 0);
  double yp = y - coord(som0, 1);

  double pscal = (xref * xp) + (yref * yp);
  double norm = (xref * xref) + (yref * yref);

  assert(norm != 0.);
  double coord_bary = pscal / norm;
  assert(sup_ou_egal(coord_bary, 0) && inf_ou_egal(coord_bary, 1));
  return coord_bary;
}
