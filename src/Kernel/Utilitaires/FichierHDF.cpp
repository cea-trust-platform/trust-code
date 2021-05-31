/****************************************************************************
* Copyright (c) 2021, CEA
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
#include <vector>
#include <communications.h>

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

void FichierHDF::fill_dataset(Nom dataset_name, const Sortie_Brute& sortie, bool append) {}
void FichierHDF::fill_dataset(Nom dataset_name, const SChaine& sortie, bool append) {}

void FichierHDF::read_dataset(Nom dataset_basename, Entree_Brute& entree) {}

bool FichierHDF::exists(const char* dataset_name)
{
  return false;
}
bool FichierHDF::is_hdf5(const char *file_name)
{
  return false;
}

void FichierHDF::prepare_file_props() {}
#else


std::map<std::string, hsize_t> FichierHDF::current_size;

FichierHDF::FichierHDF():  file_id_(0), file_access_plst_(0),
  dataset_transfer_plst_(0), dataset_creation_plst_(0) {}


FichierHDF::~FichierHDF()
{
  //close();
}

void FichierHDF::create(Nom filename)
{
  prepare_file_props();
  file_id_ = H5Fcreate(filename, H5F_ACC_TRUNC /*H5F_ACC_EXCL*/, H5P_DEFAULT, file_access_plst_);
  hdf5_error<hid_t>(file_id_);
  Cerr << "HDF5 " << filename << " file created !" << finl;
}

void FichierHDF::open(Nom filename, bool readOnly)
{
  prepare_file_props();
  hid_t st = readOnly ? H5F_ACC_RDONLY : H5F_ACC_RDWR;
  file_id_ = H5Fopen(filename, st, file_access_plst_);
  hdf5_error<hid_t>(file_id_);
  Cerr << "HDF5 " << filename << " file opened !" << finl;

}

void FichierHDF::prepare_file_props()
{
  file_access_plst_ = H5Pcreate(H5P_FILE_ACCESS);

  //increasing size of the B-tree containing metadata info (to approximately match the size striping)
  //useful when we are writing a lot of chunks
  // see https://www.nersc.gov/users/training/online-tutorials/introduction-to-scientific-i-o/?show_all=1
  // file_creation_plst_ = H5Pcreate(H5P_FILE_CREATE);
  // hsize_t btree_ik = (stripe_size - 4096) / 96;
  // H5Pset_istore_k(file_creation_plst_, btree_ik);
}

void FichierHDF::prepare_dataset_props(int dcpl, hsize_t chunk_size, bool is_bin)
{
  dataset_transfer_plst_ = H5Pcreate(H5P_DATASET_XFER);

  if(dcpl)
    {
      dataset_creation_plst_ = H5Pcreate(H5P_DATASET_CREATE);
      if(is_bin)
        {
          const char value = 0;
          hid_t datatype = H5T_NATIVE_CHAR;
          H5Pset_fill_value(dataset_creation_plst_, datatype, &value);
        }
      else
        {
          const char* value = " ";
          hid_t datatype = H5T_C_S1;
          H5Pset_fill_value(dataset_creation_plst_, datatype, value);
        }
      H5Pset_alloc_time(dataset_creation_plst_, H5D_ALLOC_TIME_EARLY);

      if(chunk_size>0)
        {
          H5Pset_layout(dataset_creation_plst_, H5D_CHUNKED);
          H5Pset_chunk(dataset_creation_plst_, 1, &chunk_size);
        }
    }
}

void FichierHDF::close()
{
  //getting file name
  char filename[100];
  hdf5_error<ssize_t>(H5Fget_name(file_id_, filename, 100));

  hdf5_error<herr_t>(H5Fclose(file_id_));

  if(file_access_plst_)    hdf5_error<herr_t>(H5Pclose(file_access_plst_));
  if(dataset_transfer_plst_) hdf5_error<herr_t>(H5Pclose(dataset_transfer_plst_));
  if(dataset_creation_plst_)  hdf5_error<herr_t>(H5Pclose(dataset_creation_plst_));

  Cerr << "HDF5 " << filename << " file closed !" << finl;

}

void FichierHDF::read_dataset(Nom dataset_name, Entree_Brute& entree)
{
  prepare_dataset_props();

  hid_t dataset_id = H5Dopen2(file_id_, dataset_name, H5P_DEFAULT);
  hdf5_error<hid_t>(dataset_id);
  hid_t dataspace_id =  H5Dget_space(dataset_id);
  hssize_t sz = H5Sget_simple_extent_npoints(dataspace_id);
  char * dset_data = new char[sz];
  Cerr << "[HDF5] Reading into HDF dataset " << dataset_name << "...";
  hdf5_error<herr_t>(H5Dread(dataset_id, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, dataset_transfer_plst_, dset_data));
  Cerr << "[HDF5] Done !" << finl;

  // Put extracted data in a standard Entree_Brute from TRUST, that we then use to feed TRUST objects
  entree.set_data(dset_data, sz);  // data are copied into the Entree

  delete[] dset_data;
  hdf5_error<herr_t>(H5Dclose(dataset_id));
  hdf5_error<herr_t>(H5Sclose(dataspace_id));
}


// WARNING: this function can only be called by the proc master
void FichierHDF::create_datasets(Noms dataset_names, hsize_t datasets_length, bool extendible, bool is_bin)
{
  if(!Process::je_suis_maitre())
    {
      Cerr << "FichierHDF::create_datasets can't be called simultaneously on several processors" << finl;
      Cerr << "You have to call it on proc master" << finl;
      return;
    }

  hid_t datatype = is_bin ? H5T_NATIVE_CHAR : H5T_C_S1;

  if(datasets_length == 0) datasets_length = 1024;
  hsize_t borne_sup = datasets_length * 2;
  hsize_t chunk_size;
  hid_t dataspace_id;
  if(extendible)
    {
      hsize_t dim_max[1] = { H5S_UNLIMITED };
      dataspace_id = H5Screate_simple(1, &borne_sup, dim_max);
      chunk_size = computeChunkSize(datasets_length);
    }
  else
    {
      dataspace_id = H5Screate_simple(1, &borne_sup, NULL);
      chunk_size = 0;
    }
  prepare_dataset_props(1 /* initialize creation dataset plist */, chunk_size, is_bin);

  hid_t dataset_id;
  // Create the dataset
  for(int i=0; i<dataset_names.size(); i++)
    {
      Nom dataset_name = dataset_names[i];
      dataset_id = H5Dcreate2(file_id_, dataset_name, datatype, dataspace_id, H5P_DEFAULT, dataset_creation_plst_, H5P_DEFAULT);
      hdf5_error<herr_t>(H5Dclose(dataset_id));
    }

  // Close dataset and dataspace
  hdf5_error<herr_t>(H5Sclose(dataspace_id));
  Cerr << "[HDF5] All datasets created !" << finl;
}


void FichierHDF::fill_dataset(Nom dataset_name, const SChaine& sortie, bool append)
{
  hid_t datatype = H5T_C_S1;
  hsize_t data_length = sortie.get_size();
  const char * data = sortie.get_str();
  fill_dataset(dataset_name, datatype, data_length, data, append);

}


void FichierHDF::fill_dataset(Nom dataset_name, const Sortie_Brute& sortie, bool append)
{
  hid_t datatype = H5T_NATIVE_CHAR;
  hsize_t data_length = sortie.get_size();
  const char * data = sortie.get_data();
  fill_dataset(dataset_name, datatype, data_length, data, append);
}

void FichierHDF::fill_dataset(Nom dataset_name, hid_t datatype, hsize_t data_length, const char *data, bool append)
{
  hsize_t dims[1] = {data_length};
  hid_t dataspace_id = H5Screate_simple(1, dims, NULL);

  // Open the dataset
  hid_t dataset_id = H5Dopen2(file_id_, dataset_name, H5P_DEFAULT);
  hdf5_error<hid_t>(dataset_id);
#ifndef NDEBUG
  hid_t dataset_dt = H5Dget_type(dataset_id);
  bool equal = H5Tequal( dataset_dt, datatype );
  hdf5_error<hid_t>(dataset_dt);
  if(!equal)
    {
      Cerr << "FichierHDF::fill_dataset error : wrong datatype" << finl;
      Process::exit();
    }
#endif
  // Select hyperslab in the dataset's filespace
  hid_t filespace_id = H5Dget_space(dataset_id);
  hsize_t start;

  std::map<std::string,hsize_t>::iterator it = current_size.find(dataset_name.getString());
  if(append && it != current_size.end())
    {
      start = current_size[dataset_name.getString()];
      current_size[dataset_name.getString()] += data_length;
    }
  else
    {
      start = 0;
      current_size[dataset_name.getString()] = data_length;
    }
  hsize_t count = data_length;
  hdf5_error<herr_t>(H5Sselect_hyperslab(filespace_id, H5S_SELECT_SET, &start, NULL, &count, NULL));

  // Writing into it:
  Cerr << "[HDF5] Writing into HDF dataset " << dataset_name << "...";
  hdf5_error<herr_t>(H5Dwrite(dataset_id, datatype, dataspace_id, filespace_id, dataset_transfer_plst_, data));
  Cerr << " Done !" << finl;

  // Close dataset and dataspace
  hdf5_error<herr_t>(H5Dclose(dataset_id));
  hdf5_error<herr_t>(H5Sclose(dataspace_id));
}

// WARNING: this function can only be called by the proc master
void FichierHDF::extend_datasets(Noms dataset_names, hsize_t extent_sz)
{
  if(!Process::je_suis_maitre())
    {
      Cerr << "FichierHDF::extend_datasets can't be called simultaneously on several processors" << finl;
      Cerr << "You have to call it on proc master" << finl;
      return;
    }

  extent_sz *= 2; //upper bound
  hid_t dataset_id, dspace_id;
  for(int i=0; i<dataset_names.size(); i++)
    {
      Nom dataset_name = dataset_names[i];
      dataset_id = H5Dopen2(file_id_, dataset_name, H5P_DEFAULT);
#ifndef NDEBUG
      //can't use dataset_creation_plst_ for the test
      //(if the file has been closed then reopened, dataset_creation_plst_ isn't correct)
      hid_t creation_plist = H5Dget_create_plist(dataset_id);
      bool equal = H5Pget_layout(creation_plist) == H5D_CHUNKED;
      hdf5_error<herr_t>(H5Pclose(creation_plist));
      if(!equal)
        {
          Cerr << "FichierHDF::extend_datasets error : you need a chunked dataset to extend it" << finl;
          Process::exit();
        }
#endif
      dspace_id = H5Dget_space(dataset_id);
      hssize_t current_sz = H5Sget_simple_extent_npoints(dspace_id);
      hsize_t new_sz = extent_sz + current_sz;
      H5Dset_extent(dataset_id, &new_sz);
      hdf5_error<herr_t>(H5Dclose(dataset_id));
      hdf5_error<herr_t>(H5Sclose(dspace_id));
    }
  Cerr << "[HDF5] All datasets extended !" << finl;
}

bool FichierHDF::exists(const char* dataset_name)
{
  return H5Lexists( file_id_, dataset_name, H5P_DEFAULT )>0;
}

bool FichierHDF::is_hdf5(const char * file_name)
{
  //we're going to disable error messages of this function call
  herr_t (*old_func)(hid_t, void*);
  void *old_client_data;
  H5Eget_auto(H5E_DEFAULT, &old_func, &old_client_data);
  H5Eset_auto(H5E_DEFAULT, NULL, NULL);
  htri_t ok = H5Fis_hdf5(file_name);
  H5Eset_auto(H5E_DEFAULT, old_func, old_client_data);

  return ok>0;

}

hsize_t FichierHDF::computeChunkSize(hsize_t datasets_length)
{
  hsize_t FourGB = 4294967296; //maximum limit tolerated by HDF5
  if( datasets_length > FourGB )
    return FourGB;
  else
    return datasets_length;

}

#endif
