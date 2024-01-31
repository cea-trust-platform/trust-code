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

#include <Op_Dift_Multiphase_VDF_Elem.h>
#include <Op_Dift_Multiphase_VDF_Face.h>
#include <Pb_Multiphase.h>

Implemente_instanciable_sans_constructeur(Op_Dift_Multiphase_VDF_Elem,"Op_Diff_VDFTURBULENTE_P0_VDF|Op_Diff_VDFTURBULENT_P0_VDF",Op_Dift_VDF_Elem_base);

Sortie& Op_Dift_Multiphase_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_Multiphase_VDF_Elem::readOn(Entree& is)
{
  //lecture de la correlation de diffusivite turbulente
  Correlation_base::typer_lire_correlation(corr_, equation().probleme(), "transport_turbulent", is);
  associer_corr_impl<Type_Operateur::Op_DIFT_MULTIPHASE_ELEM, Eval_Dift_Multiphase_VDF_Elem>(corr_);
  associer_proto(equation().probleme(), champs_compris_);
  ajout_champs_proto_elem();
  return is;
}

void Op_Dift_Multiphase_VDF_Elem::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Op_Dift_VDF_Elem_base::get_noms_champs_postraitables(nom,opt);
  get_noms_champs_postraitables_proto(que_suis_je(), nom, opt);
}

void Op_Dift_Multiphase_VDF_Elem::creer_champ(const Motcle& motlu)
{
  Op_Dift_VDF_Elem_base::creer_champ(motlu);
  creer_champ_proto_elem(motlu);
}

void Op_Dift_Multiphase_VDF_Elem::completer()
{
  assert(corr_.non_nul());
  completer_Op_Dift_VDF_base();
  associer_pb<Eval_Dift_Multiphase_VDF_Elem>(equation().probleme());
  completer_proto_elem(*this);
  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_ELEM, Eval_Dift_Multiphase_VDF_Elem>(nu_ou_lambda_turb_);
}

void Op_Dift_Multiphase_VDF_Elem::mettre_a_jour(double temps)
{
  Op_Dift_VDF_Elem_base::mettre_a_jour(temps);

  const Operateur_base& op_qdm = equation().probleme().equation(0).operateur(0).l_op_base();
  if (!sub_type(Op_Dift_Multiphase_VDF_Face, op_qdm))
    {
      Cerr << "Error in " << que_suis_je() << ": no turbulent momentum diffusion found!" << finl;
      Process::exit();
    }

  const Correlation& corr_visc_qdm = ref_cast(Op_Dift_Multiphase_VDF_Face, op_qdm).correlation();
  if (!sub_type(Viscosite_turbulente_base, corr_visc_qdm.valeur()))
    {
      Cerr << "Error in " << que_suis_je() << ": no turbulent viscosity correlation found!" << finl;
      Process::exit();
    }

  // on calcule d_t_
  nu_ou_lambda_turb_ = 0.; // XXX : pour n'avoir pas la partie laminaire
  call_compute_diff_turb(ref_cast(Convection_Diffusion_std, equation()), ref_cast(Viscosite_turbulente_base, corr_visc_qdm.valeur()));
  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_ELEM, Eval_Dift_Multiphase_VDF_Elem>(nu_ou_lambda_turb_);
  mettre_a_jour_proto_elem(temps);
}

double Op_Dift_Multiphase_VDF_Elem::calculer_dt_stab() const
{
  double dt_stab, coef = -1.e10;
  const Domaine_VDF& domaine_VDF = iter_->domaine();
  const IntTab& elem_faces = domaine_VDF.elem_faces();
  const DoubleTab& lambda = alpha_() /* comme mu */, &diffu = diffusivite_pour_pas_de_temps().valeurs() /* comme nu */;
  const DoubleTab* alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue()->passe() : nullptr;
  const int cL = (lambda.dimension(0) == 1), cD = (diffu.dimension(0) == 1), dim = Objet_U::dimension;

  ArrOfInt numfa(2 * dim);
  for (int elem = 0; elem < domaine_VDF.nb_elem(); elem++)
    {
      double moy = 0.;
      for (int i = 0; i < 2 * dim; i++)
        numfa[i] = elem_faces(elem, i);

      for (int d = 0; d < dim; d++)
        {
          const double hd = domaine_VDF.dist_face(numfa[d], numfa[dim + d], d);
          moy += 1. / (hd * hd);
        }

      // TODO : FIXME : peut etre si alp > 1 e-3 pour eviter dt <<<< ??
      double alpha_diff_physique = (alp ? (*alp)(elem, 0) : 1.0) * lambda(!cL * elem, 0), alpha_diff_turbulent = (alp ? (*alp)(elem, 0) : 1.0) * nu_ou_lambda_turb_(elem, 0),
             diff_physique = lambda(!cL * elem, 0), diffu_ = diffu(!cD * elem, 0);

      for (int ncomp = 1; ncomp < lambda.line_size(); ncomp++)
        {
          alpha_diff_physique = std::max(alpha_diff_physique, (alp ? (*alp)(elem, ncomp) : 1.0) * lambda(!cL * elem, ncomp));
          diff_physique = std::max(diff_physique, lambda(!cL * elem, ncomp));
        }

      for (int ncomp = 1; ncomp < nu_ou_lambda_turb_.line_size(); ncomp++)
        alpha_diff_turbulent = std::max(alpha_diff_turbulent, (alp ? (*alp)(elem, ncomp) : 1.0) * nu_ou_lambda_turb_(elem, ncomp));

      for (int ncomp = 1; ncomp < diffu.line_size(); ncomp++)
        diffu_ = std::max(diffu_, diffu(!cD * elem, ncomp));

      // si on a associe mu au lieu de nu , on a nu sans diffu_dt
      // le pas de temps de stab est alpha(nu+nu_t), on calcule a(mu+mu_t)*(nu/mu)=a(mu+mu_t)/rho=a(nu+nu_t) (avantage par rapport a la division par rho ca marche aussi pour alpha et lambda et en VEF
      const double alpha_local = (alpha_diff_physique + alpha_diff_turbulent) / (diff_physique / diffu_) * moy;
      coef = std::max(coef, alpha_local);
    }

  coef = Process::mp_max(coef);
  dt_stab = 0.5 / (coef + DMINFLOAT);

  return dt_stab;
}

