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

#include <Ecrire_YAML.h>
#include <Equation_base.h>
#include <Postraitement.h>

/*! @brief Generate the YAML file that will be read for checkpoint
 */
void Ecrire_YAML::write_checkpoint_file(std::string yaml_fname)
{
  if (Process::je_suis_maitre())
    {
      set_data();
      write_checkpoint_restart_file(1 /*save*/, yaml_fname);
    }
  Process::barrier();
}

/*! @brief Generate the YAML file that will be read for restart
 */
void Ecrire_YAML::write_restart_file(std::string yaml_fname)
{
  if (Process::je_suis_maitre())
    {
      set_data();
      write_checkpoint_restart_file(0 /*restart*/, yaml_fname);
    }
  Process::barrier();
}

/*! @brief Generate the YAML file that will be read with champ_fonc_reprise
 */
void Ecrire_YAML::write_champ_fonc_restart_file(std::string yaml_fname)
{
  if (Process::je_suis_maitre())
    write_checkpoint_restart_file(0 /*restart*/, yaml_fname); // The data have been set in champ_fonc_reprise as we only need to restore some of them (not all unknowns)
  Process::barrier();
}

/*! @brief Private method to generate the whole YAML file for checkpoint/restart
 * Here is how the checkpoint file is organized:
 * - every unknown is stored in its own dataset.
 * - for all datasets, the first dimension has to do with the rank of the processor,
 * which means in parallel, for each unknown, every processor writes collectively in the same dataset into their dedicated portion.
 * - for all datasets, the second dimension has to do with the number of backups performed.
 * For simple_sauvegarde, this dimension has a size of 1 as we only keep the last backup.
 * For classic sauvegarde, this dimension has a size of $nb_iter_max (can be managed in Schema_Temps_base). So the number of backups that we can store is limited !
 * If we exceed this number, we will overwrite the first backups !
 * - we track the timesteps that are being stored in the checkpoint file in a specific dataset (time/t)
 * and we also provide the index of the latest backup in time/last_iteration (this is important for restart so we can find the right slot to recover the data).
 *
 */
void Ecrire_YAML::write_checkpoint_restart_file(int save, std::string yaml_fname)
{
  std::string text = "pdi:";

  declare_metadata(save, text);
  declare_data(save, text);

  // declare plugins
  begin_bloc("plugins:", text);
#ifndef NDEBUG
  add_line("trace:", text);
#endif
  if(Process::is_parallel())
    add_line("mpi:", text);
  add_line("decl_hdf5:", text);

  for(unsigned i_pb=0; i_pb<pbs_.size(); i_pb++)
    {
      if(save)
        {
          // if the checkpoint file already exists, we erase it and create a brand new one
          write_file_initialization(i_pb, text);
          // block to write the unknown fields
          write_fields_for_checkpoint(i_pb, text);
          // block to write scalars
          write_scalars_for_checkpoint(i_pb, text);
          // block to write the saved timesteps
          write_time_scheme(1, pbs_[i_pb].filename, text);
        }
      else
        write_data_for_restart(i_pb, text);
      write_format(save, pbs_[i_pb].filename, text);
    }

  SFichier fic(yaml_fname.c_str());
  fic << text;
}

/*! @brief Writes the block in the YAML file that will specify the format of the checkpoint file named fname
 */
void Ecrire_YAML::write_format(int save, std::string fname, std::string& text)
{
  std::string format_sauvegarde = "- file: " + fname;
  begin_bloc(format_sauvegarde, text);
  if(Process::is_parallel())
    add_line("communicator: $master", text);
  std::string IO = save ? "write:" : "read:";
  begin_bloc(IO, text);
  write_generic_scalar("format_sauvegarde/version", "version", text);
  write_generic_scalar("format_sauvegarde/simple_sauvegarde", "simple_sauvegarde", text);
  if(!save) // for checkpoint, nb_proc is written during a special event to trigger the opening of backup file
    write_generic_scalar("format_sauvegarde/nb_proc", "nb_proc", text);
  end_bloc();
  end_bloc();
}

/*! @brief Writes the block in the YAML file regarding the time scheme for the checkpoint file fname
 */
void Ecrire_YAML::write_time_scheme(int save, std::string fname, std::string& text)
{
  std::string time_scheme = "- file: " + fname;
  begin_bloc(time_scheme, text);
  if(save)
    add_line("on_event: time_scheme", text);

  if(Process::is_parallel())
    {
      // during checkpoint, the master of the node knows all the information so no need for everyone to write
      // during restart, every processor of the node will need to retrieve the time-scheme information
      std::string communicator = save ? "$master" : "$node";
      add_line("communicator: " + communicator, text);
    }

  // For checkpoint : declaring the dataset that will contain all the timesteps that will be saved
  if(save)
    {
      begin_bloc("datasets:", text);
      std::string size = "\'$nb_iter_max\'";
      declare_array("time/t", "double", size, text);
      end_bloc();
    }

  std::string IO = save ? "write:" : "read:";
  begin_bloc(IO, text);
  if(save) // For checkpoint : writing in the dedicated dataset the timestep that is being saved
    write_TRUST_scalar("time/t", "temps", "", text);
  else // For restart : getting all the timesteps that have been saved
    write_generic_scalar("time/t", "temps_sauvegardes", text);
  // For checkpoint/restart : writing/getting the last iteration that has been saved
  write_generic_scalar("time/last_iteration", "iter", text);
  end_bloc();
  end_bloc();
}

/*! @brief  Writes the block in the YAML file that will trigger the creation of the checkpoint file for the problem pb_i
 */
void Ecrire_YAML::write_file_initialization(int pb_i, std::string& text)
{
  std::string para = "- file: " + pbs_[pb_i].filename;
  begin_bloc(para, text);
  std::string event = "on_event: init_" + pbs_[pb_i].pb->le_nom().getString();
  add_line(event, text);
  add_line("collision_policy: replace_and_warn # print a warning if file or any of dataset already exist", text);
  if(Process::is_parallel())
    add_line("communicator: $master", text);

  // we need to dump a single variable with PDI for the collision policy to work
  begin_bloc("write:", text);
  write_generic_scalar("format_sauvegarde/nb_proc", "nb_proc", text);
  end_bloc();
  end_bloc();
}

/*! @brief Writes the block in the YAML file that describes how the fields of the problem pb_i to be saved will be dumped in a dataset
 */
void Ecrire_YAML::write_fields_for_checkpoint(int pb_i, std::string& text)
{
  std::string checkpoint = "- file: " + pbs_[pb_i].filename;
  begin_bloc(checkpoint, text);
  std::string event = "on_event: fields_backup_" + pbs_[pb_i].pb->le_nom().getString();
  add_line(event, text);
  if(Process::is_parallel())
    add_line("communicator: $node", text);

  // declaring datasets
  begin_bloc("datasets:", text);
  for(const auto& f: pbs_[pb_i].fields)
    {
      std::string name = f.first;
      std::string type = f.second.first;
      int nb_dim = f.second.second;
      declare_dataset(name, type, nb_dim, text);
    }
  end_bloc();

  // writing fields
  begin_bloc("write:", text);
  for(const auto& f: pbs_[pb_i].fields)
    {
      std::string name = f.first;
      int nb_dim = f.second.second;
      std::string cond = pbs_[pb_i].conditions.count(name) ? pbs_[pb_i].conditions[name] : "" ;
      write_dtab(name, nb_dim, cond, text);
    }
  end_bloc();

  end_bloc();
}

/*! @brief Writes the block in the YAML file that describes how the scalars of the problem pb_i to be saved will be written
 */
void Ecrire_YAML::write_scalars_for_checkpoint(int pb_i, std::string& text)
{
  std::string checkpoint = "- file: " + pbs_[pb_i].filename;
  begin_bloc(checkpoint, text);
  std::string event = "on_event: scalars_backup_" + pbs_[pb_i].pb->le_nom().getString();
  add_line(event, text);
  if(Process::is_parallel())
    add_line("communicator: $master", text);

  // declaring datasets
  begin_bloc("datasets:", text);
  for (auto const& scal : pbs_[pb_i].scalars)
    {
      const std::string& name = scal.first;
      const std::string& type = scal.second;
      declare_array(name, type, "\'$nb_iter_max\'", text);
    }
  end_bloc();

  // writing scalars
  begin_bloc("write:", text);
  for(const auto& scal: pbs_[pb_i].scalars)
    {
      const std::string& name = scal.first;
      std::string cond = pbs_[pb_i].conditions.count(name) ? pbs_[pb_i].conditions[name] : "" ;
      write_TRUST_scalar(name, name, cond, text);
    }
  end_bloc();

  end_bloc();
}

/*! @brief Writes the block in the YAML file that describes how the data of the problem pb_i to be restored will be read from the checkpoint file
 */
void Ecrire_YAML::write_data_for_restart(int pb_i, std::string& text)
{
  std::string fname = pbs_[pb_i].filename;

  // writing fields
  for(const auto& f: pbs_[pb_i].fields)
    {
      std::string checkpoint = "- file: " + fname;
      begin_bloc(checkpoint, text);
      if(Process::is_parallel())
        add_line("communicator: $node", text);

      begin_bloc("read:", text);
      std::string name = f.first;
      int nb_dim = f.second.second;
      std::string cond = pbs_[pb_i].conditions.count(name) ? pbs_[pb_i].conditions[name] : "" ;
      write_dtab(name, nb_dim, cond, text);
      end_bloc();
      end_bloc();
    }

  // writing scalars
  for(const auto& scal: pbs_[pb_i].scalars)
    {
      std::string checkpoint = "- file: " + fname;
      begin_bloc(checkpoint, text);
      if(Process::is_parallel())
        add_line("communicator: $node", text);

      begin_bloc("read:", text);
      const std::string& name = scal.first;
      std::string cond = pbs_[pb_i].conditions.count(name) ? pbs_[pb_i].conditions[name] : "" ;
      write_TRUST_scalar(name, name, cond, text);
      end_bloc();
      end_bloc();
    }

  // writing time
  write_time_scheme(0, fname, text);
}

/*! @brief Declaring all metadata (ie data that will be kept in PDI memory) in the YAML file
 */
void Ecrire_YAML::declare_metadata(int save, std::string& text)
{
  begin_bloc("metadata:  # small values for which PDI keeps a copy", text);

  add_line("# scheme information", text);
  if(save)
    {
      add_line("iter: int         # Number of checkpoints performed until now (WARNING: does not correspond to the current iteration in my time loop)", text);
      add_line("nb_iter_max: int  # Maximum number of checkpoints (WARNING: if this number is too small, we overwrite the first checkpoints)", text);
      add_line("temps : double  # current physical time", text);
    }
  else
    {
      add_line("iter : int # last saved iteration", text);
      add_line("temps : double  # physical time from which we want to restart computation", text);
    }

  add_line("# information on format", text);
  add_line("version : int", text);
  add_line("simple_sauvegarde: int", text);

  // Declaring the dimensions of all the unknowns that will be saved/restored
  for(unsigned i_pb=0; i_pb<pbs_.size(); i_pb++)
    {
      for(const auto& f: pbs_[i_pb].fields)
        {
          std::string fname = f.first;
          int nb_dim = f.second.second;
          std::string asize = std::to_string(nb_dim);
          std::string aname = "dim_" + fname;
          declare_array(aname, "int", asize, text);

          std::string glob_dim = "glob_dim_" + fname + " : int";
          add_line(glob_dim, text);
        }
    }

  add_line("# metadata regarding parallelism", text);
  add_line("nb_proc : int", text);
  if(Process::is_parallel())
    {
      add_line("# MPI communicator for my group", text);
      add_line("node : MPI_Comm", text);
      add_line("# number of processors inside my group", text);
      add_line("nodeSize : int", text);
      add_line("# my rank inside my group", text);
      add_line("nodeRk : int", text);
      add_line("# rank of my group among all the other groups", text);
      add_line("nodeId : int", text);
      add_line("# MPI communicator for masters of each group", text);
      add_line("master : MPI_Comm", text);
    }
  end_bloc();
}

/*! @brief Declaring all data in the YAML file
 */
void Ecrire_YAML::declare_data(int save, std::string& text)
{
  begin_bloc("data:  # data we want to save/restore (essentially fields of unknown)", text);

  for(unsigned i_pb=0; i_pb<pbs_.size(); i_pb++)
    {
      for(const auto& f: pbs_[i_pb].fields)
        {
          std::string name = f.first;
          std::string type = f.second.first;
          int nb_dim = f.second.second;
          declare_dtab(name, type, nb_dim, text);
        }
      // additional scalar data:
      for(const auto& s: pbs_[i_pb].scalars)
        {
          std::string scal = s.first + " : " + s.second;
          add_line(scal, text);
        }
    }

  // For restart: declaring the array that will contain the timesteps that have been saved
  if(!save)
    add_line("temps_sauvegardes : { type: array, subtype: double, size: [ \'$iter + 1' ]  }", text);

  end_bloc();
}

/*! @brief Parse every added problems to collect the data that we need to save/restore
 */
void Ecrire_YAML::set_data()
{
  assert(!pbs_.empty());

  for(unsigned i_pb=0; i_pb<pbs_.size(); i_pb++)
    {
      OBS_PTR(Probleme_base) pb = pbs_[i_pb].pb;
      // equations unknowns
      for(int i_eq=0; i_eq<pb->nombre_d_equations(); i_eq++)
        {
          pb->equation(i_eq).champ_a_sauvegarder(pbs_[i_pb].fields);
          pb->equation(i_eq).scal_a_sauvegarder(pbs_[i_pb].scalars);
        }

      // statistical post-processing fields
      for (const auto& post_base : pb->postraitements())
        {
          post_base->champ_a_sauvegarder(pbs_[i_pb].fields, pbs_[i_pb].conditions);
          post_base->scal_a_sauvegarder(pbs_[i_pb].scalars, pbs_[i_pb].conditions);
        }
    }
}

/*! @brief Adds a field (name+dimension) to the block of the problem pb_name in the YAML file
 */
void Ecrire_YAML::add_field(Nom pb_name, Nom nom, int nb_dim)
{
  assert(!pbs_.empty());

  for(unsigned i_pb=0; i_pb<pbs_.size(); i_pb++)
    {
      // this method is meant to be called at the beginning of the computation only + pbs_ is usually really small
      // so the search shouldn't be costly
      if(pbs_[i_pb].pb->le_nom() == pb_name)
        {
          Field2Type& pb_fields = pbs_[i_pb].fields;
          std::string fname = nom.getString();
          pb_fields[fname] = std::make_pair("double",nb_dim);
          return;
        }
    }
  Cerr << "Ecrire_Yaml::add_field Trying to add field " << nom << " to non-existant problem " << pb_name << finl;
  Cerr << "Add " << pb_name << " to the Yaml list of problems first " << finl;
  Process::exit();
}

/*! @brief Adds a scalar (name+type) to the block of the problem pb_name in the YAML file
 */
void Ecrire_YAML::add_scalar(Nom pb_name, Nom nom, Nom type)
{
  assert(!pbs_.empty());
  for(unsigned i_pb=0; i_pb<pbs_.size(); i_pb++)
    {
      // this method is meant to be called at the beginning of the computation only + pbs_ is usually really small
      // so the search shouldn't be costly
      if(pbs_[i_pb].pb->le_nom() == pb_name)
        {
          Scalar2Type& pb_scalars = pbs_[i_pb].scalars;
          std::string sname = nom.getString();
          pb_scalars[sname] = type.getString();
          return;
        }
    }
  Cerr << "Ecrire_Yaml::add_scalar Trying to add scalar " << nom << " to non-existant problem " << pb_name << finl;
  Cerr << "Add " << pb_name << " to the Yaml list of problems first " << finl;
  Process::exit();
}

/*! @brief Declaring a one-dimensional array in the YAML file
 *  @param (std::string name)  name of the array
 *  @param (std::string type)  type of the array
 *  @param (std::string size)  size of the array
 *  @param (std::string& text) the string that will be completed by the method
 */
void Ecrire_YAML::declare_array(std::string name, std::string type, std::string size, std::string& text)
{
  std::string decl = name + " : { type: array, subtype: " + type + ", size: " + size + " }";
  add_line(decl, text);
}

/*! @brief Declaring a multi-dimensional array (ie TRUSTTab) in the YAML file
 *  @param (std::string name)  name of the array
 *  @param (std::string type)  type of the array
 *  @param (int nb_dim) number of the dimensions of the array
 *  @param (std::string& text) the string that will be completed by the method
 */
void Ecrire_YAML::declare_dtab(std::string name, std::string type, int nb_dim, std::string& text)
{
  std::string data = name + " : { type: array, subtype: " + type + ", size: [ \'$dim_" + name + "[0]\'";
  for(int d=1; d<nb_dim; d++)
    data = data + "," + "\'$dim_" + name + "[" + std::to_string(d) + "]\'";
  data = data + " ] }";
  add_line(data, text);
}

/*! @brief Declaring a dataset containing an unknown field in the YAML file:
 *  1st dimension is that of the process rank in the node for parallel computation,
 *  2nd dimension is that of the various iterations to be saved,
 *  3rd dimension of the dataset actually corresponds to the first dimension of the field (we take the largest first dimension of the node so that it can fit the data of every process)
 *  remaining dimensions of the dataset are those of the field
 *  @param (std::string fname) name of the field
 *  @param (std::string type)  type of the field
 *  @param (int nb_dim) number of the dimensions of the field
 *  @param (std::string& text) the string that will be completed by the method
 */
void Ecrire_YAML::declare_dataset(std::string fname, std::string type, int nb_dim, std::string& text)
{
  std::string dataset_dim = fname + ": { type: array, subtype: " + type + ", size: [";
  if(Process::is_parallel())
    dataset_dim = dataset_dim + "\'$nodeSize\', ";
  dataset_dim = dataset_dim + "\'$nb_iter_max\', ";
  dataset_dim = dataset_dim + "\'$glob_dim_" + fname + "\'";
  for(int d=1; d<nb_dim; d++)
    dataset_dim = dataset_dim + ", \'$dim_" + fname + "[" + std::to_string(d) + "]\'";
  dataset_dim = dataset_dim + " ] }";
  add_line(dataset_dim, text);
}

/*! @brief Writes the block in the YAML file to dump a generic scalar
 *  (useful to write invariant information about the nature of the checkpoint file)
 *  @param (std::string dname) name of the dataset where the scalar is stored
 *  @param (std::string name)  name of the data
 *  @param (std::string& text) the string that will be completed by the method
 */
void Ecrire_YAML::write_generic_scalar(std::string dname, std::string name, std::string& text)
{
  std::string header = name + ":";
  begin_bloc(header, text);
  std::string dataset_name = "dataset: " + dname;
  add_line(dataset_name, text);
  end_bloc();
}

/*! @brief Writes the block in the YAML file to dump a scalar coming from the simulation
 *  (useful to write data that vary and that we want to save at each iteration)
 *  @param (std::string dname) name of the dataset where the scalar is stored
 *  @param (std::string name)  name of the data
 *  @param (std::string cond)  conditions to respect in order to trigger the writing/reading of the scalar
 *  @param (std::string& text) the string that will be completed by the method
 */
void Ecrire_YAML::write_TRUST_scalar(std::string dname, std::string name, std::string cond, std::string& text)
{
  std::string header = name + ":";
  begin_bloc(header, text);
  if(!cond.empty())
    {
      std::string condition = "when: \'" + cond + "\'";
      add_line(condition, text);
    }
  std::string dataset_name = "dataset: " + dname;
  add_line(dataset_name, text);
  write_scalar_selection(text);
  end_bloc();
}

/*! @brief Writes the block in the YAML file to select the section in the dataset where the scalar will be written
 */
void Ecrire_YAML::write_scalar_selection(std::string& text)
{
  begin_bloc("dataset_selection:", text);
  add_line("size: [1]", text);
  std::string start = "start: [\'$iter\']";
  add_line(start, text);
  end_bloc();
}

/*! @brief Writes the block in the YAML file to dump a TRUST DoubleTab in a dataset
 *  @param (std::string name)  name of the data
 *  @param (int nb_dim) number of the dimensions of the DoubleTab
 *  @param (std::string cond)  conditions to respect in order to trigger the writing/reading of the DoubleTab
 *  @param (std::string& text) the string that will be completed by the method
 */
void Ecrire_YAML::write_dtab(std::string name, int nb_dim, std::string cond, std::string& text)
{
  std::string header = name + ":";
  begin_bloc(header, text);
  if(!cond.empty())
    {
      std::string condition = "when: \'" + cond + "\'";
      add_line(condition, text);
    }
  std::string dataset_name = "dataset: " + name;
  add_line(dataset_name, text);
  write_dtab_selection(name, nb_dim, text);
  end_bloc();
}

/*! @brief Writes the block in the YAML file to select the section in the dataset where the DoubleTab will be written
 *  @param (std::string name)  name of the data
 *  @param (int nb_dim) number of the dimensions of the DoubleTab
 *  @param (std::string& text) the string that will be completed by the method
 */
void Ecrire_YAML::write_dtab_selection(std::string name, int nb_dim, std::string& text)
{
  begin_bloc("dataset_selection:", text);
  std::string size =  "size: [";
  if(Process::is_parallel()) size = size + "1, ";
  size = size + "1, ";
  size = size + "\'$dim_" + name + "[0]\'";
  for(int d=1; d<nb_dim; d++)
    size = size + "," + "\'$dim_" + name + "[" + std::to_string(d) + "]\'";
  size = size + "]";
  add_line(size, text);

  std::string start = "start: [";
  if(Process::is_parallel()) start = start + "\'$nodeRk\',";
  start = start + "\'$iter\',";
  start = start + "0";
  for(int d=1; d<nb_dim; d++)
    start = start + "," + "0";
  start = start + "]";
  add_line(start, text);
  end_bloc();
}

