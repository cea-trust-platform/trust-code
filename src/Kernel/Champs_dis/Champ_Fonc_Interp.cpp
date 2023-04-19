/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Champ_Fonc_Interp.h>
#include <Param.h>

Implemente_instanciable(Champ_Fonc_Interp, "Champ_Fonc_Interp", Champ_Fonc_P0_base);

Sortie& Champ_Fonc_Interp::printOn(Sortie& os) const { return Champ_Fonc_P0_base::printOn(os); }

Entree& Champ_Fonc_Interp::readOn(Entree& is)
{
  Param param(que_suis_je());
  Nom nom_pb_loc, nom_pb_dist;
  param.ajouter("nom_champ", &nom_, Param::REQUIRED);
  param.ajouter("pb_loc", &nom_pb_loc, Param::REQUIRED);
  param.ajouter("pb_dist", &nom_pb_dist, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);

  pb_loc_ = ref_cast(Probleme_base,Interprete::objet(nom_pb_loc));
  pb_dist_ = ref_cast(Probleme_base,Interprete::objet(nom_pb_dist));
  return is;
}


int Champ_Fonc_Interp::initialiser(double temps)
{
  const int ok = Champ_Fonc_P0_base::initialiser(temps);
  nb_compo_ = pb_dist_->get_champ(le_nom()).valeurs().line_size();
  valeurs_.resize(0, nb_compo_);
  pb_loc_->domaine().creer_tableau_elements(valeurs_);
  is_initialized_ = true;
  return ok;
}

void Champ_Fonc_Interp::mettre_a_jour(double t)
{
  Champ_Fonc_P0_base::mettre_a_jour(t);
  if (!is_initialized_) return;

  MEDCoupling::MEDCouplingRemapper *rmp = pb_loc_->domaine_dis().domaine().get_remapper(pb_dist_->domaine_dis().domaine());
  const std::vector<std::map<int, double>>& mat_rmp = rmp->getCrudeMatrix();

  if (false) rmp->PrintMatrix(mat_rmp);

  valeurs() = 0.0;
  const DoubleTab& distant_values = pb_dist_->get_champ(le_nom()).valeurs();

  /* codage du transfert en IntensiveMaximum */
  for (int e = 0; e < (int)mat_rmp.size(); e++)
    {
      double s = 0.0;
      for (const auto& i_coef: mat_rmp[e])
        {
          s += i_coef.second;
          for (int n = 0; n < nb_compo_; n++)
            valeurs()(e, n) += i_coef.second * distant_values(i_coef.first, n);
        }
      for (int n = 0; n < nb_compo_; n++)
        valeurs()(e, n) /= s;
    }
}
