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

#include <Segment_axi.h>
#include <DomaineAxi1d.h>

Implemente_instanciable_32_64( Segment_axi_32_64, "Segment_axi", Segment_32_64<_T_> );

template <typename _SIZE_>
Sortie& Segment_axi_32_64<_SIZE_>::printOn( Sortie& os ) const
{
  Segment_32_64<_SIZE_>::printOn( os );
  return os;
}

template <typename _SIZE_>
Entree& Segment_axi_32_64<_SIZE_>::readOn( Entree& is )
{
  Segment_32_64<_SIZE_>::readOn( is );
  return is;
}

template <typename _SIZE_>
void Segment_axi_32_64<_SIZE_>::calculer_volumes(DoubleVect_t& volumes) const
{
  const Domaine_t& domaine=this->mon_dom.valeur();
  const DomaineAxi1d_32_64<_SIZE_>& dom = ref_cast(DomaineAxi1d_32_64<_SIZE_>,domaine);

  int_t S1,S2;

  int_t size_tot = domaine.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);

  assert(dimension==3);

  for (int_t num_poly=0; num_poly<size_tot; num_poly++)
    {
      S1 = domaine.sommet_elem(num_poly,0);
      S2 = domaine.sommet_elem(num_poly,1);

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


template class Segment_axi_32_64<int>;
#if INT_is_64_ == 2
template class Segment_axi_32_64<trustIdType>;
#endif

