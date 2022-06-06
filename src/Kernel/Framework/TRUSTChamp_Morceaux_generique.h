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

#ifndef TRUSTChamp_Morceaux_generique_included
#define TRUSTChamp_Morceaux_generique_included

#include <List_Ref_Sous_Zone.h>
#include <Ref_Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Champ_Don_base.h>
#include <Probleme_base.h>
#include <Vect_Parser_U.h>
#include <Ref_Domaine.h>
#include <Domaine.h>

enum class Champ_Morceaux_Type { FONC , FONC_TXYZ , FONC_TABULE , UNIFORME };

template <Champ_Morceaux_Type _TYPE_>
class TRUSTChamp_Morceaux_generique : public Champ_Don_base
{
public:

  Champ_base& affecter_(const Champ_base& ch) override;

  DoubleVect& valeur_a(const DoubleVect& position, DoubleVect& valeurs) const override;

  DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& valeurs, int le_poly) const override;

  double valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const override;

  DoubleTab& valeur_aux(const DoubleTab& positions, DoubleTab& valeurs) const override;

  DoubleVect& valeur_aux_compo(const DoubleTab& positions, DoubleVect& valeurs, int ncomp) const override;

  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const override;

  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& valeurs, int ncomp) const override;

  void mettre_a_jour(double temps) override;

  int initialiser(const double temps) override;

  const REF(Domaine)& domaine() const { return mon_domaine; }
  REF(Domaine)& domaine() { return mon_domaine; }
  const LIST(REF(Sous_Zone))& sous_zones() const { return les_sous_zones; }
  LIST(REF(Sous_Zone))& sous_zones() { return les_sous_zones; }

protected:
  LIST(REF(Sous_Zone)) les_sous_zones;
  REF(Domaine) mon_domaine;
  REF(Probleme_base) ref_pb;
  VECT(Parser_U) parser; /* vecteur de tous les parsers */
  Nom nom_champ_parametre_; /* nom du champ dont les fonctions ont parametres */
  IntTab parser_idx; /* parser_idx(i, j) : parser a appeller pour calculer la composante j du champ a la maille i */

  void interprete_get_domaine(const Nom& nom);

  void creer_tabs(const int);
  Entree& complete_readOn(const int dim, const Nom& qsj, Entree& is, Nom& nom);

};

#include <TRUSTChamp_Morceaux_generique.tpp>

#endif /* TRUSTChamp_Morceaux_generique_included */
