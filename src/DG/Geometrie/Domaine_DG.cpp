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


/*! @brief Compute mesh parameters, allocate quadratures and link them to the domain
 *
 * The function use Domaine_Poly_base::discretiser() that builds every geometric connectivities that are not in the mesh file
 */
void Domaine_DG::discretiser()
{
  Domaine_Poly_base::discretiser();
  build_nfaces_elem_();

  gram_schmidt_ = Option_DG::GRAM_SCHMIDT;

  compute_mesh_param();
  calculer_h_carre();
  Quadrature_base* quad1 = new Quadrature_Ord1_Polygone(*this);
  Quadrature_base* quad2 = new Quadrature_Ord3_Polygone(*this);
  Quadrature_base* quad5 = new Quadrature_Ord5_Polygone(*this);
  set_quadrature(1, quad1);
  set_quadrature(2, quad2);
  set_quadrature(5, quad5);


  int nelem_tot = nb_elem_tot();
  const int nb_faces_max = elem_faces_.dimension(1);

  stencil_sorted_.resize(nelem_tot, nb_faces_max+1);
  stencil_sorted_ = -1;

  int elem1, elem2, size_stencil, face;

  std::vector<int> elem_stencil;

  for (int nelem = 0 ; nelem < nelem_tot ; nelem++)
    {
      elem_stencil.clear();
      elem_stencil.push_back(nelem);
      for (int num_face = 0 ; num_face < nb_faces_max; num_face++)
        {
          if ( elem_faces(nelem,num_face) < 0 ) break;

          face = elem_faces(nelem,num_face);
          elem1 = face_voisins(face, 0);
          elem2 = face_voisins(face, 1);

          if (elem1 != nelem)
            elem_stencil.push_back(elem1);
          else if (elem2 != -1)
            elem_stencil.push_back(elem2);
        }
      std::sort(elem_stencil.begin(), elem_stencil.end());

      size_stencil = (int)elem_stencil.size();
      for (int k = 0; k < size_stencil; k++)
        stencil_sorted_(nelem,k) = elem_stencil[k];
    }

}
/*! @brief Set the global default order
 *
 * @param order : order to specify
 */
void Domaine_DG::set_default_order(int order)
{
  order_quad_=order;
}

/*! @brief New feature in Trust, not yet available in DG, ask Elie
 *
 */
void Domaine_DG::init_equiv() const
{
  //TODO DG
}

/*! @brief Compute positions of the quadrature points
 *
 * For now it uses the default quadrature order and compute positions for every cells
 *
 * @param positions coordinates of the quadrature points
 */
void Domaine_DG::get_position(DoubleTab& positions) const
{
  //TODO Kokkos DG
  const Quadrature_base& quad = get_quadrature(5);
  positions = quad.get_integ_points();
}

/*! @brief Give an IntTab that contains the number of integration points for each cell
 *
 * @param nb_integ_points : nb_integ_points[i] give the number of integration points for cell i
 *
 */
void Domaine_DG::get_nb_integ_points(IntTab& nb_integ_points) const
{
  const Quadrature_base& quad = get_quadrature(5);
  nb_integ_points = quad.get_tab_nb_pts_integ();
//  nb_integ_points.ref(tab_pts_integ);
}

/*! @brief Create the indirection that give for each cell, the index number of the first integration point
 *
 * @param ind_integ_points : ind_integ_points[i] give the index of the first integration point associated with cell i
 */
void Domaine_DG::get_ind_integ_points(IntTab& ind_integ_points) const
{
  const Quadrature_base& quad = get_quadrature(5);
  ind_integ_points = quad.get_ind_pts_integ();
//  ind_integ_points.ref(ind_pts_integ);
}

/*! @brief Compute L_1 norm
 *
 */
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
/*! @brief Compute L_2 norm
 *
 */
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
/*! @brief Compute geometric quantities used for the computation
 *  TODO :: Put this in the Domain_Poly_base and delete h_carre
 */
void Domaine_DG::compute_mesh_param()
{
  int nb_elem_tot = this->nb_elem_tot();
  DoubleTab& xs = domaine().les_sommets(); // facets barycentre
  const IntTab& vert_elems = domaine().les_elems();
  const IntTab& elem_faces=this->elem_faces();
  const IntTab& face_som=this->face_sommets();
  dia_.resize(nb_elem_tot); ///< Array of the diameter for each cell
  per_.resize(nb_elem_tot); ///< Perimeter of each cell
  rho_.resize(nb_elem_tot); ///< Diameter of the largest incircle for each cell
  sig_.resize(nb_elem_tot); ///< Aspect ratio of each cell

  for (int e = 0; e < nb_elem_tot; e++)
    {
      int nsom = nfaces_elem_(e);
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
          rho_(e) = 4. * volumes(e) / per_(e);
          sig_(e) = dia_(e) / rho_(e);
        }
      else
        {
          per_(e)=0;
          double h_e=0;
          for (int f = 0; f < nsom; f++)  // local index of facet
            {
              int f_e = elem_faces(e,f);
              int s1 = face_som(f_e,0);
              int s2 = face_som(f_e,1);
//              double x1= xs(s1,0);
//              double y2=xs(s2,1);
//              double x2=xs(s2,0);
//              double y1=xs(s1,1);
//              double prod=x1*y2-x2*y1;
//              std::cout<< "Les sommets sont ("<<x1<<","<<y1<<") et (" <<x2<<","<<y2<<") et prod : "<<prod<<std::endl ;
              per_(e)+= std::sqrt((xs(s1,0) - xs(s2,0)) * (xs(s1,0) - xs(s2,0)) + (xs(s1,1) - xs(s2,1)) * (xs(s1,1) - xs(s2,1)));

              // calcul of h
              for (int loc_vert2 = f+1; loc_vert2 < nsom; loc_vert2++) // That's tricky: we use the bijection between vertices and faces to loop over the vertices simultaneously.
                h_e=std::max(h_e,std::sqrt((xs(vert_elems(e, f),0) - xs(vert_elems(e, loc_vert2),0)) * (xs(vert_elems(e, f),0) - xs(vert_elems(e, loc_vert2),0)) + (xs(vert_elems(e, f),1) - xs(vert_elems(e, loc_vert2),1)) * (xs(vert_elems(e, f),1) - xs(vert_elems(e, loc_vert2),1)))); // max between the distance of each vertices
            }
          rho_(e)=2.*(volumes(e)/per_(e));
          dia_(e)=h_e;
          sig_(e)=h_e/rho_(e);
        }
    }
}

/*! @brief Should disappear, as well as h_carre, as we have dia_, this come with a refactoring of Domaine_Poly_base
 *
 */
void Domaine_DG::calculer_h_carre()
{
  h_carre=0;
  for (int e = 0; e < this->nb_elem(); e++)
    {
      h_carre=std::max(h_carre,dia_(e)*dia_(e));
    }
  h_carre = Process::mp_max(h_carre);

  // Calcul de h_carre
  //h_carre = 1;
  if (h_carre_.size()) return; // deja fait
  int nb_elem_tot = this->nb_elem_tot();
  h_carre_.resize(nb_elem_tot);
  h_carre_ = h_carre;
//  h_carre = 1.;
}

/*! @brief Create an array that store the number of faces per element
 *
 * If the mesh is only composed with the same type of element, return yes.
 */
bool Domaine_DG::build_nfaces_elem_()
{
  nfaces_elem_.resize(nb_elem_tot());
  bool only_tri_quad=true;
  const IntTab& elem_face = elem_faces();
  int nb_f_elem_max=elem_face.dimension(1);

  if (Objet_U::dimension == 2)
    {
      for (int e = 0; e < nb_elem_tot(); e++)
        {
          if ((nb_f_elem_max == 3) || (elem_face(e, 3) == -1))
            {
              nfaces_elem_(e) = 3; // triangles
            }
          else if ((nb_f_elem_max == 4) || (elem_face(e, 4) == -1))
            {
              nfaces_elem_(e) = 4; // quads
            }
          else
            {
              int i_f = 5;
              for (; i_f < nb_f_elem_max; i_f++)
                {
                  if (elem_face(e, i_f) == -1)
                    {
                      nfaces_elem_(e) = i_f; // polys
                      break;
                    }
                }
              if (i_f == nb_f_elem_max)
                nfaces_elem_(e) = i_f; // full poly
            }
        }
    }
  else
    {
      Process::exit("3D not implemented yet");
    }
  return only_tri_quad;
}
