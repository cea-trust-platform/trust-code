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

#ifndef Champ_implementation_included
#define Champ_implementation_included

#include <TRUSTTabs_forward.h>

class Champ_base;
class Domaine_VF;
#include <Domaine_forward.h>
#include <Domaine_forward.h>

class Champ_implementation
{
public:
  virtual ~Champ_implementation() { }
  virtual DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const =0;
  virtual double valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const =0;
  virtual DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const =0;
  virtual DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const =0;
  virtual DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const =0;
  virtual int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const =0;

  // pas pure ...
  virtual DoubleTab& valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const;
  virtual DoubleVect& valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const;

protected:
  const Domaine_VF& get_domaine_dis() const;
  const Domaine& get_domaine_geom() const;

  virtual Champ_base& le_champ() =0;
  virtual const Champ_base& le_champ() const =0;
  virtual DoubleTab& valeur_aux_sommets_impl(DoubleTab& result) const =0;
  virtual DoubleVect& valeur_aux_sommets_compo_impl(DoubleVect& result, int ncomp) const =0;
};

#endif /* Champ_implementation_inclus */
