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

#ifndef Eval_Echange_Himp_VDF_Elem_included
#define Eval_Echange_Himp_VDF_Elem_included

#include <Evaluateur_Source_Elem.h>
#include <Champ_Inc.h>
#include <TRUST_Ref.h>

class Eval_Echange_Himp_VDF_Elem: public Evaluateur_Source_Elem
{
public:
  Eval_Echange_Himp_VDF_Elem() : h_(-1.) { }
  template <typename Type_Double> void calculer_terme_source(const int , Type_Double& ) const;
  inline void associer_champs(const Champ_Inc& ,const Champ_Inc& ,const double );
  inline void mettre_a_jour() override;

protected:
  REF(Champ_Inc) T, T_voisin;
  DoubleTab Tcourant, Tvois;
  double h_;
};

inline void Eval_Echange_Himp_VDF_Elem::associer_champs(const Champ_Inc& tc, const Champ_Inc& tv, const double dh)
{
  this->h_ = dh;
  T = tc;
  T_voisin = tv;
  Tcourant.ref(T->valeurs());
  mettre_a_jour();
}

inline void Eval_Echange_Himp_VDF_Elem::mettre_a_jour()
{
  Tcourant.ref(T->valeurs());
  Tvois.ref(T_voisin->valeurs());
}

template <typename Type_Double>
void Eval_Echange_Himp_VDF_Elem::calculer_terme_source(const int num_elem, Type_Double& source) const
{
  const int size = source.size_array();
  for (int i = 0; i < size; i++) source[i] = h_*(Tvois(num_elem,i)-Tcourant(num_elem,i)) * volumes(num_elem)*porosite_vol(num_elem);
}

#endif /* Eval_Echange_Himp_VDF_Elem_included */
