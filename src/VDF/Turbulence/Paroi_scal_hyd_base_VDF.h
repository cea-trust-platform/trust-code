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

#ifndef Paroi_scal_hyd_base_VDF_included
#define Paroi_scal_hyd_base_VDF_included

#include <Turbulence_paroi_scal_base.h>

#include <Domaine_VDF.h>
#include <TRUST_Ref.h>

class Domaine_Cl_VDF;

class Paroi_scal_hyd_base_VDF : public Turbulence_paroi_scal_base
{
  Declare_base(Paroi_scal_hyd_base_VDF);
public:
  void associer(const Domaine_dis_base& ,const Domaine_Cl_dis_base& ) override;
  int init_lois_paroi() override;
  void compute_nusselt() const override;
  void imprimer_nusselt(Sortie&) const override;
  DoubleVect& equivalent_distance_name(DoubleVect& d_equiv, const Nom& nom_bord) const override;

protected:
  mutable int nb_impr_ = -123;        // Compteur d'impression
};

#endif /* Paroi_scal_hyd_base_VDF_included */
