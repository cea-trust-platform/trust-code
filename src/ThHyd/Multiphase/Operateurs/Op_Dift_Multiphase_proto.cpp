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

void Op_Dift_Multiphase_proto::ajout_champs_op_face()
{
  noms_nu_t_post_.dimensionner(pbm_->nb_phases()), nu_t_post_.resize(pbm_->nb_phases());
  for (int i = 0; i < pbm_->nb_phases(); i++)
    noms_nu_t_post_[i] = Nom("viscosite_turbulente_") + pbm_->nom_phase(i);
}

void Op_Dift_Multiphase_proto::get_noms_champs_postraitables_proto(const Nom& classe, Noms& nom, Option opt) const
{
  Noms noms_compris;
  for (int i = 0; i < pbm_->nb_phases(); i++)
    noms_compris.add(noms_nu_t_post_[i]);

  if (opt == DESCRIPTION)
    Cerr << classe << " : " << noms_compris << finl;
  else
    nom.add(noms_compris);
}

void Op_Dift_Multiphase_proto::creer_champ_proto(const Motcle& motlu)
{
  int i = noms_nu_t_post_.rang(motlu);
  if (i >= 0 && nu_t_post_[i].est_nul())
    {
      const Discret_Thyd& dis = ref_cast(Discret_Thyd, pbm_->discretisation());
      Noms noms(1), unites(1);
      noms[0] = noms_nu_t_post_[i];
      dis.discretiser_champ("CHAMP_ELEM", pbm_->domaine_dis(), scalaire, noms, unites, 1, 0, nu_t_post_[i]);
      Motcle syn = Nom("nu_turbulente_") + pbm_->nom_phase(i);
      nu_t_post_[i]->add_synonymous(syn);
      le_chmp_compris_->ajoute_champ(nu_t_post_[i]);
      Cerr << "Adding its synonym : " << syn << finl;
    }
}

void Op_Dift_Multiphase_proto::completer_proto()
{
  //si la correlation a besoin du gradient de u, on doit le creer maintenant
  if (corr_.non_nul() && ref_cast(Viscosite_turbulente_base, corr_.valeur()).gradu_required())
    pbm_->creer_champ("gradient_vitesse");
  if (corr_.non_nul())
    corr_->completer();

  // on initialise nu_t_ a 0 avec la bonne structure //
  nu_t_ = pbm_->equation_masse().inconnue()->valeurs();
  nu_t_ = 0.;
}

void Op_Dift_Multiphase_proto::mettre_a_jour_proto(const double temps)
{
  int N = pbm_->nb_phases();
  for (int n = 0; n < N; n++)
    if (nu_t_post_[n].non_nul()) //viscosite turbulente : toujours scalaire
      {
        const DoubleTab& rho = pbm_->milieu().masse_volumique().passe();
        DoubleTab& val = nu_t_post_[n]->valeurs();
        int nl = val.dimension(0), cR = (rho.dimension(0) == 1);
        for (int i = 0; i < nl; i++)
          val(i, 0) = rho(!cR * i, n) * nu_t_(i, n);
        nu_t_post_[n].mettre_a_jour(temps);
      }
}
