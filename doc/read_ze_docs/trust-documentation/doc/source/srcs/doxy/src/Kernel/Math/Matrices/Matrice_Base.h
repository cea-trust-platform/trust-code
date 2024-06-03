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

#ifndef Matrice_Base_included
#define Matrice_Base_included

#include <TRUSTTab.h>

/*! @brief Classe Matrice_Base Classe de base de la hierarchie des matrices.
 *
 *     Cette classe definie l'interface generique d'une matrice telle
 *     qu'elle est utilisee dans Trio-U. En consequence elle n'est pas
 *     instanciable. Toutes les types de matrices devront deriver de cette
 *     classe de base et implementer ses methodes abstraites.
 *
 *     Dans les commentaires des methodes A represente un objet Matrice_base.
 *
 * @sa Classe abstraite, Methodes abstraites:, DoubleVect& multvect_(const DoubleVect&, DoubleVect& ) const, int ordre() const
 */

class Matrice_Base : public Objet_U
{

  Declare_base(Matrice_Base);

public :
  virtual int ordre() const =0;
  virtual int nb_lignes() const =0;
  virtual int nb_colonnes() const =0;

  // Methodes pour le calcul de r+=Ax codees dans les classes filles
  virtual DoubleVect& ajouter_multvect_(const DoubleVect& x, DoubleVect& r) const =0;
  virtual DoubleVect& ajouter_multvectT_(const DoubleVect& x, DoubleVect& r) const =0;
  virtual DoubleTab& ajouter_multTab_(const DoubleTab& x, DoubleTab& r) const =0;
  // Methodes pour le calcul de r+=Ax avec echange_espace_virtuel() codees dans Matrice_Base
  virtual inline DoubleVect& ajouter_multvect(const DoubleVect& x, DoubleVect& r) const;
  virtual inline DoubleVect& ajouter_multvectT(const DoubleVect& x, DoubleVect& r) const;
  virtual inline DoubleTab& ajouter_multTab(const DoubleTab& , DoubleTab& r) const;
  // Methodes pour le calcul de r=Ax codees dans Matrice_Base
  virtual inline DoubleVect& multvect_(const DoubleVect&, DoubleVect& ) const;
  virtual inline DoubleVect& multvect(const DoubleVect&, DoubleVect& ) const;
  virtual inline DoubleVect& multvectT_(const DoubleVect&, DoubleVect& ) const;
  virtual inline DoubleVect& multvectT(const DoubleVect&, DoubleVect& ) const;
  virtual inline DoubleTab& multTab(const DoubleTab& , DoubleTab& r) const;
  // Methodes d'impression d'une matrice
  virtual inline Sortie& imprimer(Sortie&) const;
  virtual inline Sortie& imprimer_formatte(Sortie& s) const;

  friend DoubleVect operator*(const Matrice_Base&, const DoubleVect&);

  virtual void scale(const double x) =0;

  virtual void get_stencil(IntTab& stencil) const;

  virtual void get_symmetric_stencil(IntTab& stencil) const;

  virtual void get_stencil_and_coefficients(IntTab& stencil, ArrOfDouble& coefficients) const;
  virtual void get_stencil_and_coeff_ptrs(IntTab& stencil, std::vector<const double *>& coeff_ptr) const;

  virtual void get_symmetric_stencil_and_coefficients(IntTab& stencil, ArrOfDouble& coefficients) const;

  int get_stencil_size( void ) const ;
  virtual void build_stencil( void );

  void set_stencil( const IntTab& stencil );

  bool is_stencil_up_to_date( void ) const ;

protected:
  bool is_stencil_up_to_date_ = false;
  IntTab stencil_ ;
};



/*! @brief Multiplication d'un vecteur par la matrice.
 *
 * Operation: r = A*x
 *
 * @param (DoubleVect& x) le vecteur a multiplier
 * @param (DoubleVect& r) le vecteur resultat de l'operation
 * @return (DoubleVect&) le vecteur resultat de l'operation
 */
inline DoubleVect& Matrice_Base::
multvect(const DoubleVect& x, DoubleVect& r) const
{
  multvect_(x,r);
  r.echange_espace_virtuel();
  return r;
}

inline DoubleVect& Matrice_Base::
multvect_(const DoubleVect& x, DoubleVect& r) const
{
  r=0;
  ajouter_multvect_(x,r);
  return r;
}

/*! @brief Multiplication d'un vecteur par la matrice transposee.
 *
 * Operation: r = AT*x
 *
 * @param (DoubleVect& x) le vecteur a multiplier
 * @param (DoubleVect& r) le vecteur resultat de l'operation
 * @return (DoubleVect&) le vecteur resultat de l'operation
 */
inline DoubleVect& Matrice_Base::
multvectT(const DoubleVect& x, DoubleVect& r) const
{
  multvectT_(x,r);
  r.echange_espace_virtuel();
  return r;
}

inline DoubleVect& Matrice_Base::
multvectT_(const DoubleVect& x, DoubleVect& r) const
{
  r=0;
  ajouter_multvectT_(x,r);
  return r;
}

/*! @brief NON CODE Multiplication d'une matrice representee par un tableau par la matrice.
 *
 *     Operation: R = A*X
 *
 * @param (DoubleTab&) la matrice a multiplier
 * @param (DoubleTab& r) la matrice resultat de l'operation
 * @return (DoubleTab&) la matrice resultat de l'operation
 * @throws NON CODE
 */
inline DoubleTab& Matrice_Base::
multTab(const DoubleTab& x, DoubleTab& r) const
{
  r=0;
  ajouter_multTab_(x,r);
  r.echange_espace_virtuel();
  return r;
}


/*! @brief Operation de multiplication-accumulation (saxpy) matrice vecteur.
 *
 * Operation: r = r + A*x
 *
 * @param (DoubleVect& x) le vecteur a multiplier
 * @param (DoubleVect& r) le vecteur resultat de l'operation
 * @return (DoubleVect&) le vecteur resultat de l'operation
 */
inline DoubleVect& Matrice_Base::
ajouter_multvect(const DoubleVect& x, DoubleVect& r) const
{
  ajouter_multvect_(x,r);
  r.echange_espace_virtuel();
  return r;
}

/*! @brief Operation de multiplication-accumulation (saxpy) matrice vecteur.
 *
 * Operation: r = r + A*x
 *
 * @param (DoubleVect& x) le vecteur a multiplier
 * @param (DoubleVect& r) le vecteur resultat de l'operation
 * @return (DoubleVect&) le vecteur resultat de l'operation
 */
inline DoubleVect& Matrice_Base::
ajouter_multvectT(const DoubleVect& x, DoubleVect& r) const
{
  ajouter_multvectT_(x,r);
  r.echange_espace_virtuel();
  return r;
}

/*! @brief NON CODE Operation de multiplication-accumulation (saxpy) matrice matrice
 *
 *     (matrice represente par un tableau)
 *     Operation: R = R + A*X
 *
 * @param (DoubleTab&) la matrice a multiplier
 * @param (DoubleTab& r) la matrice resultat de l'operation
 * @return (DoubleTab&) la matrice resultat de l'operation
 * @throws NON CODE
 */
inline DoubleTab& Matrice_Base::
ajouter_multTab(const DoubleTab& x, DoubleTab& r) const
{
  ajouter_multTab_(x,r);
  r.echange_espace_virtuel();
  return r;
}


/*! @brief Fonction (hors classe) amie de la classe Matrice_Base.
 *
 * Operateur de multiplication: renvoie (A*vect)
 *
 * @param (Matrice_Base& A) la matrice multiplicatrice
 * @param (DoubleVect& vect) le vecteur a multiplier
 * @return (DoubleVect) le vecteur resultat de l'operation
 */
DoubleVect operator * (const Matrice_Base& A, const DoubleVect& vect);


inline Sortie& Matrice_Base::imprimer(Sortie& os) const
{
  return os<<*this;
}

inline Sortie& Matrice_Base::imprimer_formatte(Sortie& os) const
{
  return os<<*this;
}

#endif
