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

#include <Domaine_dis_cache.h>
#include <TRUST_2_CGNS.h>
#include <Option_CGNS.h>
#include <Domaine_VF.h>
#include <Polyedre.h>
#include <Domaine.h>

#ifdef MPI_
#ifdef INT_is_64_
#define MPI_ENTIER MPI_LONG
#else
#define MPI_ENTIER MPI_INT
#endif
#endif

void TRUST_2_CGNS::associer_domaine_TRUST(const Domaine * dom, const DoubleTab& som, const IntTab& elem)
{
  if (dom) dom_trust_ = *dom;
  sommets_ = som;
  elems_ = elem;
}

void TRUST_2_CGNS::fill_coords(std::vector<double>& xCoords, std::vector<double>& yCoords, std::vector<double>& zCoords)
{
  const int dim = sommets_->dimension(1), nb_som = sommets_->dimension(0);
  const DoubleTab& sommets = sommets_.valeur();

  for (int i = 0; i < nb_som; i++)
    {
      xCoords.push_back(sommets(i, 0));
      yCoords.push_back(sommets(i, 1));
      if (dim > 2) zCoords.push_back(sommets(i, 2));
    }
}

void TRUST_2_CGNS::fill_global_infos()
{
  assert (!Option_CGNS::PARALLEL_OVER_ZONE);
  assert (sommets_.non_nul() && elems_.non_nul());

  const int nb_som = sommets_->dimension(0), nb_elem = elems_->dimension(0);
  const int nb_procs = Process::nproc();

  par_in_zone_ = true;
  global_nb_elem_.assign(nb_procs, -123 /* default */);
  global_nb_som_.assign(nb_procs, -123 /* default */);

#ifdef MPI_
//  grp.all_gather(&nb_elem, global_nb_elem.data(), 1); // Elie : pas MPI_CHAR desole
  MPI_Allgather(&nb_elem, 1, MPI_ENTIER, global_nb_elem_.data(), 1, MPI_ENTIER, MPI_COMM_WORLD);
  MPI_Allgather(&nb_som, 1, MPI_ENTIER, global_nb_som_.data(), 1, MPI_ENTIER, MPI_COMM_WORLD);
#endif

  global_incr_min_elem_.assign(nb_procs, -123 /* default */);
  global_incr_max_elem_.assign(nb_procs, -123 /* default */);
  global_incr_min_som_.assign(nb_procs, -123 /* default */);
  global_incr_max_som_.assign(nb_procs, -123 /* default */);

  global_incr_min_elem_[0] = 1, global_incr_min_som_[0] = 1; // start from 1 !
  ns_tot_ = 0, ne_tot_ = 0;

  // now we fill global incremented min/max stuff
  for (int i = 0; i < nb_procs; i++)
    {
      // 1 : min
      global_incr_min_elem_[i] = ne_tot_ + 1;
      global_incr_min_som_[i] = ns_tot_ + 1;
      // 2 : increment
      ne_tot_ += global_nb_elem_[i];
      ns_tot_ += global_nb_som_[i];
      // 3 : max
      global_incr_max_elem_[i] = ne_tot_;
      global_incr_max_som_[i] = ns_tot_;
    }

  const auto min_nb_elem = std::min_element(global_nb_elem_.begin(), global_nb_elem_.end());
  nb_procs_writing_ = nb_procs; // pour le moment

  if (*min_nb_elem <= 0) // not all procs will write !
    {
      // remplir proc_non_zero_elem avec le numero de proc si nb_elem > 0 !!
      for (int i = 0; i < static_cast<int>(global_nb_elem_.size()); i++)
        if (global_nb_elem_[i] > 0) proc_non_zero_elem_.push_back(i);

      nb_procs_writing_ = static_cast<int>(proc_non_zero_elem_.size());
      all_procs_write_ = false;
    }
}

#ifdef HAS_CGNS
int TRUST_2_CGNS::convert_connectivity(const CGNS_TYPE type , std::vector<cgsize_t>& elems)
{
  const int nb_elem = elems_->dimension(0);
  const IntTab& les_elems =  elems_.valeur();

  int decal = 0;

  if (par_in_zone_)
    {
      const int proc_me = Process::me();
      if (all_procs_write_)
        {
          if (proc_me > 0) // pas maitre
            decal = global_incr_max_som_[proc_me - 1];
        }
      else
        {
          for (int i = 0; i < nb_procs_writing_; i++)
            if (proc_non_zero_elem_[i] == proc_me)
              if (i > 0) // pas premier case
                {
                  decal = global_incr_max_som_[proc_non_zero_elem_[i] - 1];
                  break;
                }
        }
    }

  switch (type)
    {
    case CGNS_ENUMV(HEXA_8):
      for (int i = 0; i < nb_elem; i++)
        {
          elems.push_back(les_elems(i, 0) + 1 + decal);
          elems.push_back(les_elems(i, 1) + 1 + decal);
          elems.push_back(les_elems(i, 3) + 1 + decal);
          elems.push_back(les_elems(i, 2) + 1 + decal);
          elems.push_back(les_elems(i, 4) + 1 + decal);
          elems.push_back(les_elems(i, 5) + 1 + decal);
          elems.push_back(les_elems(i, 7) + 1 + decal);
          elems.push_back(les_elems(i, 6) + 1 + decal);
        }
      return 8;
    case CGNS_ENUMV(QUAD_4):
      for (int i = 0; i < nb_elem; i++)
        {
          elems.push_back(les_elems(i, 0) + 1 + decal);
          elems.push_back(les_elems(i, 1) + 1 + decal);
          elems.push_back(les_elems(i, 3) + 1 + decal);
          elems.push_back(les_elems(i, 2) + 1 + decal);
        }
      return 4;
    case CGNS_ENUMV(TETRA_4):
      for (int i = 0; i < nb_elem; i++)
        for (int j = 0; j < 4; j++)
          elems.push_back(les_elems(i, j) + 1 + decal);
      return 4;
    case CGNS_ENUMV(TRI_3):
      for (int i = 0; i < nb_elem; i++)
        for (int j = 0; j < 3; j++)
          elems.push_back(les_elems(i, j) + 1 + decal);
      return 3;
    case CGNS_ENUMV(BAR_2):
      for (int i = 0; i < nb_elem; i++)
        for (int j = 0; j < 2; j++)
          elems.push_back(les_elems(i, j) + 1 + decal);
      return 2;
    default:
      {
        Cerr << "Type not yet coded in TRUST_2_CGNS::convert_connectivity ! Call the 911 !" << finl;
        Process::exit();
        return -100;
      }
    }
}

CGNS_TYPE TRUST_2_CGNS::convert_elem_type(const Motcle& type)
{
  if (type == "HEXAEDRE" || type == "HEXAEDRE_VEF")
    return CGNS_ENUMV(HEXA_8);
  else if (type == "RECTANGLE" || type == "QUADRANGLE" || type == "QUADRANGLE_3D")
    return CGNS_ENUMV(QUAD_4);
  else if (type == "TRIANGLE" || type == "TRIANGLE_3D")
    return CGNS_ENUMV(TRI_3);
  else if (type == "SEGMENT" || type == "SEGMENT_2D")
    return CGNS_ENUMV(BAR_2);
  else if (type == "TETRAEDRE")
    return CGNS_ENUMV(TETRA_4);
  else if (type == "POLYEDRE" || type == "POLYGONE" || type == "PRISME" || type == "PRISME_HEXAG" || type == "POLYGONE_3D")
    return CGNS_ENUMV(NGON_n);
  else
    {
      Cerr << "The type " << type << " is not yet available for the CGNS format ! Call the 911 !" << finl;
      Process::exit();
      return CGNS_ENUMV(ElementTypeNull);
    }
}

int TRUST_2_CGNS::convert_connectivity_nface(std::vector<cgsize_t>& econ, std::vector<cgsize_t>& eoff)
{
  assert (dom_trust_.non_nul());
  const Domaine_dis& domaine_dis = Domaine_dis_cache::Build_or_get_poly_post("Domaine_PolyMAC", dom_trust_.valeur());
  const Domaine_VF& vf = ref_cast (Domaine_VF, domaine_dis.valeur());
  const IntTab& ef = vf.elem_faces();

  eoff.push_back(0); // first index = > 0 !

  int s = 0;
  for (int i = 0; i < ef.dimension(0); i++)
    {
      for (int j = 0; j < ef.dimension(1); j++)
        {
          if (ef(i, j) > -1)
            {
              econ.push_back(ef(i, j) + 1);
              s++;
            }
          else
            break;
        }
      eoff.push_back(s);
    }

  // multiply by -1 repeated faces
  for (int i = static_cast<int>(econ.size()) -1; i >0; i-- )
    {
      int val = static_cast<int>(econ[i]);
      for (int j = i-1; j > 0; j--)
        if (econ[j] == val)
          {
            econ[i] *= -1;
            break;
          }
    }

  return ef.dimension(0);
}

int TRUST_2_CGNS::convert_connectivity_ngon(std::vector<cgsize_t>& econ, std::vector<cgsize_t>& eoff, const bool is_polyedre)
{
  assert (dom_trust_.non_nul());
  if (is_polyedre)
    {
      const Domaine_dis& domaine_dis = Domaine_dis_cache::Build_or_get_poly_post("Domaine_PolyMAC", dom_trust_.valeur());
      const Domaine_VF& vf = ref_cast(Domaine_VF, domaine_dis.valeur());
      const IntTab& fs = vf.face_sommets();

      eoff.push_back(0); // first index = > 0 !

      int s = 0;
      for (int i = 0; i < fs.dimension(0); i++)
        {
          for (int j = 0; j < fs.dimension(1); j++)
            {
              if (fs(i, j) > -1)
                {
                  econ.push_back(fs(i, j) + 1);
                  s++;
                }
              else
                break;
            }
          eoff.push_back(s);
        }
      return fs.dimension(0);
    }
  else // POLYGONE
    {
      const IntTab& les_elems =  dom_trust_->les_elems();

      eoff.push_back(0); // first index = > 0 !

      int s = 0;
      for (int i = 0; i < les_elems.dimension(0); i++)
        {
          for (int j = 0; j < les_elems.dimension(1); j++)
            {
              if (les_elems(i, j) > -1)
                {
                  econ.push_back(les_elems(i, j) + 1);
                  s++;
                }
              else
                break;
            }
          eoff.push_back(s);
        }

      return les_elems.dimension(0);
    }
}

#endif
