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

#ifndef Op_Conv_VEF_base_included
#define Op_Conv_VEF_base_included

#include <Operateur_Conv.h>
#include <Domaine_Cl_VEF.h>
#include <Domaine_VEF.h>
#include <Op_VEF_Face.h>
#include <Milieu_base.h>
#include <Dirichlet.h>
#include <TRUST_Ref.h>
#include <Domaine.h>

class Champ_Inc_base;

/*! @brief class Op_Conv_VEF_base
 *
 *  Classe de base des operateurs de convection VEF
 *
 *
 */
class Op_Conv_VEF_base : public Operateur_Conv_base, public Op_VEF_Face
{
  Declare_base(Op_Conv_VEF_base);
public:
  void associer_vitesse(const Champ_base& ) override;
  inline const Champ_Inc_base& vitesse() const { return vitesse_.valeur(); }
  inline Champ_Inc_base& vitesse() { return vitesse_.valeur(); }
  void associer(const Domaine_dis& , const Domaine_Cl_dis& ,const Champ_Inc& ) override;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  void abortTimeStep() override;
  double calculer_dt_stab() const override ;
  void calculer_dt_local(DoubleTab&) const override ; //Local time step calculation
  void calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const override;
  Motcle get_localisation_pour_post(const Nom& option) const override;

  virtual void remplir_fluent(DoubleVect& ) const;
  int impr(Sortie& os) const override;
  void associer_domaine_cl_dis(const Domaine_Cl_dis_base&) override;
  int  phi_u_transportant(const Equation_base& eq) const;

protected:
  REF(Domaine_VEF) le_dom_vef;
  REF(Domaine_Cl_VEF) la_zcl_vef;
  REF(Champ_Inc_base) vitesse_;

  mutable DoubleVect fluent;           // tableau qui sert pour le calcul du pas
  // Les jeton pour la permmutation des schemas de convection
  mutable int jeton = -1;
  mutable int roue = -1;
};

#endif
