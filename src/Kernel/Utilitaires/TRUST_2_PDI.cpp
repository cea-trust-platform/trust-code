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

#include <TRUST_2_PDI.h>
#include <Comm_Group_MPI.h>
#include <PE_Groups.h>

int TRUST_2_PDI::PDI_checkpoint_=0;
int TRUST_2_PDI::PDI_restart_=0;
int TRUST_2_PDI::PDI_initialized_=0;
std::vector<std::string> TRUST_2_PDI::shared_data_;

void TRUST_2_PDI::multiple_IO_(const std::string& event, const std::map<std::string,void*>& data, int write)
{
  for(auto& d: data)
    {
      if(write)
        PDI_share(d.first.c_str(), d.second, PDI_OUT);
      else
        PDI_share(d.first.c_str(), d.second, PDI_INOUT);
    }

  trigger(event);

  // stop sharing data, starting from the last shared object
  for (auto it = data.rbegin(); it != data.rend(); ++it)
    PDI_reclaim(it->first.c_str());
}

void TRUST_2_PDI::share_node_parallelism()
{
#ifdef MPI_
  const Comm_Group& ngrp = PE_Groups::get_node_group();
  const Comm_Group_MPI* nodeComm = dynamic_cast<const Comm_Group_MPI*>(&ngrp);
  if(nodeComm)
    {
      MPI_Comm comm = nodeComm->get_mpi_comm();
      int nodeSz = nodeComm->nproc();
      int nodeRk = nodeComm->rank();

      PDI_multi_expose("Parallelism",
                       "nodeComm",&comm, PDI_OUT,
                       "nodeSize",&nodeSz, PDI_OUT,
                       "nodeRk",    &nodeRk, PDI_OUT,
                       nullptr);
    }
#endif
}


void TRUST_2_PDI::share_TRUSTTab_dimensions(const DoubleTab& tab, Nom name, int write)
{
  int nb_dim = tab.nb_dim();
  ArrOfInt dimensions(nb_dim);
  for(int i=0; i< nb_dim; i++)
    dimensions[i] = tab.dimension(i);

  int glob_dim_0;
  PE_Groups::get_node_group().mp_collective_op(&dimensions[0], &glob_dim_0, 1, Comm_Group::COLL_MAX);

  std::string name_str = name.getString();
  std::string dim_str = "dim_" + name_str;
  std::string glob_dim_str = "glob_dim_" + name_str;

  std::map<std::string,void*> data_dims;
  data_dims[dim_str] = dimensions.addr();
  data_dims[glob_dim_str] = &glob_dim_0;
  multiple_IO_("dimensions", data_dims, write);
}

