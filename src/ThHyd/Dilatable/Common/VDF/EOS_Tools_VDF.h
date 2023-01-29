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

#ifndef EOS_Tools_VDF_included
#define EOS_Tools_VDF_included

#include <EOS_Tools_base.h>
#include <TRUST_Ref.h>
#include <TRUSTTab.h>

class Fluide_Dilatable_base;
class Domaine_Cl_dis;
class Domaine_VDF;

/*! @brief classe EOS_Tools_VDF Cette classe et specifique a discretisation de type VDF.
 *
 * @sa Fluide_Dilatable_base EOS_Tools_base
 */

class EOS_Tools_VDF: public EOS_Tools_base
{
  Declare_instanciable(EOS_Tools_VDF);
public :
  const DoubleTab& rho_discvit() const override;
  void associer_domaines(const Domaine_dis&,const Domaine_Cl_dis&) override;
  void divu_discvit(const DoubleTab&, DoubleTab&) override;
  void secmembre_divU_Z(DoubleTab& ) const override;
  void mettre_a_jour(double temps) override;
  void calculer_rho_face_np1(const DoubleTab& rho) override;
  double moyenne_vol(const DoubleTab&) const override ;
  inline const DoubleTab& rho_face_n() const override { return tab_rho_face; }
  inline const DoubleTab& rho_face_np1() const override { return tab_rho_face_np1; }
  inline const Fluide_Dilatable_base& le_fluide() const { return le_fluide_.valeur(); }

protected :
  REF2(Domaine_VDF) le_dom;
  REF2(Domaine_Cl_dis) le_dom_Cl;
  DoubleTab tab_rho_face, tab_rho_face_demi, tab_rho_face_np1;
};

#endif /* EOS_Tools_VDF_included */
