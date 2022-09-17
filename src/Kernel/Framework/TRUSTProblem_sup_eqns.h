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

#ifndef TRUSTProblem_sup_eqns_included
#define TRUSTProblem_sup_eqns_included

#include <List_Equations_Scalaires_Passifs_Especes.h>

template <typename _DERIVED_TYPE_>
class TRUSTProblem_sup_eqns : public _DERIVED_TYPE_
{
protected:
  unsigned taille_memoire() const override { return sizeof(TRUSTProblem_sup_eqns<_DERIVED_TYPE_>); }

  int duplique() const override
  {
    TRUSTProblem_sup_eqns* xxx = new  TRUSTProblem_sup_eqns(*this);
    if(!xxx)
      {
        Cerr << "Not enough memory " << finl;
        Process::exit();
      }
    return xxx->numero();
  }

  Sortie& printOn(Sortie& os) const override { return _DERIVED_TYPE_::printOn(os); }

  Entree& readOn(Entree& is) override
  {
    is_lecture = 1;
    _DERIVED_TYPE_::readOn(is);
    is_lecture = 0;
    return is;
  }

public:

  TRUSTProblem_sup_eqns() : is_lecture(-1) { }
  ~TRUSTProblem_sup_eqns() { }

  int nombre_d_equations() const override
  {
    if (list_eq_supp.complete())
      return _DERIVED_TYPE_::nombre_d_equations() + list_eq_supp.nb_equation();
    else
      return _DERIVED_TYPE_::nombre_d_equations() + 1;
  }

  const Equation_base& equation(int i) const override
  {
    if (i < _DERIVED_TYPE_::nombre_d_equations()) return _DERIVED_TYPE_::equation(i);
    else
      {
        assert(is_lecture == 0);
        return list_eq_supp.equation(i - _DERIVED_TYPE_::nombre_d_equations());
      }
  }

  Equation_base& equation(int i) override
  {
    if (i < _DERIVED_TYPE_::nombre_d_equations()) return _DERIVED_TYPE_::equation(i);
    else if (list_eq_supp.complete() == 0)
      {
        is_lecture = 0;
        return list_eq_supp;
      }
    else if (is_lecture == -1) return list_eq_supp;
    else return list_eq_supp.equation(i - _DERIVED_TYPE_::nombre_d_equations());
  }

  void associer_milieu_base(const Milieu_base& mil) override
  {
    _DERIVED_TYPE_::associer_milieu_base(mil);
    list_eq_supp.associer_milieu_base(mil);
  }

protected:
  List_Equations_Scalaires_Passifs_Especes list_eq_supp;
  int is_lecture;
};

#endif /* TRUSTProblem_sup_eqns_included */
