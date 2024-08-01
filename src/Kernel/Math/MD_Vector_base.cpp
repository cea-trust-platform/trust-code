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

#include <MD_Vector_base.h>
#include <TRUSTTabs.h>
#include <ArrOfBit.h>
#include <Param.h>

Implemente_base(MD_Vector_base, "MD_Vector_base", Objet_U);

int MD_Vector_base::get_seq_flags_(ArrOfBit& flags, int line_size) const
{
  int count = 0;
  const int nblocs = blocs_items_to_sum_.size_array() >> 1;
  const int *ptr = blocs_items_to_sum_.addr();
  int j = 0;
  for (int i = nblocs; i; i--)
    {
      // On pourrait optimiser pour les ArrOfBit en ajoutant a cette classe
      //  une methode setflag/clearflag(range_begin, range_end);
      int jmax = (*(ptr++)) * line_size; // debut du prochain bloc d'items sequentiels
      for (; j < jmax; j++)
        flags.clearbit(j);
      jmax = (*(ptr++)) * line_size; // fin du bloc d'items sequentiels
      assert(jmax > j);
      count += jmax - j;
      for (; j < jmax; j++)
        {
          assert(flags[j] == 1); // was already set by default in caller
        }
    }
  // Fin du remplissage
  int sz = get_nb_items_tot() * line_size;
  for (; j < sz; j++)
    flags.clearbit(j);
  return count;
}


/*! @brief method used to dump/restore a descriptor in a file Each process writes a different descriptor.
 *
 * See MD_Vector_tools::dump_vector_with_md()
 *
 */
Entree& MD_Vector_base::readOn(Entree& is)
{
  Param p("MD_Vector_base2");
  p.ajouter("nb_items_tot", &nb_items_tot_);
  p.ajouter("nb_items_reels", &nb_items_reels_);
  p.ajouter("nb_items_seq_tot", &nb_items_seq_tot_);
  p.ajouter("nb_items_seq_local", &nb_items_seq_local_);
  p.ajouter("blocs_items_to_sum", &blocs_items_to_sum_);
  p.ajouter("blocs_items_to_compute", &blocs_items_to_compute_);
  p.lire_avec_accolades(is);

  return is;
}

Sortie& MD_Vector_base::printOn(Sortie& os) const
{
  os << "{" << finl;
  os << "nb_items_tot" << tspace << nb_items_tot_ << finl;
  os << "nb_items_reels" << tspace << nb_items_reels_ << finl;
  os << "nb_items_seq_tot" << tspace << nb_items_seq_tot_ << finl;
  os << "nb_items_seq_local" << tspace << nb_items_seq_local_ << finl;
  os << "blocs_items_to_sum" << tspace << blocs_items_to_sum_;
  os << "blocs_items_to_compute" << tspace << blocs_items_to_compute_;
  os << "}" << finl;
  return os;
}

bool MD_Vector_base::validate(trustIdType sz_array, int line_size) const
{
  int size_r = this->get_nb_items_reels();
  if (size_r >= 0) size_r *= line_size;
  else size_r = -1; // Cas particulier ou la size_reelle ne veut rien dire

  int size_tot = this->get_nb_items_tot() * line_size;
  if (size_tot != sz_array)
    {
      Cerr << "Internal error in TRUSTVect::set_md_vector(): wrong array size\n"
           << " Needed size = " << this->get_nb_items_tot() << " x " << line_size
           << "\n Actual size = " << sz_array << finl;
      Process::exit();
    }
  if (line_size == 0)
    {
      Cerr << "Internal error in TRUSTVect::set_md_vector():\n"
           << " cannot attach descriptor to empty array (line_size_ is zero)" << finl;
      Process::exit();
    }
  return true;
}

int MD_Vector_base::get_sequential_items_flags(ArrOfBit& flags, int line_size) const
{
  const int sz = get_nb_items_tot() * line_size;
  flags.resize_array(sz);
  flags = 1; // valeur par defaut la plus courante (0 est le cas particulier), voir setflag(ArrOfBit)
  return get_seq_flags_(flags, line_size);
}

// This version is just used in SSOR I think:
int MD_Vector_base::get_sequential_items_flags(ArrOfInt& flags, int line_size) const
{
  const int sz = get_nb_items_tot() * line_size;
  flags.resize_array(sz);
  flags = 1; // valeur par defaut la plus courante (0 est le cas particulier), voir setflag(ArrOfBit)

  ArrOfBit flgs_bits(sz);
  int n = get_seq_flags_(flgs_bits, line_size);
  for (int i=0; i<n; i++)
    if (!flgs_bits[i])
      flags[i] = 0;

  return n;
}


/*! @brief methode outil pour ajouter un item a un tableau du genre "blocs" contenant des series de blocs.
 *
 * .. alonge le bloc precedent ou commence un nouveau bloc si l'item n'est pas contigu avec le bloc precedent.
 */
void MD_Vector_base::append_item_to_blocs(ArrOfInt& blocs, int item)
{
  int n = blocs.size_array();
  assert(n % 2 == 0); // le tableau contient des blocs, donc un nombre pair d'elements
  assert(n == 0 || item >= blocs[n - 1]); // les items doivent etre ajoutes dans l'ordre croissant
  if (n == 0 || blocs[n - 1] != item)
    {
      // nouveau bloc
      blocs.append_array(item);
      blocs.append_array(item + 1);
    }
  else blocs[n - 1] = item + 1;
}
