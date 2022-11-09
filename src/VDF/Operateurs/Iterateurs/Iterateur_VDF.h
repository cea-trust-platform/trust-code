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

#ifndef Iterateur_VDF_included
#define Iterateur_VDF_included

#include <Iterateur_VDF_base.h>

Declare_deriv(Iterateur_VDF_base);

class Iterateur_VDF : public DERIV(Iterateur_VDF_base)
{
  Declare_instanciable(Iterateur_VDF);
public:
  Iterateur_VDF(const Iterateur_VDF_base& Opb) : DERIV(Iterateur_VDF_base)() { DERIV(Iterateur_VDF_base)::operator=(Opb); }

  inline int impr(Sortie& os) const { return valeur().impr(os); }
  inline void completer_() { valeur().completer_(); }
  inline void associer(const Zone_VDF& zvdf, const Zone_Cl_VDF& zcl_vdf, const Operateur_base& op) { valeur().associer(zvdf,zcl_vdf,op); }
  inline void ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice) const { valeur().ajouter_contribution(inco, matrice); }
  inline Evaluateur_VDF& evaluateur() { return valeur().evaluateur(); }
  inline const Evaluateur_VDF& evaluateur() const { return valeur().evaluateur(); }
  inline const Zone_VDF& zone() const { return valeur().zone(); }
  inline const Zone_Cl_VDF& zone_Cl() const { return valeur().zone_Cl(); }
  inline DoubleTab& ajouter(const DoubleTab& inco, DoubleTab& resu) const { return valeur().ajouter(inco,resu); }
  inline DoubleTab& calculer(const DoubleTab& inco, DoubleTab& resu) const { return valeur().calculer(inco, resu); }
  virtual void ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const { valeur().ajouter_blocs(mats, secmem, semi_impl); }
  inline void ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>& f2e) const
  {
    return valeur().ajouter_contribution_autre_pb(inco, matrice, la_cl, f2e);
  }

protected:
  inline Type_Cl_VDF type_cl(const Cond_lim& cl) const { return valeur().type_cl(cl); }
};

#endif /* Iterateur_VDF_included */
