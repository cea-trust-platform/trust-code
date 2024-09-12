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

#ifndef Champ_Elem_PolyVEF_P0_included
#define Champ_Elem_PolyVEF_P0_included

#include <Champ_Elem_PolyMAC_P0.h>
#include <Operateur.h>

class Domaine_PolyVEF_P0;

/*! @brief : class Champ_Elem_PolyVEF_P0
 *
 *  Champ correspondant a une inconnue scalaire (type temperature ou pression)
 *  Degres de libertes : valeur aux elements + flux aux faces
 *
 */
class Champ_Elem_PolyVEF_P0: public Champ_Elem_PolyMAC_P0
{
  Declare_instanciable(Champ_Elem_PolyVEF_P0);
public:
  const Domaine_PolyVEF_P0& domaine_PolyVEF_P0() const;
  void init_auxiliary_variables() override;
  void init_grad(int full_stencil) const;      // Call to initialise the tables ; no updates necessary
  void calc_grad(int full_stencil) const;      // Call to calculate the tables ; updates necessary
};

#endif /* Champ_Elem_PolyVEF_P0_included */
