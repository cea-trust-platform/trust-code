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

#ifndef Matrice_Morse_Sym_included
#define Matrice_Morse_Sym_included

#include <Matrice_Morse.h>
#include <Matrice_Sym.h>
#include <Matrice.h>


/*! @brief Classe Matrice_Morse_Sym Represente une matrice M (creuse) symetrique stockee au format Morse
 *
 *     Symetrique: on ne stocke que la partie triangulaire superieure de la
 *     matrice. Important: Tous les coefficients diagonaux doivent etre stockes
 *     meme s'ils sont nuls (hypothese TRUST pour plusieurs methodes dont multvect).
 *     Cette classe derive de Matrice_Morse et Matrice_Sym.
 *
 * @sa Matrice_Morse Matrice_Sym
 */
class Matrice_Morse_Sym : public Matrice_Morse, public Matrice_Sym
{

  Declare_instanciable_sans_constructeur(Matrice_Morse_Sym);

public :

  // constructeurs :
  Matrice_Morse_Sym(int n1=1,int n2=1) : Matrice_Morse(n1,n2)
  {
    symetrique_=1;
  }
  Matrice_Morse_Sym(int ,int ,const IntLists& ,const DoubleLists& ,const DoubleVect& );
  Matrice_Morse_Sym(const Matrice& ) ;
  Matrice_Morse_Sym(const Matrice_Morse& ) ;
  Matrice_Morse_Sym(const Matrice_Morse_Sym& ) ;

  // operateurs :
  Matrice_Morse_Sym& operator=(const Matrice_Morse_Sym& );
  Matrice_Morse_Sym& operator=(const Matrice_Morse& );
  Matrice_Morse_Sym& operator=(const Matrice& );
  friend Matrice_Morse_Sym operator +(const Matrice_Morse_Sym&, const Matrice_Morse_Sym& );
  friend Matrice_Morse_Sym operator *(const Matrice_Morse_Sym&, double );
  friend Matrice_Morse_Sym operator *(double, const Matrice_Morse_Sym& );
  Matrice_Morse_Sym& operator *=(double );
  void scale( const double x ) override;

  void get_stencil(IntTab& stencil) const override;

  void get_symmetric_stencil(IntTab& stencil) const override;

  void get_stencil_and_coefficients(IntTab& stencil, ArrOfDouble& coefficients) const override;

  void get_symmetric_stencil_and_coefficients(IntTab& stencil, ArrOfDouble& coefficients) const override;

  Matrice_Morse_Sym  operator -() const;

  DoubleTab&  ajouter_multTab_(const DoubleTab&, DoubleTab& ) const override;
  DoubleVect& ajouter_multvect_(const DoubleVect&, DoubleVect& ) const override;
  DoubleVect& ajouter_multvectT_(const DoubleVect&, DoubleVect& ) const override;

  virtual double multvect_et_prodscal(const DoubleVect&, DoubleVect& ) const ;
  int inverse(const DoubleVect&, DoubleVect&, double ) const  override;
  int inverse(const DoubleVect&, DoubleVect&, double, int ) const  override;

  Sortie& imprimer_formatte(Sortie& s) const override;
  void compacte(int elim_coeff_nul=0);
  void renumerote() const;                       // Creation d'une matrice renumerotee pour reduire la largeur de bande
  Matrice& matrice_renumerotee() { return matrice_renumerotee_; }// Renvoie la matrice renumerotee
  Matrice& matrice_renumerotee() const { return matrice_renumerotee_; }// Renvoie la matrice renumerotee
  inline ArrOfInt& permutation() { return permutation_; }
  inline ArrOfInt& permutation_inverse() { return permutation_inverse_; }
  inline ArrOfInt& permutation() const { return permutation_; }
  inline ArrOfInt& permutation_inverse() const { return permutation_inverse_; }

  bool check_symmetric_morse_matrix_structure( void ) const;
  bool check_sorted_symmetric_morse_matrix_structure( void ) const;
  void assert_check_symmetric_morse_matrix_structure( void ) const;
  void assert_check_sorted_symmetric_morse_matrix_structure( void ) const;

private :
  int isLU() const { return 0; }
  mutable ArrOfInt permutation_;
  mutable ArrOfInt permutation_inverse_;
  mutable Matrice matrice_renumerotee_;
};

int Matrice_Morse_Sym_test();

Matrice_Morse operator +(const Matrice_Morse&, const Matrice_Morse& );
Matrice_Morse operator *(double, const Matrice_Morse& );
Matrice_Morse_Sym operator +(const Matrice_Morse_Sym&, const Matrice_Morse_Sym& );
Matrice_Morse_Sym operator *(const Matrice_Morse_Sym&, double );
Matrice_Morse_Sym operator *(double, const Matrice_Morse_Sym& );
#endif
