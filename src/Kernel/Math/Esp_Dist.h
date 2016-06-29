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
// File:        Esp_Dist.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Esp_Dist_included
#define Esp_Dist_included

#include <assert.h>
#include <ArrOfInt.h>
#include <Vect_Descripteur.h>



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Espace distant utilise pour la gestion des tableaux distribues
//    Cette classe permet de decrire un espace distant d'un vecteur de donnees.
//    Elle porte un int representant le numero de processeur a qui est destine la zone decrite.
//    La description de cet espace distant consiste en un vecteur d'entiers contenant les indices des elements a envoyer.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class Esp_Dist : public ArrOfInt
{

  Declare_instanciable_sans_constructeur(Esp_Dist);
public:
  Esp_Dist();

private:
  int PE_voisin_;
  VECT(Descripteur) desc_ed_;
};
#endif
