/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Convection_Diffusion_Espece_Binaire_WC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Weakly_Compressible/Equations
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Espece_Binaire_WC.h>
#include <Fluide_Weakly_Compressible.h>
#include <Loi_Etat_Binaire_GP_WC.h>
#include <EcritureLectureSpecial.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Avanc.h>

Implemente_instanciable(Convection_Diffusion_Espece_Binaire_WC,"Convection_Diffusion_Espece_Binaire_WC",Convection_Diffusion_Espece_Binaire_base);

Sortie& Convection_Diffusion_Espece_Binaire_WC::printOn(Sortie& is) const
{
  return Convection_Diffusion_Espece_Binaire_base::printOn(is);
}

Entree& Convection_Diffusion_Espece_Binaire_WC::readOn(Entree& is)
{
  return Convection_Diffusion_Espece_Binaire_base::readOn(is);
}

void Convection_Diffusion_Espece_Binaire_WC::completer()
{
  assert(le_fluide->loi_etat().valeur().que_suis_je() == "Loi_Etat_Binaire_Gaz_Parfait_WC");
  Convection_Diffusion_Espece_Binaire_base::completer();
}

int Convection_Diffusion_Espece_Binaire_WC::sauvegarder(Sortie& os) const
{
  int bytes=0,a_faire,special;
  bytes += Equation_base::sauvegarder(os);
  EcritureLectureSpecial::is_ecriture_special(special,a_faire);

  if (a_faire)
    {
      Fluide_Weakly_Compressible& FWC = ref_cast_non_const(Fluide_Weakly_Compressible,le_fluide.valeur());
      Champ_Inc p_tab = FWC.inco_chaleur(); // Initialize with same discretization
      p_tab->nommer("Pression_EOS");
      p_tab->valeurs() = FWC.pression_th_tab(); // Use good values
      if (special && Process::nproc() > 1)
        Cerr << "ATTENTION : For a parallel calculation, the field Pression_EOS is not saved in xyz format ... " << finl;
      else
        bytes += p_tab->sauvegarder(os);
    }

  return bytes;
}

int Convection_Diffusion_Espece_Binaire_WC::reprendre(Entree& is)
{
  // start resuming
  Convection_Diffusion_Espece_Binaire_base::reprendre(is);
  // XXX : should be set so that Pression_EOS is read and not initialized from data file
  Fluide_Weakly_Compressible& FWC = ref_cast(Fluide_Weakly_Compressible,le_fluide.valeur());
  FWC.set_resume_flag();
  // resume EOS pressure field
  double temps = schema_temps().temps_courant();
  Champ_Inc p_tab = inconnue(); // Same discretization normally
  p_tab->nommer("Pression_EOS");
  Nom field_tag(p_tab->le_nom());
  field_tag += p_tab.valeur().que_suis_je();
  field_tag += probleme().domaine().le_nom();
  field_tag += Nom(temps,probleme().reprise_format_temps());

  if (EcritureLectureSpecial::is_lecture_special() && Process::nproc() > 1)
    {
      Cerr << "Error in Convection_Diffusion_Espece_Binaire_WC::reprendre !" << finl;
      Cerr << "Use the sauv file to resume a parallel WC calculation (Pression_EOS is required) ... " << finl;
      Process::exit();
    }
  else
    {
      avancer_fichier(is, field_tag);
      p_tab->reprendre(is);
    }

  // set good field
  FWC.set_pression_th_tab(p_tab->valeurs());

  return 1;
}
