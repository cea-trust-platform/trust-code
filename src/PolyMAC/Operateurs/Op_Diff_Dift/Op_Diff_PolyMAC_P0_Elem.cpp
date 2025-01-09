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

  if (has_flux_par_)
    if (ref_cast(Flux_parietal_base, eq.probleme().get_correlation("Flux_parietal")).calculates_bubble_nucleation_diameter())
      {
        d_nuc_.resize(0, ch.valeurs().line_size());
        domaine.domaine().creer_tableau_elements(d_nuc_);
      }
}

void Op_Diff_PolyMAC_P0_Elem::init_op_ext() const
{
  if (is_pb_coupl_ || has_flux_par_)
    init_op_ext_couplage_parietal();
  else
    init_op_ext_simple();
}

double Op_Diff_PolyMAC_P0_Elem::calculer_dt_stab() const
{
  const Domaine_PolyMAC_P0& domaine = ref_cast(Domaine_PolyMAC_P0, le_dom_poly_.valeur());
  const Champ_Elem_PolyMAC_P0& 	ch	= ref_cast(Champ_Elem_PolyMAC_P0, equation().inconnue());
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

void Op_Diff_PolyMAC_P0_Elem::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  init_op_ext();
  update_phif(!nu_constant_); //calcul de (nf.nu.grad T) : si nu variable, stencil complet

  const std::string nom_inco = equation().inconnue().le_nom().getString();
  if (semi_impl.count(nom_inco))
    return; //semi-implicite -> rien a dimensionner

  if (is_pb_coupl_ || has_flux_par_)
    dimensionner_blocs_couplage_parietal(matrices, semi_impl);
  else
    dimensionner_blocs_simple(matrices, semi_impl);
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
    ajouter_blocs_couplage_parietal(matrices, secmem, semi_impl);
  else
    ajouter_blocs_simple(matrices, secmem, semi_impl);

  statistiques().end_count(diffusion_counter_);
#endif
}

void Op_Diff_PolyMAC_P0_Elem::mettre_a_jour(double t)
{
  Op_Diff_PolyMAC_P0_base::mettre_a_jour(t);

  d_nuc_a_jour_ = 0;
}
