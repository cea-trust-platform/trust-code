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
// File:        Eval_Dirac_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources/Evaluateurs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Eval_Dirac_VDF_Elem.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>

void Eval_Dirac_VDF_Elem::associer_champs(const Champ_Don& rho,
                                          const Champ_Don& capa,
                                          const Champ_Don& Q)
{
  rho_ref = rho;
  rho_ref_ = rho(0,0);
  Cp = capa;
  Cp_ = capa(0,0);
  la_puissance = ref_cast(Champ_Uniforme,Q.valeur());
  puissance = Q(0,0);
}

void Eval_Dirac_VDF_Elem::mettre_a_jour( )
{
  const Champ_Uniforme& puiss_unif = la_puissance.valeur();
  puissance= puiss_unif(0,0);
  ma_zone = la_zone.valeur().zone();
}

void Eval_Dirac_VDF_Elem::associer_nb_elem_dirac(int n)
{
  nb_dirac = 1./n;
  //Cout << "Nb Dirac = " << n << finl;
}
