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

#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Operateur_base.h>
#include <Probleme_base.h>
#include <stat_counters.h>
#include <TRUSTTrav.h>
#include <Operateur.h>

Sortie& Operateur::ecrire(Sortie& os) const
{
  os << " { " << typ << " } " << finl;
  return os;
}


/*! @brief Lit un operateur sur un flot d'entree.
 *
 * Type l'operateur et lui associe son equation.
 *     Format:
 *       {
 *        [UN Motcle REPRESENTANT UN TYPE]
 *       }
 *
 * @param (Entree& is) le flot d'entree ou lire l'operateur
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 * @throws accolade fermante attendue
 */
Entree& Operateur::lire(Entree& is)
{
  typ="non defini";
  Motcle motlu;
  is >> motlu ;
  assert(motlu=="{");
  while(motlu!="}")
    {
      is>>motlu;
      if (motlu=="}")
        {
          if(typ=="non defini")
            {
              typ = "";
              typer();
              l_op_base().associer_eqn(equation());
              l_op_base().set_nb_ss_pas_de_temps(1);
              l_op_base().set_decal_temps(0);
            }
        }
      else if (motlu=="nb_sous_pas_de_temps")
        {
          if(typ=="non defini")
            {
              typ = "";
              typer();
              l_op_base().associer_eqn(equation());
            }
          int nb_ss_pas_de_temps;
          is >> nb_ss_pas_de_temps;
          l_op_base().set_nb_ss_pas_de_temps(nb_ss_pas_de_temps);
          l_op_base().set_decal_temps(0);
        }
      else if (motlu=="implicite")
        {
          // XD op_implicite objet_lecture nul 0 not_set
          // XD attr implicite chaine(into=["implicite"]) implicite 0 not_set
          // XD attr mot chaine(into=["solveur"]) mot 0 not_set
          // XD attr solveur solveur_sys_base solveur 0 not_set
          if(typ=="non defini")
            {
              typ = "";
              typer();
              l_op_base().associer_eqn(equation());
            }
          l_op_base().set_decal_temps(1);
          l_op_base().set_nb_ss_pas_de_temps(1);
          is >> motlu;
          if(motlu != "solveur")
            {
              Cerr << "We expected the keyword \"solveur\" instead of : "
                   << motlu << finl;;
              Process::exit();
            }
          l_op_base().lire_solveur(is);
        }
      else
        {
          if(typ!="non defini")
            {
              Cerr << "We must choose the type of operator in beginning "
                   <<"of reading block (before implicit in particular) "
                   << finl;
              Process::exit();
            }
          typ=motlu;
          typer();
          l_op_base().associer_eqn(equation());
          l_op_base().set_decal_temps(0);
          l_op_base().set_nb_ss_pas_de_temps(1);
          is >> l_op_base();
        }
    }
  return is;
}

/*! @brief Renvoie le champ representant l'inconnue de l'equation dont l'operateur fait partie.
 *
 * @return (Champ_Inc&) le champ inconuu de l'equation associee
 */
const Champ_Inc& Operateur::mon_inconnue() const
{
  return le_champ_inco.valeur();
}

/*! @brief Renvoie la discretisation de l'equation dont l'operateur fait partie.
 *
 * @return (Discretisation_base&) la discretisation de l'equation associee
 */
const Discretisation_base& Operateur::discretisation() const
{
  return mon_equation->discretisation();
}

/*! @brief Met a jour les references des objets associes a l'operateur.
 *
 * Operateur::le_champ_inco, Operateur::champ_inco
 *     Appelle Operateur_base::completer()
 *
 */
void Operateur::completer()
{
  if (!le_champ_inco.non_nul())
    le_champ_inco=mon_equation->inconnue();

  l_op_base().completer();
}

void Operateur::associer_champ(const Champ_Inc& ch, const std::string& nom_ch)
{
  le_champ_inco = ch;
  nom_inco_ = nom_ch;
  l_op_base().associer_champ(ch, nom_ch);
}

/*! @brief Effecttue une mise a jour en temps de l'operateur.
 *
 * Appelle Operateur_base::mettre_a_jour(double)
 *
 * @param (double temps) le pas de temps de mise a jour
 */
void Operateur::mettre_a_jour(double temps)
{
  l_op_base().mettre_a_jour(temps);
}
/*! @brief Calcule le prochain pas de temps.
 *
 */
double Operateur::calculer_pas_de_temps() const
{
  // Si l'equation de l'operateur n'est pas resolue, on ne calcule pas son pas de temps de stabilite
  if (equation().equation_non_resolue())
    return DMAXFLOAT;
  statistiques().begin_count(dt_counter_);
  double dt_stab = l_op_base().calculer_dt_stab();
  statistiques().end_count(dt_counter_);
  // Verification que l'operateur a bien un mp_min de fait:
  assert(dt_stab==Process::mp_min(dt_stab));
  return dt_stab;
}
/*! @brief Calculate the next local time steps
 *
 */
void Operateur::calculer_pas_de_temps_locaux(DoubleTab& dt_locaux) const
{
  l_op_base().calculer_dt_local(dt_locaux);
}
/*! @brief Demande a l'equation si une impression est necessaire Renvoie 1 pour OUI, 0 sinon.
 *
 * @return (int) 1 si une impression est necessaire 0 sinon
 */
int  Operateur::limpr() const
{
  return mon_equation->limpr();
}

/*! @brief Imprime l'operateur sur un flot de sortie, si c'est necessaire.
 *
 * (voir Schema_Temp_base::limpr())
 *
 * @param (Sortie& os) le flot de sortie pour l'impression
 */
void  Operateur::imprimer(Sortie& os) const
{
  if(limpr()) l_op_base().impr(os);
}


/*! @brief Imprime l'operateur sur un flot de sortie de facon inconditionnelle.
 *
 * @param (Sortie& os) le flot de sortie pour l'impression
 * @return (int) code de Operateur_base::impr(Sortie&)
 */
int Operateur::impr(Sortie& os) const
{
  if (l_op_base().has_impr_file()) return l_op_base().impr(os);
  return 1;
}

/*! @brief Calcule et ajoute la contribution de l'operateur au second membre de l'equation.
 *
 *     Appelle Operateur::ajouter(const DoubleTab&, DoubleTab& )
 *
 * @param (Champ_Inc& ch) le champ inconnu sur lequel l'operateur agit
 * @param[in,out] (DoubleTab& resu) le tableau stockant les valeurs du second membre auquel on ajoute la contribution de l'operateur
 * @return (DoubleTab&) le second membre auquel on a ajoute la contribution de l'operateur
 */
DoubleTab& Operateur::ajouter(const Champ_Inc& ch, DoubleTab& resu) const
{
  int i ;
  int nstep=l_op_base().get_nb_ss_pas_de_temps();
  double dt=equation().schema_temps().pas_de_temps();
  dt/=nstep;
  if(nstep==1)
    return ajouter(ch->valeurs(), resu);
  DoubleTrav derivee(resu);
  DoubleTrav inco(ch->valeurs());
  inco=ch->valeurs();
  const Solveur_Masse& solveur_masse=equation().solv_masse();
  double dt_inv=1./(double(nstep));
  for (i=0; i<nstep; i++)
    {
      calculer(inco, derivee);
      derivee.echange_espace_virtuel();
      resu.ajoute(dt_inv, derivee) ;
      solveur_masse->appliquer(derivee);
      inco.ajoute_sans_ech_esp_virt(dt, derivee, VECT_ALL_ITEMS) ;
    }
  return resu;
}

/*! @brief Renvoie le (nom du) type de l'operateur a creer.
 *
 * @return (Nom&) le nom du type de l'operateur a creer
 */
const Nom& Operateur::type() const
{
  return typ;
}


/*! @brief Calcule la contribution de l'operateur, et renvoie le tableau des valeurs.
 *
 * @param (Champ_Inc& ch) le champ inconnu sur lequel l'operateur agit
 * @param (DoubleTab& resu) le tableau stockant les valeurs resultant de l'application de l'operateur sur le champ inconnu.
 * @return (DoubleTab&) le resultat de l'application de l'operateur sur le champ inconnu
 */
DoubleTab& Operateur::calculer(const Champ_Inc& ch,DoubleTab& resu) const
{
  return calculer(ch->valeurs(), resu);
}

/*! @brief Ajoute la contribution de l'operateur au tableau passe en parametre.
 *
 *     Appelle Operateur::ajouter(const Champ_Inc&, DoubleTab& )
 *
 * @param (DoubleTab& resu) le tableau stockant les valeurs du second membre auquel on ajoute la contribution de l'operateur
 * @return (DoubleTab&) le second membre auquel on a ajoute la contribution de l'operateur
 */
DoubleTab& Operateur::ajouter(DoubleTab& resu) const
{
  return ajouter(le_champ_inco->valeur().valeurs(), resu);
}

/*! @brief Applique l'operateur au champ inconnu et renvoie le resultat.
 *
 * Appelle Operateur::calculer(const Champ_Inc&, DoubleTab& );
 *
 * @param (DoubleTab& resu) le tableau stockant les valeurs resultant de l'application de l'operateur sur le champ inconnu.
 * @return (DoubleTab&) le resultat de l'application de l'operateur sur le champ inconnu
 */
DoubleTab& Operateur::calculer(DoubleTab& resu) const
{
  resu=0.;
  return ajouter(le_champ_inco->valeur().valeurs(), resu);
}

void Operateur::set_fichier(const Nom& nom)
{
  l_op_base().set_fichier(nom);
}

void Operateur::set_description(const Nom& nom)
{
  l_op_base().set_description(nom);
}

void Operateur::ajouter_contribution_explicite_au_second_membre (const Champ_Inc_base& linconnue, DoubleTab& derivee) const
{
  l_op_base().ajouter_contribution_explicite_au_second_membre (linconnue, derivee);
}
