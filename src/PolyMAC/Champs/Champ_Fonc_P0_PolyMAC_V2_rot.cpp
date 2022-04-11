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
// File:        Champ_Fonc_P0_PolyMAC_V2_rot.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_P0_PolyMAC_V2_rot.h>
#include <Champ_Fonc_P0_PolyMAC.h>
#include <Champ_Face_PolyMAC_V2.h>
#include <grad_Champ_Face_PolyMAC_V2.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_Cl_dis_base.h>
#include <Zone_Cl_dis.h>
#include <Champ_Fonc.h>
#include <Navier_Stokes_std.h>


Implemente_instanciable(Champ_Fonc_P0_PolyMAC_V2_rot,"Champ_Fonc_P0_PolyMAC_V2_rot",Champ_Fonc_P0_PolyMAC);


//     printOn()
/////

Sortie& Champ_Fonc_P0_PolyMAC_V2_rot::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Champ_Fonc_P0_PolyMAC_V2_rot::readOn(Entree& s)
{
  return s ;
}

void Champ_Fonc_P0_PolyMAC_V2_rot::mettre_a_jour(double tps)
{
  if (temps()!=tps)
    {
      if (dimension == 2) me_calculer_2D();
      if (dimension == 3) me_calculer_3D();
    }
  Champ_Fonc_base::mettre_a_jour(tps);
}

void Champ_Fonc_P0_PolyMAC_V2_rot::me_calculer_2D()
{
  const Champ_Face_PolyMAC_V2& vit = ref_cast(Champ_Face_PolyMAC_V2,champ_a_deriver());
  const Zone_PolyMAC_V2&          zone = ref_cast(Zone_PolyMAC_V2,vit.zone_vf());
  int e, n ;
  int D = dimension, N = champ_a_deriver().valeurs().line_size();
  int ne = zone.nb_elem(), nf_tot = zone.nb_faces_tot();

  const Navier_Stokes_std& eq = ref_cast(Navier_Stokes_std, vit.equation());
  DoubleTab&          tab_rot = valeurs();
  const grad_Champ_Face_PolyMAC_V2& grad = ref_cast(grad_Champ_Face_PolyMAC_V2, eq.get_champ("gradient_vitesse"));
  const DoubleTab& tab_grad = grad.valeurs();

  for (n = 0 ; n<N ; n++) for (e = 0; e < ne; e++)
      {
        tab_rot(e, n) = tab_grad(nf_tot + D * e + 0 , 1 + n * D) - tab_grad(nf_tot + D * e + 1 , 0 + n * D); // dUy/dx - dUx/dy
      }

  tab_rot.echange_espace_virtuel();
}

void Champ_Fonc_P0_PolyMAC_V2_rot::me_calculer_3D()
{
  const Champ_Face_PolyMAC_V2& vit = ref_cast(Champ_Face_PolyMAC_V2,champ_a_deriver());
  const Zone_PolyMAC_V2& zone = ref_cast(Zone_PolyMAC_V2,vit.zone_vf());
  int e, n ;
  int D = dimension, N = champ_a_deriver().valeurs().line_size();
  int ne = zone.nb_elem(), nf_tot = zone.nb_faces_tot();

  const Navier_Stokes_std& eq = ref_cast(Navier_Stokes_std, vit.equation());
  DoubleTab&          tab_rot = valeurs();
  const grad_Champ_Face_PolyMAC_V2& grad = ref_cast(grad_Champ_Face_PolyMAC_V2, eq.get_champ("gradient_vitesse"));
  const DoubleTab& tab_grad = grad.valeurs();

  for (n = 0 ; n<N ; n++) for (e = 0; e < ne; e++)
      {
        tab_rot(e, 0 + n * D) = tab_grad(nf_tot + D * e + 1 , 2 + n * D) - tab_grad(nf_tot + D * e + 2 , 1 + n * D); // dUz/dy - dUy/dz
        tab_rot(e, 1 + n * D) = tab_grad(nf_tot + D * e + 2 , 0 + n * D) - tab_grad(nf_tot + D * e + 0 , 2 + n * D); // dUx/dz - dUz/dx
        tab_rot(e, 2 + n * D) = tab_grad(nf_tot + D * e + 0 , 1 + n * D) - tab_grad(nf_tot + D * e + 1 , 0 + n * D); // dUy/dx - dUx/dy
      }

  tab_rot.echange_espace_virtuel();
}

