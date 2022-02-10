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
// File:        Op_Conv_EF_VEF_P1NC_Stab.h
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Conv_EF_VEF_P1NC_Stab_included
#define Op_Conv_EF_VEF_P1NC_Stab_included

#include <Equation_base.h>
#include <Matrice_Morse.h>
#include <Op_Conv_VEF_Face.h>
#include <Ref_Sous_zone_VF.h>
#include <DoubleTabs.h>
#include <IntList.h>

//
// .DESCRIPTION class Op_Conv_EF_VEF_P1NC_Stab
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


class Op_Conv_EF_VEF_P1NC_Stab : public Op_Conv_VEF_Face
{

  Declare_instanciable(Op_Conv_EF_VEF_P1NC_Stab);

public:

  //Methodes annexes
  void remplir_fluent(DoubleVect& ) const override;
  int is_compressible() const;
  void completer() override;

  //Methodes pour l'explicite
  DoubleTab& ajouter(const DoubleTab& , DoubleTab& ) const override;

  //Methodes pour l'implicite
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse&) const;

  //test
  void         modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const override;

private :

  //Methodes annexes
  void calculer_flux_bords(const DoubleTab&, const DoubleTab&, const DoubleTab&) const;

  //Methodes pour l'explicite
  void reinit_conv_pour_Cl(const DoubleTab&,const IntList&, const DoubleTabs&, const DoubleTab&, DoubleTab&) const;
  void calculer_coefficients_operateur_centre(DoubleTab&,const int&, const DoubleTab& vitesse) const;
  DoubleTab& ajouter_partie_compressible(const DoubleTab&, DoubleTab&, const DoubleTab& vitesse) const;
  DoubleTab& ajouter_operateur_centre(const DoubleTab&, const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_diffusion(const DoubleTab&, const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_antidiffusion(const DoubleTab&, const DoubleTab&, DoubleTab&) const;

  inline void calculer_senseur(const DoubleTab&, const DoubleVect&, const int&, const int&, const IntTab&, const IntTab&, const IntTab&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&) const;
  void mettre_a_jour_pour_periodicite(DoubleTab&) const;
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
  IntTab elem_nb_faces_dirichlet_;
  IntTab elem_faces_dirichlet_;

  ArrOfDouble alpha_tab;
  ArrOfDouble beta; // vaut zero pour les faces ou l'on souhaite degenerer en Amont.
  //  mutable DoubleTab limiteurs_;//tableau stockant pour chaque face la moyenne algebrique du limiteur

  double alpha_;

  int is_compressible_;
  int test_;
  int old_;
  int volumes_etendus_;

  bool sous_zone;  // Cas d'une sous-zone a definir pour que l'EF_Stab degenere en Amont
  int new_jacobienne_;
  Nom nom_sous_zone;
  REF(Sous_zone_VF) la_sous_zone_dis;

  int nb_ssz_alpha;
  DoubleVect alpha_ssz;
  Noms noms_ssz_alpha;
  bool ssz_alpha;

};

inline void Op_Conv_EF_VEF_P1NC_Stab::contribuer_a_avec(const DoubleTab& inco,
                                                        Matrice_Morse& matrice) const
{
  ajouter_contribution(inco, matrice);
}

inline int Op_Conv_EF_VEF_P1NC_Stab::is_compressible() const
{
  return is_compressible_;
}

#endif
