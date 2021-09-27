/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Modifier_pour_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Modifier_pour_QC
#define Modifier_pour_QC

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     Fonctions destinees a multiplier ou diviser un tableau de valeurs par un vecteur.
//     Ces fonctions sont utilisees ici pour multiplier ou diviser un tableau par rho
//     pour le cas ou le milieu est un Fluide_Quasi_Compressible
//
// .SECTION
//////////////////////////////////////////////////////////////////////////////

class DoubleVect;
class Milieu_base;
class Fluide_Quasi_Compressible;

void multiplier_diviser_rho(DoubleVect& tab,const Fluide_Quasi_Compressible& le_fluide,int diviser=0);
void diviser_par_rho_si_qc(DoubleVect& val,const Milieu_base& mil);
void multiplier_par_rho_si_qc(DoubleVect& val,const Milieu_base& mil);

#endif
