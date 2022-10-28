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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Boundary_Conditions_Thermique.cpp
// Directory : $IJK_ROOT/src/Temperature
//
/////////////////////////////////////////////////////////////////////////////

#include <Boundary_Conditions_Thermique.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur( Boundary_Conditions_Thermique, "Boundary_Conditions_Thermique", Objet_U ) ;
Implemente_ref(Boundary_Conditions_Thermique);

static const double invalid_thermal_value = 12345e30;

Boundary_Conditions_Thermique::Boundary_Conditions_Thermique()
{
  bctype_kmin_ = Paroi_Temperature_imposee;
  bctype_kmax_ = Paroi_Temperature_imposee;
  temperature_imposee_kmin_ = invalid_thermal_value;
  temperature_imposee_kmax_ = invalid_thermal_value;
  flux_impose_kmin_ = invalid_thermal_value;
  flux_impose_kmax_ = invalid_thermal_value;
}

// SYNTAXE:
//  {
//    BCTYPE_KMIN Paroi_Temperature_imposee|Paroi_Flux_impose|perio
//    temperature_imposee_kmin double (optional)
//    flux_impose_kmin double (optional)
//    BCTYPE_KMAX Paroi_Temperature_imposee|Paroi_Flux_impose|perio
//


Sortie& Boundary_Conditions_Thermique::printOn( Sortie& os ) const
{
  Objet_U::printOn( os );
  return os;
}

Entree& Boundary_Conditions_Thermique::readOn( Entree& is )
{
  Param param(que_suis_je());

  param.ajouter("bctype_kmin", &bctype_kmin_, Param::REQUIRED);
  param.dictionnaire("Paroi_Temperature_imposee", Paroi_Temperature_imposee);
  param.dictionnaire("Paroi_Flux_impose", Paroi_Flux_impose);
  param.dictionnaire("Perio", Perio);

  param.ajouter("bctype_kmax", &bctype_kmax_, Param::REQUIRED);
  param.dictionnaire("Paroi_Temperature_imposee", Paroi_Temperature_imposee);
  param.dictionnaire("Paroi_Flux_impose", Paroi_Flux_impose);
  param.dictionnaire("Perio", Perio);
//  param.ajouter_condition("IS_READ_TEMPERATURE_IMPOSEE_KMIN && VALUE_OF_TEMPERATURE_IMPOSEE_KMAX","Flux or Temperature must be associated to the BC if not perio");

  param.ajouter("temperature_imposee_kmin", &temperature_imposee_kmin_);
  param.ajouter("temperature_imposee_kmax", &temperature_imposee_kmax_);
  param.ajouter("flux_impose_kmin", &flux_impose_kmin_);
  param.ajouter("flux_impose_kmax", &flux_impose_kmax_);
  param.lire_avec_accolades(is);

  // Check whether kmin BC is correctly defined
  if ((get_bctype_k_min() == Paroi_Temperature_imposee) && (temperature_imposee_kmin_>invalid_thermal_value*0.99))
    {
      Cerr << "Value of temperature for kmin Boundary Condition is missing." << finl;
      Process::exit();
    }
  else if ((get_bctype_k_min() == Paroi_Flux_impose) && (flux_impose_kmin_>invalid_thermal_value*0.99))
    {
      Cerr << "Value of flux for kmin Boundary Condition is missing." << finl;
      Process::exit();
    }

  // Check whether kmax BC is correctly defined
  if ((get_bctype_k_max() == Paroi_Temperature_imposee) && (temperature_imposee_kmax_>invalid_thermal_value*0.99))
    {
      Cerr << "Value of temperature for kmax Boundary Condition is missing." << finl;
      Process::exit();
    }
  else if ((get_bctype_k_max() == Paroi_Flux_impose) && (flux_impose_kmax_>invalid_thermal_value*0.99))
    {
      Cerr << "Value of flux for kmax Boundary Condition is missing." << finl;
      Process::exit();
    }
  return is;
}
