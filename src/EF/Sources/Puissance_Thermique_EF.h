/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Puissance_Thermique_EF.h
// Directory:   $TRUST_ROOT/src/EF/Sources
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Puissance_Thermique_EF_included
#define Puissance_Thermique_EF_included



#include <Terme_Puissance_Thermique.h>
#include <Terme_Source_EF_base.h>
#include <Eval_Puiss_Th_EF.h>
#include <It_Sou_EF.h>

Declare_It_Sou_EF(Eval_Puiss_Th_EF);

//.DESCRIPTION class Puissance_Thermique_EF
//
// Cette classe represente un terme source de l'equation de la thermique
// du type degagement volumique de puissance thermique
//
//.SECTION
// voir aussi Terme_Puissance_Thermique, Terme_Source_EF_base


class Puissance_Thermique_EF : public Terme_Puissance_Thermique,
  public Terme_Source_EF_base
{
  Declare_instanciable_sans_constructeur(Puissance_Thermique_EF);

public:

  inline Puissance_Thermique_EF();
  void associer_zones(const Zone_dis&, const Zone_Cl_dis& );
  void associer_pb(const Probleme_base& );
  virtual void mettre_a_jour(double temps)
  {
    Terme_Puissance_Thermique::mettre_a_jour(temps);
  }

};


//
// Fonctions inline de la classe Puissance_Thermique_EF
//

inline Puissance_Thermique_EF::Puissance_Thermique_EF()
  : Terme_Puissance_Thermique(),Terme_Source_EF_base(It_Sou_EF(Eval_Puiss_Th_EF)())
{
}


#endif

