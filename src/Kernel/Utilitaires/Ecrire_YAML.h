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

#ifndef Ecrire_YAML_included
#define Ecrire_YAML_included

#include <Probleme_base.h>

/*! @brief classe Ecrire_YAML Use this to generate a yaml file that will then be read by the PDI library (for checkpoint/restart or for domain partitioning)
 */
class Ecrire_YAML
{
public:
  Ecrire_YAML() : indent_(2) { }
  void write_checkpoint_file(std::string yaml_fname);
  void write_restart_file(std::string yaml_fname);
  void write_champ_fonc_restart_file(std::string filename);

  void add_pb_base(const Probleme_base& pb_base, Nom file_name)
  {
    Pb2Save new_pb;
    new_pb.pb = pb_base;
    new_pb.filename = file_name;
    pbs_.push_back(new_pb);
  }

  void add_field(Nom pb, Nom nom, int nb_dim);
  void add_scalar(Nom pb, Nom nom, Nom type);

private:
  void begin_bloc_(const std::string line, std::string& text)
  {
    add_line_(line, text);
    indent_ += 2;
  }
  void end_bloc_() { indent_ -= 2; }
  void add_line_(const std::string line, std::string& text)
  {
    text = text + "\n";
    for(int i=0; i<indent_; i++)
      text = text + " ";
    text = text + line;
  }

  void set_data_();

  void write_checkpoint_restart_file_(int save, std::string yaml_fname);
  void declare_metadata_(int save, std::string& text);
  void declare_data_(int save, std::string& text);

  void write_data_for_checkpoint_(int pb_i, std::string& text);
  void write_file_initialization_(int pb_i, std::string& text);
  void write_data_for_restart_(int pb_i, std::string& text);

  void write_format_(int save, std::string fname, std::string& text);
  void write_time_scheme_(int save, std::string fname, std::string& text);

  void declare_array_(std::string name, std::string type, std::string size, std::string& text);
  void declare_dtab_(std::string name, std::string type, int nb_dim, std::string& text);
  void declare_dataset_(std::string name, std::string type, int nb_dim, std::string& text);
  void write_attribute_(std::string dname, std::string fname, std::string& text);
  void write_scalar_(std::string dname, std::string fname, std::string cond, std::string& text);
  void write_scalar_selection_(std::string& text);
  void write_dtab_(std::string fname, int nb_dim, std::string cond, std::string& text);
  void write_dtab_selection_(std::string name, int nb_dim, std::string& text);

  int indent_;

  using Field2Type = std::map<std::string, std::pair<std::string, int>>; //fields to save/restore : fields["name_of_field"] = (type_of_the_field, dimensions_of_field)
  using Scalar2Type = std::map<std::string, std::string>;  //additional scalars to save/restore : scalars["name_of_scalar"] = type of the scalar
  typedef struct
  {
    OBS_PTR(Probleme_base) pb;
    Scalar2Type scalars;
    Field2Type fields;
    std::map<std::string, std::string> conditions; //for each field/scalar, is it a condition to respect in order to write it?
    std::string filename;
  } Pb2Save;
  std::vector<Pb2Save> pbs_;

};

#endif
