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
// File:        Problemes_Scalaires_Passifs.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Problemes_Scalaires_Passifs_inclus
#define Problemes_Scalaires_Passifs_inclus

#include <MacPbeqsup.h>

#include <Pb_Thermohydraulique.h>
#include <Pb_Conduction.h>
#include <Pb_Thermohydraulique_QC.h>

#include <Pb_Hydraulique_Concentration.h>
#include <Pb_Thermohydraulique_Concentration.h>

declare_instanciable_pb_eq_sup(Pb_Thermohydraulique_Scalaires_Passifs,Pb_Thermohydraulique)
declare_instanciable_pb_eq_sup(Pb_Conduction_Scalaires_Passifs,Pb_Conduction)
declare_instanciable_pb_eq_sup(Pb_Thermohydraulique_QC_fraction_massique,Pb_Thermohydraulique_QC)

declare_instanciable_pb_eq_sup(Pb_Hydraulique_Concentration_Scalaires_Passifs,Pb_Hydraulique_Concentration)
declare_instanciable_pb_eq_sup(Pb_Thermohydraulique_Concentration_Scalaires_Passifs,Pb_Thermohydraulique_Concentration)

#endif
