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

#ifndef Champ_Fonc_Fonction_included
#define Champ_Fonc_Fonction_included

#include <Champ_Fonc_Tabule.h>

/*! @brief Classe Champ_Fonc_Fonction Classe derivee de Champ_Fonc_Tabule qui represente les
 *
 *      champs fonctions d'un autre champ par une fonction
 *      L'objet porte un membre de type Champ_Fonc qui stocke
 *      les valeurs du champ tabule.
 *
 * @sa Champ_Fonc_base
 */
class Champ_Fonc_Fonction : public Champ_Fonc_Tabule
{
  Declare_instanciable(Champ_Fonc_Fonction);
};

class Sutherland : public Champ_Fonc_Fonction
{
  Declare_instanciable_sans_constructeur(Sutherland);

public :
  Sutherland();
  void lire_expression();

  // Methodes inlines
  inline void set_val_params(const Nom& prob,const double A, const double C,const double Tref);
  inline void set_prob(const Nom& prob) { prob_ = prob ;}
  inline void set_A(const double A) { A_ = A; }
  inline void set_C(const double C) { C_ = C; }
  inline void set_Tref(const double Tref) { Tref_ = Tref; }
  inline Nom& get_prob() { return prob_; }
  inline const double& get_A() const { return A_; }
  inline const double& get_C() const { return C_; }
  inline const double& get_Tref() const { return Tref_; }

protected:
  double A_, C_, Tref_;
  Nom prob_;
};

inline void Sutherland::set_val_params(const Nom& prob,const double A,const double C,const double Tref)
{
  set_prob(prob);
  set_A(A);
  set_C(C);
  set_Tref(Tref);
}

#endif /* Champ_Fonc_Fonction_included */
