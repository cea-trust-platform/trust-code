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
// File:        Elem_PolyMAC_old.h
// Directory:   $TRUST_ROOT/src/PolyMAC_old/Domaines
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Elem_PolyMAC_old_included
#define Elem_PolyMAC_old_included

//
// .DESCRIPTION class Elem_PolyMAC_old


#include <Elem_PolyMAC_old_base.h>
#include <TRUSTTabs_forward.h>
#include <TRUST_Deriv.h>

class Champ_Inc_base;

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Elem_PolyMAC_old
//
//////////////////////////////////////////////////////////////////////////////
class Elem_PolyMAC_old: public DERIV(Elem_PolyMAC_old_base)
{
  Declare_instanciable(Elem_PolyMAC_old);

public:

  inline Elem_PolyMAC_old& operator =(const Elem_PolyMAC_old_base&);
  void typer(Nom);
  inline void normale(int, DoubleTab&, const IntTab&, const IntTab&, const IntTab&, const Domaine&) const;

};


inline Elem_PolyMAC_old& Elem_PolyMAC_old::operator =(const Elem_PolyMAC_old_base& elem_base)
{
  DERIV(Elem_PolyMAC_old_base)::operator=(elem_base);
  return *this;
}

inline void Elem_PolyMAC_old::normale(int num_face, DoubleTab& Face_normales, const IntTab& Face_sommets, const IntTab& Face_voisins, const IntTab& elem_faces, const Domaine& domaine_geom) const
{
  valeur().normale(num_face, Face_normales, Face_sommets, Face_voisins, elem_faces, domaine_geom);
}


#endif

