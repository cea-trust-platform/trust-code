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

#ifndef Champ_Elem_PolyVEF_P0P1_included
#define Champ_Elem_PolyVEF_P0P1_included

#include <Champ_Elem_PolyMAC_P0.h>
#include <Operateur.h>

class Domaine_PolyVEF;

/*! @brief : class Champ_Elem_PolyVEF_P0P1
 *
 *  Champ correspondant a une inconnue scalaire (type temperature ou pression)
 *  Degres de libertes : valeur aux elements + flux aux faces
 *
 */
class Champ_Elem_PolyVEF_P0P1: public Champ_Elem_PolyMAC_P0
{
  Declare_instanciable(Champ_Elem_PolyVEF_P0P1);
public:
  const Domaine_PolyVEF& domaine_PolyVEF() const;
  void init_auxiliary_variables() override;
  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const override;
  DoubleTab& valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const override;
  DoubleVect& valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const override;
  const DoubleTab& alpha_es() const;
private:
  mutable DoubleTab alpha_es_;
};

#endif /* Champ_Elem_PolyVEF_P0P1_included */
