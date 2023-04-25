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
#include <TRUSTTab_parts.h>
#include <Param.h>

Implemente_instanciable(Champ_Fonc_Interp, "Champ_Fonc_Interp", Champ_Fonc_P0_base);
// XD Champ_Fonc_Interp champ_don_base Champ_Fonc_Interp 1 Field that is interpolated from a distant domain via MEDCoupling (remapper).

Sortie& Champ_Fonc_Interp::printOn(Sortie& os) const { return Champ_Fonc_P0_base::printOn(os); }

Entree& Champ_Fonc_Interp::readOn(Entree& is)
{
#ifdef MEDCOUPLING_
  using namespace MEDCoupling;

  Param param(que_suis_je());
  Nom nom_pb_loc, nom_pb_dist, nat;
  param.ajouter("nom_champ", &nom_, Param::REQUIRED); // XD_ADD_P chaine Name of the field (for example: temperature).
  param.ajouter("pb_loc", &nom_pb_loc, Param::REQUIRED); // XD_ADD_P chaine Name of the local problem.
  param.ajouter("pb_dist", &nom_pb_dist, Param::REQUIRED); // XD_ADD_P chaine Name of the distant problem.
  param.ajouter("nature", &nat, Param::REQUIRED); // XD_ADD_P chaine Nature of the field (knowledge from MEDCoupling is required; IntensiveMaximum, IntensiveConservation, ...).
  param.lire_avec_accolades_depuis(is);

  pb_loc_ = ref_cast(Probleme_base,Interprete::objet(nom_pb_loc));
  pb_dist_ = ref_cast(Probleme_base,Interprete::objet(nom_pb_dist));

  if (nat == "IntensiveMaximum") nature_ = IntensiveMaximum;
  else if (nat == "IntensiveConservation") nature_ = IntensiveConservation;
  else if (nat == "ExtensiveMaximum") nature_ = ExtensiveMaximum;
  else if (nat == "ExtensiveConservation") nature_ = ExtensiveConservation;
  else
    {
      Cerr << "Champ_Front_Interp : wrong NatureOfField read : " << nat << finl;
      Process::exit();
    }
  return is;
#else
  Cerr << "Champ_Fonc_Interp::readOn should not be called since it requires a TRUST version compiled with MEDCoupling !" << finl;
  throw;
#endif
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

void Champ_Fonc_Interp::init_fields()
{
#ifdef MEDCOUPLING_
  using namespace MEDCoupling;
  Domaine& local_dom = pb_loc_->domaine_dis().domaine(), &distant_dom = pb_dist_->domaine_dis().domaine();

  local_field_ = MEDCouplingFieldDouble::New(ON_CELLS, ONE_TIME);
  local_array_ = DataArrayDouble::New();
  local_field_->setMesh(local_dom.get_mc_mesh());
  local_field_->setNature(nature_);

  distant_field_ = MEDCouplingFieldDouble::New(ON_CELLS, ONE_TIME);
  distant_array_ = DataArrayDouble::New();
  distant_field_->setMesh(distant_dom.get_mc_mesh());
  distant_field_->setNature(nature_);
#endif
}

void Champ_Fonc_Interp::update_fields()
{
#ifdef MEDCOUPLING_
  using namespace MEDCoupling;

  const DoubleTab& distant_values = pb_dist_->get_champ(le_nom()).valeurs();
  ConstDoubleTab_parts local_parts(valeurs()), distant_parts(distant_values);

  if (local_field_ == nullptr) init_fields();
  local_array_->useExternalArrayWithRWAccess(valeurs().addr(), local_parts[0].dimension(0), nb_compo_);
  local_field_->setArray(local_array_);

  distant_array_->useArray(distant_values.addr(), false, MEDCoupling::DeallocType::CPP_DEALLOC, distant_parts[0].dimension(0), nb_compo_);
  distant_field_->setArray(distant_array_);
#endif
}

void Champ_Fonc_Interp::mettre_a_jour(double t)
{
#ifdef MEDCOUPLING_
  Champ_Fonc_P0_base::mettre_a_jour(t);
  if (!is_initialized_) return;

  Domaine& local_dom = pb_loc_->domaine_dis().domaine(), &distant_dom = pb_dist_->domaine_dis().domaine();
  MEDCouplingRemapper *rmp = local_dom.get_remapper(distant_dom);

  update_fields();
  rmp->transfer(distant_field_, local_field_, 1e30);
#endif
}
