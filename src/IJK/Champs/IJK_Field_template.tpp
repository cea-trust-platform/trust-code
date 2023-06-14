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
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::exchange_data(int pe_send_, /* processor to send to */
                                                             int is, int js, int ks, /* ijk coordinates of first data to send */
                                                             int pe_recv_, /* processor to recv from */
                                                             int ir, int jr, int kr, /* ijk coordinates of first data to recv */
                                                             int isz, int jsz, int ksz, /* size of block data to send/recv */
                                                             double offset, double jump_i, int nb_ghost)  /* decallage a appliquer pour la condition de shear periodique*/
{

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
                  int real_size_i = isz - 2*nb_ghost; // --> ni
                  // indices dans lespace complet (indice entre -ghost et real_size + ghost)
                  //int real_send_i_sauv = i + is ;

                  // retourne un indice du domaine reel (valeur comprise entre 0 et real_size -1).
                  send_i = (int) round((double) i + (double) is +  offset) ; // de -ghost a isz+ghost

                  //4-th order interpolation
				  _TYPE_ istmp = (_TYPE_)((double) i + (double) is +  offset);

            	  if (monofluide_variable_ )
            	  {
            	// interpoler p_l
                // interpoler p_v
                // reconstruire la pression monofluide avec l'indicatrice de la position de reception
				  _TYPE_ buf_V = fourth_order_interpolation_for_shear_periodicity(1, send_i, send_j, send_k, istmp, real_size_i);
				  _TYPE_ buf_L = fourth_order_interpolation_for_shear_periodicity(0, send_i, send_j, send_k, istmp, real_size_i);
				  _TYPE_ buf_I =(_TYPE_)indicatrice_(recevd_i , recevd_j , recevd_k);
				  buf+= buf_V *((_TYPE_)1.-buf_I)
					  + buf_L * buf_I;
            	  }
            	  else
            	  {
            	// interpolation directe de la variable monofluide
                  buf+=fourth_order_interpolation_for_shear_periodicity(2, send_i, send_j, send_k, istmp, real_size_i);

            	  }
                }
              else
                {
                  buf+=IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(send_i, send_j, send_k);
                }

              // indices dans lespace du tableau echange (recoi)
              dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(recevd_i , recevd_j , recevd_k)] = buf;

            }

      return;
    }
  const int data_size = isz * jsz * ksz;
  const int type_size = sizeof(_TYPE_);
  _TYPE_ *send_buffer = 0;
  _TYPE_ *send_buffer_2 = 0;
  _TYPE_ *recv_buffer = 0;
  _TYPE_ *recv_buffer_2 = 0;

  if (pe_send_ >= 0)
    {
      send_buffer = new _TYPE_[data_size];
      send_buffer_2 = new _TYPE_[data_size];
      // Pack send data
      _TYPE_ *buf = send_buffer;
      _TYPE_ *buf2 = send_buffer_2;

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
                  int real_size_i = isz - 2*nb_ghost; // --> ni
                  // retourne un indice du domaine reel (valeur comprise entre 0 et real_size -1).
                  send_i = (int) round((double) i + (double) is +  offset) ; // de -ghost a isz+ghost
                  //4-th order interpolation
                  _TYPE_ istmp = (_TYPE_)((double) i + (double) is +  offset);


            	  if (monofluide_variable_ )
            	  {
            	// interpoler p_l
                // interpoler p_v
                // reconstruire la pression monofluide avec l'indicatrice de la position de reception a la reception des buffer
				  *buf = fourth_order_interpolation_for_shear_periodicity(0, send_i, send_j, send_k, istmp, real_size_i)+ (_TYPE_) jump_i;
				  *buf2 = fourth_order_interpolation_for_shear_periodicity(1, send_i, send_j, send_k, istmp, real_size_i)+ (_TYPE_) jump_i;
            	  }
            	  else
            	  {
                  *buf= fourth_order_interpolation_for_shear_periodicity(2, send_i, send_j, send_k, istmp, real_size_i) + (_TYPE_) jump_i;
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
  	recv_buffer_2 = new _TYPE_[data_size];
  ::envoyer_recevoir(send_buffer, data_size * type_size, pe_send_, recv_buffer, data_size * type_size, pe_recv_);
  ::envoyer_recevoir(send_buffer_2, data_size * type_size, pe_send_, recv_buffer_2, data_size * type_size, pe_recv_);

  if (pe_recv_ >= 0)
    {
      // Unpack recv data
      _TYPE_ *buf = recv_buffer;
      _TYPE_ *buf2 = recv_buffer_2;
      _TYPE_ *dest = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data().addr();
      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++, buf++)
          {
        	  if (monofluide_variable_ )
        	  {
        	  _TYPE_ buf_I =(_TYPE_)indicatrice_(ir + i, jr + j, kr + k);
              dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)] = *buf2 * ((_TYPE_)1.-buf_I)+ *buf * buf_I;
        	  }
        	  else
        	  {
        	  dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)] = *buf;
        	  }
          }

    }

  delete[] send_buffer;
  delete[] recv_buffer;
  delete[] send_buffer_2;
  delete[] recv_buffer_2;
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
	  if (monofluide_variable_)
		  update_monofluide_to_phase_value();

      double Lx =  splitting.get_grid_geometry().get_domain_length(0);
      IJK_Splitting::Lx_for_shear_perio = Lx;
      double DX = Lx/nii ;
      double Shear_x_time = IJK_Splitting::shear_x_time_;
      offset_i = Shear_x_time/DX;


      // send left layer of real cells to right layer of virtual cells
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
      if (z_index != z_index_min)
        {
          exchange_data(pe_kmin_, 0, 0, 0, pe_kmax_, 0, 0, nkk, nii , njj , le_ghost);
        }
      else
        {
          exchange_data(pe_kmin_, 0, 0, 0, pe_kmax_, 0, 0, nkk, nii, njj, le_ghost, -offset_i, jump_i, 0);
        }

      if (z_index != z_index_max)
        {
          exchange_data(pe_kmax_, 0, 0, nkk - le_ghost, pe_kmin_, 0, 0, -le_ghost, nii, njj, le_ghost);
        }
      else
        {
          exchange_data(pe_kmax_, 0, 0, nkk - le_ghost, pe_kmin_, 0, 0, -le_ghost, nii, njj, le_ghost, offset_i, -jump_i, 0);
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


// change_to_sheared_reference_frame : advecte le champ par le cisaillement moyen with 4th order interpolation
// hypothese de Fourier : phi_new(x,y,z)=phi(x-tSz, y, z)
// avec t le temps et S le cisaillement.
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::change_to_u_prime_to_u(double sens, int loc, double jump_i)
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

              double x_deplacement = sens*jump_i/Lz*(xyz[2]-Lz/2.);

              tmptab(IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(i, j, k)) =
                IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(i, j, k) + x_deplacement;

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

// change_to_sheared_reference_frame : advecte le champ par le cisaillement moyen with 4th order interpolation
// hypothese de Fourier : phi_new(x,y,z)=phi(x-tSz, y, z)
// avec t le temps et S le cisaillement.
template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::redistribute_with_shear_domain_ft(const IJK_Field_double& input_field, double DU_perio, int dir)
{

  IJK_Splitting splitting_ns = input_field.get_splitting();
  double Lx =  splitting_ns.get_grid_geometry().get_domain_length(0);
  double Lz =  splitting_ns.get_grid_geometry().get_domain_length(2);
  const IJK_Splitting& splitting_ft_ = splitting_ref_.valeur();
  const int ni = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int nj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();
  _TYPE_ARRAY_ tmptab = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data();
  double DX = Lx/ni;
  int ni_ref = input_field.ni();
  for (int i = 0; i < ni; i++ )
    {
      for (int j = 0; j < nj; j++ )
        {
          for (int k = 0; k < nk; k++ )
            {
              Vecteur3 xyz = splitting_ft_.get_coords_of_dof(i,j,k,IJK_Splitting::FACES_I);
              if (dir==0)
                xyz = splitting_ft_.get_coords_of_dof(i,j,k,IJK_Splitting::FACES_I);
              else if (dir==1)
                xyz = splitting_ft_.get_coords_of_dof(i,j,k,IJK_Splitting::FACES_J);
              else if (dir==2)
                xyz = splitting_ft_.get_coords_of_dof(i,j,k,IJK_Splitting::FACES_K);
              else
                xyz = splitting_ft_.get_coords_of_dof(i,j,k,IJK_Splitting::ELEM);
              double x_deplacement = 0.;

              if (xyz[2]<=0)
                {
                  x_deplacement = IJK_Splitting::shear_x_time_;
                }
              else if (xyz[2]>=Lz)
                {
                  x_deplacement = -IJK_Splitting::shear_x_time_;
                }

              double istmp = i+x_deplacement/DX;

              int ifloorm2 = (int) round(istmp) - 2;
              int ifloorm1 = (int) round(istmp) - 1;
              int ifloor0 = (int) round(istmp);
              int ifloorp1 = (int) round(istmp) + 1;
              int ifloorp2 = (int) round(istmp) + 2;

              int x[5] = {ifloorm2, ifloorm1, ifloor0, ifloorp1, ifloorp2};

              ifloorm2 = (ifloorm2 % ni_ref + ni_ref) % ni_ref;
              ifloorm1 = (ifloorm1 % ni_ref + ni_ref) % ni_ref;
              ifloor0 = (ifloor0 % ni_ref + ni_ref) % ni_ref;
              ifloorp1 = (ifloorp1 % ni_ref + ni_ref) % ni_ref;
              ifloorp2 = (ifloorp2 % ni_ref + ni_ref) % ni_ref;

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

              if (xyz[2]<0)
                {
                  tmptab(IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(i, j, k)) -= DU_perio;
                }
              else if (xyz[2]>Lz)
                {
                  tmptab(IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(i, j, k)) +=DU_perio;
                }

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
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::fourth_order_interpolation_for_shear_periodicity(const int phase, const int send_i, const int send_j, const int send_k, const _TYPE_ istmp, const int real_size_i)
{

	  _TYPE_ x[5] = {(_TYPE_)send_i-2, (_TYPE_)send_i-1, (_TYPE_)send_i, (_TYPE_)send_i+1, (_TYPE_)send_i+2};
	  _TYPE_ y[5] = {(_TYPE_)0., (_TYPE_)0., (_TYPE_)0., (_TYPE_)0., (_TYPE_)0.};
	  if (phase==0)
	  {
	   y[0] = (_TYPE_)projection_liquide_(((send_i-2) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[1] = (_TYPE_)projection_liquide_(((send_i-1) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[2] = (_TYPE_)projection_liquide_(((send_i) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[3] = (_TYPE_)projection_liquide_(((send_i+1) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[4] = (_TYPE_)projection_liquide_(((send_i+2) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	  }
	  else if (phase==1)
	  {
	   y[0] = (_TYPE_)projection_vapeur_(((send_i-2) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[1] = (_TYPE_)projection_vapeur_(((send_i-1) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[2] = (_TYPE_)projection_vapeur_(((send_i) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[3] = (_TYPE_)projection_vapeur_(((send_i+1) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[4] = (_TYPE_)projection_vapeur_(((send_i+2) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	  }
	  else if (phase==2)
	  {
	   y[0] = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(((send_i-2) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[1] = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(((send_i-1) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[2] = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(((send_i) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[3] = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(((send_i+1) % real_size_i + real_size_i) % real_size_i, send_j, send_k);
	   y[4] = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(((send_i+2) % real_size_i + real_size_i) % real_size_i, send_j, send_k);

	  }
	  else
	  {
		  std::cout << "ce choix n existe pas" ;
		  Process::exit();

	  }

	  _TYPE_ a0 = y[0] / ((x[0] - x[1]) * (x[0] - x[2]) * (x[0] - x[3]) * (x[0] - x[4]));
	  _TYPE_ a1 = y[1] / ((x[1] - x[0]) * (x[1] - x[2]) * (x[1] - x[3]) * (x[1] - x[4]));
	  _TYPE_ a2 = y[2] / ((x[2] - x[0]) * (x[2] - x[1]) * (x[2] - x[3]) * (x[2] - x[4]));
	  _TYPE_ a3 = y[3] / ((x[3] - x[0]) * (x[3] - x[1]) * (x[3] - x[2]) * (x[3] - x[4]));
	  _TYPE_ a4 = y[4] / ((x[4] - x[0]) * (x[4] - x[1]) * (x[4] - x[2]) * (x[4] - x[3]));


	  return (a0 * ((istmp - x[1]) * (istmp - x[2]) * (istmp - x[3]) * (istmp - x[4]))
			   + a1 * ((istmp - x[0]) * (istmp - x[2]) * (istmp - x[3]) * (istmp - x[4]))
			   + a2 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[3]) * (istmp - x[4]))
			   + a3 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[2]) * (istmp - x[4]))
			   + a4 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[2]) * (istmp - x[3])));

}


template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::update_indicatrice(const IJK_Field_double & indic_ft, const int ft_extension)
{
	  // besoin d'indic_ft pour remplir les valeurs exactes de l'indicatrice dans les mailles ghost
	  // on veut se servir de la valeur exacte pour interpoler au mieux les grandeurs monofluides
	  if (monofluide_variable_)
	  {
	  int ghost = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ghost();
	  const int ni = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
	  const int nj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
	  const int nk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();

	  for (int k = -ghost; k < nk+ghost; k++)
	    {
	      for (int j = -ghost; j < nj+ghost; j++)
	        {
	          for (int i = -ghost; i < ni+ghost; i++)
	            {
	        	  indicatrice_(i,j,k) = indic_ft(i+ft_extension,j+ft_extension,k+ft_extension);
	            }
	        }
	    }
	  }
	  else
	  {
	  std::cout << "ne devrait pas être ici car variable non monofluide" << std::endl;
	  Process::exit();
	  }
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::update_monofluide_to_phase_value()
{
//	std::cout << " entree dans update_monofluide " << std::endl;
// remplissage des variable p_v et p_l avec les valeurs connues;
// Fast Inverse Distance Weighting (IDW) Interpolation
// cette methode n'est pas bonne. Il faut reconstruire les pression plus proprement, en prenant en compte la loi de Laplace a linterface etc.
// --> Reprendre les travaux d'Anamissia.
// Methode codee uniquement pour lexemple et la structure informatique... Voir si ca peut marcher.
// Pour les espaces virtuels, besoin uniquement sur les bords periodique, pas dans le coeur.
// Mais peut être utile de le faire partout pour post-traiter les pressions aux interfaces ?
	  if (monofluide_variable_)
	  {
  const IJK_Splitting& splitting_ = splitting_ref_.valeur();
  _TYPE_ Lx =  (_TYPE_) splitting_.get_grid_geometry().get_domain_length(0);
  _TYPE_ Ly =  (_TYPE_) splitting_.get_grid_geometry().get_domain_length(1);
  _TYPE_ Lz =  (_TYPE_) splitting_.get_grid_geometry().get_domain_length(2);
  const int ni = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int nj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();
  _TYPE_ beta = 20.;
  bool option_conserv_p_monofluide = false;
  bool option_bord_periok_uniquement = true;
  Vecteur3 xyz_cible(0.,0.,0.) ;
  int taille_boite_interpol = 6;

  for (int kcible = 0; kcible < nk; kcible++)
	{
	  if (kcible > 1 && kcible < nk-1 && option_bord_periok_uniquement)
		  continue;

	  for (int jcible = 0; jcible < nj; jcible++)
		{
		  for (int icible = 0; icible < ni; icible++)
			{
		      _TYPE_ num_l = 0.;
		      _TYPE_ denum_l = 0.;
		      _TYPE_ num_v = 0.;
		      _TYPE_ denum_v = 0.;
			xyz_cible = splitting_.get_coords_of_dof(icible,jcible,kcible,IJK_Splitting::ELEM);

			// on parcours ensuite une boite de taille_boite_interpol^3 autour de la cible pour interpoler a partir de ces valeurs les plus proches
		      for (int kk = kcible-taille_boite_interpol; kk < kcible+taille_boite_interpol; kk++)
		        {
		          for (int jj = jcible-taille_boite_interpol; jj < jcible+taille_boite_interpol; jj++)
		            {
		              for (int ii = icible-taille_boite_interpol; ii < icible+taille_boite_interpol; ii++)
		                {

		            	  int itable = ii ;
		            	  int jtable = jj ;
		            	  int ktable = kk ;

		                  if(splitting_.get_grid_geometry().get_periodic_flag(0))
		                    {
		            	    itable = (itable % ni + ni) % ni ;
		                    }
		                  if(splitting_.get_grid_geometry().get_periodic_flag(1))
		                    {
		            	    jtable = (jtable % nj + nj) % nj ;
		                    }
		                  if(splitting_.get_grid_geometry().get_periodic_flag(2))
		                    {
		            	    ktable = (ktable % nk + nk) % nk ;
		                    }

		                  if (itable<0 || itable>ni || jtable<0 || jtable>nj || ktable<0 || ktable>nk)
		                  {
		                	 continue;
		                  }

		            	  Vecteur3 xyz_reel = splitting_.get_coords_of_dof(itable,jtable,ktable,IJK_Splitting::ELEM);
		                  _TYPE_ dx = (_TYPE_) (xyz_reel[0]-xyz_cible[0]);
		                  _TYPE_ dy = (_TYPE_) (xyz_reel[1]-xyz_cible[1]);
		                  _TYPE_ dz = (_TYPE_) (xyz_reel[2]-xyz_cible[2]);

		                  if(splitting_.get_grid_geometry().get_periodic_flag(0))
		                    {
		                      if(dx > Lx / 2.)
		                        {
		                          dx = dx - Lx;
		                        }
		                      if (dx < - Lx / 2.)
		                        {
		                          dx = dx + Lx;
		                        }
		                    }
		                  if(splitting_.get_grid_geometry().get_periodic_flag(1))
		                    {
		                      if(dy > Ly / 2.)
		                        {
		                          dy = dy - Ly;
		                        }
		                      if (dy < - Ly / 2.)
		                        {
		                          dy = dy + Ly;
		                        }
		                    }
		                  if(splitting_.get_grid_geometry().get_periodic_flag(2))
		                    {
		                      if(dz > Lz / 2.)
		                        {
		                          dz = dz - Lz;
		                        }
		                      if (dz < - Lz / 2.)
		                        {
		                          dz = dz + Lz;
		                        }
		                    }


		                  if (indicatrice_(itable,jtable,ktable)==1. && dx*dx+dy*dy+dz*dz != 0.)
		                    {
		                      num_l += IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(itable, jtable, ktable)/(_TYPE_) std::pow(std::sqrt(dx*dx+dy*dy+dz*dz),beta);
		                      denum_l += (_TYPE_) 1./(_TYPE_) std::pow(std::sqrt(dx*dx+dy*dy+dz*dz),beta);
		                    }
		                  else if (indicatrice_(itable,jtable,ktable)==0. && dx*dx+dy*dy+dz*dz != 0.)
		                    {
		                      num_v += IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(itable, jtable, ktable)/(_TYPE_) std::pow(std::sqrt(dx*dx+dy*dy+dz*dz),beta);
		                      denum_v += (_TYPE_) 1./(_TYPE_) std::pow(std::sqrt(dx*dx+dy*dy+dz*dz),beta);
		                    }

		                }
		            }
		        }
		      if (indicatrice_(icible,jcible,kcible)==1.)
		      {
		    	  if (denum_v==0.)
		    	  {
				  std::cout << "Trop loin pour faire linterpolation avec la limite taille_boite_interpol --> valeur random " ;
				  projection_vapeur_(icible,jcible,kcible) =(_TYPE_)  1.e-5 ;
		    	  }
		    	  {
		      	  projection_vapeur_(icible,jcible,kcible) = num_v/denum_v ;
		    	  }

	      		  projection_liquide_(icible,jcible,kcible) = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(icible, jcible, kcible) ;

		      }
		      else if (indicatrice_(icible,jcible,kcible)==0.)
		      {
		    	  if (denum_l==0.)
		    	  {
				  std::cout << "Trop loin pour faire linterpolation avec la limite taille_boite_interpol --> valeur random " ;
				  projection_liquide_(icible,jcible,kcible) = (_TYPE_) 1.e-5 ;
		    	  }
		    	  {
		      	  projection_liquide_(icible,jcible,kcible) = num_l/denum_l ;
		    	  }

	      		  projection_vapeur_(icible,jcible,kcible) = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(icible, jcible, kcible) ;
		      }
		      else
		      {

		    	  if (denum_v==0.)
		    	  {
				  std::cout << "Trop loin pour faire linterpolation avec la limite taille_boite_interpol --> valeur random " ;
				  projection_vapeur_(icible,jcible,kcible) =(_TYPE_)  1.e-5 ;
		    	  }
		    	  {
		      	  projection_vapeur_(icible,jcible,kcible) = num_v/denum_v ;
		    	  }
		    	  if (denum_l==0.)
		    	  {
				  std::cout << "Trop loin pour faire linterpolation avec la limite taille_boite_interpol --> valeur random " ;
				  projection_liquide_(icible,jcible,kcible) =(_TYPE_)  1.e-5 ;
		    	  }
		    	  {
		      	  projection_liquide_(icible,jcible,kcible) = num_l/denum_l ;
		    	  }

		      }
		      if (option_conserv_p_monofluide)
		        {
		          // on interpole la pression vapeur qui est moins variable en proche interface (interpolation plus safe, et on deduit p_l a partir de la valeur monofluide)
		    	  // on fait l inverse uniquement pour les toutes petites valeur de l indicatrice (<10%), sinon divergence de la valeur p_l
		    	  _TYPE_ Il = indicatrice_(icible,jcible,kcible);
		    	  _TYPE_ Iv =(_TYPE_)1. - Il;
		    	  _TYPE_ P = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(icible,jcible,kcible);
		    	  if (Il > 0.5)
		    	  {
		    	  projection_liquide_(icible,jcible,kcible) = (P - Iv*num_v/denum_v)/Il ;
		    	  }
		    	  else
		    	  {
		    	  projection_vapeur_(icible,jcible,kcible) = (P - Il*num_l/denum_l)/Iv ;
		    	  }
		        }
			}
		}
	}
	  }
	  else
	  {
	  std::cout << "ne devrait pas être ici car variable non monofluide" << std::endl;
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
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::allocate(const IJK_Splitting& splitting, IJK_Splitting::Localisation loc, int ghost_size, int additional_k_layers, int ncompo, bool external_storage, bool monofluide)
{
  const int ni_local = splitting.get_nb_items_local(loc, 0);
  const int nj_local = splitting.get_nb_items_local(loc, 1);
  const int nk_local = splitting.get_nb_items_local(loc, 2);
  monofluide_variable_ = monofluide;
  IJK_Field_local_template<_TYPE_, _TYPE_ARRAY_>::allocate(ni_local, nj_local, nk_local, ghost_size, additional_k_layers, ncompo);
  if (monofluide_variable_)
  {
	  projection_liquide_.allocate(ni_local, nj_local, nk_local, ghost_size, additional_k_layers, ncompo);
	  projection_vapeur_.allocate(ni_local, nj_local, nk_local, ghost_size, additional_k_layers, ncompo);
	  indicatrice_.allocate(ni_local, nj_local, nk_local, ghost_size, additional_k_layers, ncompo);
  }
  splitting_ref_ = splitting;
  localisation_ = loc;
}


#endif /* IJK_Field_template_TPP_H */
