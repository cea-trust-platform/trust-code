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

#include <Assembleur.h>

Implemente_instanciable(Assembleur,"Assembleur",DERIV(Assembleur_base));

Sortie& Assembleur::printOn(Sortie& os) const
{
  return DERIV(Assembleur_base)::printOn(os);
}

Entree& Assembleur::readOn(Entree& is)
{
  return DERIV(Assembleur_base)::readOn(is);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Permet de completer le systeme.
 *
 */
void Assembleur::completer(const Equation_base& eqn)
{
  valeur().completer(eqn);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Permet d'assembler le systeme.
 *
 * @return (int)
 */
int Assembleur::assembler(Matrice& mat)
{
  return valeur().assembler(mat);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Permet d'assembler le systeme.
 *
 * @return (int)
 */
int Assembleur::assembler_QC(const DoubleTab& rho, Matrice& mat)
{
  return valeur().assembler_QC(rho,mat);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Permet d'associer l'objet au domaine discretisee
 *
 */
void Assembleur::associer_domaine_dis_base(const Domaine_dis_base& zdis)
{
  valeur().associer_domaine_dis_base(zdis);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Permet d'associer l'objet a la Domaine_Cl_dis
 *
 */
void Assembleur::associer_domaine_cl_dis_base(const Domaine_Cl_dis_base& zcldis)
{
  valeur().associer_domaine_cl_dis_base(zcldis);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie le domaine discretisee associee a l'objet.
 *
 * @return (int)
 */
const Domaine_dis_base& Assembleur::domaine_dis_base() const
{
  return valeur().domaine_dis_base();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Renvoie la Domaine_Cl_dis associee a l'objet.
 *
 * @return (int)
 */
const Domaine_Cl_dis_base& Assembleur::domaine_Cl_dis_base() const
{
  return valeur().domaine_Cl_dis_base();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Permet de modifier le second membre du systeme
 *     Modifie egalement la solution (dans le cas du systeme en pression)
 *     en fonction des conditions aux limites
 *
 * @return (int)
 */
int Assembleur::modifier_secmem(DoubleTab& secmem)
{
  return valeur().modifier_secmem(secmem);
}
/*! @brief Appel a l'objet sous-jacent.
 *
 * Permet de modifier la pression
 *
 * @return (int)
 */
int Assembleur::modifier_solution(DoubleTab& sol)
{
  return valeur().modifier_solution(sol);
}

