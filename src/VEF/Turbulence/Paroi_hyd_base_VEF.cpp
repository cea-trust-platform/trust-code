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
// File:        Paroi_hyd_base_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////


#include <Paroi_hyd_base_VEF.h>
#include <Scatter.h>
#include <Zone_Cl_dis.h>

Implemente_base_sans_constructeur(Paroi_hyd_base_VEF,"Paroi_hyd_base_VEF",Turbulence_paroi_base);
Paroi_hyd_base_VEF::Paroi_hyd_base_VEF():flag_face_keps_imposee_(0)
{
  ;
}

//     printOn()
/////

Sortie& Paroi_hyd_base_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_hyd_base_VEF::readOn(Entree& s)
{
  Turbulence_paroi_base::readOn(s);
  return s ;
}


/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_hyd_base_VEF
//
/////////////////////////////////////////////////////////////////////

void Paroi_hyd_base_VEF::associer(const Zone_dis& zone_dis,const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VEF = ref_cast(Zone_VEF,zone_dis.valeur());
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF,zone_Cl_dis.valeur());
}

void Paroi_hyd_base_VEF::init_lois_paroi_()
{
  const Zone_VF& zvf = la_zone_VEF.valeur();
  const int nb_faces_bord = la_zone_VEF->nb_faces_bord();
  tab_u_star_.resize(nb_faces_bord);
  Cisaillement_paroi_.resize(0, dimension);
  zvf.creer_tableau_faces_bord(Cisaillement_paroi_);
}

DoubleTab& Paroi_hyd_base_VEF::corriger_derivee_impl(DoubleTab& d) const
{
  int size=d.dimension_tot(0);
  assert(size==face_keps_imposee_.size_array());
  for (int face=0; face<size; face++)
    {
      if (face_keps_imposee_(face)!=-2)
        {
          d(face,0)=0;
          d(face,1)=0;
        }
    }
  return d;
}


