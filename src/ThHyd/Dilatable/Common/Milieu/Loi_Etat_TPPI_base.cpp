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

#include <Fluide_Dilatable_base.h>
#include <Loi_Etat_TPPI_base.h>
#include <Champ_Uniforme.h>

#include <Param.h>

Implemente_base(Loi_Etat_TPPI_base, "Loi_Etat_TPPI_base", Loi_Etat_Mono_GP_base);

Sortie& Loi_Etat_TPPI_base::printOn(Sortie& os) const { return os << que_suis_je() << finl; }

Entree& Loi_Etat_TPPI_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("model|modele", &model_name_, Param::REQUIRED);
  param.ajouter("fluid|fluide", &fluid_name_, Param::REQUIRED);
  param.ajouter("Cp", &Cp_, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);

  return is;
}

double Loi_Etat_TPPI_base::inverser_Pth(double, double)
{
  Cerr << "Loi_Etat_TPPI_base::inverser_Pth should not be called !! " << finl;
  throw;
}

void Loi_Etat_TPPI_base::verify_fields()
{
  if (le_fluide->has_viscosite_dynamique())
    {
      Cerr << "Error in Loi_Etat_TPPI_base::verify_fields() ... Mu is not read in your medium and it must be calculated by EOS/CoolProp !!!" << finl;
      Process::exit();
    }
  Champ_Don_base& mu = le_fluide->viscosite_dynamique();
  if (sub_type(Champ_Uniforme, mu))
    {
      Cerr << "Error in Loi_Etat_TPPI_base::verify_fields() ... Mu should not be Champ_Uniforme since it must be calculated by EOS/CoolProp !!!" << finl;
      Process::exit();
    }

  if (!le_fluide->has_conductivite())
    {
      Cerr << "Error in Loi_Etat_TPPI_base::verify_fields() ... Lambda is not read in your medium and it must be calculated by EOS/CoolProp !!!" << finl;
      Process::exit();
    }
  Champ_Don_base& lambda = le_fluide->conductivite();
  if (sub_type(Champ_Uniforme, lambda))
    {
      Cerr << "Error in Loi_Etat_TPPI_base::verify_fields() ... Lambda should not be Champ_Uniforme since it must be calculated by EOS/CoolProp !!!" << finl;
      Process::exit();
    }

  if (!le_fluide->has_diffusivite())
    {
      Cerr << "Error in Loi_Etat_TPPI_base::verify_fields() ... Alpha is not read in your medium and it must be calculated by EOS/CoolProp !!!" << finl;
      Process::exit();
    }
  Champ_Don_base& alpha = le_fluide->diffusivite();
  if (sub_type(Champ_Uniforme, alpha))
    {
      Cerr << "Error in Loi_Etat_TPPI_base::verify_fields() ... Alpha should not be Champ_Uniforme since it must be calculated by EOS/CoolProp !!!" << finl;
      Process::exit();
    }
}

void Loi_Etat_TPPI_base::preparer_calcul()
{
  Loi_Etat_Mono_GP_base::preparer_calcul();
  verify_fields();
}

double Loi_Etat_TPPI_base::calculer_masse_volumique(double, double) const
{
  Cerr << "Loi_Etat_TPPI_base::calculer_masse_volumique should not be called !! " << finl;
  throw;
}
