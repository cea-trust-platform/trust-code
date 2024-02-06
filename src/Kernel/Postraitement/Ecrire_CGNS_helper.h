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

#ifndef Ecrire_CGNS_helper_included
#define Ecrire_CGNS_helper_included

#include <TRUST_2_CGNS.h>
#include <Option_CGNS.h>
#include <TRUSTTab.h>
#include <cgns++.h>

#ifdef HAS_CGNS

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#include <pcgnslib.h>
#pragma GCC diagnostic pop

#define CGNS_STR_SIZE 32
#define CGNS_DOUBLE_TYPE Option_CGNS::SINGLE_PRECISION>0?CGNS_ENUMV(RealSingle):CGNS_ENUMV(RealDouble)

enum class TYPE_ECRITURE { SEQ , PAR_IN, PAR_OVER };

struct Ecrire_CGNS_helper
{
  template<TYPE_ECRITURE _TYPE_>
  inline void cgns_write_zone_grid_coord(const int, const True_int, const std::vector<True_int>&, const char*, const cgsize_t*, std::vector<True_int>&,
                                         const std::vector<double>&, const std::vector<double>&, const std::vector<double>&, True_int&, True_int&, True_int&);

  template<TYPE_ECRITURE _TYPE_>
  inline std::enable_if_t< _TYPE_ != TYPE_ECRITURE::SEQ, void>
  cgns_write_grid_coord_data(const int, const True_int, const std::vector<True_int>&, const True_int, const True_int, const True_int, const True_int, const cgsize_t, const cgsize_t,
                             const std::vector<double>&, const std::vector<double>&, const std::vector<double>&);

  template<TYPE_ECRITURE _TYPE_>
  inline void cgns_sol_write(const int, const True_int, const True_int, const int, const double, const std::vector<True_int>&, const std::string&,
                             std::string&, std::string&, bool&, bool&, True_int&, True_int&);

  template<TYPE_ECRITURE _TYPE_>
  inline std::enable_if_t< _TYPE_ != TYPE_ECRITURE::SEQ, void>
  cgns_field_write(const int, const True_int, const True_int, const int, const std::vector<True_int>&, const std::string&, const True_int, const True_int, const char*, True_int&, True_int&);

  template<TYPE_ECRITURE _TYPE_>
  inline std::enable_if_t<_TYPE_ == TYPE_ECRITURE::SEQ, void>
  cgns_field_write_data(const True_int, const True_int, const int, const std::vector<True_int>&, const std::string&, const True_int, const True_int, const int,
                        const char * , const DoubleTab& , True_int& , True_int& );

  template<TYPE_ECRITURE _TYPE_>
  inline std::enable_if_t<_TYPE_ != TYPE_ECRITURE::SEQ, void>
  cgns_field_write_data(const True_int, const True_int, const int, const std::vector<True_int>&, const std::string&, const True_int, const True_int, const True_int, const True_int, const int,
                        const cgsize_t, const cgsize_t, const DoubleTab&);

  template<TYPE_ECRITURE _TYPE_>
  inline void cgns_write_iters(const bool, const int, const True_int , const True_int, const int, const std::vector<True_int>&,
                               const std::string&, const std::string&, const std::string&, const std::vector<double>&);
};

template<TYPE_ECRITURE _TYPE_>
inline void Ecrire_CGNS_helper::cgns_write_zone_grid_coord(const int icelldim, const True_int fileId, const std::vector<True_int>& baseId, const char *zonename, const cgsize_t *isize, std::vector<True_int>& zoneId,
                                                           const std::vector<double>& xCoords, const std::vector<double>& yCoords, const std::vector<double>& zCoords,
                                                           True_int& coordsIdx, True_int& coordsIdy, True_int& coordsIdz)
{
  constexpr bool is_SEQ = (_TYPE_ == TYPE_ECRITURE::SEQ);

  if (cg_zone_write(fileId, baseId.back(), zonename, isize, CGNS_ENUMV(Unstructured), &zoneId.back()) != CG_OK)
    Cerr << "Error Ecrire_CGNS_helper::cgns_write_zone_grid_coord : cg_zone_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

  if (is_SEQ)
    {
      if (cg_coord_write(fileId, baseId.back(), zoneId.back(), CGNS_DOUBLE_TYPE, "CoordinateX", xCoords.data(), &coordsIdx) != CG_OK)
        Cerr << "Error Ecrire_CGNS_helper::cgns_write_zone_grid_coord : cg_coord_write - X !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

      if (cg_coord_write(fileId, baseId.back(), zoneId.back(), CGNS_DOUBLE_TYPE, "CoordinateY", yCoords.data(), &coordsIdy) != CG_OK)
        Cerr << "Error Ecrire_CGNS_helper::cgns_write_zone_grid_coord : cg_coord_write - Y !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

      if (icelldim > 2)
        if (cg_coord_write(fileId, baseId.back(), zoneId.back(), CGNS_DOUBLE_TYPE, "CoordinateZ", zCoords.data(), &coordsIdz) != CG_OK)
          Cerr << "Error Ecrire_CGNS_helper::cgns_write_zone_grid_coord : cg_coord_write - Z !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
    }
  else
    {
      True_int gridId = -123;
      if (cg_grid_write(fileId, baseId.back(), zoneId.back(), "GridCoordinates", &gridId) != CG_OK)
        Cerr << "Error Ecrire_CGNS_helper::cgns_write_zone_grid_coord : cg_grid_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

      if (cgp_coord_write(fileId, baseId.back(), zoneId.back(), CGNS_DOUBLE_TYPE, "CoordinateX", &coordsIdx) != CG_OK)
        Cerr << "Error Ecrire_CGNS_helper::cgns_write_zone_grid_coord : cgp_coord_write - X !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

      if (cgp_coord_write(fileId, baseId.back(), zoneId.back(), CGNS_DOUBLE_TYPE, "CoordinateY", &coordsIdy) != CG_OK)
        Cerr << "Error Ecrire_CGNS_helper::cgns_write_zone_grid_coord : cgp_coord_write - Y !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

      if (icelldim > 2)
        if (cgp_coord_write(fileId, baseId.back(), zoneId.back(), CGNS_DOUBLE_TYPE, "CoordinateZ", &coordsIdz) != CG_OK)
          Cerr << "Error Ecrire_CGNS_helper::cgns_write_zone_grid_coord : cgp_coord_write - Z !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
    }
}

template<TYPE_ECRITURE _TYPE_>
inline std::enable_if_t<_TYPE_ != TYPE_ECRITURE::SEQ, void>
Ecrire_CGNS_helper::cgns_write_grid_coord_data(const int icelldim, const True_int fileId, const std::vector<True_int>& baseId, const True_int zoneId,
                                               const True_int coordsIdx, const True_int coordsIdy, const True_int coordsIdz, const cgsize_t min, const cgsize_t max,
                                               const std::vector<double>& xCoords, const std::vector<double>& yCoords, const std::vector<double>& zCoords)
{
  if (cgp_coord_write_data(fileId, baseId.back(), zoneId, coordsIdx, &min, &max, xCoords.data()) != CG_OK)
    Cerr << "Error Ecrire_CGNS_helper::cgns_write_grid_coord_data : cgp_coord_write_data - X !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

  if (cgp_coord_write_data(fileId, baseId.back(), zoneId, coordsIdy, &min, &max, yCoords.data()) != CG_OK)
    Cerr << "Error Ecrire_CGNS_helper::cgns_write_grid_coord_data : cgp_coord_write_data - Y !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

  if (icelldim > 2)
    if (cgp_coord_write_data(fileId, baseId.back(), zoneId, coordsIdz, &min, &max, zCoords.data()) != CG_OK)
      Cerr << "Error Ecrire_CGNS_helper::cgns_write_grid_coord_data : cgp_coord_write_data - Z !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
}

template<TYPE_ECRITURE _TYPE_>
inline void Ecrire_CGNS_helper::cgns_sol_write(const int nb_zones_to_write, const True_int fileId, const True_int baseId, const int ind, const double temps, const std::vector<True_int>& zoneId, const std::string& LOC,
                                               std::string& solname_som, std::string& solname_elem, bool& solname_som_written, bool& solname_elem_written, True_int& flowId_som, True_int& flowId_elem)
{
  // uen fois par dt !!
  constexpr bool is_SEQ = (_TYPE_ == TYPE_ECRITURE::SEQ), is_PAR_OVER = (_TYPE_ == TYPE_ECRITURE::PAR_OVER);

  if (!solname_som_written && LOC == "SOM")
    {
      std::string solname = "FlowSolution" + std::to_string(temps) + "_" + LOC;
      solname.resize(CGNS_STR_SIZE, ' ');
      solname_som += solname;

      // on boucle seulement sur les procs qui n'ont pas des nb_elem 0
      for (int ii = 0; ii != nb_zones_to_write; ii++)
        {
          if (cg_sol_write(fileId, baseId, zoneId[is_PAR_OVER ? ii : ind], solname.c_str(), CGNS_ENUMV(Vertex), &flowId_som) != CG_OK)
            Cerr << "Error Ecrire_CGNS_helper::cgns_sol_write : cg_sol_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

          if (!is_SEQ)
            if (cg_goto(fileId, baseId, "Zone_t", zoneId[is_PAR_OVER ? ii : ind], "FlowSolution_t", flowId_som, "end") != CG_OK)
              Cerr << "Error Ecrire_CGNS_helper::cgns_sol_write : cg_goto !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }

      solname_som_written = true;
    }

  if (!solname_elem_written && LOC == "ELEM")
    {
      std::string solname = "FlowSolution" + std::to_string(temps) + "_" + LOC;
      solname.resize(CGNS_STR_SIZE, ' ');
      solname_elem += solname;

      // on boucle seulement sur les procs qui n'ont pas des nb_elem 0
      for (int ii = 0; ii != nb_zones_to_write; ii++)
        {
          if (cg_sol_write(fileId, baseId, zoneId[is_PAR_OVER ? ii : ind], solname.c_str(), CGNS_ENUMV(CellCenter), &flowId_elem) != CG_OK)
            Cerr << "Error Ecrire_CGNS_helper::cgns_sol_write : cg_sol_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

          if (!is_SEQ)
            if (cg_goto(fileId, baseId, "Zone_t", zoneId[is_PAR_OVER ? ii : ind], "FlowSolution_t", flowId_elem, "end") != CG_OK)
              Cerr << "Error Ecrire_CGNS_helper::cgns_sol_write : cg_goto !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }

      solname_elem_written = true;
    }
}

template<TYPE_ECRITURE _TYPE_>
inline std::enable_if_t<_TYPE_ != TYPE_ECRITURE::SEQ, void>
Ecrire_CGNS_helper::cgns_field_write(const int nb_zones_to_write, const True_int fileId, const True_int baseId, const int ind, const std::vector<True_int>& zoneId, const std::string& LOC,
                                     const True_int flowId_som, const True_int flowId_elem, const char * id_champ, True_int& fieldId_som, True_int& fieldId_elem)
{
  constexpr bool is_PAR_OVER = (_TYPE_ == TYPE_ECRITURE::PAR_OVER);
  for (int ii = 0; ii != nb_zones_to_write; ii++)
    {
      if (LOC == "SOM")
        {
          if (cgp_field_write(fileId, baseId, zoneId[is_PAR_OVER ? ii : ind], flowId_som, CGNS_DOUBLE_TYPE, id_champ, &fieldId_som) != CG_OK)
            Cerr << "Error Ecrire_CGNS_helper::cgns_field_write : cgp_field_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }
      else if (LOC == "ELEM")
        {
          if (cgp_field_write(fileId, baseId, zoneId[is_PAR_OVER ? ii : ind], flowId_elem, CGNS_DOUBLE_TYPE, id_champ, &fieldId_elem) != CG_OK)
            Cerr << "Error Ecrire_CGNS_helper::cgns_field_write : cgp_field_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }
    }
}

template<TYPE_ECRITURE _TYPE_>
inline std::enable_if_t<_TYPE_ == TYPE_ECRITURE::SEQ, void>
Ecrire_CGNS_helper::cgns_field_write_data(const True_int fileId, const True_int baseId, const int ind, const std::vector<True_int>& zoneId,
                                          const std::string& LOC, const True_int flowId_som, const True_int flowId_elem, const int comp,
                                          const char * id_champ, const DoubleTab& valeurs, True_int& fieldId_som, True_int& fieldId_elem)
{
  if (valeurs.dimension(1) == 1) /* No stride ! */
    {
      if (LOC == "SOM")
        {
          if (cg_field_write(fileId, baseId, zoneId[ind], flowId_som, CGNS_DOUBLE_TYPE, id_champ, valeurs.addr(), &fieldId_som) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_field_seq : cg_field_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }
      else // ELEM // TODO FIXME FACES
        {
          if (cg_field_write(fileId, baseId, zoneId[ind], flowId_elem, CGNS_DOUBLE_TYPE, id_champ, valeurs.addr(), &fieldId_elem) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_field_seq : cg_field_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }
    }
  else
    {
      std::vector<double> field_cgns; /* XXX TODO Elie Saikali : try DoubleTrav with addr() later ... mais je pense pas :p */
      for (int i = 0; i < valeurs.dimension(0); i++) field_cgns.push_back(valeurs(i, comp));

      if (LOC == "SOM")
        {
          if (cg_field_write(fileId, baseId, zoneId[ind], flowId_som, CGNS_DOUBLE_TYPE, id_champ, field_cgns.data(), &fieldId_som) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_field_seq : cg_field_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }
      else // ELEM // TODO FIXME FACES
        {
          if (cg_field_write(fileId, baseId, zoneId[ind], flowId_elem, CGNS_DOUBLE_TYPE, id_champ, field_cgns.data(), &fieldId_elem) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_field_seq : cg_field_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }
    }
}

template<TYPE_ECRITURE _TYPE_>
inline std::enable_if_t<_TYPE_ != TYPE_ECRITURE::SEQ, void>
Ecrire_CGNS_helper::cgns_field_write_data(const True_int fileId, const True_int baseId, const int ind, const std::vector<True_int>& zoneId,
                                          const std::string& LOC, const True_int flowId_som, const True_int flowId_elem, const True_int fieldId_som,
                                          const True_int fieldId_elem, const int comp, const cgsize_t min, const cgsize_t max, const DoubleTab& valeurs)
{
  if (valeurs.dimension(1) == 1) /* No stride ! */
    {
      if (LOC == "SOM")
        {
          if (cgp_field_write_data(fileId, baseId, zoneId[ind], flowId_som, fieldId_som, &min, &max, valeurs.addr()) != CG_OK)
            Cerr << "Error Ecrire_CGNS_helper::cgns_field_write_data : cgp_field_write_data !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }
      else
        {
          if (cgp_field_write_data(fileId, baseId, zoneId[ind], flowId_elem, fieldId_elem, &min, &max, valeurs.addr()) != CG_OK)
            Cerr << "Error Ecrire_CGNS_helper::cgns_field_write_data : cgp_field_write_data !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }
    }
  else
    {
      std::vector<double> field_cgns; /* XXX TODO Elie Saikali : try DoubleTrav with addr() later ... mais je pense pas :p */
      for (int i = 0; i < valeurs.dimension(0); i++) field_cgns.push_back(valeurs(i, comp));

      if (LOC == "SOM")
        {
          if (cgp_field_write_data(fileId, baseId, zoneId[ind], flowId_som, fieldId_som, &min, &max, field_cgns.data()) != CG_OK)
            Cerr << "Error Ecrire_CGNS_helper::cgns_field_write_data : cgp_field_write_data !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }
      else
        {
          if (cgp_field_write_data(fileId, baseId, zoneId[ind], flowId_elem, fieldId_elem, &min, &max, field_cgns.data()) != CG_OK)
            Cerr << "Error Ecrire_CGNS_helper::cgns_field_write_data : cgp_field_write_data !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
        }
    }
}

template<TYPE_ECRITURE _TYPE_>
inline void Ecrire_CGNS_helper::cgns_write_iters(const bool has_field, const int nb_zones_to_write, const True_int fileId, const True_int baseId, const int ind, const std::vector<True_int>& zoneId,
                                                 const std::string& LOC, const std::string& solname_som, const std::string& solname_elem, const std::vector<double>& time_post)
{
  const int nsteps = static_cast<int>(time_post.size());
  constexpr bool is_PAR_OVER = (_TYPE_ == TYPE_ECRITURE::PAR_OVER);

  /* create BaseIterativeData */
  if (cg_biter_write(fileId, baseId, "TimeIterValues", nsteps) != CG_OK)
    Cerr << "Error Ecrire_CGNS_helper::cgns_write_iters : cg_biter_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

  /* go to BaseIterativeData level and write time values */
  if (cg_goto(fileId, baseId, "BaseIterativeData_t", 1, "end") != CG_OK)
    Cerr << "Error Ecrire_CGNS_helper::cgns_write_iters : cg_goto !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

  cgsize_t nuse = static_cast<cgsize_t>(nsteps);
  if (cg_array_write("TimeValues", CGNS_DOUBLE_TYPE, 1, &nuse, time_post.data()) != CG_OK)
    Cerr << "Error Ecrire_CGNS_helper::cgns_write_iters : cg_array_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

  cgsize_t idata[2];
  idata[0] = CGNS_STR_SIZE;
  idata[1] = nsteps;

  for (int ii = 0; ii != nb_zones_to_write; ii++)
    if (zoneId[ind] != -123)
      {
        /* create ZoneIterativeData */
        if (cg_ziter_write(fileId, baseId, zoneId[is_PAR_OVER ? ii : ind], "ZoneIterativeData") != CG_OK)
          Cerr << "Error Ecrire_CGNS_helper::cgns_write_iters : cg_ziter_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

        if (cg_goto(fileId, baseId, "Zone_t", zoneId[is_PAR_OVER ? ii : ind], "ZoneIterativeData_t", 1, "end") != CG_OK)
          Cerr << "Error Ecrire_CGNS_helper::cgns_write_iters : cg_goto !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();

        if (has_field)
          {
            if (LOC == "SOM")
              {
                if (cg_array_write("FlowSolutionPointers", CGNS_ENUMV(Character), 2, idata, solname_som.c_str()) != CG_OK)
                  Cerr << "Error Ecrire_CGNS_helper::cgns_write_iters : cg_array_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
              }
            else if (LOC == "ELEM")
              {
                if (cg_array_write("FlowSolutionPointers", CGNS_ENUMV(Character), 2, idata, solname_elem.c_str()) != CG_OK)
                  Cerr << "Error Ecrire_CGNS_helper::cgns_write_iters : cg_array_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
              }
          }
      }

  if (cg_simulation_type_write(fileId, baseId, CGNS_ENUMV(TimeAccurate)) != CG_OK)
    Cerr << "Error Ecrire_CGNS_helper::cgns_write_iters : cg_simulation_type_write !" << finl, Process::is_sequential() ? cg_error_exit() : cgp_error_exit();
}

#endif /* HAS_CGNS */

#endif /* Ecrire_CGNS_helper_included */
