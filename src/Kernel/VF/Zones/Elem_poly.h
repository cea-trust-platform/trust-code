/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Elem_poly_included
#define Elem_poly_included

//
// .DESCRIPTION class Elem_poly


#include <Elem_poly_base.h>
#include <TRUSTTabs_forward.h>
#include <Deriv.h>

class Champ_Inc_base;

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Elem_poly
//
//////////////////////////////////////////////////////////////////////////////
Declare_deriv(Elem_poly_base);
class Elem_poly: public DERIV(Elem_poly_base)
{
  Declare_instanciable(Elem_poly);

public:

  inline Elem_poly& operator =(const Elem_poly_base&);
  void typer(Nom);
  inline void normale(int, DoubleTab&, const IntTab&, const IntTab&, const IntTab&, const Zone&) const;

};


inline Elem_poly& Elem_poly::operator =(const Elem_poly_base& elem_base)
{
  DERIV(Elem_poly_base)::operator=(elem_base);
  return *this;
}

inline void Elem_poly::normale(int num_face, DoubleTab& Face_normales, const IntTab& Face_sommets, const IntTab& Face_voisins, const IntTab& elem_faces, const Zone& zone_geom) const
{
  valeur().normale(num_face, Face_normales, Face_sommets, Face_voisins, elem_faces, zone_geom);
}


#endif

