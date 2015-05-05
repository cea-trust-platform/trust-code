/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Sonde_Int.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sonde_Int_included
#define Sonde_Int_included


#include <Ref_Postraitement.h>
#include <IntTab.h>
#include <DoubleTab.h>
#include <Ref_IntVect.h>
#include <SFichier.h>

class Zone;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Sonde_Int
//     Cette classe permet d'effectuer l'evolution d'un champ au cours du temps.
//     On choisit l'ensemble des points sur lesquels on veut sonder un champ et
//     la periodicite des observations. Les objets Postraitement porte des
//     des sondes sur les champs a observer, une sonde porte d'ailleurs une
//     reference sur un postraitement.
// .SECTION voir aussi
//     Postraitement Sonde_Ints
//////////////////////////////////////////////////////////////////////////////
class Sonde_Int : public Objet_U
{
  Declare_instanciable_sans_destructeur(Sonde_Int);

public :

  inline Sonde_Int(const Nom& );
  void associer_post(const Postraitement& );
  void initialiser(const Zone& );
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
  inline ~Sonde_Int();

private :

  REF(Postraitement) mon_post;
  Nom nom_;                                // le nom de la sonde
  int dim;                                // la dimension de la sone (point:0,segment:1,plan:2,volume:3)
  int nbre_points1,nbre_points2,nbre_points3;        // faire des sonde_segment,sonde_plan,etc...
  REF(IntVect) mon_tableau;                // Le tableau sonde
  DoubleTab les_positions_;                // les coordonnees des sondes ponctuelles
  IntVect elem_;                        // les elements contenant les sondes ponctuelles
  IntTab valeurs;                      // les valeurs de mon_tableau pour les indices donnes par elem_
  double periode;                        // periode d'echantillonnage
  double nb_bip;
  SFichier* le_fichier;
};


// Description:
//    Constructeur d'une sonde a partir de son nom.
// Precondition:
// Parametre: Nom& nom
//    Signification: le nom de la sonde a construire
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Sonde_Int::Sonde_Int(const Nom& nom)
  : nom_(nom), le_fichier(0)
{}



// Description:
//    Le temps ecoule.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le temps ecoule
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Sonde_Int::temps() const
{
  return nb_bip*periode;
}



// Description:
//    Ferme le fichier sur laquelle la sonde ecrit.
// Precondition:
// Parametre:
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
inline void Sonde_Int::fermer_fichier()
{
  if (le_fichier)
    {
      if (dim==2 || dim==3) fichier() << "FIN" << finl;
      delete le_fichier;
    }
}


// Description:
//    Fixe la periode avec laquelle on sonde le champ.
// Precondition:
// Parametre: double pe
//    Signification: la periode de sondage du champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: on sondera le champ tous les "pe" seconde
inline void Sonde_Int::fixer_periode(double pe)
{
  periode=pe;
}


// Description:
//    Renvoie le champ associe.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_base&
//    Signification: le champ associe
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const IntVect& Sonde_Int::le_tableau() const
{
  return mon_tableau.valeur();
}


// Description:
//    Renvoie le tableau des positions du champ qui sont sondees.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: les positions sondees
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const DoubleTab& Sonde_Int::les_positions() const
{
  return les_positions_;
}


// Description:
//    Renvoie le tableau des elements qui sont sondes.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: IntVect&
//    Signification: les ments qui sont sondes
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const IntVect& Sonde_Int::les_poly() const
{
  return elem_;
}


// Description:
//    Renvoie un flot de sortie Fichier, pointant sur
//    le fichier de sortie utilise par la sonde.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: SFichier&
//    Signification: le fichier de sortie utilise par la sonde
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline SFichier& Sonde_Int::fichier()
{
  return *le_fichier;
}

// Description:
//    Destructeur. Ferme le fichier avant de detruire l'objet.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le fichier associe a la sonde est ferme
inline Sonde_Int::~Sonde_Int()
{
  fermer_fichier();
}


#endif
