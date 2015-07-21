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
// File:        Array_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Array_base_included
#define Array_base_included

#include <Objet_U.h>

// .DESCRIPTION: classe vide servant de base a tous les tableaux.
//  (permet de passer un type generique quel que soit le sous-type du tableau
//   par exemple Domaine::creer_tableau_sommets())
class Array_base : public Objet_U
{
  Declare_base_sans_constructeur_ni_destructeur(Array_base);
public:
  // Options de resize:
  //  NOCOPY_NOINIT: ne pas copier les anciennes valeurs, ne pas initialiser les nouvelles
  //  COPY_NOINIT: copier les anciennes, ne pas initialiser les nouvelles
  //  COPY_INIT: copier les anciennes et initialiser les nouvelles a zero
  enum Resize_Options { NOCOPY_NOINIT = 0, COPY_NOINIT = 1, COPY_INIT = 2 };
};
#endif
