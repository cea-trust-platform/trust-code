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

#ifndef TClearable_included
#define TClearable_included

#include <list>

/*! @brief A small class representing any object with a "clear()" method.
 *
 * Used notably to invoke Domaine_dis_cache::clear() properly at the end of TRUST, but without
 * having to refer to the Domaine_dis_cache class in the MAIN of TRUST, since Domaine_dis_cache is not
 * part of TRUST numerical kernel.
 *
 */
class TClearable
{
public:
  virtual ~TClearable() { }
  virtual void clear() = 0;

  /*! Register an object to be cleared */
  static inline void Register_clearable(TClearable * c)
  {
    to_clear_.push_back(c);
  }

  /*! Clean all registered objects.
   * Typically invoked from TRUST main.
   */
  static inline void Clear_all()
  {
    for(auto& c: to_clear_)
      c->clear();
  }

private:
  static std::list<TClearable *> to_clear_; ///< list of objects to be cleared when exiting TRUST
};

#endif
