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
// File:        Op_Grad_VDF_Face_base.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Divers
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Grad_VDF_Face_base_included
#define Op_Grad_VDF_Face_base_included

#include <Operateur_Grad_base.h>
#include <Ref_Zone_Cl_VDF.h>
#include <Ref_Zone_VDF.h>
#include <Zone_VDF.h>

class Op_Grad_VDF_Face_base : public Operateur_Grad_base
{
  Declare_base(Op_Grad_VDF_Face_base);
public:
  void associer(const Zone_dis& , const Zone_Cl_dis& , const Champ_Inc& ) override;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  int impr(Sortie& os) const override;

  //virtual DoubleTab& ajouter(const DoubleTab& , DoubleTab& ) const = 0; // ET OUI ! ATTENTION !! VIRTUELLE PURE !!!
  inline DoubleTab& ajouter(const DoubleTab& inco, DoubleTab& secmem) const override
  {
    ajouter_blocs({}, secmem, {{ "pression", inco }}); /* pour avoir le bon signe */
    return secmem;
  }
  inline void contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const override
  {
    DoubleTab secmem(equation().inconnue().valeurs()); // pour etre dimensione avec nb_faces
    ajouter_blocs({{ "pression", &matrice }}, secmem);
  }
  inline int has_interface_blocs() const override
  {
    return 1;
  };
  inline Zone_VDF& zone_VDF() { return la_zone_vdf.valeur(); }
  inline const Zone_VDF& zone_VDF() const { return la_zone_vdf.valeur(); }
  inline Zone_Cl_VDF& zone_Cl_VDF() { return la_zcl_vdf.valeur(); }
  inline const Zone_Cl_VDF& zone_Cl_VDF() const { return la_zcl_vdf.valeur(); }
  inline int& face_voisins_(int i, int j) { return face_voisins(i,j); }
  inline const int& face_voisins_(int i, int j) const { return face_voisins(i,j); }
  inline double volume_entrelaces_(int i) { return volume_entrelaces(i); }
  inline const double& volume_entrelaces_(int i) const { return volume_entrelaces(i); }
  inline double porosite_surf_(int i)  { return porosite_surf(i); }
  inline const double& porosite_surf_(int i) const { return porosite_surf(i); }
  inline int orientation_(int face) { return orientation(face); }
  inline const int& orientation_(int face) const { return orientation(face); }
  inline double xp_(int elem, int ori) { return xp(elem,ori); }
  inline const double& xp_(int elem, int ori) const { return xp(elem,ori); }

protected:
  REF(Zone_VDF) la_zone_vdf;
  REF(Zone_Cl_VDF) la_zcl_vdf;
  IntVect orientation;
  IntTab face_voisins;
  DoubleVect porosite_surf, volume_entrelaces;
  DoubleTab xp;
};

#endif /* Op_Grad_VDF_Face_base_included */
