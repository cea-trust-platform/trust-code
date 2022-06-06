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


#ifndef Traitement_particulier_NS_Profils_thermo_VDF_included
#define Traitement_particulier_NS_Profils_thermo_VDF_included

#include <Traitement_particulier_NS_Profils_VDF.h>
#include <Ref_Convection_Diffusion_std.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_NS_Profils_thermo_VDF
//
// .SECTION voir aussi
//      Navier_Stokes_Turbulent, Traitement_particulier_base,
//      Traitement_particulier_VDF, Trait_part_NS_Profils_VDF
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_Profils_thermo_VDF : public Traitement_particulier_NS_Profils_VDF
{
  Declare_instanciable(Traitement_particulier_NS_Profils_thermo_VDF);

public :

  Entree& lire(Entree& is) override;
  Entree& lire(const Motcle& , Entree& ) override;
  void reprendre_stat() override;
  void sauver_stat() const override;
  void associer_eqn(const Equation_base&) override;
  void ecriture_fichier_moy_spat_thermo(const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const IntVect&, const DoubleVect&,  const DoubleVect& );
  void ecriture_fichier_moy_temp_thermo(const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const double , const IntVect& );
  void calculer_moyennes_spatiales_thermo(DoubleTab& , DoubleTab& , DoubleTab& , DoubleTab& , DoubleTab& , const IntTab& , const IntTab& , const IntVect& , const DoubleTab& );
  void calculer_integrales_temporelles(DoubleTab& , const DoubleTab& , const DoubleTab& , const DoubleVect& , const DoubleVect& );

  void post_traitement_particulier() override;
  void init_calcul_moyenne(void) override;
  void preparer_calcul_particulier(void) override;
  void init_calcul_stats(void);

protected :

  REF(Convection_Diffusion_std) mon_equation_NRJ;
  int oui_stats_thermo, oui_repr_stats_thermo,tmin_tmax;
  Nom fich_repr_stats_thermo;
  DoubleTab Tmoy_temp,Trms_temp ,upTp_temp,vpTp_temp,wpTp_temp;

private:



};
#endif

