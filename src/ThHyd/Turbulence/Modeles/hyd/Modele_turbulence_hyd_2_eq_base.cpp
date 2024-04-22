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

#include <Modele_turbulence_hyd_2_eq_base.h>
#include <TRUSTTrav.h>
#include <Param.h>

Implemente_base(Modele_turbulence_hyd_2_eq_base, "Modele_turbulence_hyd_2_eq_base", Modele_turbulence_hyd_base);
// XD mod_turb_hyd_rans modele_turbulence_hyd_deriv mod_turb_hyd_rans -1 Class for RANS turbulence model for Navier-Stokes equations.

Sortie& Modele_turbulence_hyd_2_eq_base::printOn(Sortie& is) const { return Modele_turbulence_hyd_base::printOn(is); }
Entree& Modele_turbulence_hyd_2_eq_base::readOn(Entree& is) { return Modele_turbulence_hyd_base::readOn(is); }

void Modele_turbulence_hyd_2_eq_base::set_param(Param& param)
{
  Modele_turbulence_hyd_base::set_param(param);
  param.ajouter("k_min", &K_MIN_); // XD_ADD_P floattant Lower limitation of k (default value 1.e-10).
  param.ajouter_flag("quiet", &lquiet_); // XD_ADD_P flag To disable printing of information about K and Epsilon/Omega.
}

void Modele_turbulence_hyd_2_eq_base::verifie_loi_paroi()
{
  Nom lp = loipar_->que_suis_je();
  if (lp == "negligeable_VEF" || lp == "negligeable_VDF")
    {
      Cerr << "The turbulence model of type " << que_suis_je() << finl;
      Cerr << "must not be used with a wall law of type negligeable." << finl;
      Cerr << "Another wall law must be selected with this kind of turbulence model." << finl;
      Process::exit();
    }
}

int Modele_turbulence_hyd_2_eq_base::reprendre_generique(Entree& is)
{
  double dbidon;
  Nom bidon;
  DoubleTrav tab_bidon;
  is >> bidon >> bidon;
  is >> dbidon;
  tab_bidon.jump(is);
  return 1;
}
