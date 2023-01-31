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

#ifndef Eval_Forchheimer_VEF_Face_included
#define Eval_Forchheimer_VEF_Face_included

#include <Evaluateur_Source_VEF_Face.h>
#include <Modele_Permeabilite.h>
#include <TRUST_Ref.h>

class Champ_Inc;

class Eval_Forchheimer_VEF_Face: public Evaluateur_Source_VEF_Face
{
public:
  Eval_Forchheimer_VEF_Face() : Cf(1.), porosite(1.) { }
  Eval_Forchheimer_VEF_Face(const Eval_Forchheimer_VEF_Face& eval) : Evaluateur_Source_VEF_Face(eval),Cf(1.), porosite(1.) { }

  inline void mettre_a_jour( ) override { }

  template <typename Type_Double>
  inline void calculer_terme_source_standard(const int num_face, Type_Double& source) const { calculer_terme_source(num_face, source, volumes_entrelaces); }

  template <typename Type_Double>
  inline void calculer_terme_source_non_standard(const int num_face, Type_Double& source) const { calculer_terme_source(num_face, source, volumes_entrelaces_Cl); }

  inline void setCf(double c) { Cf = c; }
  inline void associer(const Champ_Inc& v) { vitesse = v; }

  Modele_Permeabilite modK;
  inline void setPorosite(double p) { porosite = p; }

private:
  template <typename Type_Double>
  inline void calculer_terme_source(int , Type_Double& , const DoubleVect&) const;

  double Cf, porosite;
  REF2(Champ_Inc) vitesse;
};

// Compute -Cf.psi.U.|U|/sqrt(K).dvol
template <typename Type_Double>
inline void Eval_Forchheimer_VEF_Face::calculer_terme_source(int num_face, Type_Double& source, const DoubleVect& volumes) const
{
  int size = source.size_array();
  for (int i = 0; i < size; i++)
    {
      double U = vitesse->valeurs()(num_face, i);
      source[i] = -Cf / sqrt(modK->getK(porosite)) * volumes[num_face] * porosite_surf[num_face] * std::fabs(U) * U;
    }
}

#endif /* Eval_Forchheimer_VEF_Face_included */
