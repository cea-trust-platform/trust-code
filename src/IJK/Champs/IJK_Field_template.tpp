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

template<typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::exchange_data(int pe_send_, /* processor to send to */
                                                             int is, int js, int ks, /* ijk coordinates of first data to send */
                                                             int pe_recv_, /* processor to recv from */
                                                             int ir, int jr, int kr, /* ijk coordinates of first data to recv */
                                                             int isz, int jsz, int ksz, /* size of block data to send/recv */
															 double offset)  /* decallage a appliquer pour la condition de shear periodique*/
{

	// si un seul proc, besoin quand meme d'un echange
	if (pe_send_ == Process::me() && pe_recv_ == Process::me())
    {
      // a changer si decoupage en i

      // Self (periodicity on same processor)
      _TYPE_ *dest = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::data().addr();
      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++)
          {
        	  _TYPE_ offset_i = (_TYPE_) offset;
        	  _TYPE_ istmp = (_TYPE_)is + (_TYPE_)i + offset_i;
              int ifloor = (int) floor(istmp);
              ifloor = ifloor % isz;
              int iceil = (int) ceil(istmp) ;
              iceil = iceil % isz ;
              _TYPE_ weight = (_TYPE_)(istmp - floor(istmp));
              _TYPE_ vmin = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(ifloor, js + j, ks + k);
              _TYPE_ vmax = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(iceil, js + j, ks + k);

              dest[IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::linear_index(ir + i, jr + j, kr + k)] =
            		  ((_TYPE_)1.- weight) * vmin + weight * vmax;
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
      // Pack send data
      _TYPE_ *buf = send_buffer;

      for (int k = 0; k < ksz; k++)
        for (int j = 0; j < jsz; j++)
          for (int i = 0; i < isz; i++, buf++)
          {
        	  // is, js, ks, coordone du premier element a envoyer
        	  // ajouter un offset pour aller chercher un element decalle
        	  // le faire uniquement si le proc na pas de voisin, donc que cest un bord (methode dispo dans splitting)
        	  // si je m'en sort pas, demander a Anida en lui donnant un cas test
        	  // changer uniquement a cet endroit (valeur envoyée dans le buffer), rien ne change pour la reception.
        	  _TYPE_ offset_i = (_TYPE_) offset;
        	  _TYPE_ istmp = (_TYPE_)is + (_TYPE_)i + offset_i;
              int ifloor = (int) floor(istmp);
              ifloor = ifloor % isz;
              int iceil = (int) ceil(istmp) ;
              iceil = iceil % isz ;
              _TYPE_ weight = (_TYPE_)(istmp - floor(istmp));
              _TYPE_ vmin = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(ifloor, js + j, ks + k);
              _TYPE_ vmax = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::operator()(iceil, js + j, ks + k);

            *buf = ((_TYPE_)1.- weight) * vmin + weight * vmax;
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
void IJK_Field_template<_TYPE_, _TYPE_ARRAY_>::echange_espace_virtuel(int le_ghost, double Shear_x_time)
{
  statistiques().begin_count(echange_vect_counter_);
  assert(le_ghost <= (IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ghost()));
  // Exchange in i direction real cells,
  // then in j direction with ghost cells in i,
  // then in k direction, with ghost cells in i and j

  // dans un premier temps, pas de splitting en i pour avoir a gerer que les voisins
  // sinon il faudra faire des choses compliquees et potentiellement lourdes, appeler les voisins des voisins etc..
  const IJK_Splitting& splitting = splitting_ref_.valeur();
  int pe_imin_ = splitting.get_neighbour_processor(0, 0);
  int pe_imax_ = splitting.get_neighbour_processor(1, 0);
  int pe_jmin_ = splitting.get_neighbour_processor(0, 1);
  int pe_jmax_ = splitting.get_neighbour_processor(1, 1);

  // si pe_kmin_ = -1 >> bord z=0 (mais peut-être pas pour un domaine periodique....)
  // si pe_kmax = -1 >> bord z=Lz
  // si marche pas en perio, creer une fonction dans le splitting pour reperer les procs en question.
  int pe_kmin_ = splitting.get_neighbour_processor(0, 2);
  int pe_kmax_ = splitting.get_neighbour_processor(1, 2);
  int z_index = splitting.get_local_slice_index(2);
  int z_index_min = 0;
  int z_index_max = splitting.get_nprocessor_per_direction(2) - 1;

  const int nii = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::ni();
  const int njj = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nj();
  const int nkk = IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_>::nk();
  double Lz =  splitting.get_coords_of_dof(nii,njj,nkk,IJK_Splitting::FACES_K)[2];
  double Lx =  splitting.get_coords_of_dof(nii,njj,nkk,IJK_Splitting::FACES_I)[0];
  double DX = Lx/nii ;
  double offset_i = Shear_x_time*Lz/DX;
  // get_neighbour_processor renvoie -1 sur pas de proc (bord du domaine). Appeler dans ce cas pour la condition de cisaillement perio

  // send left layer of real cells to right layer of virtual cells
  exchange_data(pe_imin_, 0, 0, 0, pe_imax_, nii, 0, 0, le_ghost, njj, nkk); /* size of block data to send */

  // send right real cells to left virtual cells
  exchange_data(pe_imax_, nii - le_ghost, 0, 0, pe_imin_, -le_ghost, 0, 0, le_ghost, njj, nkk);

  exchange_data(pe_jmin_, -le_ghost, 0, 0, pe_jmax_, -le_ghost, njj, 0, nii + 2 * le_ghost, le_ghost, nkk);

  exchange_data(pe_jmax_, -le_ghost, njj - le_ghost, 0, pe_jmin_, -le_ghost, -le_ghost, 0, nii + 2 * le_ghost, le_ghost, nkk);

  if (z_index == z_index_min)
  {
  exchange_data(pe_kmin_, -le_ghost, -le_ghost, 0, pe_kmax_, -le_ghost, -le_ghost, nkk, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost, -offset_i);
  }
  else
  {
  exchange_data(pe_kmin_, -le_ghost, -le_ghost, 0, pe_kmax_, -le_ghost, -le_ghost, nkk, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost);
  }

  if (z_index == z_index_max)
  {
	  exchange_data(pe_kmax_, -le_ghost, -le_ghost, nkk - le_ghost, pe_kmin_, -le_ghost, -le_ghost, -le_ghost, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost, offset_i);
  }
  else
  {
	  exchange_data(pe_kmax_, -le_ghost, -le_ghost, nkk - le_ghost, pe_kmin_, -le_ghost, -le_ghost, -le_ghost, nii + 2 * le_ghost, njj + 2 * le_ghost, le_ghost);
  }




  statistiques().end_count(echange_vect_counter_);
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
