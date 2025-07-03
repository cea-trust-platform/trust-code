/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Champ_Morceaux_included
#define Champ_Morceaux_included

#include <Champ_Fonc_P0_base.h>
#include <Champ_Don_base.h>
#include <Probleme_base.h>

/*
field on subdomains defined from several fields, each defined on the whole domain.
Example:
Champ_Morceaux dom n
{
  subdomain1 Champ_Uniforme 1 x
  subdomain2 Champ_fonc_txyz_Morceaux dom n ...
}
*/

class Champ_Morceaux : public Champ_Fonc_P0_base
{
  Declare_instanciable(Champ_Morceaux);
public:
  void mettre_a_jour(double) override;
  int initialiser(const double t) override;
  double changer_temps(const double t) override;
  const Domaine& domaine() const override { return pb_->domaine(); }

  const Nom& fixer_unite(const Nom& unit) override;
  const Nom& fixer_unite(int i, const Nom& unit) override;

  void associer_domaine_dis_base(const Domaine_dis_base& zdb) override;

private:
  //sub_field[subdomain] = { the field }
  std::map<std::string, OWN_PTR(Champ_Don_base)> field_;
  std::map<std::string, DoubleTab> xp_ssz_;
  OBS_PTR(Probleme_base) pb_;
  IntTab default_vals_;
};

#endif /* Champ_Morceaux_included */
