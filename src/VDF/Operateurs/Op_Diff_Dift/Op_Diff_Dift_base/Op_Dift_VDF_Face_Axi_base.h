/****************************************************************************
* Copyright (c) 2022, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Dift_VDF_Face_Axi_base.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Diff_Dift/Op_Diff_Dift_base
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Dift_VDF_Face_Axi_base_included
#define Op_Dift_VDF_Face_Axi_base_included

#include <Op_Dift_VDF_Face_base.h>
#include <Ref_Mod_turb_hyd_base.h>
#include <Ref_Champ_Uniforme.h>
#include <Ref_Champ_Face.h>

class Op_Dift_VDF_Face_Axi_base : public Op_Dift_VDF_Face_base
{
  Declare_base(Op_Dift_VDF_Face_Axi_base);
public:
  double calculer_dt_stab() const override;
  void completer() override;
  void associer(const Zone_dis& , const Zone_Cl_dis& ,const Champ_Inc& ) override;
  void associer_modele_turbulence(const Mod_turb_hyd_base& );
  void mettre_a_jour(double ) override;
  void contribue_au_second_membre(DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;

  inline void contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const override { ajouter_contribution(inco, matrice); }
  inline void contribuer_au_second_membre(DoubleTab& resu) const override { contribue_au_second_membre(resu); }
  inline void associer_loipar(const Turbulence_paroi& ) { /* do nothing */}
  inline void associer_diffusivite_turbulente(const Champ_Fonc& visc_turb) { Op_Diff_Turbulent_base::associer_diffusivite_turbulente(visc_turb);}
  inline void dimensionner(Matrice_Morse& matrice) const override { Op_VDF_Face::dimensionner(la_zone_vdf.valeur(), la_zcl_vdf.valeur(), matrice); }
  inline void modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const override { Op_VDF_Face::modifier_pour_Cl( la_zone_vdf.valeur(), la_zcl_vdf.valeur(), matrice,  secmem); }

protected:
  REF(Mod_turb_hyd_base) le_modele_turbulence;
  REF(Champ_Face) inconnue;
  REF(Zone_VDF) la_zone_vdf;
  REF(Zone_Cl_VDF) la_zcl_vdf;
  IntVect orientation, type_arete_bord;
  IntTab Qdm, face_voisins, elem_faces;
  DoubleVect surface, volumes_entrelaces, porosite;
  DoubleTab xp, xv, tau_tan;

  virtual void mettre_a_jour_var(double ) const = 0;
  virtual bool is_VAR() const = 0;
  virtual double nu_(const int ) const = 0;
  virtual double nu_mean_2_pts_(const int , const int ) const = 0;
  virtual double nu_mean_4_pts_(const int , const int ) const = 0;
  virtual double nu_mean_4_pts_(const int , const int , const int , const int ) const = 0;

private:
  DoubleTab& ajouter(const DoubleTab& , DoubleTab& ) const override;
  void ajouter_elem(const DoubleVect& , const DoubleTab& , DoubleTab& ) const;
  void ajouter_elem_3D(const DoubleVect& , const DoubleTab& , DoubleTab& ) const;
  void ajouter_aretes_bords(const DoubleVect& , const DoubleTab& , DoubleTab& ) const;
  void fill_resu_aretes_mixtes(const int , const int , const int , const int , const double , const double , DoubleTab& ) const;
  void ajouter_aretes_mixtes(const DoubleTab& , DoubleTab& ) const;
  void fill_resu_aretes_internes(const int , const int , const int , const int , const double , const double , const double , const double , DoubleTab& ) const;
  void ajouter_aretes_internes(const DoubleVect& , const DoubleTab& , DoubleTab& ) const;

  void fill_coeff_matrice_morse(const int , const int , const double , Matrice_Morse& ) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_elem(const DoubleVect& , const DoubleTab& , Matrice_Morse& ) const;
  void ajouter_contribution_elem_3D(const DoubleVect& , const DoubleTab& , Matrice_Morse& ) const;
  void ajouter_contribution_aretes_bords(const DoubleVect& , const DoubleTab& , Matrice_Morse& ) const;
  void ajouter_contribution_aretes_mixtes(Matrice_Morse& ) const;
  void ajouter_contribution_aretes_internes(const DoubleVect& , Matrice_Morse& ) const;

  inline void not_implemented(const char * nom_funct) const
  {
    std::cerr << "Erreur dans : " << nom_funct << std::endl;
    std::cerr << "On ne sait pas traiter la diffusion d'un Champ_Face a plusieurs inconnues ! " << std::endl;
    throw;
  }
};

#endif /* Op_Dift_VDF_Face_Axi_base_included */
