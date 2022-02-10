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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Courant_maille_Champ_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Courant_maille_Champ_Face_included
#define Courant_maille_Champ_Face_included


#include <Champ_Fonc_Face.h>
#include <Ref_Champ_Face.h>
#include <Ref_Schema_Temps_base.h>

//.DESCRIPTION  classe Courant_maille_Champ_Face
//

class Courant_maille_Champ_Face : public Champ_Fonc_Face

{

  Declare_instanciable(Courant_maille_Champ_Face);

public:

  inline const Champ_Face& vitesse() const
  {
    return vitesse_.valeur();
  };
  void mettre_a_jour(double ) override;
  void associer_champ(const Champ_Face&, const Schema_Temps_base& );

protected:

  REF(Champ_Face) vitesse_;
  REF(Schema_Temps_base) sch_;
};

#endif
