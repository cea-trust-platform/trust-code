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
// File:        Traitement_particulier_NS_Pression.h
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Traitement_particulier
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_Pression_included
#define Traitement_particulier_NS_Pression_included

#include <Traitement_particulier_NS_base.h>
#include <IntTab.h>
#include <Champ_Don.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//
//
// .SECTION voir aussi
//      Navier_Stokes_Turbulent, Traitement_particulier_base,
//
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_Pression : public Traitement_particulier_NS_base
{
  Declare_base(Traitement_particulier_NS_Pression);

public :

  Entree& lire(Entree& ) override;
  void associer_eqn(const Equation_base& ) override;
  void preparer_calcul_particulier(void) override ;
  void post_traitement_particulier(void) override ;
  void en_cours_de_resolution(int , DoubleTab&, DoubleTab& ,double) override ;
  inline void sauver_stat(void)  const override ;
  inline void reprendre_stat(void) override  ;
  inline int a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const  ;
  inline int comprend_champ(const Motcle& mot) const  ;


protected :



};

#endif

inline void Traitement_particulier_NS_Pression::sauver_stat(void)  const
{
  ;
}

inline void Traitement_particulier_NS_Pression::reprendre_stat(void)
{
  ;
}

inline int Traitement_particulier_NS_Pression::a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const
{
  return 0 ;
}

inline int Traitement_particulier_NS_Pression::comprend_champ(const Motcle& mot) const
{
  return 0 ;
}
