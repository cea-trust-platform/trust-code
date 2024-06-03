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

#ifndef Matrice_Grossiere_included
#define Matrice_Grossiere_included

#include <TRUSTTab.h>
#include <TRUSTLists.h>
#include <TRUSTLists.h>
#include <TRUSTTab.h>
#include <MD_Vector_std.h>
#include <Matrice_Bloc.h>
#include <IJK_Field.h>

class Matrice_Grossiere
{
public:

  template <typename _TYPE_, typename _TYPE_ARRAY_>
  void build_matrix(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& coeffs_face);

  const int& renum(int i, int j, int k) const
  {
    return renum_(k+1, j+1, i+1);
  }
  const int& renum_m2(int i, int j, int k) const
  {
    return renum_m2_(k+1, j+1, i+1);
  }
  const int& renum_m1(int i, int j, int k) const
  {
    return renum_m1_(k+1, j+1, i+1);
  }
  const int& renum_p1(int i, int j, int k) const
  {
    return renum_p1_(k+1, j+1, i+1);
  }
  const int& renum_p2(int i, int j, int k) const
  {
    return renum_p2_(k+1, j+1, i+1);
  }

  const Matrice_Base& matrice() const
  {
    return mat_;
  }
  const MD_Vector& md_vector() const
  {
    return md_;
  }

protected:
  void ajoute_coeff(int i, int j, int k,
                    int i_voisin, int j_voisin, int k_voisin,
                    const double coeff, const int shear_perio = 0);

  int& renum(int i, int j, int k)
  {
    return renum_(k+1, j+1, i+1);
  }
  int& renum_m2(int i, int j, int k)
  {
    return renum_m2_(k+1, j+1, i+1);
  }
  int& renum_m1(int i, int j, int k)
  {
    return renum_m1_(k+1, j+1, i+1);
  }
  int& renum_p1(int i, int j, int k)
  {
    return renum_p1_(k+1, j+1, i+1);
  }
  int& renum_p2(int i, int j, int k)
  {
    return renum_p2_(k+1, j+1, i+1);
  }



  void add_virt_bloc(int pe, int& count, int imin, int jmin, int kmin,
                     int imax, int jmax, int kmax,ArrOfInt& virt_blocs,
                     IJK_Splitting splitting, double offset = 0.);
  void add_dist_bloc(int pe, int imin, int jmin, int kmin,
                     int imax, int jmax, int kmax,
                     ArrOfInt& items_to_send,
                     IJK_Splitting splitting, double offset = 0.);

  Matrice_Bloc mat_;

  MD_Vector md_p1_;
  MD_Vector md_;
  MD_Vector md_m1_;

  // renum_(k+1,j+1,i+1) = indice de l'inconnue dans le vecteur inconnue de la matrice

  int interpolation_order_ = 2;
  IntTab renum_;
  IntTab renum_m2_;
  IntTab renum_m1_;
  IntTab renum_p1_;
  IntTab renum_p2_;

  double ponderation_shear_m2_scal_;
  double ponderation_shear_m1_scal_;
  double ponderation_shear_0_scal_;
  double ponderation_shear_p1_scal_;
  double ponderation_shear_p2_scal_;

  IntLists voisins_;
  DoubleLists coeffs_;
  DoubleVect coeff_diag_;

  IntLists voisins_virt_;
  DoubleLists coeffs_virt_;
};

#include <Matrice_Grossiere.tpp>
#endif
