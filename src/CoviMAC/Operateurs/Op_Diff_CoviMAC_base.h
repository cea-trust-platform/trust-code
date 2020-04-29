/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Op_Diff_CoviMAC_base.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Op_Diff_CoviMAC_base_included
#define Op_Diff_CoviMAC_base_included

#include <Operateur_Diff_base.h>
#include <Op_Diff_Turbulent_base.h>
#include <Ref_Zone_CoviMAC.h>
#include <Ref_Zone_Cl_CoviMAC.h>
#include <Zone_CoviMAC.h>
class Champ_Fonc;


//
// .DESCRIPTION class Op_Diff_CoviMAC_base
//
// Classe de base des operateurs de diffusion CoviMAC

//
// .SECTION voir aussi
//
//


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_CoviMAC_base
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_CoviMAC_base : public Operateur_Diff_base, public Op_Diff_Turbulent_base
{


  Declare_base(Op_Diff_CoviMAC_base);

public:
  void associer(const Zone_dis& , const Zone_Cl_dis& ,const Champ_Inc& );

  void associer_diffusivite(const Champ_base& );
  void completer();
  const Champ_base& diffusivite() const;
  void mettre_a_jour(double t)
  {
    Operateur_base::mettre_a_jour(t);
    nu_a_jour_ = 0;
  }

  void update_nu() const; //met a jour nu et nu_fac
  const DoubleTab& get_nu() const
  {
    return nu_;
  }

  const DoubleTab& get_nu_faces() const
  {
    return nu_faces_;
  }

  const DoubleTab& get_nu_fac() const
  {
    return nu_fac_;
  }

  inline void remplir_nu_ef(int e, DoubleTab& nu_ef) const;

  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  virtual int impr(Sortie& os) const;
  mutable DoubleTab nu_fac_mod; //facteur multiplicatif "utilisateur" a appliquer a nu_fac

protected:
  REF(Zone_CoviMAC) la_zone_poly_;
  REF(Zone_Cl_CoviMAC) la_zcl_poly_;
  REF(Champ_base) diffusivite_;
  mutable DoubleTab nu_, nu_fac_, nu_faces_; //conductivite aux elements, facteur multiplicatif a appliquer par face
  mutable int nu_a_jour_; //si on doit mettre a jour nu
};

//
// Fonctions inline de la classe Op_Diff_CoviMAC_base
//
/* diffusivite a l'interieur d'un element e : nu_ef(i, n) : diffusivite de la composante n entre le centre de l'element et celui de la face i */
inline void Op_Diff_CoviMAC_base::remplir_nu_ef(int e, DoubleTab& nu_ef) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& e_f = zone.elem_faces();
  const DoubleTab& xp = zone.xp(), &xv = zone.xv();
  int i, j, k, f, n, N = nu_ef.dimension(1), N_nu = nu_.line_size();
  double fac;

  for (i = 0; i < zone.m2d(e + 1) - zone.m2d(e); i++)
    {
      f = e_f(e, i);
      /* diffusivite de chaque composante dans la direction (xf - xe) */
      if (N_nu == N) for (n = 0; n < N; n++) nu_ef(i, n) = nu_.addr()[N * e + n]; //isotrope
      else if (N_nu == N * dimension) for (n = 0; n < N; n++) for (j = 0, nu_ef(i, n) = 0; j < dimension; j++) //anisotrope diagonal
            nu_ef(i, n) += nu_.addr()[dimension * (N * e + n) + j] * std::pow(xv(f, j) - xp(e, j), 2);
      else if (N_nu == N * dimension * dimension) for (n = 0; n < N; n++) for (j = 0, nu_ef(i, n) = 0; j < dimension; j++) for (k = 0; k < dimension; k++)
              nu_ef(i, n) += nu_.addr()[dimension * (dimension * (N * e + n) + j) + k] * (xv(f, j) - xp(e, j)) * (xv(f, k) - xp(e, k)); //anisotrope complet
      else abort();
      for (n = 0, fac = nu_fac_.addr()[f] * (N_nu > N ? 1. / zone.dot(&xv(f, 0), &xv(f, 0), &xp(e, 0), &xp(e, 0)) : 1); n < N; n++) nu_ef(i, n) *= fac;
    }
}
#endif
