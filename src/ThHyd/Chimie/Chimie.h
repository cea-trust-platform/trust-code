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

#ifndef Chimie_included
#define Chimie_included

#include <Objet_U.h>
#include <Champ.h>
#include <Champ_Don.h>
#include <Champs_compris_interface.h>
#include <List.h>
#include <Ref_Probleme_base.h>
#include <Motcle.h>
#include <Reaction.h>

class Probleme_base;
class Discretisation_base;
class Zone_dis_base;
class Motcles;

class Chimie : public Objet_U
{
  Declare_instanciable(Chimie);
public:
  virtual void completer(const Probleme_base& pb);
  virtual int preparer_calcul();
  void mettre_a_jour(double temps);
  double calculer_pas_de_temps() const;
  void discretiser(const Probleme_base&);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  int sauvegarder(Sortie& ) const override;
  int reprendre(Entree& ) override;

protected:
  LIST(Reaction) reactions_;
  REF(Probleme_base) pb_;
  Motcles alias;
  VECT(REF(Champ_Inc_base)) liste_Y_,liste_ai_,liste_nd10_,liste_ngrains_,liste_C_;
  int nb_grains_;
  DoubleTab Puissance_volumique_;
  int modele_micro_melange_;
  double constante_modele_micro_melange_;
  Motcle espece_en_competition_micro_melange_;
  int marqueur_espece_en_competition_micro_melange_;
};
#endif
