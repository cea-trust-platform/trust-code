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

#ifndef DomaineAxi1d_included
#define DomaineAxi1d_included

#include <TRUSTTabs_forward.h>
#include <TRUST_Ref.h>
#include <Domaine.h>
#include <Champ_base.h>

class Param;

/*! @brief : class DomaineAxi1d
 *
 *  <Description of class DomaineAxi1d>
 */
template <typename _SIZE_>
class DomaineAxi1d_32_64 : public Domaine_32_64<_SIZE_>
{

  Declare_instanciable_32_64( DomaineAxi1d_32_64 ) ;

public :
  using int_t = _SIZE_;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;


  const Champ_base& champ_origine() const {   return champ_orig.valeur(); }
  const Champ_base& champ_origine()  {   return champ_orig.valeur(); }
  bool has_champ_origine() const  {   return champ_orig.non_nul(); }
  const DoubleTab_t& origine_repere() {   return ref_origine_.valeur(); }
  const DoubleTab_t& origine_repere() const {   return ref_origine_.valeur(); }
  void associer_origine_repere(const DoubleTab_t& orig) {   ref_origine_ = orig; }
  inline double origine_repere(int_t i,int j) { return ref_origine_.valeur()(i,j); }
  inline double origine_repere(int_t i,int j) const { return ref_origine_.valeur()(i,j); }

protected :
  void set_param(Param& param);

  OWN_PTR(Champ_base) champ_orig;
  REF(DoubleTab_t) ref_origine_;
};

using DomaineAxi1d = DomaineAxi1d_32_64<int>;
using DomaineAxi1d_64 = DomaineAxi1d_32_64<trustIdType>;

#endif /* DomaineAxi1d_included */
