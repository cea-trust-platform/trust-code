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

#include <Discretisation_base.h>
#include <Operateur_Diff.h>
#include <stat_counters.h>
#include <Champ_base.h>
#include <Champ_Don.h>

Implemente_instanciable(Operateur_Diff,"Operateur_Diff",DERIV(Operateur_Diff_base));

Sortie& Operateur_Diff::printOn(Sortie& os) const
{
  return Operateur::ecrire(os);
}

Entree& Operateur_Diff::readOn(Entree& is)
{
  return Operateur::lire(is);
}

/*! @brief Type l'operateur: se type "Op_Diff_"+discretisation() + ("_"ou"_Multi_inco_") + inconnue().suffix
 *     Associe la diffusivite a l'operateur base.
 *
 */
void Operateur_Diff::typer()
{
  Cerr << "Operateur_Diff::typer("<<typ<<")" << finl;
  if (Motcle(typ)==Motcle("negligeable"))
    {
      DERIV(Operateur_Diff_base)::typer("Op_Diff_negligeable");
      valeur().associer_diffusivite(diffusivite());
    }
  else
    {
      assert(la_diffusivite.non_nul());
      Equation_base& eqn=mon_equation.valeur();
      Nom nom_type= eqn.discretisation().get_name_of_type_for(que_suis_je(),typ,eqn,diffusivite());
      DERIV(Operateur_Diff_base)::typer(nom_type);
      valeur().associer_diffusivite(diffusivite());
    }
  Cerr << valeur().que_suis_je() << finl;
}

/*! @brief Renvoie l'objet sous-jacent upcaste en Operateur_base
 *
 * @return (Operateur_base&) l'objet sous-jacent upcaste en Operateur_base
 */
Operateur_base& Operateur_Diff::l_op_base()
{
  return valeur();
}
/*! @brief Renvoie l'objet sous-jacent upcaste en Operateur_base (version const)
 *
 * @return (Operateur_base&) l'objet sous-jacent upcaste en Operateur_base
 */
const Operateur_base& Operateur_Diff::l_op_base() const
{
  return valeur();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Ajoute la contribution de l'operateur au tableau
 *     passe en parametre
 *
 * @param (DoubleTab& donnee) tableau contenant les donnees sur lesquelles on applique l'operateur.
 * @param (DoubleTab& resu) tableau auquel on ajoute la contribution de l'operateur
 * @return (DoubleTab&) le tableau contenant le resultat
 */
DoubleTab& Operateur_Diff::ajouter(const DoubleTab& donnee,
                                   DoubleTab& resu) const
{
  statistiques().begin_count(diffusion_counter_);
  DoubleTab& tmp = valeur().ajouter(donnee, resu);
  statistiques().end_count(diffusion_counter_);
  return tmp;
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Initialise le tableau passe en parametre avec la contribution
 *     de l'operateur.
 *
 * @param (DoubleTab& donnee) tableau contenant les donnees sur lesquelles on applique l'operateur.
 * @param (DoubleTab& resu) tableau dans lequel stocke la contribution de l'operateur
 * @return (DoubleTab&) le tableau contenant le resultat
 */
DoubleTab& Operateur_Diff::calculer(const DoubleTab& donnee,
                                    DoubleTab& resu) const
{
  statistiques().begin_count(diffusion_counter_);
  DoubleTab& tmp = valeur().calculer(donnee, resu);
  statistiques().end_count(diffusion_counter_);
  return tmp;
}


/*! @brief Renvoie le champ representant la diffusivite.
 *
 * @return (Champ_Don&) le champ representant la diffusivite
 */
const Champ_base& Operateur_Diff::diffusivite() const
{
  return la_diffusivite.valeur();
}


/*! @brief Associe la diffusivite a l'operateur.
 *
 * @param (Champ_Don& nu) le champ representant la diffusivite
 * @return le champ representant la diffusivite
 */
void Operateur_Diff::associer_diffusivite(const Champ_base& nu)
{
  la_diffusivite=nu;
}

void  Operateur_Diff::associer_diffusivite_pour_pas_de_temps(const Champ_base& nu)
{
  valeur().associer_diffusivite_pour_pas_de_temps(nu);
}

/*! @brief Type l'operateur.
 *
 * @param (Nom& typ) le nom representant le type de l'operateur
 */
void Operateur_Diff::typer(const Nom& un_type)
{
  DERIV(Operateur_Diff_base)::typer(un_type);
}
