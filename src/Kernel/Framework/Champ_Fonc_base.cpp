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

#include <EcritureLectureSpecial.h>
#include <Champ_Fonc_base.h>
#include <MD_Vector_tools.h>
#include <Zone_VF.h>
#include <Zone.h>

Implemente_base(Champ_Fonc_base, "Champ_Fonc_base", Champ_Don_base);

Sortie& Champ_Fonc_base::printOn(Sortie& s) const { return s; }

Entree& Champ_Fonc_base::readOn(Entree& s) { return s; }

void Champ_Fonc_base::associer_domaine_dis_base(const Zone_dis_base& z_dis)
{
  le_dom_VF = ref_cast(Zone_VF, z_dis);
}

const Zone_dis_base& Champ_Fonc_base::zone_dis_base() const
{
  return le_dom_VF.non_nul() ? le_dom_VF.valeur() : Champ_Don_base::zone_dis_base() /* throw */;
}

const Zone_VF& Champ_Fonc_base::zone_vf() const
{
  assert (le_dom_VF.non_nul());
  return le_dom_VF.valeur();
}

/*! @brief Mise a jour en temps du champ.
 *
 * @param (double temps) le temps de mise a jour
 */
void Champ_Fonc_base::mettre_a_jour(double un_temps)
{
  changer_temps(un_temps);
  valeurs().echange_espace_virtuel();
}

int Champ_Fonc_base::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  Cerr << "Error in Champ_Fonc_base::fixer_nb_valeurs_nodales: method not reimplemented for the class " << que_suis_je() << finl;
  Process::exit();
  return nb_noeuds;
}

void Champ_Fonc_base::creer_tableau_distribue(const MD_Vector& md, Array_base::Resize_Options opt)
{
  // Note B.M.: pour etre symetrique avec Champ_Inc_base, il faudrait tester si le
  // champ est scalaire ou multi-scalaire (voir Champ_Inc_base::creer_tableau_distribue())
  if (valeurs_.size_array() == 0 && (!valeurs_.get_md_vector().non_nul()))
    {
      // Note B.M.: les methodes fixer_nb_valeurs_nodales sont appelees a tort et a travers.
      // Ne rien faire si le tableau a deja la bonne structure
      valeurs_.resize(0, nb_compo_);
    }
  // Ca va planter si on a attache une autre structure parallele (c'est voulu !)
  if (!(valeurs_.get_md_vector() == md))
    {
      if (valeurs_.get_md_vector().non_nul())
        {
          Cerr << "Internal error in Champ_Fonc_base::creer_tableau_distribue:\n" << " array has already a (wrong) parallel descriptor" << finl;
          Process::exit();
        }
      MD_Vector_tools::creer_tableau_distribue(md, valeurs_, opt);
    }
}

/*! @brief Sauvegarde le champ sur un flot de sortie Ecrit le nom, le temps et les valeurs.
 *
 * @param (Sortie& fich) un flot de sortie
 * @return (int) renvoie toujours 1
 */
int Champ_Fonc_base::sauvegarder(Sortie& fich) const
{
  // en mode ecriture special seul le maitre ecrit l'entete
  int a_faire, special;
  EcritureLectureSpecial::is_ecriture_special(special, a_faire);

  if (a_faire)
    {
      Nom mon_ident(nom_);
      mon_ident += que_suis_je();
      mon_ident += zone_dis_base().zone().le_nom();
      mon_ident += Nom(temps_, "%e");
      fich << mon_ident << finl;
      fich << que_suis_je() << finl;
      fich << temps_ << finl;
    }
  int bytes = 0;
  if (special)
    bytes = EcritureLectureSpecial::ecriture_special(*this, fich);
  else
    {
      bytes = 8 * valeurs().size_array();
      valeurs().ecrit(fich);
    }
  if (a_faire)
    {
      //fich << flush ; Ne marche pas en binaire, preferer:
      fich.flush();
    }
  if (Process::je_suis_maitre())
    Cerr << "Backup of the field " << nom_ << " performed on time : " << Nom(temps_, "%e") << finl;

  return bytes;
}

/*! @brief Reprise a partir d'un flot d'entree Lit le temps et les valeurs du champ.
 *
 *     Saute un bloc si le nom du champ est "anonyme".
 *
 * @param (Entree& fich) un flot d'entree
 * @return (int) renvoie toujours 1
 */
int Champ_Fonc_base::reprendre(Entree& fich)
{
  double un_temps;
  int special = EcritureLectureSpecial::is_lecture_special();
  if (nom_ != Nom("anonyme")) // lecture pour reprise
    {
      fich >> un_temps;
      if (special)
        EcritureLectureSpecial::lecture_special(*this, fich);
      else
        valeurs().lit(fich);
      Cerr << "Resume of the field " << nom_ << " performed " << finl;
    }
  else // lecture pour sauter le bloc
    {
      DoubleTab tempo;
      fich >> un_temps;
      tempo.jump(fich);
    }
  return 1;
}

/*! @brief Affecte un Champ_base dans un Champ_Fonc_base.
 *
 * @param (Champ_base& ch) le champ partie droite de l'affectation
 * @return (Champ_base&) le resultat de l'affectation (*this) (avec upcast)
 */
Champ_base& Champ_Fonc_base::affecter_(const Champ_base& ch)
{
  DoubleTab noeuds;
  remplir_coord_noeuds(noeuds);
  ch.valeur_aux(noeuds, valeurs());

  return *this;
}

/*! @brief Affecte une composante d'un Champ_base dans la meme composnate d'un Champ_Fonc_base.
 *
 * @param (Champ_base& ch) le champ partie droite de l'affectation
 * @param (int compo) l'indice de la composante a affecter
 * @return (Champ_base&) le resultat de l'affectation (*this) (avec upcast)
 */
Champ_base& Champ_Fonc_base::affecter_compo(const Champ_base& ch, int compo)
{
  DoubleTab noeuds;
  IntVect polys;
  if (!remplir_coord_noeuds_et_polys_compo(noeuds, polys, compo))
    {
      remplir_coord_noeuds_compo(noeuds, compo);
      ch.valeur_aux_compo(noeuds, valeurs(), compo);
    }
  else
    ch.valeur_aux_elems_compo(noeuds, polys, valeurs(), compo);
  return *this;
}

/*! @brief NE FAIT RIEN A surcharger dans les classes derivees
 *
 * @param (DoubleTab&)
 * @param (IntVect&)
 * @return (int) renvoie toujours 0
 */
int Champ_Fonc_base::remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const
{
  return 0;
}

/*! @brief NE FAIT RIEN A surcharger dans les classes derivees
 *
 * @param (DoubleTab& coord)
 * @return (DoubleTab&) renvoie toujours le parametre coord
 */
DoubleTab& Champ_Fonc_base::remplir_coord_noeuds(DoubleTab& coord) const
{
  return coord;
}

/*! @brief NE FAIT RIEN A surcharger dans les classes derivees
 *
 * @param (DoubleTab& coord)
 * @param (int)
 * @return (DoubleTab&) renvoie toujours le parametre coord
 */
DoubleTab& Champ_Fonc_base::remplir_coord_noeuds_compo(DoubleTab& coord, int) const
{
  return coord;
}

/*! @brief NE FAIT RIEN A surcharger dans les classes derivees
 *
 * @param (DoubleTab&)
 * @param (IntVect&)
 * @param (int)
 * @return (int) renvoie toujours 0
 */
int Champ_Fonc_base::remplir_coord_noeuds_et_polys_compo(DoubleTab&, IntVect&, int) const
{
  return 0;
}

const Zone& Champ_Fonc_base::domaine() const
{
  return zone_dis_base().zone();
}

DoubleTab& Champ_Fonc_base::valeur_aux(const DoubleTab& positions, DoubleTab& tab_valeurs) const
{
  const Zone& zone = zone_dis_base().zone();
  IntVect les_polys(positions.dimension(0));
  zone.chercher_elements(positions, les_polys);
  return valeur_aux_elems(positions, les_polys, tab_valeurs);
}
