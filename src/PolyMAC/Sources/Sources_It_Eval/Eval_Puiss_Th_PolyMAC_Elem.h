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

#ifndef Eval_Puiss_Th_PolyMAC_Elem_included
#define Eval_Puiss_Th_PolyMAC_Elem_included

#include <Evaluateur_Source_Elem.h>
#include <Champ_Uniforme.h>
#include <Equation_base.h>
#include <Champ_Don.h>
#include <TRUST_Ref.h>
#include <TRUSTTab.h>

class Eval_Puiss_Th_PolyMAC_Elem: public Evaluateur_Source_Elem
{
public:
  void mettre_a_jour() override { }
  inline void associer_champs(const Champ_Don&);

  template <typename Type_Double>
  inline void calculer_terme_source(const int, Type_Double&) const;

protected:
  REF2(Champ_Don) la_puissance;
  DoubleTab puissance;
};

inline void Eval_Puiss_Th_PolyMAC_Elem::associer_champs(const Champ_Don& Q)
{
  la_puissance = Q;
  puissance.ref(Q.valeurs());
}

template <typename Type_Double>
inline void Eval_Puiss_Th_PolyMAC_Elem::calculer_terme_source(const int e, Type_Double& S) const
{
  const int k = sub_type(Champ_Uniforme,la_puissance.valeur().valeur()) ? 0 : e, size = S.size_array();
  for (int i = 0; i < size; i++) S[i] = puissance(k, i) * volumes(e) * porosite_vol(e);
}

#endif /* Eval_Puiss_Th_PolyMAC_Elem_included */
