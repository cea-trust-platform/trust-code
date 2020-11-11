/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Champ_implementation_Q1.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_implementation_Q1.h>
#include <Domaine.h>
#include <Linear_algebra_tools.h>
#include <Linear_algebra_tools_impl.h>


#if 0

void Champ_implementation_Q1::value_interpolation(const ArrOfDouble& position, int cell, const DoubleTab& values, ArrOfDouble& resu,int ncomp) const
{
  const Zone&      zone              = get_zone_geom();
  const IntTab&    cells             = zone.les_elems();
  const int     nb_nodes_per_cell = zone.nb_som_elem();

  assert(cell>=0);
  assert(cell < cells.dimension_tot(0));

  if (ncomp!=-1)
    {
      for (int j=0; j< nb_nodes_per_cell; j++)
        {
          int node = cells(cell,j);
          resu(0) += values(node,ncomp) * form_function(position,cell,j);
        }
    }
  else
    {
      if (values.nb_dim()==1)
        {
          assert(resu.size_array()==1);
          for (int j=0; j< nb_nodes_per_cell; j++)
            {
              int node = cells(cell,j);
              resu(0) += values(node) * form_function(position,cell,j);
            }
        }
      else
        {
          int nb_components=resu.size_array();
          assert(values.nb_dim() == 2);
          assert(values.dimension(1) == nb_components);
          for (int j=0; j<nb_nodes_per_cell; j++)
            {
              double weight = form_function(position, cell, j);
              int node   = cells(cell,j);
              for (int k=0; k<nb_components; k++)
                {
                  resu(k) += values(node,k) * weight;
                }
            }
        }
    }
}
#else

static int faces_sommets_quadra[4][2] =
{
  { 0, 2 },
  { 0, 1 },
  { 1, 3 },
  { 2, 3 }
};




void calcul_plan_quadra(Vecteur3& coeff_plan,double& d,int cell,const IntTab& cells,const DoubleTab& nodes,int dir)
{
  // Plan 012
  int s0=cells(cell,faces_sommets_quadra[dir][0]);
  int s1=cells(cell,faces_sommets_quadra[dir][1]);
  double S0x=nodes(s0,0);
  double S0y=nodes(s0,1);
  double SOS1x=nodes(s1,0)-S0x;
  double SOS1y=nodes(s1,1)-S0y;
  double len=sqrt(SOS1x*SOS1x+SOS1y*SOS1y);

  coeff_plan.set(-SOS1y/len,SOS1x/len,0);
  d=-(S0x*coeff_plan[0]+S0y*coeff_plan[1]);


}

static int faces_sommets_hexa[6][4] =
{
  { 0, 2, 4, 6 },
  { 0, 1, 4, 5 },
  { 0, 1, 2, 3 },
  { 1, 3, 5, 7 },
  { 2, 3, 6, 7 },
  { 4, 5, 6, 7 }
};

void calcul_plan_hexa(Vecteur3& coeff_plan,double& d,int cell,const IntTab& cells,const DoubleTab& nodes,int dir)
{
  // Plan 012
  int s0=cells(cell,faces_sommets_hexa[dir][0]);
  int s1=cells(cell,faces_sommets_hexa[dir][1]);
  int s2=cells(cell,faces_sommets_hexa[dir][2]);


  Vecteur3 S0(nodes(s0,0),nodes(s0,1),nodes(s0,2));
  Vecteur3 S0S1(nodes(s1,0)-S0[0],nodes(s1,1)-S0[1],nodes(s1,2)-S0[2]);
  Vecteur3 S0S2(nodes(s2,0)-S0[0],nodes(s2,1)-S0[1],nodes(s2,2)-S0[2]);
  Vecteur3::produit_vectoriel(S0S1,S0S2,coeff_plan);
  coeff_plan*=(1./coeff_plan.length());
  d=-Vecteur3::produit_scalaire(coeff_plan,S0);
}

void Champ_implementation_Q1::value_interpolation(const DoubleTab& positions, const ArrOfInt& cells, const DoubleTab& values, DoubleTab& resu, int ncomp) const
{
  const Zone&      zone  = get_zone_geom();
  const IntTab&    les_elems = zone.les_elems();
  const DoubleTab& nodes = zone.domaine().les_sommets();
  const int nb_nodes_per_cell = les_elems.dimension(1);
  ArrOfDouble position(Objet_U::dimension);

  Vecteur3 coeff_plan;
  double d;
  IntTab cell3D;
  Vecteur3 coord_bar;

  resu = 0;
  for (int ic=0; ic<cells.size_array(); ic++)
    {
      int cell = cells(ic);
      if (cell<0) continue;
      for (int k = 0; k < Objet_U::dimension; k++)
        position(k) = positions(ic, k);

      if (nb_nodes_per_cell == 4)
        {
          if (Objet_U::dimension == 3)
            {
              cell3D.resize(1, 8);
              for (int i = 0; i < 4; i++)
                cell3D(0, 2 * i) = les_elems(cell, i);
              calcul_plan_hexa(coeff_plan, d, 0, cell3D, nodes, 0);
              double A = d;
              for (int i = 0; i < Objet_U::dimension; i++) A += coeff_plan[i] * position(i);
              if (dabs(A) > dabs(d) * 1e-5)
                {
                  Cerr << "Error point not in the plane " << finl;
                  Process::exit();
                }
              coord_bar = 0.5;
            }
          else
            for (int dir = 0; dir < 2; dir++)
              {

                calcul_plan_quadra(coeff_plan, d, cell, les_elems, nodes, dir);
                double A = d;
                for (int i = 0; i < Objet_U::dimension; i++) A += coeff_plan[i] * position(i);
                calcul_plan_quadra(coeff_plan, d, cell, les_elems, nodes, dir + 2);
                double B = d;
                for (int i = 0; i < Objet_U::dimension; i++) B += coeff_plan[i] * position(i);
                assert(inf_ou_egal((A * B), 0, 1e-10));
                coord_bar[dir] = A / (A - B);
              }
        }
      else
        {
          assert(Objet_U::dimension == 3);
          for (int dir = 0; dir < 3; dir++)
            {

              calcul_plan_hexa(coeff_plan, d, cell, les_elems, nodes, dir);
              double A = d;
              for (int i = 0; i < 3; i++) A += coeff_plan[i] * position(i);
              calcul_plan_hexa(coeff_plan, d, cell, les_elems, nodes, dir + 3);
              double B = d;
              for (int i = 0; i < 3; i++) B += coeff_plan[i] * position(i);
              assert(inf_ou_egal((A * B), 0, 1e-10));
              coord_bar[dir] = A / (A - B);
            }
        }
      ArrOfDouble val(nb_nodes_per_cell);
      int nb_dim = resu.nb_dim();
      int nb_components = nb_dim == 1 ? 1 : resu.dimension_tot(1);
      for (int k = 0; k < nb_components; k++)
        {
          if (ncomp != -1)
            {
              for (int j = 0; j < nb_nodes_per_cell; j++)
                {
                  int node = les_elems(cell, j);
                  val(j) = values(node, ncomp);
                }
            }
          else
            {
              if (values.nb_dim() == 1)
                {
                  for (int j = 0; j < nb_nodes_per_cell; j++)
                    {
                      int node = les_elems(cell, j);
                      val(j) = values(node);
                    }
                }
              else
                {
                  assert(values.nb_dim() == 2);
                  for (int j = 0; j < nb_nodes_per_cell; j++)
                    {
                      int node = les_elems(cell, j);
                      val(j) = values(node, k);
                    }
                }
            }
          double res = 0;
          if (nb_nodes_per_cell == 4)
            {
              const double& i = coord_bar[0];
              const double& j = coord_bar[1];
              double unmi = 1. - i;
              double unmj = 1. - j;

              res = unmj * (unmi * val[0] + i * val[1])
                    + j * (unmi * val[2] + i * val[3]);
            }
          else
            {
              assert(Objet_U::dimension == 3);
              const double& i = coord_bar[0];
              const double& j = coord_bar[1];
              const double& kk = coord_bar[2];
              double unmi = 1. - i;
              double unmj = 1. - j;
              double unmk = 1. - kk;
              res = unmk * (unmj * (unmi * val[0] + i * val[1])
                            + j * (unmi * val[2] + i * val[3]))
                    + kk * (unmj * (unmi * val[4] + i * val[5])
                            + j * (unmi * val[6] + i * val[7]));
            }
          if (nb_dim==1)
            resu(ic) = res;
          else
            resu(ic, k) = res;
        }
    }
}
#endif



static int info=0;

double Champ_implementation_Q1::form_function(const ArrOfDouble& position, int cell, int ddl) const
{
  if (info==0)
    {
      Cerr << "***********************************************************************" << finl;
      Cerr << "Warning : shape functions of the Q1 elements are not coded !" << finl;
      Cerr << finl;
      Cerr << "For the moment, the value of the elements is not interpolated." << finl;
      Cerr << "It returns the average value on the element, ie the point value" << finl;
      Cerr << "at the barycenter of the element" << finl;
      Cerr << finl;
      Cerr << "It is therefore recommended to postprocess the Q1 fields only"<< finl;
      Cerr << "on mesh nodes." << finl;
      Cerr << "***********************************************************************" << finl;
      info=1;
    }


  const Zone& zone_geom = get_zone_geom();
  int test_sommet=1;
  if (test_sommet)
    {
      const IntTab& cells    = zone_geom.les_elems();
      const DoubleTab& nodes = zone_geom.domaine().les_sommets();

      int nb_nodes_per_cell = cells.dimension(1);
      int sc=-1;
      double l=Objet_U::precision_geom*Objet_U::precision_geom;
      for (int s=0; s<nb_nodes_per_cell; s++)
        {
          int som=cells(cell,s);
          double dist=0;
          for (int j=0; j<Objet_U::dimension; j++)
            {
              double dx=(nodes(som,j)-position(j));
              dist+=dx*dx;
            }
          if (dist<l)
            sc=s;
        }
      if (sc!=-1)
        {
          //Cerr<< ddl<< " sc "<< sc <<finl;
          if (ddl==sc)
            return 1;
          else
            return 0;
        }

    }
  double factor = 1./zone_geom.nb_som_elem();
  return factor;
}

