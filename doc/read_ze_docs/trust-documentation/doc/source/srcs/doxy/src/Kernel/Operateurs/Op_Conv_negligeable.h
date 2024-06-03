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

#ifndef Op_Conv_negligeable_included
#define Op_Conv_negligeable_included

#include <Operateur_negligeable.h>
#include <Operateur_Conv_base.h>
#include <TRUST_Ref.h>

class Champ_base;

/*! @brief Classe Op_Conv_negligeable Cette classe represente un opperateur de convection negligeable.
 *
 *     Lorsqu'un operateur de ce type est utilise dans une equation
 *     cela revient a negliger le terme de convection.
 *     Les methodes de modification et de participation a un calcul de
 *     l'operateur sont en fait des appels aux meme methodes de
 *     Operateur_negligeable qui ne font rien.
 *
 * @sa Operateur_negligeable Operateur_Conv_base
 */
class Op_Conv_negligeable: public Operateur_negligeable,
  public Operateur_Conv_base
{
  Declare_instanciable(Op_Conv_negligeable);

public :

  inline void contribuer_au_second_membre(DoubleTab& ) const override { }
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const override { }
  inline void associer_domaine_cl_dis(const Domaine_Cl_dis_base&) override { }
  /* interface {dimensionner,ajouter}_blocs -> ne font rien */
  int  has_interface_blocs() const override { return 1; }
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override { }
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override { }

  inline void mettre_a_jour(double) override;
  void associer_vitesse(const Champ_base& ) override ;
  const Champ_base& vitesse() const;

  void ajouter_flux(const DoubleTab& inconnue, DoubleTab& contribution) const override;
  void calculer_flux(const DoubleTab& inconnue, DoubleTab& flux) const override;

  void check_multiphase_compatibility() const override { };
  void set_incompressible(const int) override { };

protected :

  REF(Champ_base) la_vitesse;
  inline void associer(const Domaine_dis&, const Domaine_Cl_dis&, const Champ_Inc& ) override ;
};

/*! @brief Mise a jour en temps d'un operateur negligeable: NE FAIT RIEN Simple appel a Operateur_negligeable::mettre_a_jour(double)
 *
 * @param (double temps)
 */
inline void Op_Conv_negligeable::mettre_a_jour(double temps)
{
  Operateur_negligeable::mettre_a_jour(temps);
}


/*! @brief Associe divers objets a un operateurs negligeable: NE FAIT RIEN Simple appel a Operateur_negligeable::associer(const Domaine_dis&,
 *
 *                                                      const Domaine_Cl_dis&,
 *                                                      const Champ_Inc&)
 *
 * @param (Domaine_dis& z)
 * @param (Domaine_Cl_dis& zcl)
 * @param (Champ_Inc& ch)
 */
inline void Op_Conv_negligeable::associer(const Domaine_dis& z,
                                          const Domaine_Cl_dis& zcl,
                                          const Champ_Inc& ch)
{
  Operateur_negligeable::associer(z, zcl, ch);
}
#endif
