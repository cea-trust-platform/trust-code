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

#ifndef TRUST_2_CGNS_included
#define TRUST_2_CGNS_included

#include <TRUSTTabs_forward.h>
#include <TRUST_Ref.h>
#include <cgns++.h>
#include <vector>

#ifdef HAS_CGNS
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#include <pcgnslib.h>
#pragma GCC diagnostic pop
using CGNS_TYPE = CGNS_ENUMT(ElementType_t);
#endif

class Domaine;
class Motcle;

class TRUST_2_CGNS
{
#ifdef HAS_CGNS
public:
  TRUST_2_CGNS() { }
  void associer_domaine_TRUST(const Domaine *, const DoubleTab&, const IntTab&);
  void fill_coords(std::vector<double>&, std::vector<double>&, std::vector<double>&);

  void fill_global_infos();
  void fill_global_infos_poly(const bool);

  const bool& all_procs_write() const { return all_procs_write_; }
  const std::vector<int>& get_proc_non_zero_elem() const { return proc_non_zero_elem_; }

  int get_ne_tot() const { return ne_tot_; }
  const std::vector<int>& get_global_nb_elem() const { return global_nb_elem_; }
  const std::vector<int>& get_global_incr_min_elem() const { return global_incr_min_elem_; }
  const std::vector<int>& get_global_incr_max_elem() const { return global_incr_max_elem_; }

  int get_ns_tot() const { return ns_tot_; }
  const std::vector<int>& get_global_nb_som() const { return global_nb_som_; }
  const std::vector<int>& get_global_incr_min_som() const { return global_incr_min_som_; }
  const std::vector<int>& get_global_incr_max_som() const { return global_incr_max_som_; }


  int get_nfs_tot() const { return nfs_tot_; }
  int get_nfs_offset_tot() const { return nfs_offset_tot_; }
  const std::vector<int>& get_global_incr_min_face_som() const { return global_incr_min_face_som_; }
  const std::vector<int>& get_global_incr_max_face_som() const { return global_incr_max_face_som_; }

  int get_nef_tot() const { return nef_tot_; }
  int get_nef_offset_tot() const { return nef_offset_tot_; }
  const std::vector<int>& get_global_incr_min_elem_face() const { return global_incr_min_elem_face_; }
  const std::vector<int>& get_global_incr_max_elem_face() const { return global_incr_max_elem_face_; }

  int get_nes_offset_tot() const { return nes_offset_tot_; }

  int convert_connectivity(const CGNS_TYPE, std::vector<cgsize_t>&);
  int convert_connectivity_ngon(std::vector<cgsize_t>&, std::vector<cgsize_t>&, const bool);
  int convert_connectivity_nface(std::vector<cgsize_t>&, std::vector<cgsize_t>&);
  CGNS_TYPE convert_elem_type(const Motcle&);

  const std::vector<cgsize_t>& get_local_fs() const { return local_fs_; }
  const std::vector<cgsize_t>& get_local_fs_offset() const { return local_fs_offset_; }

  const std::vector<cgsize_t>& get_local_ef() const { return local_ef_; }
  const std::vector<cgsize_t>& get_local_ef_offset() const { return local_ef_offset_; }

  const std::vector<cgsize_t>& get_local_es() const { return local_es_; }
  const std::vector<cgsize_t>& get_local_es_offset() const { return local_es_offset_; }

private:
  REF(Domaine) dom_trust_;
  REF(DoubleTab) sommets_;
  REF(IntTab) elems_;

  bool par_in_zone_ = false, all_procs_write_ = true;
  int ns_tot_ = -123, ne_tot_ = -123, nb_procs_writing_ = -123;

  std::vector<int> proc_non_zero_elem_;

  std::vector<int> global_nb_elem_, global_incr_min_elem_, global_incr_max_elem_; // elem

  std::vector<int> global_nb_som_, global_incr_min_som_, global_incr_max_som_; //som

  // pour polyedre
  int nfs_tot_ = -123, nef_tot_ = -123, nfs_offset_tot_ = -123, nef_offset_tot_ = -123, nes_offset_tot_ = -123;

  std::vector<int> global_nb_face_som_, global_nb_face_som_offset_,
      global_incr_min_face_som_, global_incr_max_face_som_,
      global_incr_min_face_som_offset_, global_incr_max_face_som_offset_; // pour NGON_n - face_som

  std::vector<int> global_nb_elem_face_, global_nb_elem_face_offset_,
      global_incr_min_elem_face_, global_incr_max_elem_face_,
      global_incr_min_elem_face_offset_, global_incr_max_elem_face_offset_; // pour NFACE_n - elem_face

  // pour polygon
  std::vector<int> global_nb_elem_som_, global_nb_elem_som_offset_,
      global_incr_min_elem_som_offset_, global_incr_max_elem_som_offset_; // pour NGON_n

  std::vector<cgsize_t> local_fs_, local_fs_offset_, local_ef_, local_ef_offset_, local_es_, local_es_offset_;
#endif
};

#endif /* TRUST_2_CGNS_included */
