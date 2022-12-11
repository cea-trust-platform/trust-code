/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Parser_Eval_included
#define Parser_Eval_included

#include <TRUSTTabs_forward.h>
#include <Vect_Parser_U.h>
#include <TRUSTTabs.h>

/*! @brief Classe Parser_Eval Cette classe a pour fonction d evaleur les valeurs prises par une fonction analytique.
 *
 *     La fonction peut dependre de l espace (x, y, z), du temps (t) et d'un champ parametre (val).
 *
 */
class Parser_Eval
{
public :
  inline VECT(Parser_U)& fonction() { return fonction_;  }
  Parser_U& parser(int i) { return fonction_[i]; }
  Parser_U& parser(int i) const { return fonction_[i]; }

  // multiple points evaluation - x, y, z
  void eval_fct(const DoubleTab& positions, DoubleTab& val) const { eval_fct_(positions, nullptr, nullptr, val); }
  // multiple points evaluation - x, y, z, t
  void eval_fct(const DoubleTab& positions, const double t, DoubleTab& val) const { eval_fct_(positions, &t, nullptr, val); }
  // multiple points evaluation - x, y, z, t, field
  void eval_fct(const DoubleTab& positions, const double t, const DoubleTab& val_param, DoubleTab& val) const { eval_fct_(positions, &t, &val_param, val); }
  // multiple points evaluation, single component - x, y, z
  void eval_fct(const DoubleTab& positions, DoubleVect& val, const int ncomp) const { eval_fct_single_compo(positions, nullptr, val, ncomp); }
  // multiple points evaluation, single component - x, y, z, t
  void eval_fct(const DoubleTab& positions, const double t, DoubleVect& val, const int ncomp) const { eval_fct_single_compo(positions, &t, val, ncomp); }
  // single point evaluation - x, y, z
  void eval_fct(const DoubleVect& position, DoubleVect& val) const { eval_fct_single_position(position, nullptr, nullptr, val); }
  // single point evaluation - x, y, z, t
  void eval_fct(const DoubleVect& position, const double t, DoubleVect& val) const { eval_fct_single_position(position, &t, nullptr, val); }

  // Fonction generale qui depend de plusieurs champs inconnus
  void eval_fct(const DoubleTabs& variables, DoubleTab& val) const;

protected:
  mutable VECT(Parser_U) fonction_;

private:
  double eval_fct_single_position_single_compo(const DoubleVect& position, const double* t, const double* val_param, const int ncomp) const;
  void eval_fct_single_position(const DoubleVect& position, const double* t, const double* val_param, DoubleVect& val) const;
  void eval_fct_single_compo(const DoubleTab& positions, const double* t, DoubleVect& val, const int ncomp) const;
  void eval_fct_(const DoubleTab& positions, const double* t, const DoubleTab *val_param, DoubleTab& val) const;
};

#endif /* Parser_Eval_included */
