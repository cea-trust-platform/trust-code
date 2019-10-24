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
// File:        Verif_Cl.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <arch.h>
#include <Cond_lim.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Interface du module ThHyd.
//    Contient 2 fonctions:
//      int tester_compatibilite_hydr_thermique(const Zone_Cl_dis& , const Zone_Cl_dis& )
//      int tester_compatibilite_hydr_concentration(const Zone_Cl_dis& , const Zone_Cl_dis& )
//    qui servent a tester la coherence des conditions aux limites
//    et les 2 fonctions
//      int message_erreur_[therm|conc](const Cond_lim& , const Cond_lim& , int& )
//    qui affiche un message d'erreur pour la compatibilite hyd/[therm|conc]
// .SECTION voir aussi
//    Fonction de librairie hors classe
//////////////////////////////////////////////////////////////////////////////

class Zone_Cl_dis;

// Fonctions qui servent a tester la coherence des conditions aux limites

int tester_compatibilite_hydr_thermique(const Zone_Cl_dis& , const Zone_Cl_dis& );

int message_erreur_therm(const Cond_lim& , const Cond_lim& , int& );

int tester_compatibilite_hydr_concentration(const Zone_Cl_dis& , const Zone_Cl_dis& )  ;

int message_erreur_conc(const Cond_lim& , const Cond_lim& , int& );
