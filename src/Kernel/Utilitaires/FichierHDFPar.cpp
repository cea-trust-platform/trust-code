/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        FichierHDFPar.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <FichierHDFPar.h>
#include <communications.h>
#include <ArrOfInt.h>
#include <vector>

#ifdef MPI_
#include <mpi.h>
#include <Comm_Group_MPI.h>
#endif

FichierHDFPar::FichierHDFPar() :
  FichierHDF(), collective_op_(false)
{
#ifndef MPI_
  Cerr << "FichierHDFPar needs MPI to be used!" << finl;
  Process::exit(-1);
#endif
}

FichierHDFPar::~FichierHDFPar() {}

void FichierHDFPar::prepare_file_props()
{
  FichierHDF::prepare_file_props();
#ifdef MPI_

  MPI_Info infos;
  MPI_Info_create(&infos); // not used for now. CCRT supports advise to leave empty.

#ifdef MED_
  H5Pset_fapl_mpio( file_access_plst_, Comm_Group_MPI::get_trio_u_world(), infos);
#endif

  MPI_Info_free(&infos);
#endif
}

void FichierHDFPar::prepare_dataset_props()
{
  FichierHDF::prepare_dataset_props();
#ifdef MED_
  if(collective_op_)
    H5Pset_dxpl_mpio(dataset_transfer_plst_, H5FD_MPIO_COLLECTIVE);
#endif

  // int rank = Process::me();

  // //Build expected dataset name for the current proc (with the trailing _000x stuff)
  // Nom dataset_full_name = dataset_name;
  // dataset_full_name.nom_me(rank);
}

void FichierHDFPar::create_and_fill_dataset(Nom dataset_basename, Sortie_Brute& sortie)
{
  hsize_t lenData = sortie.get_size();
  const char * data = sortie.get_data();

  create_and_fill_dataset(dataset_basename, data, lenData, H5T_NATIVE_OPAQUE);
}


void FichierHDFPar::create_and_fill_dataset(Nom dataset_basename, SChaine& sortie)
{
  hsize_t lenData = sortie.get_size();
  const char * data = sortie.get_str();

  create_and_fill_dataset(dataset_basename, data, lenData, H5T_C_S1);
}


#ifdef MED_
void FichierHDFPar::create_and_fill_dataset(Nom dataset_basename, const char* data, hsize_t lenData, hid_t datatype)
{
  //collecting the lengths of the datasets from every other processors
  long long init_value = lenData;
  std::vector<long long> datasets_len(Process::nproc(), init_value);
  envoyer_all_to_all(datasets_len, datasets_len);

  // Creating the datasets from every processor
  // (metadata have to be written collectively)
  std::vector<hid_t> datasets_id(Process::nproc());
  for(int p = 0; p < Process::nproc(); p++)
    {
      Nom dname = dataset_basename;
      dname = dname.nom_me(p, 0, 1 /*without_padding*/);
      hsize_t dlen= datasets_len[p];
      hid_t dspace = H5Screate_simple(1, &dlen, NULL);

      datasets_id[p] = H5Dcreate2(file_id_, dname, datatype, dspace,
                                  H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );
      H5Sclose(dspace);
    }

  hid_t dataspace_id = H5Screate_simple(1, &lenData, NULL);

  // Writing my own dataset
  H5Dwrite(datasets_id[Process::me()], datatype, dataspace_id, H5S_ALL, dataset_transfer_plst_, data);

  // Close dataset and dataspace
  for(int p = 0; p < Process::nproc(); p++)
    H5Dclose(datasets_id[p]);
  H5Sclose(dataspace_id);
}
#endif
