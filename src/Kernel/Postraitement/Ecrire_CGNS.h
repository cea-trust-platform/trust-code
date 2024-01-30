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

#ifndef Ecrire_CGNS_included
#define Ecrire_CGNS_included

#include <TRUST_2_CGNS.h>
#include <map>

class Domaine;
class Nom;

class Ecrire_CGNS
{
#ifdef HAS_CGNS
public:
  void cgns_init_MPI();
  void cgns_set_postraiter_domain() { postraiter_domaine_ = true; }
  void cgns_set_base_name(const Nom& );
  void cgns_open_file();
  void cgns_close_file();
  void cgns_add_time(const double );
  void cgns_write_domaine(const Domaine * ,const Nom& , const DoubleTab& , const IntTab& , const Motcle& );
  void cgns_write_field(const Domaine&, const Noms&, double, const Nom&, const Nom&, const Nom&, const DoubleTab&);

private:
  // Attributes
  bool solname_elem_written_ = false, solname_som_written_ = false;
  bool postraiter_domaine_ = false, grid_file_opened_ = false;
  std::string solname_elem_ = "", solname_som_ = "", baseFile_name_ = "", baseZone_name_ = "";
  std::map<std::string, Nom> fld_loc_map_; /* { Loc , Nom_dom } */
  std::vector<Nom> doms_written_;
  std::vector<std::string> connectname_;
  std::vector<double> time_post_;
  std::vector<int> baseId_, zoneId_, sizeId_;
  std::vector<std::vector<int>> zoneId_par_; /* par ordre d'ecriture du domaine */
  std::vector<TRUST_2_CGNS> T2CGNS_;
  Ecrire_CGNS_helper cgns_helper_;
  int fileId_ = -123, flowId_elem_ = 0, fieldId_elem_ = 0, flowId_som_ = 0, fieldId_som_ = 0, cellDim_ = -123;
  int fileId2_ = -123; /* cas ou on a 2 fichiers ouvert en meme temps : utiliser seulement pour Option_CGNS::USE_LINKS */

  void cgns_fill_field_loc_map(const Domaine&, const std::string&);

  // Methodes pour LINK
  void cgns_fill_info_grid_link_file(const char*, const CGNS_TYPE&, const int, const int, const int, const bool);
  void cgns_open_close_link_files(const double);
  void cgns_open_grid_base_link_file();
  void cgns_open_solution_link_file(const int, const std::string&, const double, bool is_link = false);
  void cgns_write_final_link_file();
  void cgns_close_grid_solution_link_file(const int, const std::string&, bool is_cerr = false);

  // Version sequentielle
  void cgns_write_domaine_seq(const Domaine * ,const Nom& , const DoubleTab& , const IntTab& , const Motcle& );
  void cgns_write_field_seq(const int, const double, const Nom&, const Nom&, const Nom&, const Nom&, const DoubleTab&);
  void cgns_write_iters_seq();

  // Version parallele over zone
  void cgns_write_domaine_par_over_zone(const Domaine * ,const Nom& , const DoubleTab& , const IntTab& , const Motcle& );
  void cgns_write_field_par_over_zone(const int, const double, const Nom&, const Nom&, const Nom&, const Nom&, const DoubleTab&);
  void cgns_write_iters_par_over_zone();

  // Version parallele in zone
  void cgns_write_domaine_par_in_zone(const Domaine * ,const Nom& , const DoubleTab& , const IntTab& , const Motcle& );
  void cgns_write_field_par_in_zone(const int, const double, const Nom&, const Nom&, const Nom&, const Nom&, const DoubleTab&);
  void cgns_write_iters_par_in_zone();

#endif /* HAS_CGNS */
};

inline void verify_if_cgns(const char * nom_funct)
{
#ifdef HAS_CGNS
  return;
#else
  Cerr << "Format_Post_CGNS::" <<  nom_funct << " should not be called since TRUST is not compiled with the CGNS library !!! " << finl;
  Process::exit();
#endif /* HAS_CGNS */
}

#endif /* Ecrire_CGNS_included */
