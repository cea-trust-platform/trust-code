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

#ifndef Modele_turbulence_hyd_2_eq_base_included
#define Modele_turbulence_hyd_2_eq_base_included

#include <Modele_turbulence_hyd_base.h>

class Equation_base;

/*! @brief Classe Modele_turbulence_hyd_2_eq_base Classe de base des modeles de type RANS à deux equations
 *
 * @sa Modele_turbulence_hyd_base
 */
class Modele_turbulence_hyd_2_eq_base: public Modele_turbulence_hyd_base
{
  Declare_base(Modele_turbulence_hyd_2_eq_base);
public:
  void set_param(Param& ) override;
  virtual void verifie_loi_paroi();
  int reprendre_generique(Entree& is);

  virtual int nombre_d_equations() const { return 1; }

  inline double get_Prandtl_K() const { return Prandtl_K_; }
  inline double get_Prandtl_Eps() const { return Prandtl_Eps_; }
  inline double get_Prandtl_Omega() const { return Prandtl_Omega_; }
  inline double get_EPS_MIN() const { return EPS_MIN_; }
  inline double get_EPS_MAX() const { return EPS_MAX_; }
  inline double get_OMEGA_MIN() const { return OMEGA_MIN_; }
  inline double get_OMEGA_MAX() const { return OMEGA_MAX_; }
  inline double get_K_MIN() const { return K_MIN_; }
  inline int get_lquiet() const { return lquiet_; }

protected:
  double Prandtl_K_ = 1., Prandtl_Eps_ = 1.3, Prandtl_Omega_ = 2.;
  double K_MIN_ = 1.e-20, EPS_MIN_ = 1.e-20, EPS_MAX_ = 1.e+10, OMEGA_MIN_ = 1.e-20, OMEGA_MAX_ = 1.e+10;
  int lquiet_ = 0;
};

#endif /* Modele_turbulence_hyd_2_eq_base_included */
