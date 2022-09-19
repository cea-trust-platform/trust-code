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

#include <Pb_Conduction.h>

Implemente_instanciable(Pb_Conduction,"Pb_Conduction",Probleme_base);
// XD Pb_Conduction Pb_base Pb_Conduction -1 Resolution of the heat equation.
// XD  attr solide solide solide 1 The medium associated with the problem.
// XD   attr Conduction Conduction Conduction 1 Heat equation.

/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
Sortie& Pb_Conduction::printOn(Sortie& s ) const
{
  return s;
}


/*! @brief Simple appel a: Probleme_base::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Pb_Conduction::readOn(Entree& is )
{
  return Probleme_base::readOn(is);
}


/*! @brief Renvoie le nombre d'equations du probleme.
 *
 * Toujours egal a 1 pour un probleme de conduction standart.
 *
 * @return (int) le nombre d'equations du probleme
 */
int Pb_Conduction::nombre_d_equations() const
{
  return 1;
}

/*! @brief Renvoie l'equation de type Conduction si i = 0,
 *
 *     Provoque une erreur sinon car le probleme
 *     n'a qu'une seule equation.
 *     (version const)
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation de type Conduction
 */
const Equation_base& Pb_Conduction::equation(int i) const
{
  assert (i==0);
  return eq_conduction;
}

/*! @brief Renvoie l'equation de type Conduction si i = 0,
 *
 *     Provoque une erreur sinon car le probleme
 *     n'a qu'une seule equation.
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation de type Conduction
 */
Equation_base& Pb_Conduction::equation(int i)
{
  assert (i==0);
  return eq_conduction;
}




