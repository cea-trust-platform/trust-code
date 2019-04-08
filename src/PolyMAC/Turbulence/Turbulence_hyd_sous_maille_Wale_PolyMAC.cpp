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
// File:        Turbulence_hyd_sous_maille_Wale_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Turbulence
// Version:     /main/30
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_hyd_sous_maille_Wale_PolyMAC.h>
#include <Champ_Face_PolyMAC.h>
#include <DoubleTrav.h>
#include <Schema_Temps_base.h>
#include <Param.h>
#include <Equation_base.h>
#include <Zone_PolyMAC.h>

Implemente_instanciable_sans_constructeur(Turbulence_hyd_sous_maille_Wale_PolyMAC,"Modele_turbulence_hyd_sous_maille_Wale_PolyMAC",Mod_turb_hyd_ss_maille_PolyMAC);

Turbulence_hyd_sous_maille_Wale_PolyMAC::Turbulence_hyd_sous_maille_Wale_PolyMAC()
{
  declare_support_masse_volumique(1);
  cw=0.5;
}

//// printOn
//


Sortie& Turbulence_hyd_sous_maille_Wale_PolyMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


//// readOn
//

Entree& Turbulence_hyd_sous_maille_Wale_PolyMAC::readOn(Entree& is )
{
  Mod_turb_hyd_ss_maille_PolyMAC::readOn(is);
  return is;
}

void Turbulence_hyd_sous_maille_Wale_PolyMAC::set_param(Param& param)
{
  Mod_turb_hyd_ss_maille_PolyMAC::set_param(param);
  param.ajouter("cw",&cw);
  param.ajouter_condition("value_of_cw_ge_0","sous_maille_Wale_PolyMAC model constant must be positive.");
}

///////////////////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Turbulence_hyd_sous_maille_Wale_PolyMAC
//
//////////////////////////////////////////////////////////////////////////////


Champ_Fonc& Turbulence_hyd_sous_maille_Wale_PolyMAC::calculer_viscosite_turbulente()
{
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  double temps = mon_equation->inconnue().temps();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  if (est_egal(cw,0.,1.e-15))
    {
      visco_turb=0.;
    }
  else
    {
      int nb_elem = zone_PolyMAC.zone().nb_elem();
      const int nb_elem_tot = zone_PolyMAC.nb_elem_tot();

      OP1.resize(nb_elem_tot);  // OP1 est le premier operateur spatial du modele WALE.
      OP2.resize(nb_elem_tot);  // OP2 est le deuxieme operateur spatial du modele WALE.

      calculer_OP1_OP2();

      if (visco_turb.size() != nb_elem)
        {
          Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
          exit();
        }

      for (int elem=0; elem<nb_elem; elem++)
        {
          if (OP1[elem]!=0.) // donc sd2 (et OP2 par voie de consequence) sont differents de zero
            visco_turb[elem]=cw*cw*l_(elem)*l_(elem)*OP1[elem]/OP2[elem];
          else
            visco_turb[elem]=0;
        }
    }

  la_viscosite_turbulente.changer_temps(temps);

  return la_viscosite_turbulente;
}


void Turbulence_hyd_sous_maille_Wale_PolyMAC::calculer_OP1_OP2()
{


  Champ_Face_PolyMAC& vit = ref_cast(Champ_Face_PolyMAC, mon_equation->inconnue().valeur());
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  int nb_elem = zone_PolyMAC.zone().nb_elem_tot();

  int i,j,k,elem;
  IntVect element(4);

  DoubleTrav gij2(dimension,dimension);
  DoubleTrav sd(dimension,dimension);

  double gkk2;
  double sd2;
  double Sij,Sij2;


  DoubleTab duidxj(0,dimension,dimension);
  zone_PolyMAC.zone().creer_tableau_elements(duidxj);

  vit.interp_gve(vitesse, duidxj);

  for(elem=0 ; elem<nb_elem ; elem ++)
    {

      //Calcul du terme gij2
      for(i=0; i<dimension; i++)
        for(j=0; j<dimension; j++)
          {
            gij2(i,j)=0;

            for(k=0; k<dimension; k++)
              {
                gij2(i,j)+=duidxj(elem,i,k)*duidxj(elem,k,j);
              }
          }

      // Calcul du terme gkk2
      gkk2=0;
      for(k=0; k<dimension; k++)
        {
          gkk2 += gij2(k,k);
        }

      // Calcul de sd
      for(i=0; i<dimension; i++)
        for(j=0; j<dimension; j++)
          {
            sd(i,j)=0.5*(gij2(i,j)+gij2(j,i));
            if(i==j)
              {
                sd(i,j)-=gkk2/3.; // Terme derriere le tenseur de Kronecker
              }
          }

      // Calcul de sd2 et Sij2
      sd2=0.;
      Sij2=0.;

      for ( i=0 ; i<dimension ; i++)
        for ( j=0 ; j<dimension ; j++)
          {
            sd2+=sd(i,j)*sd(i,j);
            //Deplacement du calcul de sij
            Sij=0.5*(duidxj(elem,i,j) + duidxj(elem,j,i));

            Sij2+=Sij*Sij;
          }

      // Calcul de OP1 et OP2
      OP1(elem)=pow(sd2,1.5);
      OP2(elem)=pow(Sij2,2.5)+pow(sd2,1.25);

    }// fin de la boucle sur les elements
}

