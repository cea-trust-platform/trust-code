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
// File:        Poly_geom_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Poly_geom_base_included
#define Poly_geom_base_included

#include <Elem_geom_base.h>
#include <Elem_geom.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Poly_geom_base
//
// <Description of class Poly_geom_base>
//
/////////////////////////////////////////////////////////////////////////////

class Poly_geom_base : public Elem_geom_base
{

  Declare_base( Poly_geom_base ) ;

public :

  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const override =0;
  virtual int get_tab_faces_sommets_locaux(IntTab& faces_som_local,int elem) const=0;
  virtual const ArrOfInt& getFacesIndex() const =0;
  virtual const ArrOfInt& getElemIndex() const =0;
  virtual int get_somme_nb_faces_elem() const =0;
  virtual void calculer_un_centre_gravite(const int elem,DoubleVect& xp) const =0;
  virtual void build_reduced(Elem_geom& type_elem, const ArrOfInt& elems_sous_part) const = 0;
  virtual void compute_virtual_index()=0;
protected :

};

#endif /* Poly_geom_base_included */
