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

void TRUST_2_PDI::share_TRUSTTab_dimensions(const DoubleTab& tab, Nom name, int write)
{
  int nb_dim = tab.nb_dim();
  ArrOfInt dimensions(nb_dim);
  for(int i=0; i< nb_dim; i++)
    dimensions[i] = tab.dimension_tot(i) ? tab.dimension_tot(i) : 1; // can't share null data

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

void TRUST_2_PDI::prepareRestart(int& last_iteration, double& tinit, int resume_last_time)
{
  // Check that we have the same number of procs used for checkpoint
  if(Process::node_master())
    {
      int nb_proc = -1;
      PDI_expose("nb_proc", &nb_proc, PDI_INOUT);
      if(nb_proc != Process::nproc())
        {
          Cerr << "TRUST_2_PDI::prepareRestart():: PDI Restart Error ! The backup file has been generated with " << nb_proc << " processors, whereas the current computation is launched with "
               << Process::nproc() << " processors. With PDI, you need to restart your computation with the same number of processors used for previous computation" << finl;
          Process::exit();
        }
    }

  // Get time scheme information
  int nb_sauv = -1;
  PDI_expose("iter", &nb_sauv, PDI_INOUT);
  std::vector<double> temps(nb_sauv+1);
  PDI_expose("temps_sauvegardes", temps.data(), PDI_INOUT);

  // Restart from the last time
  if (resume_last_time)
    {
      // Look for the last time saved in checkpoint file to init current computation
      tinit = temps.back();
      last_iteration = nb_sauv;
    }
  else // resume from the requested time
    {
      // looking for tinit in backup file
      auto it = std::find_if(temps.begin(), temps.end(), [&](const double &t) { return std::fabs(t-tinit) < 1.e-8 ; } );
      if(it == temps.end())
        {
          Cerr << "------------------------------------------------------------------------------------" << finl;
          Cerr << "Time " << tinit << " not found in backup file. Please adjust tinit in your datafile "     << finl;
          Cerr << "Available times are:" << finl;
          for(auto t: temps)
            Cerr << t << " ";
          Cerr << finl << "------------------------------------------------------------------------------------" << finl;
          Process::exit();
        }
      last_iteration = (int)std::distance(temps.begin(),it);
    }

  // letting PDI know which iteration/time to read during restart
  PDI_expose("iter", &last_iteration, PDI_OUT);
  PDI_expose("temps", &tinit, PDI_OUT);
}



