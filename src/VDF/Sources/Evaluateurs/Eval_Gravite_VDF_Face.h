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
// File:        Eval_Gravite_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Sources/Evaluateurs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Gravite_VDF_Face_included
#define Eval_Gravite_VDF_Face_included

////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION class Eval_Gravite_VDF_Face
//
////////////////////////////////////////////////////////////////////////////

#include <Evaluateur_Source_VDF_Face.h>
#include <Ref_Champ_Don_base.h>
class Eval_Gravite_VDF_Face: public Evaluateur_Source_VDF_Face
{

public:

  inline Eval_Gravite_VDF_Face();
  inline Eval_Gravite_VDF_Face(const Eval_Gravite_VDF_Face& eval);
  void associer(const Champ_Don_base& );
  inline void mettre_a_jour( );
  inline double calculer_terme_source(int ) const;
  inline void calculer_terme_source(int , DoubleVect& ) const;
  inline double calculer_terme_source_bord(int ) const;
  inline void calculer_terme_source_bord(int , DoubleVect& ) const;


protected:

  REF(Champ_Don_base) la_gravite;
  DoubleVect g;

};


//
//   Fonctions inline de la classe Eval_Gravite_VDF_Face
//

inline Eval_Gravite_VDF_Face::Eval_Gravite_VDF_Face()
{}

inline Eval_Gravite_VDF_Face::Eval_Gravite_VDF_Face(const Eval_Gravite_VDF_Face& eval)
  : Evaluateur_Source_VDF_Face(eval), la_gravite(eval.la_gravite)
{
  g.ref(eval.g);
}


inline void Eval_Gravite_VDF_Face::mettre_a_jour( )
{
  ;
}

inline double Eval_Gravite_VDF_Face::calculer_terme_source(int num_face) const
{
  return g(orientation(num_face))*
         volumes_entrelaces(num_face)*porosite_surf(num_face);
}

inline void Eval_Gravite_VDF_Face::calculer_terme_source(int ,
                                                         DoubleVect& ) const
{
  // EMPTY;
}

inline double Eval_Gravite_VDF_Face::calculer_terme_source_bord(int num_face) const
{
  return calculer_terme_source(num_face);
}

inline void Eval_Gravite_VDF_Face::calculer_terme_source_bord(int ,
                                                              DoubleVect& ) const
{
  // EMPTY;
}

#endif
