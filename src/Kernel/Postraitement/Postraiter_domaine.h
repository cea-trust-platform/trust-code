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

#ifndef Postraiter_domaine_included
#define Postraiter_domaine_included

#include <Interprete_geometrique_base.h>

#include <Domaine_forward.h>
class Nom;

/*! @brief Postraiter_domaine allows to write one or more domains in a file with a specified format (MED,LML,LATA,SINGLE_LATA,CGNS).
 *
 * @sa Interprete_geometrique_base
 */
class Postraiter_domaine: public Interprete_geometrique_base
{
  Declare_instanciable(Postraiter_domaine);
public:
  Entree& interpreter_(Entree&) override;
  void ecrire(Nom&);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;

protected:
  Nom commande_, format_post_;
  int nb_domaine_ = 0, joint_non_ecrit_ = 1, format_binaire_ = 1, ecrire_frontiere_ = 1, dual_ = 0;
};

#endif /* Postraiter_domaine_included */
