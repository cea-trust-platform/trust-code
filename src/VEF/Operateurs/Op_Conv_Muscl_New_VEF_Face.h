/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Op_Conv_Muscl_New_VEF_Face.h
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Conv_Muscl_New_VEF_Face_included
#define Op_Conv_Muscl_New_VEF_Face_included

#include <Op_Conv_VEF_Face.h>
#include <Equation_base.h>
#include <Matrice_Morse.h>
#include <Ref_Sous_zone_VF.h>
#include <DoubleTabs.h>
#include <IntList.h>

//
// .DESCRIPTION class Op_Conv_Muscl_New_VEF_Face
//
//  Cette classe represente l'operateur de convection associe a une equation de
//  transport d'un scalaire.
//  La discretisation est VEF
//  Le champ convecte est scalaire ou vecteur de type Champ_P1NC
//  Le schema de convection est isu du papier
//  "High-resolution FEM-TVD schemes based on a fully multidimensional flux limiter"
//   D.Kuzmin and S.Turek.
//  On herite de Op pour recuperer l'implicitation amont
//
// .SECTION voir aussi
// Operateur_Conv_base


class Op_Conv_Muscl_New_VEF_Face : public Op_Conv_VEF_Face
{

  Declare_instanciable(Op_Conv_Muscl_New_VEF_Face);

public:

  //Methodes annexes
  virtual void remplir_fluent(DoubleVect& ) const;
  int is_compressible() const;
  void completer();

  //Methodes pour l'explicite
  DoubleTab& ajouter(const DoubleTab& , DoubleTab& ) const;
  double calculer_dt_stab() const;

  //Methodes pour l'implicite
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse&) const;

  //test
  void         modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;

private :

  //Methodes annexes
  void calculer_flux_bords(const DoubleTab&, const DoubleTab&, const DoubleTab&) const;

  //Methodes pour l'explicite

  void calculer_coefficients_operateur_centre(DoubleTab&,DoubleTab&,DoubleTab&,DoubleTab&,const int&, const DoubleTab& vitesse) const;
  void calculer_flux_operateur_centre(DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const int&,const DoubleTab&,const DoubleTab&) const;
  void modifier_flux_operateur_centre(DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const int&,const DoubleTab&,const DoubleTab&) const;

  DoubleTab& ajouter_partie_compressible(const DoubleTab&, DoubleTab&, const DoubleTab& vitesse) const;
  DoubleTab& ajouter_operateur_centre(const DoubleTab&,const DoubleTab&, const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_diffusion(const DoubleTab&,const DoubleTab&, const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_antidiffusion(const DoubleTab&, const DoubleTab&, const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_antidiffusion_v1(const DoubleTab&, const DoubleTab&, const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_antidiffusion_v2(const DoubleTab&, const DoubleTab&, const DoubleTab&, DoubleTab&) const;


  inline void calculer_senseur_v1(const DoubleTab&, const DoubleTab&, const DoubleVect&, const int&, const int&, const IntTab&, const IntTab&, const IntTab&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&) const;
  inline void calculer_senseur_v2(const DoubleTab&, const DoubleTab&, const DoubleVect&, const int&, const int&, const IntTab&, const IntTab&, const IntTab&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&) const;
  void mettre_a_jour_pour_periodicite(const DoubleTab&,const DoubleTab&,DoubleTab&) const;
  void ajouter_old(const DoubleTab& , DoubleTab&, const DoubleTab& vitesse) const;
  void calculer_data_pour_dirichlet();

  //Methodes pour l'implicite
  void ajouter_contribution_operateur_centre(const DoubleTab&, const DoubleTab&, Matrice_Morse&) const;
  void ajouter_contribution_diffusion(const DoubleTab&, const DoubleTab&, Matrice_Morse&) const;
  void ajouter_contribution_antidiffusion(const DoubleTab&,const DoubleTab&,Matrice_Morse&) const;
  void ajouter_contribution_partie_compressible(const DoubleTab&,const DoubleTab&,Matrice_Morse&) const;

  //Methodes de test
  void test(const DoubleTab&,const DoubleTab&, const DoubleTab& vitesse) const;
  void test_difference_Kij(const DoubleTab&,DoubleTab&,DoubleTab&, const DoubleTab& vitesse) const;
  void test_difference_resu(const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab&,const DoubleTab& vitesse) const;
  void test_implicite() const;

  //Attributs de la classe
  IntTab is_element_for_upwinding_;
  IntTab is_dirichlet_faces_;


  ArrOfDouble alpha_tab;
  ArrOfDouble beta; // vaut zero pour les faces ou l'on souhaite degenerer en Amont.
  //  mutable DoubleTab limiteurs_;//tableau stockant pour chaque face la moyenne algebrique du limiteur

  double alpha_;
  double max_limiteur_;
  int centered_;
  int upwind_;
  int stabilized_;
  int old_centered_;
  int version_;
  int facsec_auto_;

  bool sous_zone;  // Cas d'une sous-zone a definir pour que l'EF_Stab degenere en Amont
  Nom nom_sous_zone;
  REF(Sous_zone_VF) la_sous_zone_dis;

  double (*limiteur_)(double);

};

inline void Op_Conv_Muscl_New_VEF_Face::contribuer_a_avec(const DoubleTab& inco,
                                                          Matrice_Morse& matrice) const
{
  ajouter_contribution(inco, matrice);
}

inline int Op_Conv_Muscl_New_VEF_Face::is_compressible() const
{
  return 1;/* is_compressible_; */
}

#endif
