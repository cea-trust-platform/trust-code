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

#include <Discretisation_base.h>
#include <Champ_Uniforme.h>
#include <Equation_base.h>
#include <stat_counters.h>
#include <Milieu_base.h>
#include <TRUSTTabs.h>
#include <Source.h>

Implemente_deriv(Source_base);
Implemente_instanciable(Source,"Source",DERIV(Source_base));


/*! @brief Impression de la source sur un flot de sortie.
 *
 * @param (Sortie& os) le flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Source::printOn(Sortie& os) const
{
  return DERIV(Source_base)::printOn(os);
}


/*! @brief Lecture de la source sur un flot d'entree.
 *
 * @param (Entree& is) le flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Source::readOn(Entree& is)
{
  return DERIV(Source_base)::readOn(is);
}


/*! @brief
 *
 * @param (Nom& typ) le nom de type a donner a la source
 */
void Source::typer_direct(const Nom& typ)
{
  // Cerr << "Source::typer_direct()" << finl;
  DERIV(Source_base)::typer(typ);
}


/*! @brief Type la source en calculant le nom du type necessaire grace aux parametres fournis.
 *
 * @param (Nom& typ) le debut du type
 * @param (Equation_base& eqn) l'equation associee a la source
 */
void Source::typer(const Nom& typ, const Equation_base& eqn)
{
  Nom type= eqn.discretisation().get_name_of_type_for(que_suis_je(),typ,eqn);

  if (eqn.que_suis_je() == "Transport_Marqueur_FT")
    type = typ;

  Cerr << type << finl;
  //  Cout << "Dans source.cpp type source = " << type << finl;
  DERIV(Source_base)::typer(type);
}


/*! @brief Appel a l'objet sous-jacent.
 *
 * Met a jour les references aux objets lies a l'objet Source.
 *
 */
void Source::completer()
{
  valeur().completer();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Met a jour les references aux objets lies a l'objet Source.
 *
 */
int Source::impr(Sortie& os) const
{
  return valeur().impr(os);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 */
int Source::initialiser(double temps)
{
  return valeur().initialiser(temps);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Ajoute la contribution de la source au tableau passe en parametre.
 *
 * @param (DoubleTab& xx) le tableau auquel on ajoute la contribution de la source
 * @return (DoubleTab&) le parametre xx modifie
 */
DoubleTab& Source::ajouter(DoubleTab& xx) const
{
  statistiques().begin_count(source_counter_);
  DoubleTab& tmp = valeur().ajouter(xx);
  statistiques().end_count(source_counter_);
  return tmp;
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Affecte le terme source au tableau passe en parametre.
 *
 * @param (DoubleTab& xx) le tableau dans lequel on stocke la valeur de la source
 * @return (DoubleTab&) le parametre xx modifie
 */
DoubleTab& Source::calculer(DoubleTab& xx) const
{
  statistiques().begin_count(source_counter_);
  DoubleTab& tmp = valeur().calculer(xx);
  statistiques().end_count(source_counter_);
  return tmp;
}
