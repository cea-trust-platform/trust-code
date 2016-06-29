/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Champ_som_lu_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_som_lu_VEF.h>
#include <Domaine.h>
#include <Interprete.h>
#include <EFichier.h>

Implemente_instanciable(Champ_som_lu_VEF,"Champ_som_lu_VEF",Champ_som_lu);

Sortie& Champ_som_lu_VEF::printOn(Sortie& os) const
{
  Champ_som_lu::printOn(os);
  return os;
}

Entree& Champ_som_lu_VEF::readOn(Entree& is)
{
  // Tout est dans Champ_som_lu:
  Champ_som_lu::readOn(is);
  return is;
}

// Description:
// Renvoie la valeur en un point.
DoubleVect& Champ_som_lu_VEF::valeur_a(const DoubleVect& positions,
                                       DoubleVect& tab_valeurs) const
{
  {
    Cerr << "On ne doit pas appeler la methode valeur_a(const DoubleVect&, DoubleVect& valeurs)"
         << " pour un champ vectoriel " << finl;
    exit();
  }
  return tab_valeurs;
}

// Description:
//
DoubleVect& Champ_som_lu_VEF::valeur_a_compo(const DoubleVect& positions,
                                             DoubleVect& tab_valeurs,
                                             int ncomp) const
{
  int nb_pos=positions.size();
  assert(nb_pos==1);
  tab_valeurs.resize(nb_pos);
  assert(ncomp<3);
  switch(dimension)
    {
    case 1:
      {
        Cerr << "Il y a un pbl!!" << finl;
        Cerr << "Il faut etre en 2D ou 3D!!" << finl;
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

// Description:
//
DoubleVect& Champ_som_lu_VEF::valeur_a_elem_compo(const DoubleVect& positions,
                                                  DoubleVect& tab_valeurs,
                                                  int ,int ncomp) const
{
  return valeur_a_compo(positions, tab_valeurs, ncomp);
}

// Description:
double Champ_som_lu_VEF::valeur_a_compo(const DoubleVect&, int ) const
{
  Cerr << que_suis_je();
  Cerr << "::valeur_a_compo(...) n'est pas code " << finl ;
  exit();
  return 0;
}

// Description:
double Champ_som_lu_VEF::valeur_a_elem_compo(const DoubleVect&, int ,int ) const
{
  Cerr << que_suis_je();
  Cerr << "::valeur_a_elem_compo(...) n'est pas code " << finl ;
  exit();
  return 0;
}

// Description:
//
DoubleTab& Champ_som_lu_VEF::valeur_aux(const DoubleTab& positions,
                                        DoubleTab& tab_valeurs) const
{

  const Zone& la_zone = mon_domaine->zone(0);
  IntVect les_polys(la_zone.nb_elem());
  la_zone.chercher_elements(positions,les_polys);
  return valeur_aux_elems(positions,les_polys,tab_valeurs);
}

// Description:
//
DoubleVect& Champ_som_lu_VEF::valeur_aux_compo(const DoubleTab& positions,
                                               DoubleVect& tab_valeurs, int ncomp) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect les_polys(la_zone.nb_elem());
  la_zone.chercher_elements(positions,les_polys);
  return valeur_aux_elems_compo(positions,les_polys,tab_valeurs,ncomp);
}

// Description:
//
DoubleTab& Champ_som_lu_VEF::valeur_aux_elems(const DoubleTab& positions,
                                              const IntVect& les_polys,
                                              DoubleTab& val) const
{

  int som;
  double xs,ys,zs;

  const Zone& la_zone = mon_domaine->zone(0);
  //   const Zone_dis_base& zone_dis = zone_dis_base();
  //   const Zone& zone_geom = zone_dis.zone();
  const DoubleTab& coord = la_zone.domaine().coord_sommets();
  const IntTab& sommet_poly = la_zone.les_elems();

  if (val.nb_dim() == 1)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(nb_compo_ == 1);
    }
  else if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo_);
    }
  else
    {
      Cerr << "Erreur TRUST dans Champ_P1NC::valeur_aux()\n";
      Cerr << "Le DoubleTab val a plus de 2 entrees\n";
      exit();
    }

  int le_poly;

  if (dimension == 2)
    {
      Cerr << "ATTENTION : Cela n a pas encore ete teste en 2D!!!" << finl;
      Cerr << "Il manque les fonctions de forme en 2D!! A vous de jouer!!" << finl;
    }

  if (nb_compo_ == 1)
    {
      const DoubleVect& ch = valeurs();

      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            val(rang_poly) = 0;
          else
            {
              // Calcul d'apres les fonctions de forme sur le triangle
              val(rang_poly) = 0;
              if (dimension == 2)
                {
                  xs = positions(rang_poly,0);
                  ys = positions(rang_poly,1);
                  for (int i=0; i< 3; i++)
                    {
                      som = sommet_poly(le_poly,i);
                      val(rang_poly) += ch(som)* coord_barycentrique2D(sommet_poly, coord, xs, ys,le_poly, i);
                    }
                }
              else if (dimension == 3)
                {
                  xs = positions(rang_poly,0);
                  ys = positions(rang_poly,1);
                  zs = positions(rang_poly,2);
                  for (int i=0; i< 4; i++)
                    {
                      som = sommet_poly(le_poly,i);
                      val(rang_poly) += ch(som) * coord_barycentrique3D(sommet_poly, coord, xs, ys, zs,le_poly, i);
                    }
                }
            }
        }
    }
  else // nb_compo_ > 1
    {
      const DoubleTab& ch = valeurs();

      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            for(int ncomp=0; ncomp<nb_compo_; ncomp++)
              val(rang_poly, ncomp) = 0;
          else
            {
              for(int ncomp=0; ncomp<nb_compo_; ncomp++)
                {
                  val(rang_poly, ncomp) = 0;
                  if (dimension == 2)
                    {
                      xs = positions(rang_poly,0);
                      ys = positions(rang_poly,1);
                      for (int i=0; i< 3; i++)
                        {
                          som = sommet_poly(le_poly,i);
                          val(rang_poly, ncomp) += ch(som, ncomp)
                                                   * coord_barycentrique2D(sommet_poly, coord, xs, ys, le_poly, i);
                        }
                    }
                  else if (dimension == 3)
                    {
                      xs = positions(rang_poly,0);
                      ys = positions(rang_poly,1);
                      zs = positions(rang_poly,2);
                      for (int i=0; i< 4; i++)
                        {
                          som = sommet_poly(le_poly,i);
                          val(rang_poly, ncomp) += ch(som, ncomp)
                                                   * coord_barycentrique3D(sommet_poly, coord, xs, ys, zs,
                                                                           le_poly, i);
                        }
                    }
                }
            }
        }
    }
  return val;
}

// Description:
//
DoubleVect& Champ_som_lu_VEF::valeur_aux_elems_compo(const DoubleTab& positions,
                                                     const IntVect& les_polys ,
                                                     DoubleVect& val,
                                                     int ncomp) const
{
  int som;
  double xs,ys,zs;

  const Zone& la_zone = mon_domaine->zone(0);
  //   const Zone_dis_base& zone_dis = zone_dis_base();
  //   const Zone& zone_geom = zone_dis.zone();
  const DoubleTab& coord = la_zone.domaine().coord_sommets();
  const IntTab& sommet_poly = la_zone.les_elems();
  assert(val.size() == les_polys.size());
  int le_poly;

  const DoubleTab& ch = valeurs();

  if (dimension == 2)
    {
      Cerr << "ATTENTION : Cela n a pas encore ete teste en 2D!!!" << finl;
      Cerr << "Il manque les fonctions de forme en 2D!! A vous de jouer!!" << finl;
      exit();
    }

  for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
    {
      le_poly=les_polys(rang_poly);
      if (le_poly == -1)
        val(rang_poly) = 0;
      else
        {
          val(rang_poly) = 0;
          if (dimension == 2)
            {
              xs = positions(rang_poly,0);
              ys = positions(rang_poly,1);
              for (int i=0; i< 3; i++)
                {
                  som = sommet_poly(le_poly,i);
                  val(rang_poly) += ch(som, ncomp)
                                    * coord_barycentrique2D(sommet_poly, coord, xs, ys, le_poly, i);
                }
            }
          else if (dimension == 3)
            {
              xs = positions(rang_poly,0);
              ys = positions(rang_poly,1);
              zs = positions(rang_poly,2);
              for (int i=0; i< 4; i++)
                {
                  som = sommet_poly(le_poly,i);
                  val(rang_poly) += ch(som, ncomp)
                                    * coord_barycentrique3D(sommet_poly, coord, xs, ys, zs,
                                                            le_poly, i);
                }
            }
          else
            Cerr << "Il y a un pbl : cela ne marche qu en dimension 3" << finl;

        }
    }
  return val;
}

double coord_barycentrique3D(const IntTab& polys,
                             const DoubleTab& coord,
                             double x, double y, double z,
                             int le_poly, int i)
{
  int som0,som1,som2,som3;

  switch(i)
    {
    case 0 :
      {
        som0=polys(le_poly , 0);
        som1=polys(le_poly , 1);
        som2=polys(le_poly , 2);
        som3=polys(le_poly , 3);
        break;
      }
    case 1 :
      {
        som0=polys(le_poly , 1);
        som1=polys(le_poly , 2);
        som2=polys(le_poly , 3);
        som3=polys(le_poly , 0);
        break;
      }
    case 2 :
      {
        som0=polys(le_poly , 2);
        som1=polys(le_poly , 3);
        som2=polys(le_poly , 0);
        som3=polys(le_poly , 1);
        break;
      }
    case 3 :
      {
        som0=polys(le_poly , 3);
        som1=polys(le_poly , 0);
        som2=polys(le_poly , 1);
        som3=polys(le_poly , 2);
        break;
      }
    default:
      {
        som0=som1=som2=som3=-1;
        assert(0);
        Process::exit();
        break;
      }
    }

  double xp = (coord(som2,1)-coord(som1,1))*(coord(som0,2)-coord(som1,2))
              - (coord(som2,2)-coord(som1,2))*(coord(som0,1)-coord(som1,1));
  double yp = (coord(som2,2)-coord(som1,2))*(coord(som0,0)-coord(som1,0))
              - (coord(som2,0)-coord(som1,0))*(coord(som0,2)-coord(som1,2));
  double zp = (coord(som2,0)-coord(som1,0))*(coord(som0,1)-coord(som1,1))
              - (coord(som2,1)-coord(som1,1))*(coord(som0,0)-coord(som1,0));
  double den = xp * (coord(som3,0)-coord(som1,0))
               + yp * (coord(som3,1)-coord(som1,1))
               + zp * (coord(som3,2)-coord(som1,2));

  xp = (coord(som2,1)-coord(som1,1))*(z-coord(som1,2))
       - (coord(som2,2)-coord(som1,2))*(y-coord(som1,1));
  yp = (coord(som2,2)-coord(som1,2))*(x-coord(som1,0))
       - (coord(som2,0)-coord(som1,0))*(z-coord(som1,2));
  zp = (coord(som2,0)-coord(som1,0))*(y-coord(som1,1))
       - (coord(som2,1)-coord(som1,1))*(x-coord(som1,0));
  double num = xp * (coord(som3,0)-coord(som1,0))
               + yp * (coord(som3,1)-coord(som1,1))
               + zp * (coord(som3,2)-coord(som1,2));

  assert (den != 0.);
  double coord_bary = num/den;
  assert(sup_ou_egal(coord_bary,0) && inf_ou_egal(coord_bary,1));
  return coord_bary;
}

double coord_barycentrique2D(const IntTab& polys,
                             const DoubleTab& coord,
                             double x, double y,
                             int le_poly, int i)
{
  int som0,som1;

  som0=polys(le_poly , i);
  som1=polys(le_poly , 1);
  if(i==1) som1=polys(le_poly , 0);


  double xref = coord(som1,0)-coord(som0,0);
  double yref = coord(som1,1)-coord(som0,1);

  double xp = x-coord(som0,0);
  double yp = y-coord(som0,1);

  double pscal = (xref*xp) + (yref*yp);
  double norm  = (xref*xref) + (yref*yref);

  assert (norm != 0.);
  double coord_bary = pscal/norm;
  assert(sup_ou_egal(coord_bary,0) && inf_ou_egal(coord_bary,1));
  return coord_bary;
}
