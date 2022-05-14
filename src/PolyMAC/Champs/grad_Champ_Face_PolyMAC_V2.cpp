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
// File:        grad_Champ_Face_PolyMAC_V2.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <grad_Champ_Face_PolyMAC_V2.h>
#include <Champ_Fonc_Elem_PolyMAC.h>
#include <Champ_Face_PolyMAC_V2.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_Cl_dis_base.h>
#include <Zone_Cl_dis.h>
#include <Champ_Fonc.h>
#include <Dirichlet.h>
#include <Conds_lim.h>
#include <cmath>


Implemente_instanciable(grad_Champ_Face_PolyMAC_V2,"grad_Champ_Face_PolyMAC_V2",Champ_Fonc_Face_PolyMAC);


//     printOn()
/////

Sortie& grad_Champ_Face_PolyMAC_V2::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& grad_Champ_Face_PolyMAC_V2::readOn(Entree& s)
{
  return s ;
}

int grad_Champ_Face_PolyMAC_V2::fixer_nb_valeurs_nodales(int n)
{
  const Champ_Fonc_base& self = ref_cast(Champ_Fonc_base, *this);
  const Zone_PolyMAC_V2& zone = ref_cast(Zone_PolyMAC_V2,self.zone_dis_base());

  assert(n < 0);

  const MD_Vector& md = zone.mdv_ch_face;
  // Probleme: nb_comp vaut 2 mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  nb_compo_ /= dimension;
  creer_tableau_distribue(md);
  nb_compo_ = old_nb_compo;
  return n;

}


void grad_Champ_Face_PolyMAC_V2::me_calculer(double tps)
{
  update_tab_grad(0); // calcule les coefficients de fgrad requis pour le calcul du champ aux faces
  calc_gradfve();
  update_ge(); // calcule le champ aux elements a partir du champ aux faces
  valeurs().echange_espace_virtuel();
}

void grad_Champ_Face_PolyMAC_V2::update_tab_grad(int full_stencil)
{
  const IntTab&             f_cl = champ_a_deriver().fcl();
  const Zone_PolyMAC_V2&       zone = ref_cast(Zone_PolyMAC_V2, zone_vf());
  const Champ_Face_PolyMAC_V2&   ch = ref_cast(Champ_Face_PolyMAC_V2, champ_a_deriver());
  const Conds_lim&           cls = ch.zone_Cl_dis().les_conditions_limites(); // CAL du champ a deriver
  zone.fgrad(champ_a_deriver().valeurs().line_size(), 0, cls, f_cl, NULL, NULL, 1, full_stencil, gradve_d, gradve_e, gradve_w);
}

void grad_Champ_Face_PolyMAC_V2::calc_gradfve()
{
  const Zone_PolyMAC_V2&            zone = ref_cast(Zone_PolyMAC_V2,zone_vf());
  const Zone_Cl_PolyMAC&          zcl = ref_cast(Zone_Cl_PolyMAC, la_Zone_Cl_PolyMAC.valeur());
  const Champ_Face_PolyMAC_V2&        ch = ref_cast(Champ_Face_PolyMAC_V2, champ_a_deriver());
  const IntTab&                   fcl = ch.fcl();
  const Conds_lim&                cls = zcl.les_conditions_limites();
  int f, n;
  int d_U ; //coordonnee de la vitesse
  int D = dimension;
  int N = champ_a_deriver().valeurs().line_size(); //nombre phases
  int ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), nf = zone.nb_faces();

  const DoubleTab& tab_ch = ch.passe();
  DoubleTab&          val = valeurs();

  for (f = 0; f < nf; f++)
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
                    val(f, d_U + n*D) += gradve_w(j, n) * val_e;
                  }
                else if (fcl(f_bord = e - ne_tot, 0) == 3) //contrib d'un bord : seul Dirichlet contribue
                  {
                    double val_f_bord = ref_cast(Dirichlet, cls[fcl(f_bord, 1)].valeur()).val_imp(fcl(f_bord, 2), N * d_U + n);
                    val(f, d_U + n*D) += gradve_w(j, n) * val_f_bord;
                  }
              }
          }
    }

}

void grad_Champ_Face_PolyMAC_V2::update_ge()
{
  DoubleTab& val = valeurs() ;
  const Zone_PolyMAC_V2& zone = ref_cast(Zone_PolyMAC_V2,zone_vf());
  const DoubleVect& ve = zone.volumes(), &fs = zone.face_surfaces();
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  const DoubleTab& xp = zone.xp(), &xv = zone.xv();
  int e, f, j, d, D = dimension, n, N = val.line_size(), ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();
  double fac;

  for (e = 0; e < ne_tot; e++)
    {
      for (d = 0; d < D; d++) for (n = 0; n < N; n++) val(nf_tot + D * e + d, n) = 0;
      for (j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++) for (fac = (e == f_e(f, 0) ? 1 : -1) * fs(f) / ve(e), d = 0; d < D; d++) for (n = 0; n < N; n++)
            val(nf_tot + D * e + d, n) += fac * (xv(f, d) - xp(e, d)) * val(f, n);
    }
}

