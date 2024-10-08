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

#ifndef Sonde_Int_included
#define Sonde_Int_included

#include <TRUST_Ref.h>
#include <SFichier.h>
#include <TRUSTTab.h>

class Postraitement;
#include <Domaine_forward.h>

/*! @brief classe Sonde_Int Cette classe permet d'effectuer l'evolution d'un champ au cours du temps.
 *
 *      On choisit l'ensemble des points sur lesquels on veut sonder un champ et
 *      la periodicite des observations. Les objets Postraitement porte des
 *      des sondes sur les champs a observer, une sonde porte d'ailleurs une
 *      reference sur un postraitement.
 *
 * @sa Postraitement Sonde_Ints
 */
class Sonde_Int : public Objet_U
{
  Declare_instanciable_sans_destructeur(Sonde_Int);

public :

  inline Sonde_Int(const Nom& );
  void associer_post(const Postraitement& );
  void initialiser(const Domaine& );
  void mettre_a_jour(double temps, double tinit);
  void postraiter(double );
  void ouvrir_fichier();
  inline void fermer_fichier();
  inline const IntVect& le_tableau() const;
  inline const DoubleTab& les_positions() const;
  inline const IntVect& les_poly() const;
  inline void fixer_periode(double);
  inline double temps() const;
  //int add(const DoubleTab&);
  //int add(const double, const double);
  //int add(const double, const double, const double);
  inline SFichier& fichier();
  inline ~Sonde_Int() override;

private :

  OBS_PTR(Postraitement) mon_post;
  Nom nom_;                                // le nom de la sonde
  int dim = -1;                                // la dimension de la sone (point:0,segment:1,plan:2,volume:3)
  int nbre_points1= -1,nbre_points2= -1,nbre_points3= -1;        // faire des sonde_segment,sonde_plan,etc...
  OBS_PTR(IntVect) mon_tableau;                // Le tableau sonde
  DoubleTab les_positions_;                // les coordonnees des sondes ponctuelles
  IntVect elem_;                        // les elements contenant les sondes ponctuelles
  IntTab valeurs;                      // les valeurs de mon_tableau pour les indices donnes par elem_
  double periode= 1.e10;                        // periode d'echantillonnage
  double nb_bip= 0.;
  SFichier* le_fichier;
};


/*! @brief Constructeur d'une sonde a partir de son nom.
 *
 * @param (Nom& nom) le nom de la sonde a construire
 */
inline Sonde_Int::Sonde_Int(const Nom& nom)
  : nom_(nom), le_fichier(0)
{}



/*! @brief Le temps ecoule.
 *
 * @return (double) le temps ecoule
 */
inline double Sonde_Int::temps() const
{
  return nb_bip*periode;
}



/*! @brief Ferme le fichier sur laquelle la sonde ecrit.
 *
 */
inline void Sonde_Int::fermer_fichier()
{
  if (le_fichier)
    {
      delete le_fichier;
    }
}


/*! @brief Fixe la periode avec laquelle on sonde le champ.
 *
 * @param (double pe) la periode de sondage du champ
 */
inline void Sonde_Int::fixer_periode(double pe)
{
  periode=pe;
}


/*! @brief Renvoie le champ associe.
 *
 * @return (Champ_base&) le champ associe
 */
inline const IntVect& Sonde_Int::le_tableau() const
{
  return mon_tableau.valeur();
}


/*! @brief Renvoie le tableau des positions du champ qui sont sondees.
 *
 * @return (DoubleTab&) les positions sondees
 */
inline const DoubleTab& Sonde_Int::les_positions() const
{
  return les_positions_;
}


/*! @brief Renvoie le tableau des elements qui sont sondes.
 *
 * @return (IntVect&) les ments qui sont sondes
 */
inline const IntVect& Sonde_Int::les_poly() const
{
  return elem_;
}


/*! @brief Renvoie un flot de sortie Fichier, pointant sur le fichier de sortie utilise par la sonde.
 *
 * @return (SFichier&) le fichier de sortie utilise par la sonde
 */
inline SFichier& Sonde_Int::fichier()
{
  return *le_fichier;
}

/*! @brief Destructeur.
 *
 * Ferme le fichier avant de detruire l'objet.
 *
 */
inline Sonde_Int::~Sonde_Int()
{
  fermer_fichier();
}


#endif
