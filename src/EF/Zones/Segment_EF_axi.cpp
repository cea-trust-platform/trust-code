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

#include <Segment_EF_axi.h>
#include <Zone.h>
#include <DomaineAxi1d.h>

Implemente_instanciable( Segment_EF_axi, "Segment_EF_axi", Segment_EF ) ;

Sortie& Segment_EF_axi::printOn( Sortie& os ) const
{
  Segment_EF::printOn( os );
  return os;
}

Entree& Segment_EF_axi::readOn( Entree& is )
{
  Segment_EF::readOn( is );
  return is;
}

void Segment_EF_axi::normale(int num_Face,DoubleTab& Face_normales,
                             const  IntTab& Face_sommets,
                             const IntTab& Face_voisins,
                             const IntTab& elem_faces,
                             const Zone& zone_geom) const
{
  Segment_EF::normale(num_Face,Face_normales,Face_sommets,Face_voisins,elem_faces,zone_geom);

  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();
  const DomaineAxi1d& domax = ref_cast(DomaineAxi1d,zone_geom.domaine());
  int elem = Face_voisins(num_Face,0)==-1 ? Face_voisins(num_Face,1) : Face_voisins(num_Face,0);

  double x0 = domax.origine_repere(elem,0);
  double y0 = domax.origine_repere(elem,1);

  int som = Face_sommets(num_Face,0);
  double x = les_coords(som,0);
  double y = les_coords(som,1);

  double r = sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0));
  double surf = 2.*M_PI*r;
  if (surf>0.)
    Face_normales(num_Face,0)*=surf;
}
