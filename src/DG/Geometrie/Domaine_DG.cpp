/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Domaine_Cl_DG.h>
#include <Domaine_DG.h>
#include <Option_DG.h>
#include <Poly_geom_base.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <TRUSTList.h>
#include <Domaine.h>
#include <Hexa_poly.h>
#include <Tri_poly.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <Tetraedre.h>
#include <Rectangle.h>
#include <Quadri_poly.h>
#include <Tetra_poly.h>
#include <EChaine.h>
#include <Interprete_bloc.h>
#include <Quadrature_base.h>
#include <Quadrature_Ord1_Polygone.h>
#include <Quadrature_Ord3_Polygone.h>
#include <Quadrature_Ord5_Polygone.h>
#include <Champ_Elem_DG.h>

Implemente_instanciable(Domaine_DG, "Domaine_DG", Domaine_Poly_base);

Sortie& Domaine_DG::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }

Entree& Domaine_DG::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }

void Domaine_DG::discretiser()
{
  Domaine_Poly_base::discretiser();


  // if triangle elem

  // const Nom& type_elem_geom = domaine().type_elem()->que_suis_je();
  bool tri_or_quad_only = rempli_type_elem(); // bool_only_triangle_and_quadrangle + fullfield the tab type_elem
  if (tri_or_quad_only==false)
    Process::exit("General meshes not implemented yet"); // TODO : Change this if general meshes implemented
  compute_mesh_param();
  calculer_h_carre();
  Quadrature_base* quad1 = new Quadrature_Ord1_Polygone(*this);
  Quadrature_base* quad2 = new Quadrature_Ord3_Polygone(*this);
  Quadrature_base* quad5 = new Quadrature_Ord5_Polygone(*this);
  set_quadrature(1, quad1);
  set_quadrature(2, quad2);
  set_quadrature(5, quad5);


}

void Domaine_DG::set_default_order(int order)
{
  order_quad_=order;
}


void Domaine_DG::init_equiv() const
{
  //TODO DG
}

void Domaine_DG::get_position(DoubleTab& positions) const
{
  //TODO Kokkos DG
  const Quadrature_base& quad = get_quadrature(5);
  positions = quad.get_integ_points();
}

void Domaine_DG::get_nb_integ_points(IntTab& nb_integ_points) const
{
  const Quadrature_base& quad = get_quadrature();
  nb_integ_points = quad.get_tab_nb_pts_integ();
//  nb_integ_points.ref(tab_pts_integ);
}

void Domaine_DG::get_ind_integ_points(IntTab& ind_integ_points) const
{
  const Quadrature_base& quad = get_quadrature();
  ind_integ_points = quad.get_ind_pts_integ();
//  ind_integ_points.ref(ind_pts_integ);
}


double Domaine_DG::compute_L1_norm(const DoubleVect& val_source) const
{
  const Quadrature_base& quad = get_quadrature();
  int nb_pts_integ_max = quad.nb_pts_integ_max();
  int nelem = nb_elem();

  DoubleTab val_elem(nb_pts_integ_max);

  double sum = 0.;

  for (int i = 0; i < nelem; i++)
    {
      for (int k = 0; k < quad.nb_pts_integ(i) ; k++)
        val_elem(k) = std::fabs(val_source(i*nb_pts_integ_max+k));

      sum += quad.compute_integral_on_elem(i, val_elem);
    }

  return sum;
}

double Domaine_DG::compute_L2_norm(const DoubleVect& val_source) const
{
  const Quadrature_base& quad = get_quadrature();
  int nb_pts_integ_max = quad.nb_pts_integ_max();
  int nelem = nb_elem();

  DoubleTab val_elem(nb_pts_integ_max);

  double sum = 0.;

  for (int i = 0; i < nelem; i++)
    {
      for (int k = 0; k < quad.nb_pts_integ(i) ; k++)
        val_elem(k) = val_source(i*nb_pts_integ_max+k)*val_source(i*nb_pts_integ_max+k);

      sum += quad.compute_integral_on_elem(i, val_elem);
    }

  return sum;
}

void Domaine_DG::compute_mesh_param()
{
  int nb_elem = this->nb_elem();
  DoubleTab& xs = domaine().les_sommets(); // facets barycentre
  const IntTab& vert_elems = domaine().les_elems();
  const IntTab& elem_faces=this->elem_faces();
  const IntTab& face_som=this->face_sommets();
  dia_.resize(nb_elem);
  invdia_.resize(nb_elem);
  per_.resize(nb_elem);
  rho_.resize(nb_elem);
  sig_.resize(nb_elem);
  surf_.resize(nb_elem);

  for (int e = 0; e < nb_elem; e++)
    {
      int nsom = type_elem_(e);
      if (nsom==3)
        {
          dia_(e) = 1 / (2 * volumes(e));
          int nb_elem_face = 3; // nb_elem_faces(e)
          for (int i_f = 0; i_f < nb_elem_face; i_f++)
            {
              int f = elem_faces(e, i_f);
              double sur_f = face_surfaces(f);
              dia_(e) *= sur_f;
              per_(e) += sur_f; //
            }
          invdia_(e) = 1. / dia_(e);
          rho_(e) = 4. * volumes(e) / per_(e);
          sig_(e) = dia_(e) / rho_(e);
        }
      else
        {
          surf_(e)=0;
          per_(e)=0;
          double h_e=0;
          for (int f = 0; f < nsom; f++)  // local index of facet
            {
              int f_e = elem_faces(e,f);
              int s1 = face_som(f_e,0);
              int s2 = face_som(f_e,1);
              double x1= xs(s1,0);
              double y2=xs(s2,1);
              double x2=xs(s2,0);
              double y1=xs(s1,1);
              double prod=x1*y2-x2*y1;
              std::cout<< "Les sommets sont ("<<x1<<","<<y1<<") et (" <<x2<<","<<y2<<") et prod : "<<prod<<std::endl ;
              surf_(e) +=std::abs(xs(s1,0)*xs(s2,1)-xs(s2,0)*xs(s1,1)); // It s 2 times the surface but then we need to multiply by 2 ...
              per_(e)+= std::sqrt((xs(s1,0) - xs(s2,0)) * (xs(s1,0) - xs(s2,0)) + (xs(s1,1) - xs(s2,1)) * (xs(s1,1) - xs(s2,1)));

              // calcul of h
              for (int loc_vert2 = f+1; loc_vert2 < nsom; loc_vert2++) // That's tricky: we use the bijection between vertices and faces to loop over the vertices simultaneously.
                h_e=std::max(h_e,std::sqrt((xs(vert_elems(e, f),0) - xs(vert_elems(e, loc_vert2),0)) * (xs(vert_elems(e, f),0) - xs(vert_elems(e, loc_vert2),0)) + (xs(vert_elems(e, f),1) - xs(vert_elems(e, loc_vert2),1)) * (xs(vert_elems(e, f),1) - xs(vert_elems(e, loc_vert2),1)))); // max between the distance of each vertices
            }
          surf_(e)=std::abs(surf_(e)); // can be negative otherwise
          rho_(e)=(surf_(e)/per_(e));
          dia_(e)=h_e;
          invdia_(e)=1/h_e;
          sig_(e)=h_e/rho_(e);
        }
    }
}

//TODO DG h_carre with diameter
void Domaine_DG::calculer_h_carre()
{
  h_carre=0;
  int nb_elem = this->nb_elem();
  for (int e = 0; e < nb_elem; e++)
    {
      h_carre=std::max(h_carre,dia_(e)*dia_(e));
    }
  // Calcul de h_carre
  //h_carre = 1;
  if (h_carre_.size()) return; // deja fait
  h_carre_.resize(nb_elem_tot());
  h_carre_ = h_carre;
//  h_carre = 1.;
}


bool Domaine_DG::rempli_type_elem()
{
  type_elem_.resize(nb_elem_tot());
  bool only_tri_quad=true;
  const IntTab& elem_face = elem_faces();
  int nb_f_elem_max=elem_face.dimension(1);

  if (Objet_U::dimension == 2)
    {
      if (nb_f_elem_max == 3) // que des triangles
        {
          for (int e = 0; e < nb_elem_tot(); e++)
            {
              type_elem_(e) = 3; // triangle
            }
        }
      else if (nb_f_elem_max == 4) // que des quadrangle ou m��lange triangle et quadrangle
        {
          for (int e = 0; e < nb_elem_tot(); e++)
            {
              if (elem_face(e, 3) == -1)
                {
                  type_elem_(e) = 3; // triangle
                  continue;
                }
              type_elem_(e) = 4; // quadrangle
            }
        }
      else // melange polygone
        {
          only_tri_quad = false;
          for (int e = 0; e < nb_elem_tot(); e++)
            {
              if (elem_face(e, 3) == -1)
                {
                  type_elem_(e) = 3; // triangle
                  continue;
                }
              if (elem_face(e, 4) == -1)
                {
                  type_elem_(e) = 4; // quadrangle
                  continue;
                }
              for (int i_f = 5; i_f < nb_f_elem_max; i_f++)
                {
                  if (elem_face(e, i_f) == -1 || i_f == nb_f_elem_max)
                    type_elem_(e) = i_f+1; // polygone
                  continue;
                }
            }
        }
    }
  else
    {
      Process::exit("3D not implemented yet");
    }
  return only_tri_quad;
}
