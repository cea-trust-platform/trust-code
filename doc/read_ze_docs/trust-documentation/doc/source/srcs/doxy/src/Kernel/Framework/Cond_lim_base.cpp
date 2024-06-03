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
#include <Cond_lim_base.h>
#include <Equation_base.h>

Implemente_base(Cond_lim_base, "Cond_lim_base", Objet_U);

Sortie& Cond_lim_base::printOn(Sortie& s) const { return s << le_champ_front; }

Entree& Cond_lim_base::readOn(Entree& s) { return s >> le_champ_front; }

/*! @brief NE FAIT RIEN A surcharger dans les classes derivees
 *
 */
void Cond_lim_base::completer()
{
  champ_front()->completer();
}

int Cond_lim_base::compatible_avec_eqn(const Equation_base& eqn) const
{
  if (app_domains.size() == 0)
    {
      Cerr << "You call Cond_lim_base::compatible_avec_eqn but the std::vector app_domains is not filled ! Check your readOn !!" << finl;
      Process::exit();
    }

  Motcle dom_app = eqn.domaine_application();
  for (const auto &itr : app_domains)
    if (itr == dom_app) return 1;

  err_pas_compatible(eqn);
  return 0;
}

/*! @brief Change le i-eme temps futur de la CL.
 *
 */
void Cond_lim_base::changer_temps_futur(double temps, int i)
{
  champ_front()->changer_temps_futur(temps, i);
}

/*! @brief Tourne la roue de la CL
 *
 */
int Cond_lim_base::avancer(double temps)
{
  return champ_front()->avancer(temps);
}

/*! @brief Tourne la roue de la CL
 *
 */
int Cond_lim_base::reculer(double temps)
{
  return champ_front()->reculer(temps);
}

/*! @brief Initialisation en debut de calcul.
 *
 * A appeler avant tout calculer_coeffs_echange ou mettre_a_jour
 *     Contrairementaux methodes mettre_a_jour, les methodes
 *     initialiser des CLs ne peuvent pas dependre de l'exterieur
 *     (lui-meme peut ne pas etre initialise)
 *
 * @return (0 en cas d'erreur, 1 sinon.)
 */
int Cond_lim_base::initialiser(double temps)
{
  return le_champ_front->initialiser(temps, domaine_Cl_dis().inconnue());
}

/*! @brief Effectue une mise a jour en temps de la condition aux limites.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
void Cond_lim_base::mettre_a_jour(double temps)
{
  le_champ_front.mettre_a_jour(temps);
}

/*! @brief Cette methode indique si cette condition aux limites doit etre mise a jour dans des sous pas de temps d'un schema en temps tel que RK
 *
 *   Par defaut elle renvoie 0 pour indiquer qu'aucune mise a jour
 *   n'est necessaire ; il faut la surcharger pour renvoyer 1 au besoin
 *   (exemple Echange_impose_base)
 *
 * @param (double temps) le pas de temps de mise a jour
 */
int Cond_lim_base::a_mettre_a_jour_ss_pas_dt()
{
  return 0;
}

/*! @brief Calcul des coefficient d'echange pour un couplage par Champ_front_contact_VEF.
 *
 *     Ces calculs sont locaux au probleme et dependant uniquement de
 *     l'inconnue. Ils doivent donc etre faits chaque fois que
 *     l'inconnue est modifiee. Ils sont disponibles pour l'exterieur et
 *     stockes dans les CLs.
 *     WEC : Les Champ_front_contact_VEF devraient disparaitre et cette
 *     methode avec !!!
 *
 * @param (double temps) le pas de temps de mise a jour
 */
void Cond_lim_base::calculer_coeffs_echange(double temps)
{
  le_champ_front.calculer_coeffs_echange(temps);
}

/*! @brief Appel la verification du champ lu par l intermediaire de l equation pour laquelle on considere la condition limite
 *
 *  La methode est surchargee dans les cas ou l utilisateur doit
 *  specifier le champ frontiere
 *
 */
void Cond_lim_base::verifie_ch_init_nb_comp() const
{

}

/*! @brief Associe la frontiere a l'objet.
 *
 * L'objet Frontiere_dis_base est en fait associe au membre
 *     Champ_front de l'objet Cond_lim_base qui represente le champ des conditions
 *     aux limites imposees a la frontiere.
 *
 * @param (Frontiere_dis_base& fr) la frontiere sur laquelle on impose les conditions aux limites
 */
void Cond_lim_base::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  assert(le_champ_front.non_nul());
  le_champ_front.associer_fr_dis_base(fr);
  modifier_val_imp = 0;
}

/*! @brief Associe le Domaine_Cl_dis_base (Domaine des conditions aux limites discretisees) a l'objet.
 *
 * Ce Domaine_Cl_dis_base stocke (reference) toutes les conditions
 *     aux limites relative a un Domaine geometrique.
 *
 * @param (Domaine_Cl_dis_base& zcl) un Domaine des conditions aux limites discretisees a laquelle l'objet Cond_lim_base se rapporte
 */
void Cond_lim_base::associer_domaine_cl_dis_base(const Domaine_Cl_dis_base& zcl)
{
  mon_dom_cl_dis = zcl;
  le_champ_front.valeur().verifier(*this);
}

/*! @brief Renvoie 1 si la condition aux limites est compatible avec la discretisation passee en parametre.
 *
 * @param (Discretisation_base&) la discretisation avec laquelle on veut verifier la compatibilite
 */
int Cond_lim_base::compatible_avec_discr(const Discretisation_base& discr) const
{
  if (supp_discs.size() == 0) return 1;
  else
    {
      Nom type_discr = discr.que_suis_je();
      for (const auto &itr : supp_discs)
        if (itr == type_discr) return 1;

      err_pas_compatible(discr);
      return 0;
    }
}

/*! @brief Cette methode est appelee quand la condition aux limites n'est pas compatible avec l'equation sur laquelle on essaye
 *
 *     de l'appliquer.
 *
 * @param (Equation_base& eqn) l'equation avec laquelle la condition aux limites est incompatible
 */
void Cond_lim_base::err_pas_compatible(const Equation_base& eqn) const
{
  Cerr << "The boundary condition " << que_suis_je() << " can't apply to " << finl << "the equation of kind " << eqn.que_suis_je() << finl;
  exit();
}

/*! @brief Cette methode est appelee quand la condition aux limites n'est pas compatible avec la discretisation sur laquelle on essaye
 *
 *     de l'appliquer.
 *
 * @param (Discretisation_base& discr) la discretisation avec laquelle la condition aux limites est incompatible
 */
void Cond_lim_base::err_pas_compatible(const Discretisation_base& discr) const
{
  Cerr << "The boundary condition " << que_suis_je() << " can't be used with " << finl << "the discretization of kind " << discr.que_suis_je() << finl;
  exit();
}

void Cond_lim_base::champ_front(int face, DoubleVect& var) const
{
  le_champ_front->valeurs_face(face, var);
}

void Cond_lim_base::injecter_dans_champ_inc(const Champ_Inc&) const
{
  Cerr << "Cond_lim_base::injecter_dans_champ_inc()" << finl;
  Cerr << "this method does nothing and must be overloaded " << finl;
  Cerr << "Contact TRUST support." << finl;
  exit();
}
/*! @brief Change le i-eme temps futur de la cl.
 *
 */
void Cond_lim_base::set_temps_defaut(double temps)
{
  champ_front()->set_temps_defaut(temps);
}
/*! @brief Appele par Conds_lim::completer Appel cha_front_base::fixer_nb_valeurs_temporelles
 *
 */
void Cond_lim_base::fixer_nb_valeurs_temporelles(int nb_cases)
{
  champ_front()->fixer_nb_valeurs_temporelles(nb_cases);
}
