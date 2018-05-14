/****************************************************************************
* Copyright (c) 2017, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        FichierHDF.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <EntreeSortie.h>
#include <FichierHDF.h>

#ifndef MED_
FichierHDF::FichierHDF() {}
FichierHDF::~FichierHDF() {}

void FichierHDF::create(Nom filename) {}
void FichierHDF::open(Nom filename, bool readOnly) {}
void FichierHDF::close() {}

Entree_Brute FichierHDF::read_dataset(Nom dataset_name)
{
  return Entree_Brute();
}
void FichierHDF::create_and_fill_dataset(Nom dataset_name, Sortie_Brute data) {}
void FichierHDF::close_dataset(Nom dataset_name) {}


void FichierHDF::prepare_file_props() {}
void FichierHDF::prepare_read_dataset_props(Nom dataset_name) {}
#else

FichierHDF::FichierHDF()
  :  file_id_(0), file_prop_lst_(0), dataset_prop_lst_(0)
{
  Cerr << "FichierHDF needs HDF (so MED) to be used!" << finl;
  Process::exit(-1);

}

FichierHDF::~FichierHDF()
{
  close();
}

void FichierHDF::create(Nom filename)
{
  H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
}

void FichierHDF::open(Nom filename, bool readOnly)
{
  prepare_file_props();
  hid_t st = readOnly ? H5F_ACC_RDONLY : H5F_ACC_RDWR;
  file_id_ = H5Fopen(filename, st, file_prop_lst_);
}

void FichierHDF::prepare_file_props()
{
  file_prop_lst_ = H5P_DEFAULT;
}

void FichierHDF::prepare_read_dataset_props(Nom dataset_name)
{
  dataset_full_name_ = dataset_name;
  dataset_prop_lst_ = H5P_DEFAULT;
};


void FichierHDF::close()
{
  H5Fclose(file_id_);
  H5Pclose(file_prop_lst_);
}

void FichierHDF::read_dataset(Nom dataset_name, Entree_Brute& entree)
{
  prepare_read_dataset_props(dataset_name);

  hid_t dataset_id = H5Dopen2(file_id_, dataset_full_name_, H5P_DEFAULT);
  hid_t dataspace_id =  H5Dget_space(dataset_id);
  hssize_t sz = H5Sget_simple_extent_npoints(dataspace_id);
  char * dset_data = new char[sz];
  H5Dread(dataset_id, H5T_NATIVE_OPAQUE, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data);

  // Put extracted data in a standard Entree_Brute from TRUST, that we then use to feed TRUST objects
  entree.set_data(dset_data, sz);  // data are copied into the Entree

  delete[] dset_data;
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);
  H5Pclose(dataset_prop_lst_);
}

void FichierHDF::create_and_fill_dataset(Nom dataset_name, Sortie_Brute sortie)
{
  hsize_t lenData = sortie.get_size();
  const char * data = sortie.get_data();

  hsize_t dims[1] = {lenData};
  hid_t dataspace_id = H5Screate_simple(1, dims, NULL);

  // Create the dataset
  hid_t dataset_id = H5Dcreate2(file_id_, dataset_name, H5T_NATIVE_OPAQUE, dataspace_id,
                                H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  // Writing into it:
  Cerr << "Writing into HDF dataset " << dataset_name << finl;
  H5Dwrite(dataset_id, H5T_NATIVE_OPAQUE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

  // Close dataset and dataspace
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);
}

#endif
