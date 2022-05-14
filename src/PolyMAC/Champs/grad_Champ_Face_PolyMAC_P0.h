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
// File:        grad_Champ_Face_PolyMAC_P0.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef grad_Champ_Face_PolyMAC_P0_included
#define grad_Champ_Face_PolyMAC_P0_included


#include <Champ_Fonc_Face_PolyMAC.h>
#include <Champ_Fonc_Elem_PolyMAC.h>
#include <Champ_Fonc.h>
#include <Ref_Zone_Cl_PolyMAC.h>
#include <Ref_Champ_Face_PolyMAC_P0.h>
#include <Champ_Face_PolyMAC_P0.h>
#include <Champ_Fonc.h>
#include <Ref_Champ_Fonc.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    class grad_Champ_Face_PolyMAC_P0 for the calculation of the gradient
//      This field is a Champ_Fonc_Face_PolyMAC_P0 that calculates the gradient of a velocity field
//      grad_u(f, n*D + dU) returns the gradient of the dU velocity component in the phase n at face f
//      grad_u(nf_tot + e*D + dX, n*D + dU) returns the gradient of the dU velocity component in the phase n in element e along the dX component
//        (i.e. = dU/dX)
//      In 2D, the gradient at the element e in phase n is equal to :
//                  |   grad_u(nf_tot + e*D + 0, n*D + 0)   grad_u(nf_tot + e*D + 1, n*D + 0)   |
//        grad_u =  |                                                                           |
//                  |   grad_u(nf_tot + e*D + 0, n*D + 1)   grad_u(nf_tot + e*D + 1, n*D + 1)   |
//
//      The gradient is calculated using past values of the velocity
//        We compute the gradient at the faces for all velocity components,
//          then interpolate it to obtain the gradient at the elements
//
//////////////////////////////////////////////////////////////////////////////

class grad_Champ_Face_PolyMAC_P0 : public Champ_Fonc_Face_PolyMAC

{

  Declare_instanciable(grad_Champ_Face_PolyMAC_P0);

public:

  int fixer_nb_valeurs_nodales(int n) override;
  inline void mettre_a_jour(double ) override;
  inline void associer_champ(const Champ_Face_PolyMAC_P0& );
  void me_calculer(double );

  inline void associer_zone_Cl_dis_base(const Zone_Cl_dis_base&);

  inline virtual       Champ_Face_PolyMAC_P0& champ_a_deriver()      ;
  inline virtual const Champ_Face_PolyMAC_P0& champ_a_deriver() const;


  // Interpolation du gradient de la vitesse
  mutable IntTab gradve_d, gradve_e;           // Tables utilisees dans fgrad pour obtenir le grad aux faces de la vitesse aux elems
  mutable DoubleTab gradve_w;
  void update_tab_grad(int full_stencil);      // Mise a jour des tables utilisees dans fgrad
  void calc_gradfve()     ;                  // Mise a jour du gradient aux faces de la vitesse aux elements
  void update_ge() ;                           // Calcul du gradient aux elements a partir du gradient aux faces ; base sur la methode similaire de Champ_Face_PolyMAC_P0

protected:

  REF(Zone_Cl_PolyMAC) la_Zone_Cl_PolyMAC;
  REF(Champ_Face_PolyMAC_P0) champ_;

};

inline void grad_Champ_Face_PolyMAC_P0::mettre_a_jour(double tps)
{
  if (temps()!=tps) me_calculer(tps) ;
  Champ_Fonc_base::mettre_a_jour(tps);
}

inline void grad_Champ_Face_PolyMAC_P0::associer_champ(const Champ_Face_PolyMAC_P0& ch)
{
  Cerr << "On associe le gradient de U au champ de vitesse " << finl ;
  champ_= ch;
}

inline void grad_Champ_Face_PolyMAC_P0::associer_zone_Cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_Zone_Cl_PolyMAC  = (const Zone_Cl_PolyMAC&) la_zone_Cl_dis_base;
}

inline Champ_Face_PolyMAC_P0& grad_Champ_Face_PolyMAC_P0::champ_a_deriver()
{
  return champ_.valeur();
}
inline const Champ_Face_PolyMAC_P0& grad_Champ_Face_PolyMAC_P0::champ_a_deriver() const
{
  return champ_.valeur();
}

#endif
