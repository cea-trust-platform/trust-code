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

#ifndef Eval_Puiss_Neutr_VDF_Elem_included
#define Eval_Puiss_Neutr_VDF_Elem_included

#include <Evaluateur_Source_Elem.h>
#include <TRUST_Ref.h>

class Champ_Uniforme;
class Champ_Don;

class Eval_Puiss_Neutr_VDF_Elem: public Evaluateur_Source_Elem
{
public:
  Eval_Puiss_Neutr_VDF_Elem() : puissance(-123.) { }
  void associer_champs(const Champ_Don& );
  void associer_repartition(const Nom& fxyz, const Nom& nom_ssz);
  void mettre_a_jour() override;
  void completer() override;
  template <typename Type_Double> void calculer_terme_source(const int , Type_Double& ) const;

protected:
  REF(Champ_Uniforme) la_puissance;
  double puissance;
  DoubleTab rep;
  Nom fxyz;
};

template <typename Type_Double>
void Eval_Puiss_Neutr_VDF_Elem::calculer_terme_source(const int num_elem, Type_Double& source) const
{
  const int size = source.size_array();
  assert(size == 1);
  for (int i = 0; i < size; i++) source[i] = rep(num_elem)*puissance*volumes(num_elem)*porosite_vol(num_elem);
}

#endif /* Eval_Puiss_Neutr_VDF_Elem_included */
