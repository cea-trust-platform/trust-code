/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Elem_EF_included
#define Elem_EF_included

#include <Elem_EF_base.h>
#include <TRUST_Deriv.h>
#include <TRUSTTab.h>

class Champ_Inc_base;

class Elem_EF : public DERIV(Elem_EF_base)
{
  Declare_instanciable(Elem_EF);

public:

  inline Elem_EF& operator =(const Elem_EF_base&);
  void typer(Nom);
  inline void calcul_vc(const ArrOfInt&, ArrOfDouble&, const ArrOfDouble&, const DoubleTab&, const Champ_Inc_base&, int) const;
  inline void calcul_xg(DoubleVect&, const DoubleTab&, const int, int&, int&, int&, int&) const;
  inline void normale(int, DoubleTab&, const IntTab&, const IntTab&, const IntTab&, const Domaine&) const;
};


inline Elem_EF& Elem_EF::operator =(const Elem_EF_base& elem_base)
{
  DERIV(Elem_EF_base)::operator=(elem_base);
  return *this;
}

inline void Elem_EF::normale(int num_face, DoubleTab& Face_normales, const IntTab& Face_sommets, const IntTab& Face_voisins, const IntTab& elem_faces, const Domaine& domaine_geom) const
{
  valeur().normale(num_face, Face_normales, Face_sommets, Face_voisins, elem_faces, domaine_geom);
}

inline void Elem_EF::calcul_vc(const ArrOfInt& face, ArrOfDouble& vc, const ArrOfDouble& vs, const DoubleTab& vsom, const Champ_Inc_base& vitesse, int type_cl) const
{
  valeur().calcul_vc(face, vc, vs, vsom, vitesse, type_cl);
}

inline void Elem_EF::calcul_xg(DoubleVect& xg, const DoubleTab& x, const int type_elem_Cl, int& idirichlet, int& n1, int& n2, int& n3) const
{
  valeur().calcul_xg(xg, x, type_elem_Cl, idirichlet, n1, n2, n3);
}

#endif

