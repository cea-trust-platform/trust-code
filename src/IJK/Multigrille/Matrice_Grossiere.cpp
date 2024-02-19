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

#include <Matrice_Grossiere.h>
#include <Matrice_Morse_Sym.h>

void Matrice_Grossiere::add_virt_bloc(int pe, int& count, int imin, int jmin, int kmin,
                                      int imax, int jmax, int kmax, ArrOfInt& virt_blocs, IJK_Splitting splitting, double offset)
{
  const int ni = renum_.dimension(2) - 2;
  if (pe == Process::me())
    {
      // Frontiere periodique avec cote oppose sur meme processeur

      const int nj = renum_.dimension(1) - 2;
      const int nk = renum_.dimension(0) - 2;
      for (int k = kmin; k < kmax; k++)
        for (int j = jmin; j < jmax; j++)
          for (int i = imin; i < imax; i++)
            {
              int ii_m1 ;
              int ii ;
              int jj ;
              int kk ;
              int ii_p1 ;

              if (offset !=0. && IJK_Splitting::defilement_==1)
                {
                  double DX = splitting.get_grid_geometry().get_constant_delta(0);
                  double istmp = IJK_Splitting::shear_x_time_ * offset/DX;
                  int offset2 = (int) round(istmp);
                  //int x[5] = {offset2-2, offset2-1, offset2, offset2+1, offset2+2};
                  int x[3] = {offset2-1, offset2, offset2+1};

                  double a0 = 1. / ((x[0] - x[1]) * (x[0] - x[2]) );
                  double a1 = 1. / ((x[1] - x[0]) * (x[1] - x[2]) );
                  double a2 = 1. / ((x[2] - x[0]) * (x[2] - x[1]) );

                  ponderation_shear_m1_scal_ = a0 * ((istmp - x[1]) * (istmp - x[2]));
                  ponderation_shear_0_scal_ = a1 * ((istmp - x[0]) * (istmp - x[2]));
                  ponderation_shear_p1_scal_ = a2 * ((istmp - x[0]) * (istmp - x[1]));

                  ii_m1 = ((i + ni + offset2 - 1) % ni + ni) % ni;
                  ii = ((i + ni + offset2) % ni + ni) % ni;
                  ii_p1 = ((i + ni + offset2 + 1) % ni + ni) % ni;
                  jj = (j + nj) % nj;
                  kk = (k + nk) % nk;
                  renum(i, j, k) = renum(ii, jj, kk);
                  renum_m1(i, j, k) = renum(ii_m1, jj, kk);
                  renum_p1(i, j, k) = renum(ii_p1, jj, kk);
                }
              else
                {
                  ii_m1 = -1;
                  ii = (i + ni ) % ni;
                  ii_p1 = -1;
                  jj = (j + nj) % nj;
                  kk = (k + nk) % nk;
                  renum(i, j, k) = renum(ii, jj, kk);
                  renum_m1(i, j, k) = -1;
                  renum_p1(i, j, k) = -1;
                }
            }

    }
  else
    {
      // cas ou la frontiere nest pas des deux cotes sur le meme proc...

      virt_blocs.append_array(count);

      for (int k = kmin; k < kmax; k++)
        for (int j = jmin; j < jmax; j++)
          for (int i = imin; i < imax; i++)
            {
              {
                int index = count++;

                if (offset !=0. && IJK_Splitting::defilement_==1)
                  {
                    double DX = splitting.get_grid_geometry().get_constant_delta(0);
                    double istmp = IJK_Splitting::shear_x_time_ * offset/DX;
                    int offset2 = (int) round(istmp);

                    int x[3] = {offset2-1, offset2, offset2+1};

                    double a0 = 1. / ((x[0] - x[1]) * (x[0] - x[2]) );
                    double a1 = 1. / ((x[1] - x[0]) * (x[1] - x[2]) );
                    double a2 = 1. / ((x[2] - x[0]) * (x[2] - x[1]) );

                    ponderation_shear_m1_scal_ = a0 * ((istmp - x[1]) * (istmp - x[2]));
                    ponderation_shear_0_scal_ = a1 * ((istmp - x[0]) * (istmp - x[2]));
                    ponderation_shear_p1_scal_ = a2 * ((istmp - x[0]) * (istmp - x[1]));

                    int ii_m1 = ((i  + offset2 - 1) % ni + ni) % ni;
                    int ii = ((i  + offset2) % ni + ni) % ni;
                    int ii_p1 = ((i  + offset2 + 1) % ni + ni) % ni;
                    renum(i, j, k) = index + ii - i;
                    renum_m1(i, j, k) = index + ii_m1 - i;
                    renum_p1(i, j, k) = index + ii_p1 - i;

                  }
                else
                  {
                    renum(i,j,k) = index;
                    renum_m1(i, j, k) = -1;
                    renum_p1(i, j, k) = -1;
                  }
              }

            }

      virt_blocs.append_array(count); // end of virtual bloc of data
    }
}

void Matrice_Grossiere::add_dist_bloc(int pe, int imin, int jmin, int kmin,
                                      int imax, int jmax, int kmax, ArrOfInt& items_to_send, IJK_Splitting splitting, double offset)
{
  if (pe == Process::me())
    return;


  const int ni = renum_.dimension(2) - 2;
  for (int k = kmin; k < kmax; k++)
    for (int j = jmin; j < jmax; j++)
      for (int i = imin; i < imax; i++)
        {
          if (offset !=0. && IJK_Splitting::defilement_==1)
            {
              double DX = splitting.get_grid_geometry().get_constant_delta(0);
              double istmp = IJK_Splitting::shear_x_time_ * offset/DX;
              int offset2 = (int) round(istmp);

              int x[3] = {offset2-1, offset2, offset2+1};

              double a0 = 1. / ((x[0] - x[1]) * (x[0] - x[2]) );
              double a1 = 1. / ((x[1] - x[0]) * (x[1] - x[2]) );
              double a2 = 1. / ((x[2] - x[0]) * (x[2] - x[1]) );

              ponderation_shear_m1_scal_ = a0 * ((istmp - x[1]) * (istmp - x[2]));
              ponderation_shear_0_scal_ = a1 * ((istmp - x[0]) * (istmp - x[2]));
              ponderation_shear_p1_scal_ = a2 * ((istmp - x[0]) * (istmp - x[1]));

              int ii = ((i  + offset2) % ni + ni) % ni;
              int index = renum(i, j, k) + ii - i;
              assert(index >= 0);
              items_to_send.append_array(index);

            }
          else
            {
              int index = renum(i, j, k);
              assert(index >= 0);
              items_to_send.append_array(index);
            }


        }
}
/*! @brief ajoute deux coefficients diagonal/extra-diagonal a la matrice
 *
 */
void Matrice_Grossiere::ajoute_coeff(int i, int j, int k,
                                     int i_voisin, int j_voisin, int k_voisin,
                                     const double coeff, const int shear_perio)
{
  const int indice = renum(i, j, k);
  const int indice_voisin = renum(i_voisin, j_voisin, k_voisin);
  const int indice_voisin_m1 = renum_m1(i_voisin, j_voisin, k_voisin);
  const int indice_voisin_p1 = renum_p1(i_voisin, j_voisin, k_voisin);

  double pond_0 = 0.;
  double pond_m1 = 0.;
  double pond_p1 = 0.;
  bool voisin_shear;
  if (indice_voisin_m1!=-1 || indice_voisin_p1!=-1)
    {
      if (shear_perio==-1)
        {
          pond_m1 = ponderation_shear_p1_scal_ ;
          pond_0 = ponderation_shear_0_scal_;
          pond_p1 = ponderation_shear_m1_scal_ ;
        }
      else if (shear_perio==1)
        {
          pond_m1 = ponderation_shear_m1_scal_;
          pond_0 = ponderation_shear_0_scal_;
          pond_p1 = ponderation_shear_p1_scal_;
        }
      else
        {
          std::cout << "ne doit pas etre la" << std::endl;
          Process::exit();
        }
      voisin_shear = true;
    }
  else
    {
      pond_m1 = 0.;
      pond_0 = 1.;
      pond_p1 = 0.;
      voisin_shear = false;
    }


  // coefficient extra diagonal (- surface_face / distance_centres_elements)
  double x = -coeff;
  if (indice_voisin > indice)
    {
      // Coefficient dans la partie triangulaire superieure, a stocker.
      const int nreels = coeff_diag_.size_array();
      if (indice_voisin < nreels)
        {
          // Element reel
          voisins_[indice].add(indice_voisin);
          coeffs_[indice].add(x*pond_0);
          if(voisin_shear)
            {
              voisins_[indice].add(indice_voisin_m1);
              voisins_[indice].add(indice_voisin_p1);
              coeffs_[indice].add(x*pond_m1);
              coeffs_[indice].add(x*pond_p1);
            }
        }
      else
        {
          // Element virtuel
          voisins_virt_[indice].add(indice_voisin - nreels);
          coeffs_virt_[indice].add(x*pond_0);
          if(voisin_shear)
            {
              voisins_virt_[indice].add(indice_voisin_m1 - nreels);
              voisins_virt_[indice].add(indice_voisin_p1 - nreels);
              coeffs_virt_[indice].add(x*pond_m1);
              coeffs_virt_[indice].add(x*pond_p1);
            }

        }
    }

  // Contribution a la diagonale
  coeff_diag_[indice] -= x;
}
