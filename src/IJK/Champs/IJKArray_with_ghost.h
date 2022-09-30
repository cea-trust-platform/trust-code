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

#ifndef IJKArray_with_ghost_included
#define IJKArray_with_ghost_included

#include <IJK_communications.h>
#include <communications.h>
#include <stat_counters.h>
#include <IJK_Splitting.h>
#include <Statistiques.h>
#include <TRUST_Vector.h>
#include <simd_tools.h>
#include <TRUSTVect.h>

/*! @brief This is an array with [] operator.
 *
 * Allocate array with resize(n, ghost). tab[i] is valid for "-ghost <= i < n + ghost"
 *
 */
template<typename _TYPE_, typename _TYPE_ARRAY_>
class IJKArray_with_ghost : public Objet_U
{
protected:
  unsigned taille_memoire() const override { throw; }

  int duplique() const override
  {
    IJKArray_with_ghost *xxx = new IJKArray_with_ghost(*this);
    if (!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override { return os; }

  Entree& readOn(Entree& is) override { return is; }

public:
  IJKArray_with_ghost() : ghost_(0) { }
  void resize(int n, int new_ghost)
  {
    tab_.resize_array(n + 2 * new_ghost);
    ghost_ = new_ghost;
  }
  _TYPE_ operator[](int i) const { return tab_[i + ghost_]; }
  _TYPE_& operator[](int i) { return tab_[i + ghost_]; }
  const _TYPE_ARRAY_& data() const { return tab_; }
  int size() const { return tab_.size_array() - 2 * ghost_; }
  int ghost() const { return ghost_; }
  inline void echange_espace_virtuel(int pe_min, int pe_max);

protected:
  _TYPE_ARRAY_ tab_;
  int ghost_;
};

using ArrOfFloat_with_ghost = IJKArray_with_ghost<float,ArrOfFloat>;
using Vect_ArrOfFloat_with_ghost = TRUST_Vector<ArrOfFloat_with_ghost>;
using ArrOfDouble_with_ghost = IJKArray_with_ghost<double,ArrOfDouble>;
using Vect_ArrOfDouble_with_ghost= TRUST_Vector<ArrOfDouble_with_ghost>;

template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJKArray_with_ghost<_TYPE_,_TYPE_ARRAY_>::echange_espace_virtuel(int pe_min, int pe_max)
{
  statistiques().begin_count(echange_vect_counter_);
  // envoi a pe_max et reception de pe_min
  const int n = tab_.size_array() - ghost_ * 2;
  _TYPE_ *pdata = tab_.addr();
  ::envoyer_recevoir(pdata + n, ghost_ * sizeof(_TYPE_), pe_max, pdata, ghost_ * sizeof(_TYPE_), pe_min);
  // l'autre
  ::envoyer_recevoir(pdata + ghost_, ghost_ * sizeof(_TYPE_), pe_min, pdata + n + ghost_, ghost_ * sizeof(_TYPE_), pe_max);
  statistiques().end_count(echange_vect_counter_);
}


#endif /* IJKArray_with_ghost_included */
