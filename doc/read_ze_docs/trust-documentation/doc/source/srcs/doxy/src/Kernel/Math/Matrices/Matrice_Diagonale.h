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

#ifndef Matrice_Diagonale_included
#define Matrice_Diagonale_included

#include <TRUSTTabs_forward.h>
#include <Matrice_Base.h>

/*! @brief Classe Matrice_Diagonale Represente une matrice diagonale.
 *
 * @sa Matrice_Base
 */

class Matrice_Diagonale : public Matrice_Base
{
  Declare_instanciable_sans_constructeur(Matrice_Diagonale);
public :
  int ordre( void ) const override;
  int nb_lignes( void ) const override;
  int nb_colonnes( void ) const override;

  DoubleVect& ajouter_multvect_(const DoubleVect& x, DoubleVect& r) const override;
  DoubleVect& ajouter_multvectT_(const DoubleVect& x, DoubleVect& r) const override;
  DoubleTab& ajouter_multTab_(const DoubleTab& x, DoubleTab& r) const override;

  void scale(const double x) override;
  void get_stencil(IntTab& stencil) const override;
  void get_symmetric_stencil(IntTab& stencil) const override;
  void get_stencil_and_coefficients(IntTab& stencil, ArrOfDouble& coefficients) const override;
  void get_symmetric_stencil_and_coefficients(IntTab& stencil, ArrOfDouble& coefficients) const override;

  Matrice_Diagonale(void);
  Matrice_Diagonale(int size);
  Matrice_Diagonale(const DoubleVect& coefficients);

  DoubleVect& get_coefficients(void);
  const DoubleVect& get_coefficients(void) const;

  void dimensionner(int size);

  double coeff(int i, int j) const;
  double& coeff(int i, int j);

protected :
  DoubleVect coefficients_;
};

#endif
