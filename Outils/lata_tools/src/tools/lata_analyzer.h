/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef LataAnalyzer_H_
#define LataAnalyzer_H_

#include <LataFilter.h>

class LataAnalyzerOptions: public LataOptions
{
public:
  LataAnalyzerOptions();
  void describe();
  entier parse_option(const Nom& s);
  void parse_options(int argc, char **argv);

  // Input filter: when reading the input file, timesteps, domains and components not mentionned in these vectors are not read. If empty list, write all items.
  ArrOfInt input_timesteps_filter;
  Noms input_domains_filter, input_components_filter;

  // Output filter: when writing the output file, domains and components not mentionned here are not written. If empty list, write all items.
  Noms output_domains_filter, output_components_filter, merge_files;

  // Which parallel subdomain shall we load (default = -1, load all subdomains)
  int load_subdomain;
  // Output file will be written in binary format
  int binary_out;
  // For OpenDX: include the output Field object in a group
  int forcegroup;

  // Which data processing do we want ?
  enum ProcessingOption { WRITE_LATA_MASTER, WRITE_LATA_ALL, WRITE_LATA_CONVERT, WRITE_MED, WRITE_PRM };
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

#endif /* LataAnalyzer_H_ */
