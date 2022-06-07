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


#ifndef LecFicDistribueBin_included
#define LecFicDistribueBin_included

#include <LecFicDistribue.h>

class Objet_U;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Lecture dans un fichier au format binaire.
//    Cette classe implemente les operateurs et les methodes virtuelles de la classe EFichier de la facon suivante :
//    Il y a autant de fichiers que de processus, physiquement localises sur le disque de la machine hebergeant la tache maitre de l'applicatin Trio-U (le processus de rang 0 dans le groupe "tous").
//    Le processus maitre lit tour a tour un item dans chacun des fichiers et l'envoie au processus correspondant.
//    Il en est de meme pour les methodes d'inspection de l'etat d'un fichier.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class LecFicDistribueBin : public LecFicDistribue
{
  Declare_instanciable_sans_constructeur(LecFicDistribueBin);
  // le maitre lit le fichier et propage l'information
public:
  LecFicDistribueBin()
  {
    set_bin(1);
  };
  LecFicDistribueBin(const char* name,IOS_OPEN_MODE mode=ios::in)
  {
    set_bin(1);
    ouvrir(name,mode);
  };
};

#endif
