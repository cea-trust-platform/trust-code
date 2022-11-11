/****************************************************************************
 * Copyright (c) 2015 - 2016, CEA
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
#ifndef IJK_LATA_WRITER_H_TPP
#define IJK_LATA_WRITER_H_TPP

#include <Noms.h>
#include <LataDB.h>
#include <EcrFicPartageBin.h>
#include <errno.h>
#include <IJK_Striped_Writer.h>
#include <Parallel_io_parameters.h>
// L'objet Sortie est prevu pour long, pas pour long long.
// Mais long est peut-etre seulement sur 32 bits.
// Verification: si erreur, il faut ajouter une sortie long long dans Sortie.h
static inline long CHECKLONG(long long x)
{
  if (sizeof(long) != 8)
    {
      Cerr << "ERROR checklong" << finl;
      Process::exit();
    }
  return (long) x;
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_add_geometry(const char *filename, const  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f)
{
  if (Process::je_suis_maitre())
    {
      const IJK_Splitting& splitting = f.get_splitting();
      SFichier master_file;
      Nom prefix = Nom(filename) + Nom(".");
      SFichier binary_file;
      binary_file.set_bin(1);
      ArrOfFloat tmp;
      int n;

      master_file.set_bin(0);
      master_file.ouvrir(filename, ios::app);
      Noms fname(3);
      const Nom& geomname = splitting.get_grid_geometry().le_nom();
      if (geomname == "??")
        {
          Cerr << "Error in  dumplata_header: geometry has no name" << finl;
          Process::exit();
        }
      for (int dir = 0; dir < 3; dir++)
        {
          fname[dir] = prefix + geomname + Nom(".coord") + Nom((char)('x'+dir));
          int i;
          binary_file.ouvrir(fname[dir]);
          const ArrOfDouble& coord = splitting.get_grid_geometry().get_node_coordinates(dir);
          n = coord.size_array();
          tmp.resize_array(n);
          for (i = 0; i < n; i++)
            tmp[i] = (float)coord[i];  //ToDo:: float here ????
          binary_file.put(tmp.addr(), n, 1);
          binary_file.close();
        }
      master_file << "Geom " << geomname << " type_elem=HEXAEDRE" << finl;
      master_file << "Champ SOMMETS_IJK_I " << basename(fname[0]) << " geometrie=" << geomname;
#ifdef INT_is_64_
      master_file << " file_offset=6";
#endif
      master_file << " size=" << splitting.get_grid_geometry().get_nb_elem_tot(0)+1 << " composantes=1" << finl;
      master_file << "Champ SOMMETS_IJK_J " << basename(fname[1]) << " geometrie=" << geomname;
#ifdef INT_is_64_
      master_file << " file_offset=6";
#endif
      master_file << " size=" << splitting.get_grid_geometry().get_nb_elem_tot(1)+1 << " composantes=1" << finl;
      master_file << "Champ SOMMETS_IJK_K " << basename(fname[2]) << " geometrie=" << geomname;
#ifdef INT_is_64_
      master_file << " file_offset=6";
#endif
      master_file << " size=" << splitting.get_grid_geometry().get_nb_elem_tot(2)+1 << " composantes=1" << finl;
      master_file.close();
    }
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_header(const char *filename, const  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f)
{
  dumplata_header(filename);
  dumplata_add_geometry(filename, f);
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_vector(const char *filename, const char *fieldname,
                     const  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz,
                     int step)
{
  Process::barrier();
  Nom prefix = Nom(filename) + Nom(".") + Nom(step) + Nom(".");
  Nom fd = prefix + fieldname;
  IJK_Striped_Writer striped_writer;
  const long long n = striped_writer.write_data_template<float,_TYPE_,_TYPE_ARRAY_>(fd, vx, vy, vz); //ToDo// FLOAT HERE TOO ???
  if (Process::je_suis_maitre())
    {
      SFichier master_file;
      master_file.ouvrir(filename, ios::app);
      const Nom& geomname = vx.get_splitting().get_grid_geometry().le_nom();

      master_file << "Champ " << fieldname << " " << basename(fd) << " geometrie=" << geomname;
#ifdef INT_is_64_
      master_file << " file_offset=6";
#endif
      master_file << " size=" << CHECKLONG(n) << " localisation=FACES" << " composantes=3" << " nature=vector" << finl;
    }
  Process::barrier();
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_vector_parallele_plan(const char *filename, const char *fieldname,
                                    const  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx, const  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy, const  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz,
                                    int step)
{
  //Process::barrier();
  Nom prefix = Nom(filename) + Nom(".") + Nom(step) + Nom(".");
  Nom fd_global = prefix + fieldname;
  Nom fd = prefix + fieldname + Nom(".") + Nom(Process::me());
  IJK_Striped_Writer striped_writer;
  const long long n = striped_writer.write_data_parallele_plan_template<float,_TYPE_,_TYPE_ARRAY_>(fd, vx, vy, vz);
  if (Process::je_suis_maitre())
    {
      SFichier master_file;
      master_file.ouvrir(filename, ios::app);
      const Nom& geomname = vx.get_splitting().get_grid_geometry().le_nom();

      master_file << "Champ " << fieldname << " " << basename(fd_global) << " geometrie=" << geomname;
#ifdef INT_is_64_
      master_file << " file_offset=6";
#endif
      master_file << " size=" << CHECKLONG(n) << " localisation=FACES" << " composantes=3" << " nature=vector" << finl;
    }
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_cellvector(const char *filename, const char *fieldname,
                         const FixedVector< IJK_Field_template<_TYPE_,_TYPE_ARRAY_>, 3>& v, int step)
{
  dumplata_scalar(filename,Nom(fieldname)+Nom("_X"), v[0], step);
  dumplata_scalar(filename,Nom(fieldname)+Nom("_Y"), v[1], step);
  dumplata_scalar(filename,Nom(fieldname)+Nom("_Z"), v[2], step);
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_scalar(const char *filename, const char *fieldname,
                     const  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f, int step)
{
  Process::barrier();
  SFichier master_file;
  Nom prefix = Nom(filename) + Nom(".") + Nom(step) + Nom(".");
  Nom fd = prefix + fieldname;
  IJK_Striped_Writer striped_writer;
  const long long n = striped_writer.write_data_template<float,_TYPE_,_TYPE_ARRAY_>(fd, f);
  if (Process::je_suis_maitre())
    {
      master_file.ouvrir(filename, ios::app);
      Nom loc;
      if (f.get_localisation() == IJK_Splitting::ELEM)
        loc = "ELEM";
      else
        loc = "SOM";
      const Nom& geomname = f.get_splitting().get_grid_geometry().le_nom();
      master_file << "Champ " << fieldname << " " << basename(fd) << " geometrie=" << geomname;
#ifdef INT_is_64_
      master_file << " file_offset=6";
#endif
      master_file << " size=" << (int)n << " localisation=" << loc << " composantes=1" << finl;
    }
  Process::barrier();
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void dumplata_scalar_parallele_plan(const char *filename, const char *fieldname,
                                    const  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f, int step)
{
  //Process::barrier();
  SFichier master_file;
  Nom prefix = Nom(filename) + Nom(".") + Nom(step) + Nom(".");
  Nom fd_global = prefix + fieldname;
  Nom fd = prefix + fieldname + Nom(".") + Nom(Process::me());
  IJK_Striped_Writer striped_writer;
  const long long n = striped_writer.write_data_parallele_plan_template<float,_TYPE_,_TYPE_ARRAY_>(fd, f);
  if (Process::je_suis_maitre())
    {
      master_file.ouvrir(filename, ios::app);
      Nom loc;
      if (f.get_localisation() == IJK_Splitting::ELEM)
        loc = "ELEM";
      else
        loc = "SOM";
      const Nom& geomname = f.get_splitting().get_grid_geometry().le_nom();
      master_file << "Champ " << fieldname << " " << basename(fd_global) << " geometrie=" << geomname;
#ifdef INT_is_64_
      master_file << " file_offset=6";
#endif
      master_file << " size=" << (int)n << " localisation=" << loc << " composantes=1" << finl;
    }
}

// If vy and vz refer to the same object thant vx, consider that we are loading 1 component...
// Strategy: try to read quite large chunks of contiguous data on disk.
// Reading only local data would generate many small reads (block size is typically 64-128 values in i).
// So we read several rows in i, then dispatch the row to the processors.
template<typename _TYPE_, typename _TYPE_ARRAY_>
void read_lata_parallel_template(const char *filename_with_path, int tstep, const char *geometryname, const char *fieldname,
                                 const int i_compo,
                                 IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& field)
{
  Cerr << "Reading lata field: file=" << filename_with_path
       << " tstep=" << tstep
       << " geom=" << geometryname
       << " field=" << fieldname
       << " component= " << i_compo << finl;
  Process::barrier(); // to print message before crash
  const IJK_Splitting& splitting = field.get_splitting();
  const int offset_j = splitting.get_offset_local(DIRECTION_J);
  const int offset_k = splitting.get_offset_local(DIRECTION_K);
  const int ni_local = field.ni();
  const int nj_local = field.nj();
  const int nk_local = field.nk();
  const int slice_i = splitting.get_local_slice_index(DIRECTION_I);
  const int slice_j = splitting.get_local_slice_index(DIRECTION_J);
  const int slice_k = splitting.get_local_slice_index(DIRECTION_K);
  const int n_slices_i = splitting.get_nprocessor_per_direction(DIRECTION_I);
  // Load data by batches of 32MB.
  const int batch_size = (int)(Parallel_io_parameters::get_max_block_size() / sizeof(_TYPE_));

  // If I have no data in the field, skip:
  if (slice_i >= 0)
    {
      // Processors on slice i=0 open the lata file:
      LataDB lata_db;
      const LataDBField *db_field = 0;
      // For each slice in i, number of field values:
      ArrOfInt ni_per_slice(n_slices_i);
      ArrOfInt slices_offsets_i;
      splitting.get_slice_offsets(DIRECTION_I, slices_offsets_i);
      // On processors that do not read data on disk, input_n?_tot will be -1:
      int input_ni_tot = -1, input_nj_tot = -1, input_nk_tot = -1;
      int number_of_j_per_batch = -1;
      if (slice_i == 0)
        {
          Nom path, dbname;
          split_path_filename(filename_with_path, path, dbname);
          lata_db.read_master_file(path, filename_with_path);
          const char * locstring;
          if (field.get_localisation() == IJK_Splitting::ELEM)
            locstring = "ELEM";
          else if (field.get_localisation() == IJK_Splitting::NODES)
            locstring = "SOM";
          else
            locstring = "FACES";
          db_field = &lata_db.get_field(tstep, geometryname, fieldname, locstring);
          input_ni_tot = (int)lata_db.get_field(tstep, geometryname, "SOMMETS_IJK_I", "", LataDB::FIRST_AND_CURRENT).size_;
          input_nj_tot = (int)lata_db.get_field(tstep, geometryname, "SOMMETS_IJK_J", "", LataDB::FIRST_AND_CURRENT).size_;
          input_nk_tot = (int)lata_db.get_field(tstep, geometryname, "SOMMETS_IJK_K", "", LataDB::FIRST_AND_CURRENT).size_;
          if (input_ni_tot-1 != splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_I)
              || input_nj_tot-1 != splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_J)
              || input_nk_tot-1 != splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_K))
            {
              Cerr << "Error: size mismatch. Current grid (number of elements): "
                   << splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_I) << " "
                   << splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_J) << " "
                   << splitting.get_grid_geometry().get_nb_elem_tot(DIRECTION_K) << finl;
              Cerr << "Grid in lata file: "
                   << input_ni_tot-1 << " " << input_nj_tot-1 << " " << input_nk_tot-1 << finl;
              Process::exit();
            }
          if (field.get_localisation() == IJK_Splitting::ELEM)
            {
              input_ni_tot--;
              input_nj_tot--;
              input_nk_tot--;
            }
          number_of_j_per_batch = batch_size / input_ni_tot;
          if (number_of_j_per_batch < 1)
            number_of_j_per_batch = 1;
          ni_per_slice[0] = ni_local;
          for (int islice = 1; islice < n_slices_i; islice++)
            {
              const int src_process = splitting.get_processor_by_ijk(islice, slice_j, slice_k);
              recevoir(ni_per_slice[islice], src_process, 0 /* mpi channel */);
              envoyer(number_of_j_per_batch, src_process, 0);
            }
        }
      else
        {
          // Send my slice size to processor 0:
          const int dest_process = splitting.get_processor_by_ijk(0, slice_j, slice_k);
          envoyer(ni_local, dest_process, 0 /* mpi channel */);
          recevoir(number_of_j_per_batch, dest_process, 0);
        }

      for (int k = 0; k < nk_local; k++)
        {
          const int global_k = offset_k + k;
          for (int j = 0; j < nj_local; j += number_of_j_per_batch)
            {
              // How many rows shall we read ?
              const int number_of_j_this_batch = std::min(nj_local - j, number_of_j_per_batch);
              const int global_j = offset_j + j;
              FloatTab   tmp_read; // Buffer where reading processes put data
              ArrOfDouble tmp_dispatch; // Buffer where other buffers receive their data
              if (slice_i == 0)
                {
                  // First processor in the row reads the data
                  const long long start = ((long long) global_k * input_nj_tot + global_j) * input_ni_tot;
                  const long long n = number_of_j_this_batch * input_ni_tot;
                  lata_db.read_data(*db_field, tmp_read, start, (int)n);
                  if (tmp_read.dimension(1) <= i_compo)
                    {
                      Cerr << "Error in read_lata_parallel_template: requested component " << i_compo
                           << " but only " << tmp_read.dimension(1) << " components are available" << finl;
                      Process::exit();
                    }
                  // Send some data to other processors:
                  // process in reverse order, so at the end tmp_dispatch has the data for the local slice:
                  for (int islice = n_slices_i - 1; islice >= 0; islice--)
                    {
                      const int ni_this_slice = ni_per_slice[islice];
                      // Select and copy data for this slice into tmp_dispatch
                      tmp_dispatch.resize_array(ni_this_slice * number_of_j_this_batch);
                      const int slice_offset_i = slices_offsets_i[islice];
                      for (int j2 = 0; j2 < number_of_j_this_batch; j2++)
                        {
                          for (int i = 0; i < ni_this_slice; i++)
                            {
                              int src_index = j2 * input_ni_tot + (slice_offset_i + i);
                              int dest_index = j2 * ni_this_slice + i;
                              tmp_dispatch[dest_index] = tmp_read(src_index, i_compo);
                            }
                        }
                      // Send tmp_dispatch to appropriate process
                      if (islice > 0)
                        {
                          const int dest_process = splitting.get_processor_by_ijk(islice, slice_j, slice_k);
                          envoyer(tmp_dispatch, dest_process, 0 /* mpi channel */);
                        }
                    }
                }
              const int src_process = splitting.get_processor_by_ijk(0, slice_j, slice_k);
              // Receive tmp_dispatch if not already here:
              if (slice_i > 0)
                {
                  recevoir(tmp_dispatch, src_process, 0 /* mpi channel */);
                }
              // Extract data from tmp_dispatch and copy to destination field:
              for (int j2 = 0; j2 < number_of_j_this_batch; j2++)
                {
                  for (int i = 0; i < ni_local; i++)
                    {
                      int src_index = j2 * ni_local + i;
                      field(i, j2 + j, k) = tmp_dispatch[src_index];
                    }
                }
            }
        }
    }
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void lire_dans_lata(const char *filename_with_path, int tstep, const char *geometryname, const char *fieldname,
                    IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& f)
{
  Process::barrier();
  if (Parallel_io_parameters::get_max_block_size() > 0)
    {
      read_lata_parallel_template<_TYPE_,_TYPE_ARRAY_>(filename_with_path, tstep, geometryname, fieldname, 0 /* component */,
                                                       f);
      Process::barrier();
      return;
    }


  if (f.get_localisation() != IJK_Splitting::ELEM && f.get_localisation() != IJK_Splitting::NODES)
    {
      Cerr << "Error in lire_dans_lata(scalar field): provided field has unsupported localisation" << finl;
      Process::exit();
    }
  const int master = Process::je_suis_maitre();
  // Collate data on processor 0
  const IJK_Splitting& splitting = f.get_splitting();
  const IJK_Splitting::Localisation loc = f.get_localisation();
  const int nitot = splitting.get_nb_items_global(loc, DIRECTION_I);
  const int njtot = splitting.get_nb_items_global(loc, DIRECTION_J);
  const int nktot = splitting.get_nb_items_global(loc, DIRECTION_K);

  Nom path, dbname;
  split_path_filename(filename_with_path, path, dbname);
  LataDB db;
  if (master)
    db.read_master_file(path, filename_with_path);
  //db.read_master_file(path, dbname);

  const char * locstring;
  if (f.get_localisation() == IJK_Splitting::ELEM)
    locstring = "ELEM";
  else
    locstring = "SOM";

  const LataDBField *db_field = 0;
  int is_double;
  if (master)
    {
      db_field = &db.get_field(tstep, geometryname, fieldname, locstring);

      if (db_field->size_ != nitot * njtot * nktot)
        {
          Cerr << "Error reading field " << geometryname << " / " << fieldname << " / " << locstring << " at timestep " << tstep;
          Cerr << " in file " << filename_with_path << " : wrong size\n";
          Cerr << " Expected size = " << nitot << " x " << njtot << " x " << nktot << " = " << nitot * njtot * nktot << "\n";
          Cerr << " Size in file  = " << CHECKLONG(db_field->size_) << finl;
          Process::exit();
        }
      is_double = (db_field->datatype_.type_ == LataDBDataType::REAL64);
    }
  envoyer_broadcast(is_double, 0);
  if (!is_double)
    {
      FloatTab data;
      if (master)
        db.read_data(*db_field, data);
      IJK_Striped_Writer reader;
      reader.redistribute_load(data, f, nitot, njtot, nktot, 1 /* total nbcompo */, 0 /* this component */);
    }
  else
    {
      DoubleTab data;
      if (master)
        db.read_data(*db_field, data);
      IJK_Striped_Writer reader;
      reader.redistribute_load(data, f, nitot, njtot, nktot, 1 /* total nbcompo */, 0 /* this component */);
    }
  Process::barrier();
}

template<typename _TYPE_, typename _TYPE_ARRAY_>
void lire_dans_lata(const char *filename_with_path, int tstep, const char *geometryname, const char *fieldname,
                    IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vx,  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vy,  IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& vz)
{
  Process::barrier();
  if (Parallel_io_parameters::get_max_block_size() > 0)
    {
      read_lata_parallel_template<_TYPE_,_TYPE_ARRAY_>(filename_with_path, tstep, geometryname, fieldname, 0 /* component */, vx);
      read_lata_parallel_template<_TYPE_,_TYPE_ARRAY_>(filename_with_path, tstep, geometryname, fieldname, 1 /* component */, vy);
      read_lata_parallel_template<_TYPE_,_TYPE_ARRAY_>(filename_with_path, tstep, geometryname, fieldname, 2 /* component */, vz);
      Process::barrier();
      return;
    }


  if (vx.get_localisation() != IJK_Splitting::FACES_I
      || vy.get_localisation() != IJK_Splitting::FACES_J
      || vz.get_localisation() != IJK_Splitting::FACES_K)
    {
      Cerr << "Error in lire_dans_lata(vx, vy, vz): provided fields have incorrect localisation" << finl;
      Process::exit();
    }
  const IJK_Splitting& splitting = vx.get_splitting();
  // Collate data on processor 0
  // In lata format, the velocity is written as an array of (vx, vy, vz) vectors.
  // Size of the array is the total number of nodes in the mesh.
  // The velocity associated with a node is the combination of velocities at the faces
  // at the right of the node (in each direction).
  const int nitot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 0) + 1;
  const int njtot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 1) + 1;
  const int nktot = splitting.get_nb_items_global(IJK_Splitting::ELEM, 2) + 1;

  const int master = Process::je_suis_maitre();

  Nom path, dbname;
  split_path_filename(filename_with_path, path, dbname);
  LataDB db;
  if (master)
    db.read_master_file(path, filename_with_path);
  //db.read_master_file(path, dbname);

  const char * locstring = "FACES";

  const LataDBField *db_field = 0;
  int is_double;
  if (master)
    {
      db_field = &db.get_field(tstep, geometryname, fieldname, locstring);

      if (db_field->size_ != nitot * njtot * nktot)
        {
          Cerr << "Error reading field " << geometryname << " / " << fieldname << " / " << locstring << " at timestep " << tstep;
          Cerr << " in file " << filename_with_path << " : wrong size\n";
          Cerr << " Expected size = " << nitot << " x " << njtot << " x " << nktot << " = " << nitot * njtot * nktot << "\n";
          Cerr << " Size in file  = " << CHECKLONG(db_field->size_) << finl;
          Process::exit();
        }
      is_double = (db_field->datatype_.type_ == LataDBDataType::REAL64);
    }
  envoyer_broadcast(is_double, 0);

  if (!is_double)
    {
      FloatTab data;
      if (master)
        db.read_data(*db_field, data);
      IJK_Striped_Writer reader;
      reader.redistribute_load(data, vx, nitot, njtot, nktot, 3 /* total nbcompo */, 0 /* this component */);
      reader.redistribute_load(data, vy, nitot, njtot, nktot, 3 /* total nbcompo */, 1 /* this component */);
      reader.redistribute_load(data, vz, nitot, njtot, nktot, 3 /* total nbcompo */, 2 /* this component */);
    }
  else
    {
      DoubleTab data;
      if (master)
        db.read_data(*db_field, data);
      IJK_Striped_Writer reader;
      reader.redistribute_load(data, vx, nitot, njtot, nktot, 3 /* total nbcompo */, 0 /* this component */);
      reader.redistribute_load(data, vy, nitot, njtot, nktot, 3 /* total nbcompo */, 1 /* this component */);
      reader.redistribute_load(data, vz, nitot, njtot, nktot, 3 /* total nbcompo */, 2 /* this component */);
    }
  Process::barrier();
}

#endif
