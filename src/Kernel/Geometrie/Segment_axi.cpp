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

#include <Segment_axi.h>
#include <DomaineAxi1d.h>

Implemente_instanciable( Segment_axi, "Segment_axi", Segment ) ;

Sortie& Segment_axi::printOn( Sortie& os ) const
{
  Segment::printOn( os );
  return os;
}

Entree& Segment_axi::readOn( Entree& is )
{
  Segment::readOn( is );
  return is;
}

void Segment_axi::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=ma_zone.valeur();
  const DomaineAxi1d& dom = ref_cast(DomaineAxi1d,zone.domaine());

  int S1,S2;

  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);

  assert(dimension==3);

  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = zone.sommet_elem(num_poly,0);
      S2 = zone.sommet_elem(num_poly,1);

      double x0 = dom.origine_repere(num_poly,0);
      double y0 = dom.origine_repere(num_poly,1);

      double x1 = dom.coord(S1,0);
      double x2 = dom.coord(S2,0);

      double y1 = dom.coord(S1,1);
      double y2 = dom.coord(S2,1);

      double r1 = sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
      double r2 = sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0));


      volumes[num_poly]= M_PI*std::fabs(r2*r2-r1*r1);
    }
}
