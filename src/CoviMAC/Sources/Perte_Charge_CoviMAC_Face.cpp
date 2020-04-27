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
// File:        Perte_Charge_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Sources
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Perte_Charge_CoviMAC_Face.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Zone_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>

Implemente_base(Perte_Charge_CoviMAC_Face,"Perte_Charge_CoviMAC_Face",Source_base);



//// printOn
//

Sortie& Perte_Charge_CoviMAC_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je();
}


//// readOn
//

Entree& Perte_Charge_CoviMAC_Face::readOn(Entree& s )
{
  return s ;
}

void Perte_Charge_CoviMAC_Face::mettre_a_jour(double temps)
{
  ;
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe Perte_Charge_CoviMAC_Face
//
////////////////////////////////////////////////////////////////////

void Perte_Charge_CoviMAC_Face::associer_pb(const Probleme_base& pb)
{
  Cerr << " Perte_Charge_CoviMAC_Face::associer_pb " << finl ;
  int nb_eqn = pb.nombre_d_equations();
  int ok=0;
  for (int i=0; i<nb_eqn; i++)
    {
      const Equation_base& eqn = pb.equation(i);
      if  (sub_type(Navier_Stokes_std,eqn))
        {
          la_vitesse = ref_cast(Champ_Face_CoviMAC,eqn.inconnue().valeur());
          le_fluide = ref_cast(Fluide_Incompressible,eqn.milieu());
          associer_zones(eqn.zone_dis(),eqn.zone_Cl_dis());
          i = nb_eqn;
          ok = 1;
        }
    }

  if (!ok)
    {
      Cerr << "Erreur TRUST dans Perte_Charge_CoviMAC_Face::associer_pb()" << finl;
      Cerr << "On ne trouve pas d'equation d'hydraulique dans le probleme" << finl;
      exit();
    }
}

void Perte_Charge_CoviMAC_Face::associer_zones(const Zone_dis& zone_dis,
                                               const Zone_Cl_dis& zone_Cl_dis)
{
  Cerr << " Perte_Charge_CoviMAC_Face::associer_zones " << finl ;
  la_zone_CoviMAC = ref_cast(Zone_CoviMAC, zone_dis.valeur());
  la_zone_Cl_CoviMAC = ref_cast(Zone_Cl_CoviMAC, zone_Cl_dis.valeur());
}



