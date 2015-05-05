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
// File:        Modifier_nut_pour_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Modifier_nut_pour_QC
#define Modifier_nut_pour_QC

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Modifier_nut_pour_QC
//     Cette classe permet de faire la conversion de la viscosite cinematique turbulente nu_t
//       en viscosite dynamique turbulente mu_t dans le cas des equations compressibles.
//     Pour cela il suffit de tester si l'on est dans un cas quasi-compressible :
//       Si c'est le cas, on multiplie nu_t par rho.
//     Cette conversion ne doit etre faite que dans le cas ou l'on utilise un modele sous-maille
//       type LES puisque dans ce cas on renvoit un nu_t.
//       Par contre en simulation RANS, puisque l'on rentre les bonnes grandeurs turbulentes
//       multipliee par rho, on obtient bien au final un mu_t et l'on n'a pas besoin de faire
//       de conversion.
//
// .SECTION
//////////////////////////////////////////////////////////////////////////////

#include <Modifier_pour_QC.h>
class Mod_turb_hyd_base;

void Correction_nut_et_cisaillement_paroi_si_qc(Mod_turb_hyd_base& mod);

#endif
