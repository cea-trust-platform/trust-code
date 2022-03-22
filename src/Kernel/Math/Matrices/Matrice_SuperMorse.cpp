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
// File:        Matrice_SuperMorse.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/Matrices
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////
#include <Matrice_SuperMorse.h>

// Description:
//   Calcul de "resu += MATRICE * x" et d'un produit scalaire
//   (c'est une brique de base pour le gradient conjugue, voir class Solv_GCP)
// Valeur de retour:
//   partie locale a ce processeur de "(MATRICE * x) scalaire x"
//   (attention: le produit scalaire compte toutes les lignes de la matrice
//     les items communs ne sont pas supprimes !)
//   (attention: c'est different de resu scalaire x !)
double Matrice_SuperMorse::ajouter_mult_vect_et_prodscal(const DoubleVect& x, DoubleVect& resu) const
{
  assert(resu.size() == x.size());

  const int nb_lignes = lignes_non_vides_.size_array();
  const int *tab_lignes = lignes_non_vides_.addr();
  // Le premier indice de tab1_ qui va nous interesser est le deuxieme du tableau
  // (le premier du tableau est forcement 1)
  assert(tab1_[0] == 1);
  const int *tab1_ptr = tab1_.addr() + 1;
  assert(tab1_.size_array() == nb_lignes + 1);
  const int *tab2_ptr = tab2_.addr();
  const double *coeff_ptr = coeff_.addr();
  const double *x_ptr = x.addr() - 1; // decalage de 1 car on va indexer par des indices fortran
  double *resu_ptr = resu.addr() - 1; // idem
  int n = 1; // indice du coeff courant dans tab2 et coeff

  double prod_scal_local = 0;

  for (int i = 0; i < nb_lignes; i++)
    {
      // Numero de la ligne de la matrice:
      const int i_ligne = *(tab_lignes++);
      assert(i_ligne >= 1 && i_ligne <= resu.size_array());
      double r = 0.;
      // Indice de fin des coeffs de cette ligne dans tab2 et coeff
      const int n_fin = *(tab1_ptr++);
      for (; n < n_fin; n++)
        {
          const int colonne = *(tab2_ptr++);
          assert(colonne >= 1 && colonne <= x.size_array());
          const double coef = *(coeff_ptr++);
          const double xb = x_ptr[colonne];
          r += coef * xb;
        }
      resu_ptr[i_ligne] += r;
      // attention: on ne veut calculer que le produit scalaire entre x et la partie ajoutee a resu
      //  (pas avec les valeurs existantes de resu avant l'appel)
      prod_scal_local += r * x_ptr[i_ligne];
    }
  return prod_scal_local;
}
