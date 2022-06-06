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

#ifndef Champ_implementation_P1_included
#define Champ_implementation_P1_included

#include <Champ_implementation_sommet_base.h>
#include <TRUSTTab.h>

class Champ_implementation_P1 : public Champ_implementation_sommet_base
{

protected :
  Champ_base& le_champ(void) override =0;
  const Champ_base& le_champ(void) const override =0;

public :
  inline ~Champ_implementation_P1() override { };
  static void init_from_file(DoubleTab& val, const Domaine& dom, int nb_comp, double tolerance, Entree& input);

protected :
  void value_interpolation(const DoubleTab& positions, const ArrOfInt& cells, const DoubleTab& values, DoubleTab& resu, int ncomp=-1) const override;
private :
  virtual double form_function(const ArrOfDouble& position, const IntTab& les_elems, const DoubleTab& nodes, ArrOfInt& index, int cell, int ddl) const;
};
extern double coord_barycentrique_P1(const IntTab& polys, const DoubleTab& coord, double x, double y, int le_poly, int i);
extern double coord_barycentrique_P1(const IntTab& polys, const DoubleTab& coord, double x, double y, double z, int le_poly, int i);

// Defines 4 optimized inlined function called in Champ_implementation_P1.cpp by the 2 previous coord_barycentrique_P1 functions:
// coord_barycentrique_P1_triangle
// coord_barycentrique_P1_rectangle
// coord_barycentrique_P1_tetraedre
// coord_barycentrique_P1_hexaedre
inline double coord_barycentrique_P1_triangle(const IntTab& polys, const DoubleTab& coord, double x, double y, int le_poly, int i)
{
  assert(polys.dimension(1)==3);
  int som0 ,som1,som2;
  switch(i)
    {
    case 0 :
      {
        som0=polys(le_poly , 0);
        som1=polys(le_poly , 1);
        som2=polys(le_poly , 2);
        break;
      }
    case 1 :
      {
        som0=polys(le_poly , 1);
        som1=polys(le_poly , 2);
        som2=polys(le_poly , 0);
        break;
      }
    case 2 :
      {
        som0=polys(le_poly , 2);
        som1=polys(le_poly , 0);
        som2=polys(le_poly , 1);
        break;
      }
    default :
      {
        som0=-1;
        som1=-1;
        som2=-1;
        Cerr << "A triangle does not have " << i << " nodes in Champ_P1::coord_barycentrique." << finl;
        Process::exit();
      }
    }
  double den = (coord(som2,0)-coord(som1,0))*(coord(som0,1)-coord(som1,1))
               - (coord(som2,1)-coord(som1,1))*(coord(som0,0)-coord(som1,0));

  double num = (coord(som2,0)-coord(som1,0))*(y-coord(som1,1))
               - (coord(som2,1)-coord(som1,1))*(x-coord(som1,0));

  assert (den != 0.);
  double coord_bary=num/den;
#ifndef NDEBUG
  if ((coord_bary < -Objet_U::precision_geom) || (coord_bary > 1.+Objet_U::precision_geom))
    {
      Cerr << "WARNING: The barycentric coordinate of point :" << finl;
      Cerr << "x= " << x << " y=" << y << finl;
      Cerr << "is not between 0 and 1 : " << coord_bary << finl;
      Cerr << "On the element " << le_poly << " of the processor " << Process::me() << finl;
    }
#endif
  return coord_bary;
}

inline double coord_barycentrique_P1_rectangle(const IntTab& polys,
                                               const DoubleTab& coord,
                                               double x, double y, int le_poly, int i)
{
  assert(polys.dimension(1)==4);
  double alpha_x, alpha_y;
  switch(i)
    {
    case 0 :
      {
        alpha_x = -1.;
        alpha_y = -1;
        break;
      }
    case 1 :
      {
        alpha_x = 1.;
        alpha_y = -1.;
        break;
      }
    case 2 :
      {
        alpha_x = -1.;
        alpha_y = 1.;
        break;
      }
    case 3 :
      {
        alpha_x = 1.;
        alpha_y = 1.;
        break;
      }
    default :
      {
        alpha_x = 0;
        alpha_y = 0;
        Cerr << "Error in Champ_P1::coord_barycentrique : " << finl;
        Process::exit();
      }
    }

  int som0=polys(le_poly , 0);
  int som1=polys(le_poly , 1);
  int som2=polys(le_poly , 2);
  double delta_x = coord(som1,0)-coord(som0,0);
  double delta_y = coord(som2,1)-coord(som0,1);
  double x0 = coord(som0,0)+delta_x/2.;
  double y0 = coord(som0,1)+delta_y/2.;
  double coord_bary = 0.25*(1.+2.*alpha_x*(x-x0)/delta_x)*(1.+2.*alpha_y*(y-y0)/delta_y);
#ifndef NDEBUG
  if ((coord_bary < -Objet_U::precision_geom) || (coord_bary > 1.+Objet_U::precision_geom))
    {
      Cerr << "WARNING: The barycentric coordinate of point :" << finl;
      Cerr << "x= " << x << " y=" << y << finl;
      Cerr << "is not between 0 and 1 : " << coord_bary << finl;
      Cerr << "On the element " << le_poly << " of the processor " << Process::me() << finl;
    }
#endif
  return coord_bary;
}

inline double coord_barycentrique_P1_tetraedre(const IntTab& polys, const DoubleTab& coord, double x, double y, double z, int le_poly, int i)
{
  int som0,som1,som2,som3;
  assert(polys.dimension(1)==4);
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
    default :
      {
        som0=-1;
        som1=-1;
        som2=-1;
        som3=-1;
        Cerr << "Error in Champ_P1::coord_barycentrique : " << finl;
        Cerr << "A tetrahedron does not have " << i << "nodes " << finl;
        Process::exit();
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
#ifndef NDEBUG
  if ((coord_bary < -Objet_U::precision_geom) || (coord_bary > 1.+Objet_U::precision_geom))
    {
      Cerr << "WARNING: The barycentric coordinate of point :" << finl;
      Cerr << "x= " << x << " y=" << y << " z=" << z << finl;
      Cerr << "is not between 0 and 1 : " << coord_bary << finl;
      Cerr << "On the element " << le_poly << " of the processor " << Process::me() << finl;
    }
#endif
  return coord_bary;
}
inline double coord_barycentrique_P1_hexaedre(const IntTab& polys,
                                              const DoubleTab& coord,
                                              double x, double y, double z,
                                              int le_poly, int i)
{
  assert(polys.dimension(1)==8);
  double alpha_x, alpha_y, alpha_z;
  switch(i)
    {
    case 0 :
      {
        alpha_x = -1.;
        alpha_y = -1.;
        alpha_z = -1.;
        break;
      }
    case 1 :
      {
        alpha_x = 1.;
        alpha_y = -1.;
        alpha_z = -1.;
        break;
      }
    case 2 :
      {
        alpha_x = -1.;
        alpha_y = 1.;
        alpha_z = -1.;
        break;
      }
    case 3 :
      {
        alpha_x = 1.;
        alpha_y = 1.;
        alpha_z = -1.;
        break;
      }
    case 4 :
      {
        alpha_x = -1.;
        alpha_y = -1;
        alpha_z = 1.;
        break;
      }
    case 5 :
      {
        alpha_x = 1.;
        alpha_y = -1.;
        alpha_z = 1.;
        break;
      }
    case 6 :
      {
        alpha_x = -1.;
        alpha_y = 1.;
        alpha_z = 1.;
        break;
      }
    case 7 :
      {
        alpha_x = 1.;
        alpha_y = 1.;
        alpha_z = 1.;
        break;
      }
    default :
      {
        alpha_x = 0;
        alpha_y = 0;
        alpha_z = 0;
        Cerr << "Error in Champ_P1::coord_barycentrique : " << finl;
        Process::exit();
      }
    }

  int som0=polys(le_poly , 0);
  int som1=polys(le_poly , 1);
  int som2=polys(le_poly , 2);
  int som3=polys(le_poly , 4);

  double delta_x = coord(som1,0)-coord(som0,0);
  double delta_y = coord(som2,1)-coord(som0,1);
  double delta_z = coord(som3,2)-coord(som0,2);

  double x0 = coord(som0,0)+delta_x/2.;
  double y0 = coord(som0,1)+delta_y/2.;
  double z0 = coord(som0,2)+delta_z/2.;

  double coord_bary = (1./8.)*(1.+2.*alpha_x*(x-x0)/delta_x)*(1.+2.*alpha_y*(y-y0)/delta_y)*(1.+2.*alpha_z*(z-z0)/delta_z);
#ifndef NDEBUG
  if ((coord_bary < -Objet_U::precision_geom) || (coord_bary > 1.+Objet_U::precision_geom))
    {
      Cerr << "WARNING: The barycentric coordinate of point :" << finl;
      Cerr << "x= " << x << " y=" << y <<" z="<< z <<finl;
      Cerr << "is not between 0 and 1 : " << coord_bary << finl;
      Cerr << "On the element " << le_poly << " of the processor " << Process::me() << finl;
    }
#endif
  return coord_bary;
}

#endif /* Champ_implementation_P1_inclus */
