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
// File:        Source_Darcy_VEF_Face.h
// Directory:   $TRUST_ROOT/src/VEF/Sources
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Source_Darcy_VEF_Face_included
#define Source_Darcy_VEF_Face_included

//
// .DESCRIPTION class Source_Darcy_VEF_Face
//
// Terme de Darcy pour les milieux poreux
//
//

#include <Terme_Source_VEF_base.h>
#include <Iterateur_Source_VEF.h>
#include <Eval_Darcy_VEF_Face.h>

class Zone_dis;
class Zone_Cl_dis;

declare_It_Sou_VEF_Face(Eval_Darcy_VEF_Face)

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Source_Darcy_VEF_Face
//
// Cette classe represente le terme de Darcy pour les ecoulement en milieux poreux.
// Ce terme doit normalement etre de type "operateur" : pour l'instant il est code
// comme un terme source et donc ne  doit etre utilise qu'avec un schema en temps
// de type explicite.
//
//////////////////////////////////////////////////////////////////////////////

class Source_Darcy_VEF_Face : public Terme_Source_VEF_base
{
  Declare_instanciable_sans_constructeur(Source_Darcy_VEF_Face);

public:

  inline Source_Darcy_VEF_Face();
  void associer_pb(const Probleme_base& ) override;
  void associer_zones(const Zone_dis&, const Zone_Cl_dis& ) override;
  void mettre_a_jour(double temps) override
  {
    ;
  }
  inline Eval_Darcy_VEF_Face& eval()
  {
    return (Eval_Darcy_VEF_Face&) iter.evaluateur();
  }
};


//
// Fonctions inline de la classe Source_Darcy_VEF_Face
//
inline Source_Darcy_VEF_Face::Source_Darcy_VEF_Face() :
  Terme_Source_VEF_base(It_Sou_VEF_Face(Eval_Darcy_VEF_Face)())
{
}

#endif

