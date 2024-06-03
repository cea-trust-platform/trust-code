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

#ifndef Assembleur_included
#define Assembleur_included

/*! @brief classe Assembleur Classe generique de la hierarchie des Assembleurs de systemes.
 *
 * Un objet Assembleur peut
 *      referencer n'importe quel objet derivant de Assembleur_base.
 *
 * @sa Assembleur_base
 */

#include <TRUSTTabs_forward.h>
#include <Assembleur_base.h>
#include <TRUST_Deriv.h>

class Equation_base;
class Matrice;
class Domaine_dis_base;
class Domaine_Cl_dis_base;

class Assembleur : public DERIV(Assembleur_base)
{
  Declare_instanciable(Assembleur);

public :
  int assembler(Matrice&);
  int assembler_QC(const DoubleTab&, Matrice&);
  void completer(const Equation_base& );
  void associer_domaine_dis_base(const Domaine_dis_base&);
  void associer_domaine_cl_dis_base(const Domaine_Cl_dis_base&);
  const Domaine_dis_base& domaine_dis_base() const;
  const Domaine_Cl_dis_base& domaine_Cl_dis_base() const;
  int modifier_secmem(DoubleTab&);
  int modifier_solution(DoubleTab&);
};

#endif
