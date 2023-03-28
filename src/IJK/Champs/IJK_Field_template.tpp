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
															 double offset, double jump_i)  /* decallage a appliquer pour la condition de shear periodique*/
{
	//jump_i = 0.;
	if (pe_send_ == Process::me() && pe_recv_ == Process::me())
    {
      // Self (periodicity on same processor)
      _TYPE_ *dest = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data().addr();
      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++)
          {

        	  _TYPE_ jump = (_TYPE_) jump_i;
        	  _TYPE_ offset_i = (_TYPE_) offset;
        	  _TYPE_ istmp = (_TYPE_)i + offset_i;

        	  int ifloorm2 = (int) floor(istmp) - 2;
        	  int ifloorm1 = (int) floor(istmp) - 1;
        	  int ifloor0 = (int) floor(istmp);
        	  int ifloorp1 = (int) floor(istmp) + 1;
        	  int ifloorp2 = (int) floor(istmp) + 2;
//        	  int iceil = (int) ceil(istmp) ;
        	  _TYPE_ x[5] = {(_TYPE_)ifloorm2, (_TYPE_)ifloorm1, (_TYPE_)ifloor0, (_TYPE_)ifloorp1, (_TYPE_)ifloorp2};

        	  ifloorm2 = (ifloorm2 % isz + isz) % isz;
        	  ifloorm1 = (ifloorm1 % isz + isz) % isz;
        	  ifloor0 = (ifloor0 % isz + isz) % isz;
        	  ifloorp1 = (ifloorp1 % isz + isz) % isz;
        	  ifloorp2 = (ifloorp2 % isz + isz) % isz;

//        	  iceil = (iceil % isz + isz) % isz;

//              _TYPE_ weight = (_TYPE_)(istmp - floor(istmp));
//              _TYPE_ vmin = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloor, js + j, ks + k);
//              _TYPE_ vmax = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + iceil, js + j, ks + k);



              _TYPE_ y[5] = {IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloorm2, js + j, ks + k),
            		         IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloorm1, js + j, ks + k),
            		         IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloor0, js + j, ks + k),
            		         IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloorp1, js + j, ks + k),
            		         IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloorp2, js + j, ks + k)};


              _TYPE_ a0 = y[0] / ((x[0] - x[1]) * (x[0] - x[2]) * (x[0] - x[3]) * (x[0] - x[4]));
              _TYPE_ a1 = y[1] / ((x[1] - x[0]) * (x[1] - x[2]) * (x[1] - x[3]) * (x[1] - x[4]));
              _TYPE_ a2 = y[2] / ((x[2] - x[0]) * (x[2] - x[1]) * (x[2] - x[3]) * (x[2] - x[4]));
              _TYPE_ a3 = y[3] / ((x[3] - x[0]) * (x[3] - x[1]) * (x[3] - x[2]) * (x[3] - x[4]));
              _TYPE_ a4 = y[4] / ((x[4] - x[0]) * (x[4] - x[1]) * (x[4] - x[2]) * (x[4] - x[3]));

			 // Evaluate the interpolation polynomial at istmp

              dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)] =
            		  a0 * ((istmp - x[1]) * (istmp - x[2]) * (istmp - x[3]) * (istmp - x[4]))
            		  							 + a1 * ((istmp - x[0]) * (istmp - x[2]) * (istmp - x[3]) * (istmp - x[4]))
            		  							 + a2 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[3]) * (istmp - x[4]))
            		  							 + a3 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[2]) * (istmp - x[4]))
            		  							 + a4 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[2]) * (istmp - x[3])) + jump;

//              if ((loc == 3 or loc==2) && offset < 0. && offset !=0. && j==4 && k==0)
//              {
//            	  std::cout<<"compo= " << loc <<std::endl;
//            	  std::cout<<"offset= " << offset <<std::endl;
//            	  std::cout<<"weight= " << weight <<std::endl;
//            	  std::cout<<"i,j,k= " << i <<" " << j <<" " << k  <<std::endl;
//				std::cout<<"is,js,ks= " << is <<" " << js <<" " << ks  <<std::endl;
//				std::cout<<"isz,jsz,ksz= " << isz <<" " << jsz <<" " << ksz  <<std::endl;
//				std::cout<<"is+1,js+j,ks+k= " << is+i <<" " << js+j <<" " << ks+k  <<std::endl;
//				std::cout<<"iceil= " << iceil <<"ifloor= " << ifloor <<" is+i= " << is + i <<std::endl;
//				std::cout<<" vmin= " << vmin <<" vmax= " << vmax <<std::endl;
//				std::cout<<" v_sans_shear= " << IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is+i, js + j, ks + k) <<std::endl;
//				std::cout<<" v_avec_shear= " << dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)] <<std::endl;
//
//				std::cout<<"---------------" <<std::endl;
//              }

              //dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)] = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + i, js + j, ks + k) + jump;

          }

//      std::cout<<" " <<std::endl;
//      std::cout<<" " <<std::endl;
//      std::cout<<" " <<std::endl;
       return;
    }
  const int data_size = isz * jsz * ksz;
  const int type_size = sizeof(_TYPE_);
  _TYPE_ *send_buffer = 0;
  _TYPE_ *recv_buffer = 0;

  if (pe_send_ >= 0)
    {
      send_buffer = new _TYPE_[data_size];
      // Pack send data
      _TYPE_ *buf = send_buffer;

      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++, buf++)
          {
        	  _TYPE_ jump = (_TYPE_) jump_i;
        	  _TYPE_ offset_i = (_TYPE_) offset;
        	  _TYPE_ istmp = (_TYPE_)i + offset_i;

        	  int ifloorm2 = (int) floor(istmp) - 2;
        	  int ifloorm1 = (int) floor(istmp) - 1;
        	  int ifloor0 = (int) floor(istmp);
        	  int ifloorp1 = (int) floor(istmp) + 1;
        	  int ifloorp2 = (int) floor(istmp) + 2;
//        	  int iceil = (int) ceil(istmp) ;
        	  _TYPE_ x[5] = {(_TYPE_)ifloorm2, (_TYPE_)ifloorm1, (_TYPE_)ifloor0, (_TYPE_)ifloorp1, (_TYPE_)ifloorp2};

        	  ifloorm2 = (ifloorm2 % isz + isz) % isz;
        	  ifloorm1 = (ifloorm1 % isz + isz) % isz;
        	  ifloor0 = (ifloor0 % isz + isz) % isz;
        	  ifloorp1 = (ifloorp1 % isz + isz) % isz;
        	  ifloorp2 = (ifloorp2 % isz + isz) % isz;

//        	  iceil = (iceil % isz + isz) % isz;

//              _TYPE_ weight = (_TYPE_)(istmp - floor(istmp));
//              _TYPE_ vmin = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloor, js + j, ks + k);
//              _TYPE_ vmax = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + iceil, js + j, ks + k);



              _TYPE_ y[5] = {IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloorm2, js + j, ks + k),
            		         IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloorm1, js + j, ks + k),
            		         IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloor0, js + j, ks + k),
            		         IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloorp1, js + j, ks + k),
            		         IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(is + ifloorp2, js + j, ks + k)};


              _TYPE_ a0 = y[0] / ((x[0] - x[1]) * (x[0] - x[2]) * (x[0] - x[3]) * (x[0] - x[4]));
              _TYPE_ a1 = y[1] / ((x[1] - x[0]) * (x[1] - x[2]) * (x[1] - x[3]) * (x[1] - x[4]));
              _TYPE_ a2 = y[2] / ((x[2] - x[0]) * (x[2] - x[1]) * (x[2] - x[3]) * (x[2] - x[4]));
              _TYPE_ a3 = y[3] / ((x[3] - x[0]) * (x[3] - x[1]) * (x[3] - x[2]) * (x[3] - x[4]));
              _TYPE_ a4 = y[4] / ((x[4] - x[0]) * (x[4] - x[1]) * (x[4] - x[2]) * (x[4] - x[3]));

			 // Evaluate the interpolation polynomial at x_val
              *buf = a0 * ((istmp - x[1]) * (istmp - x[2]) * (istmp - x[3]) * (istmp - x[4]))
							 + a1 * ((istmp - x[0]) * (istmp - x[2]) * (istmp - x[3]) * (istmp - x[4]))
							 + a2 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[3]) * (istmp - x[4]))
							 + a3 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[2]) * (istmp - x[4]))
							 + a4 * ((istmp - x[0]) * (istmp - x[1]) * (istmp - x[2]) * (istmp - x[3])) + jump;




//            *buf = ((_TYPE_)1.- weight) * vmin + weight * vmax ;
    }
    }
  if (pe_recv_ >= 0)
    recv_buffer = new _TYPE_[data_size];

  ::envoyer_recevoir(send_buffer, data_size * type_size, pe_send_, recv_buffer, data_size * type_size, pe_recv_);
  if (pe_recv_ >= 0)
    {
      // Unpack recv data
      _TYPE_ *buf = recv_buffer;
      _TYPE_ *dest = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data().addr();
      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++, buf++)
            dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)] = *buf;
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

  // if pe_kmin_ = -1 >> boundary z=0
  // if pe_kmax = -1 >> boundary z=Lz
  int pe_kmin_ = splitting.get_neighbour_processor(0, 2);
  int pe_kmax_ = splitting.get_neighbour_processor(1, 2);
  int z_index = splitting.get_local_slice_index(2);
  int z_index_min = 0;
  int z_index_max = splitting.get_nprocessor_per_direction(2) - 1;

  const int nii = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int njj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nkk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();

  // calcul du decallage offset_i
  // correspond au nombre de maille en x qui separe deux elements periodique sur le bord zmin, zmax
  double offset_i=0.;
  if (IJK_Splitting::defilement_ == 1)
  {
	  double Lx =  splitting.get_coords_of_dof(nii,njj,nkk,IJK_Splitting::FACES_I)[0];
	  double DX = Lx/nii ;
	  double Shear_x_time = IJK_Splitting::shear_x_time_;
	  offset_i = Shear_x_time/DX;
  }

  // send left layer of real cells to right layer of virtual cells
  exchange_data(pe_imin_, 0, 0, 0, pe_imax_, nii, 0, 0, le_ghost, njj, nkk); /* size of block data to send */

  // send right real cells to left virtual cells
  exchange_data(pe_imax_, nii - le_ghost, 0, 0, pe_imin_, -le_ghost, 0, 0, le_ghost, njj, nkk);

  exchange_data(pe_jmin_, -le_ghost, 0, 0, pe_jmax_, -le_ghost, njj, 0, nii + 2 * le_ghost, le_ghost, nkk);

  exchange_data(pe_jmax_, -le_ghost, njj - le_ghost, 0, pe_jmin_, -le_ghost, -le_ghost, 0, nii + 2 * le_ghost, le_ghost, nkk);

  if (z_index == z_index_min)
  {
  // modification des blocs echanges dans le cas de condition de shear periodique pour ne prendre d'abord que l espace reel
  // on envoit ensuite les deux blocs fantomes --> je pense que ca ne change rien et que le code ne se sert pas des donnees a cet endroit
  exchange_data(pe_kmin_, 0, 0, 0, pe_kmax_, 0, 0, nkk, nii , njj , le_ghost, -offset_i, jump_i);
  exchange_data(pe_kmin_, -le_ghost, -le_ghost, 0, pe_kmax_, -le_ghost, -le_ghost, nkk, le_ghost , le_ghost , le_ghost);
  exchange_data(pe_kmin_, nii, -le_ghost, 0, pe_kmax_, nii, -le_ghost, nkk, le_ghost , le_ghost , le_ghost);
  }
  else
  {
  exchange_data(pe_kmin_, -le_ghost, -le_ghost, 0, pe_kmax_, -le_ghost, -le_ghost, nkk, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost);
}

  if (z_index == z_index_max)
  {
	  // modification des blocs echanges dans le cas de condition de shear periodique pour ne prendre d'abord que l espace reel
	  // on envoit ensuite les deux blocs fantomes --> je pense que ca ne change rien et que le code ne se sert pas des donnees a cet endroit
	  exchange_data(pe_kmax_, 0, 0, nkk- le_ghost , pe_kmin_, 0, 0, -le_ghost, nii , njj , le_ghost, offset_i, -jump_i);
	  exchange_data(pe_kmax_, -le_ghost, -le_ghost, nkk- le_ghost, pe_kmin_, -le_ghost, -le_ghost, -le_ghost, le_ghost , le_ghost , le_ghost);
	  exchange_data(pe_kmax_, nii, -le_ghost, nkk- le_ghost, pe_kmin_, nii, -le_ghost, -le_ghost, le_ghost , le_ghost , le_ghost);

  }
  else
  {
	  exchange_data(pe_kmax_, -le_ghost, -le_ghost, nkk - le_ghost, pe_kmin_, -le_ghost, -le_ghost, -le_ghost, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost);
  }




  statistiques().end_count(echange_vect_counter_);
}


// change_to_sheared_reference_frame advecte le champ par le cisaillement moyen
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
	  double Lz =  splitting.get_coords_of_dof(ni,nj,nk,IJK_Splitting::FACES_K)[2];
	  double Lx =  splitting.get_coords_of_dof(ni,nj,nk,IJK_Splitting::FACES_I)[0];
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
						  IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(ifloorp2, j, k)};


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
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::allocate(const IJK_Splitting& splitting, IJK_Splitting::Localisation loc, int ghost_size, int additional_k_layers, int ncompo, bool external_storage)
{
  const int ni_local = splitting.get_nb_items_local(loc, 0);
  const int nj_local = splitting.get_nb_items_local(loc, 1);
  const int nk_local = splitting.get_nb_items_local(loc, 2);
  IJK_Field_local_template<_TYPE_, _TYPE_ARRAY_>::allocate(ni_local, nj_local, nk_local, ghost_size, additional_k_layers, ncompo);
  splitting_ref_ = splitting;
  localisation_ = loc;
}


#endif /* IJK_Field_template_TPP_H */
