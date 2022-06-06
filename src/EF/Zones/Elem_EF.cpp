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

#include <Elem_EF.h>
#include <Tri_EF.h>
#include <Tetra_EF.h>
#include <Quadri_EF.h>
#include <Hexa_EF.h>

Implemente_deriv(Elem_EF_base);
Implemente_instanciable(Elem_EF,"Elem_EF",DERIV(Elem_EF_base));



// printOn et readOn

Sortie& Elem_EF::printOn(Sortie& s ) const
{
  return s << valeur() ;
}

Entree& Elem_EF::readOn(Entree& s )
{
  Nom type;
  s >> type;
  if(type == "Tri_EF")
    *this =  Tri_EF();
  else if(type == "Tetra_EF")
    *this =  Tetra_EF();
  else if(type == "Quadri_EF")
    *this =  Quadri_EF();
  else if(type == "Hexa_EF")
    *this =  Hexa_EF();
  else
    {
      Cerr << type << " n'est pas un Elem_EF" << finl;
      exit();
    }
  return s ;
}

// Description:
// determination du type
void Elem_EF::typer(Nom type_elem_geom)
{
//  Cerr << "Elem_EF::typer()" << finl ;
  Nom type;
  if(type_elem_geom=="Triangle")
    type="Tri_EF";
  else if(type_elem_geom=="Tetraedre")
    type="Tetra_EF";
  else if(type_elem_geom=="Quadrangle")
    type="Quadri_EF";
  else if(type_elem_geom=="Hexaedre_VEF")
    type="Hexa_EF";
  else if(type_elem_geom=="Segment")
    type="Segment_EF";
  else if(type_elem_geom=="Segment_axi")
    type="Segment_EF_axi";
  else if(type_elem_geom=="Point")
    type="Point_EF";
  else
    {
      Cerr << "probleme de typage dans Elem_EF::typer" << finl;
      Cerr << "type geometrique : " << type_elem_geom << finl;
      exit();
    }
  DERIV(Elem_EF_base)::typer(type);
// Cerr << "type retenu : " << valeur().que_suis_je() << finl ;
}
