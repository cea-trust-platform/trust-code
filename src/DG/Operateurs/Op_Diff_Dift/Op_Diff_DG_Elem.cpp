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
#include <Champ_front_txyz.h>

Implemente_instanciable( Op_Diff_DG_Elem , "Op_Diff_DG_Elem" , Op_Diff_DG_base );

Sortie& Op_Diff_DG_Elem::printOn(Sortie& os) const { return Op_Diff_DG_base::printOn(os); }

Entree& Op_Diff_DG_Elem::readOn(Entree& is) { return Op_Diff_DG_base::readOn(is); }


void Op_Diff_DG_Elem::completer()
{
  Op_Diff_DG_base::completer();
  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue());
  const Domaine_DG& domaine = le_dom_dg_.valeur();
  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_DG_Elem : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  ch.fcl();
  int nb_comp = (equation().que_suis_je() == "Transport_K_Eps") ? 2 : ch.valeurs().line_size();
  flux_bords_.resize(domaine.premiere_face_int(), nb_comp);

  if (!que_suis_je().debute_par("Op_Dift"))
    return;

  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Modele_turbulence_scal_base& mod_turb = ref_cast(Modele_turbulence_scal_base, modele_turbulence.valeur());
  const Champ_Fonc_base& lambda_t = mod_turb.conductivite_turbulente();
  associer_diffusivite_turbulente(lambda_t);
}


void Op_Diff_DG_Elem::dimensionner(Matrice_Morse& mat) const //TODO a remonter dans Op_DG_Elem
{
  const Domaine_DG& domaine = le_dom_dg_.valeur();

  IntTab indice(0, 2);

  const IntTab& face_voisins = domaine.face_voisins();

  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue());
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

void Op_Diff_DG_Elem::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string nom_inco = equation().inconnue().le_nom().getString();
  if (semi_impl.count(nom_inco)) return; //semi-implicite -> rien a dimensionner

  init_op_ext();  // TODO DG a completer
  int n_ext = (int)op_ext.size(); //pour la thermique monolithique

  std::vector<Matrice_Morse *> mat(n_ext);
  for (int i = 0; i < n_ext; i++)
    {
      std::string nom_mat = i ? nom_inco + "/" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;
      mat[i] = matrices.count(nom_mat) ? matrices.at(nom_mat) : nullptr;
      if(!mat[i]) continue;
      Matrice_Morse mat2;
      if(i==0) dimensionner( mat2);
      else throw; //TODO DG for dimensionner_terme_croises

      mat[i]->nb_colonnes() ? *mat[i] += mat2 : *mat[i] = mat2;
    }
}

void Op_Diff_DG_Elem::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{

  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().valeurs();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;



  update_nu();

  const Domaine_DG& domaine = le_dom_dg_.valeur();
  const IntTab& face_voisins = domaine.face_voisins();

  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue());
  const DoubleTab& eta_F = ch.get_eta_facet();  // Compute the penalisation coefficient
  const Quadrature_base& quad = domaine.get_quadrature(); //TODO a trouver avec Option_DG
  const IntTab& indices_glob_elem = ch.indices_glob_elem();
  int nb_pts_integ_max = quad.nb_pts_integ_max();
  double coeff;


  const int nb_bfunc = ch.nb_bfunc();
  DoubleTab grad_fbase_elem(nb_bfunc,nb_pts_integ_max, Objet_U::dimension);
  DoubleTab divergence(nb_pts_integ_max);

  for (int e = 0; e < le_dom_dg_->nb_elem(); e++)
    {
      ch.eval_grad_bfunc(quad, e, grad_fbase_elem);
      int ind_elem=indices_glob_elem(e);

      double nu = nu_(e);

      for (int i=0; i<nb_bfunc; i++)
        for (int j=0; j<nb_bfunc; j++)
          {
            divergence = 0.;
            for (int k = 0; k < quad.nb_pts_integ(e) ; k++)
              for (int d=0; d<Objet_U::dimension; d++)
                divergence(k) += grad_fbase_elem(i,k,d) * grad_fbase_elem(j,k,d);

            coeff = nu * quad.compute_integral_on_elem(e, divergence);
            if (mat)
              (*mat)(ind_elem+i, ind_elem+j) += coeff;
            secmem(e,i) -= coeff * inco(e,j);
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

      double h_T = sqrt(std::min(domaine.carre_pas_maille(elem0), domaine.carre_pas_maille(elem1))); //TODO possibilite de prendre moyenne harmonique (stabilite)
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

                coeff = nu * eta_F(f)* invh_T* quad.compute_integral_on_facet(f, product);
                if (mat)
                  (*mat)(ind_elem+i, ind_elem+j) += coeff;
                secmem(elem,i) -= coeff * inco(elem,j);
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
            coeff = nu * eta_F(f)* invh_T *integral;
            if (mat)
              {
                (*mat)(ind_elem0+i, ind_elem1+j) -= coeff;
                (*mat)(ind_elem1+j, ind_elem0+i) -= coeff; //symmetry
              }
            secmem(elem0,i) += coeff * inco(elem1,j);
            secmem(elem1,j) += coeff * inco(elem0,i); //symmetry
          }

      //****************//
      // symmetric term //
      //****************//
      ch.eval_grad_bfunc_on_facets(quad, elem0, f, grad_fbase0);
      ch.eval_grad_bfunc_on_facets(quad, elem1, f, grad_fbase1);

//      double nu0 = nu_(elem0);
//      double nu1 = nu_(elem1);

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

              if (mat)
                {
                  (*mat)(ind_elem0+i, ind_elem0+j) -= 0.5 *nu*integral;
                  (*mat)(ind_elem0+j, ind_elem0+i) -= 0.5 *nu*integral; //symmetry
                }
              secmem(elem0,i) += 0.5 *nu*integral*inco(elem0,j);
              secmem(elem0,j) += 0.5 *nu*integral*inco(elem0,i); //symmetry
            }


          for (int j=0; j<nb_bfunc; j++)
            {
              for (int k = 0; k < nb_pts_int_fac ; k++)
                product(k) = scalar_product(k) * fbase1(j,k);
              double integral = quad.compute_integral_on_facet(f, product);
              if (mat)
                {
                  (*mat)(ind_elem0+i, ind_elem1+j) += 0.5 *nu*integral;
                  (*mat)(ind_elem1+j, ind_elem0+i) += 0.5 *nu*integral; //symmetry
                }
              secmem(elem0,i) -= 0.5 *nu*integral*inco(elem1,j);
              secmem(elem1,j) -= 0.5 *nu*integral*inco(elem0,i); //symmetry
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
              if (mat)
                {
                  (*mat)(ind_elem1+i, ind_elem1+j) += 0.5 *nu*integral;
                  (*mat)(ind_elem1+j, ind_elem1+i) += 0.5 *nu*integral; //symmetry
                }
              secmem(elem1,i) -= 0.5 *nu*integral*inco(elem1,j);
              secmem(elem1,j) -= 0.5 *nu*integral*inco(elem1,i); //symmetry
            }

          for (int j=0; j<nb_bfunc; j++)
            {
              for (int k = 0; k < nb_pts_int_fac ; k++)
                product(k) = scalar_product(k) * fbase0(j,k);
              double integral = quad.compute_integral_on_facet(f, product);
              if (mat)
                {
                  (*mat)(ind_elem1+i, ind_elem0+j) -= 0.5 *nu*integral;
                  (*mat)(ind_elem0+j, ind_elem1+i) -= 0.5 *nu*integral; //symmetry
                }
              secmem(elem1,i) += 0.5 *nu*integral*inco(elem0,j);
              secmem(elem0,j) += 0.5 *nu*integral*inco(elem1,i); //symmetry
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
          double invh_T = 1./h_T; //TODO regarder penalisation remplacer h_T par h_F
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

                  coeff = nu*eta_F(f) * invh_T * quad.compute_integral_on_facet(f, product);
                  if (mat)
                    (*mat)(ind_elem+i, ind_elem+j) += coeff;
                  secmem(elem,i) -= coeff*inco(elem,j);

                  for (int k = 0; k < nb_pts_int_fac ; k++)
                    product(k) = scalar_product(k) * fbase0(j, k);

                  double integral = quad.compute_integral_on_facet(f, product);
                  if(mat)
                    {
                      (*mat)(ind_elem+i, ind_elem+j) -= nu*integral;
                      (*mat)(ind_elem+j, ind_elem+i) -= nu*integral;
                    }
                  secmem(elem,i) += nu*integral*inco(elem,j);
                  secmem(elem,j) += nu*integral*inco(elem,i);
                }
            }
        }
    }

  contribuer_au_second_membre(secmem); //TODO DG a integrer proprement dans la boucle

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

  const Domaine_DG& domaine = le_dom_dg_.valeur();

  int nb_bords=domaine.nb_front_Cl();

  const IntTab& face_voisins = domaine.face_voisins();
  const DoubleTab& face_normales = domaine.face_normales();

  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue());
  const Quadrature_base& quad = domaine.get_quadrature();
  const DoubleTab& integ_points_facets = quad.get_integ_points_facets();
  int nb_pts_int_fac = integ_points_facets.dimension(1);

  const int nb_bfunc = ch.nb_bfunc();

  DoubleTab fbase(nb_bfunc, nb_pts_int_fac);
  DoubleTab grad_fbase(nb_bfunc,nb_pts_int_fac, Objet_U::dimension);
  DoubleTab scalar_product(nb_pts_int_fac);

  //Les conditions aux limites pour le second membre
  const DoubleTab& eta_F = ch.get_eta_facet();  // Compute the penalisation coefficient
  int ind_face;
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = la_zcl_dg_->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
      int num1f=0;
      int num2f=le_bord.nb_faces();

      if (sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          //On ne fait rien et c'est normal
        }
      else if (sub_type(Dirichlet,la_cl.valeur()))
        {
          const Dirichlet& dirichlet =
            ref_cast(Dirichlet,la_cl.valeur());

          double xk=0.,yk=0.,zk=0.;
          double temps = equation().schema_temps().temps_courant();

          if (sub_type(Champ_front_txyz,dirichlet.champ_front()))
            {
              const Champ_front_txyz& champ_front =
                ref_cast(Champ_front_txyz,dirichlet.champ_front());

              for (int ind_faceb=num1f; ind_faceb<num2f; ind_faceb++)
                {

                  ind_face = le_bord.num_face(ind_faceb);

                  int elem = face_voisins(ind_face, 0); // The cell that have one facet on the boundary

                  ch.eval_bfunc_on_facets(quad, elem, ind_face, fbase);
                  ch.eval_grad_bfunc_on_facets(quad, elem, ind_face, grad_fbase);

                  double sur_f = domaine.face_surfaces(ind_face);

                  double h_T = sqrt(domaine.carre_pas_maille(elem));
                  double invh_T = 1./h_T; //TODO regarder penalisation remplacer h_T par h_F
                  double nu = nu_(elem);

                  for( int i=0; i< nb_bfunc; i++)
                    {
                      scalar_product = 0.;
                      for (int k = 0; k < nb_pts_int_fac ; k++)
                        {
                          //Coordonnees des points d'integration
                          xk=integ_points_facets(ind_face,k,0);
                          yk=integ_points_facets(ind_face,k,1);
                          if (dimension ==3) zk=integ_points_facets(ind_face,k,2);

                          double u_bord_k= champ_front.valeur_au_temps_et_au_point(temps,0,xk,yk,zk,0);
                          for (int d=0; d<Objet_U::dimension; d++)
                            scalar_product(k) -= face_normales(ind_face,d)/sur_f * grad_fbase(i, k, d)*u_bord_k;      //  \eta/H_F \int g \vvec_h
                          scalar_product(k) += eta_F(ind_face)*invh_T * u_bord_k*fbase(i, k);                              // \eta/H_F \int g \vvec_h
                        }
                      resu(elem,i)+=nu*quad.compute_integral_on_facet(ind_face, scalar_product);
                    }
                }
            }
        }
    }
}
