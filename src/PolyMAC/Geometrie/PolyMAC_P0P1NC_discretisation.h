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

#ifndef PolyMAC_P0P1NC_discretisation_included
#define PolyMAC_P0P1NC_discretisation_included

#include <PolyMAC_discretisation.h>

class PolyMAC_P0P1NC_discretisation: public PolyMAC_discretisation
{
  Declare_instanciable(PolyMAC_P0P1NC_discretisation);
public:

  using PolyMAC_discretisation::discretiser_champ;

  void discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, int nb_pas_dt, double temps, OWN_PTR(Champ_Inc_base)& champ,
                         const Nom& sous_type = NOM_VIDE) const override;

  void residu(const Domaine_dis_base& z, const Champ_Inc_base& ch_inco, OWN_PTR(Champ_Fonc_base)& champ) const override;

  void y_plus(const Domaine_dis_base& z, const Domaine_Cl_dis_base&, const Champ_Inc_base& vitesse, OWN_PTR(Champ_Fonc_base)& ch) const override;
  void grad_u(const Domaine_dis_base& z, const Domaine_Cl_dis_base& zcl, const Champ_Inc_base& ch_vitesse, OWN_PTR(Champ_Fonc_base)& ch) const override;

  inline type_calcul_du_residu codage_du_calcul_du_residu() const override { return VIA_AJOUTER; }

  Nom get_name_of_type_for(const Nom& class_operateur, const Nom& type_operateur, const Equation_base& eqn, const OBS_PTR(Champ_base) &champ_sup) const override;

  bool is_polymac_p0p1nc() const override { return true; }
  bool is_polymac() const override { return false; } // attention heritage !

private:
  void discretiser_champ_fonc_don(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& nom, const Noms& unite, int nb_comp, double temps, Objet_U& champ) const override;
};

#endif /* PolyMAC_P0P1NC_discretisation_included */
