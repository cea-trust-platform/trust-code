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

#ifndef Domaine_dis_cache_included
#define Domaine_dis_cache_included

#include <Nom.h>
#include <memory>
#include <map>

class Domaine;
class Domaine_dis;

/*! @brief Cache of discretized domains. Avoid repeating the discretize operation when not
 * necessary.
 *
 * This is a singleton class.
 */
class Domaine_dis_cache
{
public:
  static Domaine_dis_cache& Get_instance();

  static Domaine_dis& Build_or_get(const Nom& type, const Domaine& dom);
  static Domaine_dis& Build_or_get_poly_post(const Nom& type, const Domaine& dom);

  Domaine_dis& build_or_get(const Nom& type, const Domaine& dom);
  Domaine_dis& build_or_get_poly_post(const Nom& type, const Domaine& dom);

private:
  Domaine_dis_cache() {}

  // The actual cache hodling the true Domaine_dis objects.
  // Store them as shared_ptr since we need easy duplication, notably for NO_FACE_ discretisations.
  // See build_or_get() method.
  using Shared_Dom_dis = std::shared_ptr<Domaine_dis>;
  std::map<std::string, Shared_Dom_dis> cache_;
};

#endif
