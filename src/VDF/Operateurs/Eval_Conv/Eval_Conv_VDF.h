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
// File:        Eval_Conv_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Eval_Conv
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Conv_VDF_included
#define Eval_Conv_VDF_included

#include <Eval_Conv_VDF_tools.h>
#include <Evaluateur_VDF.h>
#include <Ref_Champ_Face.h>
#include <Champ_Face.h>

class Champ_Inc_base;

// .DESCRIPTION class Eval_Conv_VDF
// classe de base des evaluateurs de convection VDF

class Eval_Conv_VDF : public Evaluateur_VDF, public Eval_Conv_VDF_tools
{

public:
  // Constructeurs
  inline Eval_Conv_VDF() {}
  inline Eval_Conv_VDF(const Eval_Conv_VDF& eval) : Evaluateur_VDF(eval), vitesse_(eval.vitesse_) { dt_vitesse.ref(eval.dt_vitesse); }

  inline void associer(const Champ_Face& );
  inline void mettre_a_jour( ) { dt_vitesse.ref(vitesse_->valeurs()); }
  inline const Champ_Inc_base& vitesse() const { return vitesse_.valeur(); }
  inline Champ_Inc_base& vitesse() { return vitesse_.valeur(); }

  // pour CRTP
  inline int get_elem(int i, int j) const { return elem_(i,j); }
  inline int get_orientation(int i ) const { return orientation(i); }
  inline int get_premiere_face_bord() const { return premiere_face_bord; }
  inline double get_dt_vitesse(int face) const { return dt_vitesse(face); }
  inline double get_surface_porosite(int face) const { return surface(face)*porosite(face); }
  inline double get_surface(int face) const { return surface(face); }
  inline double get_porosite(int face) const { return porosite(face); }
  inline const Zone_Cl_VDF& get_la_zcl() const { return la_zcl.valeur(); }

protected:
  REF(Champ_Face) vitesse_;
  DoubleTab dt_vitesse;
};

// Description:
// associe le champ de vitesse transportante
inline void Eval_Conv_VDF::associer(const Champ_Face& vit)
{
  vitesse_=vit;
  dt_vitesse.ref(vit.valeurs());
}

#endif /* Eval_Conv_VDF_included */
