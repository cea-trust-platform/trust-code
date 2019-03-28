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
// File:        Elem_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Elem_PolyMAC.h>
#include <Tri_PolyMAC.h>
#include <Tetra_PolyMAC.h>
#include <Quadri_PolyMAC.h>
#include <Hexa_PolyMAC.h>

Implemente_deriv(Elem_PolyMAC_base);
Implemente_instanciable(Elem_PolyMAC,"Elem_PolyMAC",DERIV(Elem_PolyMAC_base));



// printOn et readOn

Sortie& Elem_PolyMAC::printOn(Sortie& s ) const
{
  return s << valeur() ;
}

Entree& Elem_PolyMAC::readOn(Entree& s )
{
  Nom type;
  s >> type;
  if(type == "Tri_PolyMAC")
    *this =  Tri_PolyMAC();
  else if(type == "Tetra_PolyMAC")
    *this =  Tetra_PolyMAC();
  else if(type == "Quadri_PolyMAC")
    *this =  Quadri_PolyMAC();
  else if(type == "Hexa_PolyMAC")
    *this =  Hexa_PolyMAC();
  else
    {
      Cerr << type << " n'est pas un Elem_PolyMAC" << finl;
      exit();
    }
  return s ;
}

// Description:
// determination du type
void Elem_PolyMAC::typer(Nom type_elem_geom)
{
  Cerr << "Elem_PolyMAC::typer()" << finl ;
  Cerr << "type geometrique : " << type_elem_geom << finl;
  Nom type;
  if(type_elem_geom=="Triangle")
    type="Tri_PolyMAC";
  else if(type_elem_geom=="Tetraedre")
    type="Tetra_PolyMAC";
  else if(type_elem_geom=="Quadrangle")
    type="Quadri_PolyMAC";
  else if(type_elem_geom=="Hexaedre_VEF")
    type="Hexa_PolyMAC";
  else if(type_elem_geom=="Segment")
    type="Segment_PolyMAC";
  else if(type_elem_geom=="Polygone")
    type="Polygone_PolyMAC";
  else if(type_elem_geom=="Polyedre")
    type="Polyedre_PolyMAC";
  else
    {
      Cerr << "probleme de typage dans Elem_PolyMAC::typer" << finl;
      exit();
    }
  DERIV(Elem_PolyMAC_base)::typer(type);
  Cerr << "type retenu : " << valeur().que_suis_je() << finl ;
}
