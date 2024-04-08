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

#ifndef TRUSTProblem_List_Concentration_Gen_included
#define TRUSTProblem_List_Concentration_Gen_included

#include <TRUSTProblem_Concentration_Gen.h>

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_ = Convection_Diffusion_Concentration, typename _MEDIUM_TYPE_ = Constituant>
class TRUSTProblem_List_Concentration_Gen : public TRUSTProblem_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>
{
protected:
  unsigned taille_memoire() const override { return sizeof(TRUSTProblem_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_ >); }

  int duplique() const override
  {
    TRUSTProblem_List_Concentration_Gen *xxx = new TRUSTProblem_List_Concentration_Gen(*this);
    if (!xxx) Process::exit("Not enough memory !!!");
    return xxx->numero();
  }

  Entree& readOn(Entree& is) override { return _DERIVED_TYPE_::readOn(is); }
  void add_concentration_equations();
  Entree& lire_equations(Entree& is, Motcle& dernier_mot) override;

public:
  void associer_milieu_base(const Milieu_base&) override;
  int verifier() override;
  void typer_lire_milieu(Entree& ) override;
};

#include <TRUSTProblem_List_Concentration_Gen.tpp>

#endif /* TRUSTProblem_List_Concentration_Gen_included */
