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

#ifndef IJK_Striped_Writer_TPP_H
#define IJK_Striped_Writer_TPP_H

#include <Parallel_io_parameters.h>
#include <IJK_Splitting.h>
#include <SFichier.h>
#include <Schema_Comm.h>

// Returns the number of written values
template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
long long IJK_Striped_Writer::write_data_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f)
{
  if (Parallel_io_parameters::get_max_block_size() > 0)
    return write_data_parallel_template<_OUT_TYPE_,_TYPE_, _TYPE_ARRAY_>(filename, f);

  if (f.get_localisation() != IJK_Splitting::ELEM && f.get_localisation() != IJK_Splitting::NODES)
    {
      Cerr << "Error in  IJK_Striped_Writer::write_data_template(scalar field): provided field has unsupported localisation" << finl;
      Process::exit();
    }
  // Collate data on processor 0
  const IJK_Splitting& splitting = f.get_splitting();
  const IJK_Splitting::Localisation loc = f.get_localisation();
  const int nitot = splitting.get_nb_items_global(loc, DIRECTION_I);
  const int njtot = splitting.get_nb_items_global(loc, DIRECTION_J);
  const int nktot = splitting.get_nb_items_global(loc, DIRECTION_K);
  const int ncompo = 1;

  TRUSTArray<_OUT_TYPE_> tmp;
  if (Process::je_suis_maitre())
    tmp.resize_array(nitot*njtot*nktot);
  redistribute(f, tmp, nitot, njtot, nktot, ncompo, 0);
  if (Process::je_suis_maitre())
    {
      SFichier binary_file;
      binary_file.set_bin(true);
      binary_file.set_64b(false);  // In IJK, nothing needs to be written in 64b:
      binary_file.ouvrir(filename);
      binary_file.put(tmp.addr(), tmp.size_array(), 1);
      binary_file.close();
    }
  return tmp.size_array();
}

// Write 3 velocity components at faces in "lata" format:
//  write ni*nj*nk lines of data with
//    ni, nj, nk = number of nodes in the domain
//    line of data = 3 values. 1st value = component i of velocity on left face of element (i,j,k), 2nd value = component j, etc...
// Returns the number of written values (1 value = 3 scalars)
template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
long long IJK_Striped_Writer::write_data_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz)
{
  if (Parallel_io_parameters::get_max_block_size() > 0)
    return write_data_parallel_template<_OUT_TYPE_, _TYPE_, _TYPE_ARRAY_>(filename, vx, vy, vz);

  if (vx.get_localisation() != IJK_Splitting::FACES_I
      || vy.get_localisation() != IJK_Splitting::FACES_J
      || vz.get_localisation() != IJK_Splitting::FACES_K)
    {
      Cerr << "Error in  IJK_Striped_Writer::write_data_template(vx, vy, vz): provided fields have incorrect localisation" << finl;
      Process::exit();
    }
  const IJK_Splitting& splitting = vx.get_splitting();
  // Collate data on processor 0
  // In lata format, the velocity is written as an array of (vx, vy, vz) vectors.
  // Size of the array is the total number of nodes in the mesh.
  // The velocity associated with a node is the combination of velocities at the faces
  // at the right of the node (in each direction).
  const int nitot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 0) + 1;
  const int njtot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 1) + 1;
  const int nktot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2) + 1;
  const int nbcompo = 3;

  TRUSTArray<_OUT_TYPE_> tmp;
  if (Process::je_suis_maitre())
    tmp.resize_array(nitot*njtot*nktot*nbcompo);
  // Modif Martin
  //tmp=-123456789;
  tmp=0;
  redistribute(vx, tmp, nitot, njtot, nktot, nbcompo, 0);
  redistribute(vy, tmp, nitot, njtot, nktot, nbcompo, 1);
  redistribute(vz, tmp, nitot, njtot, nktot, nbcompo, 2);

  if (Process::je_suis_maitre())
    {
      SFichier binary_file;
      binary_file.set_bin(true);
      binary_file.set_64b(false);  // In IJK, nothing needs to be written in 64b:
      binary_file.ouvrir(filename);
      binary_file.put(tmp.addr(), tmp.size_array(), 1);
      binary_file.close();
    }
  return tmp.size_array() / 3;

}

// Density fields are written by more than one processor. Each processor write a file.
template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
long long IJK_Striped_Writer::write_data_parallele_plan_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f)
{
  if (f.get_localisation() != IJK_Splitting::ELEM && f.get_localisation() != IJK_Splitting::NODES)
    {
      Cerr << "Error in  IJK_Striped_Writer::write_data_parallele_plan_tempalte(scalar field): provided field has unsupported localisation" << finl;
      Process::exit();
    }

  const IJK_Splitting& splitting = f.get_splitting();
  const int nitot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 0);
  const int njtot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 1);
  const int nktot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2);
  const int ni = splitting.get_nb_items_local(IJK_Splitting::ELEM, 0);
  const int nj = splitting.get_nb_items_local(IJK_Splitting::ELEM, 1);
  const int nk = splitting.get_nb_items_local(IJK_Splitting::ELEM, 2);

  TRUSTArray<_OUT_TYPE_> tmp;
  tmp.resize_array(ni*nj*nk);
  for (int k = 0; k < nk; k++)
    for (int j = 0; j < nj; j++)
      for (int i = 0; i < ni; i++)
        tmp[(k*nj+j)*ni+i] = (_OUT_TYPE_)f(i,j,k);

  SFichier binary_file;
  binary_file.set_bin(true);
  binary_file.set_64b(false); // In IJK, nothing needs to be written in 64b:
  binary_file.ouvrir(filename);
  binary_file.put(tmp.addr(), tmp.size_array(), 1);
  binary_file.close();
  return nitot*njtot*nktot;
}

// Velocity fields are written by more than one processor. Each processor write a file.
template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
long long IJK_Striped_Writer::write_data_parallele_plan_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz)
{
  if (vx.get_localisation() != IJK_Splitting::FACES_I
      || vy.get_localisation() != IJK_Splitting::FACES_J
      || vz.get_localisation() != IJK_Splitting::FACES_K)
    {
      Cerr << "Error in  IJK_Striped_Writer::write_data_parallele_plan_template(vx, vy, vz): provided fields have incorrect localisation" << finl;
      Process::exit();
    }
  const IJK_Splitting& splitting = vx.get_splitting();
  const int nitot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 0) + 1;
  const int njtot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 1) + 1;
  const int nktot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2) + 1;

  // In lata format, the velocity is written as an array of (vx, vy, vz) vectors.
  // Size of the array is the local number of nodes in the mesh.
  // The velocity associated with a node is the combination of velocities at the faces
  // at the right of the node (in each direction).

  int ni = splitting.get_nb_items_local(IJK_Splitting::ELEM, 0);
  if ( (splitting.get_local_slice_index(0) == splitting.get_nprocessor_per_direction(0) - 1) )
    {
      ni++;
    }

  int nj = splitting.get_nb_items_local(IJK_Splitting::ELEM, 1);
  if ( (splitting.get_local_slice_index(1) == splitting.get_nprocessor_per_direction(1) - 1) )
    {
      nj++;
    }

  int nk = splitting.get_nb_items_local(IJK_Splitting::ELEM, 2);
  if ( splitting.get_local_slice_index(2) == splitting.get_nprocessor_per_direction(2) - 1)
    {
      nk++;
    }
  const int nbcompo = 3;

  TRUSTArray<_OUT_TYPE_> tmp;
  tmp.resize_array(ni*nj*nk*nbcompo);
  //tmp=-123456789;
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              tmp[((k*nj+j)*ni+i)*nbcompo+0] = (_OUT_TYPE_)vx(i,j,k);
              tmp[((k*nj+j)*ni+i)*nbcompo+1] = (_OUT_TYPE_)vy(i,j,k);
              tmp[((k*nj+j)*ni+i)*nbcompo+2] = (_OUT_TYPE_)vz(i,j,k);
            }
        }
    }

  SFichier binary_file;
  binary_file.set_bin(true);
  binary_file.set_64b(false); // In IJK, nothing needs to be written in 64b:
  binary_file.ouvrir(filename);
  binary_file.put(tmp.addr(), tmp.size_array(), 1);
  binary_file.close();
  return nitot*njtot*nktot;
}


// Redistribute data from input (distributed ijk scalar field) to ouput
// (striped linear storage)
// les n_compo_tot sont inutiles ici !!!
template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Striped_Writer::redistribute(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& input, TRUSTArray<_OUT_TYPE_>& output,
                                      const int nitot, const int njtot, const int nktot, const int nbcompo, const int component)
{
  const IJK_Splitting& splitting = input.get_splitting();
  // For the moment, assume data is collected on mpi process 0
  // mpi processes send data to master
  TRUSTArray<_OUT_TYPE_> sendtmp;
  int ni = input.ni();
  int nj = input.nj();
  int nk = input.nk();
  // For data localized at faces and periodic box, write value of the rightmost face (which is not stored internally)
  // (that supposes that virtual cell available and uptodate, if not, written data at the right end will not reflect the periodicity)
  if (input.ghost() > 0 && input.get_localisation() != IJK_Splitting::ELEM)
    {
      const IJK_Grid_Geometry& geom = splitting.get_grid_geometry();
      // if periodic and we are at the right end the domain:
      // MODIFS GABRIEL : il faut boucler sur les 3 directions !!
      for (int dir = 0; dir < 3 /* pas 2 */; dir++)
        {
          if (geom.get_periodic_flag(dir) && splitting.get_local_slice_index(dir) == splitting.get_nprocessor_per_direction(dir) - 1)
            {
              int& n = (dir==0)?ni:((dir==1)?nj:nk);
              n++;
            }
        }
    }
  sendtmp.resize_array(ni * nj * nk); // allocate and store zero in array
  for (int k = 0; k < nk; k++)
    for (int j = 0; j < nj; j++)
      for (int i = 0; i < ni; i++)
        sendtmp[(k*nj+j)*ni+i] = (_OUT_TYPE_)input(i,j,k);

  // Some processors might have empty domain, will not receive any message so do not send !
  if (!Process::je_suis_maitre() && splitting.get_nb_elem_local(0) > 0)
    {
      envoyer(splitting.get_offset_local(0), 0, 0);
      envoyer(ni, 0, 0);
      envoyer(splitting.get_offset_local(1), 0, 0);
      envoyer(nj, 0, 0);
      envoyer(splitting.get_offset_local(2), 0, 0);
      envoyer(nk, 0, 0);
      envoyer(sendtmp, 0, 0);
    }

  if (Process::je_suis_maitre())
    {
      // Master mpi process collects the data
      IntTab mapping;
      splitting.get_processor_mapping(mapping);
      TRUSTArray<_OUT_TYPE_> recv_tmp;
      for (int kproc = 0; kproc < mapping.dimension(2); kproc++)
        {
          for (int jproc = 0; jproc < mapping.dimension(1); jproc++)
            {
              for (int iproc = 0; iproc < mapping.dimension(0); iproc++)
                {
                  const int numproc = mapping(iproc, jproc, kproc);
                  int imin2, jmin2, kmin2, ni2, nj2, nk2;
                  if (numproc == Process::me())
                    {
                      recv_tmp = sendtmp;
                      imin2 = splitting.get_offset_local(0);
                      jmin2 = splitting.get_offset_local(1);
                      kmin2 = splitting.get_offset_local(2);
                      ni2 = ni;
                      nj2 = nj;
                      nk2 = nk;
                    }
                  else
                    {
                      recevoir(imin2,numproc,0);
                      recevoir(ni2,numproc,0);
                      recevoir(jmin2,numproc,0);
                      recevoir(nj2,numproc,0);
                      recevoir(kmin2,numproc,0);
                      recevoir(nk2,numproc,0);
                      recevoir(recv_tmp, numproc,0);
                    }

                  for (int k = 0; k < nk2; k++)
                    for (int j = 0; j < nj2; j++)
                      for (int i = 0; i < ni2; i++)
                        output[(((k+kmin2)*njtot+(j+jmin2))*nitot+i+imin2)*nbcompo + component] = recv_tmp[(k*nj2+j)*ni2+i];
                }
            }
        }
    }
}

// les n_compo_tot sont inutiles ici !!!
template<typename _IN_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Striped_Writer::redistribute_load(const TRUSTArray<_IN_TYPE_>& input, IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& output,
                                           const int nitot, const int njtot, const int nktot, const int nbcompo, const int component)
{
  const IJK_Splitting& splitting = output.get_splitting();
  // For the moment, assume data is collected on mpi process 0
  // mpi processes send data to master
  int ni = output.ni();
  int nj = output.nj();
  int nk = output.nk();

  // Some processors might have empty domain, will not receive any message so do not send !
  if (!Process::je_suis_maitre() && splitting.get_nb_elem_local(0) > 0)
    {
      envoyer(splitting.get_offset_local(0), 0, 0);
      envoyer(ni, 0, 0);
      envoyer(splitting.get_offset_local(1), 0, 0);
      envoyer(nj, 0, 0);
      envoyer(splitting.get_offset_local(2), 0, 0);
      envoyer(nk, 0, 0);
    }

  TRUSTArray<_IN_TYPE_> send_tmp;
  TRUSTArray<_IN_TYPE_> recv_tmp;
  if (Process::je_suis_maitre())
    {
      // Master mpi process collects the data
      IntTab mapping;
      splitting.get_processor_mapping(mapping);
      for (int kproc = 0; kproc < mapping.dimension(2); kproc++)
        {
          for (int jproc = 0; jproc < mapping.dimension(1); jproc++)
            {
              for (int iproc = 0; iproc < mapping.dimension(0); iproc++)
                {
                  const int numproc = mapping(iproc, jproc, kproc);
                  int imin2, jmin2, kmin2, ni2, nj2, nk2;
                  if (numproc == Process::me())
                    {
                      imin2 = splitting.get_offset_local(0);
                      jmin2 = splitting.get_offset_local(1);
                      kmin2 = splitting.get_offset_local(2);
                      ni2 = ni;
                      nj2 = nj;
                      nk2 = nk;
                    }
                  else
                    {
                      recevoir(imin2,numproc,0);
                      recevoir(ni2,numproc,0);
                      recevoir(jmin2,numproc,0);
                      recevoir(nj2,numproc,0);
                      recevoir(kmin2,numproc,0);
                      recevoir(nk2,numproc,0);
                    }
                  send_tmp.resize_array(ni2 * nj2 * nk2);

                  for (int k = 0; k < nk2; k++)
                    for (int j = 0; j < nj2; j++)
                      for (int i = 0; i < ni2; i++)
                        send_tmp[(k*nj2+j)*ni2+i] = input[(((k+kmin2)*njtot+(j+jmin2))*nitot+i+imin2)*nbcompo + component];

                  if (numproc != Process::me())
                    envoyer(send_tmp, numproc, 0);
                  else
                    recv_tmp = send_tmp;
                }
            }
        }
    }

  // Some processors might have empty domain, will not receive any message so do not send !
  if (splitting.get_nb_elem_local(0) > 0)
    {
      if (!Process::je_suis_maitre())
        {
          recevoir(recv_tmp, 0, 0);
        }

      for (int k = 0; k < nk; k++)
        for (int j = 0; j < nj; j++)
          for (int i = 0; i < ni; i++)
            output(i,j,k) = (_TYPE_)recv_tmp[(k*nj+j)*ni+i];
    }
}


// Write 3 velocity components at faces in "lata" format:
//  write ni*nj*nk lines of data with
//    ni, nj, nk = number of nodes in the domain
//    line of data = 3 values. 1st value = component i of velocity on left face of element (i,j,k), 2nd value = component j, etc...
// Returns the number of written values (1 value = 3 scalars)
template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
long long IJK_Striped_Writer::write_data_parallel_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz)
{
  if (vx.get_localisation() != IJK_Splitting::FACES_I
      || vy.get_localisation() != IJK_Splitting::FACES_J
      || vz.get_localisation() != IJK_Splitting::FACES_K)
    {
      Cerr << "Error in  IJK_Striped_Writer::write_data_parallel_template(vx, vy, vz): provided fields have incorrect localisation" << finl;
      Process::exit();
    }
  const IJK_Splitting& splitting = vx.get_splitting();
  // In lata format, the velocity is written as an array of (vx, vy, vz) vectors.
  // Size of the array is the total number of nodes in the mesh.
  // The velocity associated with a node is the combination of velocities at the faces
  // at the right of the node (in each direction).
  const int nitot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 0) + 1;
  const int njtot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 1) + 1;
  const int nktot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2) + 1;

  write_data_parallel2_template<_OUT_TYPE_,_TYPE_,_TYPE_ARRAY_>(filename, nitot, njtot, nktot, vx, vy, vz);

  return (long long) nktot * njtot * nitot;
}

template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
long long IJK_Striped_Writer::write_data_parallel_template(const char * filename, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f)
{
  const IJK_Splitting& splitting = f.get_splitting();

  int nitot = 0, njtot = 0, nktot = 0;
  if (f.get_localisation() == IJK_Splitting::ELEM)
    {
      nitot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 0);
      njtot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 1);
      nktot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2);
    }
  else if (f.get_localisation() == IJK_Splitting::NODES)
    {
      nitot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 0)+1;
      njtot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 1)+1;
      nktot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2)+1;
    }
  else
    {
      Cerr << "Error: cannot write single component of a field that is at ELEMents or nodes" << finl;
      Process::exit();
    }
  write_data_parallel2_template<_OUT_TYPE_,_TYPE_,_TYPE_ARRAY_>(filename, nitot, njtot, nktot, f, f, f);

  return (long long) nktot * njtot * nitot;
}

// if vx, vy and vz are references to the same object, we assume that we have only 1 component.
// otherwise, assume we have 3 components
template<typename _OUT_TYPE_, typename _TYPE_, typename _TYPE_ARRAY_>
void IJK_Striped_Writer::write_data_parallel2_template(const char * filename,
                                                       const int file_ni_tot, const int file_nj_tot, const int file_nk_tot,
                                                       const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz)
{
  const IJK_Splitting& splitting = vx.get_splitting();
  const int nb_components = ((&vy == &vx) && (&vz == &vx)) ? 1 : 3;

  // Number of processes that write to the file:
  // Each process will write a portion of the total bloc of data, aligned on the bloc size.
  // According to recommandations of LRZ about GPFS, we must write blocks of data
  // which are a multiple of the gpfs block size (which is ???), aligned by the same amount,
  // in order to avoid thrashing the filesystem.

  long long block_size = Parallel_io_parameters::get_max_block_size();
  // Reasonable default value for the number of writing processes (one per node, but no more than 16).
  int nb_writing_processes = Parallel_io_parameters::get_nb_writing_processes();

  // Indices of writing processes : spread over the whole set of processes to minimize the chance that
  // we share the bandwidth with another writing process:
  ArrOfInt writing_processes(nb_writing_processes);
  int my_writing_process_index = -1;
  for (int i = 0; i < nb_writing_processes; i++)
    {
      writing_processes[i] = Process::nproc() * i / nb_writing_processes;
      if (Process::me() == writing_processes[i])
        my_writing_process_index = i;
    }
  //Cerr << "file_n_tot= " << file_ni_tot << " " << file_nj_tot << " " << file_nk_tot << finl;
  //Cerr << "ncompo= " << nb_components << finl;
  const long long total_data_bytes = (long long) file_nk_tot * file_nj_tot * file_ni_tot * nb_components * sizeof(_OUT_TYPE_);

  FILE *file_pointer = 0;
  // According to recommandations of LRZ about GPFS, we create the file on processor 0,
  // do a barrier, then open the file on the other processors. Moreover, we seek to the end of the file
  // and write something to give it the full size before other processors try to open the file.
  // Writing 8 bytes at arbitrary position and creating the file with any size is instantaneous.
  // Go back to unix interface because I need 64 bits offsets here
  if (my_writing_process_index == 0)
    {
      errno = 0;
      file_pointer = fopen(filename, "w"); // Write and truncate
      if (!file_pointer || errno)
        {
          Cerr << "Error opening file " << filename << ": fopen(filename,w) failed" << finl;
          Process::exit();
        }
      // Seek at the end of file and write something to set the file size and check if filessystem
      // accepts the final file size:
      // On 64 bit platform, the prototype for the fseek library function takes a 64 bit integer, so this is ok:
      fseek(file_pointer, total_data_bytes - sizeof(long long), SEEK_SET);
      if (errno != 0)
        {
          Cerr << "Error seeking at file offset " << (int)(total_data_bytes>>32) << "GB in file " << filename << finl;
          Process::exit();
        }
      // Write something to allocate disk space for the file:
      fwrite(&total_data_bytes, sizeof(long long), 1, file_pointer);
      if (errno != 0)
        {
          Cerr << "Error writing at file offset " << (int)(total_data_bytes>>32) << "GB in file " << filename << finl;
          Process::exit();
        }
    }
  // Wait for the processor 0 to initialize the file:
  Process::barrier();
  // Other writing processors can open the file for read/write now:
  if (my_writing_process_index > 0)
    {
      errno = 0;
      file_pointer = fopen(filename, "r+"); // Open for read/write
      if (!file_pointer || errno)
        {
          Cerr << "Error opening file " << filename << ": fopen(filename,r+) failed on writing process " << my_writing_process_index << finl;
          Process::exit();
        }
    }

  Schema_Comm schema_comm;
  schema_comm.set_all_to_allv_flag(1);
  // Sending to write-processes, array contains only other processes, not me
  ArrOfInt send_pe_list(writing_processes.size_array());

  send_pe_list.resize(0);
  for (int i = 0; i < writing_processes.size_array(); i++)
    if (i != my_writing_process_index) // Declare everybody but me.
      send_pe_list.append_array(writing_processes[i]);
  ArrOfInt recv_pe_list(Process::nproc()-1);

  recv_pe_list.resize(0);
  if (my_writing_process_index >= 0)
    {
      for (int i = 0; i < Process::nproc(); i++)
        {
          if (i != Process::me()) // Declare everybody but me.
            recv_pe_list.append_array(i);
        }
    }
  schema_comm.set_send_recv_pe_list(send_pe_list, recv_pe_list, 1 /* allow sending to myself */);

  int total_number_of_blocks = (int)((total_data_bytes + block_size - 1) / block_size);

  // Temporary array for interlaced data:
  TRUSTArray<_OUT_TYPE_> tmp(std::max(vx.ni(), std::max(vy.ni(), vz.ni())) * nb_components);
  TRUSTArray<_OUT_TYPE_> recv_tmp;
  if (my_writing_process_index >= 0)
    recv_tmp.resize_array((int)(block_size / sizeof(_OUT_TYPE_)));

  // Index of the block that the first writing processor wants to write:
  // writing process 0 writes block 0, writing process 1 writes block 1, etc
  for (int i_block_proc0 = 0; i_block_proc0 < total_number_of_blocks; i_block_proc0 += nb_writing_processes)
    {
      schema_comm.begin_comm();
      // Each processor send the data required to fill the next
      // nb_writing_processes blocks to each writing process:
      //  data for i_block_proc0 will be sent to writing_process[0]
      //  data for i_block_proc0+1 will be sent to writing_process[1],
      //  etc
      for (int iwrite_process = 0; iwrite_process < nb_writing_processes; iwrite_process++)
        {
          Sortie& send_buffer = schema_comm.send_buffer(writing_processes[iwrite_process]);

          long long offset_start_of_block = (long long) block_size * (i_block_proc0 + iwrite_process);
          long long offset_end_of_block = offset_start_of_block + block_size;
          if (offset_end_of_block > total_data_bytes)
            offset_end_of_block = total_data_bytes;
          if (offset_start_of_block >= offset_end_of_block)
            break; // Remaining writing processors have no data to write
          // Loop on j and k local coordinates, for each, find if there
          // is a segment imin..imax to send.
          // copy the segment with xyz components interlaced into tmp buffer and put
          // the requested excerpt into the buffer
          // For faces: we have perhaps not the same number of faces in each direction for each component, take max:
          const int kmax = std::max(vx.nk(), std::max(vy.nk(), vz.nk()));
          const int jmax = std::max(vx.nj(), std::max(vy.nj(), vz.nj()));
          const int imax = std::max(vx.ni(), std::max(vy.ni(), vz.ni()));
          for (int k = 0; k < kmax; k++)
            {
              for (int j = 0; j < jmax; j++)
                {
                  // This is the position of the current block in the file, in bytes
                  // This is where the segment (i=0..imax-1, j, k) resides in the file, in bytes
                  long long offset_segment_start = (long long) (k + splitting.get_offset_local(DIRECTION_K)) * file_nj_tot;
                  offset_segment_start = (offset_segment_start + j + splitting.get_offset_local(DIRECTION_J)) * file_ni_tot;
                  offset_segment_start += splitting.get_offset_local(DIRECTION_I);
                  offset_segment_start *= nb_components * sizeof(_OUT_TYPE_);
                  long long offset_segment_end = offset_segment_start + imax * nb_components * sizeof(_OUT_TYPE_);

                  // Compute the intersection of the two ranges (block and segment):
                  const long long offset_intersection_start = (offset_start_of_block < offset_segment_start) ? offset_segment_start : offset_start_of_block;
                  const long long offset_intersection_end = (offset_end_of_block < offset_segment_end) ? offset_end_of_block : offset_segment_end;

                  // Check if intersection is empty ?
                  if (offset_intersection_end > offset_intersection_start)
                    {
                      // Copy i=0..imax data into tmp buffer and interlace components.
                      // Warn: for fields with 3 components, as the block_size is a power of two, only one block every three blocks
                      // starts with components x. Other blocks start with a component y or z... Therefore we interlace xyz data
                      // and select data for the current block from this interlace data. Also convert to write type:
                      tmp = 0.; // for data that is beyond limits
                      for (int i_component = 0; i_component < nb_components; i_component++)
                        {
                          const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& src_field = (i_component==0)?vx:((i_component==1)?vy:vz);
                          // There are not the same number of faces in each direction, check that we are not beyond limits for this component:
                          if (k < src_field.nk() && j < src_field.nj())
                            {
                              const int max_this_compo = src_field.ni();
                              for (int i = 0; i < max_this_compo; i++)
                                {
                                  tmp[i * nb_components + i_component] = (_OUT_TYPE_)src_field(i, j, k);
                                }
                            }
                        }
                      // Everything still in bytes:
                      const int start_index_within_block = (int) (offset_intersection_start - offset_start_of_block);
                      const int start_index_within_tmp   = (int) (offset_intersection_start - offset_segment_start);
                      // Length is in bytes
                      const int data_length = (int) (offset_intersection_end - offset_intersection_start);
                      send_buffer << start_index_within_block << data_length;
                      const int sz =  (int)sizeof(_OUT_TYPE_);
                      send_buffer.put(tmp.addr() + start_index_within_tmp/sz,
                                      data_length/sz, 1);
                    }
                }
            }
        }
      // Transmit data to writing processes:
      schema_comm.echange_taille_et_messages();

      // Extract received data and write to disk:
      if (my_writing_process_index >= 0)
        {
          recv_tmp = 0.;
          // Cast to long long to be sure that the multiply will not overflow:
          const long long offset = (long long) block_size * (i_block_proc0 + my_writing_process_index);
          //Cerr << "Offset=" << (int)offset << finl;
          long long this_block_size = block_size;
          if (total_data_bytes - offset < this_block_size)
            this_block_size = total_data_bytes - offset;
          const int nproc=Process::nproc();
          for (int i_source_process = 0; i_source_process < nproc; i_source_process++)
            {
              Entree& recv_buffer = schema_comm.recv_buffer(i_source_process);
              while (1)
                {
                  int start_index_within_block;
                  recv_buffer >> start_index_within_block;
                  if (recv_buffer.eof())
                    break;
                  int data_length;
                  recv_buffer >> data_length;
                  //Cerr << "start_index_within_bloc= "<< start_index_within_block
                  //   << " length= " << data_length
                  //   << " this_block_size= " << (int)this_block_size << finl;
                  if (start_index_within_block < 0 || data_length < 1 || start_index_within_block + data_length > this_block_size)
                    {
                      Cerr << "Internal error in writing ijk lata file: start_index_within_block and data_length are invalid" << finl;
                      Process::exit();
                    }
                  if (start_index_within_block + data_length > recv_tmp.size_array() * (int)sizeof(_OUT_TYPE_))
                    {
                      Cerr << "Internal error in writing ijk lata file: index out of bound" << finl;
                      Process::exit();
                    }
                  const int sz =  (int)sizeof(_OUT_TYPE_);
                  recv_buffer.get(recv_tmp.addr() + start_index_within_block/sz,
                                  data_length/sz);
                }
            }
          // Each process writes the data chunk
          // On 64 bit platform, the prototype for the fseek library function takes a 64 bit integer.
          errno = 0;
          fseek(file_pointer, offset, SEEK_SET);
          if (errno != 0)
            {
              Cerr << "Error seeking at file offset " << (int)(offset>>32) << "GB in file " << filename << finl;
              Process::exit();
            }
          fwrite((const char*)recv_tmp.addr(), this_block_size /* size in bytes */, 1, file_pointer);
          if (errno != 0)
            {
              Cerr << "Error writing at file offset " << (int)(offset>>32) << "GB in file " << filename << finl;
              Process::exit();
            }
        }
      schema_comm.end_comm();
    }


  if (file_pointer) // (Some processors don't open the file)
    fclose(file_pointer);
}

#endif
