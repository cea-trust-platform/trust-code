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
// File:        Turbulence_hyd_sous_maille_Wale_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_hyd_sous_maille_Wale_VEF.h>
#include <Champ_P1NC.h>
#include <DoubleTrav.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Param.h>
#include <Equation_base.h>
#include <Zone_VEF.h>

Implemente_instanciable_sans_constructeur(Turbulence_hyd_sous_maille_Wale_VEF,"Modele_turbulence_hyd_sous_maille_Wale_VEF",Mod_turb_hyd_ss_maille_VEF);

Turbulence_hyd_sous_maille_Wale_VEF::Turbulence_hyd_sous_maille_Wale_VEF()
{
  declare_support_masse_volumique(1);
  cw=0.5;
}
//// printOn
//

Sortie& Turbulence_hyd_sous_maille_Wale_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Turbulence_hyd_sous_maille_Wale_VEF::readOn(Entree& is )
{
  Mod_turb_hyd_ss_maille_VEF::readOn(is);
  return is;
}

void Turbulence_hyd_sous_maille_Wale_VEF::set_param(Param& param)
{
  Mod_turb_hyd_ss_maille_VEF::set_param(param);
  param.ajouter("cw",&cw);
  param.ajouter_condition("value_of_cw_ge_0","sous_maille_Wale model constant must be positive.");
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Turbulence_hyd_sous_maille_Wale_VEF
//
//////////////////////////////////////////////////////////////////////////////


Champ_Fonc& Turbulence_hyd_sous_maille_Wale_VEF::calculer_viscosite_turbulente()
{
  // cw est la constante du modele WALE qui correspond a une correction
  //  de la constante Cs du modele de Smagorinsky.
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  double temps = mon_equation->inconnue().temps();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  const int nb_elem = zone_VEF.nb_elem();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();

  OP1.resize(nb_elem_tot);  // OP1 est le premier operateur spatial du modele WALE.
  OP2.resize(nb_elem_tot);  // OP2 est le deuxieme operateur spatial du modele WALE.

  calculer_OP1_OP2();

  if (visco_turb.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }


  Debog::verifier("Turbulence_hyd_sous_maille_Wale_VEF::calculer_viscosite_turbulente visco_turb 0",visco_turb);

  for (int elem=0; elem<nb_elem; elem++)
    {
      if (OP1[elem]!=0.) // donc sd2 et OP2 par voie de consequence sont differents de zero
        visco_turb[elem]=cw*cw*l_(elem)*l_(elem)*OP1[elem]/OP2[elem];
      else
        visco_turb[elem]=0;
    }

  Debog::verifier("Turbulence_hyd_sous_maille_Wale_VEF::calculer_viscosite_turbulente visco_turb 1",visco_turb);

  la_viscosite_turbulente.changer_temps(temps);
  return la_viscosite_turbulente;
}

void Turbulence_hyd_sous_maille_Wale_VEF::calculer_OP1_OP2()
{
  const DoubleTab& la_vitesse = mon_equation->inconnue().valeurs();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zone_Cl_VEF.valeur();
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_elem = zone_VEF.nb_elem_tot();

  DoubleTrav gij2(dimension,dimension);
  DoubleTrav sd(dimension,dimension);

  // Patrick : on travaille sur le champ filtre.
  //const Champ_P1NC& ch=(const Champ_P1NC&) mon_equation->inconnue().valeur();
  DoubleTab ubar(la_vitesse);
  //  ch.filtrer_L2(ubar);


  DoubleTab duidxj(nb_elem,dimension,dimension);

  Champ_P1NC::calcul_gradient(ubar,duidxj,zone_Cl_VEF);

  for (int elem=0; elem<nb_elem; elem ++)
    {

      // Calcul du terme gij2.
      for(int i=0; i<dimension; i++)
        for(int j=0; j<dimension; j++)
          {
            gij2(i,j)=0;
            for(int k=0; k<dimension; k++)
              gij2(i,j)+=duidxj(elem,i,k)*duidxj(elem,k,j);
          }

      // Calcul du terme gkk2.
      double gkk2=0;
      for(int k=0; k<dimension; k++)
        gkk2 += gij2(k,k);

      // Calcul de sd.
      for(int i=0; i<dimension; i++)
        for(int j=0; j<dimension; j++)
          {
            sd(i,j)=0.5*(gij2(i,j)+gij2(j,i));
            if(i==j)
              {
                sd(i,j)-=gkk2/3.; // Terme derriere le tenseur de Kronecker.
              }
          }

      // Calcul de sd2 et Sij2.
      double sd2=0.;
      double Sij2=0.;
      for (int i=0 ; i<dimension ; i++)
        for (int j=0 ; j<dimension ; j++)
          {
            sd2+=sd(i,j)*sd(i,j);
            //Deplacement du calcul de Sij
            double Sij=0.5*(duidxj(elem,i,j) + duidxj(elem,j,i));
            Sij2+=Sij*Sij;
          }

      // Calcul de OP1 et OP2.
      // Replace pow by sqrt and multiply, faster
      //OP1(elem)=pow(sd2,1.5);
      OP1(elem)=sd2*sqrt(sd2);
      //OP2(elem)=pow(Sij2,2.5)+pow(sd2,1.25);
      OP2(elem)=Sij2*Sij2*sqrt(Sij2)+sd2*sqrt(sqrt(sd2));

    }// fin de la boucle sur les elements
}

