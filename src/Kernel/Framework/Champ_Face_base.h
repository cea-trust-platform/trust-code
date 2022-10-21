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

#ifndef Champ_Face_base_included
#define Champ_Face_base_included

#include <Champ_Inc_base.h>
#include <Ref_Zone_VF.h>
#include <Zone_VF.h>

class Champ_Face_base : public Champ_Inc_base
{
  Declare_base(Champ_Face_base) ;
public:
  void associer_zone_dis_base(const Zone_dis_base&) override;
  const Zone_dis_base& zone_dis_base() const override { return ref_zone_vf_.valeur(); }
  const Zone_VF& zone_vf() const { return ref_zone_vf_.valeur(); }
  //tableaux de correspondance lies aux CLs : fcl(f, .) = { type de CL, num de la CL, indice de la face dans la CL }
  //types de CL : 0 -> pas de CL
  //              1 -> Neumann ou Neumann_homogene
  //              2 -> Navier
  //              3 -> Dirichlet
  //              4 -> Dirichlet_homogene
  inline const IntTab& fcl() const
  {
    if (!fcl_init_) init_fcl();
    return fcl_;
  }

protected:
  REF(Zone_VF) ref_zone_vf_;
  void init_fcl() const;
  mutable IntTab fcl_;
  mutable int fcl_init_ = 0;
};

#endif /* Champ_Face_base_included */
