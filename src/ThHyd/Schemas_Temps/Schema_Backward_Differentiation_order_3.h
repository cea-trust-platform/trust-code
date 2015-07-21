/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Schema_Backward_Differentiation_order_3.h
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Schema_Backward_Differentiation_order_3_included
#define Schema_Backward_Differentiation_order_3_included


#include <Schema_Backward_Differentiation_base.h>
class Probleme_Couple;
class Matrice_Base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Schema_Backward_Differentiation_order_3
//     Cette classe represente un schema de differentiation retrograde d'ordre 2 :
//     U(n+1) = 4/3 *U(n) - 1/3 U(n-1) + 2/3 * dt*(dU/dt)(n+1) dans le cas
//     ou le pas de temps est constant au cours du temps
// .SECTION voir aussi
//     Schema_Temps_base
//////////////////////////////////////////////////////////////////////////////
class Schema_Backward_Differentiation_order_3: public Schema_Backward_Differentiation_base
{

  Declare_instanciable(Schema_Backward_Differentiation_order_3);

public :

  int nb_valeurs_temporelles() const;
  int nb_valeurs_passees() const ;
  int nb_pas_dt_seuil() const ;

protected :

  void   compute_backward_differentiation_coefficients(double time_step, const DoubleTab& times) const ;
};

#endif
