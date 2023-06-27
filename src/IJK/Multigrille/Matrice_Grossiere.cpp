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
                                      int imax, int jmax, int kmax, ArrOfInt& virt_blocs_m1, ArrOfInt& virt_blocs, ArrOfInt& virt_blocs_p1,
                                      ArrOfInt& virt_blocs_pond_m1,ArrOfInt& virt_blocs_pond_0,ArrOfInt& virt_blocs_pond_p1,
                                      IJK_Splitting splitting, double offset)
{
  if (pe == Process::me())
    {
      // Frontiere periodique avec cote oppose sur meme processeur
      const int ni = renum_.dimension(2) - 2;
      const int nj = renum_.dimension(1) - 2;
      const int nk = renum_.dimension(0) - 2;
      for (int k = kmin; k < kmax; k++)
        for (int j = jmin; j < jmax; j++)
          for (int i = imin; i < imax; i++)
            {

              //int ii_m2 ;
              int ii_m1 ;
              int ii ;
              int ii_p1 ;
              //int ii_p2 ;
              //double ponderation_m2;
              double ponderation_m1;
              double ponderation_0;
              double ponderation_p1;
              //double ponderation_p2;

              if (offset !=0. && IJK_Splitting::defilement_==1)
                {
                  double DX = splitting.get_grid_geometry().get_constant_delta(0);
                  double istmp = IJK_Splitting::shear_x_time_ * offset/DX;
                  int offset2 = (int) round(istmp);
                  //int x[5] = {offset2-2, offset2-1, offset2, offset2+1, offset2+2};
                  int x[3] = {offset2-1, offset2, offset2+1};

//                  double a0 = 1. / ((x[0] - x[1]) * (x[0] - x[2]) * (x[0] - x[3]) * (x[0] - x[4]));
//                  double a1 = 1. / ((x[1] - x[0]) * (x[1] - x[2]) * (x[1] - x[3]) * (x[1] - x[4]));
//                  double a2 = 1. / ((x[2] - x[0]) * (x[2] - x[1]) * (x[2] - x[3]) * (x[2] - x[4]));
//                  double a3 = 1. / ((x[3] - x[0]) * (x[3] - x[1]) * (x[3] - x[2]) * (x[3] - x[4]));
//                  double a4 = 1. / ((x[4] - x[0]) * (x[4] - x[1]) * (x[4] - x[2]) * (x[4] - x[3]));

                  double a0 = 1. / ((x[0] - x[1]) * (x[0] - x[2]) );
                  double a1 = 1. / ((x[1] - x[0]) * (x[1] - x[2]) );
                  double a2 = 1. / ((x[2] - x[0]) * (x[2] - x[1]) );

//                  ponderation_m2 = a0 * ((istmp - x[1]) * (istmp - x[2]) * (istmp - x[3]) * (istmp - x[4]));
//                  ponderation_m1 = a1 * ((istmp - x[0]) * (istmp - x[2]) * (istmp - x[3]) * (istmp - x[4]));
//                  ponderation_0 = a2 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[3]) * (istmp - x[4]));
//                  ponderation_p1 = a3 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[2]) * (istmp - x[4]));
//                  ponderation_p2 = a4 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[2]) * (istmp - x[3]));

                  ponderation_m1 = a0 * ((istmp - x[1]) * (istmp - x[2]));
                  ponderation_0 = a1 * ((istmp - x[0]) * (istmp - x[2]));
                  ponderation_p1 = a2 * ((istmp - x[0]) * (istmp - x[1]));


//                  ii_m2 = ((i + ni + offset2 - 2) % ni + ni) % ni;
                  ii_m1 = ((i + ni + offset2 - 1) % ni + ni) % ni;
                  ii = ((i + ni + offset2) % ni + ni) % ni;
                  ii_p1 = ((i + ni + offset2 + 1) % ni + ni) % ni;
//                  ii_p2 = ((i + ni + offset2 + 2) % ni + ni) % ni;

                }
              else
                {
//                  ii_m2 = -1;
                  ii_m1 = -1;
                  ii = (i + ni ) % ni;
                  ii_p1 = -1;
//                  ii_p2 = -1;
//                  ponderation_m2 = -1;
                  ponderation_m1 = -1;
                  ponderation_0 = -1;
                  ponderation_p1 = -1;
//                  ponderation_p2 = -1;
                }
              int jj = (j + nj) % nj;
              int kk = (k + nk) % nk;

              renum(i, j, k) = renum(ii, jj, kk);
              if(ii_m1!=-1)
                {
//                  renum_m2(i, j, k) = renum(ii_m2, jj, kk);
                  renum_m1(i, j, k) = renum(ii_m1, jj, kk);
                  renum_p1(i, j, k) = renum(ii_p1, jj, kk);
//                  renum_p2(i, j, k) = renum(ii_p2, jj, kk);

//                  ponderation_shear_m2(i, j, k) = ponderation_m2;
                  // subterfuge un peu moche parce que les methodes apres nexistent que pour des tableau d entier, et pas de double
                  ponderation_shear_m1(i, j, k) = (int)(ponderation_m1*1.e6);
                  ponderation_shear_0(i, j, k) = (int)(ponderation_0*1.e6);
                  ponderation_shear_p1(i, j, k) = (int)(ponderation_p1*1.e6);
//                  ponderation_shear_p2(i, j, k) = ponderation_p2;
                }
              else
                {
                  renum_m1(i, j, k) = -1;
                  renum_p1(i, j, k) = -1;
                  ponderation_shear_m1(i, j, k) = -1;
                  ponderation_shear_0(i, j, k) = -1;
                  ponderation_shear_p1(i, j, k) = -1;
                }
            }

    }
  else
    {
      // cas ou la frontiere nest pas des deux cotes sur le meme proc...
      virt_blocs.set_smart_resize(1);
      virt_blocs.append_array(count);
      virt_blocs_m1.set_smart_resize(1);
      virt_blocs_m1.append_array(count);
      virt_blocs_p1.set_smart_resize(1);
      virt_blocs_p1.append_array(count);
      virt_blocs_pond_m1.set_smart_resize(1);
      virt_blocs_pond_m1.append_array(count);
      virt_blocs_pond_0.set_smart_resize(1);
      virt_blocs_pond_0.append_array(count);
      virt_blocs_pond_p1.set_smart_resize(1);
      virt_blocs_pond_p1.append_array(count);

      for (int k = kmin; k < kmax; k++)
        for (int j = jmin; j < jmax; j++)
          for (int i = imin; i < imax; i++)
            {
              {
                int index = count++;
                renum(i,j,k) = index;
                renum_m1(i, j, k) = index;
                renum_p1(i, j, k) = index;
                ponderation_shear_m1(i, j, k) = index;
                ponderation_shear_0(i, j, k) = index;
                ponderation_shear_p1(i, j, k) = index;

              }

            }

      virt_blocs.append_array(count); // end of virtual bloc of data
      virt_blocs_m1.append_array(count);
      virt_blocs_p1.append_array(count);
      virt_blocs_pond_m1.append_array(count);
      virt_blocs_pond_0.append_array(count);
      virt_blocs_pond_p1.append_array(count);
    }
}

void Matrice_Grossiere::add_dist_bloc(int pe, int imin, int jmin, int kmin,
                                      int imax, int jmax, int kmax,
                                      ArrOfInt& items_to_send_m1,ArrOfInt& items_to_send,ArrOfInt& items_to_send_p1,
                                      ArrOfInt& items_to_send_ponderation_shear_m1,ArrOfInt& items_to_send_ponderation_shear_0,ArrOfInt& items_to_send_ponderation_shear_p1,
                                      IJK_Splitting splitting, double offset)
{
  if (pe == Process::me())
    return;

  items_to_send.set_smart_resize(1);
  items_to_send_m1.set_smart_resize(1);
  items_to_send_p1.set_smart_resize(1);
  items_to_send_ponderation_shear_m1.set_smart_resize(1);
  items_to_send_ponderation_shear_0.set_smart_resize(1);
  items_to_send_ponderation_shear_p1.set_smart_resize(1);


  const int ni = renum_.dimension(2) - 2;

  for (int k = kmin; k < kmax; k++)
    for (int j = jmin; j < jmax; j++)
      for (int i = imin; i < imax; i++)
        {


          //int ii_m2 ;
          int ii_m1 ;
          int ii ;
          int ii_p1 ;
          //int ii_p2 ;
          //double ponderation_m2;
          double ponderation_m1;
          double ponderation_0;
          double ponderation_p1;
          //double ponderation_p2;

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


              ponderation_m1 = a0 * ((istmp - x[1]) * (istmp - x[2]));
              ponderation_0 = a1 * ((istmp - x[0]) * (istmp - x[2]));
              ponderation_p1 = a2 * ((istmp - x[0]) * (istmp - x[1]));

              ii_m1 = ((i  + offset2 - 1) % ni + ni) % ni;
              ii = ((i  + offset2) % ni + ni) % ni;
              ii_p1 = ((i  + offset2 + 1) % ni + ni) % ni;

            }
          else
            {

              ii_m1 = -1;
              ii = i ;
              ii_p1 = -1;

              ponderation_m1 = -1;
              ponderation_0 = -1;
              ponderation_p1 = -1;

            }

          int index = renum(ii, j, k);
          assert(index >= 0);
          items_to_send.append_array(index);


          if(ii_m1!=-1)
            {
              index = renum(ii_m1, j, k);
              assert(index >= 0);
              items_to_send_m1.append_array(index);
              index = renum(ii_p1, j, k);
              assert(index >= 0);
              items_to_send_p1.append_array(index);

              // subterfuge un peu moche parce que les methodes apres nexistent que pour des tableau d entier, et pas de double
              index = (int)(ponderation_m1*1.e6);
              assert(index >= 0);
              items_to_send_ponderation_shear_m1.append_array(index);
              index = (int)(ponderation_0*1.e6);
              assert(index >= 0);
              items_to_send_ponderation_shear_0.append_array(index);
              index = (int)(ponderation_p1*1.e6);
              assert(index >= 0);
              items_to_send_ponderation_shear_p1.append_array(index);

            }
          else
            {
              items_to_send_m1.append_array(-1);
              items_to_send_p1.append_array(-1);
              items_to_send_ponderation_shear_m1.append_array(-1);
              items_to_send_ponderation_shear_0.append_array(-1);
              items_to_send_ponderation_shear_p1.append_array(-1);
            }

        }
}
/*! @brief ajoute deux coefficients diagonal/extra-diagonal a la matrice
 *
 */
void Matrice_Grossiere::ajoute_coeff(int i, int j, int k,
                                     int i_voisin, int j_voisin, int k_voisin,
                                     const double coeff)
{
  const int indice = renum(i, j, k);
  const int indice_voisin = renum(i_voisin, j_voisin, k_voisin);
  //const int indice_voisin_m2 = renum_m2(i_voisin, j_voisin, k_voisin);
  const int indice_voisin_m1 = renum_m1(i_voisin, j_voisin, k_voisin);
  const int indice_voisin_p1 = renum_p1(i_voisin, j_voisin, k_voisin);

  const double ponderation_voisin_m1 =(double) ponderation_shear_m1(i_voisin, j_voisin, k_voisin) / 1.e6;
  const double ponderation_voisin_0 = (double) ponderation_shear_0(i_voisin, j_voisin, k_voisin) / 1.e6;
  const double ponderation_voisin_p1 = (double) ponderation_shear_p1(i_voisin, j_voisin, k_voisin) / 1.e6;
  //const int indice_voisin_p2 = renum_p2(i_voisin, j_voisin, k_voisin);

//  if(indice_voisin_m1!= -1 || indice_voisin_p1!=-1)
//    {
//      std::cout << "ici" << std::endl;
//      std::cout << "indice_voisin = " << indice_voisin << std::endl;
//      std::cout << "indice_voisin_m1 = " << indice_voisin_m1 << std::endl;
//      std::cout << "indice_voisin_p1 = " << indice_voisin_p1 << std::endl;
//      std::cout << "ponderation_voisin_m1 = " << ponderation_voisin_m1 << std::endl;
//      std::cout << "ponderation_voisin_p1 = " << ponderation_voisin_p1 << std::endl;
//      std::cout << "ici" << std::endl;
//    }

  //const double ponderation_voisin_m2 = ponderation_shear_m2(i_voisin, j_voisin, k_voisin);

  //const double ponderation_voisin_p2 = ponderation_shear_p2(i_voisin, j_voisin, k_voisin);

  // dans le cas shear-boundary :
  // On a besoin de la position des 2 voisin, ainsi que la ponderation du coeff lie au decallage shear-boundary pour chaque voisin


  // coefficient extra diagonal (- surface_face / distance_centres_elements)
  double x = -coeff;
  if (indice_voisin > indice)// || indice_voisin < indice)
    // matrice symmetrique :: shear boundary reste symmetrique ?
    {
      // Coefficient dans la partie triangulaire superieure, a stocker.
      const int nreels = coeff_diag_.size_array();
      if (indice_voisin < nreels)
        {
          if (indice_voisin_m1 != -1)
            {
              // Element reel
              voisins_[indice].add(indice_voisin);
              coeffs_[indice].add(x*ponderation_voisin_0);
            }
          else
            {
              voisins_[indice].add(indice_voisin);
              coeffs_[indice].add(x);
            }
        }
      else
        {
          if (indice_voisin_m1 != -1)
            {
              voisins_virt_[indice].add(indice_voisin - nreels);
              coeffs_virt_[indice].add(x*ponderation_voisin_0);
            }
          else
            {
              voisins_virt_[indice].add(indice_voisin - nreels);
              coeffs_virt_[indice].add(x);
            }
        }

      if (indice_voisin_m1 != -1)
        {
          if(indice_voisin_m1 < nreels)
            {
              // cas shear perio avec potentiellement plusieurs voisins, et interpolation avec somme des ponderation egale a 1
              //voisins_[indice].add(indice_voisin_m2);
              voisins_[indice].add(indice_voisin_m1);
              voisins_[indice].add(indice_voisin_p1);
              //voisins_[indice].add(indice_voisin_p2);
              //coeffs_[indice].add(x*ponderation_voisin_m2);
              coeffs_[indice].add(x*ponderation_voisin_m1);
              coeffs_[indice].add(x*ponderation_voisin_p1);
              //coeffs_[indice].add(x*ponderation_voisin_p2);
            }
          else
            {
              // cas shear perio avec potentiellement plusieurs voisins, avec somme des ponderation egale a 1
              //voisins_virt_[indice].add(indice_voisin_m2 - nreels);
              voisins_virt_[indice].add(indice_voisin_m1 - nreels);
              voisins_virt_[indice].add(indice_voisin_p1 - nreels);
              //voisins_virt_[indice].add(indice_voisin_p2 - nreels);
              //coeffs_virt_[indice].add(x*ponderation_voisin_m2);
              coeffs_virt_[indice].add(x*ponderation_voisin_m1);
              coeffs_virt_[indice].add(x*ponderation_voisin_p1);
              //coeffs_virt_[indice].add(x*ponderation_voisin_p2);
            }
        }
    }
  // Contribution a la diagonale
  coeff_diag_[indice] -= x;
}
