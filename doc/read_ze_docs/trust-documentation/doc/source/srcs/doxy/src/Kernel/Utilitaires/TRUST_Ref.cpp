/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <TRUST_Ref.h>
#include <Objet_U.h>

TRUST_Ref_Objet_U::~TRUST_Ref_Objet_U() = default;

TRUST_Ref_Objet_U::TRUST_Ref_Objet_U() = default;

TRUST_Ref_Objet_U::TRUST_Ref_Objet_U(const Objet_U& t) : p_((Objet_U*)&t) { }

TRUST_Ref_Objet_U::TRUST_Ref_Objet_U(const TRUST_Ref_Objet_U& t) : p_(t.p_) { }

const TRUST_Ref_Objet_U& TRUST_Ref_Objet_U::operator=(const Objet_U& t)
{
  p_ = const_cast<Objet_U*>(&t);
  return *this;
}

const TRUST_Ref_Objet_U& TRUST_Ref_Objet_U::operator=(const TRUST_Ref_Objet_U& t)
{
  p_ = t.p_;
  return *this;
}

bool TRUST_Ref_Objet_U::non_nul() const
{
  return p_ != nullptr;
}

bool TRUST_Ref_Objet_U::est_nul() const
{
  return p_ == nullptr;
}

int operator ==(const TRUST_Ref_Objet_U& r1, const TRUST_Ref_Objet_U& r2)
{
  if (r1.est_nul() && r2.est_nul()) return 1;
  if (r1->numero() == r2->numero()) return 1;
  return 0;
}
