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

#ifndef Operateur_Evanescence_included
#define Operateur_Evanescence_included

#include <Operateur_Evanescence_base.h>
#include <TRUST_Deriv.h>
#include <Operateur.h>

/*! @brief classe Operateur_Evanescence Classe generique de la hierarchie des operateurs representant un terme
 *
 *     de gestion de l'evanescence. Un objet Operateur_Evanescence peut referencer n'importe quel
 *     objet derivant de Operateur_Evanescence_base.
 *
 * @sa Operateur_base Operateur
 */

class Operateur_Evanescence : public Operateur, public OWN_PTR(Operateur_Evanescence_base)
{
  Declare_instanciable(Operateur_Evanescence);
public:
  Operateur_base& l_op_base() override { return valeur(); }
  const Operateur_base& l_op_base() const override { return valeur(); }
  DoubleTab& ajouter(const DoubleTab& donnee, DoubleTab& resu) const override { return valeur().ajouter(donnee, resu); }
  DoubleTab& calculer(const DoubleTab& donnee, DoubleTab& resu) const override { return valeur().calculer(donnee, resu); }
  void typer(const Nom& un_type) { OWN_PTR(Operateur_Evanescence_base)::typer(un_type); }
  void typer() override;
  inline int op_non_nul() const override { return non_nul(); };

protected:
  OBS_PTR(Champ_base) la_diffusivite_;
};

#endif /* Operateur_Evanescence_included */
