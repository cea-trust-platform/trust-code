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
// File:        PolyMAC_P0_discretisation.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <PolyMAC_P0_discretisation.h>
#include <Zone_PolyMAC_P0.h>
#include <Champ_Fonc_Tabule.h>
//#include <Ch_Fonc_Som_PolyMAC_P0.h>
#include <Champ_Fonc_Elem_PolyMAC.h>
#include <Champ_Fonc_Elem_PolyMAC_P0_TC.h>
#include <Champ_Fonc_Elem_PolyMAC_P0_rot.h>
#include <Champ_Fonc_Tabule_Elem_PolyMAC.h>
#include <grad_Champ_Face_PolyMAC_P0.h>

#include <Milieu_base.h>
#include <Equation_base.h>
#include <Champ_Uniforme.h>
#include <DescStructure.h>
#include <Champ_Inc.h>
#include <Schema_Temps.h>
#include <Schema_Temps_base.h>
#include <Motcle.h>
#include <Zone_Cl_PolyMAC.h>
#include <Zone_Cl_dis.h>
#include <Synonyme_info.h>

Implemente_instanciable(PolyMAC_P0_discretisation,"PolyMAC_P0|CoviMAC",PolyMAC_discretisation);
Add_synonym(PolyMAC_P0_discretisation,"PolyMAC_V2");


Entree& PolyMAC_P0_discretisation::readOn(Entree& s)
{
  return s ;
}

Sortie& PolyMAC_P0_discretisation::printOn(Sortie& s) const
{
  return s ;
}

void PolyMAC_P0_discretisation::y_plus(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{

#ifdef dependance
  Cerr << "Discretisation de y_plus" << finl;
  const Champ_Som_PolyMAC_P0& vit = ref_cast(Champ_Som_PolyMAC_P0,ch_vitesse.valeur());
  const Zone_PolyMAC_P0& zone_PolyMAC_P0=ref_cast(Zone_PolyMAC_P0, z.valeur());
  const Zone_Cl_PolyMAC& Zone_Cl_PolyMAC=ref_cast(Zone_Cl_PolyMAC, zcl.valeur());
  ch.typer("Y_plus_Champ_Som_PolyMAC_P0");
  Y_plus_Champ_Som_PolyMAC_P0& ch_yp=ref_cast(Y_plus_Champ_Som_PolyMAC_P0,ch.valeur());
  ch_yp.associer_zone_dis_base(zone_PolyMAC_P0);
  ch_yp.associer_zone_Cl_dis_base(Zone_Cl_PolyMAC);
  ch_yp.associer_champ(vit);
  ch_yp.nommer("Y_plus");
  ch_yp.fixer_nb_comp(1);
  ch_yp.fixer_nb_valeurs_nodales(zone_PolyMAC_P0.nb_elem());
  ch_yp.fixer_unite("adimensionnel");
  ch_yp.changer_temps(ch_vitesse.temps());
#endif
}

void PolyMAC_P0_discretisation::grad_u(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse,Champ_Fonc& ch) const
{
  const Champ_Face_PolyMAC_P0&          vit = ref_cast(Champ_Face_PolyMAC_P0,ch_vitesse.valeur());
  const Zone_PolyMAC_P0&          zone_poly = ref_cast(Zone_PolyMAC_P0, z.valeur());
  const Zone_Cl_PolyMAC&    zone_cl_poly = ref_cast(Zone_Cl_PolyMAC, zcl.valeur());

  ch.typer("grad_Champ_Face_PolyMAC_P0");

  grad_Champ_Face_PolyMAC_P0&   ch_grad_u = ref_cast(grad_Champ_Face_PolyMAC_P0,ch.valeur()); //

  ch_grad_u.associer_zone_dis_base(zone_poly);
  ch_grad_u.associer_zone_Cl_dis_base(zone_cl_poly);
  ch_grad_u.associer_champ(vit);
  ch_grad_u.nommer("gradient_vitesse");
  ch_grad_u.fixer_nb_comp(dimension * dimension * vit.valeurs().line_size());

  for (int n = 0; n<ch_grad_u.valeurs().line_size(); n++)
    {
      Nom phase   = Nom(n);
      if (dimension == 2)
        {
          ch_grad_u.fixer_nom_compo(dimension*n+0,Nom("dU_")+phase); // dU
          ch_grad_u.fixer_nom_compo(dimension*n+1,Nom("dV_")+phase); // dV
        }
      else
        {
          ch_grad_u.fixer_nom_compo(dimension*n+0,Nom("dU_")+phase); // dU
          ch_grad_u.fixer_nom_compo(dimension*n+1,Nom("dV_")+phase); // dV
          ch_grad_u.fixer_nom_compo(dimension*n+2,Nom("dW_")+phase); // dW
        }
    }
  ch_grad_u.fixer_nature_du_champ(multi_scalaire); // tensoriel pour etre precis
  ch_grad_u.fixer_nb_valeurs_nodales(-1);
  ch_grad_u.fixer_unite("s-1");
  ch_grad_u.changer_temps(-1); // so it is calculated at time 0
}

void PolyMAC_P0_discretisation::taux_cisaillement(const Zone_dis& z, const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  const Champ_Face_PolyMAC_P0&          vit = ref_cast(Champ_Face_PolyMAC_P0,ch_vitesse.valeur());
//  const Zone_PolyMAC_P0&          zone_poly = ref_cast(Zone_PolyMAC_P0, z.valeur());
  const Zone_PolyMAC_P0&          zone = ref_cast(Zone_PolyMAC_P0, vit.zone_dis_base());

  ch.typer("Champ_Fonc_Elem_PolyMAC_P0_TC");
  Champ_Fonc_Elem_PolyMAC_P0_TC&   ch_grad_u = ref_cast(Champ_Fonc_Elem_PolyMAC_P0_TC,ch.valeur()); //

  ch_grad_u.associer_zone_dis_base(zone);
  ch_grad_u.associer_champ(vit);
  ch_grad_u.nommer("Taux_cisaillement");
  ch_grad_u.fixer_nb_comp(vit.valeurs().line_size());

  ch_grad_u.fixer_nature_du_champ(scalaire); // tensoriel pour etre precis
  ch_grad_u.fixer_nb_valeurs_nodales(zone.nb_elem());
  ch_grad_u.fixer_unite("s-1");
  ch_grad_u.changer_temps(-1); // so it is calculated at time 0
}

void PolyMAC_P0_discretisation::creer_champ_vorticite(const Schema_Temps_base& sch,
                                                      const Champ_Inc& ch_vitesse,
                                                      Champ_Fonc& ch) const
{
  const Champ_Face_PolyMAC_P0&     vit = ref_cast(Champ_Face_PolyMAC_P0,ch_vitesse.valeur());
  const Zone_PolyMAC_P0&          zone = ref_cast(Zone_PolyMAC_P0, vit.zone_dis_base());
  int N = vit.valeurs().line_size();

  ch.typer("Champ_Fonc_Elem_PolyMAC_P0_rot");
  Champ_Fonc_Elem_PolyMAC_P0_rot&  ch_rot_u = ref_cast(Champ_Fonc_Elem_PolyMAC_P0_rot,ch.valeur());

  ch_rot_u.associer_zone_dis_base(zone);
  ch_rot_u.associer_champ(vit);
  ch_rot_u.nommer("vorticite");

  if (dimension == 2)
    {
      ch_rot_u.fixer_nb_comp(N);
      ch_rot_u.fixer_nature_du_champ(scalaire);
    }
  else if (dimension == 3)
    {
      ch_rot_u.fixer_nb_comp(dimension * N);
      ch_rot_u.fixer_nature_du_champ(vectoriel);
    }
  else abort();

  ch_rot_u.fixer_nb_valeurs_nodales(zone.nb_elem());
  ch_rot_u.fixer_unite("s-1");
  ch_rot_u.changer_temps(-1); // so it is calculated at time 0
}
