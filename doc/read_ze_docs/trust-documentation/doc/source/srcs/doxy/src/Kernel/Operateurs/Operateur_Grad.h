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

#ifndef Operateur_Grad_included
#define Operateur_Grad_included

#include <Operateur_Grad_base.h>
#include <TRUST_Deriv.h>
#include <Operateur.h>

/*! @brief Classe Operateur_Grad Classe generique de la hierarchie des operateurs calculant le gradient
 *
 *     d'un champ. Un objet Operateur_Grad peut referencer n'importe quel
 *     objet derivant de Operateur_Grad_base.
 *
 * @sa Operateur_Grad_base Operateur
 */
class Operateur_Grad  : public Operateur, public DERIV(Operateur_Grad_base)
{
  Declare_instanciable(Operateur_Grad);
public :

  inline Operateur_base& l_op_base() override;
  inline const Operateur_base& l_op_base() const override;
  DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const override;
  DoubleTab& calculer(const DoubleTab&, DoubleTab& ) const override;
  void typer () override;
  void typer_direct(const Nom&);
  inline int op_non_nul() const override;

};

/*! @brief Renvoie l'objet sous-jacent upcaste en Operateur_base
 *
 * @return (Operateur_base&) l'objet sous-jacent upcaste en Operateur_base
 */
inline Operateur_base& Operateur_Grad::l_op_base()
{
  return valeur();
}
/*! @brief Renvoie l'objet sous-jacent upcaste en Operateur_base (version const)
 *
 * @return (Operateur_base&) l'objet sous-jacent upcaste en Operateur_base
 */
inline const Operateur_base& Operateur_Grad::l_op_base() const
{
  return valeur();
}

inline int Operateur_Grad::op_non_nul() const
{
  if (non_nul())
    return 1;
  else
    return 0;
}

#endif
