/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Op_Dift_Multiphase_VDF_Elem.h>
#include <Op_Dift_Multiphase_VDF_Face.h>
#include <Viscosite_turbulente_base.h>
#include <Transport_turbulent_base.h>
#include <VDF_discretisation.h>
#include <Pb_Multiphase.h>

Implemente_instanciable_sans_constructeur(Op_Dift_Multiphase_VDF_Elem,"Op_Diff_VDFTURBULENTE_P0_VDF",Op_Dift_VDF_Elem_base);

Sortie& Op_Dift_Multiphase_VDF_Elem::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_Multiphase_VDF_Elem::readOn(Entree& is)
{
  //lecture de la correlation de transport turbulente
  corr_.typer_lire(equation().probleme(), "transport_turbulent", is);

  associer_corr_impl<Type_Operateur::Op_DIFT_MULTIPHASE_ELEM, Eval_Dift_Multiphase_VDF_Elem>(corr_);

  return is;
}

Op_Dift_Multiphase_VDF_Elem::Op_Dift_Multiphase_VDF_Elem() : Op_Dift_VDF_Elem_base(Iterateur_VDF_Elem<Eval_Dift_Multiphase_VDF_Elem>()) { }

void Op_Dift_Multiphase_VDF_Elem::completer()
{
  assert(corr_.non_nul());
  completer_Op_Dift_VDF_base();
  associer_pb<Eval_Dift_Multiphase_VDF_Elem>(equation().probleme());

  //si la correlation a besoin du gradient de u, on doit le creer maintenant
  if (ref_cast(Transport_turbulent_base, corr_.valeur()).gradu_required())
    equation().probleme().creer_champ("gradient_vitesse");

  // on initialise d_t_ a 0 avec la bonne structure //
  d_t_ = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue()->valeurs();
  d_t_ = 0.;
  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_ELEM, Eval_Dift_Multiphase_VDF_Elem>(d_t_);
}

void Op_Dift_Multiphase_VDF_Elem::mettre_a_jour(double temps)
{
  Op_Dift_VDF_Elem_base::mettre_a_jour(temps);

  // on calcule d_t_
  d_t_ = 0.;
  const Operateur_base& op_qdm = equation().probleme().equation(0).operateur(0).l_op_base();
  if (!sub_type(Op_Dift_Multiphase_VDF_Face, op_qdm))
    Process::exit(que_suis_je() + ": no turbulent momentum diffusion found!");
  const Correlation& corr_visc = ref_cast(Op_Dift_Multiphase_VDF_Face, op_qdm).correlation();
  if (!sub_type(Viscosite_turbulente_base, corr_visc.valeur()))
    Process::exit(que_suis_je() + ": no turbulent viscosity correlation found!");

  //un "simple" appel a la correlation!
  ref_cast(Transport_turbulent_base, corr_.valeur()).modifier_nu(ref_cast(Convection_Diffusion_std, equation()), ref_cast(Viscosite_turbulente_base, corr_visc.valeur()), d_t_);
  set_nut_impl<Type_Operateur::Op_DIFT_MULTIPHASE_ELEM, Eval_Dift_Multiphase_VDF_Elem>(d_t_);
}

