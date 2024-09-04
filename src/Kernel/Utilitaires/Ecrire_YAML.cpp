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

void Ecrire_YAML::write_checkpoint_file(Nom fname, int simple_save)
{
  if (Process::je_suis_maitre())
    {
      set_fields_();

      std::string text = "pdi:";

      declare_metadata_(1 /*save*/, text);
      declare_data_(1 /*save*/, text);

      // declare plugins
      begin_bloc_("plugins:", text);
      add_line_("trace:", text);
      if(Process::is_parallel())
        add_line_("mpi:", text);
      add_line_("decl_hdf5:", text);

      write_data_(fname, simple_save, text);

      write_time_scheme_(fname, text);
      write_format_(fname, text);

      SFichier fic("save.yml");
      fic << text;
    }
  Process::barrier();
}

void Ecrire_YAML::write_restart_file(Nom fname)
{
  if (Process::je_suis_maitre())
    {
      set_fields_();

      std::string text = "pdi:";

      declare_metadata_(0 /*restart*/, text);
      declare_data_(0 /*restart*/, text);

      // declare plugins
      begin_bloc_("plugins:", text);
      add_line_("trace:", text);
      if(Process::is_parallel())
        add_line_("mpi:", text);
      add_line_("decl_hdf5:", text);

      restore_small_data_(fname, "last_iteration", "time/last_iteration", text);
      restore_small_data_(fname, "temps", "time/t", text);
      restore_small_data_(fname, "version", "format_sauvegarde/version", text);
      restore_small_data_(fname, "simple_sauvegarde", "format_sauvegarde/simple_sauvegarde", text);
      for (const auto& post_base : pb_->postraitements())
        {
          if (sub_type(Postraitement, post_base.valeur()))
            {
              const Postraitement& post = ref_cast(Postraitement, post_base.valeur());
              if(post.stat_demande() || post.stat_demande_definition_champs())
                {
                  restore_small_data_(fname, "stat_nb_champs", "statistiques/nb_champs", text);
                  restore_small_data_(fname, "stat_tdeb", "statistiques/tdeb", text);
                  restore_small_data_(fname, "stat_tend", "statistiques/tend", text);
                }
            }
        }

      restore_data_(fname, 1 /*in case of backup file with simple checkpoint*/, text);
      restore_data_(fname, 0 /*in case of backup file with complete checkpoint*/, text);

      SFichier fic("restart.yml");
      fic << text;
    }
  Process::barrier();
}

void Ecrire_YAML::restore_data_(Nom fname, int simple_checkpoint, std::string& text)
{
  std::string restart = "- file: " + fname.getString();
  begin_bloc_(restart, text);

  std::string checkpoint_type = simple_checkpoint ? "restart_from_simple_checkpoint" : "restart_from_complete_checkpoint";
  std::string event_name = "on_event: " + checkpoint_type;
  add_line_(event_name, text);
  if(Process::is_parallel())
    add_line_("communicator: $nodeComm", text);

  begin_bloc_("read:", text);
  std::string prefix = simple_checkpoint ? "" : "\'iter_$last_iteration/";
  std::string suffix = simple_checkpoint ? "" : "\'";
  set_datasets_selection_(prefix, suffix, text);
  end_bloc_();

  end_bloc_();
}


void Ecrire_YAML::write_data_(Nom fname, int simple_checkpoint, std::string& text)
{
  std::string checkpoint = "- file: " + fname.getString();
  begin_bloc_(checkpoint, text);
  std::string checkpoint_type = simple_checkpoint ? "simple_checkpoint" : "complete_checkpoint";
  std::string event = "on_event: " + checkpoint_type;
  add_line_(event, text);
  if(Process::is_parallel())
    add_line_("communicator: $nodeComm", text);

  begin_bloc_("datasets:", text);
  if(simple_checkpoint)
    {
      std::string no_prefix = "";
      declare_datasets_dimensions_(no_prefix,text);
    }
  else
    {
      int nb_iter = 4;  //ToDo::let the user chose the number of iterations he wants to save
      for(int iter=0; iter<nb_iter; iter++)
        {
          std::string iteration = "iter_" + std::to_string(iter) + "/";
          declare_datasets_dimensions_(iteration, text);
        }
    }
  end_bloc_();

  begin_bloc_("write:", text);
  std::string prefix = simple_checkpoint ? "" : "\'iter_${iter}/";
  std::string suffix = simple_checkpoint ? "" : "\'";
  set_datasets_selection_(prefix, suffix, text);

  // writing statistics data
  auto add_stats_var = [&](const std::string& varname)
  {
    std::string bloc = "stat_" + varname + ":";
    begin_bloc_(bloc, text);
    std::string dataset_name = "dataset: statistiques/" + varname;
    add_line_(dataset_name, text);
    end_bloc_();
  };
  for (const auto& post_base : pb_->postraitements())
    {
      if (sub_type(Postraitement, post_base.valeur()))
        {
          const Postraitement& post = ref_cast(Postraitement, post_base.valeur());
          if(post.stat_demande() || post.stat_demande_definition_champs())
            {
              add_stats_var("nb_champs");
              add_stats_var("tdeb");
              add_stats_var("tend");
            }
        }
    }
  end_bloc_();

  end_bloc_();
}

void Ecrire_YAML::restore_small_data_(Nom fname, Nom data, Nom dataset, std::string& text)
{
  std::string headline = "- file: " + fname.getString();
  begin_bloc_(headline, text);
  if(Process::is_parallel())
    add_line_("communicator: $nodeComm", text);

  begin_bloc_("read:", text);
  std::string data_name = data.getString() + ":";
  begin_bloc_(data_name, text);
  std::string dataset_name = "dataset: " + dataset.getString();
  add_line_(dataset_name, text);
  end_bloc_();
  end_bloc_();
  end_bloc_();
}

void Ecrire_YAML::write_time_scheme_(Nom fname, std::string& text)
{
  std::string format_sauvegarde = "- file: " + fname.getString();
  begin_bloc_(format_sauvegarde, text);
  if(Process::is_parallel())
    add_line_("communicator: $nodeComm", text);
  begin_bloc_("write:", text);
  begin_bloc_("version:", text);
  add_line_("dataset: format_sauvegarde/version", text);
  end_bloc_();
  begin_bloc_("simple_sauvegarde:", text);
  add_line_("dataset: format_sauvegarde/simple_sauvegarde", text);
  end_bloc_();
  end_bloc_();
  end_bloc_();
}

void Ecrire_YAML::write_format_(Nom fname, std::string& text)
{
  int nb_iter = 4;  //ToDo::let the user chose the number of iterations he wants to save
  std::string time_scheme = "- file: " + fname.getString();
  begin_bloc_(time_scheme, text);
  add_line_("on_event: time_scheme", text);
  if(Process::is_parallel())
    add_line_("communicator: $nodeComm", text);
  begin_bloc_("datasets:", text);
  std::string time = "time/t: { type: array, subtype: double, size: " + std::to_string(nb_iter) + " }";
  add_line_(time, text);
  end_bloc_();
  begin_bloc_("write:", text);
  begin_bloc_("temps:", text);
  add_line_("dataset: time/t", text);
  begin_bloc_("dataset_selection:", text);
  add_line_("size: [1]", text);
  add_line_("start: [\'$iter\']", text);
  end_bloc_();
  end_bloc_();
  begin_bloc_("iter:", text);
  add_line_("dataset: time/last_iteration", text);
  end_bloc_();
  end_bloc_();
  end_bloc_();
}


void Ecrire_YAML::declare_metadata_(int save, std::string& text)
{
  begin_bloc_("metadata:  # small values for which PDI keeps a copy", text);

  add_line_("# scheme information", text);
  if(save)
    add_line_("iter: int       # Number of checkpoints performed until now (WARNING: does not correspond to the current iteration in my time loop)", text);
  else
    add_line_("last_iteration : int # last saved iteration", text);

  add_line_("# information on format", text);
  add_line_("version : int", text);
  add_line_("simple_sauvegarde: int", text);

  for(const auto& f: fields_)
    {
      std::string name = f->le_nom().getString();
      int nb_dim = f->valeurs().nb_dim();

      std::string dim = "dim_" + name + " : { type: array, subtype: int, size: " + std::to_string(nb_dim) + " }";
      add_line_(dim, text);

      std::string glob_dim = "glob_dim_" + name + " : int";
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
      std::string name = f->le_nom().getString();
      int nb_dim = f->valeurs().nb_dim();
      std::string data_i = name + " : { type: array, subtype: double, size: [ \'$dim_" + name + "[0]\'";
      for(int d=1; d<nb_dim; d++)
        data_i = data_i + "," + "\'$dim_" + name + "[" + std::to_string(d) + "]\'";
      data_i = data_i + " ] }";
      add_line_(data_i, text);
    }

  // declaring statistics data
  for (const auto& post_base : pb_->postraitements())
    {
      if (sub_type(Postraitement, post_base.valeur()))
        {
          const Postraitement& post = ref_cast(Postraitement, post_base.valeur());
          if(post.stat_demande() || post.stat_demande_definition_champs())
            {
              std::string name = "stat_";
              std::string post_nb_champs = name + "nb_champs : int";
              add_line_(post_nb_champs, text);
              std::string post_tdeb = name + "tdeb : double";
              add_line_(post_tdeb, text);
              std::string post_tend = name + "tend : double";
              add_line_(post_tend, text);
            }
        }
    }

  if(save)
    add_line_("temps : double  # current physical time", text);
  else
    add_line_("temps : { type: array, subtype: double, size: [ \'$last_iteration + 1' ]  }", text);

  end_bloc_();
}

void Ecrire_YAML::declare_datasets_dimensions_(std::string prefix, std::string& text)
{
  for(const auto& f: fields_)
    {
      std::string name = f->le_nom().getString();
      int nb_dim = f->valeurs().nb_dim();

      std::string dataset_dim = prefix + name + ": { type: array, subtype: double, size: [";
      if(Process::is_parallel())
        dataset_dim = dataset_dim + "\'$nodeSize\',";
      dataset_dim = dataset_dim + "\'$glob_dim_" + name + "\'";
      for(int d=1; d<nb_dim; d++)
        dataset_dim = dataset_dim + ", \'$dim_" + name + "[" + std::to_string(d) + "]\'";
      dataset_dim = dataset_dim + " ] }";
      add_line_(dataset_dim, text);
    }

}

void Ecrire_YAML::set_datasets_selection_(std::string prefix, std::string suffix, std::string& text)
{
  for(const auto& f: fields_)
    {
      std::string name = f->le_nom().getString();
      int nb_dim = f->valeurs().nb_dim();

      std::string write_inc = name + ":";
      begin_bloc_(write_inc, text);
      std::string dataset_name = "dataset: " + prefix + name + suffix;
      add_line_(dataset_name, text);

      begin_bloc_("dataset_selection:", text);
      std::string size =  "size: [";
      if(Process::is_parallel()) size = size + "1, ";
      size = size + "\'$dim_" + name + "[0]\'";
      for(int d=1; d<nb_dim; d++)
        size = size + "," + "\'$dim_" + name + "[" + std::to_string(d) + "]\'";
      size = size + " ]";
      add_line_(size, text);
      std::string start = "start: [";
      if(Process::is_parallel()) start = start + "\'$nodeRk\',";
      start = start + "0,0]";
      add_line_(start, text);
      end_bloc_();
      end_bloc_();
    }

}


void Ecrire_YAML::set_fields_()
{
  assert(pb_.non_nul());

  // equations unknowns
  for(int i=0; i<pb_->nombre_d_equations(); i++)
    {
      for(int ch=0; ch<pb_->equation(i).nb_champs_a_sauvegarder(); ch++)
        {
          const REF(Champ_base)& inc = pb_->equation(i).champ_a_sauvegarder(ch).valeur();
          fields_.add(inc);
        }
    }

  // statistical post-processing fields
  for (const auto& post_base : pb_->postraitements())
    {
      if (sub_type(Postraitement, post_base.valeur()))
        {
          const Postraitement& post = ref_cast(Postraitement, post_base.valeur());
          if(post.stat_demande() || post.stat_demande_definition_champs())
            {
              for (const auto& ch_post : post.champs_post_complet())
                {
                  if (sub_type(Champ_Gen_de_Champs_Gen,ch_post.valeur()))
                    {
                      const Champ_Gen_de_Champs_Gen& ch_gen = ref_cast(Champ_Gen_de_Champs_Gen,ch_post.valeur());
                      REF(Champ_base) stat;
                      int has_source = post.get_stat_dans_les_sources(ch_gen, stat);
                      if(has_source)
                        {
                          assert(stat.non_nul());
                          fields_.add(stat);
                        }
                    }
                }
            }
        }
    }
}



