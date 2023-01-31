/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Eval_Puiss_Th_QC_VEF_Face_included
#define Eval_Puiss_Th_QC_VEF_Face_included

#include <Evaluateur_Source_VEF_Face.h>
#include <Champ_Uniforme.h>
#include <Champ_Don.h>
#include <TRUST_Ref.h>
#include <TRUSTTab.h>

class Eval_Puiss_Th_QC_VEF_Face: public Evaluateur_Source_VEF_Face
{
public:
  Eval_Puiss_Th_QC_VEF_Face() { }
  void completer() override;
  void associer_puissance(const Champ_Don&);
  void mettre_a_jour() override { }

  template<typename Type_Double>
  inline void calculer_terme_source_standard(const int, Type_Double&) const;

  template<typename Type_Double>
  inline void calculer_terme_source_non_standard(const int, Type_Double&) const;

protected:
  REF(Champ_Don) la_puissance;
  DoubleTab puissance;
  IntTab face_voisins;
  DoubleVect volumes;
  int nb_faces_elem = -100;
};

template<typename Type_Double>
inline void Eval_Puiss_Th_QC_VEF_Face::calculer_terme_source_standard(const int num_face, Type_Double& source) const
{
  const int size = source.size_array();
  if (size > 1) Process::exit("Eval_Puiss_Th_QC_VEF_Face::calculer_terme_source_standard not available for multi-inco !");

  if (sub_type(Champ_Uniforme, la_puissance.valeur().valeur()))
    for (int i = 0; i < size; i++) source[i] = puissance(0, 0) * volumes_entrelaces[num_face] * porosite_surf[num_face];
  else
    {
      double P0, P1;
      int elem0 = face_voisins(num_face, 0), elem1 = face_voisins(num_face, 1);
      double V0 = volumes(elem0), V1 = volumes(elem1);
      if (puissance.nb_dim() == 1)
        {
          P0 = puissance(elem0);
          P1 = puissance(elem1);
        }
      else
        {
          P0 = puissance(elem0, 0);
          P1 = puissance(elem1, 0);
        }
      for (int i = 0; i < size; i++) source[i] = ((P0 * V0 + P1 * V1) / nb_faces_elem) * porosite_surf[num_face];
    }
}

template<typename Type_Double>
inline void Eval_Puiss_Th_QC_VEF_Face::calculer_terme_source_non_standard(int num_face, Type_Double& source) const
{
  const int size = source.size_array();
  if (size > 1) Process::exit("Eval_Puiss_Th_QC_VEF_Face::calculer_terme_source_non_standard not available for multi-inco !");

  if (sub_type(Champ_Uniforme, la_puissance.valeur().valeur()))
    for (int i = 0; i < size; i++) source[i] = puissance(0, 0) * volumes_entrelaces_Cl[num_face] * porosite_surf(num_face);
  else
    {
      double P0, P1;
      int elem0 = face_voisins(num_face, 0), elem1 = face_voisins(num_face, 1);
      double V0 = volumes(elem0), V1 = (elem1 != -1 ? volumes(elem1) : 0);
      if (puissance.nb_dim() == 1)
        {
          P0 = puissance(elem0);
          P1 = (elem1 != -1 ? puissance(elem1) : 0);
        }
      else
        {
          P0 = puissance(elem0, 0);
          P1 = (elem1 != -1 ? puissance(elem1, 0) : 0);
        }
      for (int i = 0; i < size; i++) source[i] = ((P0 * V0 + P1 * V1) / nb_faces_elem) * porosite_surf[num_face];
    }
}

#endif /* Eval_Puiss_Th_QC_VEF_Face_included */
