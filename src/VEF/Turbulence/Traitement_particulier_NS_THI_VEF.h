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
// File:        Traitement_particulier_NS_THI_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_THI_VEF_included
#define Traitement_particulier_NS_THI_VEF_included

#include <Traitement_particulier_NS_THI.h>
#include <Op_Diff_VEF_Face.h>
#include <Champ_P1NC.h>
#include <DoubleVects.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_NS_THI_VEF
//     Cette classe permet de faire les traitements particuliers
//     pour les calculs de  THI, en particulier initialisation
//     particuliere et calculs des spectres!!
//
// .SECTION voir aussi
//      Navier_Stokes_Turbulent, Traitement_particulier_base,
//      Traitement_particulier_VEF
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_THI_VEF : public Traitement_particulier_NS_THI
{
  Declare_instanciable(Traitement_particulier_NS_THI_VEF) ;

public :

  Entree& lire(Entree& );
  void preparer_calcul_particulier(void) ;
  void post_traitement_particulier(void) ;
  void determine_tab_fft_VEF_3D(IntTab& , DoubleTab& , IntVect& , int& , int& );
  void determine_tab_fft_VEF_1D(const IntTab& , const DoubleTab& , const IntVect& , IntVect& , IntVect& );
  void calcul_spectre_3D(const DoubleTab& , Nom , double& );
  void calcul_spectre_1D(const DoubleTab& , Nom , double& );
  virtual void calcul_spectre_operateur(int , DoubleTab&, DoubleTab& ,double) { };

protected :

  double calcul_volume_elem(void) ;
  void renorm_Ec(void);

  double calcul_Ec_spatial(const DoubleTab& , Nom ) ;
  void calcul_Df_spatial(double& ) ;
  void calcul_Sk(DoubleTab& ) ;
  void calcul_nu_t(void);
  void init_calc_spectre(void);
  void calcul_vitesse_moyenne(const DoubleTab& , DoubleVect& ) ;
  void calcul_moyenne(const DoubleTab& , double& ) ;
  void impression_moyenne(void);
  void suppression_vitesse_moyenne(void);
  void conservation_Ec(void);
  void calcul_spectre(void);
  void calcul_correlations(const DoubleTab&);
  void isotropie(const DoubleTab& , Nom );
  void sorties_globales(void);

  void ch_pour_fft_VEF_3D(const DoubleTab& , DoubleTab& , DoubleTab& , DoubleTab& , int ) const;
  void ch_pour_fft_VEF_1D(const DoubleTab& , DoubleVect& , DoubleVect& , DoubleVect& , int , int ) const;

  int ppcm(int , int);
  int pgcd(int , int);

private:

  double L_BOITE;
  IntTab tab_calc_fft_3D;
  IntTab tab_calc_fft_1D;
  DoubleTab tab_coord_1D;
  double Ec_tot_old, temps_old;
  int cle_premier_pas_dt;

  int  calc_sp_1D, calc_sp_3D, compteur_perio_spectre,  cle_suppr_vit_moy, calc_correlations;
  int oui_conservation_Ec;
  DoubleVect moyenne_vitesse;
  DoubleVect moyennes_scal;
  double periode_calc_spectre;
  double temps_retournement;
  double volume_total;

  int nb_points_3D_elem, nb_spectres_3D_elem;
  int nb_points_3D, nb_spectres_3D;
  IntVect nb_points_1D, nb_spectres_1D;

  int nb_champs_scalaires; //nb de champs scalaire a postraiter : temperature1 etc...
  Noms noms_champs_scalaires; // les noms des champs scalaires a postraiter
};

#endif
