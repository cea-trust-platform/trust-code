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

#include <Champ_Elem_DG.h>
#include <TRUSTTab_parts.h>
#include <Domaine_Cl_dis_base.h>
#include <Domaine_DG.h>
#include <Option_DG.h>
#include <Matrix_tools.h>
#include <Array_tools.h>


Implemente_instanciable(Champ_Elem_DG, "Champ_Elem_DG", Champ_Inc_P0_base);

Sortie& Champ_Elem_DG::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Elem_DG::readOn(Entree& s)
{
  lire_donnees(s);
  return s;
}

void Champ_Elem_DG::allocate_mass_matrix()
{
  assert(Objet_U::dimension == 2); // no triangle in 3D!

  int nb_elem_tot = le_dom_VF->nb_elem_tot();

  IntTab indice(0, 2);

  int current_indice = 0;
  indices_glob_elem_.resize(nb_elem_tot+1);
  indices_glob_elem_(0)=0;
  for (int e = 0; e < nb_elem_tot; e++)
    {
      for (int i = 0; i < nb_bfunc_; i++ )
        for (int j = 0; j < nb_bfunc_; j++ )
          indice.append_line( current_indice+i, current_indice+j);
      indices_glob_elem_(e+1) = indices_glob_elem_(e) +  nb_bfunc_;
      current_indice+=nb_bfunc_;
    }

  int size_inc = indices_glob_elem_(nb_elem_tot);

  tableau_trier_retirer_doublons(indice);
  Matrix_tools::allocate_morse_matrix(size_inc, size_inc, indice, mass_matrix_);
  Matrix_tools::allocate_morse_matrix(size_inc, size_inc, indice, inv_mass_matrix_);
}

void Champ_Elem_DG::build_mass_matrix()
{
  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());
  const DoubleVect& ve = domaine.volumes();

  const Elem_geom_base& elem_geom = domaine.domaine().type_elem().valeur();

  bool is_simplexe = strcmp(elem_geom.que_suis_je(), "Triangle") || strcmp(elem_geom.que_suis_je(), "Tetraedre");
  if (!is_simplexe) throw; //TODO for polyhedron

  int current_indice = 0;

  const Quadrature_base& quad = domaine.get_quadrature(2);
  int nb_pts_integ_max = quad.nb_pts_integ_max();
  const IntTab& tab_pts_integ= quad.get_tab_nb_pts_integ();
  DoubleTab fbase(nb_bfunc_, nb_pts_integ_max);
  DoubleTab product(nb_pts_integ_max);

  for (int e = 0; e < le_dom_VF->nb_elem_tot(); e++)
    {
      eval_bfunc(quad, e, fbase);

      mass_matrix_(current_indice, current_indice) = ve(e);

      if (order_ == 0) continue;

      /****************************************************************/
      /* Formule de quadrature : Ern, Finite Elements II, 2021, p 71  */
      /****************************************************************/

      for (int i=0; i<nb_bfunc_; i++)
        {
          for (int j=0; j<nb_bfunc_; j++)
            {
              product = 0.;
              for (int k = 0; k < tab_pts_integ(e) ; k++)
                product(k) = fbase(i, k) * fbase(j, k);

              mass_matrix_(current_indice+i, current_indice+j) = quad.compute_integral_on_elem(e, product);
            }
        }

      current_indice+=nb_bfunc_;
    }

}

void Champ_Elem_DG::build_inv_mass_matrix()
{
  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());

  const DoubleVect& ve = domaine.volumes();

  const Elem_geom_base& elem_geom = domaine.domaine().type_elem().valeur();

  bool is_simplexe = strcmp(elem_geom.que_suis_je(), "Triangle") || strcmp(elem_geom.que_suis_je(), "Tetraedre");
  if (!is_simplexe) throw; //TODO for polyhedron

  int current_indice = 0;

  for (int e = 0; e < le_dom_VF->nb_elem_tot(); e++)
    {
      double invV = 1/ve(e);

      inv_mass_matrix_(current_indice, current_indice) = invV;

      if (order_ == 0) continue;

      inv_mass_matrix_(current_indice, current_indice+1) = 0.;
      inv_mass_matrix_(current_indice, current_indice+2) = 0.;

      double sumx2 = mass_matrix_(current_indice+1, current_indice+1);
      double sumy2 = mass_matrix_(current_indice+2, current_indice+2);
      double sumxy = mass_matrix_(current_indice+1, current_indice+2);

      double inv_det = 1./(sumy2*sumx2 - sumxy*sumxy);

      inv_mass_matrix_(current_indice+1, current_indice+1) = sumy2*inv_det;
      inv_mass_matrix_(current_indice+2, current_indice+2) = sumx2*inv_det;

      inv_mass_matrix_(current_indice+1, current_indice+2) = -sumxy*inv_det;
      inv_mass_matrix_(current_indice+2, current_indice+1) = -sumxy*inv_det;

      current_indice+=nb_bfunc_;
    }
}

int Champ_Elem_DG::imprime(Sortie& os, int ncomp) const
{
  const Domaine_dis_base& domaine_dis = domaine_dis_base();
  const Domaine& domaine = domaine_dis.domaine();
  const DoubleTab& coord=domaine.coord_sommets();
  const int nb_som = domaine.nb_som();
  const DoubleTab& val = valeurs();
  int som;
  os << nb_som << finl;
  for (som=0; som<nb_som; som++)
    {
      if (dimension==3)
        os << coord(som,0) << " " << coord(som,1) << " " << coord(som,2) << " " ;
      if (dimension==2)
        os << coord(som,0) << " " << coord(som,1) << " " ;
      if (nb_compo_ == 1)
        os << val(som) << finl;
      else
        os << val(som,ncomp) << finl;
    }
  os << finl;
  Cout << "Champ_Elem_DG::imprime FIN >>>>>>>>>> " << finl;
  return 1;
}

int Champ_Elem_DG::fixer_nb_valeurs_nodales(int n)
{

  creer_tableau_distribue(domaine_dis_base().domaine().md_vector_elements()); // TODO 26/08/2024 nb_ddl a

  return n;
}



void Champ_Elem_DG::associer_domaine_dis_base(const Domaine_dis_base& z_dis)
{
  le_dom_VF = ref_cast(Domaine_VF, z_dis);

  order_ = Option_DG::Get_order_for(nom_);// Todo regler la relation ordre inconnu/quadrature avec dictionnaire ?
  nb_bfunc_ = Option_DG::Nb_col_from_order(order_);

  allocate_mass_matrix();
  build_mass_matrix();

  build_inv_mass_matrix();

}

void Champ_Elem_DG::eval_bfunc(const Quadrature_base& quad, const int& nelem, DoubleTab& fbasis) const
{
  const DoubleTab& integ_points = quad.get_integ_points();

  assert(fbasis.dimension(0) == nb_bfunc_ && fbasis.dimension(1) == quad.nb_pts_integ_max());


  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());
  const DoubleTab& xp = domaine.xp(); // barycentre elem

  if (Objet_U::dimension == 2)
    {
      double invh = 1./sqrt(domaine.carre_pas_maille(nelem));

      for (int pt = 0; pt < quad.nb_pts_integ(nelem); pt++)
        {
          fbasis(0, pt) = 1;
          if (order_ == 0) continue;

          fbasis(1, pt) = (integ_points(quad.ind_pts_integ(nelem) + pt, 0) - xp(nelem,0))*invh;
          fbasis(2, pt) = (integ_points(quad.ind_pts_integ(nelem) + pt, 1) - xp(nelem,1))*invh;

          if (order_ == 1) continue;
          throw;
        }
    }
  else if (Objet_U::dimension == 3)
    throw; //TODO
  else
    Process::exit();
}

void Champ_Elem_DG::eval_bfunc(const DoubleTab& coords, const int& nelem, DoubleTab& fbasis) const
{

  assert(fbasis.dimension(0) == nb_bfunc_);

  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());
  const DoubleTab& xp = domaine.xp(); // barycentre elem

  const Quadrature_base& quad = domaine.get_quadrature(2);
  int nb_points = quad.nb_pts_integ(nelem);

  if (Objet_U::dimension == 2)
    {
      double invh = 1./sqrt(domaine.carre_pas_maille(nelem));

      for (int pt = 0; pt < nb_points; pt++)
        {
          fbasis(0, pt) = 1;
          if (order_ == 0) continue;

          fbasis(1, pt) = (coords(pt, 0) - xp(nelem,0))*invh;
          fbasis(2, pt) = (coords(pt, 1) - xp(nelem,1))*invh;

          if (order_ == 1) continue;
          throw;
        }
    }
  else if (Objet_U::dimension == 3)
    throw; //TODO
  else
    Process::exit();
}

void Champ_Elem_DG::eval_grad_bfunc(const Quadrature_base& quad, const int& nelem, DoubleTab& grad_fbasis) const
{
//  const DoubleTab& integ_points_on_facets = quad.get_integ_points_facets();
  assert(grad_fbasis.dimension(0) == nb_bfunc_ && grad_fbasis.dimension(1) == quad.nb_pts_integ_max() && grad_fbasis.dimension(2) == Objet_U::dimension);

  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());

  if (Objet_U::dimension == 2)
    {
      double invh = 1./sqrt(domaine.carre_pas_maille(nelem));

      for (int pt = 0; pt < quad.nb_pts_integ(nelem); pt++)
        {

          grad_fbasis(1, pt, 0) = invh;
//          grad_fbasis(1, pt, 1) = 0;
//          grad_fbasis(2, pt, 0) = 0;
          grad_fbasis(2, pt, 1) = invh;

          if (order_ == 1) continue;
          throw;
        }
    }
  else if (Objet_U::dimension == 3)
    throw; //TODO
  else
    Process::exit();

}


void Champ_Elem_DG::eval_bfunc_on_facets(const Quadrature_base& quad, const int& nelem, const int& num_face, DoubleTab& fbasis) const
{
  const DoubleTab& integ_points_on_facets = quad.get_integ_points_facets();
  int nb_pts_integ_on_facets = quad.nb_pts_integ_facets();

  assert(fbasis.dimension(0) == nb_bfunc_ && fbasis.dimension(1) == nb_pts_integ_on_facets);

  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());
  const DoubleTab& xp = domaine.xp(); // barycentre elem

  if (Objet_U::dimension == 2)
    {
      double invh = 1./sqrt(domaine.carre_pas_maille(nelem));

      for (int pt = 0; pt < nb_pts_integ_on_facets; pt++)
        {
          fbasis(0, pt) = 1;
          if (order_ == 0) continue;

          fbasis(1, pt) = (integ_points_on_facets(num_face, pt, 0) - xp(nelem,0))*invh;
          fbasis(2, pt) = (integ_points_on_facets(num_face, pt, 1) - xp(nelem,1))*invh;

          if (order_ == 1) continue;
          throw;
        }
    }
  else if (Objet_U::dimension == 3)
    throw; //TODO
  else
    Process::exit();
}

void Champ_Elem_DG::eval_grad_bfunc_on_facets(const Quadrature_base& quad, const int& nelem, const int& num_face, DoubleTab& grad_fbasis) const
{
//  const DoubleTab& integ_points_on_facets = quad.get_integ_points_facets();
// Q : Arrive t-on ici quand l'ordre est a 0 ?  A.Peita
  int nb_pts_integ_on_facets = quad.nb_pts_integ_facets();

  assert(grad_fbasis.dimension(0) == nb_bfunc_ && grad_fbasis.dimension(1) == nb_pts_integ_on_facets && grad_fbasis.dimension(2) == Objet_U::dimension );


  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());

  if (Objet_U::dimension == 2)
    {
      double invh = 1./sqrt(domaine.carre_pas_maille(nelem));

      for (int pt = 0; pt < nb_pts_integ_on_facets; pt++)
        {

          grad_fbasis(1, pt, 0) = invh;
//          grad_fbasis(1, pt, 1) = 0.;
//          grad_fbasis(2, pt, 0) = 0.;
          grad_fbasis(2, pt, 1) = invh;

          if (order_ == 1) continue;
          throw;
        }
    }
  else if (Objet_U::dimension == 3)
    throw; //TODO
  else
    Process::exit();
}


const Matrice_Dense Champ_Elem_DG::eval_invMassMatrix(const Quadrature_base& quad, const int& nelem) const
{

  // TODo DG adapt to high order quadrature ? possible ? or inverse mass matrix with specific quadrature ?

  const DoubleTab& weights = quad.get_weights();
  int nb_pts_integ_max = quad.nb_pts_integ_max();

  Matrice_Dense matrice(nb_bfunc_, nb_bfunc_);

  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());
  const DoubleVect& ve = domaine.volumes();

  DoubleTab fbase(nb_bfunc_, nb_pts_integ_max);

  eval_bfunc(quad, nelem, fbase);

  if (Objet_U::dimension == 2)
    {
      double invV = 1./ve(nelem);
      matrice(0, 0) = invV;

      matrice(0, 1) = 0.;
      matrice(0, 2) = 0.;

      matrice(1, 0) = 0.;
      matrice(2, 0) = 0.;

      double sumx2 = 0.;
      double sumy2 = 0.;
      double sumxy = 0.;

      for (int f = 0; f < quad.nb_pts_integ(nelem); f++)
        {
          sumx2 += weights(quad.ind_pts_integ(nelem)+f)*ve(nelem)*fbase(1,f)*fbase(1,f);
          sumxy += weights(quad.ind_pts_integ(nelem)+f)*ve(nelem)*fbase(1,f)*fbase(2,f);;
          sumy2 += weights(quad.ind_pts_integ(nelem)+f)*ve(nelem)*fbase(2,f)*fbase(2,f);
        }

      double inv_det = 1./(sumy2*sumx2 - sumxy*sumxy);

      matrice(1, 1) = sumy2*inv_det;
      matrice(2, 2) = sumx2*inv_det;

      matrice(1, 2) = -sumxy*inv_det;
      matrice(2, 1) = -sumxy*inv_det;
    }
  else if (Objet_U::dimension == 3)
    throw; //TODO
  else
    Process::exit();

  return matrice;
}



Champ_base& Champ_Elem_DG::affecter_(const Champ_base& ch)
{
  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());

  const Quadrature_base& quad = domaine.get_quadrature(2); // if Uniforme remplissage automatique
  //sinon interdit d'avoir 1

  //creation d'un DoubleTab intermediaire pour recuperer les valeurs du champ ch sur les points de quadrature ?
  const DoubleTab& integ_points = quad.get_integ_points();
  int nb_pts_integ_max = quad.nb_pts_integ_max();

  int nb_elem = domaine.nb_elem();

  DoubleTab product(nb_pts_integ_max);
  int nb_pts_integ = integ_points.dimension(0);
  DoubleTab values(nb_pts_integ,1);
  DoubleTab phi_rhs(nb_bfunc_);
  DoubleTab res;

  DoubleTab fbase(nb_bfunc_, nb_pts_integ_max);

  ch.valeur_aux(integ_points, values);

  for (int num_elem = 0; num_elem < nb_elem; num_elem++)
    {
      eval_bfunc(quad, num_elem, fbase);

      for (int fb = 0; fb < nb_bfunc_; fb++)
        {
          for (int k = 0; k < quad.nb_pts_integ(num_elem) ; k++)
            product(k) = values(quad.ind_pts_integ(num_elem) + k,0) * fbase(fb, k);

          phi_rhs(fb) = quad.compute_integral_on_elem(num_elem, product);
        }

      Matrice_Dense invM = eval_invMassMatrix(quad, num_elem); //to remove and ref on global matrix

      res.ref_tab(valeurs(), num_elem, 1);
      invM.ajouter_multvect_(phi_rhs, res);
    }

  // computation of the stabilization parameters
  compute_stab_param();
  valeurs().echange_espace_virtuel();
  return *this;
}

DoubleTab& Champ_Elem_DG::valeur_aux(const DoubleTab& positions, DoubleTab& tab_valeurs) const
{
  const Domaine_DG& domaine_DG = ref_cast(Domaine_DG,le_dom_VF.valeur());
  const Domaine& domaine = domaine_dis_base().domaine();

  const int dim = positions.dimension(1);

  const Quadrature_base& quad = domaine_DG.get_quadrature(2);
  int nb_pts_integ_max = quad.nb_pts_integ_max();

  IntVect les_polys;
  les_polys.resize(tab_valeurs.dimension(0), RESIZE_OPTIONS::NOCOPY_NOINIT);

  domaine.chercher_elements(positions, les_polys); //TODO DG selectionner uniquement la premiere valeur de tab_valeurs

  const Champ_base& ch_base = le_champ();
  const DoubleTab& values = ch_base.valeurs();
  int nb_polys = les_polys.size();


  if (nb_polys == 0)
    return tab_valeurs;

  DoubleTab fbase(nb_bfunc_,nb_pts_integ_max);
  DoubleTab coords(nb_pts_integ_max,dim);
  for (int i = 0; i < nb_polys; i++)
    {
      int cell = les_polys(i);
      assert(cell < values.dimension_tot(0));

      if (cell != -1)
        {
          for (int j = 0; j < quad.nb_pts_integ(cell); j++)
            for (int k = 0; k<dim; k++)
              coords(j,k) = positions(quad.ind_pts_integ(cell) +j, k);
//          coords.ref_tab(positions, i*nb_points, nb_points); //pas possible car const

          eval_bfunc(coords, cell, fbase);

          for (int j = 0; j < quad.nb_pts_integ(cell) ; j++)
            {
              tab_valeurs(i,j) = 0.;
              for (int l =0; l<nb_bfunc_; l++)
                tab_valeurs(i,j) += values(cell,l) * fbase(l,j); // reconstruction valeurs du champ aux points d'integrations
            }
        }
    }

  return tab_valeurs;
}

DoubleTab& Champ_Elem_DG::eval_elem(DoubleTab& tab_valeurs) const
{
  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());

  const int nb_elem = domaine.nb_elem();

  const Quadrature_base& quad = domaine.get_quadrature(2);
  int nb_pts_integ_max = quad.nb_pts_integ_max();

  const Champ_base& ch_base = le_champ();
  const DoubleTab& values = ch_base.valeurs();


  assert(tab_valeurs.dimension(0) == nb_elem && tab_valeurs.dimension(1) == nb_pts_integ_max );

  DoubleTab fbase(nb_bfunc_,nb_pts_integ_max);
  for (int i = 0; i < nb_elem; i++)
    {
      eval_bfunc(quad, i, fbase);

      for (int j = 0; j < quad.nb_pts_integ(i) ; j++)
        {
          tab_valeurs(i,j) = 0.;
          for (int l =0; l<nb_bfunc_; l++)
            tab_valeurs(i,j) += values(i,l) * fbase(l,j); // reconstruction valeurs du champ aux points d'integrations
        }
    }

  return tab_valeurs;
}

DoubleTab& Champ_Elem_DG::valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const
{

  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());

  const DoubleVect& volume = domaine.volumes();

  const Quadrature_base& quad = domaine.get_quadrature(2);
  int nb_pts_integ_max = quad.nb_pts_integ_max();

  const Champ_base& ch_base = le_champ();
  const DoubleTab& values = ch_base.valeurs();
  int nb_polys = polys.size();

  if (nb_polys == 0)
    return result;

  // TODO : FIXME
  // For FT the resize should be done in its good position and not here ...
  if (result.nb_dim() == 1) result.resize(nb_polys, 1);

  assert(result.line_size() == 1);
  ToDo_Kokkos("critical");

  DoubleTab fbase(nb_bfunc_, nb_pts_integ_max);
  DoubleTab product(nb_pts_integ_max);

  for (int i = 0; i < nb_polys; i++)
    {
      int cell = polys(i);
      assert(cell < values.dimension_tot(0));

      if (cell != -1)
        {
          eval_bfunc(quad, cell, fbase);

          product = 0.;
          for (int k = 0; k < quad.nb_pts_integ(cell) ; k++)
            for (int l =0; l<nb_bfunc_; l++)
              product(k) += values(cell,l) * fbase(l,k); // reconstruction valeurs du champ aux points coords

          result(i,0) = quad.compute_integral_on_elem(cell, product);
          result(i,0) /= volume(cell);
        }
    }

  return result;

}

void Champ_Elem_DG::compute_stab_param()
{
  const Domaine_DG& domaine = ref_cast(Domaine_DG,le_dom_VF.valeur());
  int nb_elem = domaine.nb_elem();
  eta_elem.resize(nb_elem);
  eta_facet.resize(domaine.nb_faces());
  const IntTab& type_elem = domaine.get_type_elem();  // IntTab that indicate the number of facet of each elem

  //          Computation of the stabilisation parameters for triangle
  const DoubleTab& sig=domaine.get_sig();
  for (int e = 0; e < domaine.nb_elem(); e++)
    {
      double ordre = get_order();
      if(type_elem(e)==3)
        {
          eta_elem(e) = 6. / M_PI * (sig(e)*sig(e))*(ordre + 1.)*(ordre + 2.);
        }
      else
        {
          eta_elem(e) = 10*ordre*ordre; //  TODO: calculate a more optimized pen. coeff.
        }
    }

  for (unsigned int f = 0; f < (unsigned int) domaine.premiere_face_int(); f++) // For the boundary
    {
      int elem0 = domaine.face_voisins(f, 0);
      eta_facet(f) = eta_elem(elem0); // EtaF=EtaT
    }

  for (unsigned int f = domaine.premiere_face_int(); f < (unsigned int) domaine.nb_faces(); f++)
    {
      unsigned int elem0 = domaine.face_voisins(f, 0);
      unsigned int elem1 = domaine.face_voisins(f, 1);
      double eta_t0 = eta_elem(elem0);
      double eta_t1 = eta_elem(elem1);
      eta_facet(f) = 2. * eta_t0 * eta_t1 / (eta_t0 + eta_t1); // Harmonique mean
    }

  /*for (int e = 0; e < domaine.nb_elem(); e++)
    {
      eta_elem(e) = 50.;     // Todo DG : replace for generic order
    }
  for (unsigned int f = 0; f < (unsigned int) domaine.nb_faces(); f++) // For the boundary
    {
      eta_facet(f) = 10.; // EtaF=EtaT
    }*/
}

