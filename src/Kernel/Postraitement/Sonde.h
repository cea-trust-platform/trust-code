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

#ifndef Sonde_included
#define Sonde_included

#include <TRUSTArrays.h>
#include <TRUST_Ref.h>
#include <TRUSTTab.h>
#include <SFichier.h>
#include <Motcle.h>

class Operateur_Statistique_tps_base;
class Champ_Generique_base;
class Postraitement;
class Champ_base;
class Domaine;

/*! @brief classe Sonde Cette classe permet d'effectuer l'evolution d'un champ au cours du temps.
 *
 *      On choisit l'ensemble des points sur lesquels on veut sonder un champ et
 *      la periodicite des observations. Les objets Postraitement porte des
 *      des sondes sur les champs a observer, une sonde porte d'ailleurs une
 *      reference sur un postraitement.
 *
 * @sa Postraitement Sondes
 */
class Sonde : public Objet_U
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Sonde);

public :

  Sonde();
  Sonde(const Nom& );
  void associer_post(const Postraitement& );
  void initialiser();
  virtual void mettre_a_jour(double temps, double tinit);
  void postraiter();
  void ouvrir_fichier();
  virtual void completer();
  inline void fermer_fichier();
  inline const Champ_Generique_base& le_champ() const;
  inline const DoubleTab& les_positions_sondes_initiales() const; // Positions initiales
  inline const DoubleTab& les_positions_sondes() const; // Positions apres deplacement
  inline const DoubleTab& les_positions() const; // Positions locales au proc
  inline const IntVect& les_poly() const;
  inline void fixer_periode(double);
  inline double temps() const;
  inline SFichier& fichier();
  inline ~Sonde() override;
  inline const Nom& get_nom() const { return nom_; }
  inline const Nom& get_type() const { return type_; }
  inline const int& get_dim() const { return dim ; }
  inline void nommer(const Nom& n) override { nom_ = n; }

  // Traitement des bords (option "gravcl")
  void ajouter_bords(const DoubleTab& coords_bords);
  void init_bords();
  void mettre_a_jour_bords();

protected :

  REF(Postraitement) mon_post;
  Nom nom_;                               // le nom de la sonde
  Nom nom_fichier_;                       // le nom du fichier contenant la sonde
  int dim;                                // la dimension de la sone (point:0,segment:1,plan:2,volume:3)
  REF(Champ_Generique_base) mon_champ;
  REF(Operateur_Statistique_tps_base) operateur_statistique_;        // Reference vers un operateur statistique eventuel
  int ncomp;                              // Numero de la composante a sonder
  // Si ncomp = -1 la sonde s'applique a toutes les
  // composantes du champ
  DoubleTab les_positions_sondes_initiales_;    // les coordonnees des sondes ponctuelles initiales
  DoubleTab les_positions_sondes_;        // les coordonnees des sondes sur tout le domaine apres deplacement (uniquement sur le maitre)
  DoubleTab les_positions_;       // les coordonnees des sondes locales sur chaque proc
  int numero_elem_;                       // vaut -1 si pas defini et vaut le numero de l'elem sur le maitre
  IntVect elem_;                          // les elements contenant les sondes ponctuelles locales
  double periode;                         // periode d'echantillonnage
  // cles pour typage des sondes (sondes redefinies aux noeuds ou d'apres les valeurs aux sommets ou au centre de gravite ou aux sommets)
  bool nodes,chsom,grav,gravcl,som;
  DoubleTab valeurs_locales,valeurs_sur_maitre;     // valeurs_locales les valeurs sur chaque proc, valeurs_sur_maitre les valeurs regroupes sur le maitre
  double nb_bip;
  SFichier le_fichier_;
  Motcle nom_champ_lu_;
  ArrsOfInt participant ;            // vecteur d'ArrOfInt sur le maitre ; participant[pe][i] -> le ieme point sur pe correspond  la  participant [pe][i]  eme position
  int reprise;                            // si reprise=0, on cree la sonde, sinon on ecrit a la suite
  Nom type_;
  int orientation_faces_;

  // Traitement des bords (option "gravcl")
  ArrOfInt faces_bords_;                  // array containing the indices of the boundary faces hit by the probe
  IntTab rang_cl_;                        // for a given face, index of the CL that this face bears
  int nbre_points1 = -1,nbre_points2 = -1,nbre_points3 = -1;        // faire des sonde_segment,sonde_plan,etc...
};


/*! @brief Le temps ecoule.
 *
 * @return (double) le temps ecoule
 */
inline double Sonde::temps() const
{
  return nb_bip*periode;
}



/*! @brief Ferme le fichier sur laquelle la sonde ecrit.
 *
 */
inline void Sonde::fermer_fichier()
{
  if (fichier().is_open()) le_fichier_.close();
}


/*! @brief Fixe la periode avec laquelle on sonde le champ.
 *
 * @param (double pe) la periode de sondage du champ
 */
inline void Sonde::fixer_periode(double pe)
{
  periode=pe;
}


/*! @brief Renvoie le champ associe.
 *
 * @return (Champ_base&) le champ associe
 */
inline const Champ_Generique_base& Sonde::le_champ() const
{
  return mon_champ.valeur();
}


/*! @brief Renvoie le tableau des positions du champ qui sont sondees.
 *
 * @return (DoubleTab&) les positions sondees
 */

inline const DoubleTab& Sonde::les_positions_sondes_initiales() const
{
  return les_positions_sondes_initiales_;
}

inline const DoubleTab& Sonde::les_positions_sondes() const
{
  return les_positions_sondes_;
}

inline const DoubleTab& Sonde::les_positions() const
{
  return les_positions_;
}

/*! @brief Renvoie le tableau des elements qui sont sondes.
 *
 * @return (IntVect&) les ments qui sont sondes
 */
inline const IntVect& Sonde::les_poly() const
{
  return elem_;
}


/*! @brief Renvoie un flot de sortie Fichier, pointant sur le fichier de sortie utilise par la sonde.
 *
 * @return (SFichier&) le fichier de sortie utilise par la sonde
 */
inline SFichier& Sonde::fichier()
{
  return le_fichier_;
}

/*! @brief Destructeur.
 *
 * Ferme le fichier avant de detruire l'objet.
 *
 */
inline Sonde::~Sonde()
{
  fermer_fichier();
}


#endif
