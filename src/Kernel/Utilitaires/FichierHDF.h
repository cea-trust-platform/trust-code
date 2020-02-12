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
// File:        FichierHDF.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#ifndef FichierHDF_included
#define FichierHDF_included
#include <Process.h>
#include <Entree_Brute.h>
#include <Sortie_Brute.h>
#include <Nom.h>

#define MED_

#ifdef MED_
#include <hdf5.h>
#endif

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//   This abstract class provides all the functionalities to open and manipulate HDF files and related
//   concepts (datasets, groups, etc ...)
//   It intentionally does not place itself in the Sortie_Fichier_base or Entree_Fichier_base hierarchy
//   since it is rather the datasets inside the HDF file that are regarded as TRUST Fichier objects.
// .SECTION voir aussi
//
//////////////////////////////////////////////////////////////////////////////
class FichierHDF
{
public:
  FichierHDF();
  virtual ~FichierHDF();

  // Creates (and open) the HDF file.
  virtual void create(Nom filename);
  virtual void open(Nom filename, bool readOnly);
  virtual void close();

  // Single Writer Multiple Readers method:
  // a single proc writes all the datasets (one per proc)
  // and each proc reads its own dataset in the given file
  virtual void read_datasets(Nom dataset_name, Entree_Brute& entree);
  virtual void create_and_fill_datasets(Nom dataset_name, Sortie_Brute& data);

  // Multiple Writers Multiple Readers method:
  // each processor writes (and reads) its portion from the (unique) dataset in the given file
  // (not possible to write multiple datasets in parallel as they overwrite each other)
  virtual void read_dataset(Nom dataset_name, Entree_Brute& entree);
  virtual void create_and_fill_dataset(Nom dataset_name, Sortie_Brute& sortie);

  virtual void close_dataset(Nom dataset_name);

protected:
  virtual void prepare_file_props();
  virtual void prepare_dataset_props(Nom dataset_name);

#ifdef MED_
  hid_t file_id_;
  hid_t file_prop_lst_;
  hid_t dataset_prop_lst_;
#endif

  Nom dataset_full_name_; // the full name of the data set to be opened (with potential trailing _000x)

private:
  // Forbid copy:
  FichierHDF& operator=(const FichierHDF&);
  FichierHDF(const FichierHDF&);
};
#endif
