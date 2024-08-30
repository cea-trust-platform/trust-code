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

#ifndef MD_Vector_mono_included
#define MD_Vector_mono_included

#include <MD_Vector_base.h>

/*! @brief Generic class for all mono-block MD_Vectors (i.e. non compoosite)
 *
 * The two main members of this class are blocs_items_to_sum_ and blocs_items_to_compute_.
 * Note that those two members are hence **not** present in a MD_Vector_composite. For this case
 * the redirection is made to the inner MD_Vector_mono member which aggregates the information.
 *
 * See also class MD_Vector_composite
 */
class MD_Vector_mono : public MD_Vector_base
{

  Declare_base(MD_Vector_mono);

public:
  const ArrOfInt& get_items_to_sum() const override { return blocs_items_to_sum_; }
  const ArrOfInt& get_items_to_compute() const override { return blocs_items_to_compute_; }

protected:
  // MD_Vector_composite needs to see inside MD_Vector_mono because of its global_md_ member:
  friend class MD_Vector_composite;

  // Methods to extend/complete a MD_Vector_mono with another MD_Vector_mono - not all combinations are possible.
  // See derived classes MD_Vector_seq and MD_Vector_std.
  // For example extending a MD_Vector_std with a MD_Vector_seq is not possible.
  virtual void append_from_other_std(const MD_Vector_std& src, int offset, int multiplier) { throw; }
  virtual void append_from_other_seq(const MD_Vector_seq& src, int offset, int multiplier) { throw; }

  // ***** Les membres suivants sont utilises pour calculer des sommes, produits scalaires, normes ******
  // Indices de tous les items dont je suis proprietaire (ce sont les "items sequentiels", definis comme
  //  etant tous ceux dont la valeur n'est pas recue d'un autre processeur lors d'un echange_espace_virtuel).
  //  Pour faire une somme sur tous les items, il faut sommer les valeurs de tous les items
  //  de ces blocs. Le tableau contient debut_bloc1, fin_bloc1, debut_bloc2, fin_bloc2, etc...
  //  (fin_bloc est l'indice du dernier element + 1)
  //  (structure utilisee pour les sauvegardes sequentielles (xyz ou debog), les calculs de normes de vecteurs, etc)
  ArrOfInt blocs_items_to_sum_;
  // Indices de tous les items pour lesquels il faut calculer une valeur
  //  (utilise par DoubleTab::operator+=(const DoubleTab &) par exemple)
  // En theorie, il suffirait de prendre blocs_items_to_sum_, mais ce dernier est
  //  plein de trous et peut etre inefficace. En pratique, on calcule toutes les valeurs
  //  reeles.
  ArrOfInt blocs_items_to_compute_;


};
#endif
