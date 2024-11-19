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

#ifndef TRUST_2_PDI_included
#define TRUST_2_PDI_included

#include <pdi.h>
#include <paraconf.h>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <TRUSTTab.h>
#include <Comm_Group_MPI.h>
#include <PE_Groups.h>

/*! @brief classe TRUST_2_PDI Encapsulation of PDI methods (library used for IO operations)
 */
class TRUST_2_PDI
{
public:

  static int PDI_checkpoint_;
  static int PDI_restart_;
  static int PDI_initialized_;

  static void init(std::string IO_config)
  {
    if(PDI_initialized_)
      {
        Cerr << "TRUST_2_PDI::initialize PDI has already been initialized" << finl;
        Process::exit();
      }
    tconf_ = PC_parse_path(IO_config.c_str());
    PDI_init(PC_get(tconf_, ".pdi"));

    // share node parallelism
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

    PDI_initialized_ = 1;
  }

  static void finalize()
  {
    assert(PDI_initialized_);
    assert(shared_data_.size() == 0);
    PDI_initialized_ = 0;
    PDI_checkpoint_ = 0;
    PDI_restart_ = 0;

    // finalize PDI
    PDI_finalize();

    PC_tree_destroy(&tconf_);
  }

  void read(const std::string& name, void *data)
  {
    PDI_expose(name.c_str(), data, PDI_INOUT);
  }

  void write(const std::string& name, void *data)
  {
    PDI_expose(name.c_str(), data, PDI_OUT);
  }

  void multiple_reads(const std::string& event, const std::map<std::string,void*>& data)
  {
    multiple_IO_(event, data, 0 /* reading */);
  }

  void multiple_writes(const std::string& event, const std::map<std::string,void*>& data)
  {
    multiple_IO_(event, data, 1 /* writing */);
  }

  void TRUST_start_sharing(const std::string& name, void *data)
  {
    PDI_share(name.c_str(), data, PDI_OUT);
    shared_data_.push_back(name);
  }

  void PDI_start_sharing(const std::string& name, void *data)
  {
    PDI_share(name.c_str(), data, PDI_INOUT);
    shared_data_.push_back(name);
  }

  void trigger(const std::string& event)
  {
    PDI_event(event.c_str());
  }

  void stop_sharing_last_variable()
  {
    PDI_reclaim(shared_data_.back().c_str());
    shared_data_.pop_back();
  }

  void stop_sharing()
  {
    // stop sharing everything starting from the end
    for(auto it = shared_data_.rbegin(); it != shared_data_.rend(); it++)
      PDI_reclaim(it->c_str());
    shared_data_.clear();
  }

  // Higher level methods
  void share_TRUSTTab_dimensions(const DoubleTab& tab, Nom name, int write);
  void prepareRestart(int& last_iteration, double& tinit, int resume_last_time);

private:

  void multiple_IO_(const std::string& event, const std::map<std::string,void*>& data, int write=0);

  // data that are currently shared with PDI
  static std::vector<std::string> shared_data_;

  // configuration tree
  static PC_tree_t tconf_;
};

#endif
