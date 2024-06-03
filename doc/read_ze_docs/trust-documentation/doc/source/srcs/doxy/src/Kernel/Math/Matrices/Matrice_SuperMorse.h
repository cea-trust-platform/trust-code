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
#ifndef Matrice_SuperMorse_included
#define Matrice_SuperMorse_included

#include <Matrice_Base.h>

/*! @brief : Matrice avec un stockage encore plus creux que Matrice_Morse: On stocke uniquement les lignes non vides (economie sur la taille de tab1_,
 *
 *   tab2_ et coeff_ restent identiques)
 *
 */
class Matrice_SuperMorse
{
public :
  const int& tab1(int i) const { return tab1_[i]; } // i de 0 a n
  int& tab1(int i) { return tab1_[i]; }
  const int& tab2(int i) const { return tab2_[i]; } // i de 0 a nnz-1
  int& tab2(int i) { return tab2_[i]; }
  const double& coeff(int i) const { return coeff_[i]; } // i de 0 a nnz-1
  double& coeff(int i) { return coeff_[i]; }

  ArrOfInt& get_set_tab1( void ) { return tab1_ ; }
  ArrOfInt& get_set_tab2( void ) { return tab2_ ; }
  ArrOfDouble& get_set_coeff( void ) { return coeff_ ; }

  const ArrOfInt& get_tab1( void ) const { return tab1_ ; }
  const ArrOfInt& get_tab2( void ) const { return tab2_ ; }
  const ArrOfDouble& get_coeff( void ) const { return coeff_ ; }

  double ajouter_mult_vect_et_prodscal(const DoubleVect& x, DoubleVect& resu) const;
  // Tableau contenant les indices des lignes non vides (indices fortran)
  ArrOfInt lignes_non_vides_;
  // tab1_ est de taille lignes_non_vides_.size_array()+1


protected :
  ArrOfInt tab1_, tab2_;
  ArrOfDouble coeff_;
};

#endif
