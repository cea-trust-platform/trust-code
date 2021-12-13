/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        grad_Champ_Face_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Champs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <grad_Champ_Face_CoviMAC.h>
#include <Champ_Fonc_P0_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Zone_Cl_dis_base.h>
#include <Zone_Cl_dis.h>
#include <Champ_Fonc.h>
#include <Dirichlet.h>
#include <Conds_lim.h>
//#include <array>
#include <cmath>


Implemente_instanciable(grad_Champ_Face_CoviMAC,"grad_Champ_Face_CoviMAC",Champ_Fonc_Face_CoviMAC);


//     printOn()
/////

Sortie& grad_Champ_Face_CoviMAC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& grad_Champ_Face_CoviMAC::readOn(Entree& s)
{
  return s ;
}

void grad_Champ_Face_CoviMAC::me_calculer(double tps)
{
  Cerr << finl << "calcule le gradient de U " << finl << finl ;
  update_tab_grad(0); // calcule les coefficients de fgrad requis pour le calcul du champ aux faces
  calc_gradfve();
  update_ge(); // calcule le champ aux elements a partir du champ aux faces
}



void grad_Champ_Face_CoviMAC::update_tab_grad(int full_stencil)
{
  const IntTab&             f_cl = champ_a_deriver().fcl();
  const Zone_CoviMAC&       zone = ref_cast(Zone_CoviMAC, zone_vf());
  const Champ_Face_CoviMAC&   ch = ref_cast(Champ_Face_CoviMAC, champ_a_deriver());
  const Conds_lim&           cls = ch.zone_Cl_dis().les_conditions_limites(); // CAL du champ à dériver
  zone.fgrad(1, 0, cls, f_cl, NULL, NULL, 1, full_stencil, gradve_d, gradve_e, gradve_w);
}

void grad_Champ_Face_CoviMAC::calc_gradfve()
{
  const Zone_CoviMAC&            zone = ref_cast(Zone_CoviMAC,zone_vf());
  const Zone_Cl_CoviMAC&          zcl = ref_cast(Zone_Cl_CoviMAC, la_zone_Cl_CoviMAC.valeur());
  const Champ_Face_CoviMAC&        ch = ref_cast(Champ_Face_CoviMAC, champ_a_deriver());
  const IntTab&                   fcl = ch.fcl();
  const Conds_lim&                cls = zcl.les_conditions_limites();
  int f, n;
  int d_U ; //coordonnee de la vitesse
  int D = dimension;
  int N = champ_a_deriver().valeurs().line_size(); //nombre phases
  int ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();

  const DoubleTab& tab_ch = ch.valeurs();
  DoubleTab&          val = valeurs();

  for (f = 0; f < nf_tot; f++)
    {
      for (d_U = 0; d_U < D; d_U++) for (n = 0; n < N; n++) // Coordonnees de la vitesse et phase
          {
            val(f, d_U + n*D) = 0;
            for (int j = gradve_d(f); j < gradve_d(f+1) ; j++)
              {
                int e = gradve_e(j);
                int f_bord;
                if ( e < ne_tot) //contrib d'un element
                  {
                    double val_e = tab_ch(nf_tot+ d_U + e * D, n);
                    val(f, d_U + n*D) += gradve_w(j) * val_e;
                  }
                else if (fcl(f_bord = e - ne_tot, 0) == 3) //contrib d'un bord : seul Dirichlet contribue
                  {
                    double val_f_bord = ref_cast(Dirichlet, cls[fcl(f_bord, 1)].valeur()).val_imp(fcl(f_bord, 2), N * d_U + n);
                    val(f, d_U + n*D) += gradve_w(j) * val_f_bord;
                  }
              }
          }
    }
}

void grad_Champ_Face_CoviMAC::update_ge()
{
  DoubleTab& val = valeurs() ;
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC,zone_vf());
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem();
  int e, f, j, d, D = dimension, n, N = val.line_size(), ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();

  zone.init_ve();
  for (e = 0; e < ne_tot; e++)
    {
      for (d = 0; d < D; d++) for (n = 0; n < N; n++) val(nf_tot + D * e + d, n) = 0;
      for (j = zone.ved(e); j < zone.ved(e + 1); j++) for (f = zone.vej(j), d = 0; d < D; d++) for (n = 0; n < N; n++)
            val(nf_tot + D * e + d, n) += zone.vec(j, d) * val(f, n) * pf(f) / pe(e);
    }
}

