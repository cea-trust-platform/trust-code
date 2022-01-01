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
// File:        Source_Darcy_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Sources/Sources_It_Eval
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Source_Darcy_VDF_Face_included
#define Source_Darcy_VDF_Face_included

#include <Terme_Source_VDF_base.h>
#include <Iterateur_Source_VDF.h>
#include <Eval_Darcy_VDF_Face.h>

class Zone_Cl_dis;
class Zone_dis;

declare_It_Sou_VDF_Face(Eval_Darcy_VDF_Face)
// .DESCRIPTION class Source_Darcy_VDF_Face
// Cette classe represente le terme de Darcy pour les ecoulement en milieux poreux.
// Ce terme doit normalement etre de type "operateur" : pour l'instant il est code comme un terme
// source et donc ne  doit etre utilise qu'avec un schema en temps de type explicite.
class Source_Darcy_VDF_Face : public Terme_Source_VDF_base
{
  Declare_instanciable_sans_constructeur(Source_Darcy_VDF_Face);
public:
  inline Source_Darcy_VDF_Face();
  void associer_pb(const Probleme_base& );
  void associer_zones(const Zone_dis&, const Zone_Cl_dis& );
  void mettre_a_jour(double temps) { /* Do nothing */ }
  inline Eval_Darcy_VDF_Face& eval() { return static_cast<Eval_Darcy_VDF_Face&> (iter->evaluateur()); }
};

inline Source_Darcy_VDF_Face::Source_Darcy_VDF_Face() : Terme_Source_VDF_base(It_Sou_VDF_Face(Eval_Darcy_VDF_Face)()) { }

#endif /* Source_Darcy_VDF_Face_included */
