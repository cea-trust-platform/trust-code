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
// File:        Champ_Face_PolyMAC_P0.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Face_PolyMAC_P0_included
#define Champ_Face_PolyMAC_P0_included

#include <Champ_Face_PolyMAC.h>
#include <Zone_VF.h>
#include <Ref_Zone_VF.h>

#include <SolveurSys.h>
#include <Zone_PolyMAC_P0.h>

/////////////////////////////////////////////////////////////////////////////
// .NAME        : Champ_Face_PolyMAC_P0
// .DESCRIPTION : class Champ_Face_PolyMAC_P0
//
// Champ correspondant a une inconnue decrite par ses tangentes aux faces duales (ligne amont-aval, type vitesse)
// Les flux aux faces sont accessibles par les methodes valeurs_normales(), avec synchro automatique
// Degres de libertes : composantes tangentielles aux faces duales
/////////////////////////////////////////////////////////////////////////////

class Champ_Face_PolyMAC_P0 : public Champ_Face_PolyMAC
{

  Declare_instanciable(Champ_Face_PolyMAC_P0) ;

public :

  int fixer_nb_valeurs_nodales(int n) override;
  void init_auxiliary_variables() override; /* demande l'ajout des variables auxiliaires (\vec v aux elements) */

  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const override;
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const override;
  DoubleTab& trace(const Frontiere_dis_base& , DoubleTab& , double, int distant ) const override;

  Champ_base& affecter_(const Champ_base& ) override;

  //interpolations vitesse aux faces -> vitesse aux elems
  void update_ve(DoubleTab& val) const; //ordre 1

  void init_ve2() const; //ordre 2 -> avec une matrice
  mutable IntTab ve2d, ve2j, ve2bj;
  mutable DoubleTab ve2c, ve2bc;
  void update_ve2(DoubleTab& val, int incr = 0) const;

  /* utilitaire pour le calcul des termes sources : calcule le vecteur v_e + n_f (v_f - v_e. n_f)
     retour : le vecteur, sa norme et les derivees de celle-ci selon v_e et v_f
  */
  inline double v_norm(const DoubleTab& val, const DoubleTab& val_f, int e, int f, int k, int l, double *v_ext, double *dnv) const
  {
    const Zone_PolyMAC_P0& zone = ref_cast(Zone_PolyMAC_P0, zone_dis_base());
    int d, D = dimension, nf_tot = zone.nb_faces_tot();
    const DoubleTab& nf = zone.face_normales();
    const DoubleVect& fs = zone.face_surfaces();
    double scal = 0, vf = f >= 0 ? val_f(f, k) - (l >= 0 ? val_f(f, l) : 0) : 0, v_temp[3], *v = v_ext ? v_ext : v_temp;
    for (d = 0; d < D; d++) v[d] = val(nf_tot + D * e + d, k) - (l >= 0 ? val(nf_tot + D * e + d, l) : 0);
    if (f >= 0) for (d = 0, scal = zone.dot(v, &nf(f, 0)) / fs(f); d < D; d++) v[d] += (vf - scal) * nf(f, d) / fs(f);
    double nv = sqrt(zone.dot(v, v));
    if (dnv) for (d = 0; d < D; d++) dnv[d] = nv ? (v[d] - (f >= 0 ? vf * nf(f, d) / fs(f) : 0)) / nv : 0;
    if (dnv) dnv[3] = f >= 0 && nv ? vf / nv : 0;
    return nv;
  }
};

#endif /* Champ_Face_PolyMAC_P0_included */
