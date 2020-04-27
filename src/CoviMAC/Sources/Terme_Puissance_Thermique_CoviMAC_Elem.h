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
// File:        Terme_Puissance_Thermique_CoviMAC_Elem.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Sources
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Terme_Puissance_Thermique_CoviMAC_Elem_included
#define Terme_Puissance_Thermique_CoviMAC_Elem_included


#include <Terme_Puissance_Thermique_CoviMAC_base.h>
#include <Eval_Puiss_Th_CoviMAC_Elem.h>
#include <ItSouCoviMACEl.h>

declare_It_Sou_CoviMAC_Elem(Eval_Puiss_Th_CoviMAC_Elem)

//.DESCRIPTION class Terme_Puissance_Thermique_CoviMAC_Elem
//
// Cette classe represente un terme source de l'equation de la thermique
// du type degagement volumique de puissance thermique
//
//.SECTION
// voir aussi Terme_Puissance_Thermique, Terme_Source_CoviMAC_base

class Terme_Puissance_Thermique_CoviMAC_Elem : public Terme_Puissance_Thermique_CoviMAC_base
{
  Declare_instanciable_sans_constructeur(Terme_Puissance_Thermique_CoviMAC_Elem);

public:

  inline Terme_Puissance_Thermique_CoviMAC_Elem();
  void associer_zones(const Zone_dis&, const Zone_Cl_dis& );
  void associer_pb(const Probleme_base& );
  virtual void mettre_a_jour(double temps)
  {
    Terme_Puissance_Thermique::mettre_a_jour(temps);
  }

protected:

};


//
// Fonctions inline de la classe Terme_Puissance_Thermique_CoviMAC_Elem
//

inline Terme_Puissance_Thermique_CoviMAC_Elem::Terme_Puissance_Thermique_CoviMAC_Elem()
  : Terme_Puissance_Thermique_CoviMAC_base(It_Sou_CoviMAC_Elem(Eval_Puiss_Th_CoviMAC_Elem)())
{
}


#endif

