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

#ifndef Op_Dift_VEF_Face_Gen_included
#define Op_Dift_VEF_Face_Gen_included

enum class Type_Champ { SCALAIRE, VECTORIEL };

#include <Domaine_Cl_dis.h>
#include <Domaine_Cl_VEF.h>
#include <Domaine_VEF.h>
#include <TRUST_Ref.h>

template <typename DERIVED_T>
class Op_Dift_VEF_Face_Gen
{
public:
  void associer_gen(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis)
  {
    dom_vef = ref_cast(Domaine_VEF,domaine_dis.valeur());
    zcl_vef = ref_cast(Domaine_Cl_VEF,domaine_cl_dis.valeur());
  }

  // methodes pour l'implicite
  template <Type_Champ _TYPE_>
  void ajouter_contribution_bord_gen(const DoubleTab&, Matrice_Morse&, const DoubleTab&, const DoubleTab&, const DoubleVect&) const;

  template <Type_Champ _TYPE_>
  void ajouter_contribution_interne_gen(const DoubleTab&, Matrice_Morse&, const DoubleTab&, const DoubleTab&, const DoubleVect&) const;

private:
  REF(Domaine_VEF) dom_vef;
  REF(Domaine_Cl_VEF) zcl_vef;
};

#include <Op_Dift_VEF_Face_Gen.tpp>

#endif /* Op_Dift_VEF_Face_Gen_included */
