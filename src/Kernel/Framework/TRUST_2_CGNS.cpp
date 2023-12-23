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

#include <TRUST_2_CGNS.h>
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
  if (type == "HEXAEDRE" || type == "HEXAEDRE_VEF") return CGNS_ENUMV(HEXA_8);
  else
    {
      Cerr << "The type " << type << " is not yet available for the CGNS format ! Call the 911 !" << finl;
      Process::exit();
      return CGNS_ENUMV(ElementTypeNull);
    }
}
#endif
