/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <PolyVEF_P0_discretisation.h>
#include <Domaine_PolyVEF_P0.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Fonc_Elem_PolyMAC.h>
#include <Champ_Fonc_Elem_PolyVEF_P0_TC.h>
#include <Champ_Fonc_Elem_PolyVEF_P0_rot.h>
#include <Champ_Fonc_Tabule_Elem_PolyMAC.h>
#include <grad_Champ_Face_PolyVEF_P0.h>
#include <Milieu_base.h>
#include <Equation_base.h>
#include <Champ_Uniforme.h>
#include <DescStructure.h>
#include <Champ_Inc.h>
#include <Schema_Temps.h>
#include <Schema_Temps_base.h>
#include <Motcle.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Domaine_Cl_dis.h>

Implemente_instanciable(PolyVEF_P0_discretisation, "PolyVEF_P0", PolyVEF_P0P1NC_discretisation);

Entree& PolyVEF_P0_discretisation::readOn(Entree& s) { return s; }

Sortie& PolyVEF_P0_discretisation::printOn(Sortie& s) const { return s; }

void PolyVEF_P0_discretisation::grad_u(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  const Champ_Face_PolyVEF_P0& vit = ref_cast(Champ_Face_PolyVEF_P0, ch_vitesse.valeur());
  const Domaine_PolyVEF_P0& domaine_poly = ref_cast(Domaine_PolyVEF_P0, z.valeur());
  const Domaine_Cl_PolyMAC& domaine_cl_poly = ref_cast(Domaine_Cl_PolyMAC, zcl.valeur());
  ch.typer("grad_Champ_Face_PolyVEF_P0");

  grad_Champ_Face_PolyVEF_P0& ch_grad_u = ref_cast(grad_Champ_Face_PolyVEF_P0, ch.valeur()); //

  ch_grad_u.associer_domaine_dis_base(domaine_poly);
  ch_grad_u.associer_domaine_Cl_dis_base(domaine_cl_poly);
  ch_grad_u.associer_champ(vit);
  ch_grad_u.nommer("gradient_vitesse");
  const int D = dimension, N = vit.valeurs().line_size()/D;
  ch_grad_u.fixer_nb_comp(D * D * N);

  for (int n=0; n<N; n++)
    if (dimension == 2)
      {
        ch_grad_u.fixer_nom_compo(N*0+n,"dUdX"); // du/dx
        ch_grad_u.fixer_nom_compo(N*1+n,"dUdY"); // du/dy
        ch_grad_u.fixer_nom_compo(N*2+n,"dVdX"); // dv/dx
        ch_grad_u.fixer_nom_compo(N*3+n,"dVdY"); // dv/dy
      }
    else
      {
        ch_grad_u.fixer_nom_compo(N*0+n,"dUdX"); // du/dx
        ch_grad_u.fixer_nom_compo(N*1+n,"dUdY"); // du/dy
        ch_grad_u.fixer_nom_compo(N*2+n,"dUdZ"); // du/dz
        ch_grad_u.fixer_nom_compo(N*3+n,"dVdX"); // dv/dx
        ch_grad_u.fixer_nom_compo(N*4+n,"dVdY"); // dv/dy
        ch_grad_u.fixer_nom_compo(N*5+n,"dVdZ"); // dv/dz
        ch_grad_u.fixer_nom_compo(N*6+n,"dWdX"); // dw/dx
        ch_grad_u.fixer_nom_compo(N*7+n,"dWdY"); // dw/dy
        ch_grad_u.fixer_nom_compo(N*8+n,"dWdZ"); // dw/dz
      }
  ch_grad_u.fixer_nature_du_champ(multi_scalaire);
  ch_grad_u.fixer_nb_valeurs_nodales(domaine_poly.nb_elem());
  ch_grad_u.fixer_unite("s-1");
  ch_grad_u.changer_temps(-1.e8);
}

void PolyVEF_P0_discretisation::taux_cisaillement(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  const Champ_Face_PolyVEF_P0& vit = ref_cast(Champ_Face_PolyVEF_P0, ch_vitesse.valeur());
//  const Domaine_PolyVEF_P0&          domaine_poly = ref_cast(Domaine_PolyVEF_P0, z.valeur());
  const Domaine_PolyVEF_P0& domaine = ref_cast(Domaine_PolyVEF_P0, vit.domaine_dis_base());

  ch.typer("Champ_Fonc_Elem_PolyVEF_P0_TC");
  Champ_Fonc_Elem_PolyVEF_P0_TC& ch_grad_u = ref_cast(Champ_Fonc_Elem_PolyVEF_P0_TC, ch.valeur()); //

  ch_grad_u.associer_domaine_dis_base(domaine);
  ch_grad_u.associer_champ(vit);
  ch_grad_u.nommer("Taux_cisaillement");
  ch_grad_u.fixer_nb_comp(vit.valeurs().line_size());

  ch_grad_u.fixer_nature_du_champ(scalaire); // tensoriel pour etre precis
  ch_grad_u.fixer_nb_valeurs_nodales(domaine.nb_elem());
  ch_grad_u.fixer_unite("s-1");
  ch_grad_u.changer_temps(-1); // so it is calculated at time 0
}

void PolyVEF_P0_discretisation::creer_champ_vorticite(const Schema_Temps_base& sch, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  const Champ_Face_PolyVEF_P0& vit = ref_cast(Champ_Face_PolyVEF_P0, ch_vitesse.valeur());
  const Domaine_PolyVEF_P0& domaine = ref_cast(Domaine_PolyVEF_P0, vit.domaine_dis_base());
  int N = vit.valeurs().line_size()/dimension;

  ch.typer("Champ_Fonc_Elem_PolyVEF_P0_rot");
  Champ_Fonc_Elem_PolyVEF_P0_rot& ch_rot_u = ref_cast(Champ_Fonc_Elem_PolyVEF_P0_rot, ch.valeur());

  ch_rot_u.associer_domaine_dis_base(domaine);
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
  else
    abort();

  ch_rot_u.fixer_nb_valeurs_nodales(domaine.nb_elem());
  ch_rot_u.fixer_unite("s-1");
  ch_rot_u.changer_temps(-1.e8); // so it is calculated at time 0
}

void PolyVEF_P0_discretisation::residu( const Domaine_dis& z, const Champ_Inc& ch_inco, Champ_Fonc& champ ) const
{
  Nom ch_name(ch_inco.le_nom());
  ch_name += "_residu";
  Cerr << "Discretization of " << ch_name << finl;

  Nom type_ch = ch_inco.valeur().que_suis_je();
  if (type_ch.debute_par("Champ_Face"))
    {
      Motcle loc = "champ_face";
      Noms nom(1), unites(1);
      nom[0] = ch_name;
      unites[0] = "units_not_defined";
      int nb_comp = ch_inco.valeurs().line_size()*dimension;

      discretiser_champ(loc,z.valeur(), vectoriel, nom ,unites,nb_comp,ch_inco.temps(),champ);

      Champ_Fonc_base& ch_fonc = ref_cast(Champ_Fonc_base,champ.valeur());
      DoubleTab& tab=ch_fonc.valeurs();
      tab = -10000.0 ;
      Cerr << "[Information] Discretisation_base::residu : the residue is set to -10000.0 at initial time" <<finl;
    }

  else
    PolyVEF_P0P1NC_discretisation::residu(z, ch_inco, champ);
}
