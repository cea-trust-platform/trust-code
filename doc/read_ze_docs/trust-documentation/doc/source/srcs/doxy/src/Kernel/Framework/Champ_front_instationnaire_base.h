/****************************************************************************
* Copyright (c) 2022, CEA
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


#ifndef Champ_front_instationnaire_base_included
#define Champ_front_instationnaire_base_included

#include <Champ_front_base.h>



/*! @brief classe Champ_front_base Classe de base pour les Champs aux frontieres instationnaires,
 *
 *      mais uniformes en espace.
 *      Les aspects temporels sont equivalents a ceux de
 *      Ch_front_var_instationnaire.
 *      En revanche, le tableau de valeurs est dimensionne a une seule
 *      valeur et n'a pas d'espace virtuel.
 *
 * @sa Champ_front_base
 */
class Champ_front_instationnaire_base : public Champ_front_base
{

  Declare_base(Champ_front_instationnaire_base);

public :
  void fixer_nb_valeurs_temporelles(int nb_cases) override;
  int initialiser(double temps, const Champ_Inc_base& inco) override;
  inline DoubleTab& Gpoint();
  inline const DoubleTab& Gpoint() const;
  void Gpoint(double t1, double t2);
  DoubleTab& valeurs_au_temps(double temps) override;
  const DoubleTab& valeurs_au_temps(double temps) const override;
  int avancer(double temps) override;
  int reculer(double temps) override;
  void valeurs_face(int,DoubleVect&) const override;

protected :
  DoubleTab Gpoint_; // Derivee en temps des valeurs conditons limites

};

inline DoubleTab& Champ_front_instationnaire_base::Gpoint()
{
  return Gpoint_;
}

inline const DoubleTab& Champ_front_instationnaire_base::Gpoint() const
{
  return Gpoint_;
}

#endif
