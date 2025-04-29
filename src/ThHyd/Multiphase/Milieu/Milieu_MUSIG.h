/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Milieu_MUSIG_included
#define Milieu_MUSIG_included

#include <Milieu_composite.h>
#include <Fluide_MUSIG.h>

class Milieu_MUSIG : public Milieu_composite
{
  Declare_instanciable( Milieu_MUSIG ) ;
public :
  double get_Diameter_Inf(int iPhaseMilieu) const;
  double get_Diameter_Sup(int iPhaseMilieu) const;
  double get_Diameter_Sauter(int iPhaseMilieu) const;
  bool has_dispersed_gas(int k) const;
  bool has_carrier_liquid(int k) const;
  bool has_dispersed_liquid(int k) const;
  bool has_carrier_gas(int k) const;
private :
  std::vector<Fluide_MUSIG> fluidesMUSIG_;

  /*! For each phase, a 3 int array is stored
   *
   * phase index for the media / index of FluideMUSIG (-1 if not FluideMUSIG) /index of the phase in fluideMUSIG.
   */
  std::vector<std::array<int,3>> indexMilieuToIndexFluide_; // could be change for something better
};

#endif /* Milieu_MUSIG_included */
