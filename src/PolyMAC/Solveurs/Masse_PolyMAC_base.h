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

#ifndef Masse_PolyMAC_base_included
#define Masse_PolyMAC_base_included

#include <Solveur_Masse_base.h>
#include <TRUST_Ref.h>

class Domaine_Cl_PolyMAC;
class Domaine_PolyMAC;

class Masse_PolyMAC_base : public Solveur_Masse_base
{
  Declare_base(Masse_PolyMAC_base);
public:

  void associer_domaine_dis_base(const Domaine_dis_base& ) override;
  void associer_domaine_cl_dis_base(const Domaine_Cl_dis_base& ) override;
  void completer() override;
  void appliquer_coef(DoubleVect& coef) const;

protected:
  bool no_diff_ = false;
  OBS_PTR(Domaine_PolyMAC) le_dom_PolyMAC;
  OBS_PTR(Domaine_Cl_PolyMAC) le_dom_Cl_PolyMAC;
};

class Masse_PolyMAC_P0P1NC_base : public Masse_PolyMAC_base
{
  Declare_base(Masse_PolyMAC_P0P1NC_base);
public:
  int has_interface_blocs() const override { return 1; }
  void check_multiphase_compatibility() const override { }
  void completer() override { }
};

#endif /* Masse_PolyMAC_base_included */
