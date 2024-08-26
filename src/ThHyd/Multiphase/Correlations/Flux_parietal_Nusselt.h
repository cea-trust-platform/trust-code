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
#ifndef Flux_parietal_Nusselt_included
#define Flux_parietal_Nusselt_included
#include <Flux_parietal_base.h>
#include <Saturation_base.h>
#include <Parser_U.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Class Flux_parietal_Nusselt
//    Correlation of parietal heat flux using the Nusselt number (with Nusselt provided by the user as a function of Re and Pr):
//      lambda / D_ch * Nu * (Tp - Tl)
//////////////////////////////////////////////////////////////////////////////

class Flux_parietal_Nusselt : public Flux_parietal_base
{
  Declare_instanciable(Flux_parietal_Nusselt);
public:
  int lire_motcle_non_standard(const Motcle& mot, Entree& is) override;
  void qp(const input_t& input, output_t& output) const override;
  int T_at_wall() const override { return 0; }
private:
  mutable Parser_U nusselt_;
};

#endif
