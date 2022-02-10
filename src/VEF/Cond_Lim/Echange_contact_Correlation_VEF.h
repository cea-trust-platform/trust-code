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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Echange_contact_Correlation_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Cond_Lim
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_contact_Correlation_VEF_included
#define Echange_contact_Correlation_VEF_included


#include <Temperature_imposee_paroi.h>
#include <Parser_U.h>
#include <Conduction.h>
class Front_VF;
class Zone_VEF;
class Faces;

////////////////////////////////////////////////////////////////

//
//  .DESCRIPTION classe : Echange_contact_Correlation_VEF
//
//

////////////////////////////////////////////////////////////////

class Echange_contact_Correlation_VEF  : public Temperature_imposee_paroi
{

  Declare_instanciable(Echange_contact_Correlation_VEF);

public :
  void mettre_a_jour(double ) override;
  int compatible_avec_eqn(const Equation_base&) const override;
  void completer() override;
  virtual void imprimer(double ) const;
  virtual int limpr(double , double ) const;

  /*
   *
   * Elle se charge de renvoyer la valeur du coefficient d'echange sur la maille i.
   */
  virtual double calculer_coefficient_echange(int i);



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
  inline double getDh(int i) const
  {
    return diam(i);
  }
  inline double getQh() const
  {
    return debit;  // le debit
  }

protected :
  void calculer_CL();
  void calculer_Q();
  void calculer_prop_physique();
  void init();
  void calculer_Vitesse();
  void calculer_Tfluide();
  void calculer_h_solide(DoubleTab&);
  double pdt_scalSqrt(const Zone_VEF& ,int,int ,
                      int ,int ,double ) ;
  double pdt_scal(const Zone_VEF& ,int ,int ,
                  int ,int ,double ) ;
  double surfacesVEF(const Zone_VEF& ,int ,int );
  void init_tab_echange();

  IntVect correspondance_solide_fluide;
  DoubleTab autre_h;
  int Reprise_temperature;

  double T_CL0, T_CL1 ; // CL sur le domaine. En seq. = Tinf et Tsup ; En parallele = Tvoisin
  double Tinf, Tsup; // Temperature entree, sortie
  int dir; // Direction du cylindre
  Parser_U lambda_T;
  Parser_U mu_T;
  Parser_U rho_T;
  Parser_U fct_Nu;
  Parser_U fct_vol;
  Parser_U fct_Dh;
  double dt_impr;
  double Cp;
  double debit;
  double xinf, xsup;

  DoubleVect vol; // volumes des tranches Sdz du volume suivant la direction de l'ecoulement
  DoubleVect coord; // coordonnees des points de discretisation 1D

  int N;   // nb de points du maillage 1D pour la vitesse et la temperature
  DoubleVect U, T; // inconnues fluides vitesse et temperature
  DoubleVect Qvol; // puissance volumique dans le fluide 1D
  DoubleVect rho, mu, lambda,diam; // tableau des proprietes physiques du fluide a un instant donne
  DoubleVect h_correlation; // le coeff d'echange par correlation
  DoubleTab h_solide; // le coeff d'echange par correlation
  DoubleVect flux_radiatif; // le flux radiatif entre patch

private:
  int avec_rayo;
  double emissivite;
  IntTab patches_rayo;
  IntVect nb_faces_patch;
  IntVect correspondance_face_patch;
  IntTab correspondance_fluide_patch;

};


#endif
