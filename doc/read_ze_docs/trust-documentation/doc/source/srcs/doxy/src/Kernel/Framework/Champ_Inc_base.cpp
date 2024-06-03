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
#include <Scalaire_impose_paroi.h>
#include <Schema_Temps_base.h>
#include <Champ_Inc_P0_base.h>
#include <Champ_Inc_P1_base.h>
#include <Neumann_val_ext.h>
#include <MD_Vector_tools.h>
#include <Champ_Inc_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Dirichlet.h>
#include <Domaine.h>
#include <Domaine_VF.h>

Implemente_base_sans_constructeur(Champ_Inc_base,"Champ_Inc_base",Champ_base);

Sortie& Champ_Inc_base::printOn(Sortie& os) const { return Champ_base::printOn(os); }
Entree& Champ_Inc_base::readOn(Entree& is) { return Champ_base::readOn(is); }

/*! @brief Fixe le nombre de valeurs temporelles a conserver.
 *
 * (un nombre different suivant le schema en temps utilise)
 *     Appelle Roue::fixer_nb_cases(int)
 *
 * @param (int i) le nombre de valeurs temporelles a conserver
 * @return (int) le nombre de valeurs temporelles a conserver
 */
int Champ_Inc_base::fixer_nb_valeurs_temporelles(int i)
{
  return les_valeurs->fixer_nb_cases(i);
}

/*! @brief Renvoie le nombre de valeurs temporelles actuellement conservees.
 *
 * Cette valeur est stockee par la Roue du Champ_Inc_base
 *
 * @return (int) le nombre de valeurs temporelles actuellement conservees
 */
int Champ_Inc_base::nb_valeurs_temporelles() const
{
  return les_valeurs->nb_cases();
}

/*! @brief Lit les valeurs du champs a partir d'un flot d'entree.
 *
 * Format de lecture:
 *       int [LE NOMBRE DE VALEURS A LIRE]
 *       [LIRE LE NOMBRE DE VALEUR VOULUES]
 *
 * @param (Entree& is) le flot d'entree
 * @return (int) renvoie 1 si la lecture est correcte
 * @throws le nombre de valeur a lire est incorrect
 */
int Champ_Inc_base::lire_donnees(Entree& is)
{
  int n;
  is >> n;
  if (n != les_valeurs->valeurs().size())
    {
      Cerr << " the file does not contain the correct number of values to fill the field" << finl;
      Process::exit();
    }
  DoubleVect& tab = les_valeurs->valeurs();
  for (int i = 0; i < n; i++)
    is >> tab[i];
  return 1;
}

int Champ_Inc_base::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  Cerr << "Internal error in Champ_Inc_base::fixer_nb_valeurs_nodales: method has not been implemented for class " << que_suis_je() << finl;
  Process::exit();
  return nb_noeuds;
}

void Champ_Inc_base::creer_tableau_distribue(const MD_Vector& md, Array_base::Resize_Options opt)
{
  const int n = nb_valeurs_temporelles();
  for (int i = 0; i < n; i++)
    {
      DoubleTab& tab = futur(i);
      // Note B.M: Ce test n'est pas symetrique avec Champ_Fonc_base => incoherence de nb_dim
      // pour les champs "multiscalaires" a une composante.
      if (tab.size_array() == 0 && (!tab.get_md_vector().non_nul()))
        {
          // Note B.M.: les methodes fixer_nb_valeurs_nodales sont appelees a tort et a travers.
          // Ne rien faire si le tableau a deja la bonne structure
          tab.resize(0, nb_compo_);
        }
      if (!(tab.get_md_vector() == md))
        {
          if (tab.get_md_vector().non_nul())
            {
              Cerr << "Internal error in Champ_Inc_base::creer_tableau_distribue:\n" << " array has alreary a (wrong) parallel descriptor" << finl;
              Process::exit();
            }
          MD_Vector_tools::creer_tableau_distribue(md, tab, opt);
        }
    }
}

/*! @brief Returns the number of "real" geometric positions of the degrees of freedom, or -1 if not applicable (fields with multiple localisations)
 *
 */
int Champ_Inc_base::nb_valeurs_nodales() const
{
  int n;
  const DoubleTab& v = valeurs();
  if (v.size_reelle_ok())
    n = v.dimension(0);
  else
    n = -1;
  return n;
}

/*! @brief Renvoie le tableau des valeurs du champ au temps courant.
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
DoubleTab& Champ_Inc_base::valeurs()
{
  return les_valeurs->valeurs();
}

/*! @brief Renvoie le tableau des valeurs du champ au temp courant (version const)
 *
 * @return (DoubleTab&) le tableau des valeurs du champ
 */
const DoubleTab& Champ_Inc_base::valeurs() const
{
  return les_valeurs->valeurs();
}

/*! @brief Renvoie les valeurs du champs a l'instant temps.
 *
 * @param (double temps) le  temps  auquel on veut les valeurs du champ
 * @return (DoubleTab&) les valeurs du champs a l'instant temps
 */
// WEC : Attention dans le cas de Pb_Couple on utilisait le fait que cette fonction renvoyait le present quand elle ne trouvait
// pas un temps superieur a tous les temps disponibles!!!
// Le comportement est maintenant plus explicite : un WARNING est affiche des que le present est renvoye a la place du temps demande.
DoubleTab& Champ_Inc_base::valeurs(double tps)
{
  if (temps() == tps)
    return valeurs();
  else
    {
      Roue& la_roue = les_valeurs.valeur();
      if (temps() < tps)
        {
          for (int i = 0; i < nb_valeurs_temporelles(); i++)
            {
              if (la_roue.futur(i).temps() == tps)
                return la_roue.futur(i).valeurs();
              else if (la_roue.futur(i).temps() < temps())
                break;
            }
        }
      else if (temps() > tps)
        {
          for (int i = 0; i < nb_valeurs_temporelles(); i++)
            {
              if (la_roue.passe(i).temps() == tps)
                return la_roue.passe(i).valeurs();
              else if (la_roue.passe(i).temps() > temps())
                break;
            }
        }
    }
  Cerr << "ERROR : in Champ_Inc_base::valeurs(double), time " << tps << " not found, returns the present?" << finl;
  Cerr << "Contact TRUST support." << finl;
  Process::exit();
  return valeurs();
}

/*! @brief Renvoie les valeurs du champs a l'instant temps.
 *
 * @param (double temps) le  temps  auquel on veut les valeurs du champ
 * @return (DoubleTab&) les valeurs du champs a l'instant temps
 */
const DoubleTab& Champ_Inc_base::valeurs(double tps) const
// See above !
{
  if (temps() == tps)
    return valeurs();
  else
    {
      const Roue& la_roue = les_valeurs.valeur();

      if (temps() < tps)
        {
          // Futur ?
          for (int i = 0; i < nb_valeurs_temporelles(); i++)
            {
              if (la_roue.futur(i).temps() == tps)
                return la_roue.futur(i).valeurs();
              else if (la_roue.futur(i).temps() < temps())
                break;
            }
        }
      else if (temps() > tps)
        {
          // Passe ?
          for (int i = 1; i < nb_valeurs_temporelles(); i++)
            {
              if (la_roue.passe(i).temps() == tps)
                return la_roue.passe(i).valeurs();
              else if (la_roue.passe(i).temps() > temps())
                break;
            }
        }
    }
  Cerr << "ERROR : in Champ_Inc_base::valeurs(double), time " << tps << " not found, returns the present?" << finl;
  Cerr << "Contact TRUST support." << finl;
  Process::exit();
  return valeurs();
}
/*! @brief Renvoie les valeurs du champs a l'instant t+i.
 *
 * @param (int i) le pas de temps futur auquel on veut les valeurs du champ
 * @return (DoubleTab&) les valeurs du champs a l'instant t+i
 */
DoubleTab& Champ_Inc_base::futur(int i)
{
  return les_valeurs->futur(i).valeurs();
}

/*! @brief Renvoie les valeurs du champs a l'instant t+i.
 *
 * (version const)
 *
 * @param (int i) le pas de temps futur auquel on veut les valeurs du champ
 * @return (DoubleTab&) les valeurs du champs a l'instant t+i
 */
const DoubleTab& Champ_Inc_base::futur(int i) const
{
  return les_valeurs->futur(i).valeurs();
}

/*! @brief Renvoie les valeurs du champs a l'instant t-i.
 *
 * @param (int i) le pas de temps passe auquel on veut les valeurs du champ
 * @return (DoubleTab&) les valeurs du champs a l'instant t-i
 */
DoubleTab& Champ_Inc_base::passe(int i)
{
  return les_valeurs->passe(i).valeurs();
}

/*! @brief Renvoie les valeurs du champs a l'instant t-i.
 *
 * (version const)
 *
 * @param (int i) le pas de temps passe auquel on veut les valeurs du champ
 * @return (DoubleTab&) les valeurs du champs a l'instant t-i
 */
const DoubleTab& Champ_Inc_base::passe(int i) const
{
  return les_valeurs->passe(i).valeurs();
}

/*! @brief Avance le pointeur courant de i pas de temps, dans la liste des valeurs temporelles conservees.
 *
 * @param (int i) le nombre de pas de temps dont on avance
 * @return (Champ_Inc_base&) renvoie *this, le champ au pas de temps voulu
 */
Champ_Inc_base& Champ_Inc_base::avancer(int i)
{
  while (i--)
    les_valeurs->avancer(les_valeurs);
  temps_ = les_valeurs->temps();
  return *this;
}

/*! @brief Recule le pointeur courant de i pas de temps, dans la liste des valeurs temporelles conservees.
 *
 * @param (int i) le nombre de pas de temps dont on recule
 * @return (Champ_Inc_base&) renvoie *this, le champ au pas de temps voulu
 */
Champ_Inc_base& Champ_Inc_base::reculer(int i)
{
  while (i--)
    les_valeurs->reculer(les_valeurs);
  temps_ = les_valeurs->temps();
  return *this;
}

/*! @brief Effectue une mise a jour en temps du champ inconnue.
 *
 * WEC : Maintenant si on l'appelle 2 fois de suite avec le meme
 *     argument, la 2eme ne fait rien.
 *
 * @param (double temps) le nouveau temps
 */
void Champ_Inc_base::mettre_a_jour(double un_temps)
{
  // Champ a plusieurs valeurs temporelle :
  // On avance a la bonne valeur temporelle.
  if (les_valeurs->nb_cases() > 1)
    {
      for (int i = 0; i < les_valeurs->nb_cases(); i++)
        {
          if (les_valeurs[i].temps() == un_temps)
            {
              avancer(i);
              temps_ = un_temps;
              //Inutile:
              //valeurs().echange_espace_virtuel();
              if (fonc_calc_)
                fonc_calc_(obj_calc_.valeur(), valeurs(), val_bord_, deriv_);
              /* premier calcul d'un Champ_Fonc_Calc -> on copie les valeurs calculees dans toutes les cases */
              if (fonc_calc_ && !fonc_calc_init_)
                for (int j = 1; j < les_valeurs->nb_cases(); j++, fonc_calc_init_ = 1)
                  les_valeurs[j].valeurs() = valeurs();
              return;
            }
        }
      Cerr << "In Champ_Inc_base::mettre_a_jour(double), " << finl;
      Cerr << "time " << un_temps << " not found in field " << le_nom() << finl;
      Cerr << "The times available are :" << finl;
      for (int i = 0; i < les_valeurs->nb_cases(); i++)
        Cerr << "  " << les_valeurs[i].temps() << finl;
      Process::exit();
    }
  // Champ a une seule valeur temporelle :
  // On change le temps associe.
  else
    {
      changer_temps(un_temps);
      if (fonc_calc_)
        fonc_calc_(obj_calc_.valeur(), valeurs(), val_bord_, deriv_);
      //Inutile:
      //valeurs().echange_espace_virtuel();
    }
}

/*! @brief Fixe le temps du ieme champ futur.
 *
 * @param (double t, int i) le nouveau temps
 * @return (double) le nouveau temps
 */
double Champ_Inc_base::changer_temps_futur(double t, int i)
{
  Roue& la_roue = les_valeurs.valeur();
  la_roue.futur(i).changer_temps(t);
  return t;
}

/*! @brief Fixe le temps du ieme champ passe.
 *
 * @param (double t, int i) le nouveau temps
 * @return (double) le nouveau temps
 */
double Champ_Inc_base::changer_temps_passe(double t, int i)
{
  Roue& la_roue = les_valeurs.valeur();
  la_roue.passe(i).changer_temps(t);
  return t;
}

/*! @brief Retourne le temps du ieme champ futur.
 *
 * @param (int i) le temps
 * @return (double) le temps
 */
double Champ_Inc_base::recuperer_temps_futur(int i) const
{
  const Roue& la_roue = les_valeurs.valeur();
  return la_roue.futur(i).temps();
}

/*! @brief Retourne le temps du ieme champ passe.
 *
 * @param (int i) le temps
 * @return (double) le temps
 */
double Champ_Inc_base::recuperer_temps_passe(int i) const
{
  const Roue& la_roue = les_valeurs.valeur();
  return la_roue.passe(i).temps();
}

/*! @brief Sauvegarde le champ inconnue sur un flot de sortie.
 *
 *  Ecrit un identifiant, les valeurs du champs, et la date (le temps au moment de la sauvegarde).
 *
 * @param (Sortie& fich) un flot de sortie
 * @return (int) returns the size of array
 */
int Champ_Inc_base::sauvegarder(Sortie& fich) const
{
  // en mode ecriture special seul le maitre ecrit l'entete
  int a_faire, special;
  EcritureLectureSpecial::is_ecriture_special(special, a_faire);

  if (a_faire)
    {
      Nom mon_ident(nom_);
      mon_ident += que_suis_je();
      mon_ident += equation().probleme().domaine().le_nom();
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
      // fich << flush ; Ne flushe pas en binaire !
      fich.flush();
    }
  if (Process::je_suis_maitre())
    Cerr << "Backup of the field " << nom_ << " performed on time : " << Nom(temps_, "%e") << finl;
  if (!est_egal(temps_, equation().probleme().schema_temps().temps_courant()))
    {
      Cerr.precision(12);
      Cerr << "Problem in Champ_Inc_base::sauvegarder, temps_=" << temps_ << " temps_courant()=" << equation().probleme().schema_temps().temps_courant() << finl;
      Process::exit();
    }
  // Return the number of bytes written
  return bytes;
}

/*! @brief Lecture d'un champ inconnue a partir d'un flot d'entree en vue d'une reprise.
 *
 * @param (Entree& fich) un flot d'entree
 * @return (int) renvoie toujours 1
 */
int Champ_Inc_base::reprendre(Entree& fich)
{
  double un_temps;
  int special = EcritureLectureSpecial::is_lecture_special();
  if (nom_ != Nom("anonyme")) // lecture pour reprise
    {
      fich >> un_temps;
      int nb_val_nodales_old = nb_valeurs_nodales();
      Cerr << "Resume of the field " << nom_;

      if (special)
        EcritureLectureSpecial::lecture_special(*this, fich);
      else
        valeurs().lit(fich);
      if (nb_val_nodales_old != nb_valeurs_nodales())
        {
          Cerr << finl << "Problem in the resumption " << finl;
          Cerr << "The field wich is read, does not have same number of nodal values" << finl;
          Cerr << "that the field created by the discretization " << finl;
          Process::exit();
        }
      Cerr << " performed." << finl;
    }
  else // lecture pour sauter le bloc
    {
      DoubleTab tempo;
      fich >> un_temps;
      tempo.jump(fich);
    }
  return 1;
}

/*! @brief Calcule les valeurs du champs inconnue aux positions specifiees.
 *
 * @param (DoubleTab& positions) les positions ou l'ont doit calculer le champ inconnues
 * @param (DoubleTab& valeurs) le tableau des valeurs du champ inconnue aux positions voulues
 * @return (DoubleTab&) le tableau des valeurs du champ inconnue aux positions voulues
 */
DoubleTab& Champ_Inc_base::valeur_aux(const DoubleTab& positions, DoubleTab& tab_valeurs) const
{
  const Domaine& domaine = domaine_dis_base().domaine();
  IntVect les_polys;
  les_polys.resize(tab_valeurs.dimension_tot(0), Array_base::NOCOPY_NOINIT);

  domaine.chercher_elements(positions, les_polys);

  return valeur_aux_elems(positions, les_polys, tab_valeurs);
}

/*! @brief Calcule les valeurs du champs inconnue aux positions specifiees, pour une certaine composante du champ.
 *
 * @param (DoubleTab& positions) les positions ou l'ont doit calculer le champ inconnues
 * @param (DoubleTab& les_valeurs) le tableau des valeurs du champ inconnue aux positions voulues
 * @param (int) l'index de la composante du champ a calculer
 * @return (DoubleVect&) le tableau des valeurs de la composante du champ specifiee aux positions voulues
 */
DoubleVect& Champ_Inc_base::valeur_aux_compo(const DoubleTab& positions, DoubleVect& tab_valeurs, int ncomp) const
{
  const Domaine& domaine = domaine_dis_base().domaine();
  IntVect les_polys(positions.dimension(0));
  domaine.chercher_elements(positions, les_polys);
  return valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
}

/*! @brief Calcule la valeur du champs inconnue a la position specifiee.
 *
 * @param (DoubleVect& position) la position a laquelle on veut calculer le champ
 * @param (DoubleVect& les_valeurs) la valeur du champ inconnue a la position specifiee
 * @return (DoubleVect&) la valeur du champ inconnue a la position specifiee
 */
DoubleVect& Champ_Inc_base::valeur_a(const DoubleVect& position, DoubleVect& tab_valeurs) const
{
  const Domaine& domaine = domaine_dis_base().domaine();
  IntVect le_poly(1);
  domaine.chercher_elements(position, le_poly);
  return valeur_a_elem(position, tab_valeurs, le_poly(0));
}

/*! @brief Affectation d'un Champ generique (Champ_base) dans un champ inconnue.
 *
 * @param (Champ_base& ch) le champ partie droite de l'affectation
 * @return (Champ_base&) le resultat de l'affectation (*this)
 */
Champ_base& Champ_Inc_base::affecter_(const Champ_base& ch)
{
  DoubleTab noeuds;
  remplir_coord_noeuds(noeuds);

  // Modif B.M. pour ne pas faire d'interpolation sur les cases virtuelles
  if (valeurs().size_reelle_ok())
    {
      const int n = valeurs().dimension(0);
      DoubleTab pos, val;
      pos.ref_tab(noeuds, 0, n);
      val.ref_tab(valeurs(), 0, n);
      ch.valeur_aux(pos, val);
      //copie dans toutes les cases
      valeurs().echange_espace_virtuel();
      for (int i = 1; i < les_valeurs->nb_cases(); i++)
        les_valeurs[i].valeurs() = valeurs();
    }
  else
    {
      Cerr << "Champ_Inc_base::affecter_ not coded if size_reelle_ok()==0" << finl;
      Process::exit();
    }
  return *this;
}

//-Cas CL periodique : assure que les valeurs sur des faces periodiques
// en vis a vis sont identiques. Pour cela on prend la demi somme des deux valeurs.
//La methode est a surcharger pour des champs discretises aux faces.
void Champ_Inc_base::verifie_valeurs_cl()
{
}

/*! @brief Affectation d'une composante d'un Champ quelconque (Champ_base) dans une composante d'un champ inconnue
 *
 * @param (Champ_base& ch) la partie droite de l'affectation
 * @param (int compo) l'index de la composante a affecter
 * @return (Champ_base&) le resultat de l'affectation (avec upcast)
 */
Champ_base& Champ_Inc_base::affecter_compo(const Champ_base& ch, int compo)
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

/*! @brief voir Champ_base Cas particulier (malheureusement) du Champ_P0_VDF :
 *
 *     Si la frontiere est un raccord, le resultat est calcule sur le raccord associe. Dans ce cas, le DoubleTab x doit etre
 *     dimensionne sur le raccord associe.
 *
 */
DoubleTab& Champ_Inc_base::trace(const Frontiere_dis_base&, DoubleTab& x, double tps, int distant) const
{
  Cerr << que_suis_je() << "did not overloaded Champ_Inc_base::trace" << finl;
  return x;
}

/*! @brief NE FAIT RIEN Methode a surcharger
 *
 * @param (DoubleTab&)
 * @param (IntVect&)
 * @return (int) renvoie toujours 0
 */
int Champ_Inc_base::remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const
{
  return 0;
}

/*! @brief Simple appel a Champ_Inc_base::remplir_coord_noeuds(DoubleTab&)
 *
 * @param (DoubleTab& coord) coordonnees des noeuds a modifier
 * @param (int) l'index de la composante a modifier
 * @return (DoubleTab&)
 */
DoubleTab& Champ_Inc_base::remplir_coord_noeuds_compo(DoubleTab& coord, int) const
{
  return remplir_coord_noeuds(coord);
}

/*! @brief Simple appel a: Champ_Inc_base::remplir_coord_noeuds_et_polys(DoubleTab&,IntVect& poly)
 *
 * @param (DoubleTab& coord)
 * @param (IntVect& poly)
 * @param (int)
 * @return (int) code de retour propage
 */
int Champ_Inc_base::remplir_coord_noeuds_et_polys_compo(DoubleTab& coord, IntVect& poly, int) const
{
  return remplir_coord_noeuds_et_polys(coord, poly);
}

const Domaine& Champ_Inc_base::domaine() const
{
  return domaine_dis_base().domaine();
}

int Champ_Inc_base::imprime(Sortie& os, int ncomp) const
{
  Cerr << que_suis_je() << "::imprime not coded." << finl;
  Process::exit();
  return 1;
}

double Champ_Inc_base::integrale_espace(int ncomp) const
{
  Cerr << que_suis_je() << "::integrale_espace not coded." << finl;
  Process::exit();
  return 0.;
}

/*! @brief Fixe le temps du champ.
 *
 * @param (double t) le nouveau temps
 * @return (double) le nouveau temps
 */
double Champ_Inc_base::changer_temps(const double t)
{
  les_valeurs->changer_temps(t);
  return temps_ = t;
}

/*! @brief Associe le champ a l'equation dont il represente une inconnue.
 *
 * Simple appel a MorEqn::associer_eqn(const Equation_base&)
 *
 * @param (Equation_base& eqn) l'equation auquel le champ doit s'associer
 */
void Champ_Inc_base::associer_eqn(const Equation_base& eqn)
{
  MorEqn::associer_eqn(eqn);
}

void Champ_Inc_base::associer_domaine_cl_dis(const Domaine_Cl_dis& zcl)
{
  mon_dom_cl_dis = zcl;
}

void Champ_Inc_base::associer_domaine_dis_base(const Domaine_dis_base& z_dis)
{
  le_dom_VF = ref_cast(Domaine_VF, z_dis);
}

const Domaine_Cl_dis& Champ_Inc_base::domaine_Cl_dis() const
{
  if (!mon_dom_cl_dis.non_nul())
    return equation().domaine_Cl_dis();
  else
    return mon_dom_cl_dis.valeur();
}

Domaine_Cl_dis& Champ_Inc_base::domaine_Cl_dis()
{
  if (!mon_dom_cl_dis.non_nul())
    return equation().domaine_Cl_dis();
  else
    return mon_dom_cl_dis.valeur();
}

void Champ_Inc_base::init_champ_calcule(const Objet_U& obj, fonc_calc_t fonc)
{
  obj_calc_ = obj, fonc_calc_ = fonc, fonc_calc_init_ = 0;
  resize_val_bord();
}

void Champ_Inc_base::resize_val_bord()
{
  val_bord_.resize(ref_cast(Domaine_VF, domaine_dis_base()).xv_bord().dimension_tot(0), valeurs().line_size());
}

DoubleTab Champ_Inc_base::valeur_aux_bords() const
{
  //si Champ_Inc calcule (fonc_calc_ existe), alors les valeurs aux bords sont stockees dans val_bord_
  if (fonc_calc_ || bord_fluide_multiphase_)
    {
      DoubleTab result;
      result.ref(val_bord_);
      return result;
    }
  //sinon, calcul a partir des CLs
  const Domaine_VF& domaine = ref_cast(Domaine_VF, domaine_dis_base());
  const IntTab& f_e = domaine.face_voisins(), &f_s = domaine.face_sommets();
  DoubleTrav result(domaine.xv_bord().dimension_tot(0), valeurs().line_size());

  const Conds_lim& cls = domaine_Cl_dis().valeur().les_conditions_limites();
  int j, k, f, fb, s, n, N = result.line_size(), is_p = (le_nom().debute_par("pression") || le_nom().debute_par("pressure")), n_som;
  for (const auto& itr : cls)
    {
      const Front_VF& fr = ref_cast(Front_VF, itr->frontiere_dis());
      //valeur au bord imposee, sauf si c'est une paroi (dans ce cas, la CL peut avoir moins de composantes que le champ -> Energie_Multiphase)
      if (is_p ? sub_type(Neumann, itr.valeur()) : (sub_type(Dirichlet, itr.valeur()) && !sub_type(Scalaire_impose_paroi, itr.valeur())))
        for (j = 0; j < fr.nb_faces_tot(); j++)
          for (f = fr.num_face(j), fb = domaine.fbord(f), n = 0; n < N; n++)
            result(fb, n) = is_p ? ref_cast(Neumann, itr.valeur()).flux_impose(j, n) : ref_cast(Dirichlet, itr.valeur()).val_imp(j, n);
      else if (sub_type(Neumann_val_ext, itr.valeur())) //valeur externe imposee
        for (j = 0; j < fr.nb_faces_tot(); j++)
          for (f = fr.num_face(j), fb = domaine.fbord(f), n = 0; n < N; n++)
            result(fb, n) = ref_cast(Neumann_val_ext, itr.valeur()).val_ext(j, n);
      else if (sub_type(Champ_Inc_P0_base, *this))
        for (j = 0; j < fr.nb_faces_tot(); j++) //Champ P0 : on peut prendre la valeur en l'element
          for (f = fr.num_face(j), fb = domaine.fbord(f), n = 0; n < N; n++)
            result(fb, n) = valeurs()(f_e(f, f_e(f, 0) == -1), n);
      else if (sub_type(Champ_Inc_P1_base, *this))
        for (j = 0; j < fr.nb_faces_tot(); j++) //Champ P1 : moyenne des valeurs aux sommets
          {
            f = fr.num_face(j), fb = domaine.fbord(f);
            for (n_som = 0; n_som < f_s.dimension(1) && f_s(f, n_som) >= 0;)
              n_som++;
            for (n = 0; n < N; n++)
              result(fb, n) = 0;
            for (k = 0; k < n_som; k++)
              for (s = f_s(f, k), n = 0; n < N; n++)
                result(fb, n) += valeurs()(s, n) / n_som;
          }
      else
        Process::exit("Champ_Inc_base::valeur_aux_bords() : must code something!");
    }
  return result;
}
