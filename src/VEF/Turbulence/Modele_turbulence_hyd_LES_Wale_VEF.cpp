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

#include <Modele_turbulence_hyd_LES_Wale_VEF.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Domaine_VEF.h>
#include <Champ_P1NC.h>
#include <TRUSTTrav.h>
#include <Param.h>
#include <Debog.h>

Implemente_instanciable_sans_constructeur(Modele_turbulence_hyd_LES_Wale_VEF, "Modele_turbulence_hyd_sous_maille_Wale_VEF", Modele_turbulence_hyd_LES_VEF_base);

Modele_turbulence_hyd_LES_Wale_VEF::Modele_turbulence_hyd_LES_Wale_VEF()
{
  declare_support_masse_volumique(1);
}

Sortie& Modele_turbulence_hyd_LES_Wale_VEF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Modele_turbulence_hyd_LES_Wale_VEF::readOn(Entree& is) { return Modele_turbulence_hyd_LES_VEF_base::readOn(is); }

void Modele_turbulence_hyd_LES_Wale_VEF::set_param(Param& param)
{
  Modele_turbulence_hyd_LES_VEF_base::set_param(param);
  param.ajouter("cw", &cw_);
  param.ajouter_condition("value_of_cw_ge_0", "sous_maille_Wale model constant must be positive.");
}

Champ_Fonc_base& Modele_turbulence_hyd_LES_Wale_VEF::calculer_viscosite_turbulente()
{
  const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, le_dom_VF_.valeur());
  double temps = mon_equation_->inconnue().temps();
  DoubleTab& tab_visco_turb = la_viscosite_turbulente_->valeurs();
  const int nb_elem = domaine_VEF.nb_elem();
  const DoubleTab& la_vitesse = mon_equation_->inconnue().valeurs();
  const Domaine_Cl_VEF& domaine_Cl_VEF = ref_cast(Domaine_Cl_VEF, le_dom_Cl_.valeur());

  if (tab_visco_turb.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }
  Debog::verifier("Modele_turbulence_hyd_LES_Wale_VEF::calculer_viscosite_turbulente visco_turb 0", tab_visco_turb);

  const int nb_elem_tot = domaine_VEF.nb_elem_tot();
  DoubleTrav tab_duidxj(nb_elem_tot,dimension,dimension);
  // Patrick : on travaille sur le champ filtre.
  //const Champ_P1NC& ch=(const Champ_P1NC&) mon_equation->inconnue();
  //DoubleTab ubar(la_vitesse);
  //ch.filtrer_L2(ubar);
  //Champ_P1NC::calcul_gradient(ubar,duidxj,domaine_Cl_VEF);
  Champ_P1NC::calcul_gradient(la_vitesse,tab_duidxj,domaine_Cl_VEF);

  double cw = cw_; // Necessary to avoid kernel crash
  int dimension = Objet_U::dimension;

  CDoubleArrView l = l_.view_ro();
  CDoubleTabView3 duidxj = tab_duidxj.view3_ro();
  CDoubleArrView visco_turb = static_cast<DoubleVect&>(tab_visco_turb).view_rw();

  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                       nb_elem,
                       KOKKOS_LAMBDA (int elem)
  {
    double gij2[3][3] {};
    double sd[3][3] {};

    // Calcul du terme gij2.
    for (int i = 0; i < dimension; i++)
      for (int j = 0; j < dimension; j++)
        {
          gij2[i][j] = 0;
          for (int k = 0; k < dimension; k++)
            gij2[i][j] += duidxj(elem,i,k) * duidxj(elem,k,j);
        }

    // Calcul du terme gkk2.
    double gkk2 = 0;
    for (int k = 0; k < dimension; k++)
      gkk2 += gij2[k][k];

    // Calcul de sd.
    for (int i = 0; i < dimension; i++)
      for (int j = 0; j < dimension; j++)
        {
          sd[i][j] = 0.5 * (gij2[i][j] + gij2[j][i]);
          if (i == j)
            sd[i][j] -= gkk2 / 3.; // Terme derriere le tenseur de Kronecker.
        }

    // Calcul de sd2 et Sij2.
    double sd2 = 0.;
    double Sij2 = 0.;
    for (int i = 0; i < dimension; i++)
      for (int j = 0; j < dimension; j++)
        {
          sd2 += sd[i][j] * sd[i][j];
          //Deplacement du calcul de Sij
          double Sij = 0.5 * (duidxj(elem,i,j) + duidxj(elem,j,i));
          Sij2 += Sij * Sij;
        }

    // Calcul de OP1 et OP2.
    // Replace pow by sqrt and multiply, faster
    //OP1=pow(sd2,1.5);
    double OP1 = sd2 * sqrt(sd2);
    //OP2=pow(Sij2,2.5)+pow(sd2,1.25);
    double OP2 = Sij2 * Sij2 * sqrt(Sij2) + sd2 * sqrt(sqrt(sd2));

    if (OP1 != 0.) // donc sd2 et OP2 par voie de consequence sont differents de zero
      visco_turb(elem) = cw * cw * l(elem) * l(elem) * OP1 / OP2;
    else
      visco_turb(elem) = 0;
  }); // fin de la boucle sur les elements
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  Debog::verifier("Modele_turbulence_hyd_LES_Wale_VEF::calculer_viscosite_turbulente visco_turb 1", tab_visco_turb);

  la_viscosite_turbulente_->changer_temps(temps);
  return la_viscosite_turbulente_;
}
