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

#ifndef Champ_Post_Operateur_Eqn_included
#define Champ_Post_Operateur_Eqn_included

#include <Champ_Generique_Operateur_base.h>
#include <TRUST_Ref.h>
#include <Operateur.h>

class Equation_base;

/*! @brief class Champ_Post_Operateur_Eqn Champ destine a post-traiter le gradient d un champ generique
 *
 *  La classe porte un operateur statistique "gradient"
 *
 *  Syntaxe a respecter pour jdd
 *
 *  nom_choisi operateur_eqn
 *    {
 *      sources { refchamp { pb_champ nom_pb nom_inconnue } }
 *      numero_source/numero op 1
 *    }
 * Il faut entrer le nom de l'inconnue et pas celui de l'equation
 * Le numero des termes sources est celui du jdd, bien respecter l'ordre
 * Pour les operateurs : 0 diffusion, 1 convection
 *
 */

class Champ_Post_Operateur_Eqn : public Champ_Generique_Operateur_base
{

  Declare_instanciable(Champ_Post_Operateur_Eqn);

public:

  const Noms get_property(const Motcle& query) const override;
  Entity  get_localisation(const int index = -1) const override;
  const Champ_base&  get_champ(Champ& espace_stockage) const override;
  const Champ_base&  get_champ_without_evaluation(Champ& espace_stockage) const override;


  const Operateur_base& Operateur() const override;
  Operateur_base& Operateur() override;
  void completer(const Postraitement_base& post) override;
  void nommer_source() override;
  //virtual Entree &   lire(const Motcle & motcle, Entree & is);
  void set_param(Param& param) override;
  void verification_cas_compo() const;
  const Champ_base&  get_champ_compo_without_evaluation(Champ& espace_stockage) const;

protected:
  int numero_source_,numero_op_, numero_masse_;
  REF(Equation_base) ref_eq_;
  int sans_solveur_masse_;
  Entity localisation_inco_=NODE;
  int compo_;                            //Pour identifier la composante a recuperer
};

#endif
