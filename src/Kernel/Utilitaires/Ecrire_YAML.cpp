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

#include <Ecrire_YAML.h>
#include <Equation_base.h>
#include <Postraitement.h>
#include <Champ_Generique_Statistiques_base.h>

void Ecrire_YAML::write_checkpoint_file(Nom fname)
{
  if (Process::je_suis_maitre())
    write_checkpoint_restart_file_(1 /*save*/, fname);
  Process::barrier();
}

void Ecrire_YAML::write_restart_file(Nom fname)
{
  if (Process::je_suis_maitre())
    write_checkpoint_restart_file_(0 /*restart*/, fname);
  Process::barrier();
}

void Ecrire_YAML::write_checkpoint_restart_file_(int save, Nom fname)
{
  set_data_();

  std::string text = "pdi:";

  declare_metadata_(save, text);
  declare_data_(save, text);

  // declare plugins
  begin_bloc_("plugins:", text);
  add_line_("trace:", text);
  if(Process::is_parallel())
    add_line_("mpi:", text);
  add_line_("decl_hdf5:", text);

  if(save)
    write_data_for_checkpoint_(fname, text);
  else
    write_data_for_restart_(fname, text);
  write_metadata_(save, fname, text);

  std::string yaml_fname = save ? "save.yml" : "restart.yml";
  SFichier fic(yaml_fname.c_str());
  fic << text;
}

void Ecrire_YAML::write_metadata_(int save, Nom fname, std::string& text)
{
  std::string format_sauvegarde = "- file: " + fname.getString();
  begin_bloc_(format_sauvegarde, text);
  if(Process::is_parallel())
    add_line_("communicator: $nodeComm", text);
  std::string IO = save ? "write:" : "read:";
  begin_bloc_(IO, text);
  write_attribute_("format_sauvegarde/version", "version", text);
  write_attribute_("format_sauvegarde/simple_sauvegarde", "simple_sauvegarde", text);
  end_bloc_();
  end_bloc_();
}

void Ecrire_YAML::write_time_data_(int save, Nom fname, std::string& text)
{
  std::string time_scheme = "- file: " + fname.getString();
  begin_bloc_(time_scheme, text);
  if(save) add_line_("on_event: time_scheme", text);
  if(Process::is_parallel())
    add_line_("communicator: $nodeComm", text);

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
    write_scalar_("time/t", "temps", text);
  else
    write_attribute_("time/t", "temps", text);
  write_attribute_("time/last_iteration", "iter", text);
  end_bloc_();
  end_bloc_();
}

void Ecrire_YAML::write_data_for_checkpoint_(Nom fname, std::string& text)
{
  std::string checkpoint = "- file: " + fname.getString();
  begin_bloc_(checkpoint, text);
  std::string event = "on_event: backup";
  add_line_(event, text);
  if(Process::is_parallel())
    add_line_("communicator: $nodeComm", text);

  // declaring datasets
  begin_bloc_("datasets:", text);
  for(const auto& f: fields_)
    {
      std::string name = f.first;
      std::string type = f.second.first;
      int nb_dim = f.second.second;
      declare_dataset_(name, type, nb_dim, text);
    }
  for (auto const& scal : scalars_)
    {
      const std::string& name = scal.first;
      const std::string& type = scal.second;
      declare_array_(name, type, "\'$nb_iter_max\'", text);
    }
  end_bloc_();


  begin_bloc_("write:", text);
  // writing fields
  for(const auto& f: fields_)
    {
      std::string name = f.first;
      int nb_dim = f.second.second;
      write_dtab_(name, nb_dim, text);
    }
  // writing scalars
  for(const auto& scal: scalars_)
    {
      const std::string& name = scal.first;
      write_scalar_(name, name, text);
    }
  end_bloc_();
  end_bloc_();

  // writing time
  write_time_data_(1, fname, text);

}

void Ecrire_YAML::write_data_for_restart_(Nom fname, std::string& text)
{
  // writing fields
  for(const auto& f: fields_)
    {
      std::string checkpoint = "- file: " + fname.getString();
      begin_bloc_(checkpoint, text);
      if(Process::is_parallel())
        add_line_("communicator: $nodeComm", text);

      begin_bloc_("read:", text);
      std::string name = f.first;
      int nb_dim = f.second.second;
      write_dtab_(name, nb_dim, text);
      end_bloc_();
      end_bloc_();
    }
  // writing scalars
  for(const auto& scal: scalars_)
    {
      std::string checkpoint = "- file: " + fname.getString();
      begin_bloc_(checkpoint, text);
      if(Process::is_parallel())
        add_line_("communicator: $nodeComm", text);

      begin_bloc_("read:", text);
      const std::string& name = scal.first;
      write_scalar_(name, name, text);
      end_bloc_();
      end_bloc_();
    }

  // writing time
  write_time_data_(0, fname, text);

}

void Ecrire_YAML::declare_metadata_(int save, std::string& text)
{
  begin_bloc_("metadata:  # small values for which PDI keeps a copy", text);

  add_line_("# scheme information", text);
  if(save)
    {
      add_line_("iter: int         # Number of checkpoints performed until now (WARNING: does not correspond to the current iteration in my time loop)", text);
      add_line_("nb_iter_max: int  # Maximum number of checkpoints (WARNING: if this number is too small, we overwrite the first checkpoints)", text);
    }
  else
    add_line_("iter : int # last saved iteration", text);

  add_line_("# information on format", text);
  add_line_("version : int", text);
  add_line_("simple_sauvegarde: int", text);

  for(const auto& f: fields_)
    {
      std::string fname = f.first;
      int nb_dim = f.second.second;
      std::string asize = std::to_string(nb_dim);
      std::string aname = "dim_" + fname;
      declare_array_(aname, "int", asize, text);

      std::string glob_dim = "glob_dim_" + fname + " : int";
      add_line_(glob_dim, text);
    }

  if(Process::is_parallel())
    {
      add_line_("# metadata regarding parallelism", text);
      add_line_("# MPI communicator for my group", text);
      add_line_("nodeComm : int", text);
      add_line_("# number of processors inside my group", text);
      add_line_("nodeSize : int", text);
      add_line_("# my rank inside my group", text);
      add_line_("nodeRk : int", text);
    }
  end_bloc_();
}


void Ecrire_YAML::declare_data_(int save, std::string& text)
{
  begin_bloc_("data:  # data we want to save (essentially fields of unknown)", text);
  for(const auto& f: fields_)
    {
      std::string name = f.first;
      std::string type = f.second.first;
      int nb_dim = f.second.second;
      declare_dtab_(name, type, nb_dim, text);
    }
  // additional scalar data:
  for(const auto& s: scalars_)
    {
      std::string scal = s.first + " : " + s.second;
      add_line_(scal, text);
    }
  if(save)
    add_line_("temps : double  # current physical time", text);
  else
    add_line_("temps : { type: array, subtype: double, size: [ \'$iter + 1' ]  }", text);

  end_bloc_();
}

void Ecrire_YAML::set_data_()
{
  assert(pb_.non_nul());

  // equations unknowns
  for(int i=0; i<pb_->nombre_d_equations(); i++)
    {
      pb_->equation(i).champ_a_sauvegarder(fields_);
      pb_->equation(i).scal_a_sauvegarder(scalars_);
    }

  // statistical post-processing fields
  for (const auto& post_base : pb_->postraitements())
    {
      post_base->champ_a_sauvegarder(fields_);
      post_base->scal_a_sauvegarder(scalars_);
    }
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

void Ecrire_YAML::write_scalar_(std::string dname, std::string fname, std::string& text)
{
  std::string header = fname + ":";
  begin_bloc_(header, text);
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

void Ecrire_YAML::write_dtab_(std::string fname, int nb_dim, std::string& text)
{
  std::string header = fname + ":";
  begin_bloc_(header, text);
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

