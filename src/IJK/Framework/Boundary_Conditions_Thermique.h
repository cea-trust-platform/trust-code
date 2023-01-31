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

#ifndef Boundary_Conditions_Thermique_included
#define Boundary_Conditions_Thermique_included

#include <Objet_U.h>

/*! @brief : class Boundary_Conditions_Thermique
 *
 *  <Description of class Boundary_Conditions_Thermique>
 *
 */
class Boundary_Conditions_Thermique : public Objet_U
{
  Declare_instanciable( Boundary_Conditions_Thermique ) ;
public :
  enum BCType { Paroi_Temperature_imposee = 0, Paroi_Flux_impose = 1, Perio=2 };
  BCType get_bctype_k_min() const { return (BCType) bctype_kmin_; }
  BCType get_bctype_k_max() const { return (BCType) bctype_kmax_; }
  double get_temperature_kmax() const { return temperature_imposee_kmax_; }
  double get_temperature_kmin() const { return temperature_imposee_kmin_; }
  double get_flux_kmax() const { return flux_impose_kmax_; }
  double get_flux_kmin() const { return flux_impose_kmin_; }

protected :
  int bctype_kmin_, bctype_kmax_;
  double temperature_imposee_kmin_, temperature_imposee_kmax_;
  double flux_impose_kmin_, flux_impose_kmax_;
};

#endif /* Boundary_Conditions_Thermique_included */

