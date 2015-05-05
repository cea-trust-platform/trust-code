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
// File:        Champ_som_lu.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_som_lu_included
#define Champ_som_lu_included

#include <Champ_Don_base.h>
#include <Ref_Domaine.h>
#include <Motcle.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Champ_som_lu
//
//////////////////////////////////////////////////////////////////////////////

class Champ_som_lu : public Champ_Don_base
{
  Declare_instanciable(Champ_som_lu);
public:
  Champ_base& affecter(const Champ_base& ch);
  DoubleVect& valeur_a(const DoubleVect& positions, DoubleTab& valeurs) const;
  DoubleVect& valeur_a(const DoubleVect& positions, DoubleVect& valeurs) const;
  DoubleVect& valeur_a(const DoubleVect& positions, DoubleVect& valeurs, int ncomp) const ;
  DoubleVect& valeur_a(const DoubleVect& positions, DoubleVect& valeurs, int le_poly,int ncomp) const;
  DoubleTab&   valeur_aux(const DoubleTab& positions, DoubleTab& valeurs) const;
  DoubleVect& valeur_aux_compo(const DoubleTab& positions, DoubleVect& valeurs, int ncomp) const;
  DoubleTab&   valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const ;
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& valeurs, int ncomp) const;
protected:
  REF(Domaine) mon_domaine;
  double tolerance_;
};

#endif
