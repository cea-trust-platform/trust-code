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

#ifndef Op_Conv_EF_base_included
#define Op_Conv_EF_base_included

#include <Operateur_Conv.h>
#include <Domaine_Cl_EF.h>
#include <Op_EF_base.h>
#include <Domaine_EF.h>
#include <TRUST_Ref.h>
#include <Domaine.h>

class Champ_Inc_base;

/*! @brief class Op_Conv_EF_base
 *
 *  Classe de base des operateurs de convection EF
 */
class Op_Conv_EF_base : public Operateur_Conv_base, public Op_EF_base
{

  Declare_base(Op_Conv_EF_base);

public:

  void associer_vitesse(const Champ_base& ) override;
  const Champ_Inc_base& vitesse() const;
  Champ_Inc_base& vitesse();
  void associer(const Domaine_dis_base& , const Domaine_Cl_dis_base& ,const Champ_Inc_base& ) override;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  double calculer_dt_stab() const override ;
  void calculer_pour_post(Champ_base& espace_stockage,const Nom& option, int) const override;

  virtual void remplir_fluent() const;
  int impr(Sortie& os) const override;
  void associer_domaine_cl_dis(const Domaine_Cl_dis_base&) override;
  int  phi_u_transportant(const Equation_base& eq) const;
  void completer() override;

protected:

  OBS_PTR(Domaine_EF) le_dom_EF;
  OBS_PTR(Domaine_Cl_EF) la_zcl_EF;
  OBS_PTR(Champ_Inc_base) vitesse_;

  mutable DoubleVect fluent_;           // tableau qui sert pour le calcul du pas
  //de temps de stabilite
};

#endif
