/****************************************************************************
* Copyright (c) 2024, CEA
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
                                      int imax, int jmax, int kmax, ArrOfInt& virt_blocs)
{
  const int ni = renum_.dimension(2) - 2;
  const int nj = renum_.dimension(1) - 2;
  const int nk = renum_.dimension(0) - 2;
  if (pe == Process::me())
    {
      // Frontiere periodique avec cote oppose sur meme processeur
      for (int k = kmin; k < kmax; k++)
        for (int j = jmin; j < jmax; j++)
          for (int i = imin; i < imax; i++)
            {
              int jj ;
              int kk ;
              int ii = (i + ni ) % ni;
              jj = (j + nj) % nj;
              kk = (k + nk) % nk;
              renum(i, j, k) = renum(ii, jj, kk);
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
                renum(i,j,k) = index;
              }
            }

      virt_blocs.append_array(count); // end of virtual bloc of data
    }
}

void Matrice_Grossiere::add_dist_bloc(int pe, int imin, int jmin, int kmin,
                                      int imax, int jmax, int kmax, ArrOfInt& items_to_send)
{
  if (pe == Process::me())
    return;

  for (int k = kmin; k < kmax; k++)
    for (int j = jmin; j < jmax; j++)
      for (int i = imin; i < imax; i++)
        {
          int index=renum(i, j, k);
          assert(index >= 0);
          items_to_send.append_array(index);
        }
}

void Matrice_Grossiere::interpolation_for_shear_periodicity(const int i, const int send_i /*offset2*/, const double istmp/*istmp*/,
                                                            const int real_size_i /*ni*/, const double shear_perio)
{
  // renvoi la valeur interpolee pour la condition de shear-periodicity
  int nb_points = order_interpolation_poisson_solver_+1;
  ArrOfInt x;
  ArrOfDouble a;
  x.resize_array(nb_points);
  a.resize_array(nb_points);

  if (nb_points==2)
    {
      x[0] = (int) floor(istmp);
      x[1] = (int) floor(istmp)+1;
    }
  else if(nb_points==3)
    {
      x[0] = send_i-1;
      x[1] = send_i;
      x[2] = send_i+1;
    }
  else if(nb_points==5)
    {
      x[0] = send_i-2;
      x[1] = send_i-1;
      x[2] = send_i;
      x[3] = send_i+1;
      x[4] = send_i+2;
    }
  else if(nb_points==7)
    {
      x[0] = send_i-3;
      x[1] = send_i-2;
      x[2] = send_i-1;
      x[3] = send_i;
      x[4] = send_i+1;
      x[5] = send_i+2;
      x[6] = send_i+3;
    }


  for (int pt = 0; pt < nb_points ; pt++)
    {
      double denum = 1.;
      for (int pt_autre = 0; pt_autre < nb_points ; pt_autre++)
        {
          if (pt_autre!=pt)
            denum *= (x[pt] - x[pt_autre]);
        }
      a[pt]=1./denum;
    }


  for (int pt = 0; pt < nb_points ; pt++)
    {
      if (shear_perio>0.)
        {
          ponderation_shear_p_[pt] = a[pt];
        }
      else
        {
          ponderation_shear_m_[pt] = a[pt];
        }
      for (int pt_autre = 0; pt_autre < nb_points ; pt_autre++)
        {
          if (pt_autre!=pt)
            {
              if (shear_perio>0.)
                {
                  ponderation_shear_p_[pt] *= (istmp - x[pt_autre]);
                }
              else
                {
                  ponderation_shear_m_[pt] *= (istmp - x[pt_autre]);
                }
            }
        }
    }

  for (int pt = 0; pt < nb_points ; pt++)
    {
      if (shear_perio>0.)
        {
          ii_p_[pt] = (i + real_size_i + x[pt] % real_size_i + real_size_i) % real_size_i;
        }
      else if (shear_perio<0.)
        {
          ii_m_[pt] = (i + real_size_i + x[pt] % real_size_i + real_size_i) % real_size_i;
        }
    }

  return;

}


/*! @brief ajoute deux coefficients diagonal/extra-diagonal a la matrice
 *
 */
void Matrice_Grossiere::ajoute_coeff(int i, int j, int k,
                                     int i_voisin, int j_voisin, int k_voisin,
                                     const double coeff, IJK_Splitting splitting, const double shear_perio)
{
  int indice=renum(i, j, k);
  int indice_voisin = renum(i_voisin, j_voisin, k_voisin);

  bool voisin_shear ;
  if (shear_perio == 0.)
    {
      voisin_shear = false;
    }
  else
    {
      voisin_shear = true;
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

          if(voisin_shear)
            {
              if(shear_perio>0.)
                {
                  for (int interp = 0; interp < order_interpolation_poisson_solver_+1; interp++)
                    {
                      voisins_[indice].add(indice_voisin - i + ii_p_[interp]);
                      coeffs_[indice].add(x*ponderation_shear_p_[interp]);
                    }
                }
              else if (shear_perio<0.)
                {
                  for (int interp = 0; interp < order_interpolation_poisson_solver_+1; interp++)
                    {
                      voisins_[indice].add(indice_voisin - i + ii_m_[interp]);
                      coeffs_[indice].add(x*ponderation_shear_m_[interp]);
                    }
                }
            }
          else
            {
              voisins_[indice].add(indice_voisin);
              coeffs_[indice].add(x);
            }
        }
      else
        {
          // Element virtuel

          if(voisin_shear)
            {
              if(shear_perio>0.)
                {
                  for (int interp = 0; interp < order_interpolation_poisson_solver_+1; interp++)
                    {
                      voisins_virt_[indice].add(indice_voisin - i + ii_p_[interp] - nreels);
                      coeffs_virt_[indice].add(x*ponderation_shear_p_[interp]);
                    }
                }
              else if(shear_perio<0.)
                {
                  for (int interp = 0; interp < order_interpolation_poisson_solver_+1; interp++)
                    {
                      voisins_virt_[indice].add(indice_voisin - i + ii_m_[interp] - nreels);
                      coeffs_virt_[indice].add(x*ponderation_shear_m_[interp]);
                    }
                }
            }
          else
            {
              voisins_virt_[indice].add(indice_voisin - nreels);
              coeffs_virt_[indice].add(x);
            }

        }
    }

  // Contribution a la diagonale
  coeff_diag_[indice] -= x;
}

void Matrice_Grossiere::ajoute_coeff(int i, int j, int k,
                                     int i_voisin, int j_voisin, int k_voisin,
                                     const double coeff, IJK_Splitting splitting)
{
  int indice=renum(i, j, k);
  int indice_voisin = renum(i_voisin, j_voisin, k_voisin);

  // coefficient extra diagonal (- surface_face / distance_centres_elements)
  double x = -coeff;
  if (indice_voisin > indice)
    {
      // Coefficient dans la partie triangulaire superieure, a stocker.
      const int nreels = coeff_diag_.size_array();
      if (indice_voisin < nreels)
        {
          voisins_[indice].add(indice_voisin);
          coeffs_[indice].add(x);
        }
      else
        {
          voisins_virt_[indice].add(indice_voisin - nreels);
          coeffs_virt_[indice].add(x);
        }
    }

  // Contribution a la diagonale
  coeff_diag_[indice] -= x;
}




