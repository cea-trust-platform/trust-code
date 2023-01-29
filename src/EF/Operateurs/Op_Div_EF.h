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

#ifndef Op_Div_EF_included
#define Op_Div_EF_included

#include <Operateur_Div.h>
#include <Domaine_EF.h>
#include <TRUST_Ref.h>

class Domaine_Cl_EF;

/*! @brief class Op_Div_EF
 *
 *   Cette classe represente l'operateur de divergence. La discretisation est EF
 *   On calcule la divergence d'un champ_P1NC (la vitesse)
 *
 *
 * @sa Operateur_Div_base
 */

class Op_Div_EF : public Operateur_Div_base
{

  Declare_instanciable(Op_Div_EF);

public:

  void associer(const Domaine_dis& , const Domaine_Cl_dis&,const Champ_Inc&) override;
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const override;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  int impr(Sortie& os) const override;
  void volumique(DoubleTab& ) const override;


protected:

  REF2(Domaine_EF) le_dom_EF;
  REF2(Domaine_Cl_EF) la_zcl_EF;

  DoubleVect porosite_face;
};

#endif
