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

#ifndef Correlation_base_included
#define Correlation_base_included

#include <Champs_compris_interface.h>
#include <Champs_compris.h>
#include <TRUST_Deriv.h>
#include <TRUST_Ref.h>
#include <Param.h>

class Probleme_base;

class Correlation_base : public Objet_U, public Champs_compris_interface
{
  Declare_base(Correlation_base);
public:
  virtual void mettre_a_jour(double temps) { }
  virtual void completer() { }
  void associer_pb(const Probleme_base&);

  static void typer_lire_correlation(OWN_PTR(Correlation_base)&, const Probleme_base&, const Nom&, Entree&);

  //Methodes de l interface des champs postraitables
  void creer_champ(const Motcle& motlu) override { }
  const Champ_base& get_champ(const Motcle& nom) const override { throw std::runtime_error("Field not found !"); }
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override { }
  bool has_champ(const Motcle& nom, OBS_PTR(Champ_base) &ref_champ) const override;
  bool has_champ(const Motcle& nom) const override;

protected:
  OBS_PTR(Probleme_base) pb_;
  Champs_compris champs_compris_;
};

#endif /* Correlation_base_included */
