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
// File:        Champ_implementation.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_implementation_included
#define Champ_implementation_included

#include <DoubleTab.h>

class Domaine;
class Zone;
class Zone_dis_base;
class Champ_base;

//
class Champ_implementation
{

public:
  inline virtual ~Champ_implementation() {};
  virtual DoubleVect& valeur_a_elem(const DoubleVect& position,
                                    DoubleVect& val,
                                    int le_poly) const=0;
  virtual double valeur_a_elem_compo(const DoubleVect& position,
                                     int le_poly, int ncomp) const=0;
  virtual DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                                      const IntVect& les_polys,
                                      DoubleTab& valeurs) const=0;
  virtual DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                             const IntVect& les_polys,
                                             DoubleVect& valeurs,
                                             int ncomp) const=0 ;
  virtual DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const=0;
  virtual DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                               DoubleVect&, int) const=0;
  virtual DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const=0;
  virtual int remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                            IntVect& polys) const=0;
protected :
  virtual const Champ_base& le_champ() const=0;
  virtual Champ_base& le_champ() =0;
  const Zone& zone() const;
  virtual const Zone_dis_base& zone_dis_base() const;
};

#endif
