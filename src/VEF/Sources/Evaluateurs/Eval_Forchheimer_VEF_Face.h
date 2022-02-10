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
// File:        Eval_Forchheimer_VEF_Face.h
// Directory:   $TRUST_ROOT/src/VEF/Sources/Evaluateurs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Forchheimer_VEF_Face_included
#define Eval_Forchheimer_VEF_Face_included

////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION class Eval_Forchheimer_VEF_Face
//
////////////////////////////////////////////////////////////////////////////

#include <Evaluateur_Source_VEF_Face.h>
#include <Modele_Permeabilite.h>
#include <Ref_Champ_Inc.h>

class Eval_Forchheimer_VEF_Face: public Evaluateur_Source_VEF_Face
{

public:

  inline Eval_Forchheimer_VEF_Face();
  inline Eval_Forchheimer_VEF_Face(const Eval_Forchheimer_VEF_Face& eval);
  inline void mettre_a_jour( ) override;
  inline double calculer_terme_source_standard(int ) const override;
  inline void calculer_terme_source_standard(int , DoubleVect& ) const override;
  inline double calculer_terme_source_non_standard(int ) const override;
  inline void calculer_terme_source_non_standard(int , DoubleVect& ) const override;
  inline void setCf(double);
  inline void associer(const Champ_Inc&);
  Modele_Permeabilite modK;

  inline void setPorosite(double p)
  {
    porosite = p;
  }
protected:
  inline void calculer_terme_source(int , DoubleVect& , const DoubleVect&) const;
  REF(Champ_Inc) vitesse;
  double Cf;
  double porosite;
};


//
//   Fonctions inline de la classe Eval_Forchheimer_VEF_Face
//

inline Eval_Forchheimer_VEF_Face::Eval_Forchheimer_VEF_Face() : Cf(1.), porosite(1)
{}

inline Eval_Forchheimer_VEF_Face::Eval_Forchheimer_VEF_Face(const Eval_Forchheimer_VEF_Face& eval)
  : Evaluateur_Source_VEF_Face(eval),Cf(1.), porosite(1)
{
}

// associe la constante de forme Cf
void Eval_Forchheimer_VEF_Face::setCf(double c)
{
  Cf = c;
}


void Eval_Forchheimer_VEF_Face::associer(const Champ_Inc& v)
{
  vitesse = v;
}


inline void Eval_Forchheimer_VEF_Face::mettre_a_jour( )
{
  ;
}

// Compute -Cf.psi.U.|U|/sqrt(K).dvol
inline void Eval_Forchheimer_VEF_Face::calculer_terme_source(int num_face, DoubleVect& source, const DoubleVect& volumes) const
{
  int size=source.size();
  for (int i=0; i<size; i++)
    {
      double U = vitesse->valeurs()(num_face,i);
      source(i) = -Cf/sqrt(modK->getK(porosite))*volumes(num_face)*porosite_surf(num_face)*std::fabs(U)*U;
    }
}

inline void Eval_Forchheimer_VEF_Face::calculer_terme_source_standard(int num_face, DoubleVect& source) const
{
  calculer_terme_source(num_face, source, volumes_entrelaces);
}
inline void Eval_Forchheimer_VEF_Face::calculer_terme_source_non_standard(int num_face, DoubleVect& source) const
{
  calculer_terme_source(num_face, source, volumes_entrelaces_Cl);
}
inline double Eval_Forchheimer_VEF_Face::calculer_terme_source_standard(int num_face) const
{
  Cerr << "Eval_Forchheimer_VEF_Face::calculer_terme_source_standard(int num_face) can't be used on a vector source term." << finl;
  Process::exit();
  return 0;
}
inline double Eval_Forchheimer_VEF_Face::calculer_terme_source_non_standard(int num_face) const
{
  Cerr << "Eval_Forchheimer_VEF_Face::calculer_terme_source_non_standard(int num_face) can't be used on a vector source term." << finl;
  Process::exit();
  return 0;
}

#endif
