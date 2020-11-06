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
// File:        Elem_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Elem_CoviMAC.h>
#include <Tri_CoviMAC.h>
#include <Tetra_CoviMAC.h>
#include <Quadri_CoviMAC.h>
#include <Hexa_CoviMAC.h>

Implemente_deriv(Elem_CoviMAC_base);
Implemente_instanciable(Elem_CoviMAC,"Elem_CoviMAC",DERIV(Elem_CoviMAC_base));



// printOn et readOn

Sortie& Elem_CoviMAC::printOn(Sortie& s ) const
{
  return s << valeur() ;
}

Entree& Elem_CoviMAC::readOn(Entree& s )
{
  Nom type;
  s >> type;
  if(type == "Tri_CoviMAC")
    *this =  Tri_CoviMAC();
  else if(type == "Tetra_CoviMAC")
    *this =  Tetra_CoviMAC();
  else if(type == "Quadri_CoviMAC")
    *this =  Quadri_CoviMAC();
  else if(type == "Hexa_CoviMAC")
    *this =  Hexa_CoviMAC();
  else
    {
      Cerr << type << " n'est pas un Elem_CoviMAC" << finl;
      exit();
    }
  return s ;
}

// Description:
// determination du type
void Elem_CoviMAC::typer(Nom type_elem_geom)
{
  Cerr << "Elem_CoviMAC::typer()" << finl ;
  Cerr << "type geometrique : " << type_elem_geom << finl;
  Nom type;
  if(type_elem_geom=="Triangle")
    type="Tri_CoviMAC";
  else if(type_elem_geom=="Tetraedre")
    type="Tetra_CoviMAC";
  else if(type_elem_geom=="Quadrangle")
    type="Quadri_CoviMAC";
  else if(type_elem_geom=="Hexaedre_VEF")
    type="Hexa_CoviMAC";
  else if(type_elem_geom=="Segment")
    type="Segment_CoviMAC";
  else if(type_elem_geom=="Polygone")
    type="Polygone_CoviMAC";
  else if(type_elem_geom=="Polyedre")
    type="Polyedre_CoviMAC";
  else
    {
      Cerr << "probleme de typage dans Elem_CoviMAC::typer" << finl;
      exit();
    }
  DERIV(Elem_CoviMAC_base)::typer(type);
  Cerr << "type retenu : " << valeur().que_suis_je() << finl ;
}
