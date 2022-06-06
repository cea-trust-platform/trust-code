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

#ifndef VEFPreP1B_included
#define VEFPreP1B_included

#include <VEF_discretisation.h>

class Domaine_dis;

class VEFPreP1B : public VEF_discretisation
{

  Declare_instanciable(VEFPreP1B);

public :
  //
  // Methodes surchargees de Discretisation_base
  //
  void discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                         Nature_du_champ nature,
                         const Noms& nom, const Noms& unite,
                         int nb_comp, int nb_pas_dt, double temps,
                         Champ_Inc& champ, const Nom& sous_type = nom_vide) const override;
  void discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                         Nature_du_champ nature,
                         const Noms& nom, const Noms& unite,
                         int nb_comp, double temps,
                         Champ_Fonc& champ) const override;
  void discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                         Nature_du_champ nature,
                         const Noms& nom, const Noms& unite,
                         int nb_comp, double temps,
                         Champ_Don& champ) const override;
  void zone_Cl_dis(Zone_dis& z,Zone_Cl_dis& zcl) const override;

  int get_P1Bulle() const
  {
    return P1Bulle_;
  };
  int get_alphaE() const
  {
    return alphaE_;
  };
  int get_alphaS() const
  {
    return alphaS_;
  };
  int get_alphaA() const
  {
    return alphaA_;
  };
  int get_modif_div_face_dirichlet() const
  {
    return modif_div_face_dirichlet_;
  };
  int get_cl_pression_sommet_faible() const
  {
    return cl_pression_sommet_faible_;
  };

private:
  void discretiser_champ_fonc_don(
    const Motcle& directive, const Zone_dis_base& z,
    Nature_du_champ nature,
    const Noms& nom, const Noms& unite,
    int nb_comp, double temps,
    Objet_U& champ) const;
  void discretiser(Domaine_dis&) const override;
private :
  int P1Bulle_;
  int alphaE_;
  int alphaS_;
  int alphaA_;
  int modif_div_face_dirichlet_;
  int cl_pression_sommet_faible_; // determine si les cl de pression sont imposees de facon faible ou forte -> voir divergence et assembleur, zcl
};


#endif



