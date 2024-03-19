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

#ifndef Loi_Etat_TPPI_base_included
#define Loi_Etat_TPPI_base_included

#include <Loi_Etat_Mono_GP_base.h>
#include <TPPI.h>

class Loi_Etat_TPPI_base: public Loi_Etat_Mono_GP_base
{
  Declare_base(Loi_Etat_TPPI_base);
public:
  void preparer_calcul() override;

  double inverser_Pth(double, double) override;
  double calculer_masse_volumique(double,double) const override;
  void calculer_masse_volumique() override;

  void calculer_Cp() override;
  void calculer_mu() override { /* Do nothing */ }
  void calculer_lambda() override { /* Do nothing */ }
  void calculer_alpha() override { /* Do nothing */ }

protected:
  std::shared_ptr<TPPI> TPPI_ = nullptr;
  Motcle model_name_, fluid_name_;

private:
  std::vector<double> vec_press_;
  bool vec_press_filled_ = false;
  void verify_fields();
  void init_vec_press();
};

#endif /* Loi_Etat_TPPI_base_included */
