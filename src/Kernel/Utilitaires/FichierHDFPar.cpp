/****************************************************************************
* Copyright (c) 2020, CEA
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
#include <FichierHDFPar.h>

#ifdef MPI_
#include <comm_incl.h>
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

  //on cluster, type lfs getstripe . to see the default striping of the current repository
  hsize_t stripe_size = 1048576; //1572864;
  H5Pset_alignment(file_access_plst_, 0, stripe_size);
#endif

  MPI_Info_free(&infos);
#endif
}

void FichierHDFPar::prepare_dataset_props()
{
  FichierHDF::prepare_dataset_props();
#ifdef MED_
#ifdef MPI_
  if(collective_op_)
    H5Pset_dxpl_mpio(dataset_transfer_plst_, H5FD_MPIO_COLLECTIVE);
#endif
#endif

  // int rank = Process::me();

  // //Build expected dataset name for the current proc (with the trailing _000x stuff)
  // Nom dataset_full_name = dataset_name;
  // dataset_full_name.nom_me(rank);
}

