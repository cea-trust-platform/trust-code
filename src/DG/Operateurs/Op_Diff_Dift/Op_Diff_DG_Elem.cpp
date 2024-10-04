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

#include <Modele_turbulence_scal_base.h>
#include <Echange_externe_impose.h>
#include <Op_Diff_DG_Elem.h>
#include <Dirichlet_homogene.h>
#include <Domaine_Cl_DG.h>
#include <Champ_Elem_DG.h>
#include <Schema_Temps_base.h>
#include <Champ_front_calc.h>
#include <Domaine_DG.h>
#include <communications.h>
#include <Synonyme_info.h>
#include <Probleme_base.h>
#include <Neumann_paroi.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <cmath>
#include <Quadrature_base.h>

Implemente_instanciable( Op_Diff_DG_Elem , "Op_Diff_DG_Elem|Op_Diff_DG_var_Elem" , Op_Diff_DG_base );

Sortie& Op_Diff_DG_Elem::printOn(Sortie& os) const { return Op_Diff_DG_base::printOn(os); }

Entree& Op_Diff_DG_Elem::readOn(Entree& is) { return Op_Diff_DG_base::readOn(is); }


void Op_Diff_DG_Elem::completer()
{
  Op_Diff_DG_base::completer();
  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue().valeur());
  const Domaine_DG& domaine = le_dom_dg_.valeur();
  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_DG_Elem : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  ch.fcl();
  int nb_comp = (equation().que_suis_je() == "Transport_K_Eps") ? 2 : ch.valeurs().line_size();
  flux_bords_.resize(domaine.premiere_face_int(), nb_comp);

  if (!que_suis_je().debute_par("Op_Dift"))
    return;

  throw;
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Modele_turbulence_scal_base& mod_turb = ref_cast(Modele_turbulence_scal_base, modele_turbulence.valeur());
  const Champ_Fonc& lambda_t = mod_turb.conductivite_turbulente();
  associer_diffusivite_turbulente(lambda_t);
}


void Op_Diff_DG_Elem::dimensionner(Matrice_Morse& mat) const
{
  const Domaine_DG& domaine = le_dom_dg_.valeur();

  IntTab indice(0, 2);

  const IntTab& face_voisins = domaine.face_voisins();

  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue().valeur());
  int nordre = ch.get_order();
  int nddl = Option_DG::Nb_col_from_order(nordre);

  const IntTab& indices_glob_elem = ch.indices_glob_elem();

  int premiere_face_int = domaine.premiere_face_int();

  int elem0, elem1;
  for (int f = premiere_face_int; f < domaine.nb_faces(); f++)
    {
      elem0 = face_voisins(f,0);
      elem1 = face_voisins(f,1);

      int ind_elem0 = indices_glob_elem(elem0);
      int ind_elem1 = indices_glob_elem(elem1);

      for( int i_elem = 0; i_elem<2; i_elem++)
        {
          int elem=face_voisins(f,i_elem);
          int ind_elem=indices_glob_elem(elem);

          for (int i = 0; i < nddl; i++ )
            for (int j = 0; j < nddl; j++ )
              indice.append_line( ind_elem+i, ind_elem+j);
        }

      for (int i = 0; i < nddl; i++ )
        for (int j = 0; j < nddl; j++ )
          {
            indice.append_line( ind_elem0+i, ind_elem1+j);
            indice.append_line( ind_elem1+i, ind_elem0+j);
          }
    }

  tableau_trier_retirer_doublons(indice);

  int nb_elem_tot = le_dom_dg_->nb_elem_tot();

  int size_inc = indices_glob_elem(nb_elem_tot);

  Matrix_tools::allocate_morse_matrix(size_inc, size_inc, indice, mat);
}


DoubleTab& Op_Diff_DG_Elem::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  // TODO:
  Matrice_Morse matrice;

  dimensionner(matrice);
  contribuer_a_avec(inco, matrice);

  DoubleTab coeffs;
  coeffs.copy(resu, RESIZE_OPTIONS::NOCOPY_NOINIT);

  matrice.ajouter_multvect(inco, coeffs);

  resu -= coeffs;

  return resu;
}

void Op_Diff_DG_Elem::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  double eta_F=1; // TODO: Compute the penalisation coefficient
  // TODO : Ne pas multiplier les intégration par F sinon c'est pas homogène
  update_nu();

  const Domaine_DG& domaine = le_dom_dg_.valeur();
  const IntTab& face_voisins = domaine.face_voisins();

  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue().valeur());

  const Quadrature_base& quad = domaine.get_quadrature(2);
  const IntTab& indices_glob_elem = ch.indices_glob_elem();
  int nb_pts_integ = quad.nb_pts_integ();


  const int nb_bfunc = ch.nb_bfunc();
  DoubleTab grad_fbase_elem(nb_bfunc,nb_pts_integ, Objet_U::dimension);
  DoubleTab divergence(nb_pts_integ);

  for (int e = 0; e < le_dom_dg_->nb_elem(); e++)
    {
      ch.eval_grad_bfunc(quad, e, grad_fbase_elem);
      int ind_elem=indices_glob_elem(e);

      double nu = nu_(e);

      for (int i=0; i<nb_bfunc; i++)
        for (int j=0; j<nb_bfunc; j++)
          {
            divergence = 0.;
            for (int k = 0; k < nb_pts_integ ; k++)
              for (int d=0; d<Objet_U::dimension; d++)
                divergence(k) += grad_fbase_elem(i,k,d) * grad_fbase_elem(j,k,d);

            matrice(ind_elem+i, ind_elem+j) += nu * quad.compute_integral_on_elem(e, divergence);
          }
    }

  int nb_pts_int_fac = quad.nb_pts_integ_facets();
  int premiere_face_int = domaine.premiere_face_int();
  const DoubleTab& face_normales = domaine.face_normales();

  int elem0, elem1;

  DoubleTab product(nb_pts_int_fac);
  DoubleTab scalar_product(nb_pts_int_fac);

  DoubleTab fbase0(nb_bfunc, nb_pts_int_fac);
  DoubleTab fbase1(nb_bfunc, nb_pts_int_fac);

  DoubleTab grad_fbase0(nb_bfunc,nb_pts_int_fac, Objet_U::dimension);
  DoubleTab grad_fbase1(nb_bfunc,nb_pts_int_fac, Objet_U::dimension);


  for (int f = premiere_face_int; f < domaine.nb_faces(); f++)
    {

      elem0 = face_voisins(f,0);
      elem1 = face_voisins(f,1);

      int ind_elem0 = indices_glob_elem(elem0);
      int ind_elem1 = indices_glob_elem(elem1);

      double sur_f = domaine.face_surfaces(f);

      double h_T = sqrt(std::min(domaine.carre_pas_maille(elem0), domaine.carre_pas_maille(elem1)));
      double invh_T = 1./h_T;
      double nu = 2*nu_(elem0)*nu_(elem1)/(nu_(elem0) + nu_(elem1));

      //*****************//
      // penalizing term //
      //*****************//
      for( int i_elem = 0; i_elem<2; i_elem++)
        {
          int elem=face_voisins(f,i_elem);
          int ind_elem=indices_glob_elem(elem);

          ch.eval_bfunc_on_facets(quad, elem, f, fbase0);

          for (int i=0; i<nb_bfunc; i++)
            for (int j=0; j<nb_bfunc; j++)
              {
                for (int k = 0; k < nb_pts_int_fac ; k++)
                  product(k) = fbase0(i,k) * fbase0(j,k);

                matrice(ind_elem+i, ind_elem+j) += nu * eta_F / sur_f* invh_T* quad.compute_integral_on_facet(f, product);
              }
        }


      //crossed_term
      ch.eval_bfunc_on_facets(quad, elem0, f, fbase0);
      ch.eval_bfunc_on_facets(quad, elem1, f, fbase1);

      for (int i=0; i<nb_bfunc; i++)
        for (int j=0; j<nb_bfunc; j++)
          {
            for (int k = 0; k < nb_pts_int_fac ; k++)
              product(k) = fbase0(i,k) * fbase1(j,k);

            double integral = quad.compute_integral_on_facet(f, product);
            matrice(ind_elem0+i, ind_elem1+j) -= nu * eta_F/ sur_f* invh_T *integral;
            matrice(ind_elem1+j, ind_elem0+i) -= nu * eta_F/ sur_f* invh_T *integral; //symmetry
          }

      //****************//
      // symmetric term //
      //****************//
      ch.eval_grad_bfunc_on_facets(quad, elem0, f, grad_fbase0);
      ch.eval_grad_bfunc_on_facets(quad, elem1, f, grad_fbase1);

      double nu0 = nu_(elem0);
      double nu1 = nu_(elem1);

      for (int i=0; i<nb_bfunc; i++)
        {
          scalar_product = 0.;
          for (int k = 0; k < nb_pts_int_fac ; k++)
            for (int d=0; d<Objet_U::dimension; d++)
              scalar_product(k) += face_normales(f,d)/sur_f * grad_fbase0(i, k, d);


          for (int j=0; j<nb_bfunc; j++)
            {
              for (int k = 0; k < nb_pts_int_fac ; k++)
                product(k) = scalar_product(k) * fbase0(j,k);
              double integral = quad.compute_integral_on_facet(f, product);
              matrice(ind_elem0+i, ind_elem0+j) -= 0.5 *nu0*integral;
              matrice(ind_elem0+j, ind_elem0+i) -= 0.5 *nu0*integral; //symmetry
            }


          for (int j=0; j<nb_bfunc; j++)
            {
              for (int k = 0; k < nb_pts_int_fac ; k++)
                product(k) = scalar_product(k) * fbase1(j,k);
              double integral = quad.compute_integral_on_facet(f, product);
              matrice(ind_elem0+i, ind_elem1+j) += 0.5 *nu0*integral;
              matrice(ind_elem1+j, ind_elem0+i) += 0.5 *nu1*integral; //symmetry
            }

          scalar_product = 0.;
          for (int k = 0; k < nb_pts_int_fac ; k++)
            for (int d=0; d<Objet_U::dimension; d++)
              scalar_product(k) += face_normales(f,d)/sur_f * grad_fbase1(i, k, d);

          for (int j=0; j<nb_bfunc; j++)
            {
              for (int k = 0; k < nb_pts_int_fac ; k++)
                product(k) = scalar_product(k) * fbase1(j,k);
              double integral = quad.compute_integral_on_facet(f, product);
              matrice(ind_elem1+i, ind_elem1+j) += 0.5 *nu1*integral;
              matrice(ind_elem1+j, ind_elem1+i) += 0.5 *nu1*integral; //symmetry
            }

          for (int j=0; j<nb_bfunc; j++)
            {
              for (int k = 0; k < nb_pts_int_fac ; k++)
                product(k) = scalar_product(k) * fbase0(j,k);
              double integral = quad.compute_integral_on_facet(f, product);
              matrice(ind_elem1+i, ind_elem0+j) -= 0.5 *nu1*integral;
              matrice(ind_elem0+j, ind_elem1+i) -= 0.5 *nu0*integral; //symmetry
            }
        }
    }


  /* Treatment of the boundary conditions */

  for (int f = 0; f < premiere_face_int; f++) // For the boundary
    {

      if ((ch.fcl()(f, 0)==6)||(ch.fcl()(f, 0)==7))
        {
          int elem = face_voisins(f, 0); // The cell that have one facet on the boundary
          int ind_elem = indices_glob_elem(elem);

          ch.eval_bfunc_on_facets(quad, elem, f, fbase0);
          ch.eval_grad_bfunc_on_facets(quad, elem, f, grad_fbase0);

          double h_T = sqrt(domaine.carre_pas_maille(elem));
          double invh_T = 1./h_T;
          double nu = nu_(elem);

          double sur_f = domaine.face_surfaces(f);

          for (int i=0; i<nb_bfunc; i++)
            {
              scalar_product = 0.;
              for (int k = 0; k < nb_pts_int_fac ; k++)
                for (int d=0; d<Objet_U::dimension; d++)
                  scalar_product(k) += face_normales(f,d)/sur_f * grad_fbase0(i, k, d);

              for (int j=0; j<nb_bfunc; j++)
                {
                  for (int k = 0; k < nb_pts_int_fac ; k++)
                    product(k) = fbase0(i, k) * fbase0(j, k);

                  matrice(ind_elem+i, ind_elem+j) += nu*eta_F/sur_f * invh_T * quad.compute_integral_on_facet(f, product);

                  for (int k = 0; k < nb_pts_int_fac ; k++)
                    product(k) = scalar_product(k) * fbase0(j, k);

                  double integral = quad.compute_integral_on_facet(f, product);
                  matrice(ind_elem+i, ind_elem+j) -= nu*integral;
                  matrice(ind_elem+j, ind_elem+i) -= nu*integral;
                }
            }
        }
    }
}

void Op_Diff_DG_Elem::contribuer_a_avec_(const DoubleTab& inco, Matrice_Morse& matrice) const
{

  const Domaine_DG& domaine = le_dom_dg_.valeur();

  const IntTab& face_voisins = domaine.face_voisins();
  const DoubleTab& xp = domaine.xp();
  const DoubleTab& xv = domaine.xv();

  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue().valeur());

  const Quadrature_base& quad = domaine.get_quadrature(2);
  const IntTab& indices_glob_elem = ch.indices_glob_elem();
  int nordre = ch.get_order();

  int premiere_face_int = domaine.premiere_face_int();

  const DoubleTab& integ_points_facets = quad.get_integ_points_facets();
  int nb_pts_int_fac = integ_points_facets.dimension(1);
  DoubleTab val_pts_integ(nb_pts_int_fac);
  double integral;
  const DoubleTab& face_normales = domaine.face_normales();

  int elem0, elem1;
  for (int f = premiere_face_int; f < domaine.nb_faces(); f++)
    {
      elem0 = face_voisins(f,0);
      elem1 = face_voisins(f,1);

      int ind_elem0 = indices_glob_elem(elem0);
      int ind_elem1 = indices_glob_elem(elem1);

      double sur_f = domaine.face_surfaces(f);

      double invh0 = sqrt(1/domaine.carre_pas_maille(elem0));
      double invh1 = sqrt(1/domaine.carre_pas_maille(elem1));
      double invh[2]= {invh0, invh1};
      double invh_carr[2]= {invh0*invh0, invh1*invh1} ;
      double invh12=invh0*invh1;


      double eta_F=0; // TODO: Compute the penalisation coefficient

      if (Objet_U::dimension == 2)
        {
          // Mass matrix for the facets : TODO: Use the symmetry to compute just the upper side of the matrix
          for( int i_elem = 0; i_elem<2; i_elem++)
            {
              int elem=face_voisins(f,i_elem);
              int ind_elem=indices_glob_elem(elem);

              matrice(ind_elem, ind_elem) += eta_F; // TODO : Ne pas multiplier les intégration par F sinon c'est pas homogène

              if (nordre == 0) continue;

              matrice(ind_elem, ind_elem+1) += eta_F*1*invh[i_elem]*(xv(f,0) - xp(elem,0));
              matrice(ind_elem, ind_elem+2) += eta_F*1*invh[i_elem]*(xv(f,1) - xp(elem,1));

              matrice(ind_elem+1, ind_elem) += matrice(ind_elem, ind_elem+1);
              matrice(ind_elem+2, ind_elem) += matrice(ind_elem, ind_elem+2);


              // x x
              for (int pts=0; pts<nb_pts_int_fac; pts++ )
                val_pts_integ(pts)=(1./sur_f)*invh_carr[i_elem]*(integ_points_facets(f,pts,0)-xp(elem,0))*(integ_points_facets(f,pts,0)-xp(elem,0));
              integral = quad.compute_integral_on_facet(f, val_pts_integ);
              matrice(ind_elem+1, ind_elem+1) += eta_F*integral;
              // y y
              for (int pts=0; pts<nb_pts_int_fac; pts++ )
                val_pts_integ(pts)=(1./sur_f)*invh_carr[i_elem]*(integ_points_facets(f,pts,1)-xp(elem,1))*(integ_points_facets(f,pts,1)-xp(elem,1));
              integral = quad.compute_integral_on_facet(f, val_pts_integ);
              matrice(ind_elem+2, ind_elem+2) += eta_F*integral;
              // x y
              for (int pts=0; pts<nb_pts_int_fac; pts++ )
                val_pts_integ(pts)=(1./sur_f)*invh_carr[i_elem]*(integ_points_facets(f,pts,0)-xp(elem,0))*(integ_points_facets(f,pts,1)-xp(elem,1));
              integral = quad.compute_integral_on_facet(f, val_pts_integ);
              matrice(ind_elem+1, ind_elem+2) += eta_F*integral;
              matrice(ind_elem+2, ind_elem+1) += eta_F*integral;

              if (nordre == 1)
                continue;
            }

          // Cross terms
          matrice(ind_elem0, ind_elem1) -= eta_F*1;
          matrice(ind_elem1, ind_elem0) = matrice(ind_elem0, ind_elem1);

          if (nordre == 0) continue;

          matrice(ind_elem0, ind_elem1+1) -= eta_F* 1*invh1*(xv(f,0) - xp(elem1,0)) ; // 1,x1
          matrice(ind_elem0, ind_elem1+2) -= eta_F* 1*invh1*(xv(f,1) - xp(elem1,1)); // 1,y1
          matrice(ind_elem1+1, ind_elem0) = matrice(ind_elem0, ind_elem1+1); // sym
          matrice(ind_elem1+2, ind_elem0) = matrice(ind_elem0, ind_elem1+2); // sym

          matrice(ind_elem1, ind_elem0+1) -= eta_F* 1*invh0*(xv(f,0) - xp(elem0,0)) ; // 1,x0
          matrice(ind_elem1, ind_elem0+2) -= eta_F* 1*invh0*(xv(f,1) - xp(elem0,1)); // 1,y0
          matrice(ind_elem0+1, ind_elem1) = matrice(ind_elem1, ind_elem1+1); // sym
          matrice(ind_elem0+2, ind_elem1) = matrice(ind_elem1, ind_elem1+2); //sym

          for (int pts=0; pts<nb_pts_int_fac; pts++ )
            val_pts_integ(pts)=(1./sur_f)*invh12*(integ_points_facets(f,pts,0)-xp(elem0,0))*(integ_points_facets(f,pts,0)-xp(elem1,0));
          integral = quad.compute_integral_on_facet(f, val_pts_integ);
          matrice(ind_elem0+1, ind_elem1+1) -= eta_F*integral; // x0,x1
          matrice(ind_elem1+1, ind_elem0+1) -= eta_F*integral;//sym

          for (int pts=0; pts<nb_pts_int_fac; pts++ )
            val_pts_integ(pts)=(1./sur_f)*invh12*(integ_points_facets(f,pts,0)-xp(elem0,0))*(integ_points_facets(f,pts,1)-xp(elem1,1));
          integral = quad.compute_integral_on_facet(f, val_pts_integ);
          matrice(ind_elem0+1, ind_elem1+2) -= eta_F*integral; //x0,y1
          matrice(ind_elem1+2, ind_elem0+1) -= eta_F*integral;//sym

          for (int pts=0; pts<nb_pts_int_fac; pts++ )
            val_pts_integ(pts)=(1./sur_f)*invh12*(integ_points_facets(f,pts,1)-xp(elem0,1))*(integ_points_facets(f,pts,0)-xp(elem1,0));
          integral = quad.compute_integral_on_facet(f, val_pts_integ);
          matrice(ind_elem0+2, ind_elem1+1) -= eta_F*integral; //y0 x1
          matrice(ind_elem1+1, ind_elem0+2) -= eta_F*integral;//sym

          for (int pts=0; pts<nb_pts_int_fac; pts++ )
            val_pts_integ(pts)=(1./sur_f)*invh12*(integ_points_facets(f,pts,1)-xp(elem0,1))*(integ_points_facets(f,pts,1)-xp(elem1,1));
          integral = quad.compute_integral_on_facet(f, val_pts_integ);
          matrice(ind_elem0+2, ind_elem1+2) -= eta_F*integral; //y0 y1
          matrice(ind_elem1+2, ind_elem0+2) -= eta_F*integral;//sym

          // Stiffness Matrix for facet terms part 1 consistency
          matrice(ind_elem0+1,ind_elem1)+=(+1./2)*1*face_normales(f,0)/sur_f*invh0;
          matrice(ind_elem0+1,ind_elem1+1)+=(+1./2)*1*face_normales(f,0)/sur_f*invh12*(xv(f,0) - xp(elem1,0));
          matrice(ind_elem0+1,ind_elem1+2)+=(+1./2)*1*face_normales(f,0)/sur_f*invh12*(xv(f,1) - xp(elem1,1));
          matrice(ind_elem0+2,ind_elem1+1)+=(+1./2)*1*face_normales(f,1)/sur_f*invh12*(xv(f,0) - xp(elem1,0));
          matrice(ind_elem0+2,ind_elem1+2)+=(+1./2)*1*face_normales(f,1)/sur_f*invh12*(xv(f,1) - xp(elem1,1));
          //sym
          matrice(ind_elem1,ind_elem0+1)=matrice(ind_elem0+1,ind_elem1);
          matrice(ind_elem1+1,ind_elem0+1)=matrice(ind_elem0+1,ind_elem1+1);
          matrice(ind_elem1+2,ind_elem0+1)=matrice(ind_elem0+1,ind_elem1+2);
          matrice(ind_elem1+1,ind_elem0+2)=matrice(ind_elem0+2,ind_elem1+1);
          matrice(ind_elem1+2,ind_elem0+2)=matrice(ind_elem0+2,ind_elem1+2);
          // Stiffness Matrix for facet terms  part 2 sym
          matrice(ind_elem1+1,ind_elem0)+=(-1./2)*1*face_normales(f,0)/sur_f*invh1;
          matrice(ind_elem1+1,ind_elem0+1)+=(-1./2)*1*face_normales(f,0)/sur_f*invh12*(xv(f,0) - xp(elem0,0));
          matrice(ind_elem1+1,ind_elem0+2)+=(-1./2)*1*face_normales(f,0)/sur_f*invh12*(xv(f,1) - xp(elem0,1));
          matrice(ind_elem1+2,ind_elem0+1)+=(-1./2)*1*face_normales(f,1)/sur_f*invh12*(xv(f,0) - xp(elem0,0));
          matrice(ind_elem1+2,ind_elem0+2)+=(-1./2)*1*face_normales(f,1)/sur_f*invh12*(xv(f,1) - xp(elem0,1));

          // symmetry
          matrice(ind_elem0,ind_elem1+1)=matrice(ind_elem1+1,ind_elem0);
          matrice(ind_elem0+1,ind_elem1+1)=matrice(ind_elem1+1,ind_elem0+1);
          matrice(ind_elem0+2,ind_elem1+1)=matrice(ind_elem1+1,ind_elem0+2);
          matrice(ind_elem0+1,ind_elem1+2)=matrice(ind_elem1+2,ind_elem0+1);
          matrice(ind_elem0+2,ind_elem1+2)=matrice(ind_elem1+2,ind_elem0+2);

          Cout << face_normales(f,0) << " " << face_normales(f, 1) << finl;
          Cout << invh0 << " " << invh1 << finl;


          if (nordre == 1)
            continue;

        }

    }


  /* Treatment of the boundary conditions */

  for (int f = 0; f < premiere_face_int; f++) // For the boundary
    {
      int elem = face_voisins(f, 0); // The cell that have one facet on the boundary
      int ind_elem = indices_glob_elem(elem);
      double sur_f = domaine.face_surfaces(f);


      double eta_F=0; // TODO: Compute the penalisation coefficient

      double invh = sqrt(1/domaine.carre_pas_maille(elem));
      double invh_carr = invh*invh;

      double Svec2[3]= {-invh*face_normales(f,0)/sur_f, -invh*face_normales(f,1)/sur_f, 0} ;
      double Svec[3]= {1/2*Svec2[0], 1/2*Svec2[1],0 } ;


      if ((ch.fcl()(f, 0)==6)||(ch.fcl()(f, 0)==7))
        {
          matrice(ind_elem, ind_elem) += eta_F;

          if (nordre == 0)
            continue;
          double inte1x,inte1y,inte_xx,inte_yy,inte_xy;

          inte1x = 1 * invh * (xv(f, 0) - xp(elem, 0));
          inte1y = 1 * invh * (xv(f, 1) - xp(elem, 1));
          matrice(ind_elem, ind_elem + 1) += eta_F * inte1x;
          matrice(ind_elem, ind_elem + 2) += eta_F * inte1y;
          // symmetry
          matrice(ind_elem + 1, ind_elem) +=  eta_F * inte1x;
          matrice(ind_elem + 2, ind_elem) +=  eta_F * inte1y;
          // x x
          for (int pts = 0; pts < nb_pts_int_fac; pts++)
            val_pts_integ(pts) = (1./sur_f)*invh_carr * (integ_points_facets(f, pts, 0) - xp(elem, 0)) * (integ_points_facets(f, pts, 0) - xp(elem, 0));
          inte_xx = quad.compute_integral_on_facet(f, val_pts_integ);
          matrice(ind_elem + 1, ind_elem + 1) += eta_F * inte_xx;
          // y y
          for (int pts = 0; pts < nb_pts_int_fac; pts++)
            val_pts_integ(pts) = (1./sur_f)*invh_carr * (integ_points_facets(f, pts, 1) - xp(elem, 1)) * (integ_points_facets(f, pts, 1) - xp(elem, 1));
          inte_yy = quad.compute_integral_on_facet(f, val_pts_integ);
          matrice(ind_elem + 2, ind_elem + 2) += eta_F * inte_yy;
          // x y
          for (int pts = 0; pts < nb_pts_int_fac; pts++)
            val_pts_integ(pts) = (1./sur_f)*invh_carr * (integ_points_facets(f, pts, 0) - xp(elem, 0)) * (integ_points_facets(f, pts, 1) - xp(elem, 1));
          inte_xy = quad.compute_integral_on_facet(f, val_pts_integ);
          matrice(ind_elem + 1, ind_elem + 2) += eta_F * inte_xy;
          matrice(ind_elem + 2, ind_elem + 1) += eta_F * inte_xy;


          // Stiffness matrix // TODO: faire des boucles sur les dimensions
          matrice(ind_elem, ind_elem + 1) += Svec[0];
          matrice(ind_elem, ind_elem + 2) += Svec[1];
          matrice(ind_elem + 1, ind_elem) += Svec[0];
          matrice(ind_elem + 2, ind_elem) += Svec[1];
          // xx yy
          matrice(ind_elem + 1, ind_elem + 1) += inte1x*Svec2[0];
          matrice(ind_elem + 2, ind_elem + 2) += inte1y*Svec2[1];
          // xy
          matrice(ind_elem + 1, ind_elem + 2) += inte1x*Svec[1]+inte1y*Svec[0];
          matrice(ind_elem + 2, ind_elem + 1) += inte1x*Svec[1]+inte1y*Svec[0];


          if (nordre == 1)
            continue;
        }
    }

  matrice.imprimer(Cout);


// Matrice_Morse sub_part;
// matrice.construire_sous_bloc( 0,  0,  2,  2, sub_part);

// Cout << sub_part << finl;
}

void Op_Diff_DG_Elem::dimensionner_termes_croises(Matrice_Morse& matrice, const Probleme_base& autre_pb, int nl, int nc) const
{
  // TODO pour problemes croises

}

void Op_Diff_DG_Elem::ajouter_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, DoubleTab& resu) const
{
  throw;
  // TODO idem above

}

void Op_Diff_DG_Elem::contribuer_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, Matrice_Morse& matrice) const
{
  // TODO idem above
  throw;

}



void Op_Diff_DG_Elem::contribuer_au_second_membre(DoubleTab& resu ) const
{
  int f=0;
  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue().valeur());
  if ((ch.fcl()(f, 0)==6)||(ch.fcl()(f, 0)==7))
    {
      // Nothing to do
    }
  else
    {
      throw;
      if (ch.fcl()(f, 0)==6) // Non-homogeneous Dirichlet
        throw;
    }
}
