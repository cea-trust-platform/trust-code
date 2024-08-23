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

#include <Modifier_pour_fluide_dilatable.h>
#include <Fluide_Dilatable_base.h>
#include <Modele_turbulence_hyd_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Domaine_VF.h>
#include <TRUSTVect.h>

static void multiplier_ou_diviser(DoubleVect& x, const DoubleVect& y, int diviser)
{
  if (!diviser)
    tab_multiply_any_shape(x, y, VECT_REAL_ITEMS);
  else
    tab_divide_any_shape(x, y, VECT_REAL_ITEMS);
  x.echange_espace_virtuel();
  //Debog::verifier("multiplier_ou_diviser x/y apres x:",x);
}

// Modif B.M: on ne remplit que la partie reelle du tableau.
void multiplier_diviser_rho(DoubleVect& tab, const Fluide_Dilatable_base& le_fluide, int diviser)
{
  const Domaine_VF& zvf = ref_cast(Domaine_VF, le_fluide.vitesse()->domaine_dis_base());
  // Descripteurs des tableaux aux elements et aux faces:
  const Domaine& domaine = zvf.domaine();
  const MD_Vector& md_elem = domaine.les_elems().get_md_vector();
  const MD_Vector& md_faces = zvf.md_vector_faces();
  const MD_Vector& md_faces_bord = zvf.md_vector_faces_bord();

  if (tab.get_md_vector() == md_faces_bord)
    {
      const DoubleTab& rho = ref_cast(Fluide_Dilatable_base,le_fluide).rho_discvit();
      DoubleTrav rho_bord;
      // B.M. je cree une copie sinon il faut truander les tests sur les tailles dans multiply_any_shape
      // ou creer un DoubleTab qui pointe sur rho...
      zvf.creer_tableau_faces_bord(rho_bord, RESIZE_OPTIONS::NOCOPY_NOINIT);
      rho_bord.inject_array(rho, rho_bord.size());
      multiplier_ou_diviser(tab, rho_bord, diviser);
      return;
    }

  if (tab.get_md_vector() == md_faces)
    {
      const DoubleTab& rho_faces = ref_cast(Fluide_Dilatable_base,le_fluide).rho_discvit();
      multiplier_ou_diviser(tab, rho_faces, diviser);
      return;
    }

  const DoubleTab& rho = le_fluide.masse_volumique()->valeurs();
  if (tab.get_md_vector() == md_elem && rho.get_md_vector() == md_elem)
    {
      multiplier_ou_diviser(tab, rho, diviser);
      return;
    }

  if (tab.get_md_vector() == md_elem && rho.get_md_vector() == md_faces)
    {
      // Il faut calculer rho aux elements
      DoubleTrav tab_rho_elem;
      domaine.creer_tableau_elements(tab_rho_elem, RESIZE_OPTIONS::NOCOPY_NOINIT);
      const int nb_elem_tot = domaine.nb_elem_tot();
      const int nfe = zvf.elem_faces().dimension(1);
      const double facteur = 1. / nfe;
      CDoubleArrView tab_rho = static_cast<const DoubleVect&>(le_fluide.masse_volumique()->valeurs()).view_ro();
      CIntTabView elem_faces = zvf.elem_faces().view_ro();
      DoubleArrView rho_elem = static_cast<DoubleVect&>(tab_rho_elem).view_wo();
      Kokkos::parallel_for(__KERNEL_NAME__, nb_elem_tot, KOKKOS_LAMBDA(const int elem)
      {
        double x = 0.;
        for (int face = 0; face < nfe; face++)
          {
            int f = elem_faces(elem, face);
            x += tab_rho[f];
          }
        rho_elem[elem] = x * facteur;
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
      multiplier_ou_diviser(tab, tab_rho_elem, diviser);
      return;
    }

  Cerr << "Error in Modifier_pour_fluide_dilatable.cpp: multiplier_rho. Invalid discretization of tab and rho." << finl;
  Process::exit();
}

/*! @brief multiplie le tableau val par la masse volumique si le fluide est dilatable.
 *
 * Le tableau val peut avoir diverses localisations (determinees a partir de get_md_vector())
 *   et peut etre de type DoubleTab avec des dimension(i>0) quelconques (plusieurs composantes)
 *
 */
void multiplier_par_rho_si_dilatable(DoubleVect& val, const Milieu_base& mil)
{
  if (sub_type(Fluide_Dilatable_base, mil))
    multiplier_diviser_rho(val, ref_cast(Fluide_Dilatable_base, mil), 0 /* multiplier */);
  else
    {
      Cerr << "What ?? The method multiplier_par_rho_si_dilatable should not be called since your fluid is not dilatable !!" << finl;
      Process::exit();
    }
}

/*! @brief Idem que multiplier_par_rho_si_dilatable mais on divise par rho.
 *
 */
void diviser_par_rho_si_dilatable(DoubleVect& val, const Milieu_base& mil)
{
  if (sub_type(Fluide_Dilatable_base, mil))
    multiplier_diviser_rho(val, ref_cast(Fluide_Dilatable_base, mil), 1 /* diviser */);
  else
    {
      Cerr << "What ?? The method diviser_par_rho_si_dilatable should not be called since your fluid is not dilatable !!" << finl;
      Process::exit();
    }
}

void correction_nut_et_cisaillement_paroi_si_qc(Modele_turbulence_hyd_base& mod)
{
  // on recgarde si on a un fluide QC
  if (sub_type(Fluide_Dilatable_base, mod.equation().probleme().milieu()))
    {
      const Fluide_Dilatable_base& le_fluide = ref_cast(Fluide_Dilatable_base, mod.equation().probleme().milieu());
      // 1 on multiplie nu_t par rho

      DoubleTab& nut = ref_cast_non_const(DoubleTab, mod.viscosite_turbulente()->valeurs());
      multiplier_diviser_rho(nut, le_fluide, 0 /* multiplier */);

      // 2  On modifie le ciasaillement paroi
      const DoubleTab& cisaillement_paroi = mod.loi_paroi()->Cisaillement_paroi();
      DoubleTab& cisaillement = ref_cast_non_const(DoubleTab, cisaillement_paroi);
      multiplier_diviser_rho(cisaillement, le_fluide, 0 /* multiplier */);
      cisaillement.echange_espace_virtuel();
    }
  else
    {
      Cerr << "What ?? The method Correction_nut_et_cisaillement_paroi_si_qc should not be called since your fluid is not dilatable !!" << finl;
      Process::exit();
    }
}
