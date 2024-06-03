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

#ifndef Entree_fluide_temperature_imposee_H_included
#define Entree_fluide_temperature_imposee_H_included

#include <Dirichlet_entree_fluide_leaves.h>
#include <TRUST_Ref.h>

class Fluide_Dilatable_base;

/*! @brief classe Entree_fluide_temperature_imposee_H Cas particulier de la classe Dirichlet_entree_fluide pour une equation ayant l'enthalpie en inconnue
 *
 * @sa Dirichlet_entree_fluide
 */
class Entree_fluide_temperature_imposee_H  : public Entree_fluide_temperature_imposee
{
  Declare_instanciable(Entree_fluide_temperature_imposee_H);
public :
  void completer() override;
  double val_imp(int i) const override;
  double val_imp(int i, int j) const override;

protected :
  REF(Fluide_Dilatable_base) le_fluide;
};

#endif /* Entree_fluide_temperature_imposee_H_included */
