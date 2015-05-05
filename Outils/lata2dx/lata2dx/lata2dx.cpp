/*****************************************************************************
*
* Copyright (c) 2011 - 2013, CEA
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of CEA, nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <iostream>
#include <fstream>
#include <LataDB.h>
#include <LataFilter.h>
#include <OpenDXWriter.h>
#include <LmlReader.h>
#include <Rebuild_virtual_layer.h>
#include <stdlib.h>
#ifdef WITH_MED
#include <MEDWriter.h>
#endif
#include <LataWriter.h>

using namespace std;

class Lata2dxOptions: public LataOptions
{
public:
  Lata2dxOptions();
  void describe();
  entier parse_option(const Nom & s);
  void parse_options(int argc, char **argv);

  // Input filter: when reading the input file, timesteps, domains
  //  and components not mentionned in these vectors are not read.
  // If empty list, write all items.
  ArrOfInt input_timesteps_filter;
  Noms     input_domains_filter;
  Noms     input_components_filter;

  // Output filter: when writing the output file, domains and components
  //  not mentionned here are not written. If empty list, write all items.
  Noms     output_domains_filter;
  Noms     output_components_filter;

  Noms     merge_files;

  // Which parallel subdomain shall we load (default = -1, load all subdomains)
  int load_subdomain;
  // Output file will be written in binary format
  int binary_out;
  // For OpenDX: include the output Field object in a group
  int forcegroup;
  
  // Which data processing do we want ?
  enum ProcessingOption { 
    WRITE_DX, 
    WRITE_LATA_MASTER, 
    WRITE_LATA_ALL, 
    WRITE_LATA_CONVERT,
    WRITE_MED,
    WRITE_PRM
  };
  ProcessingOption processing_option;

  enum TimeAverage { NO_TIME_AVERAGE, SIMPLE_TIME_AVERAGE, LINEAR_TIME_AVERAGE, RECTANGLES_TIME_AVERAGE };
  TimeAverage time_average_;

  Nom output_filename;

  // Shall we dump the list of available timesteps, domains and fields ?
  int dump_list;

  // Add virtual elements information in the lata file
  int compute_virtual_elements;

  // Lata output options:
  // split all: one file for each LataDB entry (one file for mesh nodes, another for mesh elements, etc)
  enum Lata_file_splitting { DEFAULT, SPLIT_ALL, SPLIT_COMPONENTS, SPLIT_TIMESTEPS, SPLIT_NONE };
  // default: keep like in source file
  // split components: one file for each component and for each timestep, merge geometry data into one file
  // split timesteps: one file for each timestep
  // split none: one big file with everything
  Lata_file_splitting lata_file_splitting;
  // Include fortran blocs markers
  int fortran_blocs;
  // Ordering of data in components files
  int use_fortran_data_ordering; // 1->fortran 0->c
  // Indexes begin at 1 (fortran_indexing=1) or 0
  int use_fortran_indexing;
  // Shall we compute the rms fluctuations ?
  int rms_fluctuations;
};

Lata2dxOptions::Lata2dxOptions()
{
  load_subdomain = -1; // load all subdomains
  binary_out = 1;
  forcegroup = 0;
  processing_option = WRITE_DX;
  dump_list = 0;
  lata_file_splitting = DEFAULT;
  compute_virtual_elements = 0;
  lata_file_splitting = DEFAULT;
  fortran_blocs = 1;
  use_fortran_data_ordering = 0;
  use_fortran_indexing = 1;
  time_average_ = NO_TIME_AVERAGE;
  rms_fluctuations = 0;
}

void Lata2dxOptions::describe()
{
  cerr << "Usage : lata2dx input_file_name" << endl;
  cerr << " [timestep=n]" << endl;
  cerr << " [domain=name]" << endl;
  cerr << " [component=label]" << endl;
  cerr << " [[binary|ascii]] [bigendian|littleendian]] [[int32|int64]] [[real32|real64]]" << endl;
  cerr << " [[binout|asciiout]] [[bigendianout|littleendianout]] [[int32out|int64out]] [[real32out|real64out]]" << endl;
  cerr << " [forcegroup]" << endl;
  cerr << " [regularize=tolerance [invalidate]]" << endl;
  cerr << " [reconnect=tolerance]" << endl;
  cerr << " [verbosity=n]" << endl;
  cerr << endl;
  cerr << "Database filtering options" << endl;
  cerr << " Filtering of input database, applied to the input file before data processing:" << endl;
  cerr << "  timestep=n      : Load the specified timestep. Can be specified several times." << endl;
  cerr << "                    If no one is specified, load all timesteps." << endl;
  cerr << "                    timestep=0 specifies the first TEMPS entry in the lata file." << endl;
  cerr << "                    To discard all timesteps and keep only global definitions, use timestep=-1" << endl;
  cerr << "  domain=label    : Read the specified domain from the input file. Can be specified several times." << endl;
  cerr << "                    If no one is specified, read all domains." << endl;
  cerr << "  component=label : Read the specified component (or field). Can be specified several times." << endl;
  cerr << "                    If no one is specified, load all components." << endl;
  cerr << "  subdomain=n :     Read only parallel subdomain number n (0<=n<nproc)" << endl;
  cerr << " Filtering of output database (only domains and fields specified here are output)" << endl;
  cerr << "  export_domain=label :   Request exporting specified domain (this list might include newly" << endl;
  cerr << "                          created domains like DOM_dual, DOM_nc, DOM_boundary, etc...)" << endl;
  cerr << "  export_component=label: Same for components (you can just specify the name, or a complete name" << endl;
  cerr << "                          (name or geom_name_localisation)" << endl;
  cerr << " Filtering information:" << endl;
  cerr << "  dumplist : Dump available timesteps, domains and components for input and output filtering" << endl;
  cerr << endl;
  cerr << "Output file format selection" << endl;
  cerr << " writedx                      : output an openDx file to stdout (this is the default)" << endl;
  cerr << "                                same data processing than writelata option" << endl;
  cerr << " writelata=path/file_with_ext : Write a lata master file and lata data files" << endl;
  cerr << "                                - database filtering (timestep, domain, component)" << endl;
  cerr << "                                - subdomain extraction" << endl;
  cerr << "                                - full data processing (reconnect, dualmesh, etc...)" << endl;
  cerr << "                                - data format conversion (asciiout, binaryout, int32out, ...)" << endl;
  cerr << " writelata_convert=path/masterfile : Rewrite a lata master file and lata data files" << endl;
  cerr << "                                - database filtering (timestep, domain, component)" << endl;
  cerr << "                                - data format conversion (asciiout, binaryout, int32out, ...)" << endl;
  cerr << " writelata_master=path/masterfile : Rewrite a lata master file" << endl;
  cerr << "                                - database filtering only, plus compute_virtual_elements, and writing a lataV2.1 master file" << endl;
  cerr << "                                (files containing data are not rewritten, master file points to existing files)" << endl;
  cerr << " writeprm                     : Print to stdout a .prm text that will generate a full report" << endl;
  cerr << "                                containing all possible fields and components" << endl;
  cerr << "Processing options" << endl;
  cerr << " compute_virtual_elements: Rebuild the VIRTUAL_ELEMENTS data and rewrite the master lata file" 
       << "           (if used, you must also specify writelata_master)" << endl;
  cerr << " mergelata=path/masterfile: Merge meshes (must have identical timesteps, geometries and fields)" << endl;
  cerr << " timeaverage=OPT: produces a file with only one timestep containing a time average of selected fields" << endl;
  cerr << "                  If used, you must also specify writelata. Verbosity level 3 prints the coefficients for each tstep." << endl;
  cerr << "                  timeaverage=simple: computes the sum of all timesteps, divided by number of timesteps" << endl;
  cerr << "                  timeaverage=linear: computes a piecewise linear integral between timesteps," << endl;
  cerr << "                                    taking into account non uniform timesteps. Needs at least 2 timesteps!" << endl;
  cerr << "                  timeaverage=rectangles: piecewise constant integral between timesteps, the value between" << endl;
  cerr << "                                    two timesteps is taken equal to the value at the end of the interval." << endl;
  cerr << " rms_fluctuations: If timeaverage is specified, computes also the RMS fluctuation of each selected field." << endl;
  cerr << endl;
  cerr << "Output formatting options: define the format of output data" << endl;
  cerr << " (useful combination is \"asciiout fortranblocs=no splitfiles=all\" to get raw ascii data in separate files)" << endl;
  cerr << " binout|asciiout              : default is binary (applies to dx and lata formats)" << endl;
  //cerr << " bigendianout|littleendianout : default is the native format for the current computer architecture" << endl;
  //cerr << " int32out|int64out            : default is sizeof(int) for the current computer/compilation" << endl;
  //cerr << " real32out|real64out          : default is sizeof(float) for the current computer/compilation" << endl;
  cerr << " fortranblocs=yes|no          : default is yes (tells if datafiles contain fortran bloc markers)" << endl;
  cerr << " fortranindex=yes|no          : default is yes (tells if integer arrays contain fortran indexes (first item number 1)" << endl;
  cerr << " splitfiles=all|none          : tells if data should be written in one file per field and per timestep" << endl;
  cerr << endl;
  cerr << "verbosity=n : The greater n, the more debugging info you get." << endl;
  cerr << endl;
  LataOptions::describe();
}

entier Lata2dxOptions::parse_option(const Nom & s)
{
  if (s == "binout") {
    binary_out = 1;
  } else if (s == "asciiout") {
    binary_out = 0;
  } else if (s == "splitfiles=none") {
    lata_file_splitting = SPLIT_NONE;
  } else if (s == "splitfiles=all") {
    lata_file_splitting = SPLIT_ALL;
  } else if (s == "fortranblocs=no") {
    fortran_blocs = 0;
  } else if (s == "fortranblocs=yes") {
    fortran_blocs = 1;
  } else if (s == "fortranindex=yes") {
    use_fortran_indexing = 1;
  } else if (s == "fortranindex=no") {
    use_fortran_indexing = 0;
  } else if (s == "forcegroup") {
    forcegroup = 1;
  } else if (s == "dumplist") {
    dump_list = 1;
  } else if (s == "compute_virtual_elements") {
    compute_virtual_elements = 1;
  } else if (s.debute_par("subdomain=")) {
    load_subdomain = read_int_opt(s);
  } else if (s.debute_par("writelata=")) {
    processing_option = WRITE_LATA_ALL;
    output_filename = read_string_opt(s);
  } else if (s.debute_par("writelata_convert=")) {
    processing_option = WRITE_LATA_CONVERT;
    output_filename = read_string_opt(s);
  } else if (s.debute_par("writelata_master=")) {
    processing_option = WRITE_LATA_MASTER;
    output_filename = read_string_opt(s);
  } else if (s == "writeprm") {
    processing_option = WRITE_PRM;
    output_filename = "??";
  } else if (s == "writedx") {
    processing_option = WRITE_DX;
    output_filename = "??";
  } else if (s.debute_par("timestep=")) {
    // Internally, first timestep is 1.
    input_timesteps_filter.set_smart_resize(1);
    input_timesteps_filter.append_array(read_int_opt(s)+1);    
  } else if (s.debute_par("domain=")) {
    input_domains_filter.add(read_string_opt(s));
  } else if (s.debute_par("component=")) {
    input_components_filter.add(read_string_opt(s));
  } else if (s.debute_par("export_domain=")) {
    output_domains_filter.add(read_string_opt(s));
  } else if (s.debute_par("export_component=")) {
    output_components_filter.add(read_string_opt(s));
  } else if (s.debute_par("mergelata=")) {
    merge_files.add(read_string_opt(s));
  } else if (s == "timeaverage=simple") {
    time_average_ = SIMPLE_TIME_AVERAGE;
  } else if (s == "timeaverage=linear") {
    time_average_ = LINEAR_TIME_AVERAGE;
  } else if (s == "timeaverage=rectangles") {
    time_average_ = RECTANGLES_TIME_AVERAGE;
  } else if (s == "rms_fluctuations") {
    rms_fluctuations = 1;
  } else
    return LataOptions::parse_option(s);
  return 1;
}


void Lata2dxOptions::parse_options(int argc, char **argv) 
{
  if (argc < 2) {
    describe();
    exit(-1);
  }
  extract_path_basename(argv[1], path_prefix, basename);

  Journal(3) << "Basename : " << basename << endl
             << "Prefix   : " << path_prefix << endl;

  for (int i = 2; i < argc; i++) {
    if (!parse_option(argv[i])) {
      cerr << "Lata2dx : unknown command line option: " << argv[i] << endl;
      describe();
      exit(-1);
    }
  }
}

void write_prmfile(const char *source_file, LataDB & lata_db)
{
  Journal() << "Writing a .prm file with all fields to stdout " << endl;
  const entier last_tstep = lata_db.nb_timesteps() - 1;
  // Get all meshes:
  Noms meshes = lata_db.geometry_names(last_tstep, LataDB::FIRST_AND_CURRENT);
  entier i;
  for (i = 0; i < meshes.size(); i++) {
    Journal() << "plotting mesh " << meshes[i] << endl;
    cout << "mesh " << source_file << " "<<meshes[i] << endl;
  }
  // Get all fields:
  Field_UNames fields = lata_db.field_unames(last_tstep, "*", "*", LataDB::FIRST_AND_CURRENT);
  // Plot all floating point fields with known localisation:
  for (i = 0; i < fields.size(); i++) {
    const LataDBField & field = lata_db.get_field(last_tstep, fields[i], LataDB::FIRST_AND_CURRENT);
    Nom geom(field.geometry_);
    Nom loc(field.localisation_);
    entier is_vector = 0;
    if (field.nature_ == LataDBField::VECTOR)
      is_vector = 1;
    if (field.localisation_ == "FACES") {
      // fields at faces must be plot on the dual mesh:
      geom += "_dual";
      loc = "FACES";
      const LataDBGeometry & latageom = lata_db.get_geometry(last_tstep, field.geometry_, LataDB::FIRST_AND_CURRENT);
      // in VDF, fields at faces are always vectors...
      if (latageom.elem_type_ == "QUADRANGLE" || latageom.elem_type_ == "RECTANGLE")
        is_vector = 1;
    }
    entier nb_comp = field.nb_comp_;
    if (is_vector)
      nb_comp = lata_db.get_field(last_tstep, field.geometry_, "SOMMETS", "*", LataDB::FIRST_AND_CURRENT).nb_comp_;

    if ((field.localisation_ == "SOM" 
        || field.localisation_ == "ELEM" 
        || field.localisation_ == "FACES")
        && (field.datatype_.type_ == LataDBDataType::REAL32
            || field.datatype_.type_ == LataDBDataType::REAL64)) {
      Journal() << "plotting field " << field.uname_.build_string() << endl;
      // Plot each component
      for (entier j = 0; j < nb_comp; j++) {
        // The name built here must be identical to the name built in the visit/lata plugin
        Nom name(field.name_);
        if (is_vector) {
          if (j == 0)
            name += "_X";
          else if (j == 1)
            name += "_Y";
          else 
            name += "_Z";
        } else if (field.component_names_.size() == nb_comp && nb_comp > 1) {
          // Component names given ?
          name += "_";
          name += field.component_names_[j];
          // Special case: scalar field with component name set. What to do ?
        } else if (nb_comp > 1) {
          // No component names given
          name += "_";
          name += Nom(j);
        }
        cout << "scalar " << source_file << " " 
           << geom << " "
           << name << " "
           << loc   << endl;
      }
      // For vector fields, plot a vector
      if (is_vector) {
        cout << "vector " << source_file  << " "
           << geom << " "
           << field.name_ << " "
           << loc << endl;
      }
    } else {
      Journal() << "Field " << field.uname_.build_string() << " not plotted" << endl;
    }
  }
}

// Rewrite the lata master file in latav2 format (keeping other files untouched)
// Compute the virtual elements if requested and add it to the database
static void write_lata_master(const LataDB & lata_db, const Lata2dxOptions & opt)
{
  Journal(1) << "Rewrite a master lata file (no data conversion, no data processing): " 
             << opt.output_filename << endl
             << " Warning : the new master lata file references the original lata data files." << endl
             << "           Do not erase them !!!" << endl;
  LataDB dest_db(lata_db);
  if (opt.compute_virtual_elements) {
    // Loop on all timesteps and geometries:
    for (entier t = 0; t < lata_db.nb_timesteps(); t++) {
      Noms geoms = lata_db.geometry_names(t);
      for (entier i = 0; i < geoms.size(); i++) 
        rebuild_virtual_layer(dest_db, Domain_Id(geoms[i], t), opt.reconnect_tolerance);
    }
  }
  dest_db.write_master_file(opt.output_filename);
}

static void write_lata_convert(const LataDB & lata_db, const Lata2dxOptions & opt)
{
  Journal(1) << "Rewrite lata master file and all data files with data format conversion "
             << "(no data processing): " <<opt.output_filename << endl
             << "Writing new data files..." << endl;
  LataDB dest_db(lata_db);

  Nom new_prefix, new_basename;
  LataOptions::extract_path_basename(opt.output_filename, new_prefix, new_basename);
  dest_db.set_path_prefix(new_prefix);
  dest_db.change_all_data_filenames(opt.basename, new_basename);
  dest_db.check_all_data_fileoffsets(opt.lata_file_splitting == Lata2dxOptions::SPLIT_ALL);
  if (opt.binary_out) {
    Journal(3) << " Setting datatypes to binary" << endl;
    LataDBDataType t1, t2;
    t2.msb_ = LataDBDataType::machine_msb_;
    dest_db.change_all_data_types(t1, t2);
  } else {
    Journal(3) << " Setting datatypes to ascii" << endl;
    LataDBDataType t1, t2;
    t2.msb_ = LataDBDataType::ASCII;
    dest_db.change_all_data_types(t1, t2);            
  }
  if (opt.fortran_blocs) {
    Journal(3) << " Setting datatypes to fortran_blocs=yes" << endl;
    LataDBDataType t1, t2;
    t2.fortran_bloc_markers_ = LataDBDataType::BLOC_MARKERS_SINGLE_WRITE;
    dest_db.change_all_data_types(t1, t2);
  } else {
    Journal(3) << " Setting datatypes to fortran_blocs=no" << endl;
    LataDBDataType t1, t2;
    t2.fortran_bloc_markers_ = LataDBDataType::NO_BLOC_MARKER;
    dest_db.change_all_data_types(t1, t2);            
  }
          
  // Loop on timesteps and fields and write everything
  IntTab tmp_int;
  FloatTab tmp_float;
  DoubleTab tmp_double;
  for (entier i = 0; i < lata_db.nb_timesteps(); i++) {
    Field_UNames fields = lata_db.field_unames(i, "*", "*");
    for (entier k = 0; k < fields.size(); k++) {
      const LataDBField & fld = lata_db.get_field(i, fields[k]);
      switch(fld.datatype_.type_) {
      case LataDBDataType::INT32:
      case LataDBDataType::INT64:
        lata_db.read_data(fld, tmp_int);
        dest_db.write_data(i, fields[k], tmp_int);
        break;
      case LataDBDataType::REAL32:
        lata_db.read_data(fld, tmp_float);
        dest_db.write_data(i, fields[k], tmp_float);
        break;
      case LataDBDataType::REAL64:
        lata_db.read_data(fld, tmp_int);
        dest_db.write_data(i, fields[k], tmp_double);
        break;
      default:
        Journal() << "Error in lata2dx: type not implemented !" << endl;
        throw;
      }
    }
  }
  dest_db.write_master_file(opt.output_filename);
}

static void write_dx(LataFilter & filter, const Lata2dxOptions & opt)
{
  if (opt.input_timesteps_filter.size_array() != 1) {
    Journal() << " Error: to export dx data, exactly one timestep must be selected" << endl;
    exit(-1);
  }
  Journal(1) << "Write a DX data file" << endl;
  OpenDXWriter dx_writer;
  // The data base has been filtered, there is exactely 1 timestep left
  const entier lasttstep = filter.get_nb_timesteps() - 1;
  const double t = filter.get_timestep(lasttstep);
  dx_writer.init_cout(t, !opt.binary_out);
  Noms geoms = filter.get_exportable_geometry_names();
  for (entier i = 0; i < geoms.size(); i++) {
    if (opt.output_domains_filter.size() > 0 && opt.output_domains_filter.rang(geoms[i]) < 0)
      continue; // Ignore this geometry

    Domain_Id id(geoms[i], lasttstep, opt.load_subdomain);
    const Domain & dom = filter.get_geometry(id);
    dx_writer.write_geometry(dom);
    
    Field_UNames fields = filter.get_exportable_field_unames(geoms[i]);
    for (entier j = 0; j < fields.size(); j++) {
      const Nom & fieldname = fields[j].get_field_name();
      const Nom & complete_fieldname = fields[j].build_string();
      if (opt.output_components_filter.size() > 0 
          && opt.output_components_filter.rang(fieldname) < 0
          && opt.output_components_filter.rang(complete_fieldname) < 0)
        continue; // Ignore this component
      
      Journal(3) << "Exporting field " << fieldname << endl;
      Field_Id id(fields[j], lasttstep, opt.load_subdomain);
      const LataField_base & field = filter.get_field(id);
      dx_writer.write_component(field);
      filter.release_field(field);
    }
    filter.release_geometry(dom);
  }
  dx_writer.finish(opt.forcegroup);
}

static void write_med(LataFilter & filter, const Lata2dxOptions & opt)
{
#ifdef WITH_MED
        MEDWriter writer;
        writer.init_file(opt.medfilename);
 Noms geoms = filter.get_exportable_geometry_names();        
        for (entier i = 0; i < geoms.size(); i++) {
          if (opt.domains_filter.size() > 0 && opt.domains_filter.rang(geoms[i]) < 0)
            continue;
          Domain_Id id(geoms[i], timestep, opt.load_subdomain);
          const Domain & dom = filter.get_geometry(id);
          writer.write_geometry(dom);
          
          Noms fields = filter.get_exportable_field_names(geoms[i]);
          for (entier j = 0; j < fields.size(); j++) {
            if (opt.components_filter.size() > 0 && opt.components_filter.rang(fields[j]) < 0)
              continue;
            Journal(3) << "Exporting field " << fields[j] << endl;
            Field_Id id(dom.id_, timestep, fields[j]);
            const LataField_base & field = filter.get_field(id);
            writer.write_component(dom, field, filter.get_timestep(timestep));
            filter.release_field(field);
          }
          filter.release_geometry(dom);
        }
#else
        Journal() << "Error: lata2dx compiled without med" << endl;
#endif
}

static void write_lata_all(LataFilter & filter, const Lata2dxOptions & opt)
{
  Journal(2) << "Writing fields from LataFilter to lata file" << endl;

  Nom new_prefix, new_basename;
  LataOptions::extract_path_basename(opt.output_filename, new_prefix, new_basename);

  LataDBDataType default_int;
  if (opt.binary_out)
    default_int.msb_ = LataDBDataType::machine_msb_;
  else
    default_int.msb_ = LataDBDataType::ASCII;
  default_int.type_ = LataDBDataType::INT32;
  if (opt.use_fortran_indexing)
    default_int.array_index_ = LataDBDataType::F_INDEXING;
  else
    default_int.array_index_ = LataDBDataType::C_INDEXING;
  if (opt.use_fortran_data_ordering)
    default_int.data_ordering_ = LataDBDataType::F_ORDERING;
  else
    default_int.data_ordering_ = LataDBDataType::C_ORDERING;
  if (opt.fortran_blocs)
    default_int.fortran_bloc_markers_ = LataDBDataType::BLOC_MARKERS_SINGLE_WRITE;
  else
    default_int.fortran_bloc_markers_ = LataDBDataType::NO_BLOC_MARKER;
  default_int.bloc_marker_type_ = LataDBDataType::INT32;
  default_int.file_offset_ = 0;

  LataWriter lata_writer;
  lata_writer.init_file(new_prefix, new_basename,
                        default_int, LataDBDataType::REAL32);
  
  const entier time_average = (opt.time_average_ != Lata2dxOptions::NO_TIME_AVERAGE);
  const entier ntimesteps = time_average ? 2 : filter.get_nb_timesteps();

  if (time_average && filter.get_nb_timesteps() < 3) {
    Journal() << "Error: database has less than 2 timesteps: cannot compute time average" << endl;
    exit(-1);
  }
  
  // Build list of geometries to export
  Noms geometries;
  {
    const entier take_all_geoms = (opt.output_domains_filter.size() == 0);
    Noms names = filter.get_exportable_geometry_names();
    for (entier i = 0; i < names.size(); i++)
      if (take_all_geoms || opt.output_domains_filter.rang(names[i]) >= 0) {
        if (time_average && filter.get_geometry_metadata(names[i]).dynamic_) {
          Journal(2) << " Geometry rejected (cannot compute time average on dynamic mesh): " << names[i] << endl;
        } else {
          geometries.add(names[i]);
          Journal(2) << " Geometry kept: " << names[i] << endl;
        }
      } else {
        Journal(2) << " Geometry rejected (output selection): " << names[i] << endl;
      }
  }
  
  for (entier tstep = 0; tstep < ntimesteps; tstep++)
    {
      if (tstep > 0) {
        // Real timestep (timestep 0 contains global definitions)
        double t = filter.get_timestep(tstep);
        Journal(2) << " Writing timestep to lata: " << t << endl;
        lata_writer.write_time(t);
      }
      entier i;
      for (i = 0; i < geometries.size(); i++) 
        {
          const LataGeometryMetaData & md = filter.get_geometry_metadata(geometries[i]);
          if ((md.dynamic_ && tstep > 0) || ((!md.dynamic_) && tstep == 0)) 
            {
              // Output geometry data defined at this timestep:
              const Domain & domain = filter.get_geometry(Domain_Id(md.internal_name_, tstep, -1));
              Journal(2) << " Writing geometry to lata: " << md.internal_name_ << endl;
              lata_writer.write_geometry(domain);
              filter.release_geometry(domain);
            }
          if (tstep > 0) {
            // Output fields
            LataVector<Field_UName> field_names = filter.get_exportable_field_unames(md.internal_name_);
            const entier n = field_names.size();
            for (entier j = 0; j < n; j++) {
              const Nom & fieldname = field_names[j].get_field_name();
              const Nom & complete_field_name = field_names[j].build_string();
              if (opt.output_components_filter.size() > 0 
                  && opt.output_components_filter.rang(fieldname) < 0
                  && opt.output_components_filter.rang(complete_field_name) < 0)
                continue; // Ignore this component
              
              if (time_average) 
                {
                  // Compute time average of all timesteps (if type float)
                  const LataField_base & field = filter.get_field(Field_Id(field_names[j], tstep, -1));
                  const Field<FloatTab> * fld_ptr = dynamic_cast<const Field<FloatTab> *> (&field);
                  if (fld_ptr) {
                    Journal(2) << "Computing time average of field " << field_names[j].build_string() << endl;
                    // take a copy:
                    Field<FloatTab> fld(*fld_ptr);
                    filter.release_field(field);

                    // Compute in double precision:
                    ArrOfDouble data_array(fld.data_.size_array());
                    // Integral of the square (for rms fluctuations)
                    ArrOfDouble square_data_array;
                    if (opt.rms_fluctuations)
                      square_data_array.resize_array(fld.data_.size_array());

                    const entier nt = filter.get_nb_timesteps();
                    double f;
                    double total_time = 0.;
                    switch(opt.time_average_) {
                    case Lata2dxOptions::SIMPLE_TIME_AVERAGE:  
                      f = 1. / (nt-1);
                      Journal(1) << "Simple time average: weight = 1 / N  with N = " << nt - 1 << endl;
                      break;
                    case Lata2dxOptions::LINEAR_TIME_AVERAGE:
                      total_time = filter.get_timestep(nt-1) - filter.get_timestep(1);
                      if (total_time <= 0.) {
                        Journal() << "Error computing time average: time between first and last timestep is not positive" << endl;
                        exit(-1);
                      }
                      f = 0.5 * (filter.get_timestep(2) - filter.get_timestep(1)) / total_time; 
                      Journal(1) << "Piecewise linear time integral using N = " << nt - 1 << " timesteps" << endl;
                      break;
                    case Lata2dxOptions::RECTANGLES_TIME_AVERAGE:
                      total_time = filter.get_timestep(nt-1) - filter.get_timestep(1);
                      // First timestep has zero weight.
                      f = 0.;
                      Journal(1) << "Piecewise constant time integral using N = " << nt - 2 << " timesteps" << endl;
                      break;
                    default:
                      Journal() << "Internal error: time average option not implemented" << endl;
                      exit(-1);
                    }
                      
                    const entier sz = data_array.size_array();
                    if (opt.time_average_ != Lata2dxOptions::RECTANGLES_TIME_AVERAGE)
                      Journal(3) << "Adding timestep 1, factor " << f << endl;
                    else
                      Journal(3) << "Timestep 1 not used for time integration (only time position)" << endl;

                    entier k; 
                    {
                      const ArrOfFloat & fld_data = fld.data_;
                      for (k = 0; k < sz; k++) {
                        double x = fld_data[k];
                        data_array[k] = x * f;
                        if (opt.rms_fluctuations)
                          square_data_array[k] = x * x * f; 
                      }
                    }

                    // Loop on other timesteps
                    for (entier tstep2 = 2; tstep2 < nt; tstep2++) {
                      const LataField_base & field = filter.get_field(Field_Id(field_names[j], tstep2, -1));
                      const ArrOfFloat & arr2 = (*dynamic_cast<const Field<FloatTab> *> (&field)).data_;
                      switch(opt.time_average_) {
                      case Lata2dxOptions::SIMPLE_TIME_AVERAGE:
                        break; // f unchanged
                      case Lata2dxOptions::LINEAR_TIME_AVERAGE:
                        {
                          // Compute coefficient for this timestep for piecewise linear interpolation
                          const entier x = (tstep2 < nt - 1) ? (tstep2 + 1) : tstep2;
                          f = 0.5 * (filter.get_timestep(x) - filter.get_timestep(tstep2-1)) / total_time;
                          break;
                        }
                      case Lata2dxOptions::RECTANGLES_TIME_AVERAGE:
                        f = (filter.get_timestep(tstep2) -  filter.get_timestep(tstep2-1)) / total_time;
                        break;
                      default:
                        Journal() << "Internal error: time average option not implemented" << endl;
                        exit(-1);
                      }
                      Journal(3) << "Adding timestep " << tstep2 << ", factor " << f << endl;

                      for (k = 0; k < sz; k++)
                        data_array[k] += arr2[k] * f;

                      if (opt.rms_fluctuations) {
                        for (k = 0; k < sz; k++) {
                          double x = arr2[k];
                          square_data_array[k] += x * x * f;
                        }
                      }
                      filter.release_field(field);
                    }
                    
                    // Write field
                    ArrOfFloat & fld_data = fld.data_;

                    for (k = 0; k < sz; k++)
                      fld_data[k] = data_array[k];

                    lata_writer.write_component(fld);

                    if (opt.rms_fluctuations) {
                      // Compute fluctuation:
                      for (k = 0; k < sz; k++) {
                        double x = data_array[k];
                        double y = square_data_array[k];
                        double fluct = y - x * x;
                        if (fluct < 0.)
                          fluct = 0.; // just in case...
                        fluct = sqrt(fluct);
                        // cast to float
                        fld_data[k] = fluct;
                      }
                      // change name:
                      Nom new_name = "rms_fluct_";
                      new_name += fld.id_.uname_.get_field_name();
                      fld.id_.uname_.set_field_name(new_name);
                      lata_writer.write_component(fld);
                    }
                  } else {
                    filter.release_field(field);
                    Journal(2) << "Field " << field_names[j].build_string() << " is not float, cannot compute time average" << endl;
                  }
                } 
              else 
                {
                  const LataField_base & field = filter.get_field(Field_Id(field_names[j], tstep, -1));
                  Journal(2) << " Writing field to lata: " << field_names[j].build_string() << endl;
                  lata_writer.write_component(field);
                  filter.release_field(field);
                }
            }
          }
        }
    }
  lata_writer.finish();
}

template <class TAB>
static void merge_tab(const LataVector<LataDB> & lata_db, LataDB & dest_db, const LataDBField & dest_fld)
{
  TAB tmp, tab;
  const entier n = lata_db.size();
  entier offset = 0;
  for (entier i = 0; i < n; i++) {
    const LataDBField & fld = lata_db[i].get_field(dest_fld.timestep_, dest_fld.uname_);
    lata_db[i].read_data(fld, tmp);
    if (offset)
      tmp += offset;
    const entier sz = tab.dimension(0);
    if (i > 0 && tmp.dimension(1) != tab.dimension(1)) {
      cerr << "Error while merging arrays: dimensions mismatch for field " << dest_fld.uname_.build_string() << endl;
      exit(-1);
    }
    const entier old_sz = tab.size_array();
    tab.resize(sz + tmp.dimension(0), tmp.dimension(1));
    tab.inject_array(tmp, tmp.size_array(), old_sz, 0);
    if (dest_fld.name_ == "ELEMENTS" || dest_fld.name_ == "FACES")
      offset += lata_db[i].get_field(dest_fld.timestep_, dest_fld.geometry_, "SOMMETS", "*", LataDB::FIRST_AND_CURRENT).size_;
    else if (dest_fld.name_ == "ELEM_FACES")
      offset += lata_db[i].get_field(dest_fld.timestep_, dest_fld.geometry_, "FACES", "*", LataDB::FIRST_AND_CURRENT).size_;
  }
  LataDBField newfield(dest_fld);
  newfield.size_ = tab.dimension(0);
  dest_db.add_field(newfield);
  dest_db.write_data(newfield.timestep_, newfield.uname_, tab);
}

static void merge_lata_geometries(const Lata2dxOptions & opt)
{
  const Noms & filenames = opt.merge_files;
  if (filenames.size() < 1) {
    cerr << "Error in merge_lata_geometries: must have at least one lata file" << endl;
    exit(-1);
  }
  const entier nb_db = filenames.size();
  LataVector<LataDB> lata_db(nb_db);

  for (entier i = 0; i < nb_db; i++) {
    LataDB input;
    Nom a, b;
    LataOptions::extract_path_basename(filenames[i], a, b);
    b += ".lata";
    input.read_master_file(a, b);
    LataDB_apply_input_filter(input, lata_db[i], opt.input_timesteps_filter,
                       opt.input_domains_filter,
                       opt.input_components_filter);
  }

  LataDB dest_db1(lata_db[0]);
  Nom new_prefix, new_basename;
  LataOptions::extract_path_basename(opt.output_filename, new_prefix, new_basename);
  dest_db1.set_path_prefix(new_prefix);
  dest_db1.change_all_data_filenames(opt.basename, new_basename);
  dest_db1.check_all_data_fileoffsets(opt.lata_file_splitting == Lata2dxOptions::SPLIT_ALL);

  // La difference entre dest_db1 et dest_db est la taille des tableaux
  //  on construit dest_db en recopiant les entrees de dest_db et en modifiant cette taille.
  LataDB dest_db;
  dest_db.set_path_prefix(new_prefix);
  dest_db.header_             = dest_db1.header_;
  dest_db.case_               = dest_db1.case_;
  dest_db.software_id_        = dest_db1.software_id_;
  dest_db.default_type_int_   = dest_db1.default_type_int_;
  dest_db.default_float_type_ = dest_db1.default_float_type_;

  for (entier i = 0; i < dest_db1.nb_timesteps(); i++) {
    dest_db.add_timestep(dest_db1.get_time(i));

    // Copy geometries from dest_db1 to dest_db
    Noms geoms = dest_db1.geometry_names(i);
    for (entier j = 0; j < geoms.size(); j++)
      dest_db.add_geometry(dest_db1.get_geometry(i, geoms[j]));

    // Copy and merge fields 
    Field_UNames fields = dest_db1.field_unames(i, "*", "*");
    for (entier k = 0; k < fields.size(); k++) {
      // Make a copy of the field
      LataDBField fld = dest_db1.get_field(i, fields[k]);
      switch(fld.datatype_.type_) {
      case LataDBDataType::INT32:
      case LataDBDataType::INT64:
        merge_tab<IntTab>(lata_db, dest_db, fld);
        break;
      case LataDBDataType::REAL32:
        merge_tab<FloatTab>(lata_db, dest_db, fld);
        break;
      case LataDBDataType::REAL64:
        merge_tab<DoubleTab>(lata_db, dest_db, fld);
        break;
      default:
        Journal() << "Error in lata2dx: type not implemented !" << endl;
        throw;
      }
    }
  }
  dest_db.write_master_file(opt.output_filename);
}

int main(int argc,char **argv) 
{
  Lata2dxOptions opt;
  Journal(8) << "command line ";
  for (int i=0; i<argc; i++)
    Journal(8) << argv[i] << " ";
  Journal(8) << endl;

  opt.parse_options(argc, argv);
  read_any_format_options(argv[1], opt);
  
  {
    Motcle mot(argv[1]);
    if (mot.finit_par(".lml")) {
      if (opt.processing_option != Lata2dxOptions::WRITE_DX) {
        if (opt.processing_option != Lata2dxOptions::WRITE_LATA_CONVERT) {
          Journal(0) << "Input file " << argv[1] << "is lml format: " << endl;
          Journal(0) << " lml can only be processed with writelata_convert." << endl;
          exit(-1);
        }
        Journal(0) << "Input file " << argv[1] << " converted to lata format: " << opt.output_filename << endl;
        Journal(0) << " (note: single lata data file, no database filtering)" << endl;
        lml_to_lata(argv[1], opt.output_filename, !opt.binary_out, opt.fortran_blocs, opt.use_fortran_data_ordering, opt.use_fortran_indexing);
        exit(0);
      }
    }
  }
  if (opt.merge_files.size() > 0) {
    merge_lata_geometries(opt);
    exit(0);
  }

  // Reading input file into this
  LataDB lata_db_input;
  read_any_format(argv[1], opt.path_prefix, lata_db_input);
    
  // Display available data in input database:
  if (opt.dump_list) {
    Noms list_all_domains = lata_db_input.geometry_names(lata_db_input.nb_timesteps()-1, LataDB::FIRST_AND_CURRENT);
    Noms list_all_fields;
    Noms list_all_unames;
    {
      Field_UNames fields = lata_db_input.field_unames(lata_db_input.nb_timesteps()-1, "*", "*", LataDB::FIRST_AND_CURRENT);
      for (entier i = 0; i < fields.size(); i++) {
        const Nom & n = fields[i].get_field_name();
        if (list_all_fields.rang(n) < 0) {
          list_all_fields.add(n);
          list_all_unames.add(fields[i].build_string());
        }
      }
    }
    Journal() << "Input database dump:" << endl;
    Journal() << " List of available timesteps:" << endl;
    entier i;
    for (i = 1; i < lata_db_input.nb_timesteps(); i++)
      Journal() << "  " << i-1 << "  " << lata_db_input.get_time(i) << endl;
    Journal() << " List of available geometries:" << endl;
    for (i = 0; i < list_all_domains.size(); i++)
      Journal() << "  " << list_all_domains[i] << endl;
    Journal() << "List of available fields in source database:" << endl;
    for (i = 0; i < list_all_fields.size(); i++)
      Journal() << "  " << list_all_fields[i] << "   (" << list_all_unames[i] << ")" << endl;
  }

  // Apply input filtering:
  LataDB lata_db;
  LataDB_apply_input_filter(lata_db_input, lata_db, 
                     opt.input_timesteps_filter,
                     opt.input_domains_filter,
                     opt.input_components_filter);

  LataFilter filter;

  if (opt.time_average_ != Lata2dxOptions::NO_TIME_AVERAGE 
      && opt.processing_option != Lata2dxOptions::WRITE_LATA_ALL) {
    Journal() << "Error: timeaverage option specified without the writelata=FILENAME option" << endl;
    exit(-1);
  }

  if (opt.processing_option != Lata2dxOptions::WRITE_PRM 
      && opt.processing_option != Lata2dxOptions::WRITE_LATA_MASTER
      && opt.processing_option != Lata2dxOptions::WRITE_LATA_CONVERT) {
    filter.initialize(opt, lata_db);
    if (opt.dump_list) {
      // Dump available output geometries and fields
      Noms geoms = filter.get_exportable_geometry_names();
      Journal() << "Available output data:" << endl;
      Journal() << " List of available geometries and fields:" << endl;
      for (entier i = 0; i < geoms.size(); i++) {
        Journal() << "  Geometry " << geoms[i] << endl;
        Field_UNames fields = filter.get_exportable_field_unames(geoms[i]);
        for (entier i = 0; i < fields.size(); i++)
          Journal() << "   Field     " << fields[i].get_field_name() 
                    << "     (" << fields[i].build_string() << ")" << endl;
      }
    }
  }

  if (opt.dump_list) {
    exit(0); // no further processing
  }

  switch(opt.processing_option) {
  case Lata2dxOptions::WRITE_PRM:
    write_prmfile(argv[1], lata_db);
    break;
  case Lata2dxOptions::WRITE_LATA_MASTER:
    write_lata_master(lata_db, opt);
    break;
  case Lata2dxOptions::WRITE_LATA_CONVERT:
    write_lata_convert(lata_db, opt);
    break;
  case Lata2dxOptions::WRITE_LATA_ALL:
    write_lata_all(filter, opt);
    break;
  case Lata2dxOptions::WRITE_DX:
    write_dx(filter, opt);
    break;
  case Lata2dxOptions::WRITE_MED:
    write_med(filter, opt);
    break;
  default:
    exit(-1);
  }
  return 0;
}

