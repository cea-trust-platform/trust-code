/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Source_base_included
#define Source_base_included

#include <Champs_compris_interface.h>
#include <TRUSTTabs_forward.h>
#include <Interface_blocs.h>
#include <Champs_compris.h>
#include <Matrice_Bloc.h>
#include <MorEqn.h>
#include <SFichier.h>
#include <Champ_Don.h>

class Probleme_base;
class Domaine_dis;
class Domaine_Cl_dis;
class Matrice_Morse;

/*! @brief classe Source_base Un objet Source_base est un terme apparaissant au second membre d'une
 *
 *      equation. Cette classe est la base de la hierarchie des Sources, une
 *      source est un morceau d'equation donc Source_base herite de MorEqn.
 *
 *
 */

class Source_base: public Champs_compris_interface, public MorEqn, public Objet_U
{
  Declare_base(Source_base);

public:

  virtual DoubleTab& ajouter(DoubleTab&) const;
  virtual DoubleTab& calculer(DoubleTab&) const;
  virtual void mettre_a_jour(double temps);
  virtual void resetTime(double t);
  virtual void completer();
  virtual void dimensionner(Matrice_Morse&) const;
  virtual void dimensionner_bloc_vitesse(Matrice_Morse&) const;
  virtual void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  virtual void contribuer_au_second_membre(DoubleTab&) const;
  virtual int impr(Sortie& os) const;
  // temporaire : associer_domaines sera rendue publique
  inline void associer_domaines_public(const Domaine_dis& zdis, const Domaine_Cl_dis& zcldis) { associer_domaines(zdis,zcldis); }
  virtual int initialiser(double temps);
  virtual void associer_champ_rho(const Champ_base& champ_rho);
  virtual int a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base) &ch_ref) const;
  virtual void contribuer_jacobienne(Matrice_Bloc&, int) const { }

  /* interface {dimensionner,ajouter}_blocs -> cf Equation_base.h */
  virtual int has_interface_blocs() const { return 0; }
  virtual void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = { }) const;
  virtual void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = { }) const;

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  virtual bool has_champ(const Motcle& nom, REF(Champ_base) &ref_champ) const;
  void get_noms_champs_postraitables(Noms& nom, Option opt = NONE) const override;
  /////////////////////////////////////////////////////

  virtual void ouvrir_fichier(SFichier& os, const Nom&, const int flag = 1) const;
  void set_fichier(const Nom&);
  inline void set_description(const Nom& nom) { description_ = nom; }
  inline void set_col_names(const Noms& col_names) { col_names_ = col_names; }
  inline const Nom fichier() const { return out_; }
  inline const Nom description() const { return description_; }

  inline DoubleVect& bilan() { return bilan_; }
  inline DoubleVect& bilan() const { return bilan_; }
  inline Champs_compris& champs_compris() { return champs_compris_; }

  // Liste des champs des sources:
  const LIST(REF(Champ_Don))& champs_don() const { return champs_don_; }

protected:

  virtual void associer_domaines(const Domaine_dis&, const Domaine_Cl_dis&) =0;
  virtual void associer_pb(const Probleme_base&) =0;
  int col_width_ = 0;
  Nom out_;                  // Nom du fichier .out pour l'impression
  Nom description_;
  Noms col_names_;            //nom des colonnes (optionnel)
  mutable DoubleVect bilan_; // Vecteur contenant les valeurs du terme source dans le domaine
  mutable SFichier Flux;
  Champs_compris champs_compris_;
  LIST(REF(Champ_Don)) champs_don_;
};

#endif /* Source_base_included */
