/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        FichierHDFCollectif.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <FichierHDFCollectif.h>

#ifdef MPI_
#include <mpi.h>
#include <Comm_Group_MPI.h>
#endif

FichierHDFCollectif::FichierHDFCollectif() :
  FichierHDF()
{
#ifndef MPI_
  Cerr << "FichierHDFCollectif needs MPI to be used!" << finl;
  Process::exit(-1);
#endif
}

FichierHDFCollectif::~FichierHDFCollectif()
{
}

void FichierHDFCollectif::prepare_file_props()
{
#ifdef MPI_
  MPI_Info infos;
  MPI_Info_create(&infos); // not used for now. CCRT supports advise to leave empty.

  file_prop_lst_ = H5Pcreate(H5P_FILE_ACCESS);
  H5Pset_fapl_mpio( file_prop_lst_, Comm_Group_MPI::get_trio_u_world(), infos);

  MPI_Info_free(&infos);
#endif
}

void FichierHDFCollectif::prepare_read_dataset_props(Nom dataset_name)
{
  int rank = Process::me();

  // Build expected dataset name for the current proc (with the trailing _000x stuff)
  Nom dataset_full_name = dataset_name;
  dataset_full_name.nom_me(rank);

  //status = H5Pset_dxpl_mpio( propList, H5FD_MPIO_COLLECTIVE);  // ABN: to be seen
}


