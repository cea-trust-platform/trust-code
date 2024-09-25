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

#include <Op_Dift_Multiphase_VDF_Face.h>
#include <VDF_discretisation.h>
#include <Pb_Multiphase.h>

Implemente_instanciable_sans_constructeur(Op_Dift_Multiphase_VDF_Face,"Op_Diff_VDFTURBULENTE_Face|Op_Diff_VDFTURBULENT_Face",Op_Dift_VDF_Face_base);

Sortie& Op_Dift_Multiphase_VDF_Face::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_Multiphase_VDF_Face::readOn(Entree& is)
{
  //lecture de la correlation de viscosite turbulente
  Correlation_base::typer_lire_correlation(corr_, equation().probleme(), "viscosite_turbulente", is);
  associer_corr_impl<Type_Operateur::Op_DIFT_MULTIPHASE_FACE, Eval_Dift_Multiphase_VDF_Face>(corr_);
  associer_proto(equation().probleme(), champs_compris_);
  ajout_champs_proto_face();
  return is;
}

void Op_Dift_Multiphase_VDF_Face::get_noms_champs_postraitables(Noms& noms,Option opt) const
{
  Op_Dift_VDF_Face_base::get_noms_champs_postraitables(noms,opt);
  get_noms_champs_postraitables_proto(que_suis_je(), noms, opt);
}

void Op_Dift_Multiphase_VDF_Face::creer_champ(const Motcle& motlu)
{
  Op_Dift_VDF_Face_base::creer_champ(motlu);
  creer_champ_proto_face(motlu);
}

void Op_Dift_Multiphase_VDF_Face::completer()
{
  assert(corr_.non_nul());
  completer_Op_Dift_VDF_base();
  associer_pb<Eval_Dift_Multiphase_VDF_Face>(equation().probleme());

  completer_proto_face(*this);

  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_FACE, Eval_Dift_Multiphase_VDF_Face>(nu_ou_lambda_turb_);
}

void Op_Dift_Multiphase_VDF_Face::preparer_calcul()
{
  Op_Dift_VDF_Face_base::preparer_calcul();
  call_compute_nu_turb();
}

void Op_Dift_Multiphase_VDF_Face::mettre_a_jour(double temps)
{
  // MAJ nu_t
  nu_ou_lambda_turb_ = 0.;
  call_compute_nu_turb();
  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_FACE, Eval_Dift_Multiphase_VDF_Face>(nu_ou_lambda_turb_);
  mettre_a_jour_proto_face(temps);
}

double Op_Dift_Multiphase_VDF_Face::calculer_dt_stab() const
{
  double dt_stab, coef = -1.e10;
  const Domaine_VDF& domaine_VDF = iter_->domaine();
  const Champ_base& champ_diffu = diffusivite_pour_pas_de_temps();
  const DoubleTab& diffu = diffusivite().valeurs() /* mu */, &rho = equation().milieu().masse_volumique().passe(), &diffu_dt = champ_diffu.valeurs() /* nu */;
  const DoubleTab* alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const int cN = (diffu_dt.dimension(0) == 1), cM = (diffu.dimension(0) == 1), cR = (rho.dimension(0) == 1);

  for (int elem = 0; elem < domaine_VDF.nb_elem(); elem++)
    {
      double diflo = 0.;
      for (int i = 0; i < Objet_U::dimension; i++)
        {
          const double h = domaine_VDF.dim_elem(elem, i);
          diflo += 1. / (h * h);
        }

      // TODO : FIXME : pt etre alp > 1 e-3 pour eviter dt <<<< ??
      double mu_physique = diffu(!cM * elem, 0), alpha_mu_physique = (alp ? (*alp)(elem, 0) : 1.0) * diffu(!cM * elem, 0),
             alpha_mu_turbulent = (alp ? (*alp)(elem, 0) : 1.0) * rho(!cR * elem, 0) * nu_ou_lambda_turb_(elem, 0), nu_physique = diffu_dt(!cN * elem, 0);

      for (int ncomp = 1; ncomp < diffu.line_size(); ncomp++)
        {
          mu_physique = std::max(mu_physique, diffu(!cM * elem, ncomp));
          alpha_mu_physique = std::max(alpha_mu_physique, (alp ? (*alp)(elem, ncomp) : 1.0) * diffu(!cM * elem, ncomp));
        }

      for (int ncomp = 1; ncomp < nu_ou_lambda_turb_.line_size(); ncomp++)
        alpha_mu_turbulent = std::max(alpha_mu_turbulent, (alp ? (*alp)(elem, 0) : 1.0) * rho(!cR * elem, ncomp) * nu_ou_lambda_turb_(elem, ncomp));

      for (int ncomp = 1; ncomp < diffu_dt.line_size(); ncomp++)
        nu_physique = std::max(nu_physique, diffu_dt(!cN * elem, ncomp));

      // si on a associe mu au lieu de nu , on a nu sans diffu_dt
      // le pas de temps de stab est alpha(nu+nu_t), on calcule a(mu+mu_t)*(nu/mu)=a(mu+mu_t)/rho=a(nu+nu_t) (avantage par rapport a la division par rho ca marche aussi pour alpha et lambda et en VEF
      diflo *= (alpha_mu_physique + alpha_mu_turbulent) * (nu_physique / mu_physique);
      coef = std::max(coef, diflo);
    }
  coef = Process::mp_max(coef);
  dt_stab = 0.5 / (coef + DMINFLOAT);

  return dt_stab;
}
