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
#ifndef Prepro_IBM_base_included
#define Prepro_IBM_base_included

#include <Objet_U.h>
#include <Champ_Don_base.h>
#include <Param.h>
#include <Debog.h>

using namespace std;

class Prepro_IBM_base :public Objet_U
{

  Declare_base(Prepro_IBM_base); // declaration de l'instance a utiliser

public:
  void set_param(Param&);

protected:
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  OWN_PTR(Champ_Don_base) aDomUMesh_; // Mesh calcul
  OWN_PTR(Champ_Don_base) aSkinUMesh_; // Mesh IBM
  double eps_ = 1.0e-12; // precision geometrique
  double c_prepro_ = 0.; // facteur multiplicatif pour la recherche du ptr fluide
  IntTab dimTab_ ; // choix des directions de recherche du pt fluide
  int save_prepro_ = false; // Sauvegarde des résultats dans un fichier MED

  OWN_PTR(Champ_Don_base) fluid_points_;
  OWN_PTR(Champ_Don_base) fluid_elems_;
  OWN_PTR(Champ_Don_base) solid_points_;
  OWN_PTR(Champ_Don_base) solid_elems_;
  OWN_PTR(Champ_Don_base) corresp_elems_;
  OWN_PTR(Champ_Don_base) champ_rotation_, champ_aire_;


  friend class Source_PDF_base;
  friend class Interpolation_IBM_base;
};
#endif


