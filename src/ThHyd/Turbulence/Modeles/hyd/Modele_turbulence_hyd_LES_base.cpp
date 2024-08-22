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

#include <Modele_turbulence_hyd_LES_base.h>
#include <Modifier_pour_fluide_dilatable.h>
#include <Discretisation_base.h>
#include <stat_counters.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Param.h>
#include <kokkos++.h>
#include <TRUSTArray_kokkos.tpp>
#include <TRUSTTab_kokkos.tpp>

Implemente_base_sans_constructeur(Modele_turbulence_hyd_LES_base, "Modele_turbulence_hyd_LES_base", Modele_turbulence_hyd_0_eq_base);

// XD form_a_nb_points objet_lecture nul 0 The structure fonction is calculated on nb points and we should add the 2 directions (0:OX, 1:OY, 2:OZ) constituting the homegeneity planes. Example for channel flows, planes parallel to the walls.
// XD  attr nb entier(into=[4]) nb 0 Number of points.
// XD  attr dir1 entier(max=2) dir1 0 First direction.
// XD  attr dir2 entier(max=2) dir2 0 Second direction.

// XD mod_turb_hyd_ss_maille modele_turbulence_hyd_deriv mod_turb_hyd_ss_maille -1 Class for sub-grid turbulence model for Navier-Stokes equations.
// XD attr formulation_a_nb_points form_a_nb_points formulation_a_nb_points 1 The structure fonction is calculated on nb points and we should add the 2 directions (0:OX, 1:OY, 2:OZ) constituting the homegeneity planes. Example for channel flows, planes parallel to the walls.

Modele_turbulence_hyd_LES_base::Modele_turbulence_hyd_LES_base()
{
  methode_ = "volume"; // Parametre par defaut pour calculer la longueur caracteristique
}

Sortie& Modele_turbulence_hyd_LES_base::printOn(Sortie& is) const
{
  return Modele_turbulence_hyd_0_eq_base::printOn(is);
}

Entree& Modele_turbulence_hyd_LES_base::readOn(Entree& is)
{
  return Modele_turbulence_hyd_0_eq_base::readOn(is);
}

void Modele_turbulence_hyd_LES_base::set_param(Param& param)
{
  Modele_turbulence_hyd_base::set_param(param);
  param.ajouter("longueur_maille", &methode_); // XD_ADD_P chaine(into=["volume","volume_sans_lissage","scotti","arrete"]) Different ways to calculate the characteristic length may be specified : NL2 volume : It is the default option. Characteristic length is based on the cubic root of the volume cells. A smoothing procedure is applied to avoid discontinuities of this quantity in VEF from a cell to another. NL2 volume_sans_lissage : For VEF only. Characteristic length is based on the cubic root of the volume cells (without smoothing procedure).NL2 scotti : Characteristic length is based on the cubic root of the volume cells and the Scotti correction is applied to take into account the stretching of the cell in the case of anisotropic meshes. NL2 arete : For VEF only. Characteristic length relies on the max edge (+ smoothing procedure) is taken into account.

}

int Modele_turbulence_hyd_LES_base::preparer_calcul()
{
  Modele_turbulence_hyd_base::preparer_calcul();
  if (methode_ != "??")
    Cerr << "The characteristic length of the subgrid scale model is calculated with the method : " << methode_ << finl;
  calculer_longueurs_caracteristiques();
  mettre_a_jour(0.);
  return 1;
}

// E.Saikali
// In the current version of TrioCFD and the FT part in particular, it is possible only to use LES WALE (Smagorinsky,1963,Mon.Weather Rev. 91,99)
// For this reason, associating the wall laws of multi-phase flows is done in this method Modele_turbulence_hyd_LES_base::completer()
// We only go inside if the user defines the wall law as loi_standard_hydr, for both discretizations VEF and VDF
// Otherwise, in the case of negligeable, the multiphase wall law is not used
void Modele_turbulence_hyd_LES_base::verifie_loi_paroi_diphasique()
{
  const Milieu_base& mil = equation().milieu(); // returns Fluide_Diphasique or Fluide_Incompressible
  const Nom& nom_mil = mil.que_suis_je();
  const Nom& nom_loipar = loipar_->que_suis_je();
  const Nom& nom_eq = equation().que_suis_je();

  if ((nom_loipar == "loi_standard_hydr_VEF" || nom_loipar == "loi_standard_hydr_VDF") && nom_eq == "Navier_Stokes_FT_Disc" && nom_mil == "Fluide_Diphasique")
    {
      const Nom& discr = equation().discretisation().que_suis_je();
      if (discr == "VEF" || discr == "VEFPreP1B")
        {
          loipar_.typer("loi_standard_hydr_diphasique_VEF");
          Cerr << "Associating the two-phase hydraulic wall turbulence model : loi_standard_hydr_diphasique_VEF ..." << finl;
        }
      else if (discr == "VDF")
        {
          loipar_.typer("loi_standard_hydr_diphasique_VDF");
          Cerr << "Associating the two-phase hydraulic wall turbulence model : loi_standard_hydr_diphasique_VDF ..." << finl;
        }
      else
        {
          Cerr << "Unknown discretization type !!!";
          Process::exit();
        }
      loipar_->associer_modele(*this);
      loipar_->associer(equation().domaine_dis(), equation().domaine_Cl_dis());
    }
}

void Modele_turbulence_hyd_LES_base::completer()
{
  Cerr << "Modele_turbulence_hyd_LES_base::completer()" << finl;
  verifie_loi_paroi_diphasique();
}

void Modele_turbulence_hyd_LES_base::calculer_energie_cinetique_turb()
{
  DoubleVect& k = energie_cinetique_turb_.valeurs();
  DoubleTab& visco_turb = la_viscosite_turbulente_.valeurs();
  double Cq = 0.094;

  // PQ : 10/08/06 : on utilise ici la formule de Schuman : q_sm = (nu_t)^2 / (Cq.l)^2

  const int nb_elem = visco_turb.size();
  if (k.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent kinetic energy." << finl;
      exit();
    }

  CDoubleArrView l_v = l_.view_ro();
  CDoubleTabView visco_turb_v = visco_turb.view_ro();
  DoubleArrView k_v = k.view_rw();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem, KOKKOS_LAMBDA(
                         const int elem)
  {
    k_v(elem)=visco_turb_v(elem, 0)*visco_turb_v(elem, 0)/(Cq*Cq*l_v(elem)*l_v(elem));
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);

  double temps = mon_equation_->inconnue()->temps();
  energie_cinetique_turb_->changer_temps(temps);
}

