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

#include <TRUSTTabs_forward.h>
#include <TRUST_2_CGNS.h>
#include <cgns++.h>
#include <map>

class Domaine;
class Nom;

class Ecrire_CGNS
{
#ifdef HAS_CGNS
public:
  void cgns_init_MPI();
  void cgns_open_file(const Nom& );
  void cgns_close_file(const Nom& );

  void cgns_add_time(const double );

  void cgns_write_domaine(const Domaine * ,const Nom& , const DoubleTab& , const IntTab& , const Motcle& );
  void cgns_write_field(const Domaine&, const Noms&, double, const Nom&, const Nom&, const Nom&, const DoubleTab&);

private:
  int get_index_nom_vector(const std::vector<Nom>&, const Nom&);
  Motcle modify_field_name_for_post(const Nom&, const Nom&, const std::string&);

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

  // Attributes
  bool solname_elem_written_ = false, solname_som_written_ = false;
  std::string solname_elem_ = "", solname_som_ = "";
  std::map<std::string, Nom> fld_loc_map_; /* { Loc , Nom_dom } */
  std::map<std::string, std::string> solname_map_; /* { Loc , solname_ } */
  std::vector<Nom> doms_written_;
  std::vector<double> time_post_;
  std::vector<int> baseId_, zoneId_;
  std::vector<std::vector<int>> zoneId_par_, global_nb_elem_, proc_non_zero_write_; /* par ordre d'ecriture du domaine */
  std::vector<TRUST_2_CGNS> T2CGNS_;
  int fileId_ = -123, flowId_elem_ = 0, fieldId_elem_ = 0, flowId_som_ = 0, fieldId_som_ = 0;
#endif
};

inline void verify_if_cgns(const char * nom_funct)
{
#ifdef HAS_CGNS
  return;
#else
  Cerr << "Format_Post_CGNS::" <<  nom_funct << " should not be called since TRUST is not compiled with the CGNS library !!! " << finl;
  Process::exit();
#endif
}

#endif /* Ecrire_CGNS_included */
