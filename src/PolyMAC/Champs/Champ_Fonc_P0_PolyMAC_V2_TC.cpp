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
// File:        Champ_Fonc_P0_PolyMAC_V2_TC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_P0_PolyMAC_V2_TC.h>
#include <Champ_Fonc_P0_PolyMAC.h>
#include <Champ_Face_PolyMAC_V2.h>
#include <grad_Champ_Face_PolyMAC_V2.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_Cl_dis_base.h>
#include <Zone_Cl_dis.h>
#include <Champ_Fonc.h>
#include <Navier_Stokes_std.h>
#include <cmath>


Implemente_instanciable(Champ_Fonc_P0_PolyMAC_V2_TC,"Champ_Fonc_P0_PolyMAC_V2_TC",Champ_Fonc_P0_PolyMAC);

Sortie& Champ_Fonc_P0_PolyMAC_V2_TC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Champ_Fonc_P0_PolyMAC_V2_TC::readOn(Entree& s)
{
  return s ;
}

void Champ_Fonc_P0_PolyMAC_V2_TC::me_calculer(double tps) //See Pope 2000 page 367 for ref
{
  const Champ_Face_PolyMAC_V2& vitesse = ref_cast(Champ_Face_PolyMAC_V2,champ_.valeur());
  const Zone_PolyMAC_V2& zone = ref_cast(Zone_PolyMAC_V2,vitesse.zone_vf());
  int e, d_U, d_X, n ;
  int D = dimension, N = champ_a_deriver().valeurs().line_size();
  int ne = zone.nb_elem(), nf_tot = zone.nb_faces_tot();

  const Navier_Stokes_std& eq = ref_cast(Navier_Stokes_std, vitesse.equation());
  DoubleTab& tab_tc = valeurs();
  const grad_Champ_Face_PolyMAC_V2& grad = ref_cast(grad_Champ_Face_PolyMAC_V2, eq.get_champ("gradient_vitesse"));
  const DoubleTab& tab_grad = grad.valeurs();

  for (e = 0; e < ne; e++) for (n = 0; n < N; n++)
      {
        tab_tc(e, n) = 0;
        for (d_U = 0; d_U < D; d_U++) for (d_X = 0; d_X < D; d_X++)
            {
              double Sij = 0.5 * (tab_grad(nf_tot + d_X + e * D , D * n + d_U) + tab_grad(nf_tot + d_U + e * D , D * n + d_X)) ;
              if (d_U == d_X) for (int i = 0 ; i <D ; i++) Sij += -1./D*tab_grad(nf_tot + i + e * D , D * n + i) ; // Substract the divergence : this term is zero in incompressible NS
              tab_tc(e, n) += Sij * Sij ;
            }
        tab_tc(e, n) = sqrt(2 * tab_tc(e, n));
      }

  tab_tc.echange_espace_virtuel();
}

