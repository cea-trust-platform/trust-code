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
// File:        Elem_PolyMAC_old.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC_old/Domaines
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Elem_PolyMAC_old.h>
#include <Tri_PolyMAC_old.h>
#include <Tetra_PolyMAC_old.h>
#include <Quadri_PolyMAC_old.h>
#include <Hexa_PolyMAC_old.h>

Implemente_instanciable(Elem_PolyMAC_old,"Elem_PolyMAC_old",DERIV(Elem_PolyMAC_old_base));



// printOn et readOn

Sortie& Elem_PolyMAC_old::printOn(Sortie& s ) const
{
  return s << valeur() ;
}

Entree& Elem_PolyMAC_old::readOn(Entree& s )
{
  Nom type;
  s >> type;
  if(type == "Tri_PolyMAC_old")
    *this =  Tri_PolyMAC_old();
  else if(type == "Tetra_PolyMAC_old")
    *this =  Tetra_PolyMAC_old();
  else if(type == "Quadri_PolyMAC_old")
    *this =  Quadri_PolyMAC_old();
  else if(type == "Hexa_PolyMAC_old")
    *this =  Hexa_PolyMAC_old();
  else
    {
      Cerr << type << " n'est pas un Elem_PolyMAC_old" << finl;
      exit();
    }
  return s ;
}

// Description:
// determination du type
void Elem_PolyMAC_old::typer(Nom type_elem_geom)
{
  Cerr << "Elem_PolyMAC_old::typer()" << finl ;
  Cerr << "type geometrique : " << type_elem_geom << finl;
  Nom type;
  if(type_elem_geom=="Triangle")
    type="Tri_PolyMAC_old";
  else if(type_elem_geom=="Tetraedre")
    type="Tetra_PolyMAC_old";
  else if(type_elem_geom=="Quadrangle")
    type="Quadri_PolyMAC_old";
  else if(type_elem_geom=="Hexaedre_VEF")
    type="Hexa_PolyMAC_old";
  else if(type_elem_geom=="Segment")
    type="Segment_PolyMAC_old";
  else if(type_elem_geom=="Polygone")
    type="Polygone_PolyMAC_old";
  else if(type_elem_geom=="Polyedre")
    type="Polyedre_PolyMAC_old";
  else
    {
      Cerr << "probleme de typage dans Elem_PolyMAC_old::typer" << finl;
      exit();
    }
  DERIV(Elem_PolyMAC_old_base)::typer(type);
  Cerr << "type retenu : " << valeur().que_suis_je() << finl ;
}
