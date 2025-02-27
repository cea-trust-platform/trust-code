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

#ifndef Ecrire_YAML_included
#define Ecrire_YAML_included

#include <TRUST_Ref.h>
#include <YAML_data.h>
#include <Nom.h>
#include <string>
#include <vector>
#include <map>

class Probleme_base;

/*! @brief classe Ecrire_YAML Use this to generate a yaml file that will then be read by the PDI library (for checkpoint/restart or for domain partitioning)
 *
 * The data that will be written are the ones that have been set in Equation_base::data_a_sauvegarder() (+overloads)
 * and Postraitement_base::data_a_sauvegarder() (+ overloads)
 *
 * These data must then be shared with PDI (during the simulation) in order to trigger the wanted IO operations, see also the class TRUST_2_PDI
 *
 * The structure of the file is as follows (indentation matters!):
 *
 * Metadata:
 *   mdata1 : type                                     # for scalar
 *   mdata2 : { type: array, subtype: ..., size: ... } # for arrays
 *      ...
 * Data:
 *   data1 : type
 *      ...
 * Plugins:
 *   plugin1:
 *   plugin2:
 *      ...
 *
 * We're using the HDF5 plugin. Here is an example of how to declare a HDF5 dataset (structure gathering data of the same type) and write our TRUST data into it:
 * - file: filename
 *   datasets:
 *     dataset1: { type: array, subtype: ..., size: ... }  #declaration of first dataset
 *     dataset2: { type: array, subtype: ..., size: ... }  #declaration of second dataset
 *        ...
 *   write:
 *     data1:                                              # TRUST data (it has to be declared in the data section first!)
 *       dataset: dataset1                                 # the dataset to which the data belongs
 *       dataset_selection:
 *         size: [...,...]                                 # What is the size of the section in the dataset (for every dimension) we will work with ?
 *         start:[...,...]                                 # Where do we want to start to write/read the data in the dataset (for every dimension)?
 *
 */
class Ecrire_YAML
{
public:
  Ecrire_YAML() : indent_(2) { }

  void write_checkpoint_file(const std::string& yaml_fname);
  void write_restart_file(const std::string& yaml_fname);
  void write_champ_fonc_restart_file(const std::string& filename);

  // Adds a problem to write into the YAML file
  // (with the name of the corresponding checkpoint filename)
  // (we can have independent checkpoint files for each problem or a common one)
  // This has to be called before generating the YAML file for checkpoint/restart!
  void add_pb_base(const Probleme_base& pb_base, const Nom& file_name)
  {
    Pb2Save new_pb;
    new_pb.pb = pb_base;

    // if we're in parallel, we need to index the checkpoint filename with the node id
    int with_ext = file_name.find(".");
    if(Process::is_parallel() && with_ext >= 0)
      {
        std::string suffix = file_name.getString().substr(with_ext+1);
        std::string prefix = file_name.getString().substr(0,with_ext);
        new_pb.filename =  prefix + "_${nodeId}." + suffix;
      }
    else
      new_pb.filename = file_name;

    pbs_.push_back(new_pb);
  }

  void add_field(Nom pb, Nom nom, int nb_dim);
  void add_scalar(Nom pb, Nom nom, Nom type, bool is_local);

private:
  // Begins a new bloc in the YAML file
  void begin_bloc(const std::string& line, std::string& text)
  {
    add_line(line, text);
    indent_ += 2;
  }
  // End the current bloc in the YAML file
  void end_bloc() { indent_ -= 2; }
  // Adds a line with the correct indentation in the YAML file
  void add_line(const std::string& line, std::string& text)
  {
    text = text + "\n";
    for(int i=0; i<indent_; i++)
      text = text + " ";
    text = text + line;
  }

  void set_data();

  // ==============================================================
  // Private methods to generate the YAML file that are common
  //                for checkpoint and restart
  // ==============================================================
  void write_checkpoint_restart_file(int save, const std::string& yaml_fname);
  void declare_metadata(int save, std::string& text);
  void declare_data(int save, std::string& text);
  void write_format(int save, const std::string& fname, std::string& text);
  void write_time_scheme(int save, const std::string& fname, std::string& text);

  // ==============================================================
  // Private methods to generate the YAML file for checkpoint only
  // ==============================================================
  void write_data_for_checkpoint(int pb_i, bool is_parallel, std::string& text);
  void write_file_initialization(int pb_i, std::string& text);
  void write_fields_types_for_checkpoint(int pb_i, std::string& text);

  // ==============================================================
  // Private methods to generate the YAML file for restart only
  // ==============================================================
  void write_data_for_restart(int pb_i, std::string& text);
  void write_fields_types_for_restart(int pb_i, std::string& text);

  // ==============================================================
  // Private methods useful to write generic structures
  // ==============================================================
  void declare_scalar(const std::string& name, const std::string& type, std::string& text);
  void declare_array(const std::string& name, const std::string& type, const std::string& size, std::string& text);
  void declare_dtab(const std::string& dname, const std::string& name, const std::string& type, int nb_dim, std::string& text);
  void declare_TRUST_dataset(const std::string& dname, const std::string& name, const std::string& type, int nb_dim, bool is_parallel, std::string& text);
  void write_impl_dataset(const std::string& dname, const std::string& fname, std::string& text);
  void write_impl_dataset(const std::string& dname, const std::string& fname, const std::vector<std::string>& attribute, std::string& text);
  void write_TRUST_dataset(const std::string& dname, const std::string& name, int nb_dim, const std::string& cond, bool is_parallel, const std::vector<std::string>& attribute, std::string& text);
  void write_TRUST_dataset_selection(const std::string& name, int nb_dim, bool is_parallel, std::string& text);
  void write_attributes(const std::vector<std::string>& attributes, std::string& text);

  // indicates the number of spaces to add at the beginning of the next line to write in the file (so that the YAML file correctly indented)
  int indent_;

  typedef struct
  {
    OBS_PTR(Probleme_base) pb;
    std::vector<YAML_data> data;
    std::string filename;
  } Pb2Save;
  // List of all the problems we want to save/restore
  // Each of the problems contain:
  // - a reference to the problem
  // - the list of the fields and the scalars to be saved/restored, with all the corresponding information
  // - the name of the corresponding checkpoint file
  std::vector<Pb2Save> pbs_;
};

#endif
