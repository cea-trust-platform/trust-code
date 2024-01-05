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

#ifndef Format_Post_CGNS_included
#define Format_Post_CGNS_included

#include <TRUSTTabs_forward.h>
#include <Format_Post_base.h>
#include <map>

#include <cgns++.h>

/*! @brief classe Format_Post_CGNS
 *
 *  The CFD General Notation System (CGNS) provides a general, portable, and extensible standard for the storage and
 *  retrieval of computational fluid dynamics (CFD) analysis data. It consists of a collection of conventions,
 *  and free and open software implementing those conventions. It is self-descriptive, machine-independent, well-documented,
 *  and administered by an international steering committee.
 *
 *  It is also an American Institute of Aeronautics and Astronautics (AIAA) Recommended Practice.
 *  The system consists of two parts: (1) a standard data model and mapping format for recording the data,
 *  and (2) software that reads, writes, and modifies data in that format. The format is a conceptual entity established by
 *  the documentation; the software is a physical product supplied to enable developers to access and produce data recorded
 *  in that format.
 *
 *  The CGNS system is designed to facilitate the exchange of data between sites and applications, and to help stabilize
 *  the archiving of aerodynamic data. The data are stored in a compact, binary format and are accessible through a complete and
 *  extensible library of functions.
 *  The API (Application Program Interface) is platform independent and can be easily implemented in C, C++, Fortran and
 *  Fortran90 applications.
 *
 *   CGNS is self-describing, allowing an application to interpret the structure and contents of a file without any outside information.
 *   CGNS can make use of either two different low-level data formats:
 *
 *      - an internally developed and supported method called Advanced Data Format (ADF),
 *        based on a common file format system previously in use at McDonnell Douglas,
 *
 *
 *      - HDF5, a widely used hierarchical data format
 *
 *  see : https://cgns.github.io/cgns-modern.github.io/index.html
 *        https://en.wikipedia.org/wiki/CGNS
 *
 */

class Format_Post_CGNS : public Format_Post_base
{
  Declare_instanciable_sans_constructeur(Format_Post_CGNS);
public:
  Format_Post_CGNS();

  void reset() override;
  void set_param(Param& param) override;
  int initialize_by_default(const Nom&) override;
  int initialize(const Nom&, const int, const Nom&) override;

  int completer_post(const Domaine&, const int, const Nature_du_champ&, const int, const Noms&, const Motcle&, const Nom&) override { return 1; }
  int preparer_post(const Nom&, const int, const int, const double) override { return 1; }

  int ecrire_entete(const double, const int, const int) override;
  int ecrire_temps(const double) override;
  int finir(const int) override;

  int ecrire_domaine(const Domaine&, const int) override;
  int ecrire_item_int(const Nom&, const Nom&, const Nom&, const Nom&, const Nom&, const IntVect&, const int) override { return 1; }
  int ecrire_champ(const Domaine&, const Noms&, const Noms&, int, double, const Nom&, const Nom&, const Nom&, const Nom&, const DoubleTab&) override;

private:
  inline bool is_parallel() const { return Process::nproc() > 1; }
  Nom cgns_basename_;

#ifdef HAS_CGNS
  bool solname_elem_written_ = false, solname_som_written_ = false;
  std::string solname_elem_ = "", solname_som_ = "";
  std::map<std::string, Nom> fld_loc_map_; /* { Loc , Nom_dom } */
  std::map<std::string, std::string> solname_map_; /* { Loc , solname_ } */
  std::vector<Nom> doms_written_;
  std::vector<double> time_post_;
  std::vector<int> baseId_, zoneId_;
  std::vector<std::vector<int>> zoneId_par_, global_nb_elem_, proc_non_zero_write_; /* par ordre d'ecriture du domaine */
  int fileId_ = -123, flowId_elem_ = 0, fieldId_elem_ = 0, flowId_som_ = 0, fieldId_som_ = 0;

  int get_index_nom_vector(const std::vector<Nom>&, const Nom&);
  void ecrire_domaine_(const Domaine&, const Nom&);
  void ecrire_domaine_par_(const Domaine&, const Nom&);
  void ecrire_champ_(const int, const double, const Nom&, const Nom&, const Nom&, const Nom&, const DoubleTab&);
  void ecrire_champ_par_(const int, const double, const Nom&, const Nom&, const Nom&, const Nom&, const DoubleTab&);
  void finir_();
  void finir_par_();
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

#endif /* Format_Post_CGNS_included */
