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

#ifndef Op_Grad_EF_included
#define Op_Grad_EF_included

#include <Operateur_Grad.h>
#include <Domaine_Cl_EF.h>
#include <Domaine_EF.h>
#include <TRUST_Ref.h>


/*! @brief class Op_Grad_EF
 *
 *   Cette classe represente l'operateur de gradient. La discretisation est EF
 *   On calcule le gradient d'un champ_P0_EF (la pression)
 *
 * @sa Operateur_Grad_base
 */

class Op_Grad_EF : public Operateur_Grad_base
{

  Declare_instanciable(Op_Grad_EF);

public:

  void associer(const Domaine_dis_base& , const Domaine_Cl_dis_base&,const Champ_Inc_base&) override;
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const override;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  int impr(Sortie& os) const override;
  void mettre_a_jour(double) override;
  void calculer_flux_bords() const override;

protected:
  void ajouter_bord(DoubleTab& resu,const Domaine_EF& domaine_EF,const  Domaine_Cl_EF& domaine_Cl_EF) const;

  OBS_PTR(Domaine_EF) le_dom_EF;
  OBS_PTR(Domaine_Cl_EF) la_zcl_EF;
  OWN_PTR(Champ_Don_base) int_P_bord_;

};

#endif
