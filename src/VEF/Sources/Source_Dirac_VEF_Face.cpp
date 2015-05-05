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
// File:        Source_Dirac_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Dirac_VEF_Face.h>
#include <Fluide_Incompressible.h>
#include <Navier_Stokes_std.h>
Implemente_instanciable_sans_constructeur(Source_Dirac_VEF_Face,"Dirac_VEF_P1NC",Terme_Puissance_Thermique_VEF_base);
implemente_It_Sou_VEF_Face(Eval_Dirac_VEF_Face)


Sortie& Source_Dirac_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


Entree& Source_Dirac_VEF_Face::readOn(Entree& s )
{
  Cerr << "Entree dans le readOn Terme_Source_Dirac_VEF_Face " << finl;
  point = 0;
  point.resize(dimension);
  s >> point;
  Terme_Puissance_Thermique_VEF_base::readOn(s);
  set_fichier("Source_Dirac");
  set_description("Heat power release = nb_dirac*P*V [W]");

  return s ;
}

void Source_Dirac_VEF_Face::associer_pb(const Probleme_base& pb )
{
  preparer_source(pb);
  Eval_Dirac_VEF_Face& eval_puis = (Eval_Dirac_VEF_Face&) iter.evaluateur();
  eval_puis.associer_champs(rho_ref.valeur(),Cp.valeur(),la_puissance);
  eval_puis.le_point.copy(point);
}

void Source_Dirac_VEF_Face::associer_zones(const Zone_dis& zone_dis,
                                           const Zone_Cl_dis& zone_cl_dis)
{
  Terme_Puissance_Thermique_VEF_base::associer_zones(zone_dis,zone_cl_dis);
  Eval_Dirac_VEF_Face& eval_grav = (Eval_Dirac_VEF_Face&) iter.evaluateur();
  eval_grav.associer_zones(zone_dis.valeur(),zone_cl_dis.valeur());

  int nb_elem = zone_dis.valeur().nb_elem();
  const Zone& ma_zone = zone_dis.valeur().zone();
  nb_dirac=0;
  for (int elem=0; elem<nb_elem; elem++)
    {
      int test =  ma_zone.type_elem().contient(point,elem) ;
      if (test == 1) nb_dirac++;
    }

  eval_grav.associer_nb_elem_dirac(nb_dirac);
}

