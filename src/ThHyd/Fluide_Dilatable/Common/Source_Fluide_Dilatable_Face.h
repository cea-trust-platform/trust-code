/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Source_Fluide_Dilatable_VDF_Face.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Common
// Version:     /main/integration_fauchet_165/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Source_Fluide_Dilatable_VDF_Face_included
#define Source_Fluide_Dilatable_VDF_Face_included

#include <DoubleTrav.h>
#include <Milieu_base.h>
#include <Matrice_Morse.h>
#include <Modifier_pour_QC.h>
#include <Source_QC_QDM_Gen.h>

template <typename DERIVED_T>
class Source_Fluide_Dilatable_VDF_Face : public DERIVED_T
{
public:
  DoubleTab& ajouter(DoubleTab& ) const;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const ;
};

/* Class template specializations */
template <typename DERIVED_T>
DoubleTab& Source_Fluide_Dilatable_VDF_Face<DERIVED_T>::ajouter(DoubleTab& resu ) const
{
  DoubleTrav trav(resu);
  DERIVED_T::ajouter(trav);
  multiplier_par_rho_si_qc(trav,DERIVED_T::equation().milieu());
  resu+=trav;
  return resu;
}

template <typename DERIVED_T>
void Source_Fluide_Dilatable_VDF_Face<DERIVED_T>::contribuer_a_avec(const DoubleTab& present, Matrice_Morse& matrice) const
{

  return; /* on ne fait rien pour l'instant ... */

  DoubleTrav toto(present);
  ajouter(toto);
  int nb_comp=toto.dimension(1);
  for (int i=0; i < toto.dimension(0); i++)
    for (int comp=0; comp<nb_comp; comp++)
      {
        if (present(i,comp)!=0)
          {
            double coef=toto(i,comp)/present(i,comp);
            int i0=i*nb_comp+comp;
            matrice(i0,i0)-=coef;
          }
      }
}

#endif /* Source_Fluide_Dilatable_VDF_Face_included */
