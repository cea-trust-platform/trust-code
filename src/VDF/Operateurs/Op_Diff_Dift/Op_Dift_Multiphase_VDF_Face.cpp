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
#include <Viscosite_turbulente_base.h>
#include <VDF_discretisation.h>
#include <Pb_Multiphase.h>

Implemente_instanciable_sans_constructeur(Op_Dift_Multiphase_VDF_Face,"Op_Diff_VDFTURBULENTE_Face",Op_Dift_VDF_Face_base);

Sortie& Op_Dift_Multiphase_VDF_Face::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_Multiphase_VDF_Face::readOn(Entree& is)
{
  //lecture de la correlation de viscosite turbulente
  corr_.typer_lire(equation().probleme(), "viscosite_turbulente", is);

  associer_corr_impl<Type_Operateur::Op_DIFT_MULTIPHASE_FACE, Eval_Dift_Multiphase_VDF_Face>(corr_);

  const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, equation().probleme());
  noms_nu_t_post_.dimensionner(pb.nb_phases()), nu_t_post_.resize(pb.nb_phases());
  for (int i = 0; i < pb.nb_phases(); i++)
    noms_nu_t_post_[i] = Nom("viscosite_turbulente_") + pb.nom_phase(i);
  return is;
}

Op_Dift_Multiphase_VDF_Face::Op_Dift_Multiphase_VDF_Face() : Op_Dift_VDF_Face_base(Iterateur_VDF_Face<Eval_Dift_Multiphase_VDF_Face>())
{
  // declare_support_masse_volumique(1); // poubelle
}

void Op_Dift_Multiphase_VDF_Face::creer_champ(const Motcle& motlu)
{
  Op_Dift_VDF_Face_base::creer_champ(motlu);
  int i = noms_nu_t_post_.rang(motlu);
  if (i >= 0 && nu_t_post_[i].est_nul())
    {
      const VDF_discretisation dis = ref_cast(VDF_discretisation, equation().discretisation());
      Noms noms(1), unites(1);
      noms[0] = noms_nu_t_post_[i];
      Motcle typeChamp = "champ_elem" ;
      dis.discretiser_champ(typeChamp, equation().domaine_dis(), scalaire, noms , unites, 1, 0, nu_t_post_[0]);
      champs_compris_.ajoute_champ(nu_t_post_[i]);
    }
}

void Op_Dift_Multiphase_VDF_Face::completer()
{
  assert(corr_.non_nul());
  completer_Op_Dift_VDF_base();
  associer_pb<Eval_Dift_Multiphase_VDF_Face>(equation().probleme());

  //si la correlation a besoin du gradient de u, on doit le creer maintenant
  if (ref_cast(Viscosite_turbulente_base, corr_.valeur()).gradu_required())
    equation().probleme().creer_champ("gradient_vitesse");

  // on initialise nu_t_ a 0 avec la bonne structure //
  nu_t_ = ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue()->valeurs();
  nu_t_ = 0.;
  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_FACE, Eval_Dift_Multiphase_VDF_Face>(nu_t_);
  ref_cast(Viscosite_turbulente_base, corr_.valeur()).eddy_viscosity(nu_t_); //remplissage par la correlation
}

void Op_Dift_Multiphase_VDF_Face::mettre_a_jour(double temps)
{
  // MAJ nu_t
  nu_t_ = 0.;
  ref_cast(Viscosite_turbulente_base, corr_.valeur()).eddy_viscosity(nu_t_); //remplissage par la correlation
  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_FACE, Eval_Dift_Multiphase_VDF_Face>(nu_t_);

  int N = ref_cast(Pb_Multiphase, equation().probleme()).nb_phases();
  for (int n = 0; n < N; n++)
    if (nu_t_post_[n].non_nul()) //viscosite turbulente : toujours scalaire
      {
        const DoubleTab& rho = equation().milieu().masse_volumique().passe() ;
        DoubleTab& val = nu_t_post_[n]->valeurs();
        int nl = val.dimension(0), cR = (rho.dimension(0) == 1);
        for (int i = 0; i < nl; i++) val(i, 0) = rho(!cR * i, n) * nu_t_(i, n);
        nu_t_post_[n].mettre_a_jour(temps);
      }
}

double Op_Dift_Multiphase_VDF_Face::calculer_dt_stab() const
{
  double dt_stab, coef = -1.e10;
  const Domaine_VDF& domaine_VDF = iter->domaine();
  const Champ_base& champ_diffu = diffusivite_pour_pas_de_temps();
  const DoubleTab& diffu = diffusivite().valeurs() /* mu */, &rho = equation().milieu().masse_volumique().passe(), &diffu_dt = champ_diffu.valeurs() /* nu */;
  const DoubleTab& alp = ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe();
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
      double mu_physique = diffu(!cM * elem, 0), alpha_mu_physique = alp(elem, 0) * diffu(!cM * elem, 0),
             alpha_mu_turbulent = alp(elem, 0) * rho(!cR * elem, 0) * nu_t_(elem, 0), nu_physique = diffu_dt(!cN * elem, 0);

      for (int ncomp = 1; ncomp < diffu.line_size(); ncomp++)
        {
          mu_physique = std::max(mu_physique, diffu(!cM * elem, ncomp));
          alpha_mu_physique = std::max(alpha_mu_physique, alp(elem, ncomp) * diffu(!cM * elem, ncomp));
        }

      for (int ncomp = 1; ncomp < nu_t_.line_size(); ncomp++)
        alpha_mu_turbulent = std::max(alpha_mu_turbulent, alp(elem, 0) * rho(!cR * elem, ncomp) * nu_t_(elem, ncomp));

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
