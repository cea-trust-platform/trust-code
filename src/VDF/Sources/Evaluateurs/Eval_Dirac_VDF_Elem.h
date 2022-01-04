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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Eval_Dirac_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Sources/Evaluateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Dirac_VDF_Elem_included
#define Eval_Dirac_VDF_Elem_included

#include <Evaluateur_Source_VDF_Elem.h>
#include <Ref_Champ_Don.h>
#include <Champ_Don.h>
#include <Zone.h>

class Eval_Dirac_VDF_Elem: public Evaluateur_Source_VDF_Elem
{
public:
  Eval_Dirac_VDF_Elem() : puissance(-100.), nb_dirac(-1.) { }
  virtual ~Eval_Dirac_VDF_Elem() { }
  template <typename Type_Double> void calculer_terme_source(const int , Type_Double& source) const;
  inline void associer_nb_elem_dirac(int n) { nb_dirac = 1./n; }
  inline void associer_champs(const Champ_Don& );
  inline void mettre_a_jour();

  DoubleVect le_point;
protected:

  REF(Champ_Don) la_puissance;
  REF(Zone) ma_zone;
  double puissance, nb_dirac;
};

inline void Eval_Dirac_VDF_Elem::associer_champs(const Champ_Don& Q)
{
  la_puissance = Q;
  puissance = Q(0);
}

inline void Eval_Dirac_VDF_Elem::mettre_a_jour()
{
  puissance = la_puissance.valeur()(0);
  ma_zone = la_zone.valeur().zone();
}

template <typename Type_Double>
void Eval_Dirac_VDF_Elem::calculer_terme_source(const int num_elem, Type_Double& source) const
{
  const int size = source.size_array(), test = ma_zone.valeur().type_elem().contient(le_point,num_elem);
  for (int i = 0; i < size; i++) source(i) = (test == 1) ? nb_dirac*puissance : 0.;
}

#endif /* Eval_Dirac_VDF_Elem_included */
