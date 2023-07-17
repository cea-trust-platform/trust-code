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

#ifndef Op_Rot_VEFP1B_included
#define Op_Rot_VEFP1B_included

#include <Champ_P1_isoP1Bulle.h>
#include <Op_Grad_VEF_Face.h>
#include <Operateur_base.h>
#include <TRUST_Ref.h>

class Domaine_VEF;
class Domaine_VEF;

class Op_Rot_VEFP1B : public Operateur_base
{
  Declare_instanciable(Op_Rot_VEFP1B);
public:
  void associer( const Domaine_dis&, const Domaine_Cl_dis&, const Champ_Inc&) override;
  DoubleTab& calculer( const DoubleTab&, DoubleTab&) const override;
  DoubleTab& ajouter( const DoubleTab&, DoubleTab&) const override;

  //Methode pour rendre le vecteur normal a la "face" de l'element "elem"
  DoubleTab vecteur_normal(const int face, const int elem) const;
  const Domaine_VEF& domaine_Vef() const;

  void associer_coins(const ArrOfInt&);

protected:
  ArrOfInt coins;
  REF(Domaine_VEF) le_dom_vef;
  REF(Domaine_Cl_VEF) la_zcl_vef;
};

#endif
