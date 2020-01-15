/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Eval_Dirac_VEF_Face.h
// Directory:   $TRUST_ROOT/src/VEF/Sources/Evaluateurs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Dirac_VEF_Face_included
#define Eval_Dirac_VEF_Face_included

#include <Evaluateur_Source_VEF_Face.h>
#include <Ref_Champ_Don.h>
#include <Zone.h>



////////////////////////////////////////////////////////////////////////////
//
//  CLASS Eval_Dirac_VEF_Face
//
////////////////////////////////////////////////////////////////////////////

class Eval_Dirac_VEF_Face: public Evaluateur_Source_VEF_Face
{

public:

  inline Eval_Dirac_VEF_Face();
  void associer_champs(const Champ_Don& );
  void mettre_a_jour();
  inline double calculer_terme_source_standard(int ) const;
  inline double calculer_terme_source_non_standard(int ) const ;
  inline void calculer_terme_source_standard(int , DoubleVect& ) const;
  inline void calculer_terme_source_non_standard(int , DoubleVect&  ) const ;
  void associer_nb_elem_dirac(int );

  DoubleVect le_point;
protected:

  REF(Champ_Don) la_puissance;
  double puissance;
  REF(Zone) ma_zone;
  double nb_dirac;

};


//
//   Fonctions inline de la classe Eval_Dirac_VDF_Elem
//

inline Eval_Dirac_VEF_Face::Eval_Dirac_VEF_Face():puissance(-123.),nb_dirac(-123.) {}


inline double Eval_Dirac_VEF_Face::calculer_terme_source_standard(int num_face) const
{
  //on recuperre l'element associe a cette face
  int elem;
  if (face_voisins(num_face,1)==-1)
    {
      elem = face_voisins(num_face,0);
    }
  else
    {
      elem = face_voisins(num_face,1);
    }

  int test =  ma_zone.valeur().type_elem().contient(le_point,elem) ;

  if (test == 1)
    {
      assert(nb_dirac!=-123.);
      assert(puissance!=-123.);
      double source =  nb_dirac*puissance;
      return source;
    }
  else
    {
      return 0;
    }

}

inline void Eval_Dirac_VEF_Face::calculer_terme_source_standard(int  num_face, DoubleVect& source) const
{
  ;
}

inline double Eval_Dirac_VEF_Face::calculer_terme_source_non_standard(int num_face) const
{
  int elem;
  if (face_voisins(num_face,1)==-1)
    {
      elem = face_voisins(num_face,0);
    }
  else
    {
      elem = face_voisins(num_face,1);
    }

  int test =  ma_zone.valeur().type_elem().contient(le_point,elem) ;
  if (test == 1)
    {
      assert(nb_dirac!=-123.);
      assert(puissance!=-123.);
      double source =  nb_dirac*puissance;
      return source;
    }
  else
    {
      return 0;
    }
}

inline void Eval_Dirac_VEF_Face::calculer_terme_source_non_standard(int num_face, DoubleVect& source) const
{
  Cerr << "Eval_Dirac_VEF_Face::calculer_terme_source_non_standard(int num_face, DoubleVect& source) is not available." << finl;
  Process::exit();
}


#endif
