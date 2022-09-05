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

#ifndef Fluide_sodium_liquide_included
#define Fluide_sodium_liquide_included

#include <Fluide_reel_base.h>
#include <Lois_sodium_span.h>

/*! @brief Classe Fluide_sodium_liquide Cette classe represente un milieu reel
 *
 *     dont les lois viennent de "Lois_Na"
 *
 */
class Fluide_sodium_liquide: public Fluide_reel_base
{
  Declare_instanciable(Fluide_sodium_liquide);

  std::map<std::string, std::array<double, 2>> unknown_range() const override
  {
    return { { "temperature", { 371 - 273.15, 2503.7 - 273.15 } } }; //de la temperature de solidification au pt tricritique
  }

protected :
  inline void rho__(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int id = 0) const override { RhoL_span(T,P,res,ncomp,id); }
  inline void dP_rho__(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int id = 0) const override { DPRhoL_span(T,P,res,ncomp,id); }
  inline void dT_rho__(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int id = 0) const override { DTRhoL_span(T,P,res,ncomp,id); }
  inline void h__(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int id = 0) const override { HL_span(T,P,res,ncomp,id); }
  inline void dP_h__(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int id = 0) const override { DPHL_span(T,P,res,ncomp,id); }
  inline void dT_h__(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int id = 0) const override { DTHL_span(T,P,res,ncomp,id); }
  inline void cp__(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int id = 0) const override { DTHL_span(T,P,res,ncomp,id); }
  inline void beta__(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int id = 0) const override { BETAL_span(T,res,ncomp,id); } // passe pas P
  inline void mu__(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int id = 0) const override { MuL_span(T,res,ncomp,id); } // passe pas P
  inline void lambda__(const SpanD T, const SpanD P, SpanD res, int ncomp = 1, int id = 0) const override { LambdaL_span(T,res,ncomp,id); } // passe pas P
};

#endif /* Fluide_sodium_liquide_included */
