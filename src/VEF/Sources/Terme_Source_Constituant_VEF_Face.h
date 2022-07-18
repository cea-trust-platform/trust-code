/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Terme_Source_Constituant_VEF_Face_included
#define Terme_Source_Constituant_VEF_Face_included



#include <Terme_Source_Constituant.h>
#include <Terme_Source_VEF_base.h>
#include <Eval_Source_C_VEF_Face.h>
#include <Iterateur_Source_VEF.h>

declare_It_Sou_VEF_Face(Eval_Source_C_VEF_Face)

//.DESCRIPTION class Terme_Source_Constituant_VEF_Face
//
// Cette classe represente un terme source de l'equation de la concentration
//
//.SECTION voir aussi Terme_Source_Constituant, Terme_Source_VEF_base
//
class Terme_Source_Constituant_VEF_Face : public Terme_Source_Constituant,
  public Terme_Source_VEF_base
{
  Declare_instanciable_sans_constructeur(Terme_Source_Constituant_VEF_Face);

public:

  inline Terme_Source_Constituant_VEF_Face();
  void associer_zones(const Zone_dis&, const Zone_Cl_dis& ) override;
  void associer_pb(const Probleme_base& ) override;
  void mettre_a_jour(double temps) override
  {
    Terme_Source_Constituant::mettre_a_jour(temps);
  }
};


//
// Fonctions inline de la classe Terme_Source_Constituant_VEF_Face
//

inline Terme_Source_Constituant_VEF_Face::Terme_Source_Constituant_VEF_Face()
  : Terme_Source_Constituant(),Terme_Source_VEF_base(It_Sou_VEF_Face(Eval_Source_C_VEF_Face)())
{
}


#endif
