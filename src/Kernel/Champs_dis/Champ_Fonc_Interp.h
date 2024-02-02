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

#ifndef Champ_Fonc_Interp_included
#define Champ_Fonc_Interp_included

#include <Champ_Fonc_P0_base.h>
#include <Probleme_base.h>

#ifdef MEDCOUPLING_
#include <MEDCouplingFieldDouble.hxx>
#include <MEDCouplingRemapper.hxx>
#ifdef MPI_
#include <OverlapDEC.hxx>
#endif
#endif // MEDCOUPLING_

class Champ_Fonc_Interp : public Champ_Fonc_P0_base
{
  Declare_instanciable_sans_destructeur(Champ_Fonc_Interp);
public:
  virtual ~Champ_Fonc_Interp();
  int initialiser(double ) override;
  void mettre_a_jour(double) override;

protected:
  void init_fields();
  void update_fields();
  REF(Probleme_base) pb_loc_, pb_dist_;
  REF(Domaine) dom_loc_, dom_dist_;
  bool is_initialized_ = false, is_elem_trgt_ = true /* par default aux elems */;
  int use_dec_ = -123, sharing_algo_ = -123;
  double default_value_ = DMAXFLOAT;
  DoubleTab valeurs_elem_, valeurs_faces_elem_;
  Champ espace_stockage_;

#ifdef MEDCOUPLING_
  MEDCoupling::NatureOfField nature_;
  MEDCoupling::MCAuto<MEDCoupling::MEDCouplingFieldDouble> local_field_, distant_field_;
  MEDCoupling::MCAuto<MEDCoupling::DataArrayDouble> local_array_, distant_array_;
#ifdef MPI_
  std::shared_ptr<MEDCoupling::OverlapDEC> dec_ = nullptr;
#endif
  bool is_dec_initialized_ = false;
  bool verbose_ = false;
#endif // MEDCOUPLING_
};

#endif /* Champ_Fonc_Interp_included */
