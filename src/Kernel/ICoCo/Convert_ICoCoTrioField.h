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
// File:        Convert_ICoCoTrioField.h
// Directory:   $TRUST_ROOT/src/Kernel/ICoCo
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

// ICoCo file common to several codes
// ICoCoTrioField.h
// version 1.2 10/05/2010

#ifndef Convert_ICoCoTrioField_included
#define Convert_ICoCoTrioField_included

#include <ICoCoTrioField.h>
#include <arch.h>
class Champ_Generique_base;
class Domaine;
class DoubleTab;
class ArrOfDouble;
class ArrOfInt;
void affecte_double_avec_doubletab(double** p, const ArrOfDouble& trio);
void affecte_int_avec_inttab(True_int** p, const ArrOfInt& trio);
#include <ICoCoMEDField.hxx>
#ifndef NO_MEDFIELD
ICoCo::MEDField build_medfield(ICoCo::TrioField&);
ICoCo::MEDField build_medfield(const Champ_Generique_base&);
#endif
ICoCo::TrioField build_triofield(const Champ_Generique_base&);

#endif
