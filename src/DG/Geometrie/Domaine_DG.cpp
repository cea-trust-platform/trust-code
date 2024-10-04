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


Implemente_instanciable(Domaine_DG, "Domaine_DG", Domaine_Poly_base);

Sortie& Domaine_DG::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }

Entree& Domaine_DG::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }

void Domaine_DG::discretiser()
{
  Domaine_Poly_base::discretiser();

  calculer_h_carre();

  Quadrature_base* quad1 = new Quadrature_Ord1_Triangle(*this);
  Quadrature_base* quad2 = new Quadrature_Ord2_Triangle(*this);
  Quadrature_base* quad5 = new Quadrature_Ord5_Triangle(*this);

  set_quadrature(1, quad1);
  set_quadrature(2, quad2);
  set_quadrature(5, quad5);

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

