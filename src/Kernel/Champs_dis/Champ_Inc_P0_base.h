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
// File:        Champ_Inc_P0_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Inc_P0_base_included
#define Champ_Inc_P0_base_included

#include <Champ_Inc_base.h>
#include <Champ_implementation_P0.h>

/////////////////////////////////////////////////////////////////////////////
// .DESCRIPTION        : class Champ_Inc_P0_base
//
// Decrire ici la classe Champ_Inc_P0_base
//
//////////////////////////////////////////////////////////////////////////////

class Champ_Inc_P0_base : public Champ_Inc_base, public Champ_implementation_P0
{

  Declare_base(Champ_Inc_P0_base);

protected :
  virtual       Champ_base& le_champ(void)      ;
  virtual const Champ_base& le_champ(void) const;

public :
  inline virtual DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const;
  inline virtual double valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const;
  inline virtual DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const;
  inline virtual DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const;
  inline virtual DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const;
  inline virtual int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const;
  inline virtual DoubleTab& valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const;
  inline virtual DoubleVect& valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const;
  virtual DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double ) const;
  virtual Champ_base& affecter_(const Champ_base& ) ;
  int fixer_nb_valeurs_nodales(int n);
};

inline DoubleVect& Champ_Inc_P0_base::valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const
{
  return Champ_implementation_P0::valeur_a_elem(position,result,poly);
}

inline double Champ_Inc_P0_base::valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const
{
  return Champ_implementation_P0::valeur_a_elem_compo(position,poly,ncomp);
}

inline DoubleTab& Champ_Inc_P0_base::valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const
{
  return Champ_implementation_P0::valeur_aux_elems(positions,polys,result);
}

inline DoubleVect& Champ_Inc_P0_base::valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const
{
  return Champ_implementation_P0::valeur_aux_elems_compo(positions,polys,result,ncomp);
}

inline DoubleTab& Champ_Inc_P0_base::remplir_coord_noeuds(DoubleTab& positions) const
{
  return Champ_implementation_P0::remplir_coord_noeuds(positions);
}

inline int Champ_Inc_P0_base::remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const
{
  return Champ_implementation_P0::remplir_coord_noeuds_et_polys(positions,polys);
}

inline DoubleTab& Champ_Inc_P0_base::valeur_aux_sommets(const Domaine& domain, DoubleTab& result) const
{
  return Champ_implementation_base::valeur_aux_sommets(domain,result);
}

inline DoubleVect& Champ_Inc_P0_base::valeur_aux_sommets_compo(const Domaine& domain, DoubleVect& result, int ncomp) const
{
  return Champ_implementation_base::valeur_aux_sommets_compo(domain,result,ncomp);
}

#endif /* Champ_Inc_P0_base_inclus*/
