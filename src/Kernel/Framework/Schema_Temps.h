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
// File:        Schema_Temps.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Schema_Temps_included
#define Schema_Temps_included

#include <Schema_Temps_base.h>
#include <Deriv.h>

Declare_deriv(Schema_Temps_base);

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Schema_Temps
//     Classe generique de la hierarchie des schemas en temps, un objet
//     Schema_Temps peut referencer n'importe quel objet derivant de
//     Schema_Temps_base.
//     La plupart des methodes appellent les methodes de l'objet
//     Schema_Temps_base sous-jacent via la methode valeur() declaree grace
//alamacroDeclare_deriv().;
// .SECTION voir aussi
//      Sch_Tps_base
//////////////////////////////////////////////////////////////////////////////
class Schema_Temps : public DERIV(Schema_Temps_base)
{
  Declare_instanciable(Schema_Temps);
  //
public :
  inline int sauvegarder(Sortie& ) const override;
  inline int reprendre(Entree& ) override;
  inline int faire_un_pas_de_temps_eqn_base(Equation_base& );
  inline bool iterateTimeStep(bool& converged);
  inline int mettre_a_jour();
  inline void imprimer(Sortie& os) const;
  inline double pas_de_temps() const;

  inline void corriger_dt_calcule(double& ) const;
  inline bool initTimeStep(double dt);
  inline double pas_temps_min() const;
  inline double& pas_temps_min();
  inline double pas_temps_max() const;
  inline double& pas_temps_max();
  inline int nb_pas_dt() const;
  inline double temps_courant() const;
  inline double temps_calcul() const;
  inline void changer_temps_courant(const double );
  inline double facteur_securite_pas() const;
  inline double& facteur_securite_pas();
  inline int stop();
  inline int impr(Sortie& ) const;
  inline int lsauv() const;
  inline Schema_Temps& operator=(const Schema_Temps_base&);
  //inline Schema_Temps& operator=(const Schema_Temps&);
};


// Description:
//    Operateur d'affectation d'un Schema_Temp_base dans un
//    Schema_temps.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Schema_Temps&
//    Signification: renvoie une reference sur *this: le schema nouvellement
//                   affecte.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'objet contient un reference sur le parametre

inline Schema_Temps& Schema_Temps::operator=(const Schema_Temps_base& sch)
{
  DERIV(Schema_Temps_base)::operator=(sch);
  return *this;
}
/*
inline Schema_Temps& Schema_Temps::operator=(const Schema_Temps& sch)
{
  *this=sch;
  return *this;
}
*/
// Description:
//    Appel a l'objet sous-jacent.
//    Sauvegarde le schema en temps sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour sous-jacent
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Schema_Temps::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}
// Description:
//    Appel a l'objet sous-jacent.
//    Effectue une reprise (lecture) du schema en temps a partir
//    d'un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour sous-jacent
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Schema_Temps::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Effectue un pas de temps sur une equation.
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation sur laquelle on effectue un pas de temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour sous-jacent
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Schema_Temps::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{
  return valeur().faire_un_pas_de_temps_eqn_base(eqn);
}

// Description:
//    Appel a l'objet sous-jacent.
//    Effectue un pas de temps sur le probleme
// Precondition:
// Parametre: Probleme_base& pb
//    Signification: le probleme sur lequel on effectue un pas de temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour sous-jacent
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline bool Schema_Temps::iterateTimeStep(bool& converged)
{
  return valeur().iterateTimeStep(converged);
}

// Description:
//    Appel a l'objet sous-jacent
//    Mise a jour du temps et du nombre de pas de temps..
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour sous-jacent
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Schema_Temps::mettre_a_jour()
{
  return valeur().mettre_a_jour();
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le pas en temps courant.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:  le pas de temps courant.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Schema_Temps::pas_de_temps() const
{
  return valeur().pas_de_temps();
}
// Description:
//    Appel a l'objet sous-jacent
//    Corrige le pas de temps : dtmin <= dt <= dtmax.
// Precondition:
// Parametre: double& dt
//    Signification: le pas de temps a corriger
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le pas de temps est corrige
inline void Schema_Temps::corriger_dt_calcule(double& dt) const
{
  valeur().corriger_dt_calcule(dt);
}
// Description:
//    Appel a l'objet sous-jacent
//    Pretraitement eventuel.
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
inline bool Schema_Temps::initTimeStep(double dt)
{
  return valeur().initTimeStep(dt);
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le pas de temps minimum.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le pas de temps minimum
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Schema_Temps::pas_temps_min() const
{
  return valeur().pas_temps_min();
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie une reference sur le pas de temps minimum.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double&
//    Signification: le pas de temps minimum
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double& Schema_Temps::pas_temps_min()
{
  return valeur().pas_temps_min();
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le pas de temps maximum.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le pas de temps maximum
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Schema_Temps::pas_temps_max() const
{
  return valeur().pas_temps_max();
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie une reference sur le pas de temps maximum.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:double&
//    Signification: le pas de temps maximum
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double& Schema_Temps::pas_temps_max()
{
  return valeur().pas_temps_max();
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nombre de pas de temps effectues.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de pas de temps effectues
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Schema_Temps::nb_pas_dt() const
{
  return valeur().nb_pas_dt();
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le temps courant
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le temps courant
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Schema_Temps::temps_courant() const
{
  return valeur().temps_courant();
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le temps ecoule calcule: (temps courant - temps debut)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le temps ecoule calcule
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Schema_Temps::temps_calcul() const
{
  return valeur().temps_calcul();
}
// Description:
//    Appel a l'objet sous-jacent
//    Change le temps courant
// Precondition:
// Parametre: double& t
//    Signification: la nouvelle valeur du temps courant
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Schema_Temps::changer_temps_courant(const double t)
{
  valeur().changer_temps_courant(t);
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le facteur de securite ou multiplicateur de pas de temps.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le facteur de securite du schema en temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Schema_Temps::facteur_securite_pas() const
{
  return valeur().facteur_securite_pas();
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie une reference sur le facteur de securite ou multiplicateur de
//    pas de temps
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double&
//    Signification: le facteur de securite du schema en temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double& Schema_Temps::facteur_securite_pas()
{
  return valeur().facteur_securite_pas();
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie 1 si il y lieu de stopper le calcul pour differente raisons:
//        - le temps final est atteint
//        - le nombre de pas de temps maximum est depasse
//        - l'etat stationnaire est atteint
//        - indicateur d'arret fichier
//    Renvoie 0 sinon
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:entier
//    Signification: 1 si il y a lieu de s'arreter 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Schema_Temps::stop()
{
  return valeur().stop();
}
// Description:
//    Appel a l'objet sous-jacent
//    Renvoie 1 s'il y a lieu de faire une impression.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: code de retour sous-jacent
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Schema_Temps::impr(Sortie& os) const
{
  return valeur().impr(os);
}
// Description:
//    Appel a l'objet sous-jacent
//    Imprime le schema en temp sur un flot de sortie (si il y a lieu).
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline void Schema_Temps::imprimer(Sortie& os) const
{
  valeur().imprimer(os);
}
// Description
//    Appel a l'objet sous-jacent:
//    Renvoie 1 s'il y a lieu de faire une sauvegarde
//    0 sinon.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si il faut faire une sauvegarde 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Schema_Temps::lsauv() const
{
  return valeur().lsauv();
}
#endif
