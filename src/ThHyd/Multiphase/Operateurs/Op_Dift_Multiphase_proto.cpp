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

#include <Op_Dift_Multiphase_proto.h>
#include <Transport_turbulent_base.h>
#include <Convection_Diffusion_std.h>
#include <Pb_Multiphase.h>
#include <Discret_Thyd.h>

void Op_Dift_Multiphase_proto::associer_proto(const Pb_Multiphase& pb, Champs_compris& ch)
{
  pbm_ = pb;
  le_chmp_compris_ = ch;
}

void Op_Dift_Multiphase_proto::ajout_champs_(const bool is_face)
{
  noms_nu_ou_lambda_turb_post_.dimensionner(pbm_->nb_phases());
  nu_ou_lambda_turb_post_.resize(pbm_->nb_phases());

  for (int i = 0; i < pbm_->nb_phases(); i++)
    noms_nu_ou_lambda_turb_post_[i] = is_face ? Nom("viscosite_turbulente_") + pbm_->nom_phase(i) : Nom("diffusivite_turbulente_") + pbm_->nom_phase(i);
}

void Op_Dift_Multiphase_proto::get_noms_champs_postraitables_proto(const Nom& classe, Noms& nom, Option opt) const
{
  Noms noms_compris;
  for (int i = 0; i < pbm_->nb_phases(); i++)
    noms_compris.add(noms_nu_ou_lambda_turb_post_[i]);

  if (opt == DESCRIPTION)
    Cerr << classe << " : " << noms_compris << finl;
  else
    nom.add(noms_compris);
}

void Op_Dift_Multiphase_proto::creer_champ_(const Motcle& motlu, const bool is_face)
{
  int i = noms_nu_ou_lambda_turb_post_.rang(motlu);
  if (i >= 0 && nu_ou_lambda_turb_post_[i].est_nul())
    {
      const Discret_Thyd& dis = ref_cast(Discret_Thyd, pbm_->discretisation());
      Noms noms(1), unites(1);
      noms[0] = noms_nu_ou_lambda_turb_post_[i];
      dis.discretiser_champ("CHAMP_ELEM", pbm_->domaine_dis(), scalaire, noms, unites, 1, 0, nu_ou_lambda_turb_post_[i]);

      // We add some synonyms
      std::vector<Motcle> syn;
      if (is_face)
        syn.push_back(Nom("nu_turbulente_") + pbm_->nom_phase(i));
      else
        {
          syn.push_back(Nom("lambda_turbulente_") + pbm_->nom_phase(i));
          syn.push_back(Nom("conductivite_turbulente_") + pbm_->nom_phase(i));
        }

      for (auto& itr : syn)
        nu_ou_lambda_turb_post_[i]->add_synonymous(itr);

      le_chmp_compris_->ajoute_champ(nu_ou_lambda_turb_post_[i]);
      for (auto& itr : syn)
        Cerr << "   => Adding its synonym : " << itr << finl;
    }
}

void Op_Dift_Multiphase_proto::completer_(const Operateur_Diff_base& op,const bool is_face)
{
  //si la correlation a besoin du gradient de u, on doit le creer maintenant
  if (is_face)
    {
      if (corr_.non_nul() && ref_cast(Viscosite_turbulente_base, corr_.valeur()).gradu_required())
        pbm_->creer_champ("gradient_vitesse");
    }
  else
    {
      if (corr_.non_nul() && ref_cast(Transport_turbulent_base, corr_.valeur()).gradu_required())
        pbm_->creer_champ("gradient_vitesse");
    }

  if (corr_.non_nul())
    corr_->completer();

  const int nb_elem = ref_cast(Domaine_VF, op.equation().domaine_dis().valeur()).nb_elem_tot(),  N = op.equation().inconnue().valeurs().line_size();
  nu_ou_lambda_turb_.resize(nb_elem, N);
  nu_ou_lambda_turb_ = 0.;
}

void Op_Dift_Multiphase_proto::mettre_a_jour_(const double temps, const bool is_face)
{
  int N = pbm_->nb_phases();
  for (int n = 0; n < N; n++)
    if (nu_ou_lambda_turb_post_[n].non_nul()) // viscosite/diffusivite turbulente : toujours scalaire
      {
        DoubleTab& val = nu_ou_lambda_turb_post_[n]->valeurs();
        const int nl = val.dimension(0);
        if (is_face)
          {
            const DoubleTab& rho = pbm_->milieu().masse_volumique().passe();
            const int cR = (rho.dimension(0) == 1);
            for (int i = 0; i < nl; i++)
              val(i, 0) = rho(!cR * i, n) * nu_ou_lambda_turb_(i, n);
          }
        else
          for (int i = 0; i < nl; i++)
            val(i, 0) = nu_ou_lambda_turb_(i, n);

        nu_ou_lambda_turb_post_[n].mettre_a_jour(temps);
      }
}
