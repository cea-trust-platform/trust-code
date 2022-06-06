/****************************************************************************
* Copyright (c) 2022, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Viscosite_turbulente_multiple.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Turbulence
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Viscosite_turbulente_multiple.h>
#include <Param.h>
#include <Probleme_base.h>
#include <Champ_base.h>
#include <TRUSTTab_parts.h>
#include <TRUSTTab.h>
#include <TRUSTTrav.h>
#include <MD_Vector_tools.h>


Implemente_instanciable(Viscosite_turbulente_multiple, "Viscosite_turbulente_multiple", Viscosite_turbulente_base);

Sortie& Viscosite_turbulente_multiple::printOn(Sortie& os) const
{
  return os;
}

Entree& Viscosite_turbulente_multiple::readOn(Entree& is)
{
  Motcle mot;
  is >> mot;
  if (mot != "{") Cerr << "Multiple turbulent viscosities : { expected instead of " << mot << finl, Process::exit();

  for (is >> mot; mot != "}"; is >> mot)
    if (viscs_turbs.count(mot.getString())) Process::exit(que_suis_je() + " : a correlation already exists for " + mot + " !");
    else viscs_turbs[mot.getString()].typer_lire(pb_.valeur(), "viscosite_turbulente", is);
  return is;
}

void Viscosite_turbulente_multiple::eddy_viscosity(DoubleTab& nu_t) const
{
  //On fait appel aux correlations d'en_dessous
  nu_t = 0;
  DoubleTrav nu_loc = DoubleTrav(nu_t);
  for (auto &&corr : viscs_turbs)
    {
      nu_loc = 0;
      ref_cast(Viscosite_turbulente_base, corr.second.valeur()).eddy_viscosity(nu_loc);
      for (int i = 0; i < nu_t.dimension_tot(0); i++)
        for (int n = 0; n < nu_t.dimension(1); n++)
          nu_t(i, n) += nu_loc(i,n);
    }
}

void Viscosite_turbulente_multiple::reynolds_stress(DoubleTab& R_ij) const // Renvoie <u_i'u_j'>
{
  int D = dimension;
  R_ij = 0;
  DoubleTrav R_ij_loc = DoubleTrav(R_ij);
  for (auto &&corr : viscs_turbs)
    {
      R_ij_loc = 0;
      ref_cast(Viscosite_turbulente_base, corr.second.valeur()).reynolds_stress(R_ij_loc);
      for (int i = 0; i < R_ij.dimension(0); i++)
        for (int n = 0; n < R_ij.dimension(1); n++)
          for (int d = 0; d < D; d++)
            for (int db = 0; db < D; db++)
              R_ij(i, n, d, db) += R_ij_loc(i, n, d, db);
    }
}

void Viscosite_turbulente_multiple::reynolds_stress_BIF(DoubleTab& R_ij) const // Renvoie <u_i'u_j'>
{
  int D = dimension;
  R_ij = 0;
  DoubleTrav R_ij_loc = DoubleTrav(R_ij);
  for (auto &&corr : viscs_turbs)
    if ((corr.first == "BIF") | (corr.first == "WIF") | (corr.first == "WIT"))
      {
        R_ij_loc = 0;
        ref_cast(Viscosite_turbulente_base, corr.second.valeur()).reynolds_stress(R_ij_loc);
        for (int i = 0; i < R_ij.dimension(0); i++)
          for (int n = 0; n < R_ij.dimension(1); n++)
            for (int d = 0; d < D; d++)
              for (int db = 0; db < D; db++)
                R_ij(i, n, d, db) += R_ij_loc(i, n, d, db);
      }
}


void Viscosite_turbulente_multiple::k_over_eps(DoubleTab& k_sur_eps) const
{
  k_sur_eps = 0;
  DoubleTrav k_sur_eps_loc = DoubleTrav(k_sur_eps);
  for (auto &&corr : viscs_turbs)
    {
      k_sur_eps_loc = 0;
      ref_cast(Viscosite_turbulente_base, corr.second.valeur()).k_over_eps(k_sur_eps_loc);
      for (int i = 0; i < k_sur_eps.dimension_tot(0); i++)
        for (int n = 0; n < k_sur_eps.dimension(1); n++)
          k_sur_eps(i, n) += k_sur_eps_loc(i,n);
    }

}

void Viscosite_turbulente_multiple::eps(DoubleTab& eps_) const
{
  eps_ = 0;
  DoubleTrav eps_loc = DoubleTrav(eps_);
  for (auto &&corr : viscs_turbs)
    {
      eps_loc = 0;
      ref_cast(Viscosite_turbulente_base, corr.second.valeur()).eps(eps_loc);
      for (int i = 0; i < eps_.dimension_tot(0); i++)
        for (int n = 0; n < eps_.dimension(1); n++)
          eps_(i, n) += eps_loc(i,n);
    }

}
