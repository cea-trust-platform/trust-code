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
// File:        Debog.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Debog_included
#define Debog_included

class DoubleVect;
class IntVect;
class Champ_Inc;
class Champ_Inc_base;
class MD_Vector;
class Nom;
class Matrice_Base;

class Debog
{
public:
  static int active();
  static void verifier(const char* const msg, double);
  static void verifier(const char* const msg, int);
  static void verifier(const char* const msg, const DoubleVect&);
  static void verifier(const char* const msg, const IntVect&);
  static void verifier_bord(const char* const msg, const DoubleVect& arr, int num_deb);
  static void verifier(const char* const msg, const Champ_Inc&);
  static void verifier(const char* const msg, const Champ_Inc_base&);
  static void verifier_indices_items(const char* const msg, const MD_Vector&, const IntVect&);
  static void set_nom_pb_actuel(const Nom& nom);

  static void verifier_getref(const char* const msg, double val, double& refval);
  static void verifier_getref(const char* const msg, int val, int& refval);
  static void verifier_getref(const char* const msg, const DoubleVect& val, DoubleVect& refval);
  static void verifier_getref(const char* const msg, const IntVect& val, IntVect& refval);

  static void verifier_Mat_elems(const char* const msg, const Matrice_Base& la_matrice);
  static void verifier_matrice(const char * const msg, const Matrice_Base&,
                               const MD_Vector& md_lignes, const MD_Vector& md_colonnes);
};

#endif
