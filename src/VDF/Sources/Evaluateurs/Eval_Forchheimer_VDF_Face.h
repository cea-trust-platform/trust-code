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
// File:        Eval_Forchheimer_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Sources/Evaluateurs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Forchheimer_VDF_Face_included
#define Eval_Forchheimer_VDF_Face_included

#include <Evaluateur_Source_VDF_Face.h>
#include <Modele_Permeabilite.h>
#include <Ref_Champ_Inc.h>

class Eval_Forchheimer_VDF_Face: public Evaluateur_Source_VDF_Face
{
public:
  Eval_Forchheimer_VDF_Face() : Cf(1.), porosite(1) { }
  Eval_Forchheimer_VDF_Face(const Eval_Forchheimer_VDF_Face& eval) : Evaluateur_Source_VDF_Face(eval), Cf(1.), porosite(1) { }
  inline double calculer_terme_source(int ) const;
  inline double calculer_terme_source_bord(int num_face) const { return calculer_terme_source(num_face); }
  inline void calculer_terme_source(int , DoubleVect& ) const { /* Do nothing */}
  inline void calculer_terme_source_bord(int , DoubleVect& ) const { /* Do nothing */}
  inline void mettre_a_jour() { /* Do nothing */}
  inline void setCf(double c) { Cf = c; }
  inline void associer(const Champ_Inc& vit) { vitesse = vit;}
  inline void setPorosite(double p) { porosite = p; }

  Modele_Permeabilite modK;

protected:
  REF(Champ_Inc) vitesse;
  double Cf, porosite;
};

inline double Eval_Forchheimer_VDF_Face::calculer_terme_source(int num_face) const
{
  const double U = (vitesse->valeurs())(num_face);
  return -Cf/sqrt(modK->getK(porosite))*volumes_entrelaces(num_face)*porosite_surf(num_face)*dabs(U)*U;
}

#endif /* Eval_Forchheimer_VDF_Face_included */
