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
// File:        Champ_implementation_P0.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_implementation_P0_included
#define Champ_implementation_P0_included

#include <Champ_implementation_base.h>
#include <Frontiere.h>
#include <Frontiere_dis_base.h>

/////////////////////////////////////////////////////////////////////////////
// .DESCRIPTION        : class Champ_implementation_P0
//
// Decrire ici la classe Champ_implementation_P0
//
//////////////////////////////////////////////////////////////////////////////

class Champ_implementation_P0 : public Champ_implementation_base
{

protected :
  virtual       Champ_base& le_champ(void)       =0;
  virtual const Champ_base& le_champ(void) const =0;

public :
  inline virtual ~Champ_implementation_P0() { };

public :
  virtual DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const;
  virtual double valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const;
  virtual DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const;
  virtual DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const;

  virtual DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const;
  virtual int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const;
  int imprime_P0(Sortie& , int ) const;

protected :
  virtual DoubleTab& valeur_aux_sommets_impl(DoubleTab& result) const;
  virtual DoubleVect& valeur_aux_sommets_compo_impl(DoubleVect& result, int ncomp) const;
  int affecter_(const Champ_base& ) ;
  inline DoubleTab& trace(const Frontiere_dis_base& fr, const DoubleTab& y, DoubleTab& x,int distant) const;
};

inline DoubleTab& Champ_implementation_P0::trace(const Frontiere_dis_base& fr, const DoubleTab& y, DoubleTab& x,int distant) const
{
  if (distant)
    fr.frontiere().trace_elem_distant(y, x);
  else
    fr.frontiere().trace_elem_local(y, x);
  // useless ? x.echange_espace_virtuel();
  return x;
}

#endif /* Champ_implementation_P0_inclus */
