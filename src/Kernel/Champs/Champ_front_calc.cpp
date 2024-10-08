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

#include <Raccord_distant_homogene.h>
#include <Frontiere_dis_base.h>
#include <Champ_front_calc.h>
#include <Probleme_Couple.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Interprete.h>
#include <Domaine.h>
#include <Domaine_VF.h>

Implemente_instanciable_sans_constructeur(Champ_front_calc,"Champ_front_calc",Ch_front_var_instationnaire_dep);
// XD champ_front_calc front_field_base champ_front_calc 0 This keyword is used on a boundary to get a field from another boundary. The local and remote boundaries should have the same mesh. If not, the Champ_front_recyclage keyword could be used instead. It is used in the condition block at the limits of equation which itself refers to a problem called pb1. We are working under the supposition that pb1 is coupled to another problem.
// XD attr problem_name ref_Pb_base problem_name 0 Name of the other problem to which pb1 is coupled.
// XD attr bord chaine bord 0 Name of the side which is the boundary between the 2 domains in the domain object description associated with the problem_name object.
// XD attr field_name chaine field_name 0 Name of the field containing the value that the user wishes to use at the boundary. The field_name object must be recognized by the problem_name object.


Champ_front_calc::Champ_front_calc() { set_distant(1); }

Sortie& Champ_front_calc::printOn(Sortie& os) const { return os; }

/*! @brief Lit le nom d'un probleme, le nom d'un bord et le nom d'un champ inconnue a partir d'un flot d'entree.
 *
 *     Cree ensuite le champ de frontiere correspondant.
 *     Format:
 *       Champ_front_calc nom_probleme nom_bord nom_champ
 *
 * @param (Entree& is) un flot d'entre
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Champ_front_calc::readOn(Entree& is)
{
  is >> nom_autre_pb_ >> nom_autre_bord_ >> nom_inco_;
  via_readon_ = true;
  (nom_inco_ == "VITESSE") ? fixer_nb_comp(dimension) : fixer_nb_comp(1);
  return is;
}

/*! @brief Cree l'objet Champ_front_calc representant la trace d'un champ inconnue sur une frontiere a partir des noms:
 *
 *          - du probleme portant l'inconnue
 *          - du bord concerne (la frontiere)
 *          - de l'inconnue
 *
 * @param (Nom& nom_pb) le nom du probleme auquel appartent l'inconnue dont on veut prendre la trace
 * @param (Nom& nom) le nom de la frontiere sur laquelle on prend la trace de l'inconnue
 * @param (Motcle& nom_inco) le nom de l'inconnue dont on veut prendre la trace
 * @throws pas de probleme du nom specifie
 * @throws le probleme n'a pas de champ du nom specifie
 */
void Champ_front_calc::creer(const Nom& nom_pb, const Nom& nom_bord, const Motcle& nom_inco)
{
  nom_autre_pb_ = nom_pb;
  nom_autre_bord_ = nom_bord;
  OBS_PTR(Probleme_base) autre_pb;
  Objet_U& ob = Interprete::objet(nom_autre_pb_);
  if (sub_type(Probleme_base, ob))
    {
      autre_pb = ref_cast(Probleme_base, ob);
    }
  else
    {
      Cerr << "We did not find problem with name " << nom_pb << finl;
      exit();
    }
  OBS_PTR(Champ_base) rch;
  rch = autre_pb->get_champ(nom_inco);
  if (sub_type(Champ_Inc_base, rch.valeur()))
    {
      l_inconnue = ref_cast(Champ_Inc_base, rch.valeur());
      fixer_nb_comp(rch->nb_comp());
    }
  else
    {
      Cerr << autre_pb->le_nom() << " did not have unknown field with name " << nom_inco << finl;
      exit();
    }
}

int Champ_front_calc::initialiser(double temps, const Champ_Inc_base& inco)
{
  // 1er chose a faire : la c'est bon ! on creer le champ
  if (via_readon_) creer(nom_autre_pb_, nom_autre_bord_, nom_inco_);

  Ch_front_var_instationnaire_dep::initialiser(temps, inco);

  // Check/initialize Raccord boundaries in parallel:
  if (nproc() > 1)
    {
      const Domaine_dis_base& domaine_dis_opposee = front_dis().domaine_dis();
      const Domaine_dis_base& domaine_dis_locale = frontiere_dis().domaine_dis();
      const Frontiere& frontiere_opposee = front_dis().frontiere();
      const Frontiere& frontiere_locale = frontiere_dis().frontiere();
      if (distant_ && !sub_type(Raccord_distant_homogene, frontiere_opposee))
        {
          const Nom& nom_domaine_oppose = domaine_dis_opposee.domaine().le_nom();
          Cerr << "Error, the boundary " << frontiere_opposee.le_nom() << " should be a Raccord." << finl;
          Cerr << "Add in your data file between the definition and the partition of the domain " << nom_domaine_oppose << " :" << finl;
          Cerr << "Modif_bord_to_raccord " << nom_domaine_oppose << " " << frontiere_opposee.le_nom() << finl;
          exit();
        }
      if (distant_ == 1)
        {
          Raccord_distant_homogene& raccord_distant = ref_cast_non_const(Raccord_distant_homogene, frontiere_opposee);
          if (!raccord_distant.est_initialise())
            raccord_distant.initialise(frontiere_locale, domaine_dis_locale, domaine_dis_opposee);
        }
    }
  return 1;
}

/*! @brief Associe le champ inconnue a l'objet
 *
 * @param (Champ_Inc_base& inco) le champ inconnue dont on prendra la trace
 */
void Champ_front_calc::associer_ch_inc_base(const Champ_Inc_base& inco)
{
  l_inconnue = inco;
}


/*! @brief Non code
 *
 * @param (Champ_front_base& ch)
 * @return (Champ_front_base&)
 */
Champ_front_base& Champ_front_calc::affecter_(const Champ_front_base& ch)
{
  return *this;
}

/*! @brief Mise a jour en temps du champ On prend juste la trace du champ inconnue au pas de
 *
 *     temps auquel il se situe.
 *     WEC : verifier qu'on prend l'inconnue au bon temps !
 *
 */

// Precondition: le nom du bord doit etre different de "??"
// Parametre: double
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_front_calc::mettre_a_jour(double temps)
{
  assert (nom_autre_bord_ != "??") ;
  DoubleTab& tab=valeurs_au_temps(temps);
  const Frontiere_dis_base& frontiere_dis_opposee = domaine_dis().frontiere_dis(nom_bord_oppose());
  l_inconnue->trace(frontiere_dis_opposee,tab,temps,distant_ /* distant */);
}

/*! @brief Renvoie le champ inconnue associe
 *
 * @return (Champ_Inc_base&) le champ inconnue associe
 */
const Champ_Inc_base& Champ_front_calc::inconnue() const
{
  return l_inconnue.valeur();
}

/*! @brief Renvoie le nom du bord sur lequel on calcule la trace.
 *
 * @return (Nom&) le nom du bord sur lequel on calcule la trace
 */
const Nom& Champ_front_calc::nom_bord_oppose() const
{
  return nom_autre_bord_;
}

/*! @brief Renvoie l'equation associee a l'inconnue dont on prend la trace.
 *
 * @return (Equation_base&) l'equation associee a l'inconnue dont on prend la trace
 */
const Equation_base& Champ_front_calc::equation() const
{
  if (l_inconnue.non_nul()==0)
    {
      Cerr << "\nError in Champ_front_calc::equation() : not able to return the equation !" << finl;
      Process::exit();
    }
  return inconnue().equation();
}

/*! @brief Renvoie le milieu associe a l'equation qui porte le champ inconnue dont on prend la trace.
 *
 * @return (Milieu_base&) le milieu associe a l'equation qui porte le champ inconnue dont on prend la trace
 */
const Milieu_base& Champ_front_calc::milieu() const
{
  return equation().milieu();
}

/*! @brief Renvoie le domaine discretisee associee a l'equation qui porte le champ inconnue dont on prend la trace.
 *
 * @return (Domaine_dis_base&) le domaine discretisee associee a l'equation qui porte le champ inconnue dont on prend la trace
 */
const Domaine_dis_base& Champ_front_calc::domaine_dis() const
{
  return inconnue().domaine_dis_base();
}

/*! @brief Renvoie le domaine des conditions au limites discretisees portee par l'equation qui porte le champ inconnue
 *
 *     dont on prend la trace
 *
 * @return (Domaine_Cl_dis_base&) le domaine des conditions au limites discretisees portee par l'equation qui porte le champ inconnue dont on prend la trace
 */
const Domaine_Cl_dis_base& Champ_front_calc::domaine_Cl_dis() const
{
  return equation().domaine_Cl_dis();
}

/*! @brief Renvoie la frontiere discretisee correspondante au domaine sur lequel prend la trace.
 *
 * @return (Frontiere_dis_base&) frontiere discretisee correspondante au domaine sur lequel prend la trace
 * @throws frontiere du nom specifie introuvable
 */
const Frontiere_dis_base& Champ_front_calc::front_dis() const
{
  return domaine_dis().frontiere_dis(nom_autre_bord_);
}


void Champ_front_calc::verifier(const Cond_lim_base& la_cl) const
{
}
