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

#ifndef Operateur_Div_included
#define Operateur_Div_included

#include <Operateur_Div_base.h>
#include <TRUST_Deriv.h>
#include <Operateur.h>

/*! @brief classe Operateur_Div Classe generique de la hierarchie des operateurs calculant la divergence
 *
 *     d'un champ. Un objet Operateur_Div peut referencer n'importe quel
 *     objet derivant de Operateur_Div_base.
 *
 * @sa Operateur_Div_base Operateur
 */
class Operateur_Div  : public Operateur, public OWN_PTR(Operateur_Div_base)
{
  Declare_instanciable(Operateur_Div);
public :

  inline Operateur_base& l_op_base() override;
  inline const Operateur_base& l_op_base() const override;
  DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const override;
  DoubleTab& calculer(const DoubleTab&, DoubleTab& ) const override;
  inline void volumique(DoubleTab& ) const;
  void typer () override;
  inline int op_non_nul() const override;

};


/*! @brief Renvoie l'objet sous-jacent upcaste en Operateur_base
 *
 * @return (Operateur_base&) l'objet sous-jacent upcaste en Operateur_base
 */
inline Operateur_base& Operateur_Div::l_op_base()
{
  return valeur();
}

/*! @brief Renvoie l'objet sous-jacent upcaste en Operateur_base (version const)
 *
 * @return (Operateur_base&) l'objet sous-jacent upcaste en Operateur_base
 */
inline const Operateur_base& Operateur_Div::l_op_base() const
{
  return valeur();
}

/*! @brief Initialise le tableau passe en parametre avec la contribution de l'operateur.
 *
 * @param (DoubleTab& div) tableau dans lequel stocke la contribution de l'operateur
 */
inline void Operateur_Div::volumique(DoubleTab& div) const
{
  valeur().volumique(div);
}

inline int Operateur_Div::op_non_nul() const
{
  if (non_nul())
    return 1;
  else
    return 0;
}
#endif
