/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        NSCBC.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef NSCBC_included
#define NSCBC_included


#include <Neumann.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe
// .SECTION voir aussi
//     Neumann
//////////////////////////////////////////////////////////////////////////////
class NSCBC : public Neumann
{

  Declare_instanciable(NSCBC);

public:

  void mettre_a_jour(double temps);
  virtual void calculer_contribution_valeurs_propres(double temps);
  int compatible_avec_eqn(const Equation_base& eqn) const;

  inline Champ_front& flux_aretes();
  inline const Champ_front& flux_aretes() const ;
  inline DoubleTab& val_propre_face();
  inline const DoubleTab& val_propre_face() const ;
  inline DoubleTab& val_propre_arete();
  inline const DoubleTab& val_propre_arete() const ;

protected:
  Champ_front flux_aretes_;
  DoubleTab val_propre_face_;
  DoubleTab val_propre_arete_;

};

inline DoubleTab& NSCBC::val_propre_face()
{
  return val_propre_face_;
}

inline const DoubleTab& NSCBC::val_propre_face() const
{
  return val_propre_face_;
}

inline DoubleTab& NSCBC::val_propre_arete()
{
  return val_propre_arete_;
}

inline const DoubleTab& NSCBC::val_propre_arete() const
{
  return val_propre_arete_;
}

inline Champ_front& NSCBC::flux_aretes()
{
  return flux_aretes_;
}

inline const Champ_front& NSCBC::flux_aretes() const
{
  return flux_aretes_;
}


#endif
