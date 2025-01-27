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

#ifndef MD_Vector_base_included
#define MD_Vector_base_included

#include <TRUSTArray.h>
#include <ArrOfBit.h>

class Schema_Comm_Vecteurs;
class Echange_EV_Options;
class MD_Vector;
class MD_Vector_seq;
class MD_Vector_composite;
class MD_Vector_std;

/*! @brief Base class for distributed vectors parallel descriptors.
 *
 * See also the MD_Vector class (which is a kind of DERIV() of this class), and see derived classes
 * MD_Vector_std, MD_Vector_composite and MD_Vector_seq.
 */
class MD_Vector_base : public Objet_U
{
  Declare_base(MD_Vector_base);

public:

  virtual int get_nb_items_reels() const { return nb_items_reels_; }
  virtual int get_nb_items_tot() const { return nb_items_tot_; }
  virtual int nb_items_seq_tot() const { return nb_items_seq_tot_; }
  virtual int nb_items_seq_local() const { return nb_items_seq_local_; }

  virtual const ArrOfInt& get_items_to_compute() const = 0;
  virtual const ArrOfInt& get_items_to_sum() const = 0;

  int get_sequential_items_flags(ArrOfBit& flags, int line_size=1) const;
  int get_sequential_items_flags(ArrOfInt& flags, int line_size=1) const;

  virtual bool validate(trustIdType sz_array, int line_size) const;

  virtual void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const = 0;
  virtual void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const = 0;
  virtual void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const = 0;

  virtual void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, FloatVect&) const = 0;
  virtual void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, FloatVect&) const = 0;
  virtual void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, FloatVect&) const = 0;

  virtual void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const = 0;
  virtual void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const = 0;
  virtual void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const = 0;

#if INT_is_64_ == 2
  virtual void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, TIDVect&) const = 0;
  virtual void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, TIDVect&) const = 0;
  virtual void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, TIDVect&) const = 0;
#endif

  // Used in MD_Vector_tools:
  virtual void fill_md_vect_renum(const IntVect& renum, MD_Vector& md_vect) const = 0;

  // Tool method:
  static void append_item_to_blocs(ArrOfInt& blocs, int item);

  /*! Whether algorithms in TRUSTVect_tools should use the blocks stored in members MD_Vector_mono::blocs_items_to_*
   */
  virtual bool use_blocks() const = 0;

protected:
  // Actual implementation of the sequential flags retrieval - overriden in MD_Vector_seq:
  virtual int get_seq_flags_impl(ArrOfBit& flags, int line_size) const;

  // Tool methods:
  static void append_blocs(ArrOfInt& dest, const ArrOfInt& src, int offset=0, int multiplier=1);
  static void append_items(ArrOfInt& dest, const ArrOfInt& src, int offset=0, int multiplier=1);

  // Nombre total d'items (reels+virtuels), utilise pour connaitre la taille des tableaux a creer
  int   nb_items_tot_ = -1;
  // Nombre d'items "reels" (propriete dimension(0) du tableau ou size() pour le vecteur)
  // En principe les items reels sont tous ceux appartenant aux elements reels, ils sont regroupes au
  // debut du tableau aux indices 0 <= i < nb_items_reels_.
  // Si ce nombre vaut -1, il n'y a pas de separation entre items reels et items virtuels
  //  (cas des tableaux P1Bulle multilocalisation pour lesquels les items reels et virtuels sont melanges)
  int   nb_items_reels_ = -1;

  // Nombre total (sur tous les procs) d'items sequentiels (c'est mp_sum(nb_items_seq_local_))
  int   nb_items_seq_tot_ = -1;
  // Nombre d'items sequentiels sur ce processeur (c'est le nombre d'items dans les blocs de blocs_items_to_sum_)
  int   nb_items_seq_local_ = -1;

private:
  // This class may modify ref_count_:
  friend class MD_Vector;

};

#endif
