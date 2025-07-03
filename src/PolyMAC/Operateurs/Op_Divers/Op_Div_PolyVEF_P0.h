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

#ifndef Op_Div_PolyVEF_P0_included
#define Op_Div_PolyVEF_P0_included

#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_PolyVEF_P0.h>
#include <Operateur_Div.h>
#include <TRUST_Ref.h>

/*! @brief class Op_Div_PolyVEF_P0
 *
 *   Cette classe represente l'operateur de divergence
 *   La discretisation est PolyVEF_P0
 *   On calcule la divergence d'un champ_P1NC (la vitesse)
 *
 *
 * @sa Operateur_Div_base
 */

class Op_Div_PolyVEF_P0 : public Operateur_Div_base
{

  Declare_instanciable(Op_Div_PolyVEF_P0);

public:

  void associer(const Domaine_dis& , const Domaine_Cl_dis&,const Champ_Inc&) override;

  /* interface ajouter_blocs */
  int has_interface_blocs() const override
  {
    return 1;
  };
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  /* version etendue de ajouter_blocs pour pouvoir donner en argument le champ dont on prend la divergence */
  void ajouter_blocs_ext(const DoubleTab& vit, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const;

  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override
  {
    return ajouter_blocs_ext(equation().inconnue().valeurs(), matrices, secmem, semi_impl);
  }
  /* contrairement au cas usuel, ces methodes ne peuvent pas etre reimplementees a partir d'ajouter_blocs(),
     car elles sont souvent appelees avec un tableau inco arbitraire */
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const override;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;

  int impr(Sortie& os) const override;
  void volumique(DoubleTab& ) const override;

protected:

  REF(Domaine_PolyVEF_P0) le_dom_PolyVEF_P0;
  REF(Domaine_Cl_PolyMAC) le_dcl_PolyVEF_P0;
};

#endif
