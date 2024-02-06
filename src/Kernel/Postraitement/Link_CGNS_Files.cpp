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

#include <Link_CGNS_Files.h>
#include <Motcle.h>
#include <unistd.h>
#include <cgns++.h>
#include <fstream>
#include <glob.h>
#include <regex>

#ifdef HAS_CGNS
#define CGNS_STR_SIZE 32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#include <pcgnslib.h>
#pragma GCC diagnostic pop
#endif

Implemente_instanciable(Link_CGNS_Files, "Link_CGNS_Files", Interprete);
// XD Link_CGNS_Files interprete Link_CGNS_Files -1 Creates a single CGNS xxxx.cgns file that links to a xxxx.grid.cgns and xxxx.solution.*.cgns files
// XD attr base_name chaine base_name 0 Base name of the gid/solution cgns files.
// XD attr output_name chaine output_name 0 Name of the output cgns file.

Sortie& Link_CGNS_Files::printOn(Sortie& os) const { return Interprete::printOn(os); }
Entree& Link_CGNS_Files::readOn(Entree& is) { return Interprete::readOn(is); }

inline bool compare_strings_as_doubles(const std::string& a, const std::string& b)
{
  return std::stod(a) < std::stod(b);
}

Entree& Link_CGNS_Files::interpreter(Entree& is)
{

#ifdef HAS_CGNS
  if (Process::is_parallel())
    {
      Cerr << "Error in Link_CGNS_Files::interpreter => PLEASE USE THIS INTERPRET ONLY IN SEQUENTIAL, NOT PARALLEL !!!" << finl;
      Process::exit();
    }

  Cerr << "Syntax Link_CGNS_Files base_name_cgns_files output_single_cgns_file" << finl;

  Nom base_name, output_name;
  is >> base_name >> output_name;
  if (!Motcle(output_name).finit_par(".cgns")) output_name += Nom(".cgns");


  /* Step 1 : Check if grid file exists !! */
  Nom base_name_modif(base_name);
  base_name_modif.prefix("_SOM");
  base_name_modif.prefix("_ELEM");

  std::string tmp_file = base_name_modif.getString() + ".grid.cgns";
  std::ifstream file(tmp_file.c_str());

  Cerr << finl;
  if (file.good()) Cerr << "OK : The file " << tmp_file << " is found !!!" << finl;
  else
    {
      Cerr << "Error in Link_CGNS_Files::interpreter : The file " << tmp_file << " is not found !!!" << finl;
      Process::exit();
    }

  /* Step 2 : Check if solution files exist !! Fill fileNames vector at same time */
  tmp_file = base_name.getString() + ".solution.*.cgns";
  std::vector<std::string> fileNames;

#ifndef __CYGWIN__
  glob_t globResult;
  int globStatus = glob(tmp_file.c_str(), GLOB_TILDE, nullptr, &globResult);

  if (globStatus == 0)
    {
      for (size_t i = 0; i < globResult.gl_pathc; ++i)
        fileNames.push_back(globResult.gl_pathv[i]);

      if (fileNames.size() > 0)
        {
          Cerr << finl;
          Cerr << static_cast<int>(fileNames.size()) << " files found with the pattern : " << tmp_file << "." << finl;
          for (const auto& itr : fileNames)
            Cerr << "     - " << itr << finl;
        }
      else
        {
          Cerr << "No files found with the pattern : " << tmp_file << finl;
          Process::exit();
        }

      globfree(&globResult);
    }
  else if (globStatus == GLOB_NOMATCH)
    {
      Cerr << "No matches found for the pattern : " << tmp_file << finl;
      Process::exit();
    }
  else
    {
      Cerr << "Error during globbing in Link_CGNS_Files::interpreter !!! " << finl;
      Process::exit();
    }
#else
  Cerr << "Error in Link_CGNS_Files::interpreter : Not for CYGWIN !!! Call Gauthier for help !!" << finl;
  Process::exit();
#endif

  /* Step 3 : Extract time post and fill time_post vector */
  std::vector<double> time_post_d;
  std::vector<std::string> time_post_str;

  tmp_file = base_name.getString() + "\\.solution\\.([0-9]+\\.[0-9]+)\\.cgns";
  std::regex doublePattern(tmp_file);

  for (const auto &itr : fileNames)
    {
      std::smatch match;
      if (std::regex_search(itr, match, doublePattern))
        {
          time_post_d.push_back(std::stod(match[1].str()));
          time_post_str.push_back(match[1].str());
        }
      else
        {
          Cerr << "No match found in file : " << itr << finl;
          Process::exit();
        }
    }

  /* On sort les vects !! */

  // Sort the strings as doubles
  std::sort(time_post_str.begin(), time_post_str.end(), compare_strings_as_doubles); // XXX faut ca
  std::sort(time_post_d.begin(), time_post_d.end()); // la facile

  Cerr << finl;
  Cerr << static_cast<int>(time_post_str.size()) << " post times are extracted : " << finl;
  for (const auto &itr : time_post_str)
    Cerr << "     - " << itr << finl;

  /* Step 4 : Open grid file and read some information */
  True_int fileId, baseId = 1, zoneId = 1, cell_dim, phys_dim;
  tmp_file = base_name_modif.getString() + ".grid.cgns";
  cgsize_t isize[3][1];
  char basename[33], zonename[33], sectionname[33]; /* Elem or NGON_n*/
  std::vector<std::string> connectname;

  Cerr << finl;
  Cerr << "Reading the grid file " << tmp_file << " ...";

  if (cg_open(tmp_file.c_str(), CG_MODE_READ, &fileId) != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_open !" << finl, cg_error_exit();

  if (cg_base_read(fileId, baseId, basename, &cell_dim, &phys_dim) != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_base_read !" << finl, cg_error_exit();

  if (cg_zone_read(fileId, baseId, zoneId, zonename, isize[0]) != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_zone_read !" << finl, cg_error_exit();

  True_int nsections;
  if (cg_nsections(fileId, baseId, zoneId, &nsections) != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_nsections !" << finl, cg_error_exit();

  CGNS_ENUMT(ElementType_t) itype;
  cgsize_t istart, iend;
  True_int nbndry, iparent_flag;

  for (int index_sect = 1; index_sect <= nsections; index_sect++)
    {
      if (cg_section_read(fileId, baseId, zoneId, index_sect, sectionname, &itype, &istart, &iend, &nbndry, &iparent_flag) != CG_OK)
        Cerr << "Error Link_CGNS_Files::interpreter : cg_section_read !" << finl, cg_error_exit();

      connectname.push_back(std::string(sectionname));
    }

  if (cg_close(fileId) != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_close !" << finl, cg_error_exit();

  Cerr << " OK !!  " << finl;

  /* Step 5 : Now we can link */
  tmp_file = output_name.getString(); // contient deja l'extension cgns ...
  unlink(tmp_file.c_str());

  Cerr << finl;
  Cerr << "Writing the link file " << tmp_file << " ..." << finl;

  if (cg_open(tmp_file.c_str(), CG_MODE_WRITE, &fileId) != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_open !" << finl, cg_error_exit();

  Cerr << finl;
  Cerr << "**** CGNS file " << tmp_file << " opened !" << finl;

  if (cg_base_write(fileId, basename, cell_dim, phys_dim, &baseId) != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_base_write !" << finl, cg_error_exit();

  if (cg_zone_write(fileId, baseId, zonename, isize[0], CGNS_ENUMV(Unstructured), &zoneId) != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_base_write !" << finl, cg_error_exit();

  std::string linkfile = base_name_modif.getString() + ".grid.cgns"; // file name
  std::string linkpath = "/" + std::string(basename) + "/" + std::string(zonename) + "/GridCoordinates/";

  if (cg_goto(fileId, baseId, "Zone_t", 1, "end") != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_goto !" << finl, cg_error_exit();

  if (cg_link_write("GridCoordinates", linkfile.c_str(), linkpath.c_str()) != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_link_write !" << finl, cg_error_exit();

  for (auto &itr : connectname)
    {
      linkpath = "/" + std::string(basename) + "/" + std::string(zonename) + "/" + itr + "/";
      if (cg_link_write(itr.c_str(), linkfile.c_str(), linkpath.c_str()) != CG_OK)
        Cerr << "Error Link_CGNS_Files::interpreter : cg_link_write !" << finl, cg_error_exit();
    }

  // link solutions
  std::string LOC;
  if (base_name.finit_par("ELEM")) LOC = "ELEM";
  else if (base_name.finit_par("SOM")) LOC = "SOM";
  else
    {
      Cerr << "Error in Link_CGNS_Files::interpreter : The base_name provided ( " << base_name << " ) should finish with ELEM/SOM !! " << finl;
      Cerr << "Otherwise we dont know where the solutions are located. Call the 911 !!!" << finl;
      Process::exit();
    }

  std::string solname_all = "";
  for (auto &itr : time_post_str)
    {
      std::string solname = "FlowSolution" + itr + "_" + LOC;

      linkfile = base_name.getString() + ".solution." + itr + ".cgns"; // file name

      linkpath = "/" + std::string(basename) + "/" + std::string(zonename) + "/" + solname + "/";

      if (cg_link_write(solname.c_str(), linkfile.c_str(), linkpath.c_str()) != CG_OK)
        Cerr << "Error Link_CGNS_Files::interpreter : cg_link_write !" << finl, cg_error_exit();

      solname.resize(CGNS_STR_SIZE, ' ');
      solname_all += solname;
    }

  const int nsteps = static_cast<int>(time_post_d.size());

  if (cg_biter_write(fileId, baseId, "TimeIterValues", nsteps) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_link_file : cg_biter_write !" << finl, cg_error_exit();

  if (cg_goto(fileId, baseId, "BaseIterativeData_t", 1, "end") != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_link_file : cg_goto !" << finl, cg_error_exit();

  cgsize_t nuse = static_cast<cgsize_t>(nsteps);
  if (cg_array_write("TimeValues", CGNS_ENUMV(RealDouble), 1, &nuse, time_post_d.data()) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_link_file : cg_array_write !" << finl, cg_error_exit();

  if (cg_ziter_write(fileId, baseId, zoneId, "ZoneIterativeData") != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_link_file : cg_ziter_write !" << finl, cg_error_exit();

  if (cg_goto(fileId, baseId, "Zone_t", zoneId, "ZoneIterativeData_t", 1, "end") != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_link_file : cg_goto !" << finl, cg_error_exit();

  cgsize_t idata[2];
  idata[0] = CGNS_STR_SIZE;
  idata[1] = nsteps;

  if (cg_array_write("FlowSolutionPointers", CGNS_ENUMV(Character), 2, idata, solname_all.c_str()) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_iters_seq : cg_array_write !" << finl, cg_error_exit();

  if (cg_simulation_type_write(fileId, baseId, CGNS_ENUMV(TimeAccurate)) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_link_file : cg_simulation_type_write !" << finl, cg_error_exit();

  if (cg_close(fileId) != CG_OK)
    Cerr << "Error Link_CGNS_Files::interpreter : cg_close !" << finl, cg_error_exit();

  Cerr << finl;
  Cerr << "All OK !" << finl;
  Cerr << finl;
  Cerr << "**** CGNS file " << tmp_file << " closed !" << finl;
  Cerr << finl;

#else
  Cerr << "Link_CGNS_Files::interpreter should not be called since TRUST is not compiled with the CGNS library !!! " << finl;
  Process::exit();
#endif
  return is;
}
