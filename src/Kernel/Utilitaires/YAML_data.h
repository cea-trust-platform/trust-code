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

#ifndef YAML_data_included
#define YAML_data_included

#include <string>
#include <vector>

/*! @brief classe YAML_data : collection of all needed information for data to save/restore in order to write the YAML file used to initialize PDI
 *
 */
class YAML_data
{
public:
  YAML_data() { }
  YAML_data(const std::string& name, const std::string& type) : name_(name), type_(type), dimension_(0), hdf_name_(name) { }
  YAML_data(const std::string& name, const std::string& type, int nb_dim) : name_(name), type_(type), dimension_(nb_dim), hdf_name_(name) { }
  YAML_data(const std::string& name, const std::string& type, int nb_dim, bool loc) : name_(name), type_(type), dimension_(nb_dim), local_(loc), hdf_name_(name) { }
  YAML_data(const std::string& name, const std::string& type, int nb_dim, bool loc, bool is_ds) : name_(name), type_(type), dimension_(nb_dim), local_(loc), is_dataset_(is_ds), hdf_name_(name) { }

  // Setters
  void set_local(bool l) { local_ = l; }
  void set_conditions(const std::string& cond) { conditions_ = cond;  }
  void add_attribute(const std::string& attr)  { attributes_.push_back(attr); }
  void set_dataset(bool d) { is_dataset_ = d; }
  void set_hdf_name(const std::string& n) { hdf_name_ = n; }
  void set_save_field_type(bool b) { save_field_type_ = b; }

  // Getters
  const std::string& get_name() const { return name_; }
  const std::string& get_type() const { return type_; }
  int get_dims() const { return dimension_; }
  bool is_local() const { return local_; }
  const std::string& get_conditions() const { return conditions_; }
  const std::vector<std::string>& get_attributes() const { return attributes_; }
  bool is_dataset() const { return is_dataset_; }
  const std::string& get_hdf_name() const { return hdf_name_; }
  bool save_field_type() const { return save_field_type_; }

private:
  // name of the data
  std::string name_;
  // type of the data (careful, for arrays, the type refers to the type of the data it contains)
  std::string type_;
  // dimension of the data (0 for scalars)
  int dimension_;
  // are my data local to each processor (in which case, they will be written by everyone and local_=true)
  // or are they global (only master needs to write, local_= false)?
  bool local_ = true;
  // conditions to respect in order to read/write the data
  std::string conditions_;
  // list of attributes of the data
  std::vector<std::string> attributes_;
  // am I to be written as a dataset (must be set to false for attributes!)?
  bool is_dataset_ = true;
  // name of the HDF5 object it will be stored in (by default, same name as the data)
  std::string hdf_name_;
  // do we save the type of the field into the HDF file ?
  bool save_field_type_ = false;
};


#endif
