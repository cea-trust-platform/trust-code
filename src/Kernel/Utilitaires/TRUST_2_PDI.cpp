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

#include <TRUST_2_PDI.h>

int TRUST_2_PDI::PDI_checkpoint_=0;
int TRUST_2_PDI::PDI_restart_=0;
int TRUST_2_PDI::PDI_initialized_=0;
std::vector<std::string> TRUST_2_PDI::shared_data_;

/*! @brief Generic method to share the dimensions of a TRUST DoubleTab with PDI
 *
 * @param (const DoubleTab& tab) the array we want to share with PDI
 * @param (Nom name) the name of the array
 * @param (int write) flag to specify if we want to write the dimensions or read into it
 */
void TRUST_2_PDI::share_TRUSTTab_dimensions(const DoubleTab& tab, const Nom& name, int write)
{
#ifdef HAS_PDI
  int nb_dim = tab.nb_dim();
  ArrOfInt dimensions(nb_dim);
  for(int i=0; i< nb_dim; i++)
    dimensions[i] = tab.dimension_tot(i) ? tab.dimension_tot(i) : 1; // can't share null data

  // the first dimension can vary from one process of the node to the other
  // so we take the larger one to fix it in the corresponding dataset of the DoubleTab
  int glob_dim_0;
  PE_Groups::get_node_group().mp_collective_op(&dimensions[0], &glob_dim_0, 1, Comm_Group::COLL_MAX);

  std::string name_str = name.getString();
  std::string dim_str = "dim_" + name_str;
  std::string glob_dim_str = "glob_dim_" + name_str;

  if(write)
    PDI_multi_expose("dimensions", dim_str.c_str(), dimensions.addr(), PDI_OUT, glob_dim_str.c_str(), &glob_dim_0, PDI_OUT, nullptr);
  else
    PDI_multi_expose("dimensions", dim_str.c_str(), dimensions.addr(), PDI_INOUT, glob_dim_str.c_str(), &glob_dim_0, PDI_INOUT, nullptr);
#endif
}

/*! @brief Generic method to share the type of a TRUST object
 *
 * @param (Nom name) the name of the object
 * @param (Nom type) type of the object
 */
void TRUST_2_PDI::share_type(const Nom& name, const Nom& type)
{
  std::string tname = "TYPE_" + name.getString();
  char* t = const_cast<char*>(type.getChar());

  std::string size = "size_TYPE_" + name.getString();
  int sz = type.longueur();

  // here we expose the data, so that PDI can keep a copy of it without having to share it later
  write(size, &sz);
  write(tname, t);
}

/*! @brief Generic method to read the type of a TRUST object in the HDF5 file
 *
 * @param (Nom name) the name of the object
 * @param (Nom type) type of the object
 */
void TRUST_2_PDI::get_type(const Nom& name, Nom& type)
{
#ifdef HAS_PDI
  int tmp = -1;
  PDI_share("TYPES", &tmp, PDI_INOUT);

  // getting size of string first
  std::string size = "size_TYPE_" + name.getString();
  int sz = -1;
  PDI_share(size.c_str(), &sz, PDI_INOUT);
  trigger("get_" + size);
  PDI_reclaim(size.c_str());
  assert(sz>=0);
  type.getString().resize(sz);

  // getting string
  std::string tname = "TYPE_" + name.getString();
  char* t = const_cast<char*>(type.getChar());
  PDI_share(tname.c_str(), t, PDI_INOUT);
  trigger("get_" + tname);
  PDI_reclaim(tname.c_str());

  PDI_reclaim("TYPES");
#endif
}

/*! @brief Generic method to prepare the restart of a computation
 *
 * @param (int& last_iteration) the index of the backup iteration we want to recover from
 * @param (double& tinit) the time from which we want to resume the calculation
 * @param (int resume_last_time) flag to specify if we want to resume from the last time or we want to recover from a specific time
 */
void TRUST_2_PDI::prepareRestart(int& last_iteration, double& tinit, int resume_last_time)
{
  // Check that we have the same number of procs used for checkpoint
  if(Process::node_master()) // if I'm the master of my node (no need for everyone to read)
    {
      int nb_proc = -1;
      read("nb_proc", &nb_proc);
      if(nb_proc != Process::nproc())
        {
          Cerr << "TRUST_2_PDI::prepareRestart():: PDI Restart Error ! The backup file has been generated with " << nb_proc << " processors, whereas the current computation is launched with "
               << Process::nproc() << " processors. With PDI, you need to restart your computation with the same number of processors used for previous computation" << finl;
          Process::exit();
        }
    }

  // Get time scheme information
  int nb_sauv = -1;
  read("iter", &nb_sauv);
  std::vector<double> temps(nb_sauv+1);
  read("temps_sauvegardes", temps.data());

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
  write("iter", &last_iteration);
  write("temps", &tinit);
}

