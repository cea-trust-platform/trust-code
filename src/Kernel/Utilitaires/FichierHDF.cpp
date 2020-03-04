/****************************************************************************
* Copyright (c) 2019, CEA
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
#include <communications.h>
#include <ArrOfInt.h>


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

void FichierHDF::read_datasets(Nom dataset_name, Entree_Brute& entree) {}
void FichierHDF::create_and_fill_datasets(Nom dataset_name, Sortie_Brute& data) {}

void FichierHDF::read_dataset(Nom dataset_name, Entree_Brute& entree) {}
void FichierHDF::create_and_fill_dataset(Nom dataset_name, Sortie_Brute& sortie) {}
void FichierHDF::create_and_fill_dataset(Nom dataset_name, SChaine& sortie) {}

bool FichierHDF::exists(const char* dataset_name)
{
  return true;
}
bool FichierHDF::is_hdf5(const char *file_name)
{
  return false;
}

void FichierHDF::prepare_file_props() {}
void FichierHDF::prepare_read_dataset_props() {}
void FichierHDF::create_and_fill_attribute(int data, const char* attribute_name) {}
#else

FichierHDF::FichierHDF():  file_id_(0), file_access_plst_(0),
  dataset_transfer_plst_(0), dataset_creation_plst_(0),
  chunk_size_(0) {}

FichierHDF::~FichierHDF()
{
  //close();
}

void FichierHDF::create(Nom filename)
{
  prepare_file_props();
  file_id_ = H5Fcreate(filename, H5F_ACC_TRUNC /*H5F_ACC_EXCL*/, H5P_DEFAULT, file_access_plst_);
}

void FichierHDF::open(Nom filename, bool readOnly)
{
  prepare_file_props();
  hid_t st = readOnly ? H5F_ACC_RDONLY : H5F_ACC_RDWR;
  file_id_ = H5Fopen(filename, st, file_access_plst_);
}

void FichierHDF::prepare_file_props()
{
  file_access_plst_ = H5Pcreate(H5P_FILE_ACCESS);
  //on cluster, type lfs getstripe . to see the default striping of the current repository
  hsize_t stripe_size = 1048576; //1572864;
  H5Pset_alignment(file_access_plst_, 0, stripe_size);
}

void FichierHDF::prepare_write_dataset_props(hsize_t datasetLen)
{
  dataset_transfer_plst_ = H5Pcreate(H5P_DATASET_XFER);

  dataset_creation_plst_ = H5Pcreate(H5P_DATASET_CREATE);
  get_chunking(datasetLen);
  if(chunk_size_)
    {
      H5Pset_layout(dataset_creation_plst_, H5D_CHUNKED);
      //hsize_t chunk_size = 1024; //524288, 1048576, 1572864;
      H5Pset_chunk(dataset_creation_plst_, 1, &chunk_size_);
    }
}

void FichierHDF::prepare_read_dataset_props()
{
  dataset_transfer_plst_ = H5Pcreate(H5P_DATASET_XFER);
}


void FichierHDF::close()
{
  H5Fclose(file_id_);
  if(file_access_plst_)    H5Pclose(file_access_plst_);
  if(dataset_transfer_plst_) H5Pclose(dataset_transfer_plst_);
  if(dataset_creation_plst_) H5Pclose(dataset_creation_plst_);
}

void FichierHDF::read_dataset(Nom dataset_name, Entree_Brute& entree)
{
  prepare_read_dataset_props();

  hid_t dataset_id = H5Dopen2(file_id_, dataset_name, H5P_DEFAULT);
  hid_t dataspace_id =  H5Dget_space(dataset_id);

  hsize_t sz;
  read_attribute(sz, "/sizes");
  hsize_t offset = mppartial_sum((int)sz);
  hid_t memspace = H5Screate_simple(1, &sz, NULL);

  H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, &offset, NULL, &sz, NULL);

  char * dset_data = new char[sz];
  H5Dread(dataset_id, H5T_NATIVE_OPAQUE, memspace, dataspace_id, dataset_transfer_plst_, dset_data);

  // Put extracted data in a standard Entree_Brute from TRUST, that we then use to feed TRUST objects
  entree.set_data(dset_data, sz);  // data are copied into the Entree

  delete[] dset_data;
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);
  H5Sclose(memspace);
}

void FichierHDF::read_datasets(Nom dataset_name, Entree_Brute& entree)
{
  prepare_read_dataset_props();

  hid_t dataset_id = H5Dopen2(file_id_, dataset_name, H5P_DEFAULT);
  hid_t dataspace_id =  H5Dget_space(dataset_id);
  hssize_t sz = H5Sget_simple_extent_npoints(dataspace_id);
  char * dset_data = new char[sz];
  H5Dread(dataset_id, H5T_NATIVE_OPAQUE, H5S_ALL, H5S_ALL, dataset_transfer_plst_, dset_data);

  // Put extracted data in a standard Entree_Brute from TRUST, that we then use to feed TRUST objects
  entree.set_data(dset_data, sz);  // data are copied into the Entree

  delete[] dset_data;
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);

}

void FichierHDF::create_and_fill_dataset(Nom dataset_name, Sortie_Brute& sortie)
{
  hsize_t lenData = sortie.get_size();
  const char * data = sortie.get_data();
  create_and_fill_dataset(dataset_name, lenData, data, H5T_NATIVE_OPAQUE, true /*write_attribute*/);
}

void FichierHDF::create_and_fill_dataset(Nom dataset_name, SChaine& sortie)
{
  hsize_t lenData = sortie.get_size();
  const char * data = sortie.get_str();
  create_and_fill_dataset(dataset_name, lenData, data, H5T_C_S1, false /*write_attribute*/);
}

void FichierHDF::create_and_fill_dataset(Nom dataset_name, hsize_t lenData, const char* data,
                                         hid_t datatype, bool write_attribute)
{
  hsize_t sumLenData = Process::mp_sum((int)lenData);
  prepare_write_dataset_props(sumLenData);

  hsize_t fileSize[1] = {sumLenData};
  hid_t fileSpace_id = H5Screate_simple(1, fileSize, NULL);

  hid_t dataset_id = H5Dcreate2(file_id_, dataset_name, datatype, fileSpace_id,
                                H5P_DEFAULT, dataset_creation_plst_, H5P_DEFAULT);

  hsize_t dims[1] = {lenData};
  hid_t dataspace_id = H5Screate_simple(1, dims, NULL);
  hsize_t offset = mppartial_sum((int)lenData);
  H5Sselect_hyperslab(fileSpace_id, H5S_SELECT_SET, &offset, NULL, &lenData, NULL);

  // Writing into it:
  //Cout << "Writing into HDF dataset " << dataset_name << finl;
  H5Dwrite(dataset_id, datatype, dataspace_id, fileSpace_id, dataset_transfer_plst_, data);

  // Close dataset and dataspace
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);
  H5Sclose(fileSpace_id);

  //writing length of the data owned by each proc as an attribute to keep track of it
  //(necessary if we want to re-read the datas)
  if(write_attribute)
    create_and_fill_attribute(lenData, "/sizes");

}

void FichierHDF::create_and_fill_datasets(Nom dataset_name, Sortie_Brute& sortie)
{
  //in this case, every proc will entirely write its own dataset: there's no need to chunk the dataset
  prepare_write_dataset_props(0);

  hsize_t lenData = sortie.get_size();
  const char * data = sortie.get_data();

  hsize_t dims[1] = {lenData};
  hid_t dataspace_id = H5Screate_simple(1, dims, NULL);

  // Create the dataset
  hid_t dataset_id = H5Dcreate2(file_id_, dataset_name, H5T_NATIVE_OPAQUE, dataspace_id,
                                H5P_DEFAULT, dataset_creation_plst_, H5P_DEFAULT);

  // Writing into it:
  Cout << "Writing into HDF dataset " << dataset_name << finl;
  H5Dwrite(dataset_id, H5T_NATIVE_OPAQUE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

  // Close dataset and dataspace
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);
}

void FichierHDF::create_and_fill_attribute(int data, const char* attribute_name)
{
  //attributes must be written collectively so every processor needs to write the same datas
  ArrOfInt attributes(Process::nproc());
  attributes = 0;
  attributes[Process::me()] = data;
  mp_sum_for_each_item(attributes);

  hsize_t dimsAttr = Process::nproc();
  hid_t space_id = H5Screate_simple(1, &dimsAttr, NULL);
  hid_t attr_id = H5Acreate2(file_id_, attribute_name, H5T_STD_I32BE, space_id, H5P_DEFAULT, H5P_DEFAULT);
  H5Awrite(attr_id, H5T_STD_I32BE, attributes.addr() );
  H5Aclose(attr_id);
  H5Sclose(space_id);

}

void FichierHDF::read_attribute(hsize_t& attribute, const char* attribute_name)
{
  ArrOfInt attributes(Process::nproc());
  hid_t attr = H5Aopen(file_id_, attribute_name, H5P_DEFAULT);
  H5Aread (attr, H5T_STD_I32BE, attributes.addr());
  H5Aclose(attr);

  attribute = attributes[Process::me()];
}

bool FichierHDF::exists(const char* dataset_name)
{
  htri_t exists = H5Lexists( file_id_, dataset_name, H5P_DEFAULT );
  return (exists>0);
}

bool FichierHDF::is_hdf5(const char * file_name)
{
  htri_t is_hdf5 = H5Fis_hdf5(file_name);
  return (is_hdf5>0);
}

void FichierHDF::get_chunking(hsize_t datasetLen)
{
  hsize_t OneMB = 1048576;
  hsize_t FourGB = 4294967296; //maximum limit tolerated by HDF5
  //if the dataset is not big enough, there's no need for chunking
  if(datasetLen > OneMB)
    {
      int nproc = Process::nproc();
      hsize_t tmp = datasetLen / nproc;
      chunk_size_ = (tmp >= FourGB) ? FourGB-1 : tmp;
    }
}

#endif
