/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <TRUST_Ref.h>
#include <TRUSTTab.h>

class Eval_Puiss_Th_QC_VEF_Face: public Evaluateur_Source_VEF_Face
{
public:
  Eval_Puiss_Th_QC_VEF_Face() { }
  void completer() override;
  void associer_puissance(const Champ_Don_base&);
  void mettre_a_jour() override { }

protected:
  OBS_PTR(Champ_Don_base) la_puissance;
  DoubleTab tab_puissance;
  DoubleVect tab_volumes;
  mutable int nb_faces_elem = 0;
};

class Eval_Puiss_Th_QC_VEF_Face_View: public Eval_Puiss_Th_QC_VEF_Face
{
public:
  virtual bool has_view() const override { return true; }
  void set(const Eval_Puiss_Th_QC_VEF_Face_View& eval) const
  {
    nb_faces_elem = eval.nb_faces_elem;
    if (eval.tab_puissance.nb_dim() == 1)
      puissance_vect_ = static_cast<const ArrOfDouble&>(eval.tab_puissance).view_ro();
    else
      puissance_ = eval.tab_puissance.view_ro();
    volumes_ = eval.tab_volumes.view_ro();
    face_voisins_ = eval.face_voisins.view_ro();
    volumes_entrelaces_ = eval.volumes_entrelaces.view_ro();
    volumes_entrelaces_Cl_ = eval.volumes_entrelaces_Cl.view_ro();
    porosite_surf_ = eval.porosite_surf.view_ro();
  };
  KOKKOS_INLINE_FUNCTION
  void calculer_terme_source_standard_view(int num_face, DoubleArrView source) const
  {
    const int size = (int)source.size();
    if (size > 1) Process::Kokkos_exit("Eval_Puiss_Th_QC_VEF_Face::calculer_terme_source_standard not available for multi-inco !");

    if (puissance_.extent(0)==1)
      for (int i = 0; i < size; i++) source[i] = puissance_(0, 0) * volumes_entrelaces_[num_face] * porosite_surf_[num_face];
    else
      {
        int elem0 = face_voisins_(num_face, 0), elem1 = face_voisins_(num_face, 1);
        double V0 = volumes_(elem0), V1 = volumes_(elem1);
        bool vect = puissance_vect_.size()>0;
        double P0 = vect ? puissance_vect_(elem0) : puissance_(elem0, 0);
        double P1 = vect ? puissance_vect_(elem1) : puissance_(elem1, 0);
        for (int i = 0; i < size; i++) source[i] = ((P0 * V0 + P1 * V1) / nb_faces_elem) * porosite_surf_[num_face];
      }
  }
  KOKKOS_INLINE_FUNCTION
  void calculer_terme_source_non_standard_view(int num_face, DoubleArrView source) const
  {
    const int size = (int)source.size();
    if (size > 1) Process::Kokkos_exit("Eval_Puiss_Th_QC_VEF_Face::calculer_terme_source_non_standard not available for multi-inco !");

    if (puissance_.extent(0)==1)
      for (int i = 0; i < size; i++) source[i] = puissance_(0, 0) * volumes_entrelaces_Cl_[num_face] * porosite_surf_(num_face);
    else
      {
        int elem0 = face_voisins_(num_face, 0), elem1 = face_voisins_(num_face, 1);
        double V0 = volumes_(elem0), V1 = (elem1 != -1 ? volumes_(elem1) : 0);
        bool vect = puissance_vect_.size()>0;
        double P0 = vect ? puissance_vect_(elem0) : puissance_(elem0, 0);
        double P1 = (elem1 != -1 ? (vect ? puissance_vect_(elem1) : puissance_(elem1, 0)) : 0);
        for (int i = 0; i < size; i++) source[i] = ((P0 * V0 + P1 * V1) / nb_faces_elem) * porosite_surf_[num_face];
      }
  }
private:
  // Views
  mutable CDoubleTabView puissance_;
  mutable CDoubleArrView puissance_vect_;
  mutable CDoubleArrView volumes_;
  mutable CIntTabView face_voisins_;
  mutable CDoubleArrView volumes_entrelaces_;
  mutable CDoubleArrView volumes_entrelaces_Cl_;
  mutable CDoubleArrView porosite_surf_;
};
#endif /* Eval_Puiss_Th_QC_VEF_Face_included */
