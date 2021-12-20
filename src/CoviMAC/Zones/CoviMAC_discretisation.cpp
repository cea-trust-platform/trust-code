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
// File:        CoviMAC_discretisation.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <CoviMAC_discretisation.h>
#include <Zone_CoviMAC.h>
#include <Champ_Fonc_Tabule.h>
//#include <Ch_Fonc_P1_CoviMAC.h>
#include <Champ_Fonc_P0_CoviMAC.h>
#include <Champ_Fonc_P0_CoviMAC_TC.h>
#include <Champ_Fonc_P0_CoviMAC_rot.h>
#include <Champ_Fonc_Tabule_P0_CoviMAC.h>
#include <grad_Champ_Face_CoviMAC.h>

#include <Milieu_base.h>
#include <Equation_base.h>
#include <Champ_Uniforme.h>
#include <DescStructure.h>
#include <Champ_Inc.h>
#include <Schema_Temps.h>
#include <Schema_Temps_base.h>
#include <Motcle.h>
#include <Zone_Cl_CoviMAC.h>
#include <Zone_Cl_dis.h>

Implemente_instanciable(CoviMAC_discretisation,"CoviMAC",Discret_Thyd);



Entree& CoviMAC_discretisation::readOn(Entree& s)
{
  return s ;
}

Sortie& CoviMAC_discretisation::printOn(Sortie& s) const
{
  return s ;
}

// Description:
// Discretisation d'un champ pour le CoviMAC en fonction d'une directive
// de discretisation. La directive est un Motcle comme "vitesse", "pression",
// "temperature", "champ_elem" (cree un champ de type P0), ...
// Cette methode determine le type du champ a creer en fonction du type d'element
// et de la directive de discretisation. Elle determine ensuite le nombre de ddl
// et fixe l'ensemble des parametres du champ (type, nb_compo, nb_ddl, nb_pas_dt,
// nom(s), unite(s) et nature du champ) et associe la Zone_dis au champ.
// Voir le code pour avoir la correspondance entre les directives et
// le type de champ cree.
void CoviMAC_discretisation::discretiser_champ(
  const Motcle& directive, const Zone_dis_base& z,
  Nature_du_champ nature,
  const Noms& noms, const Noms& unites,
  int nb_comp, int nb_pas_dt, double temps,
  Champ_Inc& champ, const Nom& sous_type ) const
{
  const Zone_CoviMAC& zone_CoviMAC = ref_cast(Zone_CoviMAC, z);

  Motcles motcles(7);
  motcles[0] = "vitesse";     // Choix standard pour la vitesse
  motcles[1] = "pression";    // Choix standard pour la pression
  motcles[2] = "temperature"; // Choix standard pour la temperature
  motcles[3] = "divergence_vitesse"; // Le type de champ obtenu en calculant div v
  motcles[4] = "gradient_pression";  // Le type de champ obtenu en calculant grad P
  motcles[5] = "champ_elem";    // Creer un champ aux elements (de type P0)
  motcles[6] = "champ_sommets"; // Creer un champ aux sommets (type P1)

  // Le type de champ de vitesse depend du type d'element :
  Nom type_champ_vitesse("Champ_Face_CoviMAC");
  Nom type_elem("Champ_P0_CoviMAC");
  Nom type;
  int default_nb_comp = 0; // Valeur par defaut du nombre de composantes
  int rang = motcles.search(directive);
  switch(rang)
    {
    case 0:
    case 4:
      type = type_champ_vitesse;
      default_nb_comp = 3;
      break;
    case 1:
    case 2:
    case 3:
    case 5:
      type = type_elem;
      default_nb_comp = 1;
      break;
    default:
      assert(rang < 0);
      break;
    }

  if (directive == demande_description)
    Cerr << "CoviMAC_discretisation : " << motcles;

  if (sous_type != nom_vide)
    rang = verifie_sous_type(type,sous_type,directive);

  // Si on n'a pas compris la directive (ou si c'est une demande_description)
  // alors on appelle l'ancetre :
  if (rang < 0)
    {
      Discret_Thyd::discretiser_champ(directive, z,
                                      nature, noms, unites,
                                      nb_comp, nb_pas_dt, temps, champ);
      return;
    }

  // Calcul du nombre de ddl
  int nb_ddl = 0;
  if (type.debute_par(type_elem)) nb_ddl = z.nb_elem();
  else if (type.debute_par(type_champ_vitesse)) nb_ddl = zone_CoviMAC.nb_faces() + nb_comp * zone_CoviMAC.nb_elem();
  else if (type.debute_par("Champ_P1_CoviMAC")) nb_ddl = zone_CoviMAC.nb_som();
  else assert(0);

  // Si c'est un champ multiscalaire, uh !
  /* if (nature == multi_scalaire) {
    // Pas encore code
    Cerr << "Champ multi_scalaire pas code" << finl;
    assert(0); exit();
    } else {*/
  if (nb_comp < 0)
    nb_comp = default_nb_comp;
  assert(nb_comp > 0);
  creer_champ(champ, z, type, noms[0], unites[0], nb_comp, nb_ddl, nb_pas_dt, temps,
              directive, que_suis_je());
  if (nature == multi_scalaire)
    {
      champ.valeur().fixer_nature_du_champ(nature);
      champ.valeur().fixer_unites(unites);
      champ.valeur().fixer_noms_compo(noms);
    }

}

// Description:
// Idem que CoviMAC_discretisation::discretiser_champ(... , Champ_Inc)
void CoviMAC_discretisation::discretiser_champ(
  const Motcle& directive, const Zone_dis_base& z,
  Nature_du_champ nature,
  const Noms& noms, const Noms& unites,
  int nb_comp, double temps,
  Champ_Fonc& champ) const
{
  discretiser_champ_fonc_don(directive, z,
                             nature, noms, unites,
                             nb_comp, temps, champ);
}

// Description:
// Idem que CoviMAC_discretisation::discretiser_champ(... , Champ_Inc)
void CoviMAC_discretisation::discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
                                               Nature_du_champ nature,
                                               const Noms& noms, const Noms& unites,
                                               int nb_comp, double temps,
                                               Champ_Don& champ) const
{
  discretiser_champ_fonc_don(directive, z,
                             nature, noms, unites,
                             nb_comp, temps, champ);
}

// Description:
// Idem que CoviMAC_discretisation::discretiser_champ(... , Champ_Inc)
// Traitement commun aux champ_fonc et champ_don.
// Cette methode est privee (passage d'un Objet_U pas propre vu
// de l'exterieur ...)
void CoviMAC_discretisation::discretiser_champ_fonc_don(
  const Motcle& directive, const Zone_dis_base& z,
  Nature_du_champ nature,
  const Noms& noms, const Noms& unites,
  int nb_comp, double temps,
  Objet_U& champ) const
{
  // Deux pointeurs pour acceder facilement au champ_don ou au champ_fonc,
  // suivant le type de l'objet champ.
  Champ_Fonc * champ_fonc = 0;
  Champ_Don * champ_don = 0;
  if (sub_type(Champ_Fonc, champ))
    champ_fonc = & ref_cast(Champ_Fonc, champ);
  else
    champ_don  = & ref_cast(Champ_Don, champ);

  const Zone_CoviMAC& zone_CoviMAC = ref_cast(Zone_CoviMAC, z);

  Motcles motcles(8);
  motcles[0] = "pression";    // Choix standard pour la pression
  motcles[1] = "temperature"; // Choix standard pour la temperature
  motcles[2] = "divergence_vitesse"; // Le type de champ obtenu en calculant div v
  motcles[3] = "champ_elem";  // Creer un champ aux elements (de type P0)
  motcles[6] = "champ_sommets";  // Creer un champ aux elements (de type P1)
  motcles[4] = "vitesse";     // Choix standard pour la vitesse
  motcles[7] = "champ_face";     // Choix standard pour la vitesse
  motcles[5] = "gradient_pression";  // Le type de champ obtenu en calculant grad P

  // Le type de champ de vitesse depend du type d'element :
  Nom type_champ_vitesse("Champ_Fonc_Face_CoviMAC");
  Nom type_elem("Champ_Fonc_P0_CoviMAC");

  Nom type;
  int default_nb_comp = 0; // Valeur par defaut du nombre de composantes
  int rang = motcles.search(directive);
  switch(rang)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      type = type_elem;
      default_nb_comp = 1;
      break;
    case 4:
    case 5:
    case 7:
      type = type_champ_vitesse;
      default_nb_comp = 3;
      break;
    case 6:
      type = "Champ_Fonc_P1_CoviMAC";
      default_nb_comp = 1;
      break;
    default:
      assert(rang < 0);
      break;
    }

  if (directive == demande_description)
    Cerr << "CoviMAC_discretisation : " << motcles;

  // Si on n'a pas compris la directive (ou si c'est une demande_description)
  // alors on appelle l'ancetre :
  if (rang < 0)
    {
      if (champ_fonc)
        Discret_Thyd::discretiser_champ(directive, z, nature, noms, unites,
                                        nb_comp, temps, *champ_fonc);
      else
        Discret_Thyd::discretiser_champ(directive, z, nature, noms, unites,
                                        nb_comp, temps, *champ_don);
      return;
    }

  // Calcul du nombre de ddl
  int nb_ddl = 0;
  if (type == "Champ_Fonc_P0_CoviMAC")
    nb_ddl = z.nb_elem();
  else if (type == type_champ_vitesse)
    nb_ddl = zone_CoviMAC.nb_faces();
  else if (type == "Champ_Fonc_P1_CoviMAC")
    nb_ddl = zone_CoviMAC.nb_som();
  else
    assert(0);

  // Si c'est un champ multiscalaire, uh !
  if (nb_comp < 0)
    nb_comp = default_nb_comp;
  if (champ_fonc)
    creer_champ(*champ_fonc, z, type, noms[0], unites[0], nb_comp, nb_ddl, temps,
                directive, que_suis_je());
  else
    creer_champ(*champ_don, z, type, noms[0], unites[0], nb_comp, nb_ddl, temps,
                directive, que_suis_je());
  if ((nature == multi_scalaire) && (champ_fonc))
    {
      champ_fonc->valeur().fixer_nature_du_champ(nature);
      champ_fonc->valeur().fixer_unites(unites);
      champ_fonc->valeur().fixer_noms_compo(noms);
    }
  else if ((nature == multi_scalaire) && (champ_don))
    {
      Cerr<<"There is no field of type Champ_Don with a multi_scalaire nature."<<finl;
      exit();
    }
}



void CoviMAC_discretisation::distance_paroi(const Schema_Temps_base& sch,
                                            Zone_dis& z, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de la distance paroi" << finl;
  Zone_CoviMAC& zone_CoviMAC=ref_cast(Zone_CoviMAC, z.valeur());
  ch.typer("Champ_Fonc_P0_CoviMAC");
  Champ_Fonc_P0_CoviMAC& ch_dist_paroi=ref_cast(Champ_Fonc_P0_CoviMAC,ch.valeur());
  ch_dist_paroi.associer_zone_dis_base(zone_CoviMAC);
  ch_dist_paroi.nommer("distance_paroi");
  ch_dist_paroi.fixer_nb_comp(1);
  ch_dist_paroi.fixer_nb_valeurs_nodales(zone_CoviMAC.nb_elem());
  ch_dist_paroi.fixer_unite("m");
  ch_dist_paroi.changer_temps(sch.temps_courant());
}

void CoviMAC_discretisation::proprietes_physiques_fluide_Ostwald(const Zone_dis& z, Fluide_Ostwald& le_fluide,
                                                                 const Navier_Stokes_std& eqn_hydr, const Champ_Inc& ch_temper ) const
{
  Cerr << "CoviMAC_discretisation::proprietes_physiques_fluide_Ostwald isn't coded" << finl; // voir fonction similaire dans discretisation_VDF pour le coder
  abort();
}


void CoviMAC_discretisation::zone_Cl_dis(Zone_dis& z,
                                         Zone_Cl_dis& zcl) const
{
  Cerr << "discretisation des conditions limites" << finl;
  assert(z.non_nul());
  Zone_CoviMAC& zone_CoviMAC=ref_cast(Zone_CoviMAC, z.valeur());
  zcl.typer("Zone_Cl_CoviMAC");
  assert(zcl.non_nul());
  Zone_Cl_CoviMAC& zone_cl_CoviMAC=ref_cast(Zone_Cl_CoviMAC, zcl.valeur());
  zone_cl_CoviMAC.associer(zone_CoviMAC);
  Cerr << "discretisation des conditions limites OK" << finl;
}


void CoviMAC_discretisation::critere_Q(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
	  Cerr << "CoviMAC_discretisation::critere_Q isn't coded" << finl; // voir fonction similaire dans VDF_discretisation pour le coder
	  abort();
}


void CoviMAC_discretisation::y_plus(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{

#ifdef dependance
  Cerr << "Discretisation de y_plus" << finl;
  const Champ_P1_CoviMAC& vit = ref_cast(Champ_P1_CoviMAC,ch_vitesse.valeur());
  const Zone_CoviMAC& zone_CoviMAC=ref_cast(Zone_CoviMAC, z.valeur());
  const Zone_Cl_CoviMAC& zone_cl_CoviMAC=ref_cast(Zone_Cl_CoviMAC, zcl.valeur());
  ch.typer("Y_plus_Champ_P1_CoviMAC");
  Y_plus_Champ_P1_CoviMAC& ch_yp=ref_cast(Y_plus_Champ_P1_CoviMAC,ch.valeur());
  ch_yp.associer_zone_dis_base(zone_CoviMAC);
  ch_yp.associer_zone_Cl_dis_base(zone_cl_CoviMAC);
  ch_yp.associer_champ(vit);
  ch_yp.nommer("Y_plus");
  ch_yp.fixer_nb_comp(1);
  ch_yp.fixer_nb_valeurs_nodales(zone_CoviMAC.nb_elem());
  ch_yp.fixer_unite("adimensionnel");
  ch_yp.changer_temps(ch_vitesse.temps());
#endif
}

void CoviMAC_discretisation::grad_T(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_temperature, Champ_Fonc& ch) const
{
	  Cerr << "CoviMAC_discretisation::grad_T isn't coded" << finl; // voir fonction similaire dans VDF_discretisation pour le coder
	  abort();
}

void CoviMAC_discretisation::grad_u(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse,Champ_Fonc& ch) const
{
  const Champ_Face_CoviMAC&          vit = ref_cast(Champ_Face_CoviMAC,ch_vitesse.valeur());
  const Zone_CoviMAC&          zone_poly = ref_cast(Zone_CoviMAC, z.valeur());
  const Zone_Cl_CoviMAC&    zone_cl_poly = ref_cast(Zone_Cl_CoviMAC, zcl.valeur());

  ch.typer("grad_Champ_Face_CoviMAC");

  grad_Champ_Face_CoviMAC&   ch_grad_u = ref_cast(grad_Champ_Face_CoviMAC,ch.valeur()); //

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
          ch_grad_u.fixer_nom_compo(dimension*n+2,Nom("dX_")+phase); // dW
        }
    }
  ch_grad_u.fixer_nature_du_champ(multi_scalaire); // tensoriel pour etre precis
  ch_grad_u.fixer_nb_valeurs_nodales(zone_poly.nb_faces_tot() + dimension * zone_poly.nb_elem());
  ch_grad_u.fixer_unite("s-1");
  ch_grad_u.changer_temps(-1); // so it is calculated at time 0
}

void CoviMAC_discretisation::taux_cisaillement(const Zone_dis& z, const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  const Champ_Face_CoviMAC&          vit = ref_cast(Champ_Face_CoviMAC,ch_vitesse.valeur());
//  const Zone_CoviMAC&          zone_poly = ref_cast(Zone_CoviMAC, z.valeur());
  const Zone_CoviMAC&          zone = ref_cast(Zone_CoviMAC, vit.zone_dis_base());

  ch.typer("Champ_Fonc_P0_CoviMAC_TC");
  Champ_Fonc_P0_CoviMAC_TC&   ch_grad_u = ref_cast(Champ_Fonc_P0_CoviMAC_TC,ch.valeur()); //

  ch_grad_u.associer_zone_dis_base(zone);
  ch_grad_u.associer_champ(vit);
  ch_grad_u.nommer("Taux_cisaillement");
  ch_grad_u.fixer_nb_comp(vit.valeurs().line_size());

  ch_grad_u.fixer_nature_du_champ(scalaire); // tensoriel pour etre precis
  ch_grad_u.fixer_nb_valeurs_nodales(zone.nb_elem_tot());
  ch_grad_u.fixer_unite("s-1");
  ch_grad_u.changer_temps(-1); // so it is calculated at time 0
}

void CoviMAC_discretisation::creer_champ_vorticite(const Schema_Temps_base& sch,
                                                   const Champ_Inc& ch_vitesse,
                                                   Champ_Fonc& ch) const
{
  const Champ_Face_CoviMAC&     vit = ref_cast(Champ_Face_CoviMAC,ch_vitesse.valeur());
  const Zone_CoviMAC&          zone = ref_cast(Zone_CoviMAC, vit.zone_dis_base());
  int N = vit.valeurs().line_size();

  ch.typer("Champ_Fonc_P0_CoviMAC_rot");
  Champ_Fonc_P0_CoviMAC_rot&  ch_rot_u = ref_cast(Champ_Fonc_P0_CoviMAC_rot,ch.valeur());

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

  ch_rot_u.fixer_nb_valeurs_nodales(zone.nb_elem_tot());
  ch_rot_u.fixer_unite("s-1");
  ch_rot_u.changer_temps(-1); // so it is calculated at time 0
}

void CoviMAC_discretisation::h_conv(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_temperature, Champ_Fonc& ch, Motcle& nom, int temp_ref) const
{
	  Cerr << "CoviMAC_discretisation::h_conv isn't coded" << finl; // voir fonction similaire dans VDF_discretisation pour le coder
	  abort();
}
void CoviMAC_discretisation::modifier_champ_tabule(const Zone_dis_base& Zone_CoviMAC,Champ_Fonc_Tabule& lambda_tab, const VECT(REF(Champ_base))&  champs_param) const
{
  Champ_Fonc& lambda_tab_dis = lambda_tab.le_champ_tabule_discretise();
  lambda_tab_dis.typer("Champ_Fonc_Tabule_P0_CoviMAC");
  Champ_Fonc_Tabule_P0_CoviMAC& ch_tab_lambda_dis =
    ref_cast(Champ_Fonc_Tabule_P0_CoviMAC,lambda_tab_dis.valeur());
  //ch_tab_lambda_dis.nommer(nom_champ);
  ch_tab_lambda_dis.associer_zone_dis_base(Zone_CoviMAC);
  ch_tab_lambda_dis.associer_param(champs_param, lambda_tab.table());
  ch_tab_lambda_dis.fixer_nb_comp(lambda_tab.nb_comp());
  ch_tab_lambda_dis.fixer_nb_valeurs_nodales(Zone_CoviMAC.nb_elem());
// ch_tab_lambda_dis.fixer_unite(unite);
  ch_tab_lambda_dis.changer_temps(champs_param[0].valeur().temps());
}

Nom  CoviMAC_discretisation::get_name_of_type_for(const Nom& class_operateur, const Nom& type_operateur,const Equation_base& eqn,  const REF(Champ_base)& champ_sup) const
{
  Nom type;
  if (class_operateur=="Source")
    {
      type=type_operateur;
      Nom champ = (eqn.inconnue()->que_suis_je());
      champ.suffix("Champ");
      type+=champ;
      //type+="_CoviMAC";
      return type;

    }
  else if (class_operateur=="Solveur_Masse")
    {
      Nom type_ch=eqn.inconnue()->que_suis_je();
      if (type_ch.debute_par("Champ_P0"))
        type_ch = "_Elem";

      if (type_ch.debute_par("Champ_Face"))
        type_ch = "_Face";


      type="Masse_CoviMAC";
      type+=type_ch;
    }
  else if (class_operateur=="Operateur_Grad")
    {
      type="Op_Grad_CoviMAC_Face";
    }
  else if (class_operateur=="Operateur_Div")
    {
      type="Op_Div_CoviMAC";
    }

  else if (class_operateur=="Operateur_Diff")
    {
      Nom type_ch=eqn.inconnue()->que_suis_je();
      if (type_ch.debute_par("Champ_P0"))
        type_ch = "_Elem";

      if (type_ch.debute_par("Champ_Face"))
        type_ch = "_Face";

      type="Op_Diff" ;
      if (type_operateur!="" )
        {
          type+="_";
          type+=type_operateur;
        }
      type+="_CoviMAC";
      type += type_ch;
    }
  else if (class_operateur=="Operateur_Conv")
    {
      type="Op_Conv_";
      type+=type_operateur;
      Nom tiret="_";
      type+= tiret;
      type+=que_suis_je();
      Nom type_ch=eqn.inconnue()->que_suis_je();
      if (type_ch.debute_par("Champ_P0"))
        type += "_Elem";
      if (type_ch.debute_par("Champ_Face"))
        type += "_Face";
      type+= "_CoviMAC";
    }
  else if (class_operateur=="Operateur_Evanescence")
    {
      type= Nom("Op_") + class_operateur.getSuffix("Operateur_") + "_" + type_operateur
            + (type_operateur != "" ? "_" : "") + que_suis_je();
      Nom type_ch=eqn.inconnue()->que_suis_je();
      if (type_ch.debute_par("Champ_P0"))
        type += "_Elem";
      if (type_ch.debute_par("Champ_Face"))
        type += "_Face";
    }
  else
    {
      return Discret_Thyd::get_name_of_type_for(class_operateur,type_operateur,eqn);
    }
  return type;
}
void CoviMAC_discretisation::residu( const Zone_dis& z, const Champ_Inc& ch_inco, Champ_Fonc& champ ) const
{

  Nom ch_name(ch_inco.le_nom());
  ch_name += "_residu";
  Cerr << "Discretization of " << ch_name << finl;

  const Zone_CoviMAC& zone = ref_cast( Zone_CoviMAC, z.valeur( ) );

  Motcle loc;
  int nb_comp;
  Nom type_ch = ch_inco.valeur().que_suis_je();
  if (type_ch.debute_par("Champ_Face"))
    {
      loc= "champ_face";
      nb_comp = dimension;
    }
  else
    {
      loc = "champ_elem";
      nb_comp = ch_inco.valeurs().line_size();
    }

  Discretisation_base::discretiser_champ(loc,zone, ch_name ,"units_not_defined",nb_comp,ch_inco.temps(),champ);
  Champ_Fonc_base& ch_fonc = ref_cast(Champ_Fonc_base,champ.valeur());
  DoubleTab& tab=ch_fonc.valeurs();
  tab = -10000.0 ;
  Cerr << "[Information] Discretisation_base::residu : the residue is set to -10000.0 at initial time" <<finl;

}

