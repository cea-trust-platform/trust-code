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
// File:        Op_Diff_VDF_Face_Axi.h
// Directory:   $TRUST_ROOT/src/VDF/Axi/Operateurs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Diff_VDF_Face_Axi_included
#define Op_Diff_VDF_Face_Axi_included

#include <Op_Diff_VDF_Face_base.h>

//
// .DESCRIPTION class Op_Diff_VDF_Face_Axi
//
//  Cette classe represente l'operateur de diffusion associe aux
//  equations de quantite de mouvement en coordonnees cylindriques.
//  La discretisation est VDF
//  Le champ diffuse est un Champ_Face
//  Le champ de diffusivite est uniforme
//  Cette classe n'utilise ni iterateur ni evaluateur (il y avait
//  trop de termes supplementaires dus aux coordonnees cylindriques)

//
// .SECTION voir aussi
//  Operateur_Diff_base
//

#include <Ref_Champ_Face.h>
#include <Ref_Champ_Uniforme.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_VDF_Face_Axi
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_VDF_Face_Axi : public Op_Diff_VDF_Face_base
{

  Declare_instanciable(Op_Diff_VDF_Face_Axi);

public:

  void associer(const Zone_dis& , const Zone_Cl_dis& ,
                const Champ_Inc& );
  void associer_diffusivite(const Champ_base& );
  const Champ_base& diffusivite() const;
  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  double calculer_dt_stab() const;

  // methode pour l'implicite

  inline void dimensionner(Matrice_Morse&) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  inline void contribuer_au_second_membre(DoubleTab& ) const;
  void contribue_au_second_membre(DoubleTab& ) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  inline void mettre_a_jour(double temps)
  {
    ;
  } ;

protected:

  REF(Champ_Face) inconnue;
  REF(Zone_VDF) la_zone_vdf;
  REF(Zone_Cl_VDF) la_zcl_vdf;
  REF(Champ_Uniforme) diffusivite_;

  DoubleVect surface;
  DoubleVect volumes_entrelaces;
  IntVect orientation;
  DoubleVect porosite;
  DoubleTab xp;
  DoubleTab xv;
  IntTab Qdm;
  IntTab face_voisins;
  IntTab elem_faces;
  IntVect type_arete_bord;


  void ajouter_bord(const DoubleTab& ,  DoubleTab& ) const;
  void ajouter_contribution_bord(const DoubleTab&, Matrice_Morse& ) const;
};

// Description:
// on dimensionne notre matrice.

inline void Op_Diff_VDF_Face_Axi::dimensionner(Matrice_Morse& matrice) const
{
  Op_VDF_Face::dimensionner(la_zone_vdf.valeur(), la_zcl_vdf.valeur(), matrice);
}
inline void Op_Diff_VDF_Face_Axi::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_VDF_Face::modifier_pour_Cl( la_zone_vdf.valeur(), la_zcl_vdf.valeur(), matrice,  secmem);
}

//Description:
//on assemble la matrice.

inline void Op_Diff_VDF_Face_Axi::contribuer_a_avec(const DoubleTab& inco,
                                                    Matrice_Morse& matrice) const
{
  ajouter_contribution(inco, matrice);
}

//Description:
//on ajoute la contribution du second membre.

inline void Op_Diff_VDF_Face_Axi::contribuer_au_second_membre(DoubleTab& resu) const
{
  contribue_au_second_membre(resu);
}

#endif
