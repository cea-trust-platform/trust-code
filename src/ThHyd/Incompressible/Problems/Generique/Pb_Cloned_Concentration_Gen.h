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

#ifndef Pb_Cloned_Concentration_Gen_included
#define Pb_Cloned_Concentration_Gen_included

#include <Convection_Diffusion_Concentration.h>
#include <Pb_Thermohydraulique.h>

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_ = Convection_Diffusion_Concentration, typename _MEDIUM_TYPE_ = Constituant>
class Pb_Cloned_Concentration_Gen : public _DERIVED_TYPE_
{
protected:
  LIST(_EQUATION_TYPE_) list_eq_concentration_;
  std::vector<_MEDIUM_TYPE_> mil_constituants_;
  int nb_consts_ = -123;

  unsigned taille_memoire() const override { return sizeof(Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_ >); }

  int duplique() const override
  {
    Pb_Cloned_Concentration_Gen *xxx = new Pb_Cloned_Concentration_Gen(*this);
    if (!xxx) Process::exit("Not enough memory !!!");
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override { return _DERIVED_TYPE_::printOn(os); }

  Entree& readOn(Entree& is) override;

  void clone_equations();

  void rename_equation_unknown(const int);

  inline int nb_equations_multi() { return list_eq_concentration_.size(); }
  inline int nb_equations_multi() const { return list_eq_concentration_.size(); }

  Entree& lire_equations(Entree& , Motcle& ) override;

public:
  int nombre_d_equations() const override
  {
    return (_DERIVED_TYPE_::nombre_d_equations() + nb_equations_multi());
  }

  const Equation_base& equation(int) const override;
  Equation_base& equation(int) override;

  void associer_milieu_base(const Milieu_base&) override;
  int verifier() override;

  void typer_lire_milieu(Entree& ) override;
};

#include <Pb_Cloned_Concentration_Gen.tpp>

#endif /* Pb_Cloned_Concentration_Gen_included */
