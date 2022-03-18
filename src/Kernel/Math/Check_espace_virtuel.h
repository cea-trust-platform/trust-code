/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Check_espace_virtuel.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Check_espace_virtuel_H
#define Check_espace_virtuel_H

#include <TRUSTTabs_forward.h>
#include <arch.h>

// Renvoie 1 si l'espace virtuel de v est a jour, 0 sinon
int check_espace_virtuel_vect(const DoubleVect& v);
int check_espace_virtuel_vect(const IntVect& v);

// Remplit l'espace virtuel du vecteur v avec des valeurs invalides (ou specifiees)
void remplir_items_non_calcules(DoubleVect& v, double valeur = 0.);
void assert_invalide_items_non_calcules(DoubleVect& v, double valeur = 0.);
//void assert_invalide_items_non_calcules(IntVect & v, int valeur);
void declare_espace_virtuel_invalide(DoubleVect& v);
void declare_espace_virtuel_invalide(IntVect& v);

// Provoque une erreur (sortie du code par exit()) si l'espace virtuel
// n'est pas a jour.
void assert_espace_virtuel_vect(const DoubleVect& v);
void assert_espace_virtuel_vect(const IntVect& v);
#endif
