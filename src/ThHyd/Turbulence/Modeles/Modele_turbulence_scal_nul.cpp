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

#include <Modele_turbulence_scal_nul.h>
#include <Modele_turbulence_hyd_nul.h>
#include <Convection_Diffusion_std.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>

Implemente_instanciable(Modele_turbulence_scal_nul, "Modele_turbulence_scal_nul", Modele_turbulence_scal_base);

Sortie& Modele_turbulence_scal_nul::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Modele_turbulence_scal_nul::readOn(Entree& is)
{
  // Creation d'une loi de paroi nulle:
  const Nom& discr = mon_equation->discretisation().que_suis_je();
  const Probleme_base& le_pb = mon_equation->probleme();
  // lp loi de paroi du modele de turbulence de l'hydraulique
  const RefObjU& modele_turbulence = le_pb.equation(0).get_modele(TURBULENCE);
  const Modele_turbulence_hyd_base& mod_turb_hydr = ref_cast(Modele_turbulence_hyd_base, modele_turbulence.valeur());
  if (!sub_type(Modele_turbulence_hyd_nul, mod_turb_hydr))
    {
      Cerr << "Error in Modele_turbulence_scal_nul::readOn !!!" << finl;
      Cerr << "You use a NUL turbulence model for the scalar equation " << mon_equation->que_suis_je() << " together with a non NUL turbulence model for " << le_pb.equation(0).que_suis_je() << finl;
      Cerr << "This is impossible !!! Replace the model " << mod_turb_hydr.que_suis_je() << " by the NUL model !!!" << finl;
      Process::exit();
    }

  loipar.associer_modele(*this);
  if (discr == "VEF" || discr == "VEFPreP1B") loipar.typer("negligeable_scalaire_VEF");
  else if (discr == "VDF") loipar.typer("negligeable_scalaire_VDF");
  else if (discr == "EF") loipar.typer("negligeable_scalaire_EF");
  else
    {
      Cerr << "Erreur dans Modele_turbulence_scal_nul::readOn : la discretisation " << discr << " n'est pas prise en charge" << finl;
      Process::exit();
    }
  loipar.valeur().associer_modele(*this);
  loipar.valeur().associer(le_pb.equation(0).domaine_dis(), le_pb.equation(0).domaine_Cl_dis());
  return is;
}




