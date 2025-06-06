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

#ifndef Evaluateur_Source_VEF_Face_included
#define Evaluateur_Source_VEF_Face_included

#include <Evaluateur_Source_Face.h>

class Evaluateur_Source_VEF_Face : public Evaluateur_Source_Face
{
public:
  Evaluateur_Source_VEF_Face() { }
  Evaluateur_Source_VEF_Face(const Evaluateur_Source_VEF_Face& );
  virtual void changer_volumes_entrelaces_Cl(DoubleVect& );
  void completer() override;

  template <typename Type_Double>
  void calculer_terme_source_standard(const int , Type_Double&  ) const { Process::exit("Evaluateur_Source_VEF_Face::calculer_terme_source_standard must be overloaded !!"); }
  template <typename Type_Double>
  void calculer_terme_source_non_standard(const int , Type_Double&  ) const { Process::exit("Evaluateur_Source_VEF_Face::calculer_terme_source_non_standard must be overloaded !!"); }

  // Provisoire: to incrementally port different evaluators:
  virtual bool has_view() const { return false; }
  void calculer_terme_source_standard_view(const int , DoubleArrView ) const { Process::Kokkos_exit("Evaluateur_Source_VEF_Face::calculer_terme_source_standard must be overloaded !!"); }
  void calculer_terme_source_non_standard_view(const int , DoubleArrView ) const { Process::Kokkos_exit("Evaluateur_Source_VEF_Face::calculer_terme_source_non_standard must be overloaded !!"); }
protected:
  DoubleVect volumes_entrelaces_Cl;
  IntTab face_voisins;
};

#endif /* Evaluateur_Source_VEF_Face_included */
