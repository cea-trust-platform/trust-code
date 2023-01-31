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

#ifndef Eval_Dirac_VEF_Face_included
#define Eval_Dirac_VEF_Face_included

#include <Evaluateur_Source_VEF_Face.h>
#include <TRUST_Ref.h>
#include <Domaine.h>

class Champ_Don;

class Eval_Dirac_VEF_Face: public Evaluateur_Source_VEF_Face
{
public:
  Eval_Dirac_VEF_Face() : puissance(-123.), nb_dirac(-123.) {}

  void associer_champs(const Champ_Don& );
  void mettre_a_jour() override;

  template <typename Type_Double>
  inline void calculer_terme_source_standard(const int num_face, Type_Double& source) const { calculer_terme_source(num_face,source); }

  template <typename Type_Double>
  inline void calculer_terme_source_non_standard(const int num_face, Type_Double& source) const { calculer_terme_source(num_face,source); }

  void associer_nb_elem_dirac(int );
  DoubleVect le_point;

protected:
  REF(Champ_Don) la_puissance;
  REF(Domaine) mon_dom;
  double puissance, nb_dirac;

  template <typename Type_Double>
  inline void calculer_terme_source(int , Type_Double&) const;
};

template <typename Type_Double>
inline void Eval_Dirac_VEF_Face::calculer_terme_source(const int num_face, Type_Double& source) const
{
  const int size = source.size_array();
  if (size > 1) Process::exit("Eval_Dirac_VEF_Face::calculer_terme_source not available for multi-inco !");

  int elem; // pour recuperer l'element associe a cette face

  if (face_voisins(num_face, 1) == -1) elem = face_voisins(num_face, 0);
  else elem = face_voisins(num_face, 1);

  const int test = mon_dom.valeur().type_elem().contient(le_point, elem);
  assert(nb_dirac != -123. && puissance != -123.);
  for (int i = 0; i < size; i++) source[i] = (test == 1) ? nb_dirac * puissance : 0.;
}

#endif /* Eval_Dirac_VEF_Face_included */
