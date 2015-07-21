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
// File:        Fluide_Ostwald.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION  class Fluide_Ostwald
//
// Possede les proprietes d'un fluide incompressible plus:
// N : indice de structure
// K : consistance du fluide
// La viscosite dynamique dans ce cas depend de K, N et Grad(U)
// Nouveau fluide. c'est un fluide incompressible non newtonien. Il derive donc
//   de fluide_Incompressible et possede quelques proprietes de plus (K, N)
//   mu depend de K, N, et de grad(U)
// .SECTION voir aussi
//     Fluide_Incompressible
//////////////////////////////////////////////////////////////////////////////

#ifndef Fluide_Ostwald_included
#define Fluide_Ostwald_included

#include <Fluide_Incompressible.h>

class Fluide_Ostwald : public Fluide_Incompressible
{
  Declare_instanciable_sans_constructeur(Fluide_Ostwald);

public :

  Fluide_Ostwald();
  inline const Champ_Don& consistance() const;
  inline Champ_Don& consistance();
  inline const Champ_Don& indice_struct() const;
  inline Champ_Don& indice_struct();

  void set_param(Param& param);
  void creer_champs_non_lus();
  void mettre_a_jour(double);
  int initialiser(const double& temps);
  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);

protected :

  Champ_Don K_;  // consistance lue
  Champ_Don N_;  // indice de structure lue


};


inline const Champ_Don& Fluide_Ostwald::consistance() const
{
  return K_;
}

inline Champ_Don& Fluide_Ostwald::consistance()
{
  return K_;
}

inline const Champ_Don& Fluide_Ostwald::indice_struct() const
{
  return N_;
}

inline Champ_Don& Fluide_Ostwald::indice_struct()
{
  return N_;
}




#endif
