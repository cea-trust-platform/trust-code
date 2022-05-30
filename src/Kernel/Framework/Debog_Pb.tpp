/****************************************************************************
* Copyright (c) 2022, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Debog_Pb.tpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Debog_Pb_TPP_included
#define Debog_Pb_TPP_included

// Dans gdb: mettre le numero du message entre parentheses dans cette variable avec "set msg_debog_breakpoint=x" et un point d'arret dans la methode debog_breakpoint()
static int msg_debog_breakpoint = 0;
inline void debog_breakpoint() { }
inline void debog_break_test(int msg) { if (msg == msg_debog_breakpoint) debog_breakpoint(); }

template<typename _TYPE_>
void Debog_Pb::verifier_gen(const char *const msg, const TRUSTVect<_TYPE_>& arr, TRUSTVect<_TYPE_> *arr_ref)
{
  if (!arr.get_md_vector().non_nul())
    {
      Cerr << "WARNING in Debog::verifier_gen: array has no parallel metadata. Not checked\n (message: " << msg << ")" << finl;
      if (Process::je_suis_maitre())
        log_file_ << "WARNING in Debog::verifier_gen: array has no parallel metadata. Not checked\n (message: " << msg << ")" << finl;

      detailed_log_file_ << "WARNING in Debog::verifier_gen: array has no parallel metadata. Not checked\n (message: " << msg << ")" << finl;
      return;
    }

  if (test_ignore_msg(msg)) return; // Ignorer ce test

  goto_msg(msg);
  const TRUSTTab<_TYPE_>* scalT = dynamic_cast<const TRUSTTab<_TYPE_>*>(&arr);
  if (scalT)
    {
      // read tab header
      ArrOfInt dims;
      debog_data_file_ >> dims;
    }

  TRUSTVect<_TYPE_> reference;
  debog_data_file_ >> reference;

  const int n = reference.size_array();
  const int nb_items_seq = arr.get_md_vector().valeur().nb_items_seq_tot();
  const int ls = arr.line_size();

  if (n != nb_items_seq * ls)
    {
      Cerr << "Error in Debog_Pb::verifier_gen : wrong sequential size " << n << " (expected " << nb_items_seq * ls << ")" << finl;
      Process::exit();
    }
  verifier_partie(reference, arr, arr_ref);
}

template <typename _TYPE_>
void Debog_Pb::verifier_partie(const TRUSTVect<_TYPE_>& reference, const TRUSTVect<_TYPE_>& arr, TRUSTVect<_TYPE_> *arr_ref)
{
  const MD_Vector_base& md = arr.get_md_vector().valeur();
  if (sub_type(MD_Vector_std, md))
    {
      verifier_partie_std(reference, arr, arr_ref);
    }
  else if (sub_type(MD_Vector_composite, md))
    {
      // This way, each processor writes each part sequentially : part1proc1, part1proc2, ... part2proc1, part2proc2, etc...
      ConstTRUSTTab_parts<_TYPE_> parts(arr);
      TRUSTTab<_TYPE_> ref_part;
      const int n = parts.size();
      int index = 0;
      for (int i = 0; i < n; i++)
        {
          // Construction de la sous-partie de reference:
          // (le tableau de reference contient toutes les valeurs sequentielles, sous-partie par sous-partie, la taille de la sous-partie est egale au nombre total
          // d'items sequentiels multiplie par le linesize de la sous-partie). Attention, toutes les sous-parties n'ont pas forcement la meme linesize
          const TRUSTTab<_TYPE_>& part = parts[i];
          const int sequential_size = part.get_md_vector().valeur().nb_items_seq_tot();
          const int line_size = part.line_size();
          // ref_array() veut un tableau non const, mais on va l'utiliser uniquement en const...
          TRUSTArray<_TYPE_>& cast_array = ref_cast_non_const(TRUSTArray<_TYPE_>, reference);
          ref_part.ref_array(cast_array, index, sequential_size * line_size);
          // Appel recursif pour la sous-partie:
          verifier_partie(ref_part, part, arr_ref);
          index += sequential_size * line_size;
        }
    }
}

template <typename _TYPE_>
void Debog_Pb::verifier_partie_std(const TRUSTVect<_TYPE_>& reference, const TRUSTVect<_TYPE_>& arr, TRUSTVect<_TYPE_> *arr_reference)
{
  static constexpr bool IS_DOUBLE = std::is_same<_TYPE_,double>::value;

  Nom identificateur;
  // Recherche du descripteur du tableau arr parmi les descripteurs connus
  const IntVect& renum = find_renum_vector(arr.get_md_vector(), identificateur);

  _TYPE_ adim = 0;
  if (IS_DOUBLE)
    {
      // Calcul d'une valeur de reference pour adimensionnaliser:
      // Le tableau reference est sequentiel: tous les processeurs ont le meme
      const _TYPE_ adim1 = local_max_abs_vect(reference);
      // Le tableau arr est parallele. On calcule la reference en utilisant uniquement
      //  la partie reele (eventuellement la partie virtuelle a le droit de ne pas etre a jour)
      const _TYPE_ adim2 = mp_max_abs_vect(arr, VECT_REAL_ITEMS);
      adim = std::max(adim1, adim2);

      if (seuil_absolu_ <= 0.)
        {
          Cerr << "Error in Debog.cpp verifier_partie_std: seuil_absolu <= 0" << finl;
          Process::exit();
        }
      if (adim < seuil_absolu_)
        {
          // Toutes les valeurs sont inferieures au seuil absolu => ok
          if (Process::je_suis_maitre())
            {
              log_file_ << " OK            : All values below seuil_absolu_ (" << seuil_absolu_ << ") id=" << identificateur << finl;
              detailed_log_file_ << " OK            : All values below seuil_absolu_ (" << seuil_absolu_ << ") id=" << identificateur << finl;
            }
          return;
        }
    }

  const int ls = arr.line_size();
  int i = 0;
  _TYPE_ max_err_items_reels = 0, max_err_items_virt = 0;

  int outbounds = 0;
  // items reels, puis items virtuels
  for (int step = 0; step < 2; step++)
    {
      int n;
      const char * message;
      if (step == 0)
        {
          n = renum.size();
          message = " (item reel)";
        }
      else
        {
          n = renum.size_totale();
          message = " (item virtuel)";
        }
      _TYPE_ max_err = 0;
      const _TYPE_ *arr_ptr = arr.addr();

      for (; i < n; i++)
        {
          if (renum[i]>=0) {
              const int i1 = i * ls; // indice dans "arr"
              const int i2 = renum[i] * ls; // indice dans "reference"
              for (int j = 0; j < ls; j++) {
                  const _TYPE_ x = arr_ptr[i1 + j]; // ne pas passer par operator[] pour ne pas planter sur DMAXFLOAT
                  const _TYPE_ y = reference[i2 + j];
                  // Comparaison de x et y
                  if (IS_DOUBLE) {
                      const _TYPE_ delta = std::fabs(x - y) / adim;
                      max_err = std::max(max_err, delta);
                      // pour les items reels, indiquer si on est hors bornes:
                      if (step == 0 && !(x >= -DMAXFLOAT && x <= DMAXFLOAT))
                          outbounds = 1;
                      if (delta > seuil_relatif_) {
                          detailed_log_file_ << " ECART reference[" << i2 + j << "]=" << y << " calcul[" << i1 + j
                                             << "]=" << x;
                          detailed_log_file_ << " relative error=" << delta << " (adim=" << adim << ")" << message
                                             << finl;
                      }
                  } else // int
                  {
                      // For integers, values must match exactly !
                      if (x != y) {
                          detailed_log_file_ << " ECART reference[" << i2 + j << "]=" << y << " calcul[" << i1 + j
                                             << "]=" << x << message << finl;
                          max_err = 1;
                      }
                  }
              }
          }
        }

      if (IS_DOUBLE) max_err = Process::mp_max(max_err);
      else max_err = ::mp_max(max_err);

      if (step == 0)
        max_err_items_reels = max_err;
      else
        max_err_items_virt = max_err;
    }

  const char * resu = 0;
  int call_error_function = 0;
  _TYPE_ seuil = IS_DOUBLE ? seuil_relatif_ : 0;

  if (max_err_items_reels <= seuil)
    {
      if (max_err_items_virt <= seuil)
        resu = "OK           ";
      else
        resu = "OK REAL ONLY ";
    }
  else
    {
      call_error_function = 1;
      if (!outbounds)
        resu = "ERROR        ";
      else
        resu = "ERROR(DMAXFLT)";
    }
  if (Process::je_suis_maitre())
    {
      if (IS_DOUBLE)
        log_file_ << " " << resu << " : Max relative error " << max_err_items_reels << " (max ref value=" << adim << ") id=" << identificateur << finl;
      else
        log_file_ << " " << resu << " : integer field " << identificateur << finl;
    }

  if (IS_DOUBLE)
    detailed_log_file_ << " " << resu << " : Max relative error " << max_err_items_reels << " (max ref value=" << adim << ") id=" << identificateur << finl;
  else
    detailed_log_file_ << " " << resu << " : integer field " << identificateur << finl;

  if (call_error_function)
    error_function();
  if (arr_reference)
    {
      TRUSTVect<_TYPE_>& arr_ref = *arr_reference;

      if (arr_ref.line_size() != ls || !(arr_ref.get_md_vector() == arr.get_md_vector()))
        {
          Cerr << "Error in Debog_Pb::verifier_partie_std: the array provided to store the reference value\n"
               << " does not have the same size/descriptor as the source array" << finl;
          Process::exit();
        }
      if (Process::je_suis_maitre())
        log_file_ << "                 Return reference value (including virtual items)" << finl;
      const int n = renum.size_totale();
      _TYPE_ *arr_ptr = arr_ref.addr();
      for (int ibis = 0; ibis < n; ibis++)
        {
          if (renum[ibis]>=0) {
              const int i1 = ibis * ls; // indice dans "arr"
              const int i2 = renum[ibis] * ls; // indice dans "reference"
              for (int j = 0; j < ls; j++) {
                  const _TYPE_ y = reference[i2 + j];
                  arr_ptr[i1 + j] = y; // ne pas passer par operator[] pour ne pas planter sur DMAXFLOAT
              }
          }
        }
    }
}

template <typename _TYPE_>
void Debog_Pb::ecrire_gen(const char* const msg, const TRUSTVect<_TYPE_>& arr, int num_deb)
{
  if (!arr.get_md_vector().non_nul())
    {
      Cerr << "WARNING in Debog::ecrire: array has no parallel metadata" << finl;
      Cerr << " (message: " << msg << ")" << finl;
      return;
    }
  Sortie& os = write_debog_data_file_;
  const MD_Vector_base& md = arr.get_md_vector().valeur();
  const int ls = arr.line_size();
  if (Process::je_suis_maitre())
    {
      os << "msg : " << debog_msg_count_ << " : " << msg << " FinMsg " << finl;
      Cerr << "DEBOG: writing array, message " << debog_msg_count_ << " : " << msg << finl;
      if (num_deb >= 0)
        os << num_deb << finl;
      // Dump array header:

      const TRUSTTab<_TYPE_>* tabb = dynamic_cast<const TRUSTTab<_TYPE_>*>(&arr);
      if (tabb)
        {
          // Header for TRUSTTab<_TYPE_>:
          const TRUSTTab<_TYPE_>& tab = *tabb;
          const int n =  tab.nb_dim();
          os << n << finl;
          // total number of lines:
          os << md.nb_items_seq_tot();
          // other dimensions:
          for (int i = 1; i < n; i++)
            os << space << tab.dimension(i);
          os << finl;
        }
      // Header of ArrayOfDouble:
      os << md.nb_items_seq_tot() * ls << finl;
    }

  ecrire_partie(arr);
}

template<typename _TYPE_>
void Debog_Pb::ecrire_partie(const TRUSTVect<_TYPE_>& arr)
{
  const MD_Vector_base& md = arr.get_md_vector().valeur();
  if (sub_type(MD_Vector_std, md))
    {
      const int ls = arr.line_size();
      write_debog_data_file_.put(arr.addr(), arr.size_array(), ls);
    }
  else if (sub_type(MD_Vector_composite, md))
    {
      // This way, each processor writes each part sequentially : part1proc1, part1proc2, ... part2proc1, part2proc2, etc...
      ConstTRUSTTab_parts<_TYPE_> parts(arr);
      const int n = parts.size();
      for (int i = 0; i < n; i++)
        {
          const TRUSTTab<_TYPE_>& t = parts[i];
          ecrire_partie(t);
        }
    }
}

template<typename _TYPE_>
void Debog_Pb::verifier(const char *const msg, const TRUSTVect<_TYPE_>& arr, TRUSTVect<_TYPE_> *arr_ref)
{
  if (nom_pb_ != nom_pb_actuel_)
    return;

  if (test_ignore_msg(msg))
    {
      // Ignorer ce test
      return;
    }
  debog_break_test(debog_msg_count_);
  if(mode_db_ == 0)
    {
      ecrire_gen(msg, arr);
    }
  else
    {
      verifier_gen(msg, arr, arr_ref);
    }
  debog_msg_count_++;
}

template<typename _TYPE_>
typename std::enable_if< (std::is_convertible<_TYPE_, double>::value) || (std::is_convertible<_TYPE_, int>::value),void >::type
Debog_Pb::verifier(const char *const msg, _TYPE_ x, _TYPE_ *ref_value)
{
  static constexpr bool IS_DOUBLE = std::is_same<_TYPE_,double>::value;

  if (nom_pb_ != nom_pb_actuel_) return;

  if (test_ignore_msg(msg)) return; // Ignorer ce test

  debog_break_test (debog_msg_count_);
  if (mode_db_ == 0)
    {
      // write
      Sortie& os = write_debog_data_file_;
      Cerr << "DEBOG: writing scalar, message " << debog_msg_count_ << " : " << msg << finl;
      os << "msg : " << debog_msg_count_ << " : " << msg << " FinMsg " << finl;
      os << x << finl;
    }
  else
    {
      // read and compare
      goto_msg(msg);
      _TYPE_ y, err;
      debog_data_file_ >> y;

      if (IS_DOUBLE)
        {
          const _TYPE_ adim = std::max(std::fabs(x), std::fabs(y));
          const _TYPE_ delta = std::fabs(x - y);
          err = 0;
          if (delta >= seuil_absolu_ && delta / adim >= seuil_relatif_)
            {
              err = delta;
              detailed_log_file_ << " ECART (double) reference=" << y << " calcul=" << x << " relative error=" << delta << " (adim=" << adim << ")" << finl;
            }
        }
      else // int
        {
          err = ((x - y) != 0);
          if (err) detailed_log_file_ << " ECART (int) reference=" << y << " calcul=" << x << finl;
        }

      err = mp_sum(err);
      if (Process::je_suis_maitre())
        {
          const char *ok = (err > 0.) ? " ERROR       " : " OK           ";
          if (IS_DOUBLE)
            {
              log_file_ << ok << " : comparing double: reference=" << y << " deltamax=" << err << finl;
              if (err > 0.) error_function();
            }
          else // int
            {
              log_file_ << ok << " : comparing int: reference=" << y << finl;
              if (err) error_function();
            }
        }
      if (ref_value)
        {
          if (Process::je_suis_maitre())
            log_file_ << "                Request reference value" << finl;
          *ref_value = y;
        }
    }
  debog_msg_count_++;
}

#endif /* Debog_Pb_TPP_included */
