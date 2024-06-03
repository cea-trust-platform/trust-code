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

#ifndef Champ_Fonc_base_included
#define Champ_Fonc_base_included

#include <Champ_Don_base.h>
#include <TRUST_Ref.h>

class Domaine_dis_base;
class MD_Vector;
class Domaine_VF;
class Domaine;

/*! @brief classe Champ_Fonc_base Classe de base des champs qui sont fonction d'une grandeur calculee
 *
 *      au cours du temps
 *
 * @sa Champ_base Champ_Don_base, Classe abstraite., Methodes abstraites:, void mettre_a_jour(double temps), void associer_domaine_dis_base(const Domaine_dis_base&), const Domaine_dis_base& domaine_dis_base() const
 */
class Champ_Fonc_base : public Champ_Don_base
{
  Declare_base(Champ_Fonc_base);
public:
  void mettre_a_jour(double temps) override;
  int fixer_nb_valeurs_nodales(int nb_noeuds) override;
  int reprendre(Entree&) override;
  int sauvegarder(Sortie&) const override;

  Champ_base& affecter_(const Champ_base&) override;
  Champ_base& affecter_compo(const Champ_base&, int compo) override;
  virtual int remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const;
  virtual int remplir_coord_noeuds_et_polys_compo(DoubleTab&, IntVect&, int) const;
  virtual DoubleTab& remplir_coord_noeuds(DoubleTab&) const;
  virtual DoubleTab& remplir_coord_noeuds_compo(DoubleTab&, int) const;
  DoubleTab& valeur_aux(const DoubleTab&, DoubleTab&) const override;
  const Domaine& domaine() const;
  int a_un_domaine_dis_base() const override { return 1; }
  // Obsolete method: signature changed in order to generate a compiler error if old code is not removed
  virtual void creer_espace_distant(int dummy) { }

  void associer_domaine_dis_base(const Domaine_dis_base&) override;
  const Domaine_dis_base& domaine_dis_base() const override;
  virtual const Domaine_VF& domaine_vf() const;

protected:
  // Par defaut on initialise les valeurs a zero
  virtual void creer_tableau_distribue(const MD_Vector&, Array_base::Resize_Options = Array_base::COPY_INIT);
  REF(Domaine_VF) le_dom_VF;
};

#endif /* Champ_Fonc_base_included */
