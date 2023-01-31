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

#ifndef Evaluateur_Source_included
#define Evaluateur_Source_included

#include <Ref_Zone_Cl_dis_base.h>
#include <Ref_Zone_dis_base.h>
#include <Zone_Cl_dis_base.h>
#include <Zone_dis_base.h>
#include <TRUSTTab.h>

class Evaluateur_Source
{
public:
  Evaluateur_Source() { }
  Evaluateur_Source(const Evaluateur_Source& eval) : le_dom(eval.le_dom), la_zcl(eval.la_zcl) { }
  virtual ~Evaluateur_Source() { }

  template <typename Type_Double>
  void calculer_terme_source(int, Type_Double&) const { Process::exit("Evaluateur_Source::calculer_terme_source must be overloaded !!"); }

  virtual void mettre_a_jour() = 0;
  virtual void completer() = 0;

  void associer_domaines(const Zone_dis_base& zone, const Zone_Cl_dis_base& zone_cl)
  {
    le_dom = zone;
    la_zcl = zone_cl;
    completer();
  }

protected:
  REF(Zone_dis_base) le_dom;
  REF(Zone_Cl_dis_base) la_zcl;
};

#endif /* Evaluateur_Source_included */
