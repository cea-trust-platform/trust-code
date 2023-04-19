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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Evaluateur_Source_PolyMAC_old.h
// Directory:   $TRUST_ROOT/src/PolyMAC_old/Sources
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Evaluateur_Source_PolyMAC_old_included
#define Evaluateur_Source_PolyMAC_old_included

#include <TRUSTTabs_forward.h>
#include <Domaine_Cl_PolyMAC_old.h>
#include <Domaine_PolyMAC_old.h>
#include <TRUST_Ref.h>

class Evaluateur_Source_PolyMAC_old
{
public:
  inline Evaluateur_Source_PolyMAC_old();
  inline virtual ~Evaluateur_Source_PolyMAC_old() {};
  inline Evaluateur_Source_PolyMAC_old(const Evaluateur_Source_PolyMAC_old& );
  inline void associer_domaines(const Domaine_dis_base&,const Domaine_Cl_dis_base&);
  virtual void mettre_a_jour( ) =0;
  virtual void completer() = 0;
  virtual double calculer_terme_source(int ) const =0;
  virtual void calculer_terme_source(int , DoubleVect&  ) const =0;

protected:
  REF(Domaine_PolyMAC_old) la_domaine;
  REF(Domaine_Cl_PolyMAC_old) la_zcl;
};

inline Evaluateur_Source_PolyMAC_old::Evaluateur_Source_PolyMAC_old() {}

inline Evaluateur_Source_PolyMAC_old::Evaluateur_Source_PolyMAC_old(const Evaluateur_Source_PolyMAC_old& eval)
  : la_domaine(eval.la_domaine),la_zcl(eval.la_zcl) {}

inline void Evaluateur_Source_PolyMAC_old::associer_domaines(const Domaine_dis_base& domaine_vdf, const Domaine_Cl_dis_base& domaine_cl_vdf)
{
  la_domaine = ref_cast(Domaine_PolyMAC_old, domaine_vdf);
  la_zcl = ref_cast(Domaine_Cl_PolyMAC_old, domaine_cl_vdf);
  completer();
}

#endif
