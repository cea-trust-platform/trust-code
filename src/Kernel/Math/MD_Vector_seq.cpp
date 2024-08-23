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

#include <MD_Vector_seq.h>
#include <TRUSTTabs.h>
#include <ArrOfBit.h>

Implemente_instanciable_sans_constructeur(MD_Vector_seq,"MD_Vector_seq",MD_Vector_base);

Entree& MD_Vector_seq::readOn(Entree& is) { return is; }

Sortie& MD_Vector_seq::printOn(Sortie& os) const { return os; }


int MD_Vector_seq::get_nb_items_reels() const
{
  assert(nb_items_ < std::numeric_limits<int>::max());
  return (int)nb_items_;
}

int MD_Vector_seq::get_nb_items_tot() const
{
  assert(nb_items_ < std::numeric_limits<int>::max());
  return (int)nb_items_;
}

int MD_Vector_seq::nb_items_seq_tot() const
{
  assert(nb_items_ < std::numeric_limits<int>::max());
  return (int)nb_items_;
}

int MD_Vector_seq::nb_items_seq_local() const
{
  assert(nb_items_ < std::numeric_limits<int>::max());
  return (int)nb_items_;
}


int MD_Vector_seq::get_seq_flags_(ArrOfBit& flags, int line_size) const
{
  assert(nb_items_*line_size < std::numeric_limits<int>::max());

  const int sz = (int)nb_items_ * line_size;
  flags.resize_array(sz);
  flags = 1;  // everything is present and must be included in sequential
  return sz;
}

