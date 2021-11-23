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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_Inc_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/52
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Inc_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <EcritureLectureSpecial.h>
#include <Schema_Temps_base.h>
#include <MD_Vector_tools.h>
#include <Zone_VF.h>
#include <Dirichlet.h>
#include <Neumann_val_ext.h>
#include <Scalaire_impose_paroi.h>
#include <DoubleTrav.h>
#include <Champ_Inc_P0_base.h>

Implemente_base_sans_constructeur(Champ_Inc_base,"Champ_Inc_base",Champ_base);

// Description:
//    Surcharge Champ_base::printOn(Sortie&) const
//    Imprime le champ sur un flot de sortie.
//    Simple appel a Champ_base::printOn(Sortie&) const
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_Inc_base::printOn(Sortie& os) const
{
  return Champ_base::printOn(os);
}


// Description:
//    Lit un champ inconnue sur un flot d'entree.
//    Simple appel a Champ_base::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Champ_Inc_base::readOn(Entree& is)
{
  return Champ_base::readOn(is);
}


// Description:
//    Fixe le nombre de valeurs temporelles a conserver.
//    (un nombre different suivant le schema en temps utilise)
//    Appelle Roue::fixer_nb_cases(int)
// Precondition:
// Parametre: int i
//    Signification: le nombre de valeurs temporelles a conserver
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de valeurs temporelles a conserver
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: i valeurs temporelles successives du champ seront
//                conservees.
int Champ_Inc_base::fixer_nb_valeurs_temporelles(int i)
{
  return les_valeurs->fixer_nb_cases(i);
}


// Description:
//    Renvoie le nombre de valeurs temporelles actuellement conservees.
//    Cette valeur est stockee par la Roue du Champ_Inc_base
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de valeurs temporelles actuellement conservees
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_Inc_base::nb_valeurs_temporelles() const
{
  return les_valeurs->nb_cases();
}



// Description:
//    Lit les valeurs du champs a partir d'un flot d'entree.
//    Format de lecture:
//      int [LE NOMBRE DE VALEURS A LIRE]
//      [LIRE LE NOMBRE DE VALEUR VOULUES]
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie 1 si la lecture est correcte
//    Contraintes:
// Exception: le nombre de valeur a lire est incorrect
// Effets de bord:
// Postcondition: le champ contient les valeurs lues dans le flot d'entree
int Champ_Inc_base::lire_donnees(Entree& is)
{
  int n;
  is >> n;
  if (n != les_valeurs->valeurs().size() )
    {
      Cerr << " the file does not contain the correct number of values to fill the field" << finl;
      exit();
    }
  DoubleVect& tab = les_valeurs->valeurs();
  for (int i=0; i<n; i++)
    is >> tab[i];
  return 1;
}

int Champ_Inc_base::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  Cerr << "Internal error in Champ_Inc_base::fixer_nb_valeurs_nodales: method has not been implemented for class "
       << que_suis_je() << finl;
  exit();
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
              Cerr << "Internal error in Champ_Inc_base::creer_tableau_distribue:\n"
                   << " array has alreary a (wrong) parallel descriptor" << finl;
              exit();
            }
          MD_Vector_tools::creer_tableau_distribue(md, tab, opt);
        }
    }
}

// Description: Returns the number of "real" geometric positions of the
//  degrees of freedom, or -1 if not applicable (fields with multiple
//  localisations)
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

// Description:
//    Renvoie le tableau des valeurs du champ au temps courant.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Champ_Inc_base::valeurs()
{
  return les_valeurs->valeurs();
}

// Description:
//    Renvoie le tableau des valeurs du champ au temp courant
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const DoubleTab& Champ_Inc_base::valeurs() const
{
  return les_valeurs->valeurs();
}

// Description:
//    Renvoie les valeurs du champs a l'instant temps.
// Precondition: temps doit etre un temps de discretisation
// Parametre: double temps
//    Signification: le  temps  auquel on veut les valeurs du champ
//    Valeurs par defaut: non
//    Contraintes: temps doit etre un temps de discretisation
//    Acces:
// Retour: DoubleTab&
//    Signification: les valeurs du champs a l'instant temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Champ_Inc_base::valeurs(double tps)
// WEC : Attention dans le cas de Pb_Couple on utilisait le fait que
// cette fonction renvoyait le present quand elle ne trouvait
// pas un temps superieur a tous les temps disponibles!!!
// Le comportement est maintenant plus explicite : un WARNING est
// affiche des que le present est renvoye a la place du temps demande.
{
  if (temps()==tps)
    return valeurs();
  else
    {
      Roue& la_roue=les_valeurs.valeur();
      if(temps()<tps)
        {
          for(int i=0; i<nb_valeurs_temporelles(); i++)
            {
              if(la_roue.futur(i).temps()==tps)
                return la_roue.futur(i).valeurs();
              else if(la_roue.futur(i).temps()<temps())
                break;
            }
        }
      else if(temps()>tps)
        {
          for(int i=0; i<nb_valeurs_temporelles(); i++)
            {
              if(la_roue.passe(i).temps()==tps)
                return la_roue.passe(i).valeurs();
              else if(la_roue.passe(i).temps()>temps())
                break;
            }
        }
    }
  Cerr << "ERROR : in Champ_Inc_base::valeurs(double), time " << tps << " not found, returns the present?" << finl;
  Cerr << "Contact TRUST support." << finl;
  exit();
  return valeurs();
}

// Description:
//    Renvoie les valeurs du champs a l'instant temps.
// Precondition: temps doit etre un temps de discretisation
// Parametre: double temps
//    Signification: le  temps  auquel on veut les valeurs du champ
//    Valeurs par defaut: non
//    Contraintes: temps doit etre un temps de discretisation
//    Acces:
// Retour: DoubleTab&
//    Signification: les valeurs du champs a l'instant temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const DoubleTab& Champ_Inc_base::valeurs(double tps) const
// See above !
{
  if (temps()==tps)
    return valeurs();
  else
    {
      const Roue& la_roue=les_valeurs.valeur();

      if(temps()<tps)
        {
          // Futur ?
          for(int i=0; i<nb_valeurs_temporelles(); i++)
            {
              if(la_roue.futur(i).temps()==tps)
                return la_roue.futur(i).valeurs();
              else if(la_roue.futur(i).temps()<temps())
                break;
            }
        }
      else if(temps()>tps)
        {
          // Passe ?
          for(int i=1; i<nb_valeurs_temporelles(); i++)
            {
              if(la_roue.passe(i).temps()==tps)
                return la_roue.passe(i).valeurs();
              else if(la_roue.passe(i).temps()>temps())
                break;
            }
        }
    }
  Cerr << "ERROR : in Champ_Inc_base::valeurs(double), time " << tps << " not found, returns the present?" << finl;
  Cerr << "Contact TRUST support." << finl;
  exit();
  return valeurs();
}
// Description:
//    Renvoie les valeurs du champs a l'instant t+i.
// Precondition: i doit etre inferieur au nombre de valeurs
//               temporelles conservees.
// Parametre: int i
//    Signification: le pas de temps futur auquel on veut les valeurs du champ
//    Valeurs par defaut: 1
//    Contraintes: i <= nombre de valeurs temporelles conservees
//    Acces:
// Retour: DoubleTab&
//    Signification: les valeurs du champs a l'instant t+i
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Champ_Inc_base::futur(int i)
{
  return les_valeurs->futur(i).valeurs();
}

// Description:
//    Renvoie les valeurs du champs a l'instant t+i.
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: le pas de temps futur auquel on veut les valeurs du champ
//    Valeurs par defaut: 1
//    Contraintes: i <= nombre de valeurs temporelles conservees
//    Acces:
// Retour: DoubleTab&
//    Signification: les valeurs du champs a l'instant t+i
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const DoubleTab& Champ_Inc_base::futur(int i) const
{
  return les_valeurs->futur(i).valeurs();
}


// Description:
//    Renvoie les valeurs du champs a l'instant t-i.
// Precondition:
// Parametre: int i
//    Signification: le pas de temps passe auquel on veut les valeurs du champ
//    Valeurs par defaut: 1
//    Contraintes: i <= nombre de valeurs temporelles conservees
//    Acces:
// Retour: DoubleTab&
//    Signification: les valeurs du champs a l'instant t-i
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Champ_Inc_base::passe(int i)
{
  return les_valeurs->passe(i).valeurs();
}


// Description:
//    Renvoie les valeurs du champs a l'instant t-i.
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: le pas de temps passe auquel on veut les valeurs du champ
//    Valeurs par defaut: 1
//    Contraintes: i <= nombre de valeurs temporelles conservees
//    Acces:
// Retour: DoubleTab&
//    Signification: les valeurs du champs a l'instant t-i
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const DoubleTab& Champ_Inc_base::passe(int i) const
{
  return les_valeurs->passe(i).valeurs();
}


// Description:
//    Avance le pointeur courant de i pas de temps,
//    dans la liste des valeurs temporelles conservees.
// Precondition:
// Parametre: int i
//    Signification: le nombre de pas de temps dont on avance
//    Valeurs par defaut: 1
//    Contraintes: i < nombre de valeurs temporelles conservees
//    Acces:
// Retour: Champ_Inc_base&
//    Signification: renvoie *this, le champ au pas de temps voulu
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Inc_base& Champ_Inc_base::avancer(int i)
{
  while(i--)
    les_valeurs->avancer(les_valeurs);
  temps_=les_valeurs->temps();
  return *this;
}


// Description:
//    Recule le pointeur courant de i pas de temps,
//    dans la liste des valeurs temporelles conservees.
// Precondition:
// Parametre: int i
//    Signification: le nombre de pas de temps dont on recule
//    Valeurs par defaut: 1
//    Contraintes: i < nombre de valeurs temporelles conservees
//    Acces:
// Retour: Champ_Inc_base&
//    Signification: renvoie *this, le champ au pas de temps voulu
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Inc_base& Champ_Inc_base::reculer(int i)
{
  while(i--)
    les_valeurs->reculer(les_valeurs);
  temps_=les_valeurs->temps();
  return *this;
}

// Description:
//    Effectue une mise a jour en temps du champ inconnue.
//    WEC : Maintenant si on l'appelle 2 fois de suite avec le meme
//    argument, la 2eme ne fait rien.
// Precondition:
// Parametre: double temps
//    Signification: le nouveau temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_Inc_base::mettre_a_jour(double un_temps)
{
  // Champ a plusieurs valeurs temporelle :
  // On avance a la bonne valeur temporelle.
  if (les_valeurs->nb_cases()>1)
    {
      for(int i=0; i<les_valeurs->nb_cases(); i++)
        {
          if(les_valeurs[i].temps()==un_temps)
            {
              avancer(i);
              temps_=un_temps;
              //Inutile:
              //valeurs().echange_espace_virtuel();
              if (fonc_calc_) fonc_calc_(obj_calc_.valeur(), valeurs(), val_bord_, deriv_);
              /* premier calcul d'un Champ_Fonc_Calc -> on copie les valeurs calculees dans toutes les cases */
              if (fonc_calc_ && !fonc_calc_init_) for (int j = 1; j < les_valeurs->nb_cases(); j++, fonc_calc_init_ = 1)
                  les_valeurs[j].valeurs() = valeurs();
              return;
            }
        }
      Cerr << "In Champ_Inc_base::mettre_a_jour(double), " << finl;
      Cerr << "time " << un_temps << " not found in field " << le_nom() << finl;
      Cerr << "The times available are :" << finl;
      for(int i=0; i<les_valeurs->nb_cases(); i++)
        Cerr << "  " << les_valeurs[i].temps() << finl;
      exit();
    }
  // Champ a une seule valeur temporelle :
  // On change le temps associe.
  else
    {
      changer_temps(un_temps);
      if (fonc_calc_) fonc_calc_(obj_calc_.valeur(), valeurs(), val_bord_, deriv_);
      //Inutile:
      //valeurs().echange_espace_virtuel();
    }
}

// Description:
//    Fixe le temps du ieme champ futur.
// Precondition:
// Parametre: double& t, int i
//    Signification: le nouveau temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le nouveau temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ est au temps specifie
double Champ_Inc_base::changer_temps_futur(const double& t, int i)
{
  Roue& la_roue=les_valeurs.valeur();
  la_roue.futur(i).changer_temps(t);
  return t;
}


// Description:
//    Fixe le temps du ieme champ passe.
// Precondition:
// Parametre: double& t, int i
//    Signification: le nouveau temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le nouveau temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ est au temps specifie
double Champ_Inc_base::changer_temps_passe(const double& t, int i)
{
  Roue& la_roue=les_valeurs.valeur();
  la_roue.passe(i).changer_temps(t);
  return t;
}

// Description:
//    Retourne le temps du ieme champ futur.
// Precondition:
// Parametre: int i
//    Signification: le temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ est au temps specifie
double Champ_Inc_base::recuperer_temps_futur(int i) const
{
  const Roue& la_roue=les_valeurs.valeur();
  return la_roue.futur(i).temps();
}


// Description:
//    Retourne le temps du ieme champ passe.
// Precondition:
// Parametre: int i
//    Signification: le temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ est au temps specifie
double Champ_Inc_base::recuperer_temps_passe(int i) const
{
  const Roue& la_roue=les_valeurs.valeur();
  return la_roue.passe(i).temps();
}

// Description:
//    Sauvegarde le champ inconnue sur un flot de sortie.
//    Ecrit un identifiant, les valeurs du champs, et
//    la date (le temps au moment de la sauvegarde).
// Precondition:
// Parametre: Sortie& fich
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: returns the size of array
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
// Postcondition: le champ inconnue est sauvegarde
int Champ_Inc_base::sauvegarder(Sortie& fich) const
{
  // en mode ecriture special seul le maitre ecrit l'entete
  int a_faire,special;
  EcritureLectureSpecial::is_ecriture_special(special,a_faire);

  if (a_faire)
    {
      Nom mon_ident(nom_);
      mon_ident += que_suis_je();
      mon_ident += equation().probleme().domaine().le_nom();
      mon_ident += Nom(temps_,"%e");
      fich << mon_ident << finl;
      fich << que_suis_je() << finl;
      fich << temps_ << finl;
    }
  int bytes = 0;
  if (special)
    bytes = EcritureLectureSpecial::ecriture_special(*this,fich);
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
    Cerr << "Backup of the field " << nom_ << " performed on time : " << Nom(temps_,"%e") << finl;

#ifndef NDEBUG
  if (!est_egal(temps_,equation().probleme().schema_temps().temps_courant()))
    {
      Cerr.precision(12);
      Cerr << "Problem in Champ_Inc_base::sauvegarder, temps_=" << temps_ << " temps_courant()=" << equation().probleme().schema_temps().temps_courant() << finl;
      exit();
    }
#endif
  // Return the number of bytes written
  return bytes;
}


// Description:
//    Lecture d'un champ inconnue a partir d'un flot d'entree
//    en vue d'une reprise.
// Precondition:
// Parametre: Entree& fich
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_Inc_base::reprendre(Entree& fich)
{
  double un_temps;
  int special= EcritureLectureSpecial::is_lecture_special();
  if (nom_ != Nom("anonyme")) // lecture pour reprise
    {
      fich >> un_temps;
      int nb_val_nodales_old = nb_valeurs_nodales();
      if (special)
        EcritureLectureSpecial::lecture_special(*this,fich);
      else
        valeurs().lit(fich);
      if (nb_val_nodales_old != nb_valeurs_nodales())
        {
          Cerr << "Problem in the resumption "<< finl;
          Cerr << "The field wich is read, does not have same number of nodal values" << finl;
          Cerr << "that the field created by the discretization " << finl;
          exit();
        }
      Cerr << "Resume of the field " <<nom_ << " performed." << finl;
    }
  else // lecture pour sauter le bloc
    {
      DoubleTab tempo;
      fich >> un_temps;
      tempo.jump(fich);
    }
  return 1;
}


// Description:
//    Calcule les valeurs du champs inconnue aux positions specifiees.
// Precondition:
// Parametre: DoubleTab& positions
//    Signification: les positions ou l'ont doit calculer le champ inconnues
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& valeurs
//    Signification: le tableau des valeurs du champ inconnue aux
//                   positions voulues
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ inconnue aux
//                   positions voulues
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_Inc_base::valeur_aux(const DoubleTab& positions,
                                      DoubleTab& tab_valeurs) const
{
  const Zone& zone=zone_dis_base().zone();
  IntVect les_polys;
  les_polys.resize(tab_valeurs.dimension_tot(0), Array_base::NOCOPY_NOINIT);

  zone.chercher_elements(positions, les_polys);

  return valeur_aux_elems(positions, les_polys, tab_valeurs);
}


// Description:
//    Calcule les valeurs du champs inconnue aux positions specifiees,
//    pour une certaine composante du champ.
// Precondition:
// Parametre: DoubleTab& positions
//    Signification: les positions ou l'ont doit calculer le champ inconnues
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& les_valeurs
//    Signification: le tableau des valeurs du champ inconnue aux
//                   positions voulues
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Parametre: int
//    Signification: l'index de la composante du champ a calculer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect&
//    Signification: le tableau des valeurs de la composante du champ
//                   specifiee aux positions voulues
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Inc_base::valeur_aux_compo(const DoubleTab& positions,
                                             DoubleVect& tab_valeurs,
                                             int ncomp) const
{
  const Zone& zone=zone_dis_base().zone();
  IntVect les_polys(positions.dimension(0));
  zone.chercher_elements(positions, les_polys);
  return valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
}


// Description:
//    Calcule la valeur du champs inconnue a la position specifiee.
// Precondition:
// Parametre: DoubleVect& position
//    Signification: la position a laquelle on veut calculer le champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& les_valeurs
//    Signification: la valeur du champ inconnue a la position specifiee
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: DoubleVect&
//    Signification: la valeur du champ inconnue a la position specifiee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Inc_base::valeur_a(const DoubleVect& position,
                                     DoubleVect& tab_valeurs) const
{
  const Zone& zone=zone_dis_base().zone();
  IntVect le_poly(1);
  zone.chercher_elements(position, le_poly);
  return valeur_a_elem(position, tab_valeurs, le_poly(0));
}


// Description:
//    Affectation d'un Champ generique (Champ_base) dans
//    un champ inconnue.
// Precondition:
// Parametre: Champ_base& ch
//    Signification: le champ partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_base&
//    Signification: le resultat de l'affectation (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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
      for(int i=1; i<les_valeurs->nb_cases(); i++) les_valeurs[i].valeurs() = valeurs();
    }
  else
    {
      Cerr << "Champ_Inc_base::affecter_ not coded if size_reelle_ok()==0" << finl;
      exit();
    }
  return *this;
}


//-Cas CL periodique : assure que les valeurs sur des faces periodiques
// en vis a vis sont identiques. Pour cela on prend la demi somme des deux valeurs.
//La methode est a surcharger pour des champs discretises aux faces.
void Champ_Inc_base::verifie_valeurs_cl()
{

}

// Description:
//    Affectation d'une composante d'un Champ quelconque
//    (Champ_base) dans une composante d'un champ inconnue
// Precondition:
// Parametre: Champ_base& ch
//    Signification: la partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int compo
//    Signification: l'index de la composante a affecter
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Champ_base&
//    Signification: le resultat de l'affectation (avec upcast)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_base& Champ_Inc_base::affecter_compo(const Champ_base& ch,
                                           int compo)
{
  DoubleTab noeuds;
  IntVect polys;
  if(!remplir_coord_noeuds_et_polys_compo(noeuds, polys, compo))
    {
      remplir_coord_noeuds_compo(noeuds, compo);
      ch.valeur_aux_compo(noeuds, valeurs(), compo);
    }
  else
    ch.valeur_aux_elems_compo(noeuds, polys, valeurs(), compo);
  return *this;
}



// Description:
//    voir Champ_base
//    Cas particulier (malheureusement) du Champ_P0_VDF :
//    Si la frontiere est un raccord, le resultat est calcule sur le
//    raccord associe. Dans ce cas, le DoubleTab x doit etre
//    dimensionne sur le raccord associe.
//

DoubleTab& Champ_Inc_base::trace(const Frontiere_dis_base& , DoubleTab& x , double tps,int distant) const
{
  Cerr << que_suis_je() << "did not overloaded Champ_Inc_base::trace" << finl;
  return x;
}


// Description:
//    NE FAIT RIEN
//    Methode a surcharger
// Precondition:
// Parametre: DoubleTab&
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: IntVect&
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 0
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_Inc_base::remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const
{
  return 0;
}

// Description:
//    Simple appel a
//       Champ_Inc_base::remplir_coord_noeuds(DoubleTab&)
// Precondition:
// Parametre: DoubleTab& coord
//    Signification: coordonnees des noeuds a modifier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int
//    Signification: l'index de la composante a modifier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_Inc_base::remplir_coord_noeuds_compo(DoubleTab& coord,
                                                      int ) const
{
  return remplir_coord_noeuds(coord);
}


// Description:
//    Simple appel a:
//       Champ_Inc_base::remplir_coord_noeuds_et_polys(DoubleTab&,IntVect& poly)
// Precondition:
// Parametre: DoubleTab& coord
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: IntVect& poly
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_Inc_base::remplir_coord_noeuds_et_polys_compo(DoubleTab& coord,
                                                        IntVect& poly,
                                                        int ) const
{
  return remplir_coord_noeuds_et_polys(coord, poly);
}

const Domaine& Champ_Inc_base::domaine() const
{
  return zone_dis_base().zone().domaine();
}

int Champ_Inc_base::imprime(Sortie& os, int ncomp) const
{
  Cerr << que_suis_je() << "::imprime not coded." << finl;
  exit();
  return 1;
}

double Champ_Inc_base::integrale_espace(int ncomp) const
{
  Cerr << que_suis_je() << "::integrale_espace not coded." << finl;
  exit();
  return 0.;
}

// Description:
//    Operateur de cast d'un champ en un Tableau de Double.
//    Renvoie le tableau des valeurs du champ.
//    Ex:
//        Champ_Inc_base un_champ;
//        DoubleTab un_tableau;
//
//        un_tableau = (DoubleTab) un_champ;
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Inc_base::operator DoubleTab& ()
{
  return les_valeurs->valeurs();
}

// Description:
//    Operateur de cast d'un champ en un Tableau de Double.
//    Renvoie le tableau des valeurs du champ.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Champ_Inc_base::operator const DoubleTab& () const
{
  return les_valeurs->valeurs();
}

// Description:
//    Fixe le temps du champ.
// Precondition:
// Parametre: double& t
//    Signification: le nouveau temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le nouveau temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ est au temps specifie
double Champ_Inc_base::changer_temps(const double& t)
{
  les_valeurs->changer_temps(t);
  return temps_ = t;
}

// Description:
//    Associe le champ a l'equation dont il represente une inconnue.
//    Simple appel a MorEqn::associer_eqn(const Equation_base&)
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation auquel le champ doit s'associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ est associe a son equation
void Champ_Inc_base::associer_eqn(const Equation_base& eqn)
{
  MorEqn::associer_eqn(eqn);
}

void Champ_Inc_base::associer_zone_cl_dis(const Zone_Cl_dis& zcl)
{
  ma_zone_cl_dis=zcl;
}

const Zone_Cl_dis& Champ_Inc_base::zone_Cl_dis() const
{
  if (!ma_zone_cl_dis.non_nul())
    return equation().zone_Cl_dis();
  else
    return ma_zone_cl_dis.valeur();
}

Zone_Cl_dis& Champ_Inc_base::zone_Cl_dis()
{
  if (!ma_zone_cl_dis.non_nul())
    return equation().zone_Cl_dis();
  else
    return ma_zone_cl_dis.valeur();
}

void Champ_Inc_base::init_champ_calcule(const Objet_U& obj, fonc_calc_t fonc)
{
  obj_calc_ = obj, fonc_calc_ = fonc;
  val_bord_.resize(ref_cast(Zone_VF, zone_dis_base()).xv_bord().dimension_tot(0), valeurs().line_size());
}


DoubleTab Champ_Inc_base::valeur_aux_bords() const
{
  //si Champ_Inc calcule (fonc_calc_ existe), alors les valeurs aux bords sont stockees dans val_bord_
  if (fonc_calc_)
    {
      DoubleTab result;
      result.ref(val_bord_);
      return result;
    }
  //sinon, calcul a partir des CLs
  const Zone_VF& zone = ref_cast(Zone_VF, zone_dis_base());
  const IntTab& f_e = zone.face_voisins();
  DoubleTrav result(zone.xv_bord().dimension_tot(0), valeurs().line_size());

  const Conds_lim& cls = zone_Cl_dis().valeur().les_conditions_limites();
  int i, j, f, fb, n, N = result.line_size(), is_p = (le_nom().debute_par("pression") || le_nom().debute_par("pressure"));
  for (i = 0; i < cls.size(); i++)
    {
      const Front_VF& fr = ref_cast(Front_VF, cls[i].valeur().frontiere_dis());
      //valeur au bord imposee, sauf si c'est une paroi (dans ce cas, la CL peut avoir moins de composantes que le champ -> Energie_Multiphase)
      if (is_p ? sub_type(Neumann, cls[i].valeur()) : (sub_type(Dirichlet, cls[i].valeur()) && !sub_type(Scalaire_impose_paroi, cls[i].valeur())))
        for (j = 0; j < fr.nb_faces_tot(); j++) for (f = fr.num_face(j), fb = zone.fbord(f), n = 0; n < N; n++)
            result(fb, n) = is_p ? ref_cast(Neumann, cls[i].valeur()).flux_impose(j, n) : ref_cast(Dirichlet, cls[i].valeur()).val_imp(j, n);
      else if (sub_type(Neumann_val_ext, cls[i].valeur())) //valeur externe imposee
        for (j = 0; j < fr.nb_faces_tot(); j++) for (f = fr.num_face(j), fb = zone.fbord(f), n = 0; n < N; n++)
            result(fb, n) = ref_cast(Neumann_val_ext, cls[i].valeur()).val_ext(j, n);
      else if (sub_type(Champ_Inc_P0_base, *this)) for (j = 0; j < fr.nb_faces_tot(); j++) //Champ P0 : on peut prendre la valeur en l'element
          for (f = fr.num_face(j), fb = zone.fbord(f), n = 0; n < N; n++) result(fb, n) = valeurs()(f_e(f, 0), n);
      else Process::exit("Champ_Inc_base::valeur_aux_bords() : mus code something!");
    }
  return result;
}
