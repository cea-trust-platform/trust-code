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

#include <MD_Vector_mono.h>
#include <Param.h>

Implemente_base(MD_Vector_mono,"MD_Vector_mono",MD_Vector_base);

Entree& MD_Vector_mono::readOn(Entree& is)
{
  MD_Vector_base::readOn(is);

  Param p("MD_Vector_mono");
  p.ajouter("blocs_items_to_sum", &blocs_items_to_sum_);
  p.ajouter("blocs_items_to_compute", &blocs_items_to_compute_);
  p.lire_avec_accolades(is);

  return is;
}

Sortie& MD_Vector_mono::printOn(Sortie& os) const
{
  MD_Vector_base::printOn(os);

  os << "{" << finl;
  os << "blocs_items_to_sum" << tspace << blocs_items_to_sum_;
  os << "blocs_items_to_compute" << tspace << blocs_items_to_compute_;
  os << "}" << finl;

  return os;
}

