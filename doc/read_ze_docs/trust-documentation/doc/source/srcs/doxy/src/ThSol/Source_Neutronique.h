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

#ifndef Source_Neutronique_included
#define Source_Neutronique_included



#include <Terme_Puissance_Thermique.h>
#include <Source_base.h>
#include <Parser_U.h>
class Param;

/*! @brief class Source_Neutronique
 *
 *  Cette classe represente un terme source de l'equation de la thermique
 *  du type puissance volumique issue de la neutronique.
 *  La determination de la puissance s'effectue par resolution d'equations
 *  differentielles regissant la cinetique neutronique.
 *
 *
 * @sa Terme_Puissance_Thermique
 */

class Source_Neutronique : public Terme_Puissance_Thermique, public Source_base
{
  Declare_base(Source_Neutronique);

public:
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;

  double rho(double, double);
  const Nom& repartition() const;
  const Nom& nom_ssz() const;
  void faire_un_pas_de_temps_RK();
  void faire_un_pas_de_temps_EE();
  void aller_au_temps(double);
  void mettre_a_jour(double temps) override;
  void completer() override;
  virtual void imprimer(double ) const;
  virtual int limpr(double , double ) const;
  inline double puissance_neutro() const { return Un(0); }
  virtual double calculer_Tmoyenne() = 0;

private :
  void mul(DoubleTab& m, DoubleVect& v, DoubleVect& resu);
  void mettre_a_jour_matA(double t);
  int N = -1;                 // Nombre de groupe >= 1
  double Tvie = -100.;         // duree de vie d'un neutron
  DoubleVect Un, Unp1;// Inconnus a l'instant n et n+1 : Puissance Un(0) et Concentrations Un(i) du groupe i
  DoubleVect beta;        // beta(i) = nombre de neutrons retardees issue de l espece i,
  double beta_som = -100.;          // somme des beta(i)
  DoubleVect lambda;         //
  DoubleTab matA;           // matrice du systeme d'equa diff
  double dt= -100.;                 // pas de temps
  int init = 1;
  double P0= -100.;                 // Puissance a t=0
  DoubleVect Ci0;         // Concentration a t=0
  int Ci0_ok=0;
  double dt_impr = 1e10;
  double temps_courant= -100.;
  void (Source_Neutronique::*faire_un_pas_de_temps)(void) = nullptr;
  double Tmoy= -100.; // temperature moyenne

  Parser_U fct_tT;
  Nom f_xyz;
  Nom n_ssz;

};


#endif

