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

#include <Convection_Diffusion_Temperature.h>
#include <Flux_interfacial_PolyMAC_P0P1NC.h>
#include <Echange_contact_PolyMAC_P0.h>
#include <Op_Diff_PolyMAC_P0_Elem.h>
#include <Echange_externe_impose.h>
#include <Champ_Elem_PolyMAC_P0.h>
#include <Flux_parietal_base.h>
#include <Domaine_PolyMAC_P0.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Milieu_composite.h>
#include <Option_PolyMAC.h>
#include <Neumann_paroi.h>
#include <Pb_Multiphase.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <functional>
#include <cmath>

extern Stat_Counter_Id diffusion_counter_;

Implemente_instanciable_sans_constructeur(Op_Diff_PolyMAC_P0_Elem, "Op_Diff_PolyMAC_P0_Elem|Op_Diff_PolyMAC_P0_var_Elem", Op_Diff_PolyMAC_P0_base);
Implemente_instanciable(Op_Dift_PolyMAC_P0_Elem, "Op_Dift_PolyMAC_P0_Elem_PolyMAC_P0|Op_Dift_PolyMAC_P0_var_Elem_PolyMAC_P0", Op_Diff_PolyMAC_P0_Elem);

Op_Diff_PolyMAC_P0_Elem::Op_Diff_PolyMAC_P0_Elem()
{
  declare_support_masse_volumique(1);
}

Sortie& Op_Diff_PolyMAC_P0_Elem::printOn(Sortie& os) const { return Op_Diff_PolyMAC_P0_base::printOn(os); }

Sortie& Op_Dift_PolyMAC_P0_Elem::printOn(Sortie& os) const { return Op_Diff_PolyMAC_P0_base::printOn(os); }

Entree& Op_Diff_PolyMAC_P0_Elem::readOn(Entree& is) { return Op_Diff_PolyMAC_P0_base::readOn(is); }

Entree& Op_Dift_PolyMAC_P0_Elem::readOn(Entree& is) { return Op_Diff_PolyMAC_P0_base::readOn(is); }

void Op_Diff_PolyMAC_P0_Elem::completer()
{
  Op_Diff_PolyMAC_P0_base::completer();

  const Equation_base& eq = equation();
  const Champ_Elem_PolyMAC_P0& ch = ref_cast(Champ_Elem_PolyMAC_P0, eq.inconnue());
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());

  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 1)
    {
      Cerr << "Op_Diff_PolyMAC_P0_Elem : largeur de joint insuffisante (minimum 1)!" << finl;
      Process::exit();
    }

  flux_bords_.resize(domaine.premiere_face_int(), ch.valeurs().line_size());

  /*
   * XXX fill some useful bools !
   */
  is_pb_multi_ = sub_type(Pb_Multiphase, eq.probleme());
  is_pb_coupl_ = eq.probleme().is_coupled();
  has_flux_par_ = (sub_type(Energie_Multiphase, equation()) || sub_type(Convection_Diffusion_Temperature, equation()))
                  && equation().probleme().has_correlation("flux_parietal");

  for (const auto &itr : la_zcl_poly_->les_conditions_limites())
    if (sub_type(Echange_contact_PolyMAC_P0, itr.valeur()))
      {
        has_echange_contact_ = true;
        break;
      }

  if (is_pb_coupl_ || has_flux_par_)
    couplage_parietal_helper_.associer(*this);

  if (has_flux_par_)
    if (ref_cast(Flux_parietal_base, eq.probleme().get_correlation("Flux_parietal")).calculates_bubble_nucleation_diameter())
      couplage_parietal_helper_.completer_d_nuc();
}

double Op_Diff_PolyMAC_P0_Elem::calculer_dt_stab() const
{
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  const Champ_Elem_PolyMAC_P0&  ch  = ref_cast(Champ_Elem_PolyMAC_P0, equation().inconnue());
  const IntTab& e_f = domaine.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& nf = domaine.face_normales();

  const DoubleTab& diffu = diffusivite_pour_pas_de_temps().valeurs(),
                   &lambda = diffusivite().valeurs();

  const DoubleTab *alp = sub_type(Pb_Multiphase, equation().probleme()) ?
                         &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() :
                         (has_champ_masse_volumique() ? &get_champ_masse_volumique().valeurs() : nullptr);

  const DoubleVect& pe = equation().milieu().porosite_elem(), &vf = domaine.volumes_entrelaces(), &ve = domaine.volumes();

  update_nu();

  double dt = 1e10;

  const int N = equation().inconnue().valeurs().dimension(1), cD = (diffu.dimension(0) == 1), cL = (lambda.dimension(0) == 1);

  DoubleTrav flux(N);

  for (int e = 0; e < domaine.nb_elem(); e++)
    {
      flux = 0.;

      for (int i = 0; i < e_f.dimension(1); i++)
        {
          const int f = e_f(e, i);
          if (f < 0) continue;

          if (!Option_PolyMAC::TRAITEMENT_AXI || (Option_PolyMAC::TRAITEMENT_AXI && !(fcl(f,0) == 4 || fcl(f,0) == 5)) )
            for (int n = 0; n < N; n++)
              flux(n) += domaine.nu_dot(&nu_, e, n, &nf(f, 0), &nf(f, 0)) / vf(f);
        }

      for (int n = 0; n < N; n++)
        if ((!alp || (*alp)(e, n) > 1e-3) && flux(n)) /* sous 0.5e-6, on suppose que l'evanescence fait le job */
          dt = std::min(dt, pe(e) * ve(e) * (alp ? (*alp)(e, n) : 1) * (lambda(!cL * e, n) / diffu(!cD * e, n)) / flux(n));

      if (dt < 0)
        Process::exit(que_suis_je() + " : negative dt_stab calculated !!");
    }
  return Process::mp_min(dt);
}

void Op_Diff_PolyMAC_P0_Elem::mettre_a_jour(double t)
{
  Op_Diff_PolyMAC_P0_base::mettre_a_jour(t);
  couplage_parietal_helper_.d_nuc_a_jour() = 0;
}

const DoubleTab& Op_Diff_PolyMAC_P0_Elem::d_nucleation() const
{
  if (!couplage_parietal_helper_.d_nuc_a_jour())
    {
      Cerr << "Op_Diff_PolyMAC_P0_Elem : attempt to access d_nucleation (nucleate bubble diameter) before ajouter_blocs() has been called!" << finl
           << "Please call assembler_blocs() on Energie_Multiphase before calling it on Masse_Multiphase." << finl;
      Process::exit();
    }
  return couplage_parietal_helper_.d_nuc();
}

void Op_Diff_PolyMAC_P0_Elem::init_op_ext() const
{
  if (som_ext_init_)
    return; //deja fait

  if (is_pb_coupl_ || has_flux_par_)
    couplage_parietal_helper_.init_op_ext();
  else
    {
      op_ext = { this };
      som_ext_init_ = 1;
    }
}

void Op_Diff_PolyMAC_P0_Elem::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  init_op_ext();
  update_phif(!nu_constant_); //calcul de (nf.nu.grad T) : si nu variable, stencil complet

  const std::string nom_inco = equation().inconnue().le_nom().getString();
  if (semi_impl.count(nom_inco))
    return; //semi-implicite -> rien a dimensionner

  if (is_pb_coupl_ || has_flux_par_)
    {
      couplage_parietal_helper_.dimensionner_blocs(matrices, semi_impl);
      return;
    }


  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  const IntTab& f_e = domaine.face_voisins();

  Matrice_Morse* mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;

  const int N = equation().inconnue().valeurs().line_size(); //nombre de composantes

  IntTab stencil; //stencils par matrice
  stencil.resize(0, 2);


  IntTrav tpfa(0, N); //pour suivre quels flux sont a deux points
  domaine.creer_tableau_faces(tpfa);
  tpfa = 1;

  Cerr << "Op_Diff_PolyMAC_P0_Elem::dimensionner() : ";

  //avec fgrad : parties hors Echange_contact (ne melange ni les problemes, ni les composantes)
  for (int f = 0; f < domaine.nb_faces(); f++)
    for (int i = 0; i < 2; i++)
      {
        const int e = f_e(f, i);
        if (e < 0) continue;

        if (e < domaine.nb_elem()) //stencil a l'element e
          for (int j = phif_d(f); j < phif_d(f + 1); j++)
            {
              const int e_s = phif_e(j);

              if (e_s < domaine.nb_elem_tot())
                for (int n = 0; n < N; n++)
                  {
                    stencil.append_line(N * e + n, N * e_s + n);

                    const int tmp = (e_s == f_e(f, 0)) || (e_s == f_e(f, 1)) || (phif_c(j, n) == 0);
                    tpfa(f, n) &= tmp;
                  }
            }
      }

  if (mat)
    {
      tableau_trier_retirer_doublons(stencil);
      Matrice_Morse mat2;
      Matrix_tools::allocate_morse_matrix(N * domaine.nb_elem_tot(), N * equation().domaine_dis().nb_elem_tot(), stencil, mat2);

      if (mat->nb_colonnes())
        *mat += mat2;
      else
        *mat = mat2;
    }

  int n_sten = stencil.dimension(0);

  Cerr << "width " << Process::mp_sum(n_sten) * 1. / (N * domaine.domaine().md_vector_elements()->nb_items_seq_tot())
       << " "
       << mp_somme_vect(tpfa) * 100. / (N * domaine.md_vector_faces()->nb_items_seq_tot()) << "% TPFA " << finl;
}

void Op_Diff_PolyMAC_P0_Elem::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
#ifdef _COMPILE_AVEC_PGCC_AVANT_22_7
  Cerr << "Internal error with nvc++: Internal error: read_memory_region: not all expected entries were read." << finl;
  Process::exit();
#else
  statistiques().begin_count(diffusion_counter_);

  init_op_ext();
  update_phif();

  if (is_pb_coupl_ || has_flux_par_)
    {
      couplage_parietal_helper_.ajouter_blocs(matrices, secmem, semi_impl);
      statistiques().end_count(diffusion_counter_);
      return;
    }

  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, equation().domaine_dis());
  const Champ_Inc_base& ch_inc = has_champ_inco() ? mon_inconnue() : equation().inconnue();
  const Champ_Elem_PolyMAC_P0& ch = ref_cast(Champ_Elem_PolyMAC_P0, ch_inc);

  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : ch.valeurs();
  const DoubleVect& fs = domaine.face_surfaces();

  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl();

  const Conds_lim& cls = equation().domaine_Cl_dis().les_conditions_limites();

  const int N = inco.line_size();

  Matrice_Morse* mat = !semi_impl.count(nom_inco) && matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;

  /* avec phif : flux hors Echange_contact -> mat[0] seulement */
  DoubleTrav flux(N);

  for (int f = 0; f < domaine.nb_faces(); f++)
    {
      flux = 0.;

      for (int i = phif_d(f); i < phif_d(f + 1); i++)
        {
          const int eb = phif_e(i);
          const int fb = eb - domaine.nb_elem_tot();

          if (fb < 0) //element
            {
              for (int n = 0; n < N; n++)
                flux(n) += phif_c(i, n) * fs(f) * inco(eb, n);

              if (mat)
                for (int j = 0; j < 2; j++)
                  {
                    const int e = f_e(f, j);
                    if (e < 0) continue;

                    if (e < domaine.nb_elem())
                      for (int n = 0; n < N; n++) //derivees
                        (*mat)(N * e + n, N * eb + n) += (j ? 1 : -1) * phif_c(i, n) * fs(f);
                  }
            }
          else if (fcl(fb, 0) == 1 || fcl(fb, 0) == 2)
            {
              for (int n = 0; n < N; n++) //Echange_impose_base
                flux(n) += (phif_c(i, n) ? phif_c(i, n) * fs(f) *
                            ref_cast(Echange_impose_base, cls[fcl(fb, 1)].valeur()).T_ext(fcl(fb, 2), n) : 0);
            }
          else if (fcl(fb, 0) == 4)
            {
              for (int n = 0; n < N; n++) //Neumann non homogene
                flux(n) += (phif_c(i, n) ? phif_c(i, n) * fs(f) *
                            ref_cast(Neumann_paroi, cls[fcl(fb, 1)].valeur()).flux_impose(fcl(fb, 2), n) : 0);
            }
          else if (fcl(fb, 0) == 6)
            {
              for (int n = 0; n < N; n++) //Dirichlet
                flux(n) += (phif_c(i, n) ? phif_c(i, n) * fs(f) *
                            ref_cast(Dirichlet, cls[fcl(fb, 1)].valeur()).val_imp(fcl(fb, 2), n) : 0);
            }
        }

      for (int j = 0; j < 2; j++)
        {
          const int e = f_e(f, j);
          if (e < 0) continue;

          if (e < domaine.nb_elem())
            for (int n = 0; n < N; n++) //second membre -> amont/aval
              secmem(e, n) += (j ? -1 : 1) * flux(n);
        }

      if (f < domaine.premiere_face_int())
        for (int n = 0; n < N; n++)
          flux_bords_(f, n) = flux(n); //flux aux bords
    }

  statistiques().end_count(diffusion_counter_);
#endif
}

