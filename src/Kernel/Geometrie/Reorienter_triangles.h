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

#ifndef Reorienter_triangles_included
#define Reorienter_triangles_included

#include <Interprete_geometrique_base.h>
#include <Domaine.h>
#include <Domaine_forward.h>

enum class Sens_Orient {DIRECT , INDIRECT};

/*! @brief class Reorienter_triangle Balaye les triangles du maillage pour qu'ils soient directs.
 *
 * @sa Interprete
 */
template <typename _SIZE_>
class Reorienter_triangles_32_64 : public Interprete_geometrique_base_32_64<_SIZE_>
{
  Declare_instanciable_32_64(Reorienter_triangles_32_64);

public :
  using int_t = _SIZE_;
  using IntTab_t = ITab_T<_SIZE_>;
  using DoubleTab_t = DTab_T<_SIZE_>;

  using Domaine_t = Domaine_32_64<_SIZE_>;

  Entree& interpreter_(Entree&) override;
  void reorienter(Domaine_t&) const;

protected :

  Sens_Orient test_orientation_triangle(IntTab_t& les_elems, int_t ielem, const DoubleTab_t& coord_sommets) const;
  Sens_Orient reorienter_triangle(IntTab_t& les_elems, int_t ielem) const;


};

using Reorienter_triangles = Reorienter_triangles_32_64<int>;
using Reorienter_triangles_64 = Reorienter_triangles_32_64<trustIdType>;

#endif

