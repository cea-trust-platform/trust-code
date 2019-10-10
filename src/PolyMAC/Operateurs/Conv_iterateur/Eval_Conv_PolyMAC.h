/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Eval_Conv_PolyMAC.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs/Conv_iterateur
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Conv_PolyMAC_included
#define Eval_Conv_PolyMAC_included

#include <Evaluateur_PolyMAC.h>
#include <Ref_Champ_Face_PolyMAC.h>

class Champ_Inc_base;

//
// .DESCRIPTION class Eval_Conv_PolyMAC
//
// classe de base des evaluateurs de convection PolyMAC

//
// .SECTION voir aussi Evaluateur_PolyMAC
//


class Eval_Conv_PolyMAC : public Evaluateur_PolyMAC
{

public:

  inline Eval_Conv_PolyMAC();
  inline Eval_Conv_PolyMAC(const Eval_Conv_PolyMAC&);
  void associer(const Champ_Face_PolyMAC& );
  void mettre_a_jour( );
  const Champ_Inc_base& vitesse() const;
  Champ_Inc_base& vitesse();

protected:

  REF(Champ_Face_PolyMAC) vitesse_;
  DoubleTab dt_vitesse;

};

//
//  Fonctions inline de la classe Eval_Conv_PolyMAC
//
// Description:
// constructeur par defaut
inline Eval_Conv_PolyMAC::Eval_Conv_PolyMAC()
{}

inline Eval_Conv_PolyMAC::Eval_Conv_PolyMAC(const Eval_Conv_PolyMAC& eval)
  :Evaluateur_PolyMAC(eval), vitesse_(eval.vitesse_)
{
  dt_vitesse.ref(eval.dt_vitesse);
}



#endif
