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

#ifndef Op_Conv_PolyMAC_base_included
#define Op_Conv_PolyMAC_base_included

#include <Operateur_Conv.h>
#include <TRUST_Ref.h>
#include <SFichier.h>

class Domaine_Cl_PolyMAC;
class Domaine_Poly_base;

/*! @brief class Op_Conv_PolyMAC_base
 *
 *  Classe de base des operateurs de convection VDF
 */

class Op_Conv_PolyMAC_base : public Operateur_Conv_base
{
  Declare_base(Op_Conv_PolyMAC_base);
public:
  void completer() override;
  double calculer_dt_stab() const override;
  inline DoubleTab& calculer(const DoubleTab& inco, DoubleTab& resu ) const override;
  int impr(Sortie& os) const override;
  void associer_domaine_cl_dis(const Domaine_Cl_dis_base&) override;
  void associer(const Domaine_dis& , const Domaine_Cl_dis& ,const Champ_Inc& ) override;
  void associer_vitesse(const Champ_base& ) override;

protected:
  REF2(Domaine_Poly_base) le_dom_poly_;
  REF2(Domaine_Cl_PolyMAC) la_zcl_poly_;
  REF(Champ_base) vitesse_;

  mutable SFichier Flux, Flux_moment, Flux_sum;
};

/*! @brief calcule la contribution de la convection, la range dans resu renvoie resu
 *
 */
inline DoubleTab& Op_Conv_PolyMAC_base::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu=0;
  return ajouter(inco,resu);
}

#endif /* Op_Conv_PolyMAC_base_included */
