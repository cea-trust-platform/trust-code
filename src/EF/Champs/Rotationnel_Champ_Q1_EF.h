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

#ifndef Rotationnel_Champ_Q1_EF_included
#define Rotationnel_Champ_Q1_EF_included

//
#include <Champ_Rotationnel.h>
#include <Champ_Fonc_P0_EF.h>
#include <Ref_Champ_Q1_EF.h>


class Rotationnel_Champ_Q1_EF : public Champ_Rotationnel,
  public Champ_Fonc_P0_EF
{
  Declare_instanciable(Rotationnel_Champ_Q1_EF);

public:

  inline const Champ_Q1_EF& mon_champ() const;
  inline void mettre_a_jour(double ) override;
  void associer_champ(const Champ_Q1_EF& );
  void me_calculer(double ) override;

protected:

  REF(Champ_Q1_EF) mon_champ_;
};

inline const Champ_Q1_EF& Rotationnel_Champ_Q1_EF::mon_champ() const
{
  return mon_champ_.valeur();
}

inline void Rotationnel_Champ_Q1_EF::mettre_a_jour(double tps)
{
  me_calculer(tps);
  changer_temps(tps);
}



#endif

