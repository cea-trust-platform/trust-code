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
// File:        Turbulence_hyd_sous_maille_Smago_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/39
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_hyd_sous_maille_Smago_VDF.h>
#include <Champ_Face.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Param.h>
#include <Zone_VDF.h>
#include <Equation_base.h>

Implemente_instanciable_sans_constructeur(Turbulence_hyd_sous_maille_Smago_VDF,"Modele_turbulence_hyd_sous_maille_Smago_VDF",Mod_turb_hyd_ss_maille_VDF);

Turbulence_hyd_sous_maille_Smago_VDF::Turbulence_hyd_sous_maille_Smago_VDF()
{
  cs=0.18;
}

//// printOn
//

Sortie& Turbulence_hyd_sous_maille_Smago_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Turbulence_hyd_sous_maille_Smago_VDF::readOn(Entree& is )
{
  Mod_turb_hyd_ss_maille_VDF::readOn(is);
  return is;
}

void Turbulence_hyd_sous_maille_Smago_VDF::set_param(Param& param)
{
  Mod_turb_hyd_ss_maille_VDF::set_param(param);
  param.ajouter("cs",&cs);
  param.ajouter_condition("value_of_cs_ge_0","sous_maille_smago model constant must be positive.");
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Turbulence_hyd_sous_maille_Smago_VDF
//
//////////////////////////////////////////////////////////////////////////////

Champ_Fonc& Turbulence_hyd_sous_maille_Smago_VDF::calculer_viscosite_turbulente()
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  double temps = mon_equation->inconnue().temps();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  int nb_elem = zone_VDF.zone().nb_elem();
  const int nb_elem_tot = zone_VDF.nb_elem_tot();

  SMA_barre_.resize(nb_elem_tot);
  calculer_S_barre();
  if (visco_turb.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }

  Debog::verifier("Turbulence_hyd_sous_maille_Smago_VDF::calculer_viscosite_turbulente visco_turb 0",visco_turb);

  for (int elem=0; elem<nb_elem; elem++)
    visco_turb[elem]=cs*cs*l_(elem)*l_(elem)*sqrt(SMA_barre_(elem));

  Debog::verifier("Turbulence_hyd_sous_maille_Smago_VDF::calculer_viscosite_turbulente visco_turb 1",visco_turb);

  la_viscosite_turbulente.changer_temps(temps);
  return la_viscosite_turbulente;
}

void Turbulence_hyd_sous_maille_Smago_VDF::calculer_S_barre()
{
  Champ_Face& vit = ref_cast(Champ_Face, mon_equation->inconnue().valeur());
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const Zone_Cl_VDF& zone_Cl_VDF = la_zone_Cl_VDF.valeur();
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const int nb_elem_tot = zone_VDF.nb_elem_tot();

  int i,j;
  int elem;
  DoubleTab psc(dimension,dimension);

  DoubleTab duidxj(nb_elem_tot,dimension,dimension);

  vit.calcul_duidxj(vitesse,duidxj,zone_Cl_VDF);

  double Sij,temp;
  for (elem=0 ; elem<nb_elem_tot; elem++)
    {
      temp = 0.;
      for ( i=0 ; i<dimension ; i++)
        for ( j=0 ; j<dimension ; j++)
          //Deplacement du calcul de Sij
          {
            Sij=0.5*(duidxj(elem,i,j) + duidxj(elem,j,i));
            temp+=Sij*Sij;
          }
      SMA_barre_(elem)=2.*temp;
    }
}

