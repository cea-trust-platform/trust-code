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
// File:        VDF_discretisation.cpp
// Directory:   $TRUST_ROOT/src/VDF/Zones
// Version:     /main/61
//
//////////////////////////////////////////////////////////////////////////////

#include <VDF_discretisation.h>
#include <Rotationnel_Champ_Face.h>
#include <Correlation_Vec_Sca_VDF.h>
#include <Champ_P0_VDF.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Fonc_Tabule_P0_VDF.h>
#include <Critere_Q_Champ_Face.h>
#include <Fluide_Ostwald.h>
#include <Champ_Ostwald_VDF.h>
#include <Champ_Uniforme.h>
#include <Y_plus_Champ_Face.h>
#include <Reynolds_maille_Champ_Face.h>
#include <Courant_maille_Champ_Face.h>
#include <Taux_cisaillement_P0_VDF.h>
#include <Postraitement_base.h>
#include <Schema_Temps_base.h>
// #include <Champ_T_Paroi_Face.h>
#include <Zone_Cl_VDF.h>
#include <Navier_Stokes_std.h>

Implemente_instanciable(VDF_discretisation,"VDF",Discret_Thyd);

Entree& VDF_discretisation::readOn(Entree& s)
{
  return s ;
}

Sortie& VDF_discretisation::printOn(Sortie& s) const
{
  return s ;
}

// Description:
// Discretisation d'un Champ_Inc pour le VDF en fonction d'une directive
// de discretisation. La directive est un Motcle comme "vitesse", "pression",
// "temperature", "champ_elem" (cree un champ de type P0 ...), ...
// Cette methode determine le type du champ a creer en fonction du type d'element
// et de la directive de discretisation. Elle determine ensuite le nombre de ddl
// et fixe l'ensemble des parametres du champ (type, nb_compo, nb_ddl, nb_pas_dt,
// nom(s), unite(s), nature du champ et attribue un temps) et associe la Zone_dis au champ.
// Voir le code pour avoir la correspondance entre les directives et
// le type de champ cree.
void VDF_discretisation::discretiser_champ(
  const Motcle& directive, const Zone_dis_base& z,
  Nature_du_champ nature,
  const Noms& noms, const Noms& unites,
  int nb_comp, int nb_pas_dt, double temps,
  Champ_Inc& champ) const
{
  const Zone_VDF& zone_vdf = ref_cast(Zone_VDF, z);

  Motcles motcles(7);
  motcles[0] = "vitesse";     // Choix standard pour la vitesse
  motcles[1] = "pression";    // Choix standard pour la pression
  motcles[2] = "temperature"; // Choix standard pour la temperature
  motcles[3] = "divergence_vitesse"; // Le type de champ obtenu en calculant div v
  motcles[4] = "gradient_pression";  // Le type de champ obtenu en calculant grad P
  motcles[5] = "champ_face";   // Creer un champ aux faces
  motcles[6] = "champ_elem";   // Creer un champ aux elements (de type P0)

  Nom type;
  int default_nb_comp = -1;
  int rang = motcles.search(directive);
  switch(rang)
    {
    case 0:
      type = "Champ_Face";
      default_nb_comp = dimension;
      break;
    case 1:
      type = "Champ_P0_VDF";
      default_nb_comp = 1;
      break;
    case 2:
      type = "Champ_P0_VDF";
      default_nb_comp = 1;
      break;
    case 3:
      type = "Champ_P0_VDF";
      default_nb_comp = 1;
      break;
    case 4:
      type = "Champ_Face";
      default_nb_comp = dimension;
      break;
    case 5:
      type = "Champ_Face";
      default_nb_comp = 1;
      break;
    case 6:
      type = "Champ_P0_VDF";
      default_nb_comp = 1;
      break;
    default:
      assert(rang < 0);
      break;
    }

  if (directive == demande_description)
    Cerr << "VDF_discretisation : " << motcles;

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
  if (type == "Champ_P0_VDF")    nb_ddl = z.nb_elem();
  else if (type == "Champ_Face") nb_ddl = zone_vdf.nb_faces();
  else assert(0);

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
// Idem que VDF_discretisation::discretiser_champ(... , Champ_Inc)
// pour un Champ_Fonc.
void VDF_discretisation::discretiser_champ(
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
// Idem que VDF_discretisation::discretiser_champ(... , Champ_Inc)
// pour un Champ_Don.
void VDF_discretisation::discretiser_champ(
  const Motcle& directive, const Zone_dis_base& z,
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
// Idem que VEF_discretisation::discretiser_champ(... , Champ_Inc)
// Traitement commun aux champ_fonc et champ_don.
// Cette methode est privee (passage d'un Objet_U pas propre vu
// de l'exterieur ...)
void VDF_discretisation::discretiser_champ_fonc_don(
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

  Motcles motcles(8);
  motcles[0] = "pression";
  motcles[1] = "temperature";
  motcles[2] = "divergence_vitesse"; // Le type de champ obtenu en calculant div v
  motcles[3] = "champ_elem";  // Creer un champ aux elements (de type P0)
  motcles[4] = "vitesse";     // Creer un champ comme la vitesse
  motcles[5] = "gradient_pression";  // Le type de champ obtenu en calculant grad P
  motcles[6] = "champ_face";   // Creer un champ aux faces
  motcles[7] = "champ_sommets";   // Creer un champ aux sommets

  Nom type;
  int rang = motcles.search(directive);
  int default_nb_comp = 1;
  switch(rang)
    {
    case 0:
      type = "Champ_Fonc_P0_VDF";
      break;
    case 1:
      type = "Champ_Fonc_P0_VDF";
      break;
    case 2:
      type = "Champ_Fonc_P0_VDF";
      break;
    case 3:
      type = "Champ_Fonc_P0_VDF";
      break;
    case 4:
      type = "Champ_Fonc_Face";
      default_nb_comp = dimension;
      break;
    case 5:
      type = "Champ_Fonc_Face";
      default_nb_comp = dimension;
      break;
    case 6:
      type = "Champ_Fonc_Face";
      default_nb_comp = 1;
      break;
    case 7:
      type = "Champ_Fonc_Q1_VDF";
      default_nb_comp = 1;
      break;

    default:
      assert(rang < 0);
      break;
    }

  if (directive == demande_description)
    Cerr << "VDF_discretisation : " << motcles;

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
  const Zone_VDF& zone_vdf = ref_cast(Zone_VDF, z);
  if (type == "Champ_Fonc_P0_VDF")      nb_ddl = z.nb_elem();
  else if (type == "Champ_Fonc_Face")   nb_ddl = zone_vdf.nb_faces();
  else if (type == "Champ_Fonc_Q1_VDF") nb_ddl = zone_vdf.nb_som();
  else assert(0);


  if (nb_comp < 0)
    nb_comp = default_nb_comp; // Nombre de composantes par defaut
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



void VDF_discretisation::vorticite(Zone_dis& z,
                                   const Champ_Inc& ch_vitesse,
                                   Champ_Fonc& ch) const
{
  const Champ_Face& vit = ref_cast(Champ_Face,ch_vitesse.valeur());
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, z.valeur());
  ch.typer("Rotationnel_Champ_Face");
  Rotationnel_Champ_Face& ch_W=ref_cast(Rotationnel_Champ_Face,ch.valeur());
  ch_W.associer_zone_dis_base(zone_vdf);
  ch_W.associer_champ(vit);
  ch_W.nommer("vorticite");
  if (dimension == 2)
    ch_W.fixer_nb_comp(1);
  else
    {
      ch_W.fixer_nb_comp(dimension);
      ch_W.fixer_nom_compo(0,"vorticiteX");
      ch_W.fixer_nom_compo(1,"vorticiteY");
      ch_W.fixer_nom_compo(2,"vorticiteZ");
    }
  ch_W.fixer_nb_valeurs_nodales(zone_vdf.nb_elem());
  ch_W.fixer_unite("s-1");
  ch_W.changer_temps(ch_vitesse.temps());
}

void VDF_discretisation::critere_Q(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse,Champ_Fonc& ch) const
{
  const Champ_Face& vit = ref_cast(Champ_Face,ch_vitesse.valeur());
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, z.valeur());
  const Zone_Cl_VDF& zone_cl_vdf=ref_cast(Zone_Cl_VDF, zcl.valeur());
  ch.typer("Critere_Q_Champ_Face");
  Critere_Q_Champ_Face& ch_Criter_Q=ref_cast(Critere_Q_Champ_Face,ch.valeur());
  ch_Criter_Q.associer_zone_dis_base(zone_vdf);
  ch_Criter_Q.associer_zone_Cl_dis_base(zone_cl_vdf);
  ch_Criter_Q.associer_champ(vit);
  ch_Criter_Q.nommer("Critere_Q");
  ch_Criter_Q.fixer_nb_comp(1);
  ch_Criter_Q.fixer_nb_valeurs_nodales(zone_vdf.nb_elem());
  ch_Criter_Q.fixer_unite("s-2");
  ch_Criter_Q.changer_temps(ch_vitesse.temps());
}

void VDF_discretisation::reynolds_maille(const Zone_dis& z, const Fluide_Incompressible& le_fluide, const Champ_Inc& ch_vitesse, Champ_Fonc& champ) const
{
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, z.valeur());
  champ.typer("Reynolds_maille_Champ_Face");
  Reynolds_maille_Champ_Face& ch=ref_cast(Reynolds_maille_Champ_Face,champ.valeur());
  ch.associer_zone_dis_base(zone_vdf);
  const Champ_Face& vit = ref_cast(Champ_Face, ch_vitesse.valeur());
  const Champ_Don& nu = le_fluide.viscosite_cinematique();
  ch.associer_champ(vit,nu);
  ch.nommer("Reynolds_maille");
  ch.fixer_nb_comp(dimension);
  ch.fixer_nom_compo(0, "Reynolds_maille_X");
  ch.fixer_nom_compo(1, "Reynolds_maille_Y");
  if (dimension==3) ch.fixer_nom_compo(2, "Reynolds_maille_Z");
  ch.fixer_nb_valeurs_nodales(zone_vdf.nb_faces());
  ch.fixer_unite("adimensionnel");
  ch.changer_temps(ch_vitesse.temps());
}

void VDF_discretisation::courant_maille(const Zone_dis& z, const Schema_Temps_base& sch, const Champ_Inc& ch_vitesse, Champ_Fonc& champ) const
{
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, z.valeur());
  champ.typer("Courant_maille_Champ_Face");
  Courant_maille_Champ_Face& ch=ref_cast(Courant_maille_Champ_Face,champ.valeur());
  ch.associer_zone_dis_base(zone_vdf);
  const Champ_Face& vit = ref_cast(Champ_Face, ch_vitesse.valeur());
  ch.associer_champ(vit,sch);
  ch.nommer("Courant_maille");
  ch.fixer_nb_comp(dimension);
  ch.fixer_nom_compo(0, "Courant_maille_X");
  ch.fixer_nom_compo(1, "Courant_maille_Y");
  if (dimension==3) ch.fixer_nom_compo(2, "Courant_maille_Z");
  ch.fixer_nb_valeurs_nodales(zone_vdf.nb_faces());
  ch.fixer_unite("adimensionnel");
  ch.changer_temps(ch_vitesse.temps());
}

void VDF_discretisation::taux_cisaillement(const Zone_dis& z, const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& champ) const
{
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, z.valeur());
  const Zone_Cl_VDF& zone_cl_vdf=ref_cast(Zone_Cl_VDF, zcl.valeur());
  champ.typer("Taux_cisaillement_P0_VDF");
  Taux_cisaillement_P0_VDF& ch=ref_cast(Taux_cisaillement_P0_VDF,champ.valeur());
  ch.associer_zone_dis_base(zone_vdf);
  const Champ_Face& vit = ref_cast(Champ_Face, ch_vitesse.valeur());
  ch.associer_champ(vit, zone_cl_vdf);
  ch.nommer("Taux_cisaillement");
  ch.fixer_nb_comp(1);
  ch.fixer_nb_valeurs_nodales(zone_vdf.nb_elem());
  ch.fixer_unite("s-1");
  ch.changer_temps(ch_vitesse.temps());
}

void VDF_discretisation::y_plus(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse,Champ_Fonc& ch) const
{
  const Champ_Face& vit = ref_cast(Champ_Face,ch_vitesse.valeur());
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, z.valeur());
  const Zone_Cl_VDF& zone_cl_vdf=ref_cast(Zone_Cl_VDF, zcl.valeur());
  ch.typer("Y_plus_Champ_Face");
  Y_plus_Champ_Face& ch_y_plus=ref_cast(Y_plus_Champ_Face,ch.valeur());
  ch_y_plus.associer_zone_dis_base(zone_vdf);
  ch_y_plus.associer_zone_Cl_dis_base(zone_cl_vdf);
  ch_y_plus.associer_champ(vit);
  ch_y_plus.nommer("Y_plus");
  ch_y_plus.fixer_nb_comp(1);
  ch_y_plus.fixer_nb_valeurs_nodales(zone_vdf.nb_elem());
  ch_y_plus.fixer_unite("adimensionnel");
  ch_y_plus.changer_temps(ch_vitesse.temps());
}

/* void VDF_discretisation::t_paroi(const Zone_dis& z,const Zone_Cl_dis& zcl, const Equation_base& eqn,Champ_Fonc& ch) const
{
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, z.valeur());
  const Zone_Cl_VDF& zone_cl_vdf=ref_cast(Zone_Cl_VDF, zcl.valeur());
  ch.typer("Champ_Temperature_Paroi_Face");
  Champ_T_Paroi_Face& ch_tp=ref_cast(Champ_T_Paroi_Face,ch.valeur());
  ch_tp.associer_eqn(eqn);
  ch_tp.associer_zone_dis_base(zone_vdf);
  ch_tp.associer_zone_Cl_dis_base(zone_cl_vdf);
  ch_tp.nommer("temperature_paroi");
  ch_tp.fixer_nb_comp(1);
  ch_tp.fixer_nb_valeurs_nodales(zone_vdf.nb_faces());
  ch_tp.fixer_unite("K");
  ch_tp.changer_temps(eqn.inconnue().temps());
} */

void VDF_discretisation::modifier_champ_tabule(const Zone_dis_base& zone_dis, Champ_Fonc_Tabule& le_champ_tabule,const Champ_base& ch_inc) const
{
  le_champ_tabule.le_champ_tabule_discretise().typer("Champ_Fonc_Tabule_P0_VDF");
  Champ_Fonc_Tabule_P0_VDF& le_champ_tabule_dis = ref_cast(Champ_Fonc_Tabule_P0_VDF,le_champ_tabule.le_champ_tabule_discretise().valeur());
  le_champ_tabule_dis.associer_zone_dis_base(zone_dis);
  le_champ_tabule_dis.associer_param(ch_inc,le_champ_tabule.table());
  le_champ_tabule_dis.nommer(le_champ_tabule.le_nom()); // We give a name to this field, help for debug
  le_champ_tabule_dis.fixer_nb_comp(le_champ_tabule.nb_comp());
  le_champ_tabule_dis.fixer_nb_valeurs_nodales(zone_dis.nb_elem());
  le_champ_tabule_dis.changer_temps(ch_inc.temps());
}


void VDF_discretisation::zone_Cl_dis(Zone_dis& z,
                                     Zone_Cl_dis& zcl) const
{
  Cerr << "Discretisation des conditions limites" << finl;
  Zone_VDF& zone_vdf=ref_cast(Zone_VDF, z.valeur());
  zcl.typer("Zone_Cl_VDF");
  Zone_Cl_VDF& zone_cl_vdf=ref_cast(Zone_Cl_VDF, zcl.valeur());
  zone_cl_vdf.associer(zone_vdf);
}

// Description:
//    discretise en VDF le fluide incompressible, donc  K e N
// Precondition:
// Parametre: Zone_dis&
//    Signification: zone a discretiser
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Parametre: Fluide_Ostwald&
//    Signification: fluide a discretiser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: Champ_Inc&
//    Signification: vitesse
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Parametre: Champ_Inc&
//    Signification: temperature
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void VDF_discretisation::proprietes_physiques_fluide_Ostwald
(Zone_dis& z, Fluide_Ostwald& le_fluide, const Navier_Stokes_std& eqn_hydr,
 const Champ_Inc&  ) const
{
  Cerr << "Discretisation du fluide_Ostwald" << finl;
  Zone_VDF& zone_vdf=ref_cast(Zone_VDF, z.valeur());
  const Champ_Inc& ch_vitesse = eqn_hydr.inconnue();
  const Champ_Face& vit = ref_cast(Champ_Face,ch_vitesse.valeur());


  Champ_Don& mu = le_fluide.viscosite_dynamique();
  //  mu est toujours un champ_Ostwald_VDF , il faut toujours faire ce qui suit
  mu.typer("Champ_Ostwald_VDF");
  Champ_Ostwald_VDF& ch_mu = ref_cast(Champ_Ostwald_VDF,mu.valeur());
  Cerr<<"associe zonedisbase"<<finl;
  ch_mu.associer_zone_dis_base(zone_vdf);
  ch_mu.associer_fluide(le_fluide);
  ch_mu.associer_champ(vit);
  Cerr<<"associations finies"<<finl;
  ch_mu.fixer_nb_comp(1);

  Cerr<<"fait fixer_nb_valeurs_nodales"<<finl;
  Cerr<<"nb_valeurs_nodales = "<<zone_vdf.nb_elem()<<finl;
  ch_mu.fixer_nb_valeurs_nodales(zone_vdf.nb_elem());

  Cerr<<"fait changer_temps"<<finl;
  ch_mu.changer_temps(vit.temps());

  Cerr<<"mu est discretise "<<finl;
}


void VDF_discretisation::creer_champ_vorticite(const Schema_Temps_base& sch,
                                               const Champ_Inc& ch_vitesse,
                                               Champ_Fonc& ch) const
{
  if (sub_type(Champ_Face,ch_vitesse.valeur()))
    {
      const Champ_Face& vit = ref_cast(Champ_Face,ch_vitesse.valeur());
      const Zone_VDF& zone_VDF = ref_cast(Zone_VDF,vit.zone_dis_base());
      ch.typer("Rotationnel_Champ_Face");
      Rotationnel_Champ_Face& ch_W=ref_cast(Rotationnel_Champ_Face,ch.valeur());
      ch_W.associer_zone_dis_base(zone_VDF);
      ch_W.associer_champ(vit);
      ch_W.nommer("vorticite");
      if (dimension == 2)
        ch_W.fixer_nb_comp(1);
      else
        {
          ch_W.fixer_nb_comp(dimension);
          ch_W.fixer_nom_compo(0, "vorticiteX");
          ch_W.fixer_nom_compo(1, "vorticiteY");
          ch_W.fixer_nom_compo(2, "vorticiteZ");
        }
      ch_W.fixer_nb_valeurs_nodales(zone_VDF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(sch.temps_courant());
    }
}

