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
// File:        Terme_Gravite_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Terme_Gravite_VDF_Face_included
#define Terme_Gravite_VDF_Face_included


//
// .DESCRIPTION class Terme_Gravite_VDF_Face
//
// Terme Gravite
//
//


#include <Terme_Source_VDF_base.h>
#include <Eval_Gravite_VDF_Face.h>
#include <Iterateur_Source_VDF.h>

class Zone_dis;
class Zone_Cl_dis;

declare_It_Sou_VDF_Face(Eval_Gravite_VDF_Face)

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Terme_Gravite_VDF_Face
//
// Cette classe represente le terme de gravite qui figure dans l'equation
// de la dynamique divisee par la masse volumique;la masse volumique est
// supposee constante.
//
//////////////////////////////////////////////////////////////////////////////

class Terme_Gravite_VDF_Face : public Terme_Source_VDF_base
{
  Declare_instanciable_sans_constructeur(Terme_Gravite_VDF_Face);

public:

  inline Terme_Gravite_VDF_Face();
  void associer_pb(const Probleme_base& );
  void associer_zones(const Zone_dis&, const Zone_Cl_dis& );
  void mettre_a_jour(double temps)
  {
    gravite->mettre_a_jour(temps);
  };

protected:

  REF(Champ_Don_base) gravite;
  void associer_gravite(const Champ_Don_base& );
};


//
// Fonctions inline de la classe Terme_Gravite_VDF_Face
//
inline Terme_Gravite_VDF_Face::Terme_Gravite_VDF_Face() :
  Terme_Source_VDF_base(It_Sou_VDF_Face(Eval_Gravite_VDF_Face)())
{
}




#endif

