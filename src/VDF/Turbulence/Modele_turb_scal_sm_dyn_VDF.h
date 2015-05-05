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
// File:        Modele_turb_scal_sm_dyn_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Modele_turb_scal_sm_dyn_VDF_included
#define Modele_turb_scal_sm_dyn_VDF_included

#include <Modele_turbulence_scal_base.h>
#include <Ref_Zone_VDF.h>
#include <Ref_Zone_Cl_VDF.h>
#include <Motcle.h>
#include <Champ_Fonc.h>
#include <IntTab.h>

class Zone_dis;
class Zone_Cl_dis;

class Modele_turb_scal_sm_dyn_VDF : public Modele_turbulence_scal_base
{

  Declare_instanciable(Modele_turb_scal_sm_dyn_VDF);

public:

  void associer(const Zone_dis& , const Zone_Cl_dis& );
  ////virtual Entree& lire(const Motcle&, Entree&);
  virtual void set_param(Param& titi);
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);

protected:

  Motcle methode_stabilise;
  // variable qui vaut 6_points ou plans_parallele suivant
  // la methode choisie pour stabiliser la constante dynamique
  int dynamique_y2 ; // 0 methode classique - 1 methode vT-vT
  int N_c_;
  IntVect compt_c_;
  IntVect corresp_c_;
  IntTab elem_elem_;
  DoubleTab cell_cent_vel;
  DoubleVect model_coeff;

  void mettre_a_jour(double );
  Champ_Fonc& calculer_diffusivite_turbulente();


  ////void calculer_length_scale(DoubleVect& );
  ////void calculer_cell_cent_vel(DoubleTab& );
  ////void calculer_filter_field(const DoubleTab& ,DoubleTab& );
  ////void calculer_Sij( DoubleTab& );
  ////void calculer_Sij_vel_filt(const DoubleTab& ,DoubleTab& );
  ////void calculer_S_norme(const DoubleTab& ,DoubleVect& );
  ////void interpole(const IntVect&, const DoubleVect&, const DoubleVect&, double& );

  void calc_elem_elem();
  void calcul_tableaux_correspondance(int& ,  IntVect&, IntVect& );
  void stabilise_moyenne( const DoubleTab& ,const DoubleTab& );
  void stabilise_moyenne_euler(const DoubleTab&, const DoubleTab& );
  void stabilise_moyenne_lagrange(const DoubleTab&, const DoubleTab& );
  void calcul_voisins(const int, IntVect& , DoubleVect& );
  void calculer_model_coefficient(const DoubleTab& ,const DoubleTab&  );
  //A voir
  void stabilise_moyenne_6_points( const DoubleTab& ,const DoubleTab& );
  //A voir
  void stabilise_moyenne_plans_paralleles( const DoubleTab& ,const DoubleTab& );

  void calculer_filter_coeff(const DoubleTab& ,DoubleTab& );
  void calculer_Lj(const DoubleTab& ,const DoubleTab& ,const DoubleTab& ,const DoubleTab& ,DoubleTab& );
  void calculer_grad_teta(const DoubleVect&, DoubleTab& );
  void calculer_Mj(const DoubleTab& ,const DoubleTab& ,const DoubleTab& ,const DoubleTab& ,const DoubleTab& ,DoubleTab& );

private:

  REF(Zone_VDF) la_zone_VDF;
  REF(Zone_Cl_VDF) la_zone_Cl_VDF;






};



#endif

