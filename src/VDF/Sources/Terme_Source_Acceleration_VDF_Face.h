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


#ifndef Terme_Source_Acceleration_VDF_Face_included
#define Terme_Source_Acceleration_VDF_Face_included

#include <Terme_Source_Acceleration.h>
#include <TRUST_Ref.h>

class Domaine_Cl_VDF;
class Domaine_VDF;

/*! @brief Terme source d'acceleration specialise pour la discretisation VDF
 *
 * @sa Terme_Source_Acceleration
 */

class Terme_Source_Acceleration_VDF_Face : public Terme_Source_Acceleration
{
  Declare_instanciable(Terme_Source_Acceleration_VDF_Face);

public:
  void        associer_champ_rho(const Champ_base& champ_rho) override;
  inline void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override {}
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override;
  inline int has_interface_blocs() const override
  {
    return 1;
  };

protected:
  const DoubleTab& calculer_vitesse_faces(DoubleTab& v_faces_stockage) const override;
  void associer_domaines(const Domaine_dis_base& ,const Domaine_Cl_dis_base& ) override;

private:
  // References remplies par associer_champ_rho
  OBS_PTR(Domaine_VDF)    le_dom_VDF_;
  OBS_PTR(Domaine_Cl_VDF) le_dom_Cl_VDF_;
  // Reference remplie par associer_champ_rho() et utilisee par ajouter()
  // La reference peut rester nulle (monophasique).
  OBS_PTR(Champ_base)  ref_rho_;
};

#endif
