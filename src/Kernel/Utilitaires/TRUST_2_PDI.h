/****************************************************************************
* Copyright (c) 2025, CEA
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

  // This method is used to initialize PDI
  static void init(std::string IO_config)
  {
    if(PDI_initialized_)
      {
        Cerr << "TRUST_2_PDI::initialize PDI has already been initialized" << finl;
        Process::exit();
      }

    // PDI initialization, done thanks to the specification tree contained in the yaml file IO_config
    PC_tree_t tconf = PC_parse_path(IO_config.c_str());
    PDI_init(PC_get(tconf, ".pdi"));
    PC_tree_destroy(&tconf);

    // sharing node parallelism
#ifdef MPI_
    // Retrieve communicator on node
    const Comm_Group& ngrp = PE_Groups::get_node_group();
    const Comm_Group_MPI* nodeComm = dynamic_cast<const Comm_Group_MPI*>(&ngrp);
    if(nodeComm)
      {
        MPI_Comm comm = nodeComm->get_mpi_comm(); // mpi communicator of my node
        int nodeSz = nodeComm->nproc();  // number of procs in my node
        int nodeRk = nodeComm->rank();   // my rank inside the node
        int nodeId = nodeComm->get_node_id(); // id of my node (among all the other nodes)

        const Comm_Group& nm = PE_Groups::get_node_master(); // Communicator for the master of my node (this communicator is empty if I'm not the master of my node)
        const Comm_Group_MPI* nodeMaster = dynamic_cast<const Comm_Group_MPI*>(&nm);
        assert(nodeMaster);
        MPI_Comm masterComm = nodeMaster->get_mpi_comm();  // mpi communicator reserved for the master of my node only

        PDI_multi_expose("Parallelism",
                         "node",&comm, PDI_OUT,
                         "nodeSize",&nodeSz, PDI_OUT,
                         "nodeRk",    &nodeRk, PDI_OUT,
                         "nodeId",    &nodeId, PDI_OUT,
                         "master", &masterComm, PDI_OUT,
                         nullptr);
      }
#endif

    PDI_initialized_ = 1;
  }

  // Method to call to finalize PDI
  static void finalize()
  {
    assert(PDI_initialized_);
    assert(shared_data_.size() == 0);
    PDI_initialized_ = 0;
    PDI_checkpoint_ = 0;
    PDI_restart_ = 0;

    // finalize PDI
    PDI_finalize();
  }

  static void set_PDI_checkpoint(int c) { PDI_checkpoint_ = c; }
  static void set_PDI_restart(int r) { PDI_restart_ = r; }

  static int is_PDI_initialized() { return PDI_initialized_; }
  static int is_PDI_checkpoint() { return PDI_checkpoint_; }
  static int is_PDI_restart() { return PDI_restart_; }

  // Method to use to read the data named name and retrieve it in data
  void read(const std::string& name, void *data)
  {
    PDI_expose(name.c_str(), data, PDI_INOUT);
  }

  // Method to use to write the data named name and located in data
  void write(const std::string& name, void *data)
  {
    PDI_expose(name.c_str(), data, PDI_OUT);
  }

  // Method to use to read all the data contained in the map data at once, with the following structure:
  // data[name] = pointer to the data
  // The read will be triggered when the event "event" is called
  void multiple_reads(const std::string& event, const std::map<std::string,void*>& data)
  {
    multiple_IO_(event, data, 0 /* reading */);
  }

  // Method to use to write all the data contained in the map data at once, with the following structure:
  // data[name] = pointer to the data
  // The write will be triggered when the event "event" is called
  void multiple_writes(const std::string& event, const std::map<std::string,void*>& data)
  {
    multiple_IO_(event, data, 1 /* writing */);
  }

  // Method to use to start sharing with PDI the data named name and located in data
  // (while the data is shared, TRUST can not access it)
  // This will not write or read anything unless some event is triggered!
  void TRUST_start_sharing(const std::string& name, void *data)
  {
    PDI_share(name.c_str(), data, PDI_OUT);
    shared_data_.push_back(name);
  }

  // Method to use to trigger an event (to supervise the timing of the writes/reads)
  void trigger(const std::string& event)
  {
    PDI_event(event.c_str());
  }

  // Method to use to stop sharing the last variable that we shared with PDI
  // (which makes it available once again)
  void stop_sharing_last_variable()
  {
    PDI_reclaim(shared_data_.back().c_str());
    shared_data_.pop_back();
  }

  // Method to use to stop sharing all variables that we shared with PDI
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

  static int PDI_checkpoint_;
  static int PDI_restart_;
  static int PDI_initialized_;


};

#endif
