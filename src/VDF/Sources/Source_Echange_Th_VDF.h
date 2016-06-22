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
// File:        Source_Echange_Th_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Source_Echange_Th_VDF_included
#define Source_Echange_Th_VDF_included

#include <Terme_Source_VDF_base.h>
#include <Eval_Echange_Himp_VDF_Elem.h>
#include <ItSouVDFEl.h>

//!  Source volumique d'echange thermique avec un autre probleme de meme domaine
/**
   dT/dt = h * (T'-T) / rho / Cp

   Lecture des arguments :
   Echange_thermique_h_imp nom_probleme h

*/


declare_It_Sou_VDF_Elem(Eval_Echange_Himp_VDF_Elem)

class Source_Echange_Th_VDF : public  Terme_Source_VDF_base
{
  ////Declare_instanciable_sans_constructeur(Source_Echange_Th_VDF);;
  Declare_instanciable_sans_constructeur(Source_Echange_Th_VDF);

public:
  inline Source_Echange_Th_VDF()
    : Terme_Source_VDF_base(It_Sou_VDF_Elem(Eval_Echange_Himp_VDF_Elem)()) {}
  //! Methode virtuelle pure de Source_base
  void associer_zones(const Zone_dis&, const Zone_Cl_dis& );
  //! Methode virtuelle pure de Source_base
  void associer_pb(const Probleme_base&);
  void mettre_a_jour(double temps)
  {
    ;
  }
private:
  double h; //!< Valeur du coefficient d'echange
  REF(Probleme_base) pb_voisin; //!< Le probleme avec lequel se fait l'echange
};

#endif

