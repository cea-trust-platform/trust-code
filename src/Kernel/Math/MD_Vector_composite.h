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

#ifndef MD_Vector_composite_included
#define MD_Vector_composite_included

#include <MD_Vector_std.h>
#include <MD_Vector.h>
#include <TRUSTTabs.h>
#include <Noms.h>
#include <memory>

/*! @brief Metadata for a distributed composite vector.
 *
 * This class describes a vector that contains one or more sub-vectors (see P1bubble arrays),
 *   each sub-vector has the same shape as the base vector, plus one dimension if shapes_[i] > 0
 *   See also class DoubleTab_Parts and class ConstDoubleTab_Parts
 *
 * MD_Vector_composite can be nested (for example when using the U_P solver, where velocity and pressure are
 * gathered in a single vector, and pressure itself has different support entities).
 *
 * The overall parallel information is aggregated in the member global_md_ which contains the overall equivalent
 * of all the parts.
 */
class MD_Vector_composite : public MD_Vector_base
{
  Declare_instanciable(MD_Vector_composite);
public:
  void add_part(const MD_Vector& part, int shape = 0, Nom name = "");

  void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, DoubleVect& v) const override   { global_md_->initialize_comm(opt, c, v); }
  void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, DoubleVect& v) const override { global_md_->prepare_send_data(opt, c, v); }
  void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, DoubleVect& v) const override { global_md_->process_recv_data(opt, c, v); }

  void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, FloatVect& v) const override   { global_md_->initialize_comm(opt, c, v); }
  void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, FloatVect& v) const override { global_md_->prepare_send_data(opt, c, v); }
  void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, FloatVect& v) const override { global_md_->process_recv_data(opt, c, v); }

  void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, IntVect& v) const override   { global_md_->initialize_comm(opt, c, v); }
  void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, IntVect& v) const override { global_md_->prepare_send_data(opt, c, v); }
  void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, IntVect& v) const override { global_md_->process_recv_data(opt, c, v); }

#if INT_is_64_ == 2
  void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, TIDVect& v) const override    { global_md_->initialize_comm(opt, c, v); }
  void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, TIDVect& v) const override  { global_md_->prepare_send_data(opt, c, v); }
  void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& c, TIDVect& v) const override  { global_md_->process_recv_data(opt, c, v); }
#endif

  const ArrOfInt& get_items_to_sum() const override { assert(global_md_ != nullptr); return global_md_->blocs_items_to_sum_; }
  const ArrOfInt& get_items_to_compute() const override { assert(global_md_ != nullptr); return global_md_->blocs_items_to_compute_; }

  const MD_Vector_mono& global_md() const { assert(global_md_ != nullptr); return *global_md_; }
  inline int nb_parts() const { return data_.size(); }
  inline int get_shape(int i) const { return shapes_[i]; }
  inline const MD_Vector& get_desc_part(int i) const { return data_[i]; }
  inline int get_part_offset(int i) const { return parts_offsets_[i]; }
  inline Nom get_name(int i) const { return names_[i]; }

  bool use_blocks() const override { assert(is_seq_ != -1); return is_seq_ == 0; }

  int lire_motcle_non_standard(const Motcle& mot, Entree& is) override;

protected:
  void fill_md_vect_renum(const IntVect& renum, MD_Vector& md_vect) const override;
  int get_seq_flags_impl(ArrOfBit& flags, int line_size) const override;

  // Same as in MD_Vector_std - those members should be public, but a lot of tools/etc to fix ...:
  VECT(MD_Vector) data_;
  // Descriptor for the entire vector - necessarily a non-composite MD_Vect:
  std::shared_ptr<MD_Vector_mono> global_md_;
  // Start index of the n-th part,
  // parts_offsets_[j] = SUM(i=0..j-1) of ( data_[i].valeure().nb_items_tot() * shapes[i] )
  ArrOfInt parts_offsets_;
  // Shape of each part of the array (see class DoubleTab_Parts)
  ArrOfInt shapes_;
  // Name of each part
  Noms names_;

private:
  int is_seq_ = -1; ///< 1: global_md_ is a 'MD_Vector_seq', 0: global_md_ is 'MD_Vector_std', -1: not initialized.
  void instanciate_std();
  void instanciate_seq();

};

#endif /* MD_Vector_composite_included */
