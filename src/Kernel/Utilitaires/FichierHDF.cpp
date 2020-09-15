/****************************************************************************
* Copyright (c) 2020, CEA
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
FichierHDF::FichierHDF()
{
  Cerr << "FichierHDF needs HDF (so MED) to be used!" << finl;
  Process::exit(-1);
}
FichierHDF::~FichierHDF() {}

void FichierHDF::create(Nom filename) {}
void FichierHDF::open(Nom filename, bool readOnly) {}
void FichierHDF::close() {}

void FichierHDF::read_dataset(Nom dataset_name, Entree_Brute& entree) {}
void FichierHDF::create_and_fill_dataset(Nom dataset_name, Sortie_Brute& data) {}

bool FichierHDF::exists(const char* dataset_name)
{
  return true;
}
bool FichierHDF::is_hdf5(const char *file_name)
{
  return false;
}

void FichierHDF::prepare_file_props() {}
void FichierHDF::prepare_dataset_props() {}
#else

FichierHDF::FichierHDF():  file_id_(0), file_access_plst_(0),
  dataset_transfer_plst_(0) {}

FichierHDF::~FichierHDF()
{
  //close();
}

void FichierHDF::create(Nom filename)
{
  prepare_file_props();
  file_id_ = H5Fcreate(filename, H5F_ACC_TRUNC /*H5F_ACC_EXCL*/, H5P_DEFAULT, file_access_plst_);
  Cerr << "HDF5 " << filename << " file created !" << finl;

}

void FichierHDF::open(Nom filename, bool readOnly)
{
  prepare_file_props();
  hid_t st = readOnly ? H5F_ACC_RDONLY : H5F_ACC_RDWR;
  file_id_ = H5Fopen(filename, st, file_access_plst_);
  Cerr << "HDF5 " << filename << " file opened !" << finl;

}

void FichierHDF::prepare_file_props()
{
  file_access_plst_ = H5Pcreate(H5P_FILE_ACCESS);
  //on cluster, type lfs getstripe . to see the default striping of the current repository
  hsize_t stripe_size = 1048576; //1572864;
  H5Pset_alignment(file_access_plst_, 0, stripe_size);

  //increasing size of the B-tree containing metadata info (to approximately match the size striping)
  //useful when we are writing a lot of chunks
  // see https://www.nersc.gov/users/training/online-tutorials/introduction-to-scientific-i-o/?show_all=1
  // file_creation_plst_ = H5Pcreate(H5P_FILE_CREATE);
  // hsize_t btree_ik = (stripe_size - 4096) / 96;
  // H5Pset_istore_k(file_creation_plst_, btree_ik);
}

void FichierHDF::prepare_dataset_props()
{
  dataset_transfer_plst_ = H5Pcreate(H5P_DATASET_XFER);
}


void FichierHDF::close()
{
  //getting file name
  char filename[100];
  H5Fget_name(file_id_, filename, 100);

  H5Fclose(file_id_);
  if(file_access_plst_)    H5Pclose(file_access_plst_);
  if(dataset_transfer_plst_) H5Pclose(dataset_transfer_plst_);
  Cerr << "HDF5 " << filename << " file closed !" << finl;

}

void FichierHDF::read_dataset(Nom dataset_basename, int proc_rank, Entree_Brute& entree)
{
  prepare_dataset_props();

  Nom dataset_name = dataset_basename;
  dataset_name = dataset_name.nom_me(proc_rank, 0, 1 /*without_padding*/);

  hid_t dataset_id = H5Dopen2(file_id_, dataset_name, H5P_DEFAULT);
  hid_t dataspace_id =  H5Dget_space(dataset_id);
  hssize_t sz = H5Sget_simple_extent_npoints(dataspace_id);
  char * dset_data = new char[sz];
  Cout << "[HDF5] Reading into HDF dataset " << dataset_name << "...";
  H5Dread(dataset_id, H5T_NATIVE_OPAQUE, H5S_ALL, H5S_ALL, dataset_transfer_plst_, dset_data);
  Cout << " Done !" << finl;

  // Put extracted data in a standard Entree_Brute from TRUST, that we then use to feed TRUST objects
  entree.set_data(dset_data, sz);  // data are copied into the Entree

  delete[] dset_data;
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);

}

void FichierHDF::create_and_fill_dataset(Nom dataset_basename, int proc_rank, Sortie_Brute& sortie)
{
  prepare_dataset_props();

  Nom dataset_name = dataset_basename;
  dataset_name = dataset_name.nom_me(proc_rank, 0, 1 /*without_padding*/);

  hsize_t lenData = sortie.get_size();
  const char * data = sortie.get_data();

  hsize_t dims[1] = {lenData};
  hid_t dataspace_id = H5Screate_simple(1, dims, NULL);

  // Create the dataset
  hid_t dataset_id = H5Dcreate2(file_id_, dataset_name, H5T_NATIVE_OPAQUE, dataspace_id,
                                H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  // Writing into it:
  Cout << "[HDF5] Writing into HDF dataset " << dataset_name << "...";
  H5Dwrite(dataset_id, H5T_NATIVE_OPAQUE, H5S_ALL, H5S_ALL, dataset_transfer_plst_, data);
  Cout << " Done !" << finl;

  // Close dataset and dataspace
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);
}

bool FichierHDF::exists(const char* dataset_name)
{
  return H5Lexists( file_id_, dataset_name, H5P_DEFAULT )>0;
}

bool FichierHDF::is_hdf5(const char * file_name)
{
  return H5Fis_hdf5(file_name)>0;
}

// void FichierHDF::get_chunking(hsize_t datasetLen)
// {
//   hsize_t FourGB = 4294967296; //maximum limit tolerated by HDF5
//   //if the dataset is not big enough, there's no need for chunking
//   if(datasetLen > FourGB)
//     chunk_size_ = FourGB;
// }

#endif
