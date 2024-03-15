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
#include <Champ_P1NC.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Param.h>
#include <Domaine_VEF.h>

Implemente_instanciable_sans_constructeur(Modele_turbulence_hyd_LES_Smago_VEF, "Modele_turbulence_hyd_sous_maille_Smago_VEF", Modele_turbulence_hyd_LES_VEF_base);

Modele_turbulence_hyd_LES_Smago_VEF::Modele_turbulence_hyd_LES_Smago_VEF()
{
  cs_ = 0.18;
}

Sortie& Modele_turbulence_hyd_LES_Smago_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Modele_turbulence_hyd_LES_Smago_VEF::readOn(Entree& is)
{
  Modele_turbulence_hyd_LES_VEF_base::readOn(is);
  return is;
}

void Modele_turbulence_hyd_LES_Smago_VEF::set_param(Param& param)
{
  Modele_turbulence_hyd_LES_VEF_base::set_param(param);
  param.ajouter("cs", &cs_);
  param.ajouter_condition("value_of_cs_ge_0", "sous_maille_smago model constant must be positive.");
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Modele_turbulence_hyd_LES_Smago_VEF
//
//////////////////////////////////////////////////////////////////////////////

Champ_Fonc& Modele_turbulence_hyd_LES_Smago_VEF::calculer_viscosite_turbulente()
{
  const Domaine_VEF& domaine_VEF = le_dom_VEF_.valeur();
  const int nb_elem = domaine_VEF.nb_elem();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot();
  SMA_barre_.resize(nb_elem_tot);

  calculer_S_barre();

  DoubleTab& visco_turb = la_viscosite_turbulente_.valeurs();
  if (visco_turb.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }
  for (int elem = 0; elem < nb_elem; elem++)
    visco_turb(elem) = cs_ * cs_ * l_(elem) * l_(elem) * sqrt(SMA_barre_[elem]);

  double temps = mon_equation_->inconnue().temps();
  la_viscosite_turbulente_.changer_temps(temps);
  return la_viscosite_turbulente_;
}

void Modele_turbulence_hyd_LES_Smago_VEF::calculer_S_barre()
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = le_dom_Cl_VEF_.valeur();
  const DoubleTab& la_vitesse = mon_equation_->inconnue().valeurs();

  Champ_P1NC::calcul_S_barre(la_vitesse, SMA_barre_, domaine_Cl_VEF);
}
