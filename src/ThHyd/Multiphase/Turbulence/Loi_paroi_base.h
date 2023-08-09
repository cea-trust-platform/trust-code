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

#ifndef Loi_paroi_base_included
#define Loi_paroi_base_included
#include <TRUSTTab.h>
#include <TRUSTTab.h>
#include <Correlation_base.h>
#include <vector>
#include <map>
#include <string>

/*! @brief classe Loi_paroi_base Classe de base pour les lois de paroi dans pb_mulitphase
 *
 *     Methodes implementees :
 *
 *
 */

class Loi_paroi_base : public Correlation_base
{
  Declare_base(Loi_paroi_base);
public:
  void completer() override;
  void mettre_a_jour(double temps) override;
  virtual void calc_y_plus(const DoubleTab& vit, const DoubleTab& nu_visc)=0 ;

  double get_y(int f) const {return valeurs_loi_paroi_.at("y")(f,0);};
  double get_utau(int f) const { return valeurs_loi_paroi_.at("u_tau")(f,0);};
  DoubleTab& get_tab(std::string str) {return valeurs_loi_paroi_[str];};

protected:

  double eps_y_p_ = 1.e-4; // Convergence of the y_p determination method
  double y_p_min_ = 1.e-2; // minimal y_p

  IntTab Faces_a_calculer_;
  std::map<std::string, DoubleTab> valeurs_loi_paroi_; // contient "y_plus", "u_tau" pour toutes les faces
  double tps_loc = -1.e8;
};

#endif
