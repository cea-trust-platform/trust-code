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

#include <Raccord_base.h>

Implemente_base_32_64(Raccord_base_32_64,"Raccord_base",Frontiere_32_64<_T_>);
Implemente_base_32_64(Raccord_local_32_64,"Raccord_local",Raccord_base_32_64<_T_>);
Implemente_base_32_64(Raccord_distant_32_64,"Raccord_distant",Raccord_base_32_64<_T_>);
Implemente_instanciable_32_64(Raccord_local_homogene_32_64,"Raccord_local_homogene",Raccord_local_32_64<_T_>);

/*! @brief Simple appel a: Frontiere_32_64<_SIZE_>::printOn(Sortie& )
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
template <typename _SIZE_>
Sortie& Raccord_base_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return Frontiere_32_64<_SIZE_>::printOn(s) ;
}


/*! @brief Simple appel a: Frontiere_32_64<_SIZE_>::readOn(Entree& )
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
template <typename _SIZE_>
Entree& Raccord_base_32_64<_SIZE_>::readOn(Entree& s)
{
  return Frontiere_32_64<_SIZE_>::readOn(s) ;
}


/*! @brief Simple appel a: Raccord_base_32_64<_SIZE_>::printOn(Sortie& )
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
template <typename _SIZE_>
Sortie& Raccord_local_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return Raccord_base_32_64<_SIZE_>::printOn(s) ;
}



/*! @brief Simple appel a: Raccord_base_32_64<_SIZE_>::readOn(Entree& )
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
template <typename _SIZE_>
Entree& Raccord_local_32_64<_SIZE_>::readOn(Entree& s)
{
  return Raccord_base_32_64<_SIZE_>::readOn(s) ;
}


/*! @brief Simple appel a: Raccord_base_32_64<_SIZE_>::printOn(Sortie& )
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
template <typename _SIZE_>
Sortie& Raccord_distant_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return Raccord_base_32_64<_SIZE_>::printOn(s) ;
}




/*! Simple appel a: Raccord_base_32_64<_SIZE_>::readOn(Entree& )
 *
 */
template <typename _SIZE_>
Entree& Raccord_distant_32_64<_SIZE_>::readOn(Entree& s)
{
  return Raccord_base_32_64<_SIZE_>::readOn(s) ;
}



/*! @brief Simple appel a: Raccord_local::printOn(Sortie& )
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
template <typename _SIZE_>
Sortie& Raccord_local_homogene_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return Raccord_local_32_64<_SIZE_>::printOn(s) ;
}

/*! @brief Simple appel a: Raccord_local::readOn(Entree& )
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
template <typename _SIZE_>
Entree& Raccord_local_homogene_32_64<_SIZE_>::readOn(Entree& s)
{
  return Raccord_local_32_64<_SIZE_>::readOn(s) ;
}

template <>
void Raccord_local_homogene_32_64<int>::trace_elem_distant(const DoubleTab& x, DoubleTab& y) const
{
  // en suppposant les numerotations des faces identiques sur le raccord des deux cotes
  return this->trace_elem_local(x,y);
}

template <>
void Raccord_local_homogene_32_64<int>::trace_face_distant(const DoubleTab& x, DoubleTab& y) const
{
  return this->trace_face_local(x,y);
}

template <>
void Raccord_local_homogene_32_64<int>::trace_face_distant(const DoubleVect& x, DoubleVect& y) const
{
  return this->trace_face_local(x,y);
}



// 64 bit versions should never be called:

template <typename _SIZE_>
void Raccord_local_homogene_32_64<_SIZE_>::trace_elem_distant(const DoubleTab& x, DoubleTab& y) const
{
  assert(false);
  throw;
}

template <typename _SIZE_>
void Raccord_local_homogene_32_64<_SIZE_>::trace_face_distant(const DoubleTab& x, DoubleTab& y) const
{
  assert(false);
  throw;
}

template <typename _SIZE_>
void Raccord_local_homogene_32_64<_SIZE_>::trace_face_distant(const DoubleVect& x, DoubleVect& y) const
{
  assert(false);
  throw;
}


template class Raccord_base_32_64<int>;
#if INT_is_64_ == 2
template class Raccord_base_32_64<trustIdType>;
#endif

template class Raccord_local_32_64<int>;
#if INT_is_64_ == 2
template class Raccord_local_32_64<trustIdType>;
#endif

template class Raccord_local_homogene_32_64<int>;
#if INT_is_64_ == 2
template class Raccord_local_homogene_32_64<trustIdType>;
#endif

template class Raccord_distant_32_64<int>;
#if INT_is_64_ == 2
template class Raccord_distant_32_64<trustIdType>;
#endif

