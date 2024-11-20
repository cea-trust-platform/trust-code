/****************************************************************************
* Copyright (c) 2024, CEA
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
#include <Quadrature_Ord1_Triangle.h>
#include <Quadrature_Ord2_Triangle.h>
#include <Quadrature_Ord5_Triangle.h>
#include <Quadrature_Ord1_Quadrangle.h>
#include <Quadrature_Ord3_Quadrangle.h>
#include <Quadrature_Ord5_Quadrangle.h>
#include <Champ_Elem_DG.h>

Implemente_instanciable(Domaine_DG, "Domaine_DG", Domaine_Poly_base);

Sortie& Domaine_DG::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }

Entree& Domaine_DG::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }

void Domaine_DG::discretiser()
{
  Domaine_Poly_base::discretiser();

  calculer_h_carre();
  compute_mesh_param();
  // if triangle elem

  const Nom& type_elem_geom = domaine().type_elem()->que_suis_je();

  if (type_elem_geom == "Triangle")
    {
      Quadrature_base* quad1 = new Quadrature_Ord1_Triangle(*this);
      Quadrature_base* quad2 = new Quadrature_Ord2_Triangle(*this);
      Quadrature_base* quad5 = new Quadrature_Ord5_Triangle(*this);
      // association of the quads with the dom
      set_quadrature(1, quad1);
      set_quadrature(2, quad2);
      set_quadrature(5, quad5);
    }
  else if (type_elem_geom == "Quadrangle")
    {
      // If Quadrangle elem
      Quadrature_base* quad1 = new Quadrature_Ord1_Quadrangle(*this);
      Quadrature_base* quad2 = new Quadrature_Ord3_Quadrangle(*this);  // yes 3, who can do the most can do the least
      Quadrature_base* quad5 = new Quadrature_Ord5_Quadrangle(*this);
      // association of the quads with the dom
      set_quadrature(1, quad1);
      set_quadrature(2, quad2);
      set_quadrature(5, quad5);
    }
  else
    {
      Process::exit("This geometry is not yet covered in DG");
    }

}

void Domaine_DG::get_position(DoubleTab& positions) const
{
  //TODO Kokkos DG
  const Quadrature_base& quad = get_quadrature(5);
  positions = quad.get_integ_points();
}

double Domaine_DG::compute_L1_norm(const DoubleVect& val_source) const
{
  const Quadrature_base& quad = get_quadrature(5);
  int nb_pts_integ = quad.nb_pts_integ();
  int nelem = nb_elem();

  DoubleTab val_elem(nb_pts_integ);

  double sum = 0.;

  for (int i = 0; i < nelem; i++)
    {
      for (int k = 0; k < nb_pts_integ ; k++)
        val_elem(k) = std::fabs(val_source(i*nb_pts_integ+k));

      sum += quad.compute_integral_on_elem(i, val_elem);
    }

  return sum;
}

double Domaine_DG::compute_L2_norm(const DoubleVect& val_source) const
{
  const Quadrature_base& quad = get_quadrature(5);
  int nb_pts_integ = quad.nb_pts_integ();
  int nelem = nb_elem();

  DoubleTab val_elem(nb_pts_integ);

  double sum = 0.;

  for (int i = 0; i < nelem; i++)
    {
      for (int k = 0; k < nb_pts_integ ; k++)
        val_elem(k) = val_source(i*nb_pts_integ+k)*val_source(i*nb_pts_integ+k);

      sum += quad.compute_integral_on_elem(i, val_elem);
    }

  return sum;
}

void Domaine_DG::compute_mesh_param()
{
  /*
  int nb_elem = this->nb_elem();

  DiaTri_.resize(nb_elem);
  invDiaTri_.resize(nb_elem);
  PerTri_.resize(nb_elem);
  rhoTri_.resize(nb_elem);
  sigTri_.resize(nb_elem);

  for (int e = 0; e < nb_elem; e++)
    {
      DiaTri_(e) = 1 / (2 * this->volumes(e));
      int nb_elem_face = 3; // nb_elem_faces(e)
      for (int i_f = 1; i_f < nb_elem_face; i_f++)
        {
          int f = this->elem_faces(e, i_f);
          double sur_f = this->face_surfaces(f);
          DiaTri_(e) *= sur_f;
          PerTri_(e) += sur_f; //
        }
    }

  for (int e = 0; e < this->nb_elem(); e++)
    {
      invDiaTri_(e) = 1. / DiaTri_(e);
      rhoTri_(e) = 4. * this->volumes(e) / PerTri_(e);
      sigTri_(e) = DiaTri_(e) / rhoTri_(e);
    }
    */
}

//TODO DG h_carre with diameter
//void Domaine_DG::calculer_h_carre()
//{
//  // Calcul de h_carre
//  h_carre = 1;
//  if (h_carre_.size()) return; // deja fait
//  h_carre_.resize(nb_elem_tot());
//
//  h_carre_ = 1.;
////  h_carre = 1.;
//}

