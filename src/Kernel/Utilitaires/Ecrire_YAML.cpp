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

void Ecrire_YAML::write_checkpoint_file(std::string yaml_fname)
{
  if (Process::je_suis_maitre())
    {
      set_data_();
      write_checkpoint_restart_file_(1 /*save*/, yaml_fname);
    }
  Process::barrier();
}

void Ecrire_YAML::write_restart_file(std::string yaml_fname)
{
  if (Process::je_suis_maitre())
    {
      set_data_();
      write_checkpoint_restart_file_(0 /*restart*/, yaml_fname);
    }
  Process::barrier();
}

void Ecrire_YAML::write_champ_fonc_restart_file(std::string yaml_fname)
{
  if (Process::je_suis_maitre())
    write_checkpoint_restart_file_(0 /*restart*/, yaml_fname);
  Process::barrier();
}

void Ecrire_YAML::write_checkpoint_restart_file_(int save, std::string yaml_fname)
{
  std::string text = "pdi:";

  declare_metadata_(save, text);
  declare_data_(save, text);

  // declare plugins
  begin_bloc_("plugins:", text);
#ifndef NDEBUG
  add_line_("trace:", text);
#endif
  if(Process::is_parallel())
    add_line_("mpi:", text);
  add_line_("decl_hdf5:", text);

  for(unsigned i_pb=0; i_pb<pbs_.size(); i_pb++)
    {
      if(save)
        {
          write_file_initialization_(i_pb, text);
          write_fields_for_checkpoint_(i_pb, text);
          write_scalars_for_checkpoint_(i_pb, text);
          write_time_scheme_(1, pbs_[i_pb].filename, text);
        }
      else
        write_data_for_restart_(i_pb, text);
      write_format_(save, pbs_[i_pb].filename, text);
    }

  SFichier fic(yaml_fname.c_str());
  fic << text;
}

void Ecrire_YAML::write_format_(int save, std::string fname, std::string& text)
{
  std::string format_sauvegarde = "- file: " + fname;
  begin_bloc_(format_sauvegarde, text);
  if(Process::is_parallel())
    add_line_("communicator: $master", text);
  std::string IO = save ? "write:" : "read:";
  begin_bloc_(IO, text);
  write_attribute_("format_sauvegarde/version", "version", text);
  write_attribute_("format_sauvegarde/simple_sauvegarde", "simple_sauvegarde", text);
  if(!save) // for checkpoint, nb_proc is written during a special event to trigger the opening of backup file
    write_attribute_("format_sauvegarde/nb_proc", "nb_proc", text);
  end_bloc_();
  end_bloc_();
}

void Ecrire_YAML::write_time_scheme_(int save, std::string fname, std::string& text)
{
  std::string time_scheme = "- file: " + fname;
  begin_bloc_(time_scheme, text);
  if(save)
    add_line_("on_event: time_scheme", text);

  if(Process::is_parallel())
    {
      std::string communicator = save ? "$master" : "$node";
      add_line_("communicator: " + communicator, text);
    }

  if(save)
    {
      begin_bloc_("datasets:", text);
      std::string size = "\'$nb_iter_max\'";
      declare_array_("time/t", "double", size, text);
      end_bloc_();
    }

  std::string IO = save ? "write:" : "read:";
  begin_bloc_(IO, text);
  if(save)
    write_scalar_("time/t", "temps", "", text);
  else
    write_attribute_("time/t", "temps_sauvegardes", text);
  write_attribute_("time/last_iteration", "iter", text);
  end_bloc_();
  end_bloc_();
}

void Ecrire_YAML::write_file_initialization_(int pb_i, std::string& text)
{
  std::string para = "- file: " + pbs_[pb_i].filename;
  begin_bloc_(para, text);
  std::string event = "on_event: init_" + pbs_[pb_i].pb->le_nom().getString();
  add_line_(event, text);
  add_line_("collision_policy: replace_and_warn # print a warning if file or any of dataset already exist", text);
  if(Process::is_parallel())
    add_line_("communicator: $master", text);

  // we need to dump a single variable with PDI for the collision policy to work
  begin_bloc_("write:", text);
  write_attribute_("format_sauvegarde/nb_proc", "nb_proc", text);
  end_bloc_();
  end_bloc_();
}

void Ecrire_YAML::write_fields_for_checkpoint_(int pb_i, std::string& text)
{
  std::string checkpoint = "- file: " + pbs_[pb_i].filename;
  begin_bloc_(checkpoint, text);
  std::string event = "on_event: fields_backup_" + pbs_[pb_i].pb->le_nom().getString();
  add_line_(event, text);
  if(Process::is_parallel())
    add_line_("communicator: $node", text);

  // declaring datasets
  begin_bloc_("datasets:", text);
  for(const auto& f: pbs_[pb_i].fields)
    {
      std::string name = f.first;
      std::string type = f.second.first;
      int nb_dim = f.second.second;
      declare_dataset_(name, type, nb_dim, text);
    }
  end_bloc_();

  begin_bloc_("write:", text);
  // writing fields
  for(const auto& f: pbs_[pb_i].fields)
    {
      std::string name = f.first;
      int nb_dim = f.second.second;
      std::string cond = pbs_[pb_i].conditions.count(name) ? pbs_[pb_i].conditions[name] : "" ;
      write_dtab_(name, nb_dim, cond, text);
    }
  end_bloc_();

  end_bloc_();
}


void Ecrire_YAML::write_scalars_for_checkpoint_(int pb_i, std::string& text)
{
  std::string checkpoint = "- file: " + pbs_[pb_i].filename;
  begin_bloc_(checkpoint, text);
  std::string event = "on_event: scalars_backup_" + pbs_[pb_i].pb->le_nom().getString();
  add_line_(event, text);
  if(Process::is_parallel())
    add_line_("communicator: $master", text);

  // declaring datasets
  begin_bloc_("datasets:", text);
  for (auto const& scal : pbs_[pb_i].scalars)
    {
      const std::string& name = scal.first;
      const std::string& type = scal.second;
      declare_array_(name, type, "\'$nb_iter_max\'", text);
    }
  end_bloc_();

  begin_bloc_("write:", text);
  // writing scalars
  for(const auto& scal: pbs_[pb_i].scalars)
    {
      const std::string& name = scal.first;
      std::string cond = pbs_[pb_i].conditions.count(name) ? pbs_[pb_i].conditions[name] : "" ;
      write_scalar_(name, name, cond, text);
    }
  end_bloc_();

  end_bloc_();
}

void Ecrire_YAML::write_data_for_restart_(int pb_i, std::string& text)
{
  std::string fname = pbs_[pb_i].filename;

  // writing fields
  for(const auto& f: pbs_[pb_i].fields)
    {
      std::string checkpoint = "- file: " + fname;
      begin_bloc_(checkpoint, text);
      if(Process::is_parallel())
        add_line_("communicator: $node", text);

      begin_bloc_("read:", text);
      std::string name = f.first;
      int nb_dim = f.second.second;
      std::string cond = pbs_[pb_i].conditions.count(name) ? pbs_[pb_i].conditions[name] : "" ;
      write_dtab_(name, nb_dim, cond, text);
      end_bloc_();
      end_bloc_();
    }
  // writing scalars
  for(const auto& scal: pbs_[pb_i].scalars)
    {
      std::string checkpoint = "- file: " + fname;
      begin_bloc_(checkpoint, text);
      if(Process::is_parallel())
        add_line_("communicator: $node", text);

      begin_bloc_("read:", text);
      const std::string& name = scal.first;
      std::string cond = pbs_[pb_i].conditions.count(name) ? pbs_[pb_i].conditions[name] : "" ;
      write_scalar_(name, name, cond, text);
      end_bloc_();
      end_bloc_();
    }

  // writing time
  write_time_scheme_(0, fname, text);
}

void Ecrire_YAML::declare_metadata_(int save, std::string& text)
{
  begin_bloc_("metadata:  # small values for which PDI keeps a copy", text);

  add_line_("# scheme information", text);
  if(save)
    {
      add_line_("iter: int         # Number of checkpoints performed until now (WARNING: does not correspond to the current iteration in my time loop)", text);
      add_line_("nb_iter_max: int  # Maximum number of checkpoints (WARNING: if this number is too small, we overwrite the first checkpoints)", text);
      add_line_("temps : double  # current physical time", text);
    }
  else
    {
      add_line_("iter : int # last saved iteration", text);
      add_line_("temps : double  # physical time from which we want to restart computation", text);
    }

  add_line_("# information on format", text);
  add_line_("version : int", text);
  add_line_("simple_sauvegarde: int", text);

  for(unsigned i_pb=0; i_pb<pbs_.size(); i_pb++)
    {
      for(const auto& f: pbs_[i_pb].fields)
        {
          std::string fname = f.first;
          int nb_dim = f.second.second;
          std::string asize = std::to_string(nb_dim);
          std::string aname = "dim_" + fname;
          declare_array_(aname, "int", asize, text);

          std::string glob_dim = "glob_dim_" + fname + " : int";
          add_line_(glob_dim, text);
        }
    }

  add_line_("# metadata regarding parallelism", text);
  add_line_("nb_proc : int", text);
  if(Process::is_parallel())
    {
      add_line_("# MPI communicator for my group", text);
      add_line_("node : MPI_Comm", text);
      add_line_("# number of processors inside my group", text);
      add_line_("nodeSize : int", text);
      add_line_("# my rank inside my group", text);
      add_line_("nodeRk : int", text);
      add_line_("# rank of my group among all the other groups", text);
      add_line_("nodeId : int", text);
      add_line_("# MPI communicator for masters of each group", text);
      add_line_("master : MPI_Comm", text);
    }
  end_bloc_();
}


void Ecrire_YAML::declare_data_(int save, std::string& text)
{
  begin_bloc_("data:  # data we want to save (essentially fields of unknown)", text);

  for(unsigned i_pb=0; i_pb<pbs_.size(); i_pb++)
    {
      for(const auto& f: pbs_[i_pb].fields)
        {
          std::string name = f.first;
          std::string type = f.second.first;
          int nb_dim = f.second.second;
          declare_dtab_(name, type, nb_dim, text);
        }
      // additional scalar data:
      for(const auto& s: pbs_[i_pb].scalars)
        {
          std::string scal = s.first + " : " + s.second;
          add_line_(scal, text);
        }
    }
  if(!save)
    add_line_("temps_sauvegardes : { type: array, subtype: double, size: [ \'$iter + 1' ]  }", text);

  end_bloc_();
}

void Ecrire_YAML::set_data_()
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

void Ecrire_YAML::declare_array_(std::string name, std::string type, std::string size, std::string& text)
{
  std::string decl = name + " : { type: array, subtype: " + type + ", size: " + size + " }";
  add_line_(decl, text);
}

void Ecrire_YAML::declare_dtab_(std::string name, std::string type, int nb_dim, std::string& text)
{
  std::string data = name + " : { type: array, subtype: " + type + ", size: [ \'$dim_" + name + "[0]\'";
  for(int d=1; d<nb_dim; d++)
    data = data + "," + "\'$dim_" + name + "[" + std::to_string(d) + "]\'";
  data = data + " ] }";
  add_line_(data, text);
}

void Ecrire_YAML::declare_dataset_(std::string fname, std::string type, int nb_dim, std::string& text)
{
  std::string dataset_dim = fname + ": { type: array, subtype: " + type + ", size: [";
  if(Process::is_parallel())
    dataset_dim = dataset_dim + "\'$nodeSize\', ";
  dataset_dim = dataset_dim + "\'$nb_iter_max\', ";
  dataset_dim = dataset_dim + "\'$glob_dim_" + fname + "\'";
  for(int d=1; d<nb_dim; d++)
    dataset_dim = dataset_dim + ", \'$dim_" + fname + "[" + std::to_string(d) + "]\'";
  dataset_dim = dataset_dim + " ] }";
  add_line_(dataset_dim, text);
}

void Ecrire_YAML::write_attribute_(std::string dname, std::string fname, std::string& text)
{
  std::string header = fname + ":";
  begin_bloc_(header, text);
  std::string dataset_name = "dataset: " + dname;
  add_line_(dataset_name, text);
  end_bloc_();
}

void Ecrire_YAML::write_scalar_(std::string dname, std::string fname, std::string cond, std::string& text)
{
  std::string header = fname + ":";
  begin_bloc_(header, text);
  if(!cond.empty())
    {
      std::string condition = "when: \'" + cond + "\'";
      add_line_(condition, text);
    }
  std::string dataset_name = "dataset: " + dname;
  add_line_(dataset_name, text);
  write_scalar_selection_(text);
  end_bloc_();
}

void Ecrire_YAML::write_scalar_selection_(std::string& text)
{
  begin_bloc_("dataset_selection:", text);
  add_line_("size: [1]", text);
  std::string start = "start: [\'$iter\']";
  add_line_(start, text);
  end_bloc_();
}

void Ecrire_YAML::write_dtab_(std::string fname, int nb_dim, std::string cond, std::string& text)
{
  std::string header = fname + ":";
  begin_bloc_(header, text);
  if(!cond.empty())
    {
      std::string condition = "when: \'" + cond + "\'";
      add_line_(condition, text);
    }
  std::string dataset_name = "dataset: " + fname;
  add_line_(dataset_name, text);
  write_dtab_selection_(fname, nb_dim, text);
  end_bloc_();
}

void Ecrire_YAML::write_dtab_selection_(std::string fname, int nb_dim, std::string& text)
{
  begin_bloc_("dataset_selection:", text);
  std::string size =  "size: [";
  if(Process::is_parallel()) size = size + "1, ";
  size = size + "1, ";
  size = size + "\'$dim_" + fname + "[0]\'";
  for(int d=1; d<nb_dim; d++)
    size = size + "," + "\'$dim_" + fname + "[" + std::to_string(d) + "]\'";
  size = size + "]";
  add_line_(size, text);

  std::string start = "start: [";
  if(Process::is_parallel()) start = start + "\'$nodeRk\',";
  start = start + "\'$iter\',";
  start = start + "0";
  for(int d=1; d<nb_dim; d++)
    start = start + "," + "0";
  start = start + "]";
  add_line_(start, text);
  end_bloc_();
}

