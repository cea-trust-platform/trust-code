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

#ifndef Neumann_val_ext_included
#define Neumann_val_ext_included

#include <Neumann.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Neumann_val_ext
//    Cette classe est la classe de base de la hierarchie des conditions
//    aux limites de type Neumann_val_ext.
//    Une condition aux limites de type Neumann_val_ext impose la valeur de la derivee
//    d'un champ inconnue sur une frontiere, ce qui correspond a:
//      - flux impose pour l'equation de transport d'un scalaire
//      - contrainte imposee pour l'equation de quantite de mouvement
// .SECTION voir aussi
//     Cond_lim_base Neumann_val_ext_homogene
//////////////////////////////////////////////////////////////////////////////
class Neumann_val_ext : public Neumann
{

  Declare_base(Neumann_val_ext);

public:
  virtual const DoubleTab& tab_ext() const = 0;
  virtual DoubleTab& tab_ext() = 0;

  virtual double val_ext(int i) const = 0;
  virtual double val_ext(int i,int j) const = 0;

};

#endif
