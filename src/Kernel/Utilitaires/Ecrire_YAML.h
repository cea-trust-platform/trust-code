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
  void write_checkpoint_file(Nom fname, int simple_save);
  void write_restart_file(Nom fname);

  void associer_pb_base(const Probleme_base& pb)
  {
    if (pb_.non_nul())
      {
        Cerr<<"Error: Problem while associating problem in Ecrire_YAML" <<finl;
        Process::exit();
      }
    pb_=pb;
  }

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

  void declare_datasets_dimensions_(std::string prefix, std::string& text);
  void set_datasets_selection_(std::string prefix, std::string suffix, std::string& text);

  void declare_metadata_(int save, std::string& text);
  void declare_data_(int save, std::string& text);

  void write_data_(Nom fname, int simple_checkpoint, std::string& text);
  void restore_data_(Nom fname, int simple_checkpoint, std::string& text);

  void restore_small_data_(Nom fname, Nom data, Nom dataset, std::string& text);
  void write_time_scheme_(Nom fname, std::string& text);
  void write_format_(Nom fname, std::string& text);

  void set_fields_();

  int indent_;
  REF(Probleme_base) pb_;
  LIST(REF(Champ_base)) fields_; //fields to save/restore

};

#endif
