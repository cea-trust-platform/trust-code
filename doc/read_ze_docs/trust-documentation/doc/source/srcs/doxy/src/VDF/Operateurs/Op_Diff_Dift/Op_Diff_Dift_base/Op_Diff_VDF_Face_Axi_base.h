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

#ifndef Op_Diff_VDF_Face_Axi_base_included
#define Op_Diff_VDF_Face_Axi_base_included

#include <Op_Diff_VDF_Face_base.h>
#include <TRUST_Ref.h>

class Champ_Face_VDF;

class Op_Diff_VDF_Face_Axi_base : public Op_Diff_VDF_Face_base
{
  Declare_base(Op_Diff_VDF_Face_Axi_base);
public:
  double calculer_dt_stab() const override;
  void associer(const Domaine_dis& , const Domaine_Cl_dis& , const Champ_Inc& ) override;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;

  inline void mettre_a_jour(double temps) override { }
  inline void contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const override { ajouter_contribution(inco, matrice); }
  inline void contribuer_au_second_membre(DoubleTab& resu) const override { contribue_au_second_membre(resu); }
  inline void dimensionner(Matrice_Morse& matrice) const override { Op_VDF_Face::dimensionner(le_dom_vdf.valeur(), la_zcl_vdf.valeur(), matrice); }
  inline void modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const override { Op_VDF_Face::modifier_pour_Cl( le_dom_vdf.valeur(), la_zcl_vdf.valeur(), matrice,  secmem); }

protected:
  static constexpr double deux_pi = M_PI*2.0;
  REF(Champ_Face_VDF) inconnue;
  REF(Domaine_VDF) le_dom_vdf;
  REF(Domaine_Cl_VDF) la_zcl_vdf;
  IntVect orientation, type_arete_bord;
  IntTab Qdm, face_voisins, elem_faces;
  DoubleVect surface, volumes_entrelaces, porosite;
  DoubleTab xp, xv;

  virtual double nu_(const int ) const = 0;
  virtual double nu_mean_2_pts_(const int , const int ) const = 0;
  virtual double nu_mean_4_pts_(const int , const int ) const = 0;

private:
  DoubleTab& ajouter(const DoubleTab& , DoubleTab& ) const override;
  void ajouter_elem(const DoubleTab& , DoubleTab& ) const;
  void ajouter_elem_3D(const DoubleTab& , DoubleTab& ) const;
  void ajouter_aretes_bords(const DoubleTab& , DoubleTab& ) const;
  void ajouter_aretes_mixtes_internes(const DoubleTab& , DoubleTab& ) const;

  void fill_coeff_matrice_morse(const int , const int , const double , Matrice_Morse& ) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_elem(const DoubleTab& , Matrice_Morse& ) const;
  void ajouter_contribution_elem_3D(Matrice_Morse& ) const;
  void ajouter_contribution_aretes_bords(Matrice_Morse& ) const;
  void ajouter_contribution_aretes_mixtes_internes(Matrice_Morse& ) const;

  void contribue_au_second_membre(DoubleTab& ) const;

  inline void not_implemented(const char * nom_funct) const
  {
    std::cerr << "Erreur dans : " << nom_funct << std::endl;
    std::cerr << "On ne sait pas traiter la diffusion d'un Champ_Face_VDF a plusieurs inconnues ! " << std::endl;
    throw;
  }
};

#endif /* Op_Diff_VDF_Face_Axi_base_included */
