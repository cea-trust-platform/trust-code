/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_Fonc.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Fonc_included
#define Champ_Fonc_included




#include <Champ_Fonc_base.h>


Declare_deriv(Champ_Fonc_base);

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_Fonc
//     Classe generique de la hierarchie des champs fontions du temps,
//     un objet Champ_Fonc peut referencer n'importe quel objet derivant de
//     Champ_Fonc_base.
//     La plupart des methodes appellent les methodes de l'objet Champ_Fonc
//     sous-jacent via la methode valeur() declaree grace a la macro
// .SECTION voir aussi
//     Champ_Fonc_base
//////////////////////////////////////////////////////////////////////////////
class Champ_Fonc : public DERIV(Champ_Fonc_base), public Champ_Proto
{

  Declare_instanciable(Champ_Fonc);

public :

//  inline Champ_Fonc(const Champ_Fonc_base& ) ;
  Champ_Fonc(const Champ_Fonc&) = default;
  inline Champ_Fonc& operator=(const Champ_Fonc_base& ) ;
  inline Champ_Fonc& operator=(const Champ_Fonc& ) ;
  inline void fixer_nb_valeurs_nodales(int ) ;
  inline int nb_valeurs_nodales() const ;
  inline DoubleTab& valeurs() ;
  inline const DoubleTab& valeurs() const ;
  inline operator DoubleTab& () ;
  inline operator const DoubleTab& () const ;
  inline double temps() const ;
  inline double changer_temps(const double& t) ;
  inline void mettre_a_jour(double un_temps) ;
  inline int initialiser(const double& un_temps);
  inline void associer_zone_dis_base(const Zone_dis_base&) ;
  inline const Zone_dis_base& zone_dis_base() const ;
  inline int reprendre(Entree& ) ;
  inline int sauvegarder(Sortie& ) const;
  inline DoubleVect& valeur_a_elem(const DoubleVect& position,
                                   DoubleVect& les_valeurs,
                                   int le_poly) const ;
  inline virtual double valeur_a_elem_compo(const DoubleVect& position,
                                            int le_poly,int ncomp) const ;
  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleTab& les_valeurs) const;
  inline DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const;
  inline DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                              DoubleVect&, int) const;
  inline void nommer(const Nom& nom)
  {
    valeur().nommer(nom);
  };
};

// Description:
//    Constructeur par copie d'un Champ_Fonc_base
//    Simple appel au constructeur DERIV(Champ_Fonc_base)(x)
// Precondition:
// Parametre: Champ_Fonc_base& x
//    Signification: le champ a copier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
/*
inline Champ_Fonc::Champ_Fonc(const Champ_Fonc_base& x)

 : DERIV(Champ_Fonc_base)(x) {}
*/


// Description:
//    Appel a l'objet sous-jacent
//    Fixe le nombre de degres de liberte par composante
// Precondition:
// Parametre: int n
//    Signification: le nombre de degres de liberte par comp
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Champ_Fonc::fixer_nb_valeurs_nodales(int n)
{
  valeur().fixer_nb_valeurs_nodales(n);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le nombre de degres de liberte par composante
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de degres de liberte par composante
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Champ_Fonc::nb_valeurs_nodales() const
{
  return valeur().nb_valeurs_nodales();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des valeurs du champ
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
inline DoubleTab& Champ_Fonc::valeurs()
{
  return valeur().valeurs();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le tableau des valeurs du champ
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
inline const DoubleTab& Champ_Fonc::valeurs() const
{
  return valeur().valeurs();
}


// Description:
//    Operateur de cast d'un Champ_Fonc en un DoubleTab&
//    Renvoie le tableau des valeurs du champ
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
inline Champ_Fonc::operator DoubleTab& ()
{
  return valeur().valeurs();
}


// Description:
//    Operateur de cast d'un Champ_Fonc en un DoubleTab&
//    Renvoie le tableau des valeurs du champ
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
inline Champ_Fonc::operator const DoubleTab& () const
{
  return valeur().valeurs();
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoie le temps courant du champ
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le temps courant du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Champ_Fonc::temps() const
{
  return valeur().temps();
}


// Description:
//    Appel a l'objet sous-jacent
//    Change le temps courant du champ, et le renvoie.
// Precondition:
// Parametre: double& t
//    Signification: le nouveau temps courant du champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: double
//    Signification: le nouveau temps courant du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double Champ_Fonc::changer_temps(const double& t)
{
  return valeur().changer_temps(t);
}


// Description:
//    Appel a l'objet sous-jacent
//    Associe une zone discretisee au champ.
// Precondition:
// Parametre: Zone_dis_base& zone_dis
//    Signification: la zone discretisee a associer au champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ a une zone discretisee associee
inline void Champ_Fonc::associer_zone_dis_base(const Zone_dis_base& zone_dis)
{
  valeur().associer_zone_dis_base(zone_dis);
}


// Description:
//    Appel a l'objet sous-jacent
//    Renvoe la zone discretisee associee
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone_dis_base&
//    Signification: la zone discretisee associee
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Zone_dis_base& Champ_Fonc::zone_dis_base() const
{
  return valeur().zone_dis_base();
}


// Description:
//    Appel a l'objet sous-jacent
//    Effectue une mise a jour en temps
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Champ_Fonc::mettre_a_jour(double un_temps)
{
  valeur().mettre_a_jour(un_temps);
}


// Description:
//    Appel a l'objet sous-jacent
//    Initilialise le champ (si il y lieu)
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
inline int Champ_Fonc::initialiser(const double& un_temps)
{
  return valeur().initialiser(un_temps);
}


// Description:
//    Operateur d'affectation d'un Champ_Fonc_base dans
//    un  Champ_Fonc.
// Precondition:
// Parametre: Champ_Fonc_base& ch_fonc_base
//    Signification: la partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_Fonc&
//    Signification: le resultat de l'affectation (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Fonc& Champ_Fonc::operator=(const Champ_Fonc_base& ch_fonc_base)
{
  DERIV(Champ_Fonc_base)::operator=(ch_fonc_base);
  return *this;
}

// Description:
//    Operateur d'affectation d'un Champ_Fonc_base dans
//    un  Champ_Fonc.
// Precondition:
// Parametre: Champ_Fonc_base& ch_fonc_base
//    Signification: la partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_Fonc&
//    Signification: le resultat de l'affectation (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Fonc& Champ_Fonc::operator=(const Champ_Fonc& ch_fonc)
{
  if (ch_fonc.non_nul()) DERIV(Champ_Fonc_base)::operator=(ch_fonc.valeur());
  return *this;
}


// Description:
//    Appel a l'objet sous-jacent
//    Postraite au format lml.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: Motcle& loc_post
//    Signification: la localisation du postraitement, "sommet" ou "constant par element"
//    Valeurs par defaut:
//    Contraintes: reference constante, valeurs prevues: "som","elem"
//    Acces: entree
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Champ_Fonc::reprendre(Entree& fich)
{
  return valeur().reprendre(fich);
}


// Description:
//    Appel a l'objet sous-jacent
//    Effectue une sauvegarde sur un flot de sortie
// Precondition:
// Parametre: Sortie& fich
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Champ_Fonc::sauvegarder(Sortie& fich) const
{
  return valeur().sauvegarder(fich);
}


inline DoubleVect& Champ_Fonc::valeur_a_elem(const DoubleVect& position,
                                             DoubleVect& les_valeurs,
                                             int le_poly) const
{
  return valeur().valeur_a_elem(position, les_valeurs, le_poly);
}
inline  double Champ_Fonc::valeur_a_elem_compo(const DoubleVect& position,
                                               int le_poly,int ncomp) const
{
  return valeur().valeur_a_elem_compo(position, le_poly, ncomp);
}
inline DoubleTab& Champ_Fonc::valeur_aux_elems(const DoubleTab& positions,
                                               const IntVect& les_polys,
                                               DoubleTab& les_valeurs) const
{
  return valeur().valeur_aux_elems(positions, les_polys, les_valeurs);
}
inline DoubleTab& Champ_Fonc::valeur_aux_sommets(const Domaine& dom, DoubleTab& les_valeurs) const
{
  return valeur().valeur_aux_sommets(dom, les_valeurs);
}
inline DoubleVect& Champ_Fonc::valeur_aux_sommets_compo(const Domaine& dom,
                                                        DoubleVect& les_valeurs, int compo) const
{
  return valeur().valeur_aux_sommets_compo(dom, les_valeurs, compo);
}

#endif
