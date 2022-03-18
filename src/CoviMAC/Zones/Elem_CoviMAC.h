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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Elem_CoviMAC.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Elem_CoviMAC_included
#define Elem_CoviMAC_included

//
// .DESCRIPTION class Elem_CoviMAC


#include <Elem_CoviMAC_base.h>
#include <Deriv.h>
#include <TRUSTTab.h>

class Champ_Inc_base;

Declare_deriv(Elem_CoviMAC_base);

class Elem_CoviMAC: public DERIV(Elem_CoviMAC_base)
{
  Declare_instanciable(Elem_CoviMAC);
public:
  inline Elem_CoviMAC& operator =(const Elem_CoviMAC_base&);
  void typer(Nom);
  inline void normale(int, DoubleTab&, const IntTab&, const IntTab&, const IntTab&, const Zone&) const;
};


inline Elem_CoviMAC& Elem_CoviMAC::operator =(const Elem_CoviMAC_base& elem_base)
{
  DERIV(Elem_CoviMAC_base)::operator=(elem_base);
  return *this;
}

inline void Elem_CoviMAC::normale(int num_face, DoubleTab& Face_normales, const IntTab& Face_sommets, const IntTab& Face_voisins, const IntTab& elem_faces, const Zone& zone_geom) const
{
  valeur().normale(num_face, Face_normales, Face_sommets, Face_voisins, elem_faces, zone_geom);
}

#endif
