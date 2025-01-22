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

#ifndef Iterateur_VDF_base_included
#define Iterateur_VDF_base_included

#include <CL_Types_include.h>
#include <Operateur_base.h>
#include <Evaluateur_VDF.h>
#include <Champ_Face_VDF.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Domaine_Cl_VDF.h>

#include <Domaine_VDF.h>
#include <Milieu_base.h>
#include <TRUSTTrav.h>
#include <TRUST_Ref.h>

class Champ_Inc_base;
class Operateur_base;
class Champ_base;
class Matrice_Morse;

enum class Type_Operateur { Op_CONV_ELEM , Op_CONV_FACE , Op_DIFF_ELEM , Op_DIFT_ELEM , Op_DIFF_FACE , Op_DIFT_FACE , Op_DIFT_MULTIPHASE_FACE , Op_DIFT_MULTIPHASE_ELEM } ; // ne touche pas !

enum Type_Cl_VDF
{
  navier,  // Symetrie, frottement paroi
  sortie_libre, // Neumann_sortie_libre ou derivees
  entree_fluide, // Dirichlet_entree_fluide
  paroi_fixe, // Dirichlet_paroi_fixe
  paroi_defilante, // Dirichlet_paroi_defilante
  paroi_scalaire_impose, // Scalaire_impose_paroi ou derivees
  paroi_dirichlet_loi_paroi, // Dirichlet_loi_paroi pour turbulence
  paroi_adiabatique, // Neumann_paroi_adiabatique ou derivees
  paroi, // Neumann_paroi
  echange_externe_impose, // Echange_externe_impose
  echange_externe_radiatif, // Echange_externe_radiatif
  echange_global_impose, // Echange_global_impose
  periodique, // periodique
  nouvelle_Cl_VDF // Nouvelle_Cl_VDF
};

class Iterateur_VDF_base : public Objet_U
{
  Declare_base(Iterateur_VDF_base);
public:
  void associer(const Domaine_VDF&, const Domaine_Cl_VDF&, const Operateur_base&);
  void associer_domaine_cl_dis(const Domaine_Cl_dis_base&);
  void associer_champ_convecte_ou_inc(const Champ_Inc_base& ch, const Champ_base* chv, bool use=false);
  virtual void ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const=0;
  virtual void ajouter_contribution_autre_pb(const DoubleTab& , Matrice_Morse& , const Cond_lim& , std::map<int, std::pair<int, int>>&) const { /* ne fait rien en general */ }
  virtual void contribuer_au_second_membre(DoubleTab& ) const
  {
    Cerr << "Iterateur_VDF_base::" << __func__ << " should not be called !" << finl;
    Process::exit();
  }

  virtual void completer_()=0;
  virtual int impr(Sortie& os) const=0;
  virtual Evaluateur_VDF& evaluateur() =0;
  virtual const Evaluateur_VDF& evaluateur() const=0;

  inline const Domaine_VDF& domaine() const { return le_dom.valeur(); }
  inline const Domaine_Cl_VDF& domaine_Cl() const { return la_zcl.valeur(); }
  inline Type_Cl_VDF type_cl(const Cond_lim&) const;
  inline const bool& is_convective_op() const { return is_conv_op_; }
  inline const bool& is_pb_multiphase() const { return is_pb_multi; }
  void set_name_champ_inco(const std::string& nom) { nom_ch_inco_ = nom; }
  void set_incompressible(const int flag) { incompressible_ = flag; }
  inline const int& is_incompressible() const { return incompressible_; }

  inline void set_convective_op_pb_type(const bool ty_op, const bool ty_pb)
  {
    is_conv_op_ = ty_op;
    is_pb_multi = ty_pb;
  }

  inline void set_multiscalar_diff(bool m) { multiscalar_diff_ = m; }

  virtual DoubleTab& calculer(const DoubleTab& inco, DoubleTab& resu) const final
  {
    operator_egal(resu, 0., VECT_REAL_ITEMS);
    return ajouter(inco,resu);
  }

  virtual DoubleTab& ajouter(const DoubleTab& inco, DoubleTab& secmem) const final
  {
    ajouter_blocs({}, secmem, {{ op_base->equation().inconnue().le_nom().getString(), inco }});
    return secmem;
  }

  virtual void ajouter_contribution(const DoubleTab& inco, Matrice_Morse& m) const final
  {
    DoubleTrav secmem(inco); //on va le jeter
    ajouter_blocs({{ op_base->equation().inconnue().le_nom().getString(), &m }}, secmem, {});
  }

protected:
  OBS_PTR(Domaine_VDF) le_dom;
  OBS_PTR(Domaine_Cl_VDF) la_zcl;
  OBS_PTR(Operateur_base) op_base;
  OBS_PTR(Champ_Inc_base) le_champ_convecte_ou_inc;
  OBS_PTR(Champ_base) le_ch_v;
  std::string nom_ch_inco_;
  bool is_conv_op_ = false, is_pb_multi = false, use_base_val_b_ = false, multiscalar_diff_ = false;
  int incompressible_ = 1;
};

inline Type_Cl_VDF Iterateur_VDF_base::type_cl(const Cond_lim& la_cl) const
{
  Type_Cl_VDF retour = nouvelle_Cl_VDF;

  if (sub_type(Navier, la_cl.valeur())) retour = navier;
  else if (sub_type(Neumann_sortie_libre, la_cl.valeur())) retour = sortie_libre;
  else if (sub_type(Dirichlet_entree_fluide, la_cl.valeur())) retour = entree_fluide;
  else if (sub_type(Dirichlet_paroi_fixe, la_cl.valeur())) retour = paroi_fixe;
  else if (sub_type(Dirichlet_paroi_defilante, la_cl.valeur())) retour = paroi_defilante;
  else if (sub_type(Scalaire_impose_paroi, la_cl.valeur())) retour = paroi_scalaire_impose;
  else if (sub_type(Dirichlet_loi_paroi, la_cl.valeur())) retour = paroi_dirichlet_loi_paroi;
  else if (sub_type(Neumann_paroi_adiabatique, la_cl.valeur())) retour = paroi_adiabatique;
  else if (sub_type(Neumann_paroi, la_cl.valeur())) retour = paroi;
  else if (sub_type(Echange_externe_impose, la_cl.valeur())) retour = echange_externe_impose;
  else if (sub_type(Echange_externe_radiatif, la_cl.valeur())) retour = echange_externe_radiatif;
  else if (sub_type(Echange_global_impose, la_cl.valeur())) retour = echange_global_impose;
  else if (sub_type(Periodique, la_cl.valeur())) retour = periodique;

  return retour;
}

#endif /* Iterateur_VDF_base_included */
