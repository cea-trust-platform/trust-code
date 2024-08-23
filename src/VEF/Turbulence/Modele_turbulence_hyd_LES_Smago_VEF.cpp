/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Modele_turbulence_hyd_LES_Smago_VEF.h>
#include <Schema_Temps_base.h>
#include <Domaine_Cl_VEF.h>
#include <Domaine_VEF.h>
#include <Champ_P1NC.h>
#include <Debog.h>
#include <Param.h>

Implemente_instanciable(Modele_turbulence_hyd_LES_Smago_VEF, "Modele_turbulence_hyd_sous_maille_Smago_VEF", Modele_turbulence_hyd_LES_VEF_base);

Sortie& Modele_turbulence_hyd_LES_Smago_VEF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Modele_turbulence_hyd_LES_Smago_VEF::readOn(Entree& is) { return Modele_turbulence_hyd_LES_VEF_base::readOn(is); }

void Modele_turbulence_hyd_LES_Smago_VEF::set_param(Param& param)
{
  Modele_turbulence_hyd_LES_VEF_base::set_param(param);
  param.ajouter("cs", &cs_);
  param.ajouter_condition("value_of_cs_ge_0", "sous_maille_smago model constant must be positive.");
}

Champ_Fonc& Modele_turbulence_hyd_LES_Smago_VEF::calculer_viscosite_turbulente()
{
  const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, le_dom_VF_.valeur());
  const int nb_elem = domaine_VEF.nb_elem();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot();
  if (SMA_barre_.size_array()==0)
    SMA_barre_.resize(nb_elem_tot);

  calculer_S_barre();

  DoubleTab& visco_turb = la_viscosite_turbulente_->valeurs();
  if (visco_turb.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }

  //const double cs = cs_;
  CDoubleArrView l_v = l_.view_ro();
  CDoubleArrView SMA_barre_v = SMA_barre_.view_ro();
  DoubleTabView visco_turb_v = visco_turb.view_wo();
#if __cplusplus == 201402L
  // C++14:
  const double cs = cs_;
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem, KOKKOS_LAMBDA(
                         const int elem)
  {
    visco_turb_v(elem,0) = cs*cs*l_v(elem)*l_v(elem)*sqrt(SMA_barre_v(elem));
  });
#else
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem, KOKKOS_CLASS_LAMBDA(
                         const int elem)
  {
    // KOKKOS tips: Using a KOKKOS_LAMBDA on CUDA with member class (here cs_) -> crash
    // Use KOKKOS_CLASS_LAMBDA instead (but C++17 feature) : it copies the whole instance on the device) or local copy the member attribute
    // See https://github.com/kokkos/kokkos/issues/695
    visco_turb_v(elem,0) = cs_*cs_*l_v(elem)*l_v(elem)*sqrt(SMA_barre_v(elem));
  });
#endif
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  double temps = mon_equation_->inconnue()->temps();
  la_viscosite_turbulente_->changer_temps(temps);
  return la_viscosite_turbulente_;
}

void Modele_turbulence_hyd_LES_Smago_VEF::calculer_S_barre()
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = ref_cast(Domaine_Cl_VEF, le_dom_Cl_.valeur());
  const DoubleTab& la_vitesse = mon_equation_->inconnue()->valeurs();

  Champ_P1NC::calcul_S_barre(la_vitesse, SMA_barre_, domaine_Cl_VEF);
}
