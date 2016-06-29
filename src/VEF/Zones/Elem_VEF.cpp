/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Elem_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Zones
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Elem_VEF.h>
#include <Tri_VEF.h>
#include <Tetra_VEF.h>
#include <Quadri_VEF.h>
#include <Hexa_VEF.h>
#include <Motcle.h>

Implemente_deriv(Elem_VEF_base);
Implemente_instanciable(Elem_VEF,"Elem_VEF",DERIV(Elem_VEF_base));



// printOn et readOn

Sortie& Elem_VEF::printOn(Sortie& s ) const
{
  return s << valeur() ;
}

Entree& Elem_VEF::readOn(Entree& s )
{
  Nom type;
  s >> type;
  if(type == "Tri_VEF")
    *this =  Tri_VEF();
  else if(type == "Tetra_VEF")
    *this =  Tetra_VEF();
  else if(type == "Quadri_VEF")
    *this =  Quadri_VEF();
  else if(type == "Hexa_VEF")
    *this =  Hexa_VEF();
  else
    {
      Cerr << type << " n'est pas un Elem_VEF" << finl;
      exit();
    }
  return s ;
}

// Description:
// determination du type
void Elem_VEF::typer(Nom type_elem_geom)
{
  if (Motcle(type_elem_geom)!="Segment")
    {
      Nom type;
      if(type_elem_geom=="Triangle")
        type="Tri_VEF";
      else if(type_elem_geom=="Tetraedre")
        type="Tetra_VEF";
      else if(type_elem_geom=="Quadrangle")
        type="Quadri_VEF";
      else if(type_elem_geom=="Hexaedre_VEF")
        type="Hexa_VEF";
      else
        {
          Cerr << "probleme de typage dans Elem_VEF::typer" << finl;
          exit();
        }
      DERIV(Elem_VEF_base)::typer(type);
    }
}
