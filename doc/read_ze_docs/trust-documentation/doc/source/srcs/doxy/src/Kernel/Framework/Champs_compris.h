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

#ifndef Champs_compris_included
#define Champs_compris_included

#include <TRUST_List.h>
#include <TRUST_Ref.h>
#include <Noms.h>

class Champ_base;

/*! @brief classe Champs_compris Represente un champ compris par un objet de type Equation, Milieu,
 *
 *      Operateur, Source, Traitement_particulier.
 *
 */

class Champs_compris : public Objet_U
{

  Declare_instanciable(Champs_compris);

public :

  // Return the field if found, otherwise raises.
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  // Same thing, but without raising:
  virtual bool has_champ(const Motcle& nom, REF(Champ_base)& ref_champ) const;
  virtual void ajoute_champ(const Champ_base& champ);
  virtual void ajoute_nom_compris(const Nom& nom);
  virtual const Noms liste_noms_compris() const;
  //virtual Noms& liste_noms_compris();

protected :

  LIST(REF(Champ_base)) liste_champs_;
  Noms liste_noms_;
  Noms liste_noms_construits_;

};

// ToDo commenter pour supprimer totalement les exceptions dans Flica5 ou TRUST:
class Champs_compris_erreur
{

public:
  inline Champs_compris_erreur() {}

};
#endif

