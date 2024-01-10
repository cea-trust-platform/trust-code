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
#include <Domaine_VF.h>
#include <Polyedre.h>
#include <Domaine.h>

void TRUST_2_CGNS::associer_domaine_TRUST(const Domaine& dom)
{
  dom_trust_ = dom;
}

void TRUST_2_CGNS::fill_coords(std::vector<double>& xCoords, std::vector<double>& yCoords, std::vector<double>& zCoords)
{
  const int dim = dom_trust_->les_sommets().dimension(1), nb_som = dom_trust_->nb_som();
  const DoubleTab& sommets = dom_trust_->les_sommets();

  for (int i = 0; i < nb_som; i++)
    {
      xCoords.push_back(sommets(i, 0));
      yCoords.push_back(sommets(i, 1));
      if (dim > 2) zCoords.push_back(sommets(i, 2));
    }
}

#ifdef HAS_CGNS
int TRUST_2_CGNS::convert_connectivity(const CGNS_TYPE type , std::vector<cgsize_t>& elems)
{
  const int nb_elem = dom_trust_->nb_elem();
  const IntTab& les_elems =  dom_trust_->les_elems();
  switch (type)
    {
    case CGNS_ENUMV(HEXA_8):
      for (int i = 0; i < nb_elem; i++)
        {
          elems.push_back(les_elems(i, 0) + 1);
          elems.push_back(les_elems(i, 1) + 1);
          elems.push_back(les_elems(i, 3) + 1);
          elems.push_back(les_elems(i, 2) + 1);
          elems.push_back(les_elems(i, 4) + 1);
          elems.push_back(les_elems(i, 5) + 1);
          elems.push_back(les_elems(i, 7) + 1);
          elems.push_back(les_elems(i, 6) + 1);
        }
      return 8;
    case CGNS_ENUMV(QUAD_4):
      for (int i = 0; i < nb_elem; i++)
        {
          elems.push_back(les_elems(i, 0) + 1);
          elems.push_back(les_elems(i, 1) + 1);
          elems.push_back(les_elems(i, 3) + 1);
          elems.push_back(les_elems(i, 2) + 1);
        }
      return 4;
    case CGNS_ENUMV(TETRA_4):
      for (int i = 0; i < nb_elem; i++)
        for (int j = 0; j < 4; j++)
          elems.push_back(les_elems(i, j) + 1);
      return 4;
    case CGNS_ENUMV(TRI_3):
      for (int i = 0; i < nb_elem; i++)
        for (int j = 0; j < 3; j++)
          elems.push_back(les_elems(i, j) + 1);
      return 3;
    case CGNS_ENUMV(BAR_2):
      for (int i = 0; i < nb_elem; i++)
        for (int j = 0; j < 2; j++)
          elems.push_back(les_elems(i, j) + 1);
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
  for (int i = (int)econ.size() -1; i >0; i-- )
    {
      int val = econ[i];
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
