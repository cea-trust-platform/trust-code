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
// File:        Source_Fluide_Dilatable_Face.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Sources
// Version:     /main/integration_fauchet_165/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Source_Fluide_Dilatable_Face_included
#define Source_Fluide_Dilatable_Face_included

#include <Modifier_pour_fluide_dilatable.h>
#include <Source_QC_QDM_Gen.h>
#include <Matrice_Morse.h>
#include <Milieu_base.h>
#include <TRUSTTrav.h>

template <typename DERIVED_T>
class Source_Fluide_Dilatable_Face
{
public:
  DoubleTab& ajouter_impl(DoubleTab& ) const;
  void contribuer_a_avec_impl(const DoubleTab&, Matrice_Morse&) const;
protected:
  DoubleTab& ajouter_mere_impl(DoubleTab& ) const;
  const Milieu_base& milieu_impl() const;
};

/*
 * CRTP PATTERN SUITE AU BRICOLAGE POUR QUE CHECK_SOURCES ET VERIFIE_PERE MARCHENT ... DESOLEE ...
 */

template <typename DERIVED_T>
DoubleTab& Source_Fluide_Dilatable_Face<DERIVED_T>::ajouter_mere_impl(DoubleTab& resu) const
{
  return static_cast<const DERIVED_T *>(this)->ajouter_mere(resu);
}

template <typename DERIVED_T>
const Milieu_base& Source_Fluide_Dilatable_Face<DERIVED_T>::milieu_impl() const
{
  return static_cast<const DERIVED_T *>(this)->equation().milieu();
}

/* Class template specializations */
template <typename DERIVED_T>
DoubleTab& Source_Fluide_Dilatable_Face<DERIVED_T>::ajouter_impl(DoubleTab& resu ) const
{
  DoubleTrav trav(resu);
  ajouter_mere_impl(trav);
  multiplier_par_rho_si_dilatable(trav,milieu_impl());
  resu+=trav;
  return resu;
}

template <typename DERIVED_T>
void Source_Fluide_Dilatable_Face<DERIVED_T>::contribuer_a_avec_impl(const DoubleTab& present, Matrice_Morse& matrice) const
{

  return; /* on ne fait rien pour l'instant ... */

  DoubleTrav toto(present);
  ajouter_impl(toto);
  int nb_comp=toto.dimension(1);
  for (int i=0; i < toto.dimension(0); i++)
    for (int comp=0; comp<nb_comp; comp++)
      {
        if (present(i,comp)!=0)
          {
            int i0 = i * nb_comp + comp;
            matrice(i0,i0) -= toto(i,comp) / present(i,comp);
          }
      }
}

#endif /* Source_Fluide_Dilatable_Face_included */
