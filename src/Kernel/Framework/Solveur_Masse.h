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

#ifndef Solveur_Masse_included
#define Solveur_Masse_included

#include <Solveur_Masse_base.h>
#include <TRUST_Deriv.h>

class Matrice_Base;

/*! @brief classe Solveur_Masse Classe generique de la hierarchie des solveurs de masse, un objet
 *
 *      Solveur_Masse peut referencer n'importe quel d'objet derivant de
 *      Solveur_Masse_base.
 *      La plupart des methodes appellent les methodes de l'objet Solveur_Masse
 *      sous-jacent via la methode valeur() declaree grace a la macro
 *      Contrairement a la classe de base (Solveur_Masse_base) Solveur_Masse
 *      contient une reference vers l'equation associee car il herite de MorEqn.
 *
 * @sa MorEqn Solveur_Masse_base
 */
class Solveur_Masse : public DERIV(Solveur_Masse_base), public MorEqn
{
  Declare_instanciable(Solveur_Masse);
public :
  inline DoubleTab& appliquer(DoubleTab&) const;
  inline void mettre_a_jour(double temps);
  inline void assembler();
  inline void typer(const Nom& );
  virtual void typer();
  inline Matrice_Base& ajouter_masse(double , Matrice_Base&, int penalisation=1) const;
  inline DoubleTab& ajouter_masse(double, DoubleTab& , const DoubleTab& y, int penalisation=1) const;
  inline DoubleTab& corriger_solution( DoubleTab& x, const DoubleTab& y, int incr = 0) const;
  inline Matrice_Base& ajouter_masse_dt_local(DoubleVect& , Matrice_Base&, int penalisation=1) const;
  inline DoubleTab& ajouter_masse_dt_local(DoubleVect&, DoubleTab& , const DoubleTab& y, int penalisation=1) const;
  inline void get_masse_dt_local(DoubleVect& m_dt_locaux, DoubleVect& dt_locaux, int penalisation=1);
  inline void get_masse_divide_by_local_dt(DoubleVect& m_dt_locaux, DoubleVect& dt_locaux, int penalisation=1);
};

/*! @brief Appel a l'objet sous-jacent.
 *
 * Resoud le systeme lineaire, dont la matrice est la matrice de masse.
 *     Le second membre du systeme lineaire a resoudre est passe en parametre.
 *     Le resultat est stocke dans ce meme parametre et renvoye par la methode.
 *     Operation  xx = M^{-1} * xx
 *
 * @param (DoubleTab& xx) le second membre du systeme lineraire a resoudre en entree la solution en sortie.
 * @return (DoubleTab&) la solution M^{-1} * xx
 */
inline DoubleTab& Solveur_Masse::appliquer(DoubleTab& xx) const
{
  return valeur().appliquer(xx);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Mise a jour en temps du solveur de masse.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
inline void Solveur_Masse::mettre_a_jour(double temps)
{
  valeur().mettre_a_jour(temps);
}


/*! @brief Appel a l'objet sous-jacent.
 *
 * Assemblage de la matrice de masse.
 *
 */
inline void Solveur_Masse::assembler()
{
  valeur().assembler();
}


/*! @brief Construit le bon type de Solveur de masse.
 *
 * @param (Nom& typ) le nom du type a construire
 */
inline void Solveur_Masse::typer(const Nom& typ)
{
  DERIV(Solveur_Masse_base)::typer(typ);
}

inline Matrice_Base& Solveur_Masse::ajouter_masse(double dt , Matrice_Base& matrice, int penalisation) const
{
  return valeur().ajouter_masse(dt,matrice,penalisation);
}

inline DoubleTab& Solveur_Masse::ajouter_masse(double dt, DoubleTab& x, const DoubleTab& y, int penalisation) const
{
  return valeur().ajouter_masse(dt,x,y,penalisation);
}

inline Matrice_Base& Solveur_Masse::ajouter_masse_dt_local(DoubleVect& dt_locaux, Matrice_Base& matrice, int penalisation) const
{
  return valeur().ajouter_masse_dt_local(dt_locaux,matrice,penalisation);
}

inline DoubleTab& Solveur_Masse::ajouter_masse_dt_local(DoubleVect& dt_locaux, DoubleTab& x, const DoubleTab& y, int penalisation) const
{
  return valeur().ajouter_masse_dt_local(dt_locaux,x,y,penalisation);
}
inline void Solveur_Masse::get_masse_dt_local(DoubleVect& m_dt_locaux, DoubleVect& dt_locaux, int penalisation)
{
  valeur().get_masse_dt_local(m_dt_locaux,dt_locaux,penalisation);
}
inline void Solveur_Masse::get_masse_divide_by_local_dt(DoubleVect& m_dt_locaux, DoubleVect& dt_locaux, int penalisation)
{
  valeur().get_masse_divide_by_local_dt(m_dt_locaux,dt_locaux,penalisation);
}
inline DoubleTab& Solveur_Masse::corriger_solution( DoubleTab& x, const DoubleTab& y, int incr) const
{
  return valeur().corriger_solution(x,y, incr);
}

#endif
