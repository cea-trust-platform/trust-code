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

#ifndef Op_Diff_VDF_base_included
#define Op_Diff_VDF_base_included

#include <Operateur_Diff_base.h>
#include <Iterateur_VDF_base.h>
#include <TRUST_Deriv.h>

class Champ_Fonc;

/*! @brief class Op_Diff_VDF_base Classe de base des operateurs de diffusion VDF
 *
 */
class Op_Diff_VDF_base : public Operateur_Diff_base
{
  Declare_base(Op_Diff_VDF_base);
public:
  inline Op_Diff_VDF_base(const Iterateur_VDF_base& iter_base) { iter_ = iter_base; }
  void completer() override;

  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  int impr(Sortie& os) const override;

  inline const OWN_PTR(Iterateur_VDF_base)& get_iter() const { return iter_; }
  inline OWN_PTR(Iterateur_VDF_base)& get_iter() { return iter_; }

  inline int has_interface_blocs() const override { return 1; }
  void init_op_ext() const override;

  virtual void calculer_flux_bord(const DoubleTab& inco, const DoubleTab& val_b) const = delete;
  void contribuer_au_second_membre(DoubleTab& resu) const override { iter_->contribuer_au_second_membre(resu); }
  void check_multiphase_compatibility() const override { }

protected:
  double calculer_dt_stab_(const Domaine_VDF& zone_VDF) const;
  void ajoute_terme_pour_axi(matrices_t , DoubleTab& , const tabs_t& ) const;

  OWN_PTR(Iterateur_VDF_base) iter_;
  mutable int op_ext_init_ = 0;
};

#endif /* Op_Diff_VDF_base_included */
