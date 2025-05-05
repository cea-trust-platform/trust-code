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

#ifndef Diametre_bulles_musig_included
#define Diametre_bulles_musig_included

#include <Correlation_base.h>
#include <Champ_Fonc_base.h>

/*! @brief classe Diametre_bulles_musig Contains the Sauter diameters of the bubble/droplet field (dispersed phase) for the MUSIG-style algorithm.
 *
 *
 *
 */

class Diametre_bulles_musig : public Correlation_base
{
  Declare_instanciable(Diametre_bulles_musig);
public:
  const Champ_base& get_champ(const Motcle& nom) const override;
  bool has_champ(const Motcle& nom, OBS_PTR(Champ_base) &ref_champ) const override;
  bool has_champ(const Motcle& nom) const override;

protected:
  double d_bulle_ = -100.;
  OWN_PTR(Champ_Fonc_base)  diametres_;
  int n_l = -1 ; // liquid phase
};

#endif
