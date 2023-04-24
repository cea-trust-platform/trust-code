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

#ifndef Boundary_Conditions_included
#define Boundary_Conditions_included

#include <Objet_U.h>

class Boundary_Conditions : public Objet_U
{
  Declare_instanciable(Boundary_Conditions);
public:
  enum BCType { Paroi = 0, Symetrie = 1, Perio = 2, Mixte_shear = 3 };
  BCType get_bctype_k_min() const { return (BCType) bctype_kmin_; }
  BCType get_bctype_k_max() const { return (BCType) bctype_kmax_; }

  void set_vx_kmin(double value) { vxkmin_ = value; }
  void set_vx_kmax(double value) { vxkmax_ = value; }
  void set_dU_perio(double value) { dU_perio_ = value; }

  double get_vx_kmin() { return vxkmin_; }
  double get_vx_kmax() { return vxkmax_; }
  double get_dU_perio(int fluctuations=0) { if (!fluctuations) {return dU_perio_;} else {return 0.;}}
  double get_t0_shear() { return t0_shear_; }
  int get_defilement() { return defilement_; }
  int get_resolution_u_prime_() { return resolution_u_prime_; }

protected:
  int bctype_kmin_, bctype_kmax_;
  double vxkmin_, vxkmax_;
  double dU_perio_;
  double t0_shear_;
  int defilement_;
  int resolution_u_prime_;
};

#endif /* Boundary_Conditions_included */
