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



#ifndef Op_Diff_PolyMAC_base_included
#define Op_Diff_PolyMAC_base_included

#include <Operateur_Diff_base.h>
#include <Ref_Zone_PolyMAC.h>
#include <Ref_Zone_Cl_PolyMAC.h>
#include <Zone_PolyMAC.h>
#include <SFichier.h>
class Champ_Fonc;


//
// .DESCRIPTION class Op_Diff_PolyMAC_base
//
// Classe de base des operateurs de diffusion PolyMAC

//
// .SECTION voir aussi
//
//


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_PolyMAC_base
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_PolyMAC_base : public Operateur_Diff_base
{


  Declare_base(Op_Diff_PolyMAC_base);

public:
  void associer(const Zone_dis& , const Zone_Cl_dis& ,const Champ_Inc& ) override;

  void associer_diffusivite(const Champ_base& ) override;
  void completer() override;

  const Champ_base& diffusivite() const override;
  void mettre_a_jour(double t) override;

  /* methodes surchargeables dans des classes derivees pour modifier nu avant de calculer les gradients dans update_nu_xwh */
  virtual int dimension_min_nu() const /* dimension minimale de nu / nu_bord par composante */
  {
    return 1;
  };
  virtual void modifier_nu(DoubleTab& ) const { };

  /* versions etendues de dimensionner/ajouter_blocs permettant de traiter les seules variables auxiliaires */
  virtual void dimensionner_blocs_ext(int aux_only, matrices_t matrices, const tabs_t& semi_impl = {}) const = 0;
  virtual void ajouter_blocs_ext(int aux_only, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const = 0;

  /* implementations de dimensionner/ajouter_blocs a partir de ces methodes */
  int has_interface_blocs() const override
  {
    return 1;
  };
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override
  {
    dimensionner_blocs_ext(0, matrices, semi_impl);
  }
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override
  {
    ajouter_blocs_ext(0, matrices, secmem, semi_impl);
  }

  /* diffusivite / conductivite. Attension : stockage nu(element, composante[, dim 1[, dim 2]]) */
  inline const DoubleTab& nu() const /* aux elements */
  {
    if (!nu_a_jour_) update_nu();
    return nu_;
  }

  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  int impr(Sortie& os) const override;

protected:
  REF(Zone_PolyMAC) la_zone_poly_;
  REF(Zone_Cl_PolyMAC) la_zcl_poly_;
  REF(Champ_base) diffusivite_;

  double t_last_nu_ = -1e10; //pour detecter quand on doit recalculer nu, les variables auxiliaires

  /* diffusivite aux elems */
  void update_nu() const; //mise a jour
  mutable DoubleTab nu_;

  mutable int nu_constant_ = 0 /* Elie : pour valgrind */, nu_a_jour_ = 0; //nu constant / nu a jour / phif a jour

  /* gestion des variables auxiliaires en semi-implicite */
  void update_aux(double t) const;
  mutable double t_last_aux_ = -1e10; /* dernier temps auquel on les a calcule */
  mutable int use_aux_;               /* les variables auxiliaires sont-elles stockees dans var_aux ? */
  mutable Matrice_Bloc mat_aux;      /* systeme a resoudre : mat.var_aux = secmem */
  mutable DoubleTab var_aux;
  mutable SolveurSys solv_aux; //solveur

  mutable SFichier Flux, Flux_moment, Flux_sum;
};
#endif
