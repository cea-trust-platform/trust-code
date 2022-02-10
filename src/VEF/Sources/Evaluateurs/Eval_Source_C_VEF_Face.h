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
// File:        Eval_Source_C_VEF_Face.h
// Directory:   $TRUST_ROOT/src/VEF/Sources/Evaluateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Source_C_VEF_Face_included
#define Eval_Source_C_VEF_Face_included


#include <Evaluateur_Source_VEF_Face.h>
#include <Ref_Champ_Don.h>
#include <DoubleTab.h>
#include <Champ_Uniforme.h>
#include <Champ_Don.h>

////////////////////////////////////////////////////////////////////////////
//
//  CLASS Eval_Source_C_VEF_Face
//
////////////////////////////////////////////////////////////////////////////

class Eval_Source_C_VEF_Face : public Evaluateur_Source_VEF_Face
{

public:

  inline Eval_Source_C_VEF_Face();
  void completer() override;
  void associer_champs(const Champ_Don& );
  void mettre_a_jour( ) override;
  inline double calculer_terme_source_standard(int ) const override ;
  inline double calculer_terme_source_non_standard(int ) const override ;
  inline void calculer_terme_source_standard(int , DoubleVect&  ) const override ;
  inline void calculer_terme_source_non_standard(int , DoubleVect&  ) const override ;

protected:

  REF(Champ_Don) la_source_constituant;
  DoubleTab source_constituant;
  IntTab face_voisins;
  DoubleVect volumes;
  int nb_faces_elem;
};


//
//   Fonctions inline de la classe Eval_Puiss_Th_Uniforme_VEF_Face
//

inline Eval_Source_C_VEF_Face::Eval_Source_C_VEF_Face() {}

inline double Eval_Source_C_VEF_Face::calculer_terme_source_standard(int num_face) const
{
  double source;
  if (sub_type(Champ_Uniforme,la_source_constituant.valeur().valeur()))
    source = source_constituant(0,0)*volumes_entrelaces(num_face);
  else
    source = (source_constituant(face_voisins(num_face,0),0)*volumes(face_voisins(num_face,0)) + source_constituant(face_voisins(num_face,1),0)*volumes(face_voisins(num_face,1)))/nb_faces_elem;

  return (source*porosite_surf(num_face));
}

inline double Eval_Source_C_VEF_Face::calculer_terme_source_non_standard(int num_face) const
{
  double source;
  if (sub_type(Champ_Uniforme,la_source_constituant.valeur().valeur()))
    source = source_constituant(0,0)*volumes_entrelaces_Cl(num_face);
  else
    {
      if (face_voisins(num_face,1) != -1)
        source = (source_constituant(face_voisins(num_face,0),0)*volumes(face_voisins(num_face,0)) + source_constituant(face_voisins(num_face,1),0)*volumes(face_voisins(num_face,1)))/nb_faces_elem;
      else
        source = source_constituant(face_voisins(num_face,0),0)*volumes(face_voisins(num_face,0))/nb_faces_elem;
    }

  return (source*porosite_surf(num_face));
}

inline void Eval_Source_C_VEF_Face::calculer_terme_source_standard(int num_face, DoubleVect& source) const
{
  if (sub_type(Champ_Uniforme,la_source_constituant.valeur().valeur()))
    {
      source.inject_array(source_constituant);
      source *= volumes_entrelaces(num_face)*porosite_surf(num_face);
    }
  else
    {
      int size=source.size();
      for (int i=0; i<size; i++)
        source(i) = (source_constituant(face_voisins(num_face,0),i)*volumes(face_voisins(num_face,0))+source_constituant(face_voisins(num_face,1),i)*volumes(face_voisins(num_face,1)))/nb_faces_elem
                    *porosite_surf(num_face);
    }
}

inline void Eval_Source_C_VEF_Face::calculer_terme_source_non_standard(int num_face, DoubleVect& source) const
{
  if (sub_type(Champ_Uniforme,la_source_constituant.valeur().valeur()))
    {
      source.inject_array(source_constituant);
      source *= volumes_entrelaces_Cl(num_face)*porosite_surf(num_face);
    }
  else
    {
      int size=source.size();
      for (int i=0; i<size; i++)
        if (face_voisins(num_face,1) != -1)
          source(i) = (source_constituant(face_voisins(num_face,0),i)*volumes(face_voisins(num_face,0))+source_constituant(face_voisins(num_face,1),i)*volumes(face_voisins(num_face,1)))/nb_faces_elem
                      *porosite_surf(num_face);
        else
          source(i) = source_constituant(face_voisins(num_face,0),i)*volumes(face_voisins(num_face,0))*porosite_surf(num_face)/nb_faces_elem;
    }
}

#endif
