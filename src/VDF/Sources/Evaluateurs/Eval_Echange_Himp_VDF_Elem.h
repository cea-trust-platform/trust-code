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
// File:        Eval_Echange_Himp_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Sources/Evaluateurs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Echange_Himp_VDF_Elem_included
#define Eval_Echange_Himp_VDF_Elem_included

#include <Evaluateur_Source_VDF_Elem.h>
#include <Ref_Champ_Inc.h>
#include <Champ_Inc.h>
#include <DoubleTab.h>

class Eval_Echange_Himp_VDF_Elem: public Evaluateur_Source_VDF_Elem
{
public:
  Eval_Echange_Himp_VDF_Elem() : h_(-1.) { }
  inline double calculer_terme_source(int ) const;
  inline void calculer_terme_source(int , DoubleVect& ) const { /* Do nothing */ }
  inline void associer_champs(const Champ_Inc& ,const Champ_Inc& ,const double );
  inline void mettre_a_jour();

protected:
  REF(Champ_Inc) T, T_voisin;
  DoubleTab Tcourant, Tvois;
  double h_;
};

inline void Eval_Echange_Himp_VDF_Elem::associer_champs(const Champ_Inc& tc, const Champ_Inc& tv, const double dh)
{
  this->h_ = dh;
  T = tc;
  T_voisin = tv;
  Tcourant.ref(T->valeurs());
  mettre_a_jour();
}

inline void Eval_Echange_Himp_VDF_Elem::mettre_a_jour()
{
  Tcourant.ref(T->valeurs());
  Tvois.ref(T_voisin->valeurs());
}

inline double Eval_Echange_Himp_VDF_Elem::calculer_terme_source(int num_elem) const
{
  return h_*(Tvois(num_elem)-Tcourant(num_elem)) * volumes(num_elem)*porosite_vol(num_elem);
}

#endif /* Eval_Echange_Himp_VDF_Elem_included */
