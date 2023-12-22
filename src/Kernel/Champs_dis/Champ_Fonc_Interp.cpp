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

#include <Discretisation_tools.h>
#include <Champ_Generique_base.h>
#include <Option_Interpolation.h>
#include <Champ_Fonc_Interp.h>
#include <TRUSTTab_parts.h>
#include <Comm_Group_MPI.h>
#include <Param.h>
#include <MEDLoader.hxx>

Implemente_instanciable_sans_destructeur(Champ_Fonc_Interp, "Champ_Fonc_Interp", Champ_Fonc_P0_base);
// XD Champ_Fonc_Interp champ_don_base Champ_Fonc_Interp 1 Field that is interpolated from a distant domain via MEDCoupling (remapper).

Champ_Fonc_Interp::~Champ_Fonc_Interp()
{
  if (dec_) dec_->release();
}

Sortie& Champ_Fonc_Interp::printOn(Sortie& os) const { return Champ_Fonc_P0_base::printOn(os); }

Entree& Champ_Fonc_Interp::readOn(Entree& is)
{
#ifdef MEDCOUPLING_
  using namespace MEDCoupling;

  Param param(que_suis_je());
  Nom nom_pb_loc, nom_pb_dist, nom_dom_loc, nom_dom_dist, nat, loc_trgt;
  param.ajouter("nom_champ", &nom_, Param::REQUIRED); // XD_ADD_P chaine Name of the field (for example: temperature).
  param.ajouter("pb_loc", &nom_pb_loc, Param::REQUIRED); // XD_ADD_P chaine Name of the local problem.
  param.ajouter("pb_dist", &nom_pb_dist, Param::REQUIRED); // XD_ADD_P chaine Name of the distant problem.
  param.ajouter("dom_loc", &nom_dom_loc); // XD_ADD_P chaine Name of the local domain.
  param.ajouter("dom_dist", &nom_dom_dist); // XD_ADD_P chaine Name of the distant domain.
  param.ajouter("default_value", &default_value_); // XD_ADD_P chaine Name of the distant domain.
  param.ajouter("sharing_algo", &sharing_algo_); // XD_ADD_P entier Setting the DEC sharing algo : 0,1,2
  param.ajouter("nature", &nat, Param::REQUIRED); // XD_ADD_P chaine Nature of the field (knowledge from MEDCoupling is required; IntensiveMaximum, IntensiveConservation, ...).
  param.ajouter("use_overlapdec", &use_dec_); // XD_ADD_P chaine Nature of the field (knowledge from MEDCoupling is required; IntensiveMaximum, IntensiveConservation, ...).
  param.lire_avec_accolades_depuis(is);

  pb_loc_ = ref_cast(Probleme_base, Interprete::objet(nom_pb_loc));
  pb_dist_ = ref_cast(Probleme_base, Interprete::objet(nom_pb_dist));
  if (nom_dom_loc == "??") dom_loc_ = pb_loc_->domaine();
  else dom_loc_ = ref_cast(Domaine, Interprete::objet(nom_dom_loc));
  if (nom_dom_dist == "??") dom_dist_ = pb_dist_->domaine();
  else dom_dist_ = ref_cast(Domaine, Interprete::objet(nom_dom_dist));

  is_elem_trgt_ = (pb_loc_->domaine_dis().valeur().que_suis_je() != "Domaine_VEF");

  if (nat == "IntensiveMaximum") nature_ = IntensiveMaximum;
  else if (nat == "IntensiveConservation") nature_ = IntensiveConservation;
  else if (nat == "ExtensiveMaximum") nature_ = ExtensiveMaximum;
  else if (nat == "ExtensiveConservation") nature_ = ExtensiveConservation;
  else
    {
      Cerr << "Champ_Front_Interp : wrong NatureOfField read : " << nat << finl;
      Process::exit();
    }

  if (use_dec_ != -123)
    Option_Interpolation::USE_DEC = use_dec_;
  else
    use_dec_= Option_Interpolation::USE_DEC;

  if (sharing_algo_ != -123)
    Option_Interpolation::SHARING_ALGO = sharing_algo_;
  else
    sharing_algo_ = Option_Interpolation::SHARING_ALGO;

  if (sharing_algo_ < 0 || sharing_algo_ > 2)
    {
      Cerr << "Champ_Front_Interp : wrong sharing_algo read : " << sharing_algo_ << finl;
      Cerr << "Available values are 0, 1 or 2 ! See the MEDCoupling Doc ! " << finl;
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
  const Champ_base& ch = pb_dist_->has_champ(le_nom()) ? pb_dist_->get_champ(le_nom()) : pb_dist_->get_champ_post(le_nom()).get_champ(espace_stockage_);

  nb_compo_ = ch.valeurs().line_size();

  if (is_elem_trgt_)
    {
      valeurs_.resize(0, nb_compo_);
      dom_loc_->creer_tableau_elements(valeurs_);
    }
  else
    {
      Domaine_VF& dvf = ref_cast(Domaine_VF, pb_loc_->domaine_dis().valeur());
      valeurs_.resize(dvf.nb_faces(), nb_compo_);
      dvf.creer_tableau_faces(valeurs_);
      valeurs_elem_.resize(0, nb_compo_);
      dom_loc_->creer_tableau_elements(valeurs_elem_);
    }

  is_initialized_ = true;
  return ok;
}

void Champ_Fonc_Interp::init_fields()
{
#ifdef MEDCOUPLING_
  using namespace MEDCoupling;

  local_field_ = MEDCouplingFieldDouble::New(ON_CELLS, ONE_TIME);
  local_field_->setName(le_nom().getString());
  local_array_ = DataArrayDouble::New();
  // XXX Pb with MEDCoupling : OvelapDEC seems to modify the meshes ...
  if (Process::nproc() > 1 && use_dec_)
    {
      MCAuto<MEDCouplingUMesh> msh_cpy = dom_loc_->get_mc_mesh()->deepCopy();
      local_field_->setMesh(msh_cpy);
    }
  else
    local_field_->setMesh(dom_loc_->get_mc_mesh());
  local_field_->setNature(nature_);

  distant_field_ = MEDCouplingFieldDouble::New(ON_CELLS, ONE_TIME);
  distant_array_ = DataArrayDouble::New();
  distant_field_->setName(le_nom().getString());
  // XXX Pb with MEDCoupling : OvelapDEC seems to modify the meshes ...
  if (Process::nproc() > 1 && use_dec_)
    {
      MCAuto<MEDCouplingUMesh> msh_cpy = dom_dist_->get_mc_mesh()->deepCopy();
      distant_field_->setMesh(msh_cpy);
    }
  else
    distant_field_->setMesh(dom_dist_->get_mc_mesh());
  distant_field_->setNature(nature_);
#endif
}

void Champ_Fonc_Interp::update_fields()
{
#ifdef MEDCOUPLING_
  using namespace MEDCoupling;

  const Champ_base& ch = pb_dist_->has_champ(le_nom()) ? pb_dist_->get_champ(le_nom()) : pb_dist_->get_champ_post(le_nom()).get_champ(espace_stockage_);

  const DoubleTab& distant_values = ch.valeurs();
  ConstDoubleTab_parts local_parts(valeurs()), distant_parts(distant_values);

  if (local_field_ == nullptr) init_fields();

  // Target Stuff
  if (is_elem_trgt_)
    local_array_->useExternalArrayWithRWAccess(valeurs().addr(), local_parts[0].dimension(0), nb_compo_);
  else
    local_array_->useExternalArrayWithRWAccess(valeurs_elem_.addr(), valeurs_elem_.dimension(0), nb_compo_);

  local_field_->setArray(local_array_);

  // Source Stuff
  if (pb_dist_->domaine_dis().valeur().que_suis_je() == "Domaine_VEF" &&  distant_values.dimension_tot(0) == ref_cast(Domaine_VF, pb_dist_->domaine_dis().valeur()).nb_faces_tot())
    {
      Cerr << finl << "ERROR in Champ_Fonc_Interp : in problem " << pb_loc_->le_nom() << ", the distant field is located at faces!" << finl;
      Cerr << "Use a postprocessing field located at elements instead of " << le_nom() << finl;
      Cerr << "In your case, try : " << le_nom() << "_elem_" << pb_dist_->domaine().le_nom() << finl;
      Process::exit();
    }

  distant_array_->useArray(distant_values.addr(), false, MEDCoupling::DeallocType::CPP_DEALLOC, distant_parts[0].dimension(0), nb_compo_);
  distant_field_->setArray(distant_array_);
#endif
}

void Champ_Fonc_Interp::mettre_a_jour(double t)
{
#ifdef MEDCOUPLING_
  using MEDCoupling::WriteField;
  Champ_Fonc_P0_base::mettre_a_jour(t);
  if (!is_initialized_) return;

  if (Process::nproc() > 1 && use_dec_)
    {
      std::set<int> pcs;
      for (int i=0; i<Process::nproc(); i++) pcs.insert(i);

      if (! is_dec_initialized_)
        dec_ = std::make_shared<MEDCoupling::OverlapDEC>(pcs, ref_cast(Comm_Group_MPI,PE_Groups::current_group()).get_trio_u_world());

      if (dom_loc_->get_mc_mesh() == nullptr) dom_loc_->build_mc_mesh();
      if (dom_dist_->get_mc_mesh() == nullptr) dom_dist_->build_mc_mesh();

      update_fields();
      if (! is_dec_initialized_)
        {
          dec_->setWorkSharingAlgo(sharing_algo_);
          dec_->attachSourceLocalField(distant_field_);
          dec_->attachTargetLocalField(local_field_);
          dec_->synchronize();
          is_dec_initialized_ = true;
        }
      dec_->sendRecvData(true);
    }
  else
    {
      MEDCouplingRemapper *rmp = dom_loc_->get_remapper(dom_dist_.valeur());
      update_fields();
      rmp->transfer(distant_field_, local_field_, default_value_);

      if (verbose_)
        {
          WriteField("/tmp/source" + le_nom().getString() + Nom(Process::me()).getString() + ".med", distant_field_, true);
          WriteField("/tmp/target" + le_nom().getString() + Nom(Process::me()).getString() + ".med", local_field_, true);
        }
    }

  // Put values on faces !
  if (!is_elem_trgt_)
    {
      valeurs_elem_.echange_espace_virtuel();
      const Domaine_VF& dvf = ref_cast(Domaine_VF, pb_loc_->domaine_dis().valeur());
      Discretisation_tools::cells_to_faces(dvf, valeurs_elem_, valeurs());
    }
#endif
}
