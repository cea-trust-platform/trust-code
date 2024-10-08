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

#ifndef Operateur_Conv_included
#define Operateur_Conv_included

#include <Operateur_Conv_base.h>
#include <TRUST_Deriv.h>
#include <Operateur.h>
#include <TRUST_Ref.h>

class Champ_base;

/*! @brief classe Operateur_Conv Classe generique de la hierarchie des operateurs representant un terme
 *
 *     de convection. Un objet Operateur_Conv peut referencer n'importe quel
 *     objet derivant de Operateur_Conv_base.
 *
 * @sa Operateur_Conv_base Operateur
 */
class Operateur_Conv  : public Operateur, public OWN_PTR(Operateur_Conv_base)
{
  Declare_instanciable(Operateur_Conv);
public :

  inline Operateur_base& l_op_base() override;
  inline const Operateur_base& l_op_base() const override;
  DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const override;
  DoubleTab& calculer(const DoubleTab&, DoubleTab& ) const override;
  inline void associer_vitesse(const Champ_base&) ;
  inline const Champ_base&  vitesse() const ;
  void typer() override;
  inline void typer(const Nom&);
  inline int op_non_nul() const override;
  void associer_norme_vitesse(const Champ_base&);
protected :

  OBS_PTR(Champ_base) la_vitesse;
};

/*! @brief Renvoie l'objet sous-jacent upcaste en Operateur_base
 *
 * @return (Operateur_base&) l'objet sous-jacent upcaste en Operateur_base
 */
inline Operateur_base& Operateur_Conv::l_op_base()
{
  return valeur();
}
/*! @brief Renvoie l'objet sous-jacent upcaste en Operateur_base (version const)
 *
 * @return (Operateur_base&) l'objet sous-jacent upcaste en Operateur_base
 */
inline const Operateur_base& Operateur_Conv::l_op_base() const
{
  return valeur();
}

/*! @brief Associe la vitesse (en tant que vitesse transportante) a l'operateur de convection.
 *
 * @param (Champ_Inc_base& vit) le champ inconnue representant la vitesse
 * @return le champ inconnue representant la vitesse transportante
 */
inline void Operateur_Conv::associer_vitesse(const Champ_base& vit)
{
  la_vitesse = vit;
}

/*! @brief Renvoie la vitesse transportante de l'operateur
 *
 * @return (Champ_Inc_base&) le champ inconnue representant la vitesse transportante
 */
inline const Champ_base& Operateur_Conv::vitesse() const
{
  return la_vitesse.valeur();
}



/*! @brief Type l'operateur.
 *
 * @param (Nom& typ) le nom representant le type de l'operateur
 */
inline void Operateur_Conv::typer(const Nom& a_type)
{
  OWN_PTR(Operateur_Conv_base)::typer(a_type);
}

inline int Operateur_Conv::op_non_nul() const
{
  if (non_nul())
    return 1;
  else
    return 0;
}
#endif
