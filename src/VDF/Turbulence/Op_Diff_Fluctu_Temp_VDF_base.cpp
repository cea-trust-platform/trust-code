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
// File:        Op_Diff_Fluctu_Temp_VDF_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_Fluctu_Temp_VDF_base.h>
#include <Modele_turbulence_scal_Fluctuation_Temperature.h>
#include <Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re.h>
#include <Champ_P0_VDF.h>

Implemente_base(Op_Diff_Fluctu_Temp_VDF_base,"Op_Diff_Fluctu_Temp_VDF_base",Op_Diff_Fluctu_Temp_Base);

Implemente_instanciable_sans_constructeur(Op_Diff_Fluctu_Temp_VDF_Elem,"Op_Diff_Fluctu_Temp_VDF_P0_VDF",Op_Diff_Fluctu_Temp_VDF_base);

implemente_It_VDF_Elem(Eval_Diff_Fluctu_Temp_VDF_Elem)


Sortie& Op_Diff_Fluctu_Temp_VDF_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Sortie& Op_Diff_Fluctu_Temp_VDF_Elem::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Diff_Fluctu_Temp_VDF_base::readOn(Entree& s )
{
  return s ;
}

Entree& Op_Diff_Fluctu_Temp_VDF_Elem::readOn(Entree& s )
{
  return s ;
}


void Op_Diff_Fluctu_Temp_VDF_base::completer()
{
  Operateur_base::completer();
  iter.completer_();
}

void Op_Diff_Fluctu_Temp_VDF_base::mettre_a_jour_diffusivite() const
{
}

// Description:
// complete l'iterateur et l'evaluateur
void Op_Diff_Fluctu_Temp_VDF_Elem::associer(const Zone_dis& zone_dis,
                                            const Zone_Cl_dis& zone_cl_dis,
                                            const Champ_Inc& ch_diffuse)
{
  const Champ_P0_VDF& inco = ref_cast(Champ_P0_VDF,ch_diffuse.valeur());
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter.associer(zvdf, zclvdf, *this);

  Eval_Diff_Fluctu_Temp_VDF_Elem& eval_diff = (Eval_Diff_Fluctu_Temp_VDF_Elem&) iter.evaluateur();
  eval_diff.associer_zones(zvdf, zclvdf );          // Evaluateur_VDF::associer_zones
  eval_diff.associer_inconnue(inco );        // Eval_VDF_Elem::associer_inconnue
}


// Description:
// associe le champ de diffusivite_turbulente a l'evaluateur
void Op_Diff_Fluctu_Temp_VDF_Elem::associer_diffusivite_turbulente()
{
  assert(mon_equation.non_nul());
  Cerr << "mon_equation.que_suis_je() dans Op_Diff_Fluctu_Temp_VDF_base.cpp " <<  mon_equation->que_suis_je() << finl;


  //si c'est un modele de Fluctuation de Temperature 'classique'
  if ( sub_type(Transport_Fluctuation_Temperature,mon_equation.valeur()) )
    {
      const Transport_Fluctuation_Temperature& eqn_transport_Fluctu_Temp = ref_cast(Transport_Fluctuation_Temperature,mon_equation.valeur());

      const Modele_turbulence_scal_Fluctuation_Temperature& mod_turb = eqn_transport_Fluctu_Temp.modele_turbulence();
      const Champ_Fonc& diff_turb = mod_turb.diffusivite_turbulente();
      Eval_Diff_Fluctu_Temp_VDF_Elem& eval_diff = (Eval_Diff_Fluctu_Temp_VDF_Elem&) iter.evaluateur();
      eval_diff.associer_diff_turb(diff_turb);
    }
  else if ( sub_type(Transport_Fluctuation_Temperature_W_Bas_Re,mon_equation.valeur()) )
    {

      //    const Transport_Fluctuation_Temperature_W_Bas_Re& eqn_transport_Fluctu_Temp = ref_cast(Transport_Fluctuation_Temperature_W_Bas_Re,mon_equation.valeur());

      //    const Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re& mod_turb = eqn_transport_Fluctu_Temp.modele_turbulence();
      //    const Champ_Fonc& diff_turb = mod_turb.diffusivite_turbulente();
      Eval_Diff_Fluctu_Temp_VDF_Elem& eval_diff = (Eval_Diff_Fluctu_Temp_VDF_Elem&) iter.evaluateur();
      eval_diff.associer_diff_turb(diff_tot);
    }

  //sinon c'est un modele de Fluctuation de Temperature a la Wrobel
  else if ( sub_type(Transport_Fluctuation_Temperature_W,mon_equation.valeur()) )
    {

      const Transport_Fluctuation_Temperature_W& eqn_transport_Fluctu_Temp = ref_cast(Transport_Fluctuation_Temperature_W,mon_equation.valeur());

      const Modele_turbulence_scal_Fluctuation_Temperature_W& mod_turb = eqn_transport_Fluctu_Temp.modele_turbulence();
      const Champ_Fonc& diff_turb = mod_turb.diffusivite_turbulente();
      Eval_Diff_Fluctu_Temp_VDF_Elem& eval_diff = (Eval_Diff_Fluctu_Temp_VDF_Elem&) iter.evaluateur();
      eval_diff.associer_diff_turb(diff_turb);
    }


}

const Champ_Fonc& Op_Diff_Fluctu_Temp_VDF_Elem::diffusivite_turbulente() const
{
  const Eval_Diff_Fluctu_Temp_VDF_Elem& eval_diff =
    (Eval_Diff_Fluctu_Temp_VDF_Elem&) iter.evaluateur();
  return eval_diff.diffusivite_turbulente();
}

void Op_Diff_Fluctu_Temp_VDF_Elem::mettre_a_jour_diffusivite() const
{
  assert(mon_equation.non_nul());
  if(sub_type(Transport_Fluctuation_Temperature_W_Bas_Re,mon_equation.valeur()))
    {
      const Transport_Fluctuation_Temperature_W_Bas_Re& eqn_transport = ref_cast(Transport_Fluctuation_Temperature_W_Bas_Re,mon_equation.valeur());
      const Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re& mod_turb = eqn_transport.modele_turbulence();
      const Champ_Fonc& diff_turb = mod_turb.diffusivite_turbulente();
      const Fluide_Incompressible& mil = ref_cast(Fluide_Incompressible,eqn_transport.milieu());
      const Champ_Don& diff_cinematique = mil.diffusivite();

      //        Cerr << "alpha = " <<         diff_cinematique->valeurs() << finl;

      //modif pour le Bas_Re : prise en compte du terme de diffusion moleculaire
      Champ_Fonc& verrue = ref_cast_non_const(Champ_Fonc, diff_tot);
      if (!verrue.non_nul())
        {
          // juste pour typer le champ, on s'en fiche des valeurs...
          verrue = diff_turb;
        }
      verrue.valeur().affecter(diff_cinematique.valeur());
      verrue.valeur().valeurs() += diff_turb.valeur().valeurs();
      verrue.valeur().valeurs().echange_espace_virtuel();
    }
}

