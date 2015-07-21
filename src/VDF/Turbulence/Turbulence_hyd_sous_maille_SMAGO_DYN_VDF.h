/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Turbulence_hyd_sous_maille_SMAGO_DYN_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Turbulence_hyd_sous_maille_SMAGO_DYN_VDF_included
#define Turbulence_hyd_sous_maille_SMAGO_DYN_VDF_included

#include <Turbulence_hyd_sous_maille_Smago_VDF.h>
#include <IntTab.h>

/////////////////////////////////////////////////////////////////////
class Turbulence_hyd_sous_maille_SMAGO_DYN_VDF : public Turbulence_hyd_sous_maille_Smago_VDF
{

  Declare_instanciable_sans_constructeur(Turbulence_hyd_sous_maille_SMAGO_DYN_VDF);

public:

  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF();
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void associer(const Zone_dis& zone_dis,const Zone_Cl_dis& zone_Cl_dis);
  void mettre_a_jour(double);
  int preparer_calcul();


  static void calculer_length_scale(DoubleVect& ,const Zone_VDF& );
  static void calculer_cell_cent_vel(DoubleTab& ,const Zone_VDF& ,Champ_Inc& );
  static void calculer_filter_field(const DoubleTab& ,DoubleTab& ,const Zone_VDF& );
  static void calculer_Sij( DoubleTab& ,const Zone_VDF& ,const Zone_Cl_VDF& ,Champ_Inc& );
  static void calculer_Sij_vel_filt(const DoubleTab& ,DoubleTab& ,const Zone_VDF& );
  static void calculer_S_norme(const DoubleTab& ,DoubleVect& ,int );
  static void interpole(const IntVect&, const DoubleVect&, const DoubleVect&, double& );

  /////////////////////////////////////////////////////
protected :

  Champ_Fonc coeff_field;
  Motcle methode_stabilise;
  // variable qui vaut 6_points ou plans_parallele suivant
  // la methode choisie pour stabiliser la constante dynamique
  int N_c_;
  IntVect compt_c_;
  IntVect corresp_c_;
  IntTab elem_elem_;
  DoubleTab cell_cent_vel_;
  //         DoubleTab haut_moy,bas_moy;



  void calculer_filter_tensor( DoubleTab& );
  void calculer_Lij(const DoubleTab& , const DoubleTab& , DoubleTab& );
  void calculer_Mij(const DoubleTab& ,const DoubleTab& ,const DoubleVect& ,DoubleTab& );
  void calculer_model_coefficient(const DoubleTab& ,const DoubleTab& );
  Champ_Fonc& calculer_viscosite_turbulente(const DoubleVect& ,const DoubleVect& );
  virtual Champ_Fonc& calculer_viscosite_turbulente();
  void calculer_energie_cinetique_turb();
  Champ_Fonc& calculer_energie_cinetique_turb(const DoubleVect& ,const DoubleVect& );
  void controler_grandeurs_turbulentes();

  void stabilise_moyenne( const DoubleTab& ,const DoubleTab& );
  void stabilise_moyenne_6_points( const DoubleTab& ,const DoubleTab& );
  void stabilise_moyenne_plans_paralleles( const DoubleTab& ,const DoubleTab& );
  void stabilise_moyenne_euler_lagrange(const DoubleTab&, const DoubleTab& );
  void calcul_voisins(const int, IntVect& , DoubleVect& );
  void calc_elem_elem();
  void calcul_tableaux_correspondance(int& ,  IntVect&, IntVect& );

};


#endif
