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

#ifndef Sortie_libre_Gradient_Pression_libre_VEFPreP1B_included
#define Sortie_libre_Gradient_Pression_libre_VEFPreP1B_included

/*! @brief Sortie_libre_Gradient_Pression_libre_VEFPreP1B
 *
 *  Cette classe derive de la classe Neumann_sortie_libre
 *
 *     Elle represente une frontiere ouverte avec condition de gradient de pression calculer.
 *     La fonction flux_impose() renvoie une valeur de pression a l'exterieur calculee a partir du gradient de pression impose
 *     et de la pression a l'interieur du domaine.
 *
 */
#include <Neumann_sortie_libre.h>
#include <TRUST_Ref.h>

class Champ_P1_isoP1Bulle;
class Domaine_VEF;

class Sortie_libre_Gradient_Pression_libre_VEFPreP1B: public Neumann_sortie_libre
{
  Declare_instanciable(Sortie_libre_Gradient_Pression_libre_VEFPreP1B);
public:
  void completer() override;
  int initialiser(double temps) override;
  void mettre_a_jour(double temps) override;
  double flux_impose(int) const override;
  double flux_impose(int, int) const override;
  virtual double Grad_P_lib_VEFPreP1B(int) const;

protected:
  REF(Domaine_VEF) le_dom_VEF;
  REF(Champ_P1_isoP1Bulle) pression_interne;

  DoubleVect pression_temps_moins_un;
  DoubleVect pression_temps_moins_deux;
  DoubleVect pression_moins_un_temps_moins_un;
  DoubleVect pression_moins_un_temps_moins_deux;
  DoubleVect pression_moins_deux_temps_moins_un;
  DoubleTab pression_moins_un;
  DoubleTab pression_moins_deux;

  DoubleVect Pimp;
  DoubleVect coeff;
  int init = -1;
};

#endif
