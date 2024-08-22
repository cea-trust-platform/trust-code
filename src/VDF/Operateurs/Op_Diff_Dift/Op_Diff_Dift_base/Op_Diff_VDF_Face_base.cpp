/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Op_Diff_VDF_Face_base.h>
#include <Check_espace_virtuel.h>
#include <Statistiques.h>

extern Stat_Counter_Id diffusion_counter_;

Implemente_base(Op_Diff_VDF_Face_base,"Op_Diff_VDF_Face_base",Op_Diff_VDF_base);

Sortie& Op_Diff_VDF_Face_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Diff_VDF_Face_base::readOn(Entree& s ) { return s ; }


void Op_Diff_VDF_Face_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue()->le_nom().getString();
  if (!matrices.count(nom_inco) || semi_impl.count(nom_inco)) return; //semi-implicite ou pas de bloc diagonal -> rien a faire

  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr, mat2;
  Op_VDF_Face::dimensionner(iter_->domaine(), iter_->domaine_Cl(), mat2);
  mat->nb_colonnes() ? *mat += mat2 : *mat = mat2;

}

void Op_Diff_VDF_Face_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(diffusion_counter_);
  assert_invalide_items_non_calcules(secmem, 0.);
  iter_->ajouter_blocs(matrices, secmem, semi_impl);

  // On ajoute des termes si axi ...
  Op_Diff_VDF_base::ajoute_terme_pour_axi(matrices, secmem, semi_impl);

  statistiques().end_count(diffusion_counter_);
}

double Op_Diff_VDF_Face_base::calculer_dt_stab() const { return Op_Diff_VDF_base::calculer_dt_stab_(iter_->domaine()); }

