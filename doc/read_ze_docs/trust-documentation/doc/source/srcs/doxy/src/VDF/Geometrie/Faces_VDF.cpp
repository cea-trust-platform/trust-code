/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Faces_VDF.h>
#include <Domaine.h>

Implemente_instanciable(Faces_VDF,"Faces_VDF",Faces);

//// printOn
//
Sortie& Faces_VDF::printOn(Sortie& s) const
{
  return Faces::printOn(s) ;
}


//// readOn
//

Entree& Faces_VDF::readOn(Entree& s)
{
  return Faces::readOn(s) ;
}


void Faces_VDF::calculer_orientation(IntVect& tab_orientation,
                                     int& nx, int& ny, int& nz)
{
  tab_orientation.resize(nb_faces_tot());
  orientation_.ref(tab_orientation);
  nx=ny=nz=0;

  const Domaine& mondomaine=domaine();
  const Domaine& dom=mondomaine;
  double dx=0, dy=0, dz=1.e30;

  for(int face=0; face<nb_faces_tot(); face++)
    {
      if(dimension == 2)
        {
          dx = std::fabs(dom.coord(sommet(face, 1), 0)-dom.coord(sommet(face, 0), 0));
          dy = std::fabs(dom.coord(sommet(face, 1), 1)-dom.coord(sommet(face, 0), 1));
        }
      else if(dimension == 3)
        {
          if (nb_som_faces() != 4 )
            {
              Cerr << "We found a VDF face with nb_som_faces = " << nb_som_faces() << ". Aren't you trying to read a VEF mesh in VDF ?" << finl;
              Cerr << "There is a problem with mesh or with chosen discretization." << finl;
              exit();
            }
          dx = std::fabs(dom.coord(sommet(face, 3), 0)-dom.coord(sommet(face, 0), 0));
          dy = std::fabs(dom.coord(sommet(face, 3), 1)-dom.coord(sommet(face, 0), 1));
          dz = std::fabs(dom.coord(sommet(face, 3), 2)-dom.coord(sommet(face, 0), 2));
        }
      if(axi)
        {
          if(dy<0) dy+=2.*M_PI;
        }
      if ( (dx<=dy) && (dx<=dz) )
        {
          tab_orientation(face)=0;
          nx++;
        }
      else if ( (dy<=dx) && (dy<=dz) )
        {
          tab_orientation(face)=1;
          ny++;
        }
      else if ( (dz<=dx) && (dz<=dy) )
        {
          tab_orientation(face)=2;
          nz++;
        }
    }
}
