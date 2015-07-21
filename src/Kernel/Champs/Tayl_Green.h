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
// File:        Tayl_Green.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

// .DESCRIPTION class Tayl_Green
//   by: buchal@che41b0.der.edf.fr
//   modified version on  Champs/Tourbillon
//   in *.datafile:   Tourbillon kwave
//    u=sin(kwave*x)*cos(kwave*y)
//    v=cos(kwave*x)*sin(kwave*y)
//

#ifndef Tayl_Green_included
#define Tayl_Green_included

#include <Champ_Don_base.h>

class Tayl_Green : public Champ_Don_base
{
  Declare_instanciable_sans_constructeur(Tayl_Green);
public :
  Tayl_Green();
  Champ_base& affecter(const Champ_base& ch);
  DoubleVect& valeur_a(const DoubleVect& position,
                       DoubleVect& valeurs) const;
  double valeur_a_compo(const DoubleVect& position,
                        int compo) const ;
  DoubleVect& valeur_a_elem(const DoubleVect& position,
                            DoubleVect& valeurs,
                            int elem) const ;
  double valeur_a_elem_compo(const DoubleVect& position,
                             int le_poly,int ncomp) const;

  DoubleTab& valeur_aux(const DoubleTab& positions,
                        DoubleTab& valeurs) const;
  DoubleVect& valeur_aux_compo(const DoubleTab& positions,
                               DoubleVect& valeurs, int ncomp) const;

  DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                              const IntVect& les_polys,
                              DoubleTab& valeurs) const ;
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleVect& valeurs,
                                     int ncomp) const;
protected :
  int kwave;
};
#endif
