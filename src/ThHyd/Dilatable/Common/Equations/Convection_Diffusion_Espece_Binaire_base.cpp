/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Convection_Diffusion_Espece_Binaire_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Equations
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Espece_Binaire_base.h>
#include <Navier_Stokes_Fluide_Dilatable_base.h>
#include <Fluide_Dilatable_base.h>
#include <Op_Conv_negligeable.h>
#include <Probleme_base.h>
#include <DoubleTrav.h>

Implemente_base(Convection_Diffusion_Espece_Binaire_base,"Convection_Diffusion_Espece_Binaire_base",Convection_Diffusion_Espece_Fluide_Dilatable_base);

Sortie& Convection_Diffusion_Espece_Binaire_base::printOn(Sortie& is) const
{
  return Convection_Diffusion_Espece_Fluide_Dilatable_base::printOn(is);
}

Entree& Convection_Diffusion_Espece_Binaire_base::readOn(Entree& is)
{
  Convection_Diffusion_Espece_Fluide_Dilatable_base::readOn(is);
  terme_convectif.set_fichier("Convection_Espece_Binaire");
  terme_convectif.set_description((Nom)"Convective flux =Integral(-rho*Y*u*ndS) [kg/s] if SI units used");
  terme_diffusif.set_fichier("Diffusion_Espece_Binaire");
  terme_diffusif.set_description((Nom)"Diffusive flux=Integral(rho*D*grad(Y)*ndS) [kg/s] if SI units used");

  //On modifie le nom ici pour que le champ puisse etre reconnu si une sonde de fraction_massique est demandee
  if (le_fluide->type_fluide()=="Melange_Binaire") l_inco_ch->nommer("fraction_massique");
  else
    {
      Cerr << "Error in your data file !" << finl;
      Cerr << "The equation " << que_suis_je() << " should only be used with the EOS Melange_Binaire";
      Process::exit();
    }
  champs_compris_.ajoute_champ(l_inco_ch);
  return is;
}

void Convection_Diffusion_Espece_Binaire_base::set_param(Param& param)
{
  Convection_Diffusion_Espece_Fluide_Dilatable_base::set_param(param);
}

int Convection_Diffusion_Espece_Binaire_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      /*
       * The diffusive term is div(rho*D*grad(Y))
       * recall that mu_sur_Sc = rho*D and nu_sur_Sc = D
       */

      Cerr << "Reading and typing of the diffusion operator : " << finl;
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      ref_cast_non_const(Champ_base,terme_diffusif.diffusivite()).nommer("mu_sur_Schmidt");
      is >> terme_diffusif;
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(diffusivite_pour_pas_de_temps());
      return 1;
    }
  else
    return Convection_Diffusion_Espece_Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);

}

const Champ_base& Convection_Diffusion_Espece_Binaire_base::diffusivite_pour_pas_de_temps()
{
  return le_fluide->nu_sur_Schmidt(); // D (diffusion coefficient)
}

const Champ_base& Convection_Diffusion_Espece_Binaire_base::vitesse_pour_transport()
{
  // we need rho * u and not u
  const Probleme_base& pb = probleme();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,pb.equation(0));
  const Champ_Inc& vitessetransportante = eqn_hydr.rho_la_vitesse() ;
  return vitessetransportante;
}

int Convection_Diffusion_Espece_Binaire_base::preparer_calcul()
{
  Convection_Diffusion_Espece_Fluide_Dilatable_base::preparer_calcul();
  zcl_modif_.les_conditions_limites().set_modifier_val_imp(0);
  return 1;
}

void Convection_Diffusion_Espece_Binaire_base::completer()
{
  Convection_Diffusion_Espece_Fluide_Dilatable_base::completer();
  completer_common_impl(inconnue(),zcl_modif_,zone_Cl_dis(),schema_temps(),le_fluide,*this);
}

DoubleTab& Convection_Diffusion_Espece_Binaire_base::derivee_en_temps_inco(DoubleTab& derivee)
{
  derivee=0.;
  const Operateur& op_diff = operateur(0);
  const Operateur& op_conv = operateur(1);
  return derivee_en_temps_inco_sans_solveur_masse_imp(derivee,op_diff,op_conv,
                                                      les_sources,solveur_masse,true /* explicit */);
}

void Convection_Diffusion_Espece_Binaire_base::assembler( Matrice_Morse& matrice,const DoubleTab& inco, DoubleTab& resu)
{
  const Operateur& op_diff = operateur(0);
  const Operateur& op_conv = operateur(1);
  assembler_impl(matrice,inco,resu,op_diff,op_conv,les_sources,solveur_masse);
}
