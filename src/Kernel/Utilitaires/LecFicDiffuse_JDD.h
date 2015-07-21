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
// File:        LecFicDiffuse_JDD.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef LecFicDiffuse_JDD_included
#define LecFicDiffuse_JDD_included

#include <Lec_Diffuse_base.h>
#include <EChaine.h>

class Objet_U;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Cette classe implemente les operateurs et les methodes virtuelles de la classe EFichier de la facon suivante :
//    Le fichier a lire est physiquement localise sur le disque de la machine hebergeant la tache maitre de l'application Trio-U (le processus de rang 0 dans le groupe "tous")
//    et chaque item lu dans ce fichier est diffuse a tous les autres processus du groupe tous.
//    Il en est de meme pour les methodes d'inspection de l'etat d'un fichier.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class LecFicDiffuse_JDD : public Lec_Diffuse_base
{
  Declare_instanciable_sans_constructeur(LecFicDiffuse_JDD);
  // le maitre lit le fichier et propage l'information
public:
  LecFicDiffuse_JDD();
  LecFicDiffuse_JDD(const char* name,IOS_OPEN_MODE mode=ios::in);
  int ouvrir(const char* name,IOS_OPEN_MODE mode=ios::in);
  Entree& get_entree_master();
protected:
  EChaine chaine_;
};
#endif
