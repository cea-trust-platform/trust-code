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

#ifndef Postraitement_base_included
#define Postraitement_base_included

#include <TRUST_Ref.h>
#include <Nom.h>

class Probleme_base;
class Entree;
class Sortie;
class Motcle;
class Param;

/*! @brief Classe de base pour l'ensemble des postraitements.
 *
 * @sa Postraitements
 */

class Postraitement_base : public Objet_U
{
  Declare_base_sans_constructeur(Postraitement_base);
public:
  Postraitement_base();
  virtual void associer_nom_et_pb_base(const Nom&, const Probleme_base&);
  const Nom& le_nom() const override;
  virtual void set_param(Param& param)=0;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  virtual void postraiter(int forcer) = 0;
  virtual void mettre_a_jour(double temps);
  virtual void init() {}
  virtual void finir() {}
  int sauvegarder(Sortie& os) const override;
  int reprendre(Entree& is) override;
  virtual void completer() = 0;
  virtual void completer_sondes() {}

  enum Format { ASCII, BINAIRE };
  enum Type_Champ { CHAMP=0, STATISTIQUE=1 };
  enum Localisation { SOMMETS=0, ELEMENTS=1, FACES=2 };
  enum Type_Post { ERREUR=-1, ENTIER=0, REEL=1, DOUBLE=2 };

  static const char * const demande_description;

protected:
  Nom le_nom_;
  REF(Probleme_base) mon_probleme;
  double temps_;
};

#endif
