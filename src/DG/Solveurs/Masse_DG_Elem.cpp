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

#include <Masse_DG_Elem.h>
#include <Domaine_Cl_DG.h>
#include <Champ_Elem_DG.h>
#include <Domaine_DG.h>
#include <Option_DG.h>
#include <TRUSTTab_parts.h>
#include <Equation_base.h>
#include <Neumann_paroi.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Milieu_base.h>
#include <Conds_lim.h>

Implemente_instanciable(Masse_DG_Elem, "Masse_DG_Elem", Masse_DG_base);

Sortie& Masse_DG_Elem::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_DG_Elem::readOn(Entree& s) { return s; }

//ne touche que la partie "elements"
DoubleTab& Masse_DG_Elem::appliquer_impl(DoubleTab& sm) const
{
  throw;
//  const Domaine_DG& domaine_DG = le_dom_DG.valeur();
//  const DoubleVect& volumes = domaine_DG.volumes();
//  const DoubleVect& porosite_elem = equation().milieu().porosite_elem();
//
//  throw;
//
//  const int nb_elem = domaine_DG.nb_elem(), nb_dim = sm.nb_dim();
//  if (nb_elem == 0)
//    {
//      sm.echange_espace_virtuel();
//      return sm;
//    }
//
//  if (nb_dim == 2)
//    {
//      const int nb_comp = sm.line_size(); //sm.dimension_tot(0)/nb_elem;
//      for (int num_elem = 0; num_elem < nb_elem; num_elem++)
//        for (int k = 0; k < nb_comp; k++)
//          sm(num_elem, k) /= (volumes(num_elem) * porosite_elem(num_elem));
//    }
//  else if (sm.nb_dim() == 3)
//    {
//      const int d1 = sm.dimension(1), d2 = sm.dimension(2);
//      for (int num_elem = 0; num_elem < nb_elem; num_elem++)
//        for (int k = 0; k < d1; k++)
//          for (int d = 0; d < d2; d++)
//            sm(num_elem, k, d) /= (volumes(num_elem) * porosite_elem(num_elem));
//    }
//  else
//    {
//      Cerr << "Masse_DG_Elem::appliquer ne peut pas s'appliquer a un DoubleTab a " << sm.nb_dim() << " dimensions" << endl;
//      Process::exit();
//    }
//  sm.echange_espace_virtuel();
  return sm;
}

//Masse_DG_Elem est responsable des parties de la matrice n'impliquant pas la diffusion
void Masse_DG_Elem::dimensionner(Matrice_Morse& matrix) const
{
  const Domaine_DG& domaine = le_dom_dg_.valeur();

  IntTab indice(0, 2);

//  const IntTab& dof_elem = domaine.dof_elem();

//  int nb_inc = dof_elem.size();

  int current_indice = 0;
  for (int e = 0; e < domaine.nb_elem(); e++)
//    for (int inc = 0; inc < nb_inc; inc++)
    {
      int nordre = Option_DG::Get_order_for("temperature") ; // dof_elem(e);
      int ncol = Option_DG::Nb_col_from_order(nordre);
      for (int i = 0; i < ncol; i++ )
        for (int j = 0; j < ncol; j++ )
          indice.append_line( current_indice+i, current_indice+j);
      current_indice+=ncol;
    }
  tableau_trier_retirer_doublons(indice);
  Matrix_tools::allocate_morse_matrix(current_indice, current_indice, indice, matrix);
}

DoubleTab& Masse_DG_Elem::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{

  throw;
  // TODO
  //  const Domaine_DG& domaine = le_dom_DG.valeur();
  //  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue().valeur());
  //  const Conds_lim& cls = le_dom_Cl_DG->les_conditions_limites();
  //  const DoubleVect& ve = domaine.volumes(), &pe = equation().milieu().porosite_elem(), &fs = domaine.face_surfaces();
  //  int e, f, ne_tot = domaine.nb_elem_tot(), n, N = inco.line_size();
  //  DoubleVect coef(equation().milieu().porosite_elem());
  //  coef = 1.;
  //  if (has_coefficient_temporel_) appliquer_coef(coef);
  //
  //  ch.fcl();
  //  //partie superieure : diagonale
  //  for (e = 0; e < domaine.nb_elem(); e++)
  //    for (n = 0; n < N; n++)
  //      secmem(e, n) += coef(e) * pe(e) * ve(e) * inco(e, n) / dt;
  //
  //  //partie inferieure : valeur imposee pour les CLs de Neumann / Dirichlet / Echange_Impose
  //  for (f = 0; secmem.dimension_tot(0) > ne_tot && f < domaine.nb_faces(); f++)
  //    if (ch.fcl()(f, 0) == 4)
  //      for (n = 0; n < N; n++) //Neumann_paroi
  //        secmem(N * (ne_tot + f) + n) -= fs(f) * ref_cast(Neumann_paroi, cls[ch.fcl()(f, 1)].valeur()).flux_impose(ch.fcl()(f, 2), n);
  //    else if (ch.fcl()(f, 0) == 6)
  //      for (n = 0; n < N; n++) //Dirichlet
  //        secmem(N * (ne_tot + f) + n) += ref_cast(Dirichlet, cls[ch.fcl()(f, 1)].valeur()).val_imp(ch.fcl()(f, 2), n);

  return secmem;
}

Matrice_Base& Masse_DG_Elem::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  const Domaine_DG& domaine = le_dom_dg_.valeur();
  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);
  const DoubleVect& ve = domaine.volumes();
  const DoubleTab& xp = domaine.xp(); // barycentre elem
  const DoubleTab& xv = domaine.xv(); // barycentre facettes
  const IntTab& elem_faces = domaine.elem_faces();

//  const IntTab& dof_elem = domaine.dof_elem();

//  int nb_inc = dof_elem.size();

  const Elem_geom_base& elem_geom = domaine.domaine().type_elem().valeur();

  bool is_simplexe = strcmp(elem_geom.que_suis_je(), "Triangle") || strcmp(elem_geom.que_suis_je(), "Tetraedre");

  if (!is_simplexe) throw; //TODO for polyhedron

  int current_indice = 0, ncol = 0, order = 0;
  for (int e = 0; e < domaine.nb_elem(); e++)
//    for (int inc = 0; inc < nb_inc; inc++)
    {
      if (Objet_U::dimension == 2)
        {
          order = Option_DG::Get_order_for("temperature") ; // dof_elem(e);

          mat(current_indice, current_indice) = ve(e) / dt;

          if (order == 0) continue;

          /****************************************************************/
          /* Formule de quadrature : Ern, Finite Elements II, 2021, p 71  */
          /****************************************************************/

          mat(current_indice, current_indice+1) = 0.;
          mat(current_indice, current_indice+2) = 0.; // A virer de la structure


          mat(current_indice+1, current_indice+1) = 0.;
          mat(current_indice+1, current_indice+2) = 0.;
          mat(current_indice+2, current_indice+2) = 0.;
          for (int f = 0; f < 3; f++)
            {
              mat(current_indice+1, current_indice+1) += (xv(elem_faces(e,f),0) - xp(e,0))*(xv(elem_faces(e,f),0) - xp(e,0));
              mat(current_indice+1, current_indice+2) += (xv(elem_faces(e,f),0) - xp(e,0))*(xv(elem_faces(e,f),1) - xp(e,1));
              mat(current_indice+2, current_indice+2) += (xv(elem_faces(e,f),1) - xp(e,1))*(xv(elem_faces(e,f),1) - xp(e,1));
            }

          double inv3hT2dt = 1./(3*domaine.carre_pas_maille(e)*dt);

          mat(current_indice+1, current_indice+1) *= ve(e)*inv3hT2dt;
          mat(current_indice+1, current_indice+2) *= ve(e)*inv3hT2dt;
          mat(current_indice+2, current_indice+2) *= ve(e)*inv3hT2dt;

          // symmetry
          mat(current_indice+1, current_indice) = mat(current_indice, current_indice+1); // a virer
          mat(current_indice+2, current_indice) = mat(current_indice, current_indice+2); // a virer

          mat(current_indice+2, current_indice+1) = mat(current_indice+1, current_indice+2);


          if (order == 1) continue;

          throw;

        }
      else if (Objet_U::dimension == 3)
        throw; //TODO
      else
        Process::exit();

      current_indice+=ncol;
    }

  return matrice;
}
