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

#ifndef Op_Conv_Amont_PolyMAC_Elem_included
#define Op_Conv_Amont_PolyMAC_Elem_included

#include <Op_Conv_PolyMAC_iterateur_base.h>
#include <Eval_Amont_PolyMAC_Elem.h>
#include <Iterateur_PolyMAC_Elem.h>
#include <Op_PolyMAC_Elem.h>

/*! @brief class Op_Conv_Amont_PolyMAC_Elem
 *
 *   Cette classe represente l'operateur de convection associe a une equation de
 *   transport d'un scalaire.
 *   La discretisation est PolyMAC
 *   Le champ convecte est scalaire
 *   Le schema de convection est du type Amont
 *   L'iterateur associe est de type Iterateur_PolyMAC_Elem
 *   L'evaluateur associe est de type Eval_Amont_PolyMAC_Elem
 *
 */

class Op_Conv_Amont_PolyMAC_Elem: public Op_Conv_PolyMAC_iterateur_base, public Op_PolyMAC_Elem
{

  Declare_instanciable_sans_constructeur(Op_Conv_Amont_PolyMAC_Elem);

public:

  Op_Conv_Amont_PolyMAC_Elem();
  void associer(const Domaine_dis&, const Domaine_Cl_dis&, const Champ_Inc&) override;
  void associer_vitesse(const Champ_base&) override;
  const Champ_base& vitesse() const override;
  Champ_base& vitesse() override;
  inline void dimensionner(Matrice_Morse&) const override;
  inline void dimensionner_bloc_vitesse(Matrice_Morse&) const override;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const override;

protected:
  inline Op_Conv_Amont_PolyMAC_Elem(const Iterateur_PolyMAC_base&);
};

// Ce constructeur permet de creer des classes filles (exemple : front_tracking)
inline Op_Conv_Amont_PolyMAC_Elem::Op_Conv_Amont_PolyMAC_Elem(const Iterateur_PolyMAC_base& it) :
  Op_Conv_PolyMAC_iterateur_base(it)
{
}

/*! @brief on dimensionne notre matrice.
 *
 */
inline void Op_Conv_Amont_PolyMAC_Elem::dimensionner(Matrice_Morse& matrice) const
{
  Op_PolyMAC_Elem::dimensionner(iter->domaine(), iter->domaine_Cl(), matrice);
}

inline void Op_Conv_Amont_PolyMAC_Elem::dimensionner_bloc_vitesse(Matrice_Morse& matrice) const
{
  Op_PolyMAC_Elem::dimensionner_bloc_vitesse(iter->domaine(), iter->domaine_Cl(), matrice);
}

inline void Op_Conv_Amont_PolyMAC_Elem::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_PolyMAC_Elem::modifier_pour_Cl(iter->domaine(), iter->domaine_Cl(), matrice, secmem);
}

#endif
