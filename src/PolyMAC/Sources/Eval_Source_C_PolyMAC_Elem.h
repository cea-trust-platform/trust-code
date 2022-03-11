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
// File:        Eval_Source_C_PolyMAC_Elem.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Source_C_PolyMAC_Elem_included
#define Eval_Source_C_PolyMAC_Elem_included


#include <Evaluateur_Source_PolyMAC_Elem.h>
#include <Ref_Champ_Don.h>
#include <TRUSTTab.h>
#include <Champ_Uniforme.h>
#include <Champ_Don.h>

////////////////////////////////////////////////////////////////////////////
//
//  CLASS Eval_Source_C_PolyMAC_Elem
//
////////////////////////////////////////////////////////////////////////////

class Eval_Source_C_PolyMAC_Elem : public Evaluateur_Source_PolyMAC_Elem
{

public:

  inline Eval_Source_C_PolyMAC_Elem();
  void associer_champs(const Champ_Don& );
  void mettre_a_jour( ) override;
  inline double calculer_terme_source(int ) const override;
  inline void calculer_terme_source(int , DoubleVect& ) const override;

protected:

  REF(Champ_Don) la_source_constituant;
  DoubleTab source_constituant;
};


//
//   Fonctions inline de la classe Eval_Puiss_Th_Uniforme_PolyMAC_Elem
//

inline Eval_Source_C_PolyMAC_Elem::Eval_Source_C_PolyMAC_Elem() {}

inline double Eval_Source_C_PolyMAC_Elem::calculer_terme_source(int num_elem) const
{
  if (sub_type(Champ_Uniforme,la_source_constituant.valeur().valeur()))
    return source_constituant(0,0)*volumes(num_elem)*porosite_vol(num_elem);
  else if (source_constituant.nb_dim()==2)
    return source_constituant(num_elem,0)*volumes(num_elem)*porosite_vol(num_elem);
  else
    return source_constituant(num_elem)*volumes(num_elem)*porosite_vol(num_elem);
}

inline void Eval_Source_C_PolyMAC_Elem::calculer_terme_source(int num_elem, DoubleVect& source) const
{
  int size=source.size();
  if (sub_type(Champ_Uniforme,la_source_constituant.valeur().valeur()))
    for (int i=0; i<size; i++)
      source(i) = source_constituant(0,i)*volumes(num_elem)*porosite_vol(num_elem);
  else
    for (int i=0; i<size; i++)
      source(i) = source_constituant(num_elem,i)*volumes(num_elem)*porosite_vol(num_elem);
}

#endif
