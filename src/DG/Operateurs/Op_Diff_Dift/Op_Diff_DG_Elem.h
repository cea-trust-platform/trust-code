/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Op_Diff_DG_Elem_included
#define Op_Diff_DG_Elem_included

#include <Op_Diff_DG_base.h>
class Matrice_Morse;

class Op_Diff_DG_Elem: public Op_Diff_DG_base
{
  Declare_instanciable( Op_Diff_DG_Elem );
public:
  DoubleTab& ajouter(const DoubleTab&, DoubleTab&) const override;
  virtual void calculer_flux_bord(const DoubleTab& inco) const = delete;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  void modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const override { }
  void completer() override;

  void dimensionner(Matrice_Morse& mat) const override;

  void dimensionner_termes_croises(Matrice_Morse&, const Probleme_base& autre_pb, int nl, int nc) const override;
  void ajouter_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, DoubleTab& resu) const override;
  void contribuer_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, Matrice_Morse& matrice) const override;
  void contribuer_au_second_membre(DoubleTab& resu ) const override;
protected:

};


#endif /* Op_Diff_DG_Elem_included */
