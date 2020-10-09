/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Residu_P1NC.cpp
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Residu_P1NC.h>
#include <Champ_P1NC.h>
#include <Zone_VEF.h>
#include <Schema_Temps_base.h>

Implemente_instanciable(Residu_P1NC,"Residu_P1NC",Champ_Fonc_P1NC);



Sortie& Residu_P1NC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}



Entree& Residu_P1NC::readOn(Entree& s)
{
  return s ;
}



void Residu_P1NC::associer_champ(const Champ_P1NC& un_champ)
{
  mon_champ_= un_champ;
}

void Residu_P1NC::associer_zone( const Zone_dis& zone_dis )
{
  zone_dis_ = zone_dis ;
}


void Residu_P1NC::me_calculer(double tps)
{
  int nb_faces = zone_vef( ).nb_faces( );
  const DoubleTab inco = mon_champ_.valeur( ).passe( );
  DoubleTab& champ = le_champ( ).valeurs( );

  if( nb_faces != inco.dimension( 0 ) )
    {
      Cerr << "Error in Residu_P1NC::me_calculer "<<finl;
      Cerr << "There are "<<nb_faces<<" faces and "<<inco.dimension( 0 )<<" values for the unknown field "<<finl;
      Process::abort( );
    }

  if( tps > 0.0 )
    champ = mon_champ_.valeur( ).equation( ).get_residuals();

  else
    {
      champ = -10000.0 ;
      Cerr << "[Information] Residu_P1NC::me_calculer : le residu est mis a -10000.0 au temps initial"<<finl;
    }
}


