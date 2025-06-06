/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Dirichlet_included
#define Dirichlet_included

#include <Cond_lim_base.h>

/*! @brief classe Dirichlet Cette classe est la classe de base de la hierarchie des conditions aux limites de type Dirichlet.
 *
 *     Une condition aux limites de type Dirichlet impose la valeur d'un champ inconnue sur une frontiere, ce qui correspond a:
 *
 *      - vitesse imposee pour l'equation de Navier-Stokes
 *      - scalaire impose pour l'equation de transport d'un scalaire
 *
 * @sa Cond_lim_base Dirichlet_homogene
 */
class Dirichlet: public Cond_lim_base
{
  Declare_base(Dirichlet);
public:
  virtual double val_imp(int i) const;
  virtual double val_imp(int i, int j) const;
  virtual double val_imp_au_temps(double temps, int i) const;
  virtual double val_imp_au_temps(double temps, int i, int j) const;
  const DoubleTab& val_imp(double temps=DMAXFLOAT) const;
  const DoubleTab& val_imp_au_temps(double temps) const { return val_imp(temps); }
  void verifie_ch_init_nb_comp() const override;
private:
  mutable DoubleTab tab_; // Stocke toutes les valeurs sur toutes les faces de la frontiere (pas d'hypothese sur un champ uniforme). Utile pour le GPU.
};

#endif
