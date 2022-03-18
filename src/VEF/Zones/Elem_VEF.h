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
// File:        Elem_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Zones
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Elem_VEF_included
#define Elem_VEF_included

#include <Elem_VEF_base.h>
#include <TRUSTTab.h>
#include <Deriv.h>

class Champ_Inc_base;

Declare_deriv(Elem_VEF_base);
class Elem_VEF: public DERIV(Elem_VEF_base)
{
  Declare_instanciable(Elem_VEF);
public:

  inline Elem_VEF& operator =(const Elem_VEF_base&);
  void typer(Nom);
  inline int nb_facette() const;
  inline void creer_facette_normales(const Zone&, DoubleTab&, const IntVect&) const;
  inline void creer_normales_facettes_Cl(DoubleTab&, int, int, const DoubleTab&, const DoubleVect&, const Zone&) const;
  inline void modif_volumes_entrelaces(int, int, const Zone_VEF&, DoubleVect&, int) const;
  inline void modif_volumes_entrelaces_faces_joints(int, int, const Zone_VEF&, DoubleVect&, int) const;
  inline void modif_normales_facettes_Cl(DoubleTab&, int, int, int, int, int, int) const;
  inline void calcul_vc(const ArrOfInt&, ArrOfDouble&, const ArrOfDouble&, const DoubleTab&, const Champ_Inc_base&, int, const DoubleVect&) const;
  inline void calcul_xg(DoubleVect&, const DoubleTab&, const int, int&, int&, int&, int&) const;
  inline void normale(int, DoubleTab&, const IntTab&, const IntTab&, const IntTab&, const Zone&) const;

};

//
//  Fonctions inline de la classe Elem_VEF
//

inline Elem_VEF& Elem_VEF::operator =(const Elem_VEF_base& elem_base)
{
  DERIV(Elem_VEF_base)::operator=(elem_base);
  return *this;
}

inline int Elem_VEF::nb_facette() const
{
  return valeur().nb_facette();
}

inline void Elem_VEF::normale(int num_face, DoubleTab& Face_normales, const IntTab& Face_sommets, const IntTab& Face_voisins, const IntTab& elem_faces, const Zone& zone_geom) const
{
  valeur().normale(num_face, Face_normales, Face_sommets, Face_voisins, elem_faces, zone_geom);
}

inline void Elem_VEF::creer_facette_normales(const Zone& zone_geom, DoubleTab& facette_normales, const IntVect& rang_elem) const
{
  valeur().creer_facette_normales(zone_geom, facette_normales, rang_elem);
}

inline void Elem_VEF::creer_normales_facettes_Cl(DoubleTab& norm_fa7_cl, int fa7, int num_elem, const DoubleTab& x, const DoubleVect& xg, const Zone& zone_geom) const
{
  valeur().creer_normales_facettes_Cl(norm_fa7_cl, fa7, num_elem, x, xg, zone_geom);
}

inline void Elem_VEF::modif_volumes_entrelaces(int j, int elem, const Zone_VEF& la_zone_VEF, DoubleVect& vol_ent_cl, int type_cl) const
{
  valeur().modif_volumes_entrelaces(j, elem, la_zone_VEF, vol_ent_cl, type_cl);
}

inline void Elem_VEF::modif_volumes_entrelaces_faces_joints(int j, int elem, const Zone_VEF& la_zone_VEF, DoubleVect& vol_ent_cl, int type_cl) const
{
  valeur().modif_volumes_entrelaces_faces_joints(j, elem, la_zone_VEF, vol_ent_cl, type_cl);
}

inline void Elem_VEF::modif_normales_facettes_Cl(DoubleTab& norm_fa7_cl, int fa7, int num_elem, int idirichlet, int n1, int n2, int n3) const
{
  valeur().modif_normales_facettes_Cl(norm_fa7_cl, fa7, num_elem, idirichlet, n1, n2, n3);
}

inline void Elem_VEF::calcul_vc(const ArrOfInt& face, ArrOfDouble& vc, const ArrOfDouble& vs, const DoubleTab& vsom, const Champ_Inc_base& vitesse, int type_cl, const DoubleVect& porosite_face) const
{
  valeur().calcul_vc(face, vc, vs, vsom, vitesse, type_cl, porosite_face);
}

inline void Elem_VEF::calcul_xg(DoubleVect& xg, const DoubleTab& x, const int type_elem_Cl, int& idirichlet, int& n1, int& n2, int& n3) const
{
  valeur().calcul_xg(xg, x, type_elem_Cl, idirichlet, n1, n2, n3);
}

#endif
