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

#ifndef Neumann_included
#define Neumann_included

#include <Cond_lim_base.h>

/*! @brief Classe Neumann Cette classe est la classe de base de la hierarchie des conditions aux limites de type Neumann.
 *
 *     Une condition aux limites de type Neumann impose la valeur de la derivee d'un champ inconnue sur une frontiere, ce qui correspond a:
 *
 *       - flux impose pour l'equation de transport d'un scalaire
 *       - contrainte imposee pour l'equation de quantite de mouvement
 *
 * @sa Cond_lim_base Neumann_homogene
 */
class Neumann: public Cond_lim_base
{
  Declare_base(Neumann);
public:
  virtual double flux_impose(int i) const;
  virtual double flux_impose(int i, int j) const;
  const DoubleTab& flux_impose(bool nb_faces_tot=false) const;

protected:
  mutable DoubleTab flux_impose_; // Stocke toutes les valeurs du flux sur toutes les faces de la frontiere (pas d'hypothese sur un champ uniforme). Utile pour le GPU.
};

#endif
