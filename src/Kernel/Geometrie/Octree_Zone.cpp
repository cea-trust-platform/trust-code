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
#include <Octree_Zone.h>
#include <Domaine.h>

void Octree_Zone::reset()
{
  ref_zone_.reset();
  octree_double_.reset();
  pos_.reset();
}

void Octree_Zone::build(const Zone& zone, const double epsilon)
{
  ref_zone_ = zone;
  octree_double_.build_elements(zone.domaine().les_sommets(),
                                zone.les_elems(),
                                epsilon,
                                1);
  pos_.resize_array(zone.domaine().les_sommets().dimension(1));
}

int Octree_Zone::rang_elem(double x, double y, double z) const
{
  return find_elements(x, y, z, 0 /* pointeur nul */);
}

void Octree_Zone::rang_elements(double x, double y, double z, ArrOfInt& elements) const
{
  find_elements(x, y, z, &elements);
}

// Description: si elements est non nul, remplit le tableau avec la liste des
//  elements contenant le point x,y,z et renvoie -1, sinon renvoie l'indice du
//  premier element trouve contenant le point x,y,z ou -1 si aucun element.
int Octree_Zone::find_elements(double x, double y, double z, ArrOfInt *elements) const
{
  if (elements)
    elements->resize_array(0);
  const ArrOfInt& floor_elements = octree_double_.floor_elements();
  const Elem_geom& elemgeom = ref_zone_.valeur().type_elem();
  int index;
  const int nb_elem = octree_double_.search_elements(x, y, z, index);
  pos_[0] = x;
  const int dim = pos_.size_array();
  if (dim>=2) pos_[1] = y;
  if (dim>2) pos_[2] = z;
  for (int i_elem = 0; i_elem < nb_elem; i_elem++)
    {
      const int test_elem = floor_elements[index + i_elem];
      // tester l'intersection
      if (elemgeom.contient(pos_, test_elem))
        {
          if (elements)
            elements->append_array(test_elem);
          else
            return test_elem;
        }
    }
  return -1;
}

