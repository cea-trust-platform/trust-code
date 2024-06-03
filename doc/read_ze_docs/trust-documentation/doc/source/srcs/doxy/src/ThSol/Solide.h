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

#ifndef Solide_included
#define Solide_included

#include <Milieu_base.h>

/*! @brief Classe Solide Cette classe represente un milieu solide et ses proprietes.
 *
 * @sa Milieu_base
 */
class Solide : public Milieu_base
{
  Declare_instanciable(Solide);
public:
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  void set_param(Param& param) override;
  void verifier_coherence_champs(int& err,Nom& message) override;
  void discretiser(const Probleme_base& pb, const Discretisation_base& dis) override;

  inline const DoubleTab& get_user_champ() const { return mon_champ_->valeurs(); }
  inline const bool& is_user_defined() const { return is_user_defined_; }
  inline const Motcle& get_user_champ_name() const { return nom_champ_; }

private:
  bool is_user_defined_ = false;
  Motcle nom_champ_;
  Champ_Don mon_champ_;
};

#endif /* Solide_included */
