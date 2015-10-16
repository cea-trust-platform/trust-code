/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Paroi_hyd_base_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////


#include <Paroi_hyd_base_VDF.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Fluide_Quasi_Compressible.h>
#include <Zone_Cl_dis.h>

Implemente_base(Paroi_hyd_base_VDF,"Paroi_hyd_base_VDF",Turbulence_paroi_base);

//     printOn()
/////

Sortie& Paroi_hyd_base_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_hyd_base_VDF::readOn(Entree& s)
{
  Turbulence_paroi_base::readOn(s);
  return s ;
}


/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_hyd_base_VDF
//
/////////////////////////////////////////////////////////////////////

void Paroi_hyd_base_VDF::associer(const Zone_dis& zone_dis,const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF,zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());
}

void Paroi_hyd_base_VDF::init_lois_paroi_()
{
  tab_u_star_.resize(la_zone_VDF->nb_faces_bord());
  if (!Cisaillement_paroi_.get_md_vector().non_nul())
    {
      Cisaillement_paroi_.resize(0, dimension);
      la_zone_VDF.valeur().creer_tableau_faces_bord(Cisaillement_paroi_);
    }
}
