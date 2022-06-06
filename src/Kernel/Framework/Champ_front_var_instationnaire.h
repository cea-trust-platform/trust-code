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


#ifndef Champ_front_var_instationnaire_included
#define Champ_front_var_instationnaire_included

#include <Champ_front_var.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front_var_instationnaire
//     Classe derivee de Champ_front_var qui represente les champs aux
//     frontieres variables en espace et dans le temps.
//     Les Champ_front_var_instationnaire sont classes suivant que
//     leurs valeurs dependent ou non de parametres externes a
//     l'equation, en Champ_front_var_instationnaire_indep et
//     Champ_front_var_instationnaire_dep.
//     L'implementation du calcul du champ doit se faire dans la
//     methode mettre_a_jour.
//     Dans le premier cas (indep), la methode initialiser peut
//     appeler la methode mettre_a_jour, mais pas dans le second cas
//     (dep). Elle peut de toutes facons utiliser l'inconnue qui lui
//     est passee en parametre comme une premiere estimation.
// .SECTION voir aussi
//     Champ_front_var
//////////////////////////////////////////////////////////////////////////////
class Champ_front_var_instationnaire : public Champ_front_var
{

  Declare_base(Champ_front_var_instationnaire);

public:
  void fixer_nb_valeurs_temporelles(int nb_cases) override;
  int initialiser(double temps, const Champ_Inc_base& inco) override;
  inline DoubleTab& Gpoint();
  inline const DoubleTab& Gpoint() const;
  void Gpoint(double t1, double t2);
  DoubleTab& valeurs_au_temps(double temps) override;
  const DoubleTab& valeurs_au_temps(double temps) const override;
  int avancer(double temps) override;
  int reculer(double temps) override;

  virtual double valeur_au_temps_et_au_point(double temps,int som,double x,double y, double z,int comp) const
  {
    Cerr<<"valeur_au_temps_et_au_point mut be defined si valeur_au_temps_et_au_point_disponible()==1"<<finl;
    exit();
    return 0;
  };
  inline virtual int valeur_au_temps_et_au_point_disponible() const
  {
    return 0;
  };
protected :
  DoubleTab Gpoint_; // Taux d'accroissement du champ

};

inline DoubleTab& Champ_front_var_instationnaire::Gpoint()
{
  return Gpoint_;
}

inline const DoubleTab& Champ_front_var_instationnaire::Gpoint() const
{
  return Gpoint_;
}

#endif
