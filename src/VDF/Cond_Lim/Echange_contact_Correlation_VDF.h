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

#ifndef Echange_contact_Correlation_VDF_included
#define Echange_contact_Correlation_VDF_included


#include <Echange_global_impose.h>
#include <Parser_U.h>
class Front_VF;
class Zone_VDF;
class Faces;

////////////////////////////////////////////////////////////////

//
//  .DESCRIPTION classe : Echange_contact_Correlation_VDF
//
//

////////////////////////////////////////////////////////////////

class Echange_contact_Correlation_VDF  : public Echange_global_impose
{

  Declare_instanciable(Echange_contact_Correlation_VDF);

public :

  void mettre_a_jour(double ) override;
  int compatible_avec_eqn(const Equation_base&) const override;
  void completer() override;
  virtual void imprimer(double ) const;
  virtual int limpr(double , double ) const;

  /*
   * Methode qu'on peut surcharger  dans les classes filles pour definir un Nusselt particulier
   * Sinon on peut rentrer dans le jdd directement une fonction Nusselt de Reynolds local et Prandtl local.
   * Elle se charge de renvoyer la valeur du coefficient d'echange sur la maille i.
   */
  virtual double calculer_coefficient_echange(int i);

  /*
   * Methode qu'on peut surcharger  dans les classes filles pour definir une forme particuliere.
   * Sinon on peut rentrer dans le jdd une fonction de la surface laterale S et du diametre hydraulique Dh.
   * Elle calcule le volume d'une tranche de fluide de surface laterale s et de diametre hydraulique d.
   * Cette fonction depend donc de la geometrie pour laquelle la correlation est ecrite
   * (plaques paralleles, cylindres etc...)
   */
  virtual double volume(double s, double d);

  /*
   * Renvoie U et T et les proprietes physiques sur la maille 1D i
   */
  inline double getU(int i) const
  {
    return U(i);
  }
  inline double getT(int i) const
  {
    return T(i);
  }
  inline double getMu(int i) const
  {
    return mu(i);
  }
  inline double getLambda(int i) const
  {
    return lambda(i);
  }
  inline double getRho(int i) const
  {
    return rho(i);
  }
  inline double getCp() const
  {
    return Cp;
  }
  inline double getDh() const
  {
    return diam;
  }
  inline double getQh() const
  {
    return debit;  // le debit
  }

protected :
  void calculer_CL();
  void calculer_Q();
  void calculer_prop_physique();
  void trier_coord();
  void calculer_Vitesse();
  void calculer_Tfluide();
  void calculer_h_mon_pb(DoubleTab&);
  void calculer_h_solide(DoubleTab& ,const Equation_base& ,const Zone_VDF& ,const Front_VF& ,const    Milieu_base&);
  void init_tab_echange();


  IntVect correspondance_solide_fluide;
  DoubleTab autre_h;
  int Reprise_temperature;


  DoubleTab tab_ech;

  double Tinf, Tsup; // Temperature entree, sortie
  double T_CL0, T_CL1 ; // CL sur le domaine. En seq. = Tinf et Tsup ; En parallele = Tvoisin
  int dir; // Direction du cylindre
  Parser_U lambda_T;
  Parser_U mu_T;
  Parser_U rho_T;
  Parser_U fct_Nu;
  Parser_U fct_vol;
  double dt_impr;
  double Cp;
  double debit;

  double diam; // diametre hydraulique
  DoubleVect vol; // volumes des tranches Sdz du volume suivant la direction de l'ecoulement
  DoubleVect coord; // coordonnees des points de discretisation 1D

  int N;   // nb de points du maillage 1D pour la vitesse et la temperature
  DoubleVect U,T; // inconnues fluides vitesse et temperature
  DoubleVect Qvol; // puissance volumique dans le fluide 1D
  DoubleVect rho, mu, lambda; // tableau des proprietes physiques du fluide a un instant donne
  DoubleVect h_correlation; // le coeff d'echange par correlation

};


#endif
