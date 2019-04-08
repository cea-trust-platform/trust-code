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
// File:        Op_Diff_K_Eps_PolyMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_K_Eps_PolyMAC_Elem.h>
#include <Modele_turbulence_hyd_K_Eps.h>
#include <Discretisation_base.h>

Implemente_instanciable( Op_Diff_K_Eps_PolyMAC_Elem, "Op_Diff_K_Eps_PolyMAC_Elem|Op_Diff_K_Eps_PolyMAC_P0_PolyMAC", Op_Diff_K_Eps_base ) ;

Sortie& Op_Diff_K_Eps_PolyMAC_Elem::printOn( Sortie& os ) const
{
  // op_polymac.printOn( os );
  return os;
}

Entree& Op_Diff_K_Eps_PolyMAC_Elem::readOn( Entree& is )
{
  // op_polymac.readOn( is );
  return is;
}

void Op_Diff_K_Eps_PolyMAC_Elem::completer()
{
  mon_equation->discretisation().discretiser_champ("champ_elem",mon_equation->zone_dis().valeur(),"visc","1",2,0.,diffusivite_turbulente_);
  const Transport_K_Eps& eqn_transport = ref_cast(Transport_K_Eps,mon_equation.valeur());
  const Modele_turbulence_hyd_K_Eps& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps,eqn_transport.modele_turbulence());
  const Champ_Fonc& visc_turb = mod_turb.viscosite_turbulente();
  vraie_diffusivite_turbulente_ = visc_turb;
  associer_Pr_K_Eps(mod_turb.get_Prandtl_K(),mod_turb.get_Prandtl_Eps());
  op_polymac.associer_eqn(mon_equation);
  op_polymac.completer();
}

void Op_Diff_K_Eps_PolyMAC_Elem::associer(const Zone_dis& zone_dis,
                                          const Zone_Cl_dis& zone_cl_dis,
                                          const Champ_Inc& ch_diffuse)
{
  op_polymac.associer(zone_dis, zone_cl_dis, ch_diffuse);
}

void Op_Diff_K_Eps_PolyMAC_Elem::associer_diffusivite(const Champ_base& diffu)
{
  op_polymac.associer_diffusivite(diffu);
}

void Op_Diff_K_Eps_PolyMAC_Elem::associer_diffusivite_turbulente()
{
  op_polymac.associer_diffusivite_turbulente(diffusivite_turbulente_);
}

DoubleTab& Op_Diff_K_Eps_PolyMAC_Elem::ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  calculer_nu_sur_pr();
  return op_polymac.ajouter(inconnue, resu);
}

void Op_Diff_K_Eps_PolyMAC_Elem::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  calculer_nu_sur_pr();
  op_polymac.contribuer_a_avec(inco, matrice);
}

DoubleTab& Op_Diff_K_Eps_PolyMAC_Elem::calculer(const DoubleTab& inconnue , DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inconnue,resu);
}

void Op_Diff_K_Eps_PolyMAC_Elem::dimensionner(Matrice_Morse& mat) const
{
  op_polymac.dimensionner(mat);
}

void Op_Diff_K_Eps_PolyMAC_Elem::modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const
{
  op_polymac.modifier_pour_Cl(la_matrice, secmem);
}

void Op_Diff_K_Eps_PolyMAC_Elem::calculer_nu_sur_pr() const
{
  const double Pr[2] = {Prdt_K, Prdt_Eps};
  const DoubleTab& diffu_t = vraie_diffusivite_turbulente_.valeur().valeurs();
  DoubleTab& diffu_pr = diffusivite_turbulente_.valeurs();
  for (int i = 0; i < diffu_pr.dimension(0); i++) for (int j = 0; j < 2; j++)
      diffu_pr(i, j) = diffu_t[i] / Pr[j];
}
