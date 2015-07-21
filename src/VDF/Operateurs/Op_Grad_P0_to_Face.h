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
// File:        Op_Grad_P0_to_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Grad_P0_to_Face_included
#define Op_Grad_P0_to_Face_included

#include <Operateur_Grad.h>
#include <Ref_Zone_VDF.h>
#include <Zone_VDF.h>
#include <Ref_Zone_Cl_VDF.h>

//
// .DESCRIPTION class Op_Grad_P0_to_Face
//
//  Cette classe represente l'operateur de gradient
//  La discretisation est VDF
//  On calcule le gradient d'un champ_P0 (autre que la pression)
//   et le champ de sortie est aux elements Faces

// .SECTION voir aussi
// Operateur_Grad_base
//

class Op_Grad_P0_to_Face : public Operateur_Grad_base
{

  Declare_instanciable(Op_Grad_P0_to_Face);

public:

  void associer(const Zone_dis& , const Zone_Cl_dis& ,
                const Champ_Inc& );
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  int impr(Sortie& os) const;

  inline Zone_VDF& zone_VDF();
  inline const Zone_VDF& zone_VDF() const;
  inline Zone_Cl_VDF& zone_Cl_VDF();
  inline const Zone_Cl_VDF& zone_Cl_VDF() const;
  inline int& face_voisins_(int i, int j);
  inline const int& face_voisins_(int i, int j) const ;
  inline double volume_entrelaces_(int i) ;
  inline const double& volume_entrelaces_(int i) const ;
  inline double porosite_surf_(int i) ;
  inline const double& porosite_surf_(int i) const ;
  inline int orientation_(int face);
  inline const int& orientation_(int face) const;
  inline double xp_(int elem, int ori);
  inline const double& xp_(int elem, int ori) const;

private:

  REF(Zone_VDF) la_zone_vdf;
  REF(Zone_Cl_VDF) la_zcl_vdf;

  DoubleVect porosite_surf;
  DoubleTab xp;
  DoubleVect volume_entrelaces;
  IntTab face_voisins;
  IntVect orientation;

  inline double dist_norm(int ) const;
  inline double dist_norm_bord(int ) const;


};

inline double Op_Grad_P0_to_Face::xp_(int elem, int ori)
{
  return xp(elem,ori);
}

inline const double& Op_Grad_P0_to_Face::xp_(int elem, int ori) const
{
  return xp(elem,ori);
}

inline int Op_Grad_P0_to_Face::orientation_(int face)
{
  return orientation(face);
}

inline const int& Op_Grad_P0_to_Face::orientation_(int face) const
{
  return orientation(face);
}

inline double Op_Grad_P0_to_Face::porosite_surf_(int i)
{
  return porosite_surf(i);
}

inline const double& Op_Grad_P0_to_Face::porosite_surf_(int i) const
{
  return porosite_surf(i);
}

inline double Op_Grad_P0_to_Face::volume_entrelaces_(int i)
{
  return volume_entrelaces(i);
}

inline const double& Op_Grad_P0_to_Face::volume_entrelaces_(int i) const
{
  return volume_entrelaces(i);
}


inline int& Op_Grad_P0_to_Face::face_voisins_(int i, int j)
{
  return face_voisins(i,j);
}

inline const int& Op_Grad_P0_to_Face::face_voisins_(int i, int j) const
{
  return face_voisins(i,j);
}

inline Zone_VDF& Op_Grad_P0_to_Face::zone_VDF()
{
  return la_zone_vdf.valeur();
}

inline const Zone_VDF& Op_Grad_P0_to_Face::zone_VDF() const
{
  return la_zone_vdf.valeur();
}

inline Zone_Cl_VDF& Op_Grad_P0_to_Face::zone_Cl_VDF()
{
  return la_zcl_vdf.valeur();
}

inline const Zone_Cl_VDF& Op_Grad_P0_to_Face::zone_Cl_VDF() const
{
  return la_zcl_vdf.valeur();
}

inline double Op_Grad_P0_to_Face::dist_norm(int num_face) const
{
  return la_zone_vdf->dist_norm(num_face);
}

inline double Op_Grad_P0_to_Face::dist_norm_bord(int num_face) const
{
  return la_zone_vdf->dist_norm_bord(num_face);
}

#endif
