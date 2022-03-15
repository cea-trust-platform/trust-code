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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Terme_Source_Canal_perio_VDF_P0.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Canal_perio_VDF_P0.h>
#include <Zone_VF.h>
#include <Zone_VDF.h>
#include <Convection_Diffusion_std.h>

Implemente_instanciable(Terme_Source_Canal_perio_VDF_P0, "Canal_perio_VDF_P0_VDF", Terme_Source_Canal_perio_VDF_Face);

//// printOn
//
Sortie& Terme_Source_Canal_perio_VDF_P0::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//
Entree& Terme_Source_Canal_perio_VDF_P0::readOn(Entree& s )
{
  return Terme_Source_Canal_perio_VDF_Face::readOn(s);
}

ArrOfDouble Terme_Source_Canal_perio_VDF_P0::source_convection_diffusion(double debit_e) const
{
  // Compute heat_flux:
  double heat_flux = compute_heat_flux();

  const Zone_VF& zone_vf = ref_cast(Zone_VF,equation().zone_dis().valeur());
  const double volume = zone_vf.zone().volume_total();
  int size = zone_vf.nb_elem();
  ArrOfDouble s(size);
  if (velocity_weighting_) // It seems this algorithm do not imply dT/dt -> 0
    {
      Cerr << "Option 'velocity_weighting' of source term 'canal_perio' in '" << equation().que_suis_je() << "' is not supported!!" << finl;
      Cerr << "Contact TRUST support" << finl;
      exit();
    }
  else
    {
      // Compute source term with
      // Source = -Sum(imposed_heat_flux)/Volume
      // Loop on the faces
      for (int num_elem = 0; num_elem < size; num_elem++)
        s(num_elem) = -heat_flux/volume;
    }
  return s;
}

DoubleTab& Terme_Source_Canal_perio_VDF_P0::ajouter(DoubleTab& resu) const
{
  const Zone_VF& zone_VF = la_zone_VDF.valeur();
  const DoubleVect& volumes = zone_VF.volumes();
  ArrOfDouble s(source());

  // Boucle sur les elements internes
  int nb_elem = zone_VF.nb_elem();
  for (int num_elem = 0; num_elem < nb_elem; num_elem++)
    {
      double vol = volumes(num_elem);
      resu(num_elem)+= s(num_elem)*vol;
    }

  return resu;
}
