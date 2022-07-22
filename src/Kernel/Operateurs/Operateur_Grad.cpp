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

#include <Operateur_Grad.h>
#include <Probleme_base.h>
#include <Discretisation_base.h>
#include <stat_counters.h>

Implemente_deriv(Operateur_Grad_base);
Implemente_instanciable(Operateur_Grad,"Operateur_Grad",DERIV(Operateur_Grad_base));


/*! @brief Simple appel a Operateur::ecrire(Sortie&) Ecrit l'operateur sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Operateur_Grad::printOn(Sortie& os) const
{
  return Operateur::ecrire(os);
}


/*! @brief Simple appel a Operateur::lire(Entree&) Lit l'operateur a partir d'un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Operateur_Grad::readOn(Entree& is)
{
  return Operateur::lire(is);
}


/*! @brief Type l'operateur: se type "Op_Grad_"+discretisation()+
 *
 *     "_"+inconnue().suffix
 *
 */
void Operateur_Grad::typer()
{
  Equation_base& eqn=equation();
  Nom inut;
  Nom nom_type=eqn.discretisation().get_name_of_type_for(que_suis_je(),inut,eqn);
  DERIV(Operateur_Grad_base)::typer(nom_type);
  Cerr << "Construction of the gradient operator type : ";
  Cerr << valeur().que_suis_je() << finl ;
}

void Operateur_Grad::typer_direct(const Nom& un_type)
{
  DERIV(Operateur_Grad_base)::typer(un_type);
}

/*! @brief Ajoute la contribution de l'operateur au tableau passe en parametre
 *
 * @param (DoubleTab& donnee) tableau contenant les donnees sur lesquelles on applique l'operateur.
 * @param (DoubleTab& resu) tableau auquel on ajoute la contribution de l'operateur
 * @return (DoubleTab&) le tableau contenant le resultat
 */
DoubleTab& Operateur_Grad::ajouter(const DoubleTab& donnee,
                                   DoubleTab& resu) const
{
  statistiques().begin_count(gradient_counter_);
  DoubleTab& tmp = valeur().ajouter(donnee, resu);
  statistiques().end_count(gradient_counter_);
  return tmp;
}


/*! @brief Initialise le tableau passe en parametre avec la contribution de l'operateur.
 *
 * @param (DoubleTab& donnee) tableau contenant les donnees sur lesquelles on applique l'operateur.
 * @param (DoubleTab& resu) tableau dans lequel stocke la contribution de l'operateur
 * @return (DoubleTab&) le tableau contenant le resultat
 */
DoubleTab& Operateur_Grad::calculer(const DoubleTab& donnee,
                                    DoubleTab& resu) const
{
  statistiques().begin_count(gradient_counter_);
  DoubleTab& tmp = valeur().calculer(donnee, resu);
  statistiques().end_count(gradient_counter_);
  return tmp;
}
