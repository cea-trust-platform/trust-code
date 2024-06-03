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

#ifndef Champ_Generique_Ecart_Type_included
#define Champ_Generique_Ecart_Type_included

#include <Champ_Generique_Statistiques_base.h>
#include <TRUSTTabs_forward.h>
#include <Op_Ecart_type.h>
#include <TRUST_Ref.h>

class Postraitement_base;

/*! @brief class Champ_Generique_Ecart_Type Champ destine a post-traiter un Ecart_Type d un champ generique
 *
 *  La classe porte un operateur statistique "Op_Ecart_Type"
 *
 *  Syntaxe a respecter pour jdd
 *
 *  "nom_champ" Ecart_Type { t_deb "val_tdeb" t_fin "val_tfin"
 *                 source "type_champ_gen" { ...source ref_Champ { Pb_champ "nom_pb" "nom_champ_discret" } }
 *                }
 *  "nom_champ" fixe par utilisateur sera le nom du champ generique
 *  "val_tdeb" et "val_tfin" valeur de debut et fin des statistiques pour ce champ
 *  "type_champ_gen" type d'un champ generique
 *
 */

class Champ_Generique_Ecart_Type : public Champ_Generique_Statistiques_base
{
  Declare_instanciable(Champ_Generique_Ecart_Type);
public:
  const Noms get_property(const Motcle& query) const override;

  inline double temps() const override { return Op_Ecart_Type_.integrale().temps(); }
  inline const Integrale_tps_Champ& integrale() const override { return Op_Ecart_Type_.integrale(); }
  inline const DoubleTab& valeurs() const { return Op_Ecart_Type_.moyenne().integrale()->valeurs(); }
  inline const DoubleTab& valeurs_carre() const { return Op_Ecart_Type_.integrale().valeurs(); }
  inline double dt_integration() const { return Op_Ecart_Type_.moyenne().dt_integration(); }
  inline double dt_integration_carre() const { return Op_Ecart_Type_.dt_integration_carre(); }
  inline Operateur_Statistique_tps_base& Operateur_Statistique() override { return Op_Ecart_Type_; }
  inline const Operateur_Statistique_tps_base& Operateur_Statistique() const override { return Op_Ecart_Type_; }

  const Champ_base&  get_champ(Champ& espace_stockage) const override;
  void completer(const Postraitement_base& post) override;
  void nommer_source() override;

protected:
  Op_Ecart_type Op_Ecart_Type_;
};

#endif
