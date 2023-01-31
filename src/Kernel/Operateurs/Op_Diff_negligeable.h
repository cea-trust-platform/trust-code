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

#ifndef Op_Diff_negligeable_included
#define Op_Diff_negligeable_included

#include <Operateur_negligeable.h>
#include <Operateur_Diff_base.h>
#include <TRUST_Ref.h>

class Champ_base;

/*! @brief Classe Op_Diff_negligeable Cette classe represente un operateur de diffusion negligeable.
 *
 *     Lorsqu'un operateur de ce type est utilise dans une equation
 *     cela revient a negliger le terme de diffusion.
 *     Les methodes de modification et de participation a un calcul de
 *     l'operateur sont en fait des appels aux meme methodes de
 *     Operateur_negligeable qui ne font rien.
 *
 *
 * @sa Operateur_Diff_base Operateur_negligeable
 */

class Op_Diff_negligeable: public Operateur_negligeable,
  public Operateur_Diff_base
{
  Declare_instanciable(Op_Diff_negligeable);

public :

  inline DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const override;
  inline DoubleTab& calculer(const DoubleTab&, DoubleTab& ) const override;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  inline void contribuer_au_second_membre(DoubleTab& ) const override;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const override;
  inline void dimensionner(Matrice_Morse& ) const override;
  inline void mettre_a_jour(double) override;
  void associer_diffusivite(const Champ_base& ) override ;
  const Champ_base& diffusivite() const override;
  inline void associer_champ_masse_volumique(const Champ_base&) override;
  void calculer_pour_post(Champ& espace_stockage,const Nom& option, int comp) const override;
  Motcle get_localisation_pour_post(const Nom& option) const override;

  void ajouter_flux(const DoubleTab& inconnue, DoubleTab& contribution) const override;
  void calculer_flux(const DoubleTab& inconnue, DoubleTab& flux) const override;

  int has_interface_blocs() const override
  {
    return 1;
  };
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = { }) const override { };
  void ajouter_blocs(matrices_t matrices, DoubleTab& resu, const tabs_t& semi_impl = { }) const override { };
  void check_multiphase_compatibility() const override { };

protected :

  REF2(Champ_base) la_diffusivite; inline void associer(const Domaine_dis&, const Domaine_Cl_dis&, const Champ_Inc& ) override ;
};

class Op_Dift_negligeable: public Op_Diff_negligeable
{
  Declare_instanciable(Op_Dift_negligeable);
};


/*! @brief Ajoute la contribution de l'operateur a un tableau passe en parametre.
 *
 * Simple appel a Operateur_negligeable::ajouter(const DoubleTab&,DoubleTab&)
 *
 * @param (DoubleTab& x) le tableau sur lequel on applique l'operateur
 * @param (DoubleTab& y) tableau auquel on ajoute la contribution de l'operateur
 * @return (DoubleTab&) le parametre d'entree y non modifie
 */
inline DoubleTab&
Op_Diff_negligeable::ajouter(const DoubleTab& x, DoubleTab& y) const
{
  return Operateur_negligeable::ajouter(x,y);
}


/*! @brief Initialise le parametre tableau avec la contribution de l'operateur negligeable: initialise le tableau a ZERO.
 *
 *     Simple appel a Operateur_negligeable::(calculer(const DoubleTab&, DoubleTab&)
 *
 * @param (DoubleTab& x) le tableau sur lequel on applique l'operateur
 * @param (DoubleTab& y) tableau dans lequel stocke la contribution de l'operateur
 * @return (DoubleTab&) le tableau d'entree y mis a zero
 */
inline DoubleTab&
Op_Diff_negligeable::calculer(const DoubleTab& x, DoubleTab& y) const
{
  return Operateur_negligeable::calculer(x,y);
}

/*! @brief Mise a jour en temps d'un operateur negligeable: NE FAIT RIEN Simple appel a Operateur_negligeable::mettre_a_jour(double)
 *
 * @param (double temps)
 */

/*! @brief on assemble la matrice.
 *
 */

inline void Op_Diff_negligeable::contribuer_a_avec(const DoubleTab& inco,
                                                   Matrice_Morse& amatrice) const
{
  ;
}

/*! @brief on ajoute la contribution du second membre.
 *
 */

inline void Op_Diff_negligeable::contribuer_au_second_membre(DoubleTab& resu) const
{
  ;
}

// Modification des Cl
inline void  Op_Diff_negligeable::modifier_pour_Cl(Matrice_Morse& amatrice, DoubleTab& resu) const
{
  ;
}

inline void Op_Diff_negligeable::dimensionner(Matrice_Morse& amatrice) const
{
  ;
}

inline void Op_Diff_negligeable::mettre_a_jour(double temps)
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
inline void Op_Diff_negligeable::associer(const Domaine_dis& z,
                                          const Domaine_Cl_dis& zcl,
                                          const Champ_Inc& ch)
{
  Operateur_negligeable::associer(z, zcl, ch);
}

//Surcharge pour que la methode ne fasse rien
inline void Op_Diff_negligeable::associer_champ_masse_volumique(const Champ_base&)
{

}

#endif

