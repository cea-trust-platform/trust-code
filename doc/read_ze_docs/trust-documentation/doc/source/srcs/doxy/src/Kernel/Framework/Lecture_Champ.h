/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Lecture_Champ_included
#define Lecture_Champ_included

#include <TRUST_List.h>
#include <Objet_U.h>
#include <Champ.h>

/*! @brief : class Lecture_Champ
 *
 *  <Description of class Lecture_Champ>
 *
 */
class Lecture_Champ : public Objet_U
{

  Declare_instanciable( Lecture_Champ ) ;

public :

  Entree& lire_champs(Entree& is, LIST(Nom)& noms_champs);
  Champ& champ_lu(const Nom& nom) { return liste_champs(nom); }
  const Champ& champ_lu(const Nom& nom)const { return liste_champs(nom); }
  bool champs_lus() { return champs_lus_; }
  bool champs_lus() const { return champs_lus_; }

protected :

  LIST(Champ) liste_champs;
  bool champs_lus_;
};

#endif /* Lecture_Champ_included */
