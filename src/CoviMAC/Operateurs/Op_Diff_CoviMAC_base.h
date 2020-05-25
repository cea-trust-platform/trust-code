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
  void mettre_a_jour(double t);

  void update_nu() const; //met a jour nu et nu_fac
  const DoubleTab& get_nu() const
  {
    return nu_;
  }

  const DoubleTab& get_nu_fac() const
  {
    return nu_fac_;
  }

  /* renvoie le produit v1.nu_.v2 quelle que soit la forme de nu_ */
  inline double nu_prod(const int e, const int n, const int N, const double *v1, const double *v2) const;

  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  virtual int impr(Sortie& os) const;
  mutable DoubleTab nu_fac_mod; //facteur multiplicatif "utilisateur" a appliquer a nu_fac

protected:
  REF(Zone_CoviMAC) la_zone_poly_;
  REF(Zone_Cl_CoviMAC) la_zcl_poly_;
  REF(Champ_base) diffusivite_;
  mutable DoubleTab nu_, nu_fac_; //conductivite aux elements, facteur multiplicatif a appliquer par face

  /* interpolation du flux diffusif */
  //constant : base des flux aux sommets fournie par Zone_CoviMAC::base_flux_som
  mutable IntTab bfs_d, bfs_fn, bfs_fd;
  mutable DoubleTab bfs_vl, bfs_vn;
  //variables avec nu : coeffs de l'interpolation aux sommets par Zone_CoviMAC::interp_som
  mutable IntTab ts_d;
  mutable DoubleTab ts_e, ts_f;

  mutable int nu_a_jour_; //si on doit mettre a jour nu
  int nu_constant_;       //1 si nu est constant dans le temps
};

//
// Fonctions inline de la classe Op_Diff_CoviMAC_base
//
inline double Op_Diff_CoviMAC_base::nu_prod(const int e, const int n, const int N, const double *v1, const double *v2) const
{
  double resu = 0;
  int i, j, N_nu = nu_.line_size();
  assert(N_nu == 1 || N_nu == N || N_nu == N * dimension || N_nu == N * dimension * dimension);
  if (N_nu <= N) for (i = 0; i < dimension; i++) //isotrope
      resu += nu_.addr()[N_nu < N ? e : N * e + n] * v1[i] * v2[i];
  else if (N_nu == N * dimension) for (i = 0; i < dimension; i++) //anisotrope diagonal
      resu += nu_.addr()[dimension * (N * e + n) + i] * v1[i] * v2[i];
  else if (N_nu == N * dimension * dimension) for (i = 0; i < dimension; i++) for (j = 0; j < dimension; j++) //anisotrope diagonal
        resu += nu_.addr()[dimension * (dimension * (N * e + n) + i) + j] * v1[i] * v2[j];
  return resu;
}

#endif
