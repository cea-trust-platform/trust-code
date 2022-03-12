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
// File:        Sources.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sources_included
#define Sources_included

#include <Source.h>

Declare_liste(Source);

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     class Sources
//     Sources represente une liste de Source.
//     Une equation contient un membres Equation_base::les_sources de type
//     Sources. Un objet Sources est un morceaux d'equation, cette classe
//     herite donc de MorEqn.
// .SECTION voir aussi
//      Source MorEqn Equation
//////////////////////////////////////////////////////////////////////////////
class Sources : public LIST(Source), public MorEqn
{
  Declare_instanciable(Sources);

public:
  DoubleTab& ajouter(DoubleTab& ) const;
  DoubleTab& calculer(DoubleTab& ) const;
  void contribuer_jacobienne(Matrice_Bloc& matrice, int n) const;
  void mettre_a_jour(double temps);
  void completer();
  void dimensionner(Matrice_Morse&) const;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  //void contribuer_au_second_membre(DoubleTab& ) const ;
  virtual void associer_champ_rho(const Champ_base& champ_rho);
  virtual int a_pour_Champ_Fonc(const Motcle& mot,
                                REF(Champ_base)& ch_ref) const;
  int impr(Sortie&) const;
  int initialiser(double temps);
  void check_multiphase_compatibility() const override;
};

#endif
