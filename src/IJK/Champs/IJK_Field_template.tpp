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

#ifndef IJK_Field_template_TPP_H
#define IJK_Field_template_TPP_H

#include <IJK_communications.h>
#include <IJK_Splitting.h>

template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::self_proc_add_data(int le_ghost)
{
  if (monofluide_variable_!=1 && monofluide_variable_!=2)
    return;

  const IJK_Splitting& splitting = splitting_ref_.valeur();
  int last_global_k = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2) - 1;
  const int nii = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int njj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nkk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();
  int ir = 0 ;
  int jr = 0 ;
  int kr_min = -le_ghost;
  int kr_max = nkk;
  _TYPE_ *dest = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data().addr();

  for (int k = 0; k < le_ghost; k++)
    for (int j = 0; j < njj; j++)
      for (int i = 0; i < nii; i++)
        {
          int recevd_i = ir + i;
          int recevd_j = jr + j;
          int recevd_k_min = kr_min + k;
          int recevd_k_max = kr_max + k;

          int recved_i_Isig = recevd_i + splitting.get_offset_local(0);
          int recved_j_Isig = recevd_j + splitting.get_offset_local(1);
          int recved_k_Isig_min = recevd_k_min + splitting.get_offset_local(2);
          int recved_k_Isig_max = recevd_k_max + splitting.get_offset_local(2);


          _TYPE_ I_recevd_min = (_TYPE_) 0.;
          _TYPE_ I_recevd_max = (_TYPE_) 0.;
          if (recved_k_Isig_min<le_ghost)
            {
              // indicatrice_ghost_zmin_ de 0 à 2ghost-1
              recved_k_Isig_min = recved_k_Isig_min + le_ghost ;
              if (monofluide_variable_==1)
                {
                  I_recevd_min = (_TYPE_) I_sigma_kappa_ghost_zmin_(recved_i_Isig,recved_j_Isig,recved_k_Isig_min);
                  dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(recevd_i , recevd_j , recevd_k_min)]-=I_recevd_min;

                }
              else if(monofluide_variable_==2)
                {
                  I_recevd_min = (_TYPE_) indicatrice_ghost_zmin_(recved_i_Isig,recved_j_Isig,recved_k_Isig_min);
                  dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(recevd_i , recevd_j , recevd_k_min)]+=I_recevd_min*(_TYPE_)rho_l_+((_TYPE_)1.-I_recevd_min)*(_TYPE_)rho_v_;
                }

            }
          else if(recved_k_Isig_max>last_global_k-le_ghost)
            {
              // indicatrice_ghost_zmax_ de 0 à 2ghost-1   --> recved_k_Isig-(last_global_k-ghost+1) --> 0
              // dernier = last_global_k+ghost --> 2ghost-1
              recved_k_Isig_max = recved_k_Isig_max-last_global_k + le_ghost-1;
              if (monofluide_variable_==1)
                {
                  I_recevd_max = (_TYPE_) I_sigma_kappa_ghost_zmax_(recved_i_Isig,recved_j_Isig,recved_k_Isig_max);
                  dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(recevd_i , recevd_j , recevd_k_max)]-=I_recevd_max;
                }
              else if(monofluide_variable_==2)
                {
                  I_recevd_max = (_TYPE_) indicatrice_ghost_zmax_(recved_i_Isig,recved_j_Isig,recved_k_Isig_max);
                  dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(recevd_i , recevd_j , recevd_k_max)]+=I_recevd_max*(_TYPE_)rho_l_+((_TYPE_)1.-I_recevd_max)*(_TYPE_)rho_v_;
                }
            }
          else
            {
              std::cout << "on est sur un proc central" << std::endl;
              return;
            }

        }



}



template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::exchange_data(int pe_send_, /* processor to send to */
                                                             int is, int js, int ks, /* ijk coordinates of first data to send */
                                                             int pe_recv_, /* processor to recv from */
                                                             int ir, int jr, int kr, /* ijk coordinates of first data to recv */
                                                             int isz, int jsz, int ksz, /* size of block data to send/recv */
                                                             double offset, double jump_i, int nb_ghost)  /* decallage a appliquer pour la condition de shear periodique*/
{
  const IJK_Splitting& splitting = splitting_ref_.valeur();
  int ghost = 2;
  int last_global_k = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2) - 1;

  if (pe_send_ == Process::me() && pe_recv_ == Process::me())
    {
      // Self (periodicity on same processor)
      _TYPE_ *dest = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data().addr();
      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++)
            {

              // indices dans lespace complet (indice entre -ghost et real_size + ghost)
              int send_i = i + is ;
              int send_j = j + js ;
              int send_k = k + ks ;

              int recevd_i = i + ir;
              int recevd_j = j + jr;
              int recevd_k = k + kr;
              _TYPE_ buf = (_TYPE_) jump_i;

              if (offset !=0.)
                {
                  // taille du domaine physique
                  int real_size_i = isz ; // --> ni

                  // retourne un indice du domaine reel (valeur comprise entre 0 et real_size -1).
                  send_i = (int) round((double) i + (double) is +  offset) ; // de -ghost a isz+ghost

                  //4-th order interpolation
                  _TYPE_ istmp = (_TYPE_)((double) i + (double) is +  offset);
                  // interpolation directe de la variable monofluide

                  buf+=interpolation_for_shear_periodicity(2, send_i, send_j, send_k, istmp, real_size_i);


                  if (monofluide_variable_==1 )
                    {

                      int indice_interpol=0;
                      int send_i_Isig = send_i + splitting.get_offset_local(0);
                      int send_j_Isig = send_j + splitting.get_offset_local(1);
                      int send_k_Isig = send_k + splitting.get_offset_local(2);

                      if (send_k_Isig<ghost)
                        {
                          // indicatrice_ghost_zmin_ de 0 à 2ghost-1
                          indice_interpol=0;
                          send_k_Isig = send_k_Isig + ghost ;
                        }
                      else if(send_k_Isig>last_global_k-ghost)
                        {
                          // indicatrice_ghost_zmax_ de 0 à 2ghost-1   --> recved_k_Isig-(last_global_k-ghost+1) --> 0
                          // dernier = last_global_k+ghost --> 2ghost-1
                          send_k_Isig = send_k_Isig-last_global_k + ghost-1;
                          indice_interpol=1;
                        }

                      buf+=interpolation_for_shear_periodicity(indice_interpol, send_i_Isig, send_j_Isig, send_k_Isig, istmp, real_size_i);

                    }


                }
              else
                {
                  buf+=IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(send_i, send_j, send_k);
                }

              // indices dans lespace du tableau echange (recoi)

              if (monofluide_variable_==2 && offset !=0.)
                {
                  dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(recevd_i , recevd_j , recevd_k)]=0.;
                }
              else
                {
                  dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(recevd_i , recevd_j , recevd_k)] = buf ;
                }

            }

      return;
    }
  const int data_size = isz * jsz * ksz;
  const int type_size = sizeof(_TYPE_);
  _TYPE_ *send_buffer = 0;
  _TYPE_ *recv_buffer = 0;

  if (pe_send_ >= 0)
    {
      send_buffer = new _TYPE_[data_size];
      _TYPE_ *buf = send_buffer;

      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++, buf++)
            {

              // indices dans lespace complet (indice entre -ghost et real_size + ghost)
              int send_i = i + is ;
              int send_j = j + js ;
              int send_k = k + ks ;
              if (offset !=0.)
                {
                  // taille du domaine physique
                  int real_size_i = isz ; // --> ni
                  // retourne un indice du domaine reel (valeur comprise entre 0 et real_size -1).
                  send_i = (int) round((double) i + (double) is +  offset) ; // de -ghost a isz+ghost
                  //4-th order interpolation
                  _TYPE_ istmp = (_TYPE_)((double) i + (double) is +  offset);

                  *buf= interpolation_for_shear_periodicity(2, send_i, send_j, send_k, istmp, real_size_i) + (_TYPE_) jump_i;

                  if (monofluide_variable_==1)
                    {

                      int indice_interpol=0;
                      int send_i_Isig = send_i + splitting.get_offset_local(0);
                      int send_j_Isig = send_j + splitting.get_offset_local(1);
                      int send_k_Isig = send_k + splitting.get_offset_local(2);

                      if (send_k_Isig<ghost)
                        {
                          // indicatrice_ghost_zmin_ de 0 à 2ghost-1
                          indice_interpol=0;
                          send_k_Isig = send_k_Isig + ghost ;
                        }
                      else if(send_k_Isig>last_global_k-ghost)
                        {
                          // indicatrice_ghost_zmax_ de 0 à 2ghost-1   --> recved_k_Isig-(last_global_k-ghost+1) --> 0
                          // dernier = last_global_k+ghost --> 2ghost-1
                          send_k_Isig = send_k_Isig-last_global_k + ghost-1;
                          indice_interpol=1;
                        }

                      *buf+=interpolation_for_shear_periodicity(indice_interpol, send_i_Isig, send_j_Isig, send_k_Isig, istmp, real_size_i);
                    }
                  else if (monofluide_variable_==2)
                    {
                      *buf=(_TYPE_) 0.;
                    }
                }
              else
                {
                  *buf= IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(send_i, send_j, send_k)+(_TYPE_) jump_i;
                }


            }
    }



  if (pe_recv_ >= 0)
    recv_buffer = new _TYPE_[data_size];
  ::envoyer_recevoir(send_buffer, data_size * type_size, pe_send_, recv_buffer, data_size * type_size, pe_recv_);

  if (pe_recv_ >= 0)
    {
      _TYPE_ *buf = recv_buffer;
      _TYPE_ *dest = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data().addr();
      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++, buf++)
            {
              dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)] = *buf;
            }

    }

  delete[] send_buffer;
  delete[] recv_buffer;
}

/*! @brief Exchange data over "ghost" number of cells.
 *
 */
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::echange_espace_virtuel(int le_ghost, double jump_i)
{
  // jump_i used to impose a jump at the domain z-boundary
  // Only used for velocity_x
  statistiques().begin_count(echange_vect_counter_);
  assert(le_ghost <= (IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ghost()));
  // Exchange in i direction real cells,
  // then in j direction with ghost cells in i,
  // then in k direction, with ghost cells in i and j

  // new function for shear_periodic conditions works without any splitting in i-direction
  // if splitting in i-direction --> get_neighbour_processor has to be changed
  const IJK_Splitting& splitting = splitting_ref_.valeur();
  int pe_imin_ = splitting.get_neighbour_processor(0, 0);
  int pe_imax_ = splitting.get_neighbour_processor(1, 0);
  int pe_jmin_ = splitting.get_neighbour_processor(0, 1);
  int pe_jmax_ = splitting.get_neighbour_processor(1, 1);

  int pe_kmin_ = splitting.get_neighbour_processor(0, 2);
  int pe_kmax_ = splitting.get_neighbour_processor(1, 2);
  int z_index = splitting.get_local_slice_index(2);
  int z_index_min = 0;
  int z_index_max = splitting.get_nprocessor_per_direction(2) - 1;

  const int nii = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int njj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nkk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();

  // calcul du decallage offset_i dans le cas de conditions de cisaillement periodique
  // correspond au nombre de maille en x qui separe deux elements periodique sur le bord zmin, zmax

  double offset_i=0.;

  if (IJK_Splitting::defilement_ == 1)
    {

      double Lx =  splitting.get_grid_geometry().get_domain_length(0);
      IJK_Splitting::Lx_for_shear_perio = Lx;
      double DX = Lx/nii ;
      double Shear_x_time = IJK_Splitting::shear_x_time_;
      offset_i = Shear_x_time/DX;

      compteur_=0;
      // send left layer of real cells to right layer of virtual cells
      if (monofluide_variable_ == 2 && !Process::je_suis_maitre())
        std::cout << "debut grandeur_l = " << rho_l_ << std::endl;
      exchange_data(pe_imin_, 0, -le_ghost, -le_ghost, pe_imax_, nii, -le_ghost, -le_ghost, le_ghost, njj + 2*le_ghost, nkk+ 2*le_ghost); /* size of block data to send */
      // send right real cells to left virtual cells
      exchange_data(pe_imax_, nii - le_ghost, -le_ghost, -le_ghost, pe_imin_, -le_ghost, -le_ghost, -le_ghost, le_ghost, njj + 2*le_ghost, nkk+ 2*le_ghost);

      exchange_data(pe_jmin_, 0, 0, -le_ghost, pe_jmax_, 0, njj, -le_ghost, nii , le_ghost, nkk + 2 * le_ghost);

      exchange_data(pe_jmax_, 0, njj - le_ghost, -le_ghost, pe_jmin_, 0, -le_ghost, -le_ghost, nii , le_ghost, nkk + 2 * le_ghost);
      // duCluzeau
      // changement pour faire en sorte que lechange sur z ne se fasse que sur le domaine reel (pour condition de cisaillement periodique)
      // sur x, echange d'un bloc [ghost, njj + 2*ghost, nkk+ 2*ghost]
      // sur z, echange d'un bloc [nii, njj, ghost]
      // sur y, echange d'un bloc [nii, ghost, nkk+ 2*ghost]
      compteur_=0;
      if (z_index != z_index_min)
        {
          exchange_data(pe_kmin_, 0, 0, 0, pe_kmax_, 0, 0, nkk, nii , njj , le_ghost);
//          if (monofluide_variable_ == 2 && !Process::je_suis_maitre())
//        	  std::cout << "z_-_normal " << compteur_ << std::endl;
          compteur_=0;

        }
      else
        {
          exchange_data(pe_kmin_, 0, 0, 0, pe_kmax_, 0, 0, nkk, nii, njj, le_ghost, -offset_i, jump_i, le_ghost);
          if (monofluide_variable_ == 2 && Process::je_suis_maitre())
            std::cout << "z - min " << compteur_ << std::endl;
          compteur_=0;
        }

      if (z_index != z_index_max)
        {
          exchange_data(pe_kmax_, 0, 0, nkk - le_ghost, pe_kmin_, 0, 0, -le_ghost, nii, njj, le_ghost);
//          if (monofluide_variable_ == 2 && Process::je_suis_maitre())
//        	  std::cout << "z + normal " << compteur_ << std::endl;
          compteur_=0;
        }
      else
        {
          exchange_data(pe_kmax_, 0, 0, nkk - le_ghost, pe_kmin_, 0, 0, -le_ghost, nii, njj, le_ghost, offset_i, -jump_i, le_ghost);
          if (monofluide_variable_ == 2 && !Process::je_suis_maitre())
            std::cout << "z + max " << compteur_ << std::endl;
          compteur_=0;
        }
    }
  else
    {
      // send left layer of real cells to right layer of virtual cells
      exchange_data(pe_imin_, 0, 0, 0, pe_imax_, nii, 0, 0, le_ghost, njj, nkk); /* size of block data to send */

      // send right real cells to left virtual cells
      exchange_data(pe_imax_, nii - le_ghost, 0, 0, pe_imin_, -le_ghost, 0, 0, le_ghost, njj, nkk);

      exchange_data(pe_jmin_, -le_ghost, 0, 0, pe_jmax_, -le_ghost, njj, 0, nii + 2 * le_ghost, le_ghost, nkk);

      exchange_data(pe_jmax_, -le_ghost, njj - le_ghost, 0, pe_jmin_, -le_ghost, -le_ghost, 0, nii + 2 * le_ghost, le_ghost, nkk);

      exchange_data(pe_kmin_, -le_ghost, -le_ghost, 0, pe_kmax_, -le_ghost, -le_ghost, nkk, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost);

      exchange_data(pe_kmax_, -le_ghost, -le_ghost, nkk - le_ghost, pe_kmin_, -le_ghost, -le_ghost, -le_ghost, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost);

    }

  statistiques().end_count(echange_vect_counter_);

  if (IJK_Splitting::defilement_ == 1)
    {
      self_proc_add_data(le_ghost);
    }


}


// change_to_sheared_reference_frame : advecte le champ par le cisaillement moyen with 4th order interpolation
// hypothese de Fourier : phi_new(x,y,z)=phi(x-tSz, y, z)
// avec t le temps et S le cisaillement.
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::change_to_sheared_reference_frame(double sens, int loc, int time)
{
  // loc=0 pour prendre la valeur aux elements ; loc = 1,2,3 pour prendre aux faces I,J,K
  // time = 1 par defaut (decallage total) ; =0 pour avoir le decallage sur un seul pas de temps
  // sens = 1. --> du referentiel du labo ver le referentiel cisaille
  // sens = -1.--> du referentiel cisaille vers le referentiel labo
  const IJK_Splitting& splitting = splitting_ref_.valeur();
  const int ni = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int nj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();
  _TYPE_ARRAY_ tmptab = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data();
  double Lz =  splitting.get_grid_geometry().get_domain_length(2);
  double Lx =  splitting.get_grid_geometry().get_domain_length(0);
  double DX = Lx/ni ;

  for (int k = 0 ; k < nk; k++)
    {
      for (int j = 0 ; j < nj; j++)
        {
          for (int i = 0 ; i < ni; i++)
            {
              Vecteur3 xyz;
              if (loc==0)
                xyz = splitting.get_coords_of_dof(i,j,k,IJK_Splitting::ELEM);
              else if (loc==1)
                xyz = splitting.get_coords_of_dof(i,j,k,IJK_Splitting::FACES_I);
              else if (loc==2)
                xyz = splitting.get_coords_of_dof(i,j,k,IJK_Splitting::FACES_J);
              else // (loc==3)
                xyz = splitting.get_coords_of_dof(i,j,k,IJK_Splitting::FACES_K);

              double x_deplacement;
              if (time==0)
                x_deplacement = -sens*IJK_Splitting::shear_x_DT_*(xyz[2]-Lz/2.)/Lz;
              else //(time==1)
                x_deplacement = -sens*IJK_Splitting::shear_x_time_*(xyz[2]-Lz/2.)/Lz;
              double istmp = i+x_deplacement/DX;

              int ifloorm2 = (int) floor(istmp) - 2;
              int ifloorm1 = (int) floor(istmp) - 1;
              int ifloor0 = (int) floor(istmp);
              int ifloorp1 = (int) floor(istmp) + 1;
              int ifloorp2 = (int) floor(istmp) + 2;

              int x[5] = {ifloorm2, ifloorm1, ifloor0, ifloorp1, ifloorp2};

              ifloorm2 = (ifloorm2 % ni + ni) % ni;
              ifloorm1 = (ifloorm1 % ni + ni) % ni;
              ifloor0 = (ifloor0 % ni + ni) % ni;
              ifloorp1 = (ifloorp1 % ni + ni) % ni;
              ifloorp2 = (ifloorp2 % ni + ni) % ni;

              double y[5] = {IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(ifloorm2, j, k),
                             IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(ifloorm1, j, k),
                             IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(ifloor0, j, k),
                             IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(ifloorp1, j, k),
                             IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(ifloorp2, j, k)
                            };


              double a0 = y[0] / ((x[0] - x[1]) * (x[0] - x[2]) * (x[0] - x[3]) * (x[0] - x[4]));
              double a1 = y[1] / ((x[1] - x[0]) * (x[1] - x[2]) * (x[1] - x[3]) * (x[1] - x[4]));
              double a2 = y[2] / ((x[2] - x[0]) * (x[2] - x[1]) * (x[2] - x[3]) * (x[2] - x[4]));
              double a3 = y[3] / ((x[3] - x[0]) * (x[3] - x[1]) * (x[3] - x[2]) * (x[3] - x[4]));
              double a4 = y[4] / ((x[4] - x[0]) * (x[4] - x[1]) * (x[4] - x[2]) * (x[4] - x[3]));

              // Evaluate the interpolation polynomial at istmp

              tmptab(IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(i, j, k)) = a0 * ((istmp - x[1]) * (istmp - x[2]) * (istmp - x[3]) * (istmp - x[4]))
                                                                                             + a1 * ((istmp - x[0]) * (istmp - x[2]) * (istmp - x[3]) * (istmp - x[4]))
                                                                                             + a2 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[3]) * (istmp - x[4]))
                                                                                             + a3 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[2]) * (istmp - x[4]))
                                                                                             + a4 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[2]) * (istmp - x[3]));

            }
        }
    }

  for (int k = 0 ; k < nk; k++)
    {
      for (int j = 0 ; j < nj; j++)
        {
          for (int i = 0 ; i < ni; i++)
            {
              IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(i, j, k)=tmptab(IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(i, j, k));
            }
        }
    }
  //IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::echange_espace_virtuel(IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ghost());
}



// ajout au second membre de pressure_rhs un terme correctif
// ce terme compense l erreur commise par l interpolation sur le bord perio z de la pression monofluide dans le cas de conditions shear periodique
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::ajouter_second_membre_shear_perio(IJK_Field_double& resu)
{
  //if(monofluide_variable_==0)
  return;
  const IJK_Splitting& splitting = splitting_ref_.valeur();
  const IJK_Grid_Geometry& geom = splitting.get_grid_geometry();
  const int ni = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int nj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();
  const double dxk = geom.get_constant_delta(2);
  const double dxj = geom.get_constant_delta(1);
  const double dxi = geom.get_constant_delta(0);
  //const double V = dxk*dxj*dxi;
  double Shear_x_time = IJK_Splitting::shear_x_time_;
  double offset = Shear_x_time/dxi;
  int ghost = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ghost();

  // Les coeffs de la matrice de pression sont multiplies par V/h^2
  // ici on parle d'un voisin sur dz donc h = dz
  // coef = dx*dy*dz/dz*dz = dx*dy/dz
  // on ajoute le terme a pressure_rhs juste avant l'appel du solveur de pression
  // pressure_rhs a deja integre d_velocity sur le volume et divise par rho
  // Le coeff a appliquer est donc dx*dy/dz/rho

  double coeff_matrice = dxi * dxj / dxk;
  int last_global_k = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2);

  // EQUIVALENCE GHOST_MIN_MAX ARRAY AVEC INDICE Z REEL POUR SPLITTING_NS_
  // I_sigma_kappa_ghost_zmin_[0] --> -ghost (derniere maille ghost)
  // I_sigma_kappa_ghost_zmin_[ghost-1] --> -1 (premiere maille ghost)
  // I_sigma_kappa_ghost_zmin_[ghost] --> 0 (premiere maille reelle)
  // I_sigma_kappa_ghost_zmin_[2*ghost] --> +ghost (derniere maille reelle)

//		I_sigma_kappa_ghost_zmax_[0] --> nk-1-ghost(derniere maille reelle)
//		I_sigma_kappa_ghost_zmax_[ghost] --> nk-1 (premiere maille reelle)
//		I_sigma_kappa_ghost_zmax_[ghost+1] --> nk (premiere maille ghost)
//		I_sigma_kappa_ghost_zmax_[2*ghost] --> nk-1+ghost (derniere maille ghost)


  for (int j = 0 ; j < nj; j++)
    {
      for (int i = 0 ; i < ni; i++)
        {
          // for k=0 --> pression voisine problematique en k=-1 (qui renvoie a une interpolation sur k=nk-1)
          // I_sigma_kappa interpole en k=nk-1
          // I_sigma_kappa reel en k=-1
          // for k=0 (z_index_min --> + offset)
          if(splitting.get_offset_local(2)==0)
            {

              int i_Isig = i + splitting.get_offset_local(0);
              int j_Isig = j + splitting.get_offset_local(1);
              int send_i = (int) round((double) i  +  offset) ;
              _TYPE_ istmp = (_TYPE_)((double) i +  offset);
              _TYPE_ I_sig_kappa_reel=(_TYPE_)I_sigma_kappa_ghost_zmin_(i_Isig , j_Isig , ghost-1);
              double rho = rho_l_*indicatrice_ghost_zmin_(i_Isig,j_Isig,ghost-1)+rho_v_*(1.-indicatrice_ghost_zmin_(i_Isig,j_Isig,ghost-1));

              _TYPE_ I_sig_kappa_interpol=interpolation_for_shear_periodicity(1, send_i, j_Isig, ghost, istmp, ni);
              _TYPE_ erreur = I_sig_kappa_interpol-I_sig_kappa_reel;
              resu(i,j,0)+=(coeff_matrice/rho)*erreur;
            }

          // for k=nk-1 --> pression voisine problematique en k=nk (qui renvoie a une interpolation sur k=0)
          // I_sigma_kappa interpole en k=0
          // I_sigma_kappa reel en k=nk
          // for k=nk-1 (z_index_max --> - offset)
          if(splitting.get_offset_local(2)+nk==last_global_k)
            {
              int i_Isig = i + splitting.get_offset_local(0);
              int j_Isig = j + splitting.get_offset_local(1);
              int send_i = (int) round((double) i  -  offset) ;
              _TYPE_ istmp = (_TYPE_)((double) i -  offset);
              _TYPE_ I_sig_kappa_reel=(_TYPE_)I_sigma_kappa_ghost_zmax_(i_Isig , j_Isig , ghost+1);
              double rho = rho_l_*indicatrice_ghost_zmax_(i_Isig,j_Isig,ghost+1)+rho_v_*(1.-indicatrice_ghost_zmax_(i_Isig,j_Isig,ghost+1));


              _TYPE_ I_sig_kappa_interpol=interpolation_for_shear_periodicity(0, send_i, j_Isig, ghost, istmp, ni);
              _TYPE_ erreur = I_sig_kappa_interpol-I_sig_kappa_reel;
              resu(i,j,nk-1)+=(coeff_matrice/rho)*erreur;
            }
        }
    }

}


template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::interpolation_for_shear_periodicity(const int phase, const int send_i, const int send_j, const int send_k, const _TYPE_ istmp, const int real_size_i)
{
  // renvoi la valeur interpolee pour la condition de shear-periodicity

  int nb_points = order_interpolation_+1;
  _TYPE_* x = new _TYPE_[nb_points];
  _TYPE_* y= new _TYPE_[nb_points];
  _TYPE_* a= new _TYPE_[nb_points];
  _TYPE_* res = new _TYPE_[nb_points];
  _TYPE_ resu = 0.;

  if (nb_points==2)
    {
      if(istmp >= (_TYPE_) send_i)
        {
          x[0] = (_TYPE_)send_i;
          x[1] = (_TYPE_)send_i+1;
        }
      else
        {
          x[0] = (_TYPE_)send_i-1;
          x[1] = (_TYPE_)send_i;
        }
    }
  else if(nb_points==3)
    {
      x[0] = (_TYPE_)send_i-1;
      x[1] = (_TYPE_)send_i;
      x[2] = (_TYPE_)send_i+1;
    }
  else if(nb_points==5)
    {
      x[0] = (_TYPE_)send_i-2;
      x[1] = (_TYPE_)send_i-1;
      x[2] = (_TYPE_)send_i;
      x[3] = (_TYPE_)send_i+1;
      x[4] = (_TYPE_)send_i+2;
    }

  for (int pt = 0; pt < nb_points ; pt++)
    {
      if (phase==0)
        y[pt] = (_TYPE_)I_sigma_kappa_ghost_zmin_(((int)x[pt] % real_size_i + real_size_i) % real_size_i, send_j, send_k);
      else if (phase==1)
        y[pt] = (_TYPE_)I_sigma_kappa_ghost_zmax_(((int)x[pt] % real_size_i + real_size_i) % real_size_i, send_j, send_k);
      else if (phase==2)
        y[pt] = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(((int)x[pt] % real_size_i + real_size_i) % real_size_i, send_j, send_k);
      else
        {
          std::cout << "ce choix n existe pas" ;
          Process::exit();
        }
    }

  for (int pt = 0; pt < nb_points ; pt++)
    {
      _TYPE_ denum = 1.;
      for (int pt_autre = 0; pt_autre < nb_points ; pt_autre++)
        {
          if (pt_autre!=pt)
            denum *= (x[pt] - x[pt_autre]);
        }
      a[pt]=y[pt]/denum;
    }


  for (int pt = 0; pt < nb_points ; pt++)
    {
      res[pt] = a[pt];
      for (int pt_autre = 0; pt_autre < nb_points ; pt_autre++)
        {
          if (pt_autre!=pt)
            res[pt] *= (istmp - x[pt_autre]);
        }
      resu+=res[pt];
    }

  delete[] x;
  delete[] y;
  delete[] a;
  delete[] res;

  return resu;

}


template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::update_I_sigma_kappa(const IJK_Field_double& indic_ft, const IJK_Field_double& courbure_ft, const int ft_extension, const double sigma)
{
  // besoin d'indic_ft pour remplir les valeurs exactes de l'indicatrice dans les mailles ghost
  // on veut se servir de la valeur exacte pour interpoler au mieux les grandeurs monofluides
  // on stock les valeurs en question dans des tableau de taille ni*nj*2*ghost pour avoir lindicatrice reelle et ghost de chaque cote du domaine
  // Ces tableaux doivent etre lisibles par tous les procs en entier pour simplifier....
  if(monofluide_variable_==0)
    return;

  I_sigma_kappa_ghost_zmin_.data()=0.;
  I_sigma_kappa_ghost_zmax_.data()=0.;
  indicatrice_ghost_zmin_.data() =0.;
  indicatrice_ghost_zmax_.data() =0.;
  IJK_Splitting splitting_ft = courbure_ft.get_splitting();


  int ghost = 2;
  const IJK_Splitting& splitting = splitting_ref_.valeur();
  int last_global_k = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2);
  int last_global_j = splitting.get_nb_items_global(IJK_Splitting::ELEM, 1);
  int last_global_i = splitting.get_nb_items_global(IJK_Splitting::ELEM, 0);

  // On veut les derniers indices (qui commencent a zero)
  last_global_k -= 1;
  last_global_j -= 1;
  last_global_i -= 1;

  if (monofluide_variable_!=0 )
    {
      // on parcours sur tous le domaine ft
      // on rempli dans les tableaux uniquement les valeurs qui satisfont les conditions
      // I_sigma_kappa_ghost_zmin_[0] --> -ghost (derniere maille ghost)
      // I_sigma_kappa_ghost_zmin_[ghost] --> 0 (premiere maille reelle)
      // I_sigma_kappa_ghost_zmin_[ghost-1] --> -1 (premiere maille ghost)
      // I_sigma_kappa_ghost_zmin_[2*ghost] --> +ghost (derniere maille reelle)

//
//					 k_reel [-ghost, 0, nk-1-ghost, nk-1, nk-1+ghost]
//		k_reel-(nk-1)+ghost [-(nk-1), ghost-(nk-1), nk-1-(nk-1), nk-1+ghost-(nk-1), nk-1+2ghost-(nk-1)]
//		k_reel-(nk-1)+ghost [-nk+1, ghost-nk+1, 0, ghost, 2ghost]
//		if(k_reel>=nk-1-ghost & <= nk-1+ghost)
//
//		I_sigma_kappa_ghost_zmax_[0] --> nk-1-ghost(derniere maille reelle)
//		I_sigma_kappa_ghost_zmax_[ghost] --> nk-1 (premiere maille reelle)
//		I_sigma_kappa_ghost_zmax_[ghost+1] --> nk (premiere maille ghost)
//		I_sigma_kappa_ghost_zmax_[2*ghost] --> nk-1+ghost (derniere maille ghost)


      for (int k = 0; k < indic_ft.nk() ; k++)
        {
          const int k_reel = k + splitting_ft.get_offset_local(2) - ft_extension;
          if (k_reel<-ghost || k_reel>last_global_k+ghost)
            {
              continue;
            }

          for (int j = 0; j < indic_ft.nj(); j++)
            {
              const int j_reel = j + splitting_ft.get_offset_local(1) - ft_extension;
              if (j_reel<-ghost || j_reel>last_global_j+ghost)
                {
                  continue;
                }

              for (int i = 0; i < indic_ft.ni(); i++)
                {
                  const int i_reel = i + splitting_ft.get_offset_local(0) - ft_extension;
                  if (i_reel<-ghost || i_reel>last_global_i+ghost)
                    {
                      continue;
                    }

                  if (k_reel<ghost)
                    {
                      indicatrice_ghost_zmin_(i_reel,j_reel,k_reel+ghost) = indic_ft(i,j,k);
                    }
                  else if(k_reel>=last_global_k-ghost)
                    {
                      indicatrice_ghost_zmax_(i_reel,j_reel,k_reel-last_global_k+ghost) = indic_ft(i,j,k);
                    }
                  if (monofluide_variable_==1 )
                    {
                      if (k_reel<ghost)
                        {
                          I_sigma_kappa_ghost_zmin_(i_reel,j_reel,k_reel+ghost) = std::abs(courbure_ft(i,j,k));
                        }
                      else if(k_reel>=last_global_k-ghost)
                        {
                          I_sigma_kappa_ghost_zmax_(i_reel,j_reel,k_reel-last_global_k+ghost) = std::abs(courbure_ft(i,j,k));
                        }

                    }
                }

            }
        }

      for (int k = 0; k < indicatrice_ghost_zmin_.nk() ; k++)
        {
          for (int j = 0; j < indicatrice_ghost_zmin_.nj() ; j++)
            {
              for (int i = 0; i < indicatrice_ghost_zmin_.ni() ; i++)
                {
                  indicatrice_ghost_zmin_(i,j,k)=Process::mp_sum(indicatrice_ghost_zmin_(i,j,k));
                  indicatrice_ghost_zmax_(i,j,k)=Process::mp_sum(indicatrice_ghost_zmax_(i,j,k));
                }
            }
        }


      for (int k = 0; k < I_sigma_kappa_ghost_zmin_.nk() ; k++)
        {
          for (int j = 0; j < I_sigma_kappa_ghost_zmin_.nj() ; j++)
            {
              for (int i = 0; i < I_sigma_kappa_ghost_zmin_.ni() ; i++)
                {
                  I_sigma_kappa_ghost_zmin_(i,j,k)=Process::mp_sum(I_sigma_kappa_ghost_zmin_(i,j,k));
                  I_sigma_kappa_ghost_zmax_(i,j,k)=Process::mp_sum(I_sigma_kappa_ghost_zmax_(i,j,k));
                }
            }
        }

      for (int iproc = 0; iproc < Process::nproc() ; iproc++)
        {
          envoyer_broadcast(indicatrice_ghost_zmin_, iproc);
          envoyer_broadcast(indicatrice_ghost_zmax_, iproc);
          envoyer_broadcast(I_sigma_kappa_ghost_zmin_, iproc);
          envoyer_broadcast(I_sigma_kappa_ghost_zmax_, iproc);
        }

    }
  else
    {
      std::cout << "ne devrait pas être ici car variable non monofluide" << std::endl;
      Process::exit();
    }
}


template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::relever_I_sigma_kappa_ns(IJK_Field_double& field_ns)
{
  // on stocke les deux tableaux sur les premieres et derniers mailles reelles
  // --> permet de voir facilement le contenu des ghosts en post-pro
  if(monofluide_variable_==0)
    return;
  const IJK_Splitting& splitting = splitting_ref_.valeur();
  const int ni = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int nj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();
  int last_global_k = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2)-1;
//  int last_global_j = splitting.get_nb_items_global(IJK_Splitting::ELEM, 1);
//  int last_global_i = splitting.get_nb_items_global(IJK_Splitting::ELEM, 0);
  int ghost = 2;

  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              const int i_reel = i + splitting.get_offset_local(0);
              const int j_reel = j + splitting.get_offset_local(1);
              const int k_reel = k + splitting.get_offset_local(2);

              if (monofluide_variable_==1 )
                {
                  if (k_reel<2*ghost)
                    {
                      field_ns(i,j,k)=indicatrice_ghost_zmin_(i_reel,j_reel,k_reel);
                    }
                  else if(k_reel>last_global_k-2*ghost)
                    {
                      field_ns(i,j,k)=indicatrice_ghost_zmax_(i_reel,j_reel,k_reel-(last_global_k-2*ghost));
                    }
                }
            }
        }
    }
}


// Initializes the field and allocates memory
// splitting: reference to the geometry of the IJK mesh and how the mesh is split on processors.
//   The field stores a reference to this IJK_Splitting object so do not delete it.
// loc: localisation of the field (elements, nodes, faces in direction i, j, or k)
//   The number of "real" items in each direction (returned by the ni(), nj() or nk() method) is obtained from
//   the IJK_Splitting object. Warning: on a processor that is in the middle of the mesh, the nodes on the
//   right of the rightmost real element are not real, they are virtual, values are copied from the neigbour
//   processor.
// ghost_size: number of ghost layers to allocate. When an exchange of ghost cells data is requested, a smaller
//   number of layers can be requested if all layers are not needed by the following operations
// additional_k_layers: allocates more layers of cells in the k direction for use with the
//   shift_k_origin() method (optimization trick used by the temporally blocked algorithms in the multigrid
//   solver
// nb_compo: number of components of the field. Warning: you cannot allocate the velocity field at faces
//   with nb_compo=3: numbers of faces in each direction differ.
//   Also, components are not grouped by node but stored by layers in k. nb_compo>1 is essentially used
//   in the multigrid solver to optimize memory accesses to the components of the matrix.
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::allocate(const IJK_Splitting& splitting, IJK_Splitting::Localisation loc, int ghost_size, int additional_k_layers, int ncompo, bool external_storage, int type, double rov, double rol)
{
  const int ni_local = splitting.get_nb_items_local(loc, 0);
  const int nj_local = splitting.get_nb_items_local(loc, 1);
  const int nk_local = splitting.get_nb_items_local(loc, 2);
  order_interpolation_ = 2;
  monofluide_variable_ = type;
  IJK_Field_local_template<_TYPE_, _TYPE_ARRAY_>::allocate(ni_local, nj_local, nk_local, ghost_size, additional_k_layers, ncompo);

  // monofluide_variable_==1 est fait pour la pression
  // permet de gerer linterpolation monofluide de la pression en shear periodicite
  if (monofluide_variable_==1)
    {
      indicatrice_ghost_zmin_.allocate(ni_local, nj_local, 2*ghost_size, ghost_size, additional_k_layers, ncompo);
      indicatrice_ghost_zmax_.allocate(ni_local, nj_local, 2*ghost_size, ghost_size, additional_k_layers, ncompo);
      I_sigma_kappa_ghost_zmin_.allocate(ni_local, nj_local, 2*ghost_size, ghost_size, additional_k_layers, ncompo);
      I_sigma_kappa_ghost_zmax_.allocate(ni_local, nj_local, 2*ghost_size, ghost_size, additional_k_layers, ncompo);
//      indicatrice_ghost_zmin_local_.allocate(ni_local, nj_local, 2*ghost_size, ghost_size, additional_k_layers, ncompo);
//      indicatrice_ghost_zmax_local_.allocate(ni_local, nj_local, 2*ghost_size, ghost_size, additional_k_layers, ncompo);
//      I_sigma_kappa_ghost_zmin_local_.allocate(ni_local, nj_local, 2*ghost_size, ghost_size, additional_k_layers, ncompo);
//      I_sigma_kappa_ghost_zmax_local_.allocate(ni_local, nj_local, 2*ghost_size, ghost_size, additional_k_layers, ncompo);

      for (int iproc = 0; iproc < Process::nproc() ; iproc++)
        {
          envoyer_broadcast(indicatrice_ghost_zmin_, iproc);
          envoyer_broadcast(indicatrice_ghost_zmax_, iproc);
          envoyer_broadcast(I_sigma_kappa_ghost_zmin_, iproc);
          envoyer_broadcast(I_sigma_kappa_ghost_zmax_, iproc);
        }
      rho_v_ = rov;
      rho_l_ = rol;
    }
  // monofluide_variable_==1 est fait pour les proprietes physiques constante par phase (rho, mu, nu etc.)
  // permet de gerer linterpolation monofluide
  if (monofluide_variable_==2)
    {
      indicatrice_ghost_zmin_.allocate(ni_local, nj_local, 2*ghost_size, ghost_size, additional_k_layers, ncompo);
      indicatrice_ghost_zmax_.allocate(ni_local, nj_local, 2*ghost_size, ghost_size, additional_k_layers, ncompo);
      rho_v_ = rov;
      rho_l_ = rol;
    }

  splitting_ref_ = splitting;
  localisation_ = loc;
}


#endif /* IJK_Field_template_TPP_H */
