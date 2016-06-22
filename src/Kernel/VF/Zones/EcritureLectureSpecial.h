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
// File:        EcritureLectureSpecial.h
// Directory:   $TRUST_ROOT/src/Kernel/VF/Zones
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EcritureLectureSpecial_included
#define EcritureLectureSpecial_included

#include <Interprete.h>
#include <Motcle.h>
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe EcritureLetureSpecial
//     Classe static qui permet de faire une sauvegarde ou/et une lecture
//   particuliere :
//        un seul fichier binaire contenant les positions + les valeurs
//    Permet de faire une reprise sur un nbre different de proc
// .SECTION voir aussi
//    Si le format est active (en lecture ou en ecriture) sur un probleme,
// il doit l'etre sur tous.
//
//////////////////////////////////////////////////////////////////////////////

class Sortie;
class Entree;
class Zone_VF;
class DoubleTab;
class Champ_base;

class EcritureLectureSpecial : public Interprete
{
  Declare_instanciable(EcritureLectureSpecial);
public:
  Entree& interpreter(Entree& fich);
  static void lecture_special(Champ_base& ch, Entree& fich);
  static void lecture_special(const Zone_VF& zvf, Entree& fich, DoubleTab& val);
  static void ecriture_special(const Champ_base& ch, Sortie& fich);
  static void ecriture_special(const Zone_VF& zvf, Sortie& fich, const DoubleTab& val);

  static int is_lecture_special();
  static int is_ecriture_special(int& special,int& a_faire);

  static int Active;
  static int mode_ecr;
  static int mode_lec;
  static Nom Input;
  static Nom Output;
  static Nom& get_Output();
};

#endif
