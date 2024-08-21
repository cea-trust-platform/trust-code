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

#include <Poly_geom_base.h>
#include <Domaine.h>

Implemente_base_sans_destructeur_32_64( Poly_geom_base_32_64, "Poly_geom_base", Elem_geom_base_32_64<_T_> );

template <typename _SIZE_>
Sortie& Poly_geom_base_32_64<_SIZE_>::printOn( Sortie& os ) const
{
  Elem_geom_base_32_64<_SIZE_>::printOn( os );
  return os;
}

template <typename _SIZE_>
Entree& Poly_geom_base_32_64<_SIZE_>::readOn( Entree& is )
{
  Elem_geom_base_32_64<_SIZE_>::readOn( is );
  return is;
}

template <typename _SIZE_>
int Poly_geom_base_32_64<_SIZE_>::get_nb_som_elem_max() const
{
  if (nb_som_elem_max_>-1)
    return nb_som_elem_max_ ;
  else
    // (int) cast - we assume high order dim in an array are always small:
    return this->mon_dom->les_elems().dimension_int(1);
}


template class Poly_geom_base_32_64<int>;
#if INT_is_64_ == 2
template class Poly_geom_base_32_64<trustIdType>;
#endif

