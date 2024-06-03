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

#ifndef Solv_Gen_included
#define Solv_Gen_included

#include <Precond.h>
#include <SolvElem.h>
#include <Matrice_Morse.h>
#include <solv_iteratif.h>
class Param;

#define _SEUIL_Gen_ 1e-12

class Solv_Gen : public solv_iteratif
{
  Declare_instanciable_sans_constructeur(Solv_Gen);
public :
  Solv_Gen();
  int resoudre_systeme(const Matrice_Base&, const DoubleVect&, DoubleVect&) override;
  inline int resoudre_systeme(const Matrice_Base& M, const DoubleVect& A, DoubleVect& B, int niter_max) override
  {
    return resoudre_systeme(M, A, B);
  }
  inline SolvElem& le_solveur_elem() { return le_solveur_elem_; }
  inline const SolvElem& le_solveur_elem() const { return le_solveur_elem_; }
  inline const Precond& get_precond() const { return le_precond_; }
  inline void set_precond(const Precond& pre ) { le_precond_ = pre; }
  void reinit() override;

protected :
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  int solve(const Matrice_Base& matrice, const Matrice_Base& mat_loc, const DoubleVect& secmem, DoubleVect& solution);

  Precond le_precond_;
  SolvElem le_solveur_elem_;
  int nb_it_max_, nb_it_max_flag, force_;
};

#endif /* Solv_Gen_included */
