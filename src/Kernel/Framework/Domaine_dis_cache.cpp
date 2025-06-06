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

#include <Domaine_dis_cache.h>
#include <Domaine_dis_base.h>
#include <Interprete_bloc.h>
#include <Domaine.h>
#include <sstream>

/*! @brief Get the unique instance of the cache.
 */
Domaine_dis_cache& Domaine_dis_cache::Get_instance()
{
  static Domaine_dis_cache instance_;
  static bool is_first_time = true; // will remain true only once!

  // Register unique instance for clearing when TRUST will exit:
  if (is_first_time)
    TClearable::Register_clearable(&instance_);

  is_first_time = false;
  return instance_;
}

/*! @brief Clear the content of the cache. Useful to exit nicely making sure everything is deallocated.
 */
void Domaine_dis_cache::Clear()
{
  Domaine_dis_cache& ddc = Get_instance();
  ddc.cache_.clear();
}

/*! @brief Get a discretized domain from the cache, building it and recording it if not
 * there yet.
 *
 * Process is as follows: type might start with "NO_FACE_" indicating that we just need a simplified discretisation.
 * (see Domaine_dis_base::discretiser_no_face())
 * Otherwise we want the full discretisation. The full discr can be used in place of a "NO_FACE", but not the inverse.
 * In the cache, we save both the full and the NO_FACE if the full was requested. Otherwise just the NO_FACE is saved.
 * The cache key also uses the current interpretor address to handle weird cases like:
 *      domaine dom
 *      probleme pb
 *      Discretize pb dis
 *      {
 *        Domaine dom
 *        Probleme pb
 *        Discretize pb dis
 *        Lire pb { ... }
 *      }
 *      Lire pb { ... }
 */
Domaine_dis_base& Domaine_dis_cache::build_or_get(const Nom& type, const Domaine& dom)
{
  // Do we have the prefix "NO_FACE_" at the begining of type:
  Nom typ_short(type);
  typ_short.suffix("NO_FACE_");
  bool no_face = typ_short != type;

  // Build a key of the form: "(@x07f8e63)_dom_NO_FACE_Domaine_PolyMAC_P0P1NC"
  auto make_key = [&](const std::string& t)
  {
    std::ostringstream oss;
    oss << "(@" << &(Interprete_bloc::interprete_courant()) << ")_" << dom.le_nom().getString() << "_" << t;
    return oss.str();
  };

  // Query the cache:
  std::string key = make_key(type.getString());
  if (cache_.count(key)) return cache_[key].valeur();
  // We can return the fully discretized domain even if NO_FACE_ was asked, since the former encompasses the latter:
  std::string key_no_face = make_key(typ_short.getString());
  if (cache_.count(key_no_face)) return cache_[key_no_face].valeur();

  // OK, not in cache, we have to really discretized:
  cache_[key] = OWN_PTR(Domaine_dis_base)();
  OWN_PTR(Domaine_dis_base)& ddp = cache_[key];
  ddp.typer(typ_short);
  ddp->associer_domaine(dom);
  ddp->discretiser_root(type);

  // If a full discretisation was requested, we can also register the NO_FACE_ version:
  if (!no_face)
    {
      std::string key_face = make_key("NO_FACE_" + typ_short.getString());
      cache_[key_face] = ddp;  // shared_ptr copy
    }
  return cache_[key].valeur();
}

Domaine_dis_base& Domaine_dis_cache::build_or_get_poly_post(const Nom& type, const Domaine& dom)
{
  std::string new_type = dom.le_nom().getString() + "_" + type.getString();
  for (auto &itr : cache_)
    if (itr.first.find(new_type) != std::string::npos)
      return cache_[itr.first].valeur();

  return build_or_get(type, dom);
}

Domaine_dis_base& Domaine_dis_cache::Build_or_get(const Nom& type, const Domaine& dom)
{
  return Get_instance().build_or_get(type, dom);
}

Domaine_dis_base& Domaine_dis_cache::Build_or_get_poly_post(const Nom& type, const Domaine& dom)
{
  return Get_instance().build_or_get_poly_post(type, dom);
}
