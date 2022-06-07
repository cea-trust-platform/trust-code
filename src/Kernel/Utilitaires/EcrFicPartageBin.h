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

#ifndef EcrFicPartageBin_included
#define EcrFicPartageBin_included

#include <EcrFicPartage.h>

class Objet_U;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Ecriture dans un fichier partage
//    Cette classe derive de Ecr_Fic_Par, en utilisant une sortie en binaire.
//    Ce type de fichier est notamment utilise pour la creation d'un fichier de post-traitement unique.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class EcrFicPartageBin : public EcrFicPartage
{
  Declare_instanciable_sans_constructeur_ni_destructeur(EcrFicPartageBin);
  // Chaque PE ecrit dans le meme() fichier
public:
  EcrFicPartageBin(): EcrFicPartage()
  {
    set_bin(1);
  };
  EcrFicPartageBin(const char* name,IOS_OPEN_MODE mode=ios::out)
  {
    set_bin(1);
    ouvrir(name, mode);
  };
};


#endif
