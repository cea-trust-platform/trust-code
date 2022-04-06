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
// File:        VEF_discretisation.cpp
// Directory:   $TRUST_ROOT/src/VEF/Zones
// Version:     /main/53
//
//////////////////////////////////////////////////////////////////////////////

#include <VEF_discretisation.h>
#include <Zone_VEF.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Fonc_Tabule_P0_VEF.h>
#include <Rotationnel_Champ_P1NC.h>
#include <Critere_Q_Champ_P1NC.h>
#include <Y_plus_Champ_P1NC.h>
#include <grad_T_Champ_P1NC.h>
#include <grad_U_Champ_P1NC.h>
#include <h_conv_Champ_P1NC.h>
#include <Rotationnel_Champ_Q1NC.h>
#include <Taux_cisaillement_P0_VEF.h>
#include <Correlation_Vec_Sca_VEF.h>
#include <Fluide_Ostwald.h>
#include <Champ_Ostwald_VEF.h>
#include <Navier_Stokes_std.h>
#include <Tri_VEF.h>
#include <Tetra_VEF.h>
#include <Quadri_VEF.h>
#include <Hexa_VEF.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc_Q1NC.h>
#include <Schema_Temps_base.h>


Implemente_instanciable(VEF_discretisation,"VEF",Discret_Thyd);



Entree& VEF_discretisation::readOn(Entree& s)
{
  return s ;
}

Sortie& VEF_discretisation::printOn(Sortie& s) const
{
  return s ;
}

// Description:
// Discretisation d'un champ pour le VEF en fonction d'une directive
// de discretisation. La directive est un Motcle comme "vitesse", "pression",
// "temperature", "champ_elem" (cree un champ de type P0 ...), ...
// Cette methode determine le type du champ a creer en fonction du type d'element
// et de la directive de discretisation. Elle determine ensuite le nombre de ddl
// et fixe l'ensemble des parametres du champ (type, nb_compo, nb_ddl, nb_pas_dt,
// nom(s), unite(s), nature du champ et attribue un temps) et associe la Zone_dis au champ.
// Voir le code pour avoir la correspondance entre les directives et
// le type de champ cree.
void VEF_discretisation::discretiser_champ(
  const Motcle& directive, const Zone_dis_base& z,
  Nature_du_champ nature,
  const Noms& noms, const Noms& unites,
  int nb_comp, int nb_pas_dt, double temps,
  Champ_Inc& champ, const Nom& sous_type) const
{
  const Zone_VEF& zone_vef = ref_cast(Zone_VEF, z);

  Motcles motcles(7);
  motcles[0] = "vitesse";     // Choix standard pour la vitesse
  motcles[1] = "pression";    // Choix standard pour la pression
  motcles[2] = "temperature"; // Choix standard pour la temperature
  motcles[3] = "divergence_vitesse"; // Le type de champ obtenu en calculant div v
  motcles[4] = "gradient_pression";  // Le type de champ obtenu en calculant grad P
  motcles[5] = "champ_elem";    // Creer un champ aux elements (de type P0)
  motcles[6] = "champ_sommets"; // Creer un champ aux sommets (type P1)

  // Le type de champ de vitesse depend du type d'element :
  Nom type_champ_vitesse;
  if (sub_type(Tri_VEF, zone_vef.type_elem().valeur())
      || sub_type(Tetra_VEF,zone_vef.type_elem().valeur()))
    type_champ_vitesse = "Champ_P1NC";
  else if (sub_type(Quadri_VEF,zone_vef.type_elem().valeur())
           || sub_type(Hexa_VEF,zone_vef.type_elem().valeur()))
    type_champ_vitesse = "Champ_Q1NC";
  else
    {
      Cerr << "VEF_discretisation::discretiser_champ :\n L'element geometrique ";
      Cerr << zone_vef.type_elem().valeur().que_suis_je();
      Cerr << " n'est pas supporte." << finl;
      exit();
    }

  Nom type;
  int default_nb_comp = 0; // Valeur par defaut du nombre de composantes
  int rang = motcles.search(directive);
  switch(rang)
    {
    case 0:
      type = type_champ_vitesse;
      default_nb_comp = dimension;
      break;
    case 1:
      type = "Champ_P0_VEF";
      default_nb_comp = 1;
      break;
    case 2:
      type = type_champ_vitesse;
      default_nb_comp = 1;
      break;
    case 3:
      type = "Champ_P0_VEF";
      default_nb_comp = 1;
      break;
    case 4:
      type = type_champ_vitesse;
      default_nb_comp = dimension;
      break;
    case 5:
      type = "Champ_P0_VEF";
      default_nb_comp = 1;
      break;
    case 6:
      type = "Champ_P1_VEF";
      default_nb_comp = 1;
      break;
    default:
      assert(rang < 0);
      break;
    }

  if (directive == demande_description)
    Cerr << "VEF_discretisation : " << motcles;

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
  if (type == "Champ_P0_VEF")
    nb_ddl = z.nb_elem();
  else if (type == type_champ_vitesse)
    nb_ddl = zone_vef.nb_faces();
  else if (type == "Champ_P1_VEF")
    nb_ddl = zone_vef.nb_som();
  else
    assert(0);

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
// Idem que VEF_discretisation::discretiser_champ(... , Champ_Inc)
void VEF_discretisation::discretiser_champ(
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
// Idem que VEF_discretisation::discretiser_champ(... , Champ_Inc)
void VEF_discretisation::discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
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
void VEF_discretisation::discretiser_champ_fonc_don(
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

  const Zone_VEF& zone_vef = ref_cast(Zone_VEF, z);

  Motcles motcles(8);
  motcles[0] = "pression";    // Choix standard pour la pression
  motcles[1] = "temperature"; // Choix standard pour la temperature
  motcles[2] = "divergence_vitesse"; // Le type de champ obtenu en calculant div v
  motcles[3] = "champ_elem";  // Creer un champ aux elements (de type P0)
  motcles[4] = "vitesse";     // Choix standard pour la vitesse
  motcles[5] = "gradient_pression";  // Le type de champ obtenu en calculant grad P
  motcles[6] = "champ_sommets"; // Creer un champ aux sommets
  motcles[7] = "champ_face"; // Creer un champ aux faces


  // Le type de champ de vitesse depend du type d'element :
  Nom type_champ_vitesse, type_champ_sommets;
  Nom type_elem_zone = zone_vef.zone().type_elem().valeur().que_suis_je();
  if (Motcle(type_elem_zone)!="Segment")
    {
      const Elem_VEF_base& elem_vef = zone_vef.type_elem().valeur();
      if (sub_type(Tri_VEF, elem_vef) || sub_type(Tetra_VEF, elem_vef))
        {
          type_champ_vitesse = "Champ_Fonc_P1NC";
          type_champ_sommets = "Champ_Fonc_P1_VEF";
        }
      else if (sub_type(Quadri_VEF, elem_vef) || sub_type(Hexa_VEF, elem_vef))
        {
          type_champ_vitesse = "Champ_Fonc_Q1NC";
          type_champ_sommets = "Champ_Fonc_Q1_VEF";
        }
      else
        {
          Cerr << "VEF_discretisation::discretiser_champ :\n L'element geometrique ";
          Cerr << elem_vef.que_suis_je();
          Cerr << " n'est pas supporte." << finl;
          exit();
        }
    }

  Nom type;
  int default_nb_comp = 0; // Valeur par defaut du nombre de composantes
  int rang = motcles.search(directive);
  switch(rang)
    {
    case 0:
      type = "Champ_Fonc_P0_VEF";
      default_nb_comp = 1;
      break;
    case 1:
      type = type_champ_vitesse;
      default_nb_comp = 1;
      break;
    case 2:
      type = "Champ_Fonc_P0_VEF";
      default_nb_comp = 1;
      break;
    case 3:
      type = "Champ_Fonc_P0_VEF";
      default_nb_comp = 1;
      break;
    case 4:
      type = type_champ_vitesse;
      default_nb_comp = dimension;
      break;
    case 5:
      type = type_champ_vitesse;
      default_nb_comp = dimension;
      break;
    case 6:
      type = type_champ_sommets;
      default_nb_comp = 1;
      break;
    case 7:
      type = type_champ_vitesse;
      default_nb_comp = 1;
      break;

    default:
      assert(rang < 0);
      break;
    }

  if (directive == demande_description)
    Cerr << "VEF_discretisation : " << motcles;

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
  if (type == "Champ_Fonc_P0_VEF")      nb_ddl = z.nb_elem();
  else if (type == type_champ_vitesse)  nb_ddl = zone_vef.nb_faces();
  else if ((type == "Champ_Fonc_P1_VEF") || (type == "Champ_Fonc_Q1_VEF")) nb_ddl = zone_vef.nb_som();
  else assert(0);

  if (nb_comp < 0)
    nb_comp = default_nb_comp;
  assert(nb_comp > 0);
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



void VEF_discretisation::distance_paroi(const Schema_Temps_base& sch,
                                        Zone_dis& z, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de la distance paroi" << finl;
  Zone_VEF& zone_vef=ref_cast(Zone_VEF, z.valeur());
  ch.typer("Champ_Fonc_P0_VEF");
  Champ_Fonc_P0_VEF& ch_dist_paroi=ref_cast(Champ_Fonc_P0_VEF,ch.valeur());
  ch_dist_paroi.associer_zone_dis_base(zone_vef);
  ch_dist_paroi.nommer("distance_paroi");
  ch_dist_paroi.fixer_nb_comp(1);
  ch_dist_paroi.fixer_nb_valeurs_nodales(zone_vef.nb_elem());
  ch_dist_paroi.fixer_unite("m");
  ch_dist_paroi.changer_temps(sch.temps_courant());
}



void VEF_discretisation::vorticite(Zone_dis& z,const Champ_Inc& ch_vitesse,
                                   Champ_Fonc& ch) const
{
  Cerr << "Discretisation de la vorticite " << finl;
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, z.valeur());

  if (sub_type(Tri_VEF,zone_VEF.type_elem().valeur()) || sub_type(Tetra_VEF,zone_VEF.type_elem().valeur()))
    {
      ch.typer("Rotationnel_Champ_P1NC");
      const Champ_P1NC& vit = ref_cast(Champ_P1NC,ch_vitesse.valeur());
      Rotationnel_Champ_P1NC& ch_W=ref_cast(Rotationnel_Champ_P1NC,ch.valeur());
      ch_W.associer_zone_dis_base(zone_VEF);
      ch_W.associer_champ(vit);
      ch_W.nommer("vorticite");
      if (dimension == 2)
        ch_W.fixer_nb_comp(1);
      else
        {
          ch_W.fixer_nb_comp(dimension);
          ch_W.fixer_nom_compo(0,"vorticitex");
          ch_W.fixer_nom_compo(1,"vorticitey");
          ch_W.fixer_nom_compo(2,"vorticitez");
        }
      ch_W.fixer_nb_valeurs_nodales(zone_VEF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(ch_vitesse.temps());
    }
  else if (sub_type(Quadri_VEF,zone_VEF.type_elem().valeur()) || sub_type(Hexa_VEF,zone_VEF.type_elem().valeur()))
    {
      ch.typer("Rotationnel_Champ_Q1NC");
      const Champ_Q1NC& vit = ref_cast(Champ_Q1NC,ch_vitesse.valeur());
      Rotationnel_Champ_Q1NC& ch_W=ref_cast(Rotationnel_Champ_Q1NC,ch.valeur());
      ch_W.associer_zone_dis_base(zone_VEF);
      ch_W.associer_champ(vit);
      ch_W.nommer("vorticite");
      if (dimension == 2)
        ch_W.fixer_nb_comp(1);
      else
        {
          ch_W.fixer_nb_comp(dimension);
          ch_W.fixer_nom_compo(0,"vorticitex");
          ch_W.fixer_nom_compo(1,"vorticitey");
          ch_W.fixer_nom_compo(2,"vorticitez");
        }
      ch_W.fixer_nb_valeurs_nodales(zone_VEF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(ch_vitesse.temps());
    }
  else
    {
      Cerr << "Pb dans le typage des elements dans VEF_discretisation::vorticite" << finl;
      exit();
    }
}

void VEF_discretisation::creer_champ_vorticite(const Schema_Temps_base& sch,
                                               const Champ_Inc& ch_vitesse,
                                               Champ_Fonc& ch) const
{
  if (sub_type(Champ_P1NC,ch_vitesse.valeur()))
    {
      ch.typer("Rotationnel_Champ_P1NC");
      const Champ_P1NC& vit = ref_cast(Champ_P1NC,ch_vitesse.valeur());
      const Zone_VEF& zone_VEF = ref_cast(Zone_VEF,vit.zone_dis_base());
      Rotationnel_Champ_P1NC& ch_W = ref_cast(Rotationnel_Champ_P1NC,ch.valeur());
      ch_W.associer_zone_dis_base(zone_VEF);
      ch_W.associer_champ(vit);
      ch_W.nommer("vorticite");
      if (dimension == 2)
        ch_W.fixer_nb_comp(1);
      else
        {
          ch_W.fixer_nb_comp(dimension);
          ch_W.fixer_nom_compo(0, "vorticitex");
          ch_W.fixer_nom_compo(1, "vorticitey");
          ch_W.fixer_nom_compo(2, "vorticitez");
        }
      ch_W.fixer_nb_valeurs_nodales(zone_VEF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(sch.temps_courant());
    }
  else if (sub_type(Champ_Q1NC,ch_vitesse.valeur()))
    {
      ch.typer("Rotationnel_Champ_Q1NC");
      const Champ_Q1NC& vit = ref_cast(Champ_Q1NC,ch_vitesse.valeur());
      const Zone_VEF& zone_VEF = ref_cast(Zone_VEF,vit.zone_dis_base());
      Rotationnel_Champ_Q1NC& ch_W = ref_cast(Rotationnel_Champ_Q1NC,ch.valeur());
      ch_W.associer_zone_dis_base(zone_VEF);
      ch_W.associer_champ(vit);
      ch_W.nommer("vorticite");
      if (dimension == 2)
        ch_W.fixer_nb_comp(1);
      else
        {
          ch_W.fixer_nb_comp(dimension);
          ch_W.fixer_nom_compo(0, "vorticitex");
          ch_W.fixer_nom_compo(1, "vorticitey");
          ch_W.fixer_nom_compo(2, "vorticitez");
        }
      ch_W.fixer_nb_valeurs_nodales(zone_VEF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(sch.temps_courant());
    }
  else
    {
      Cerr << "Pb dans le typage des elements dans VEF_discretisation::creer_champ_vorticite" << finl;
      exit();
    }
}



// Description:
//    discretise en VEF le fluide incompressible, donc  K e N
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
void VEF_discretisation::proprietes_physiques_fluide_Ostwald(const Zone_dis& z, Fluide_Ostwald& le_fluide,
                                                             const Navier_Stokes_std& eqn_hydr, const Champ_Inc& ch_temper ) const
{
  Cerr << "Discretisation VEF du fluide_Ostwald" << finl;
  const Zone_VEF& zone_vef=ref_cast(Zone_VEF, z.valeur());
  const Champ_Inc& ch_vitesse = eqn_hydr.inconnue();
  const Champ_P1NC& vit = ref_cast(Champ_P1NC,ch_vitesse.valeur());




  Champ_Don& mu = le_fluide.viscosite_dynamique();
  //  mu est toujours un champ_Ostwald_VEF , il faut toujours faire ce qui suit
  mu.typer("Champ_Ostwald_VEF");
  Champ_Ostwald_VEF& ch_mu = ref_cast(Champ_Ostwald_VEF,mu.valeur());
  Cerr<<"associe zonedisbase VEF"<<finl;
  ch_mu.associer_zone_dis_base(zone_vef);
  ch_mu.associer_fluide(le_fluide);
  ch_mu.associer_champ(vit);
  ch_mu.associer_eqn(eqn_hydr);
  Cerr<<"associations finies zone dis base, fluide, champ VEF"<<finl;
  ch_mu.fixer_nb_comp(1);

  Cerr<<"fait fixer_nb_valeurs_nodales"<<finl;
  Cerr<<"nb_valeurs_nodales VEF = "<<zone_vef.nb_elem()<<finl;
  ch_mu.fixer_nb_valeurs_nodales(zone_vef.nb_elem());

  Cerr<<"fait changer_temps"<<finl;
  ch_mu.changer_temps(vit.temps());
  Cerr<<"mu VEF est discretise "<<finl;
}


void VEF_discretisation::zone_Cl_dis(Zone_dis& z,
                                     Zone_Cl_dis& zcl) const
{
  Cerr << "discretisation des conditions limites" << finl;
  assert(z.non_nul());
  Zone_VEF& zone_VEF=ref_cast(Zone_VEF, z.valeur());
  zcl.typer("Zone_Cl_VEF");
  assert(zcl.non_nul());
  Zone_Cl_VEF& zone_cl_VEF=ref_cast(Zone_Cl_VEF, zcl.valeur());
  zone_cl_VEF.associer(zone_VEF);
  Cerr << "discretisation des conditions limites OK" << finl;
}


void VEF_discretisation::critere_Q(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  // On passe la zcl, pour qu'il n y ait qu une methode qqsoit la dsicretisation
  // mais on ne s'en sert pas!!!
  Cerr << "Discretisation du critere Q " << finl;
  const Champ_P1NC& vit = ref_cast(Champ_P1NC,ch_vitesse.valeur());
  const Zone_VEF& zone_vef=ref_cast(Zone_VEF, z.valeur());
  ch.typer("Critere_Q_Champ_P1NC");
  Critere_Q_Champ_P1NC& ch_cQ=ref_cast(Critere_Q_Champ_P1NC,ch.valeur());
  ch_cQ.associer_zone_dis_base(zone_vef);
  ch_cQ.associer_champ(vit);
  ch_cQ.nommer("Critere_Q");
  ch_cQ.fixer_nb_comp(1);
  ch_cQ.fixer_nb_valeurs_nodales(zone_vef.nb_elem());
  ch_cQ.fixer_unite("s-2");
  ch_cQ.changer_temps(ch_vitesse.temps());
}


void VEF_discretisation::y_plus(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de y_plus" << finl;
  const Champ_P1NC& vit = ref_cast(Champ_P1NC,ch_vitesse.valeur());
  const Zone_VEF& zone_vef=ref_cast(Zone_VEF, z.valeur());
  const Zone_Cl_VEF& zone_cl_vef=ref_cast(Zone_Cl_VEF, zcl.valeur());
  ch.typer("Y_plus_Champ_P1NC");
  Y_plus_Champ_P1NC& ch_yp=ref_cast(Y_plus_Champ_P1NC,ch.valeur());
  ch_yp.associer_zone_dis_base(zone_vef);
  ch_yp.associer_zone_Cl_dis_base(zone_cl_vef);
  ch_yp.associer_champ(vit);
  ch_yp.nommer("Y_plus");
  ch_yp.fixer_nb_comp(1);
  ch_yp.fixer_nb_valeurs_nodales(zone_vef.nb_elem());
  ch_yp.fixer_unite("adimensionnel");
  ch_yp.changer_temps(ch_vitesse.temps());
}

void VEF_discretisation::grad_u(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{

  const Champ_P1NC& vit = ref_cast( Champ_P1NC, ch_vitesse.valeur( ) );
  const Zone_VEF& zone_vef=ref_cast( Zone_VEF, z.valeur( ) );
  const int nb_comp = dimension*dimension;
  const Zone_Cl_VEF& zone_cl_vef=ref_cast( Zone_Cl_VEF, zcl.valeur( ) );
  ch.typer("grad_u_Champ_P1NC");
  grad_U_Champ_P1NC& ch_grad_u=ref_cast( grad_U_Champ_P1NC, ch.valeur( ) );
  ch_grad_u.fixer_nature_du_champ( vectoriel );
  ch_grad_u.associer_zone_dis_base( zone_vef );
  ch_grad_u.associer_zone_Cl_dis_base( zone_cl_vef );
  ch_grad_u.associer_champ( vit );
  ch_grad_u.nommer("gradient_vitesse");
  ch_grad_u.fixer_nb_comp( nb_comp );
  ch_grad_u.fixer_nb_valeurs_nodales( zone_vef.nb_elem( ) );
  ch_grad_u.fixer_unite("s-1");
  ch_grad_u.changer_temps( ch_vitesse.temps( ) );


  if (dimension == 2)
    {
      ch_grad_u.fixer_nom_compo(0,"dUdX"); // du/dx
      ch_grad_u.fixer_nom_compo(1,"dUdY"); // du/dy
      ch_grad_u.fixer_nom_compo(2,"dVdX"); // dv/dx
      ch_grad_u.fixer_nom_compo(3,"dVdY"); // dv/dy
    }
  else
    {
      ch_grad_u.fixer_nom_compo(0,"dUdX"); // du/dx
      ch_grad_u.fixer_nom_compo(1,"dUdY"); // du/dy
      ch_grad_u.fixer_nom_compo(2,"dUdZ"); // du/dz
      ch_grad_u.fixer_nom_compo(3,"dVdX"); // dv/dx
      ch_grad_u.fixer_nom_compo(4,"dVdY"); // dv/dy
      ch_grad_u.fixer_nom_compo(5,"dVdZ"); // dv/dz
      ch_grad_u.fixer_nom_compo(6,"dWdX"); // dw/dx
      ch_grad_u.fixer_nom_compo(7,"dWdY"); // dw/dy
      ch_grad_u.fixer_nom_compo(8,"dWdZ"); // dw/dz
    }

}


void VEF_discretisation::grad_T(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_temperature, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de gradient_temperature" << finl;

  const Zone_VEF& zone_vef=ref_cast(Zone_VEF, z.valeur());
  creer_champ(ch, zone_vef, "gradient_temperature_Champ_P1NC",
              "gradient_temperature", "K/m", dimension,
              zone_vef.nb_elem(),
              ch_temperature.temps()
             );

  grad_T_Champ_P1NC& ch_gt=ref_cast(grad_T_Champ_P1NC,ch.valeur());
  const Champ_P1NC& temp = ref_cast(Champ_P1NC,ch_temperature.valeur());
  const Zone_Cl_VEF& zone_cl_vef=ref_cast(Zone_Cl_VEF, zcl.valeur());
  ch_gt.associer_zone_Cl_dis_base(zone_cl_vef);
  ch_gt.associer_champ(temp);
}

void VEF_discretisation::h_conv(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_temperature, Champ_Fonc& ch, Motcle& nom, int temp_ref) const
{
  Cerr << "Discretisation de h_conv" << finl;
  const Champ_P1NC& temp = ref_cast(Champ_P1NC,ch_temperature.valeur());
  const Zone_VEF& zone_vef=ref_cast(Zone_VEF, z.valeur());
  const Zone_Cl_VEF& zone_cl_vef=ref_cast(Zone_Cl_VEF, zcl.valeur());
  ch.typer("h_conv_Champ_P1NC");
  h_conv_Champ_P1NC& ch_gt=ref_cast(h_conv_Champ_P1NC,ch.valeur());
  ch_gt.associer_zone_dis_base(zone_vef);
  ch_gt.associer_zone_Cl_dis_base(zone_cl_vef);
  ch_gt.associer_champ(temp);
  ch_gt.temp_ref()=temp_ref;
  ch_gt.nommer(nom);
  ch_gt.fixer_nb_comp(1);
  ch_gt.fixer_nb_valeurs_nodales(zone_vef.nb_elem());
  ch_gt.fixer_unite("W/m2.K");
  ch_gt.changer_temps(ch_temperature.temps());
}

void VEF_discretisation::taux_cisaillement(const Zone_dis& z, const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& champ) const
{
  const Zone_VEF& zone_vef=ref_cast(Zone_VEF, z.valeur());
  const Zone_Cl_VEF& zone_cl_vef=ref_cast(Zone_Cl_VEF, zcl.valeur());
  champ.typer("Taux_cisaillement_P0_VEF");
  Taux_cisaillement_P0_VEF& ch=ref_cast(Taux_cisaillement_P0_VEF,champ.valeur());
  ch.associer_zone_dis_base(zone_vef);
  const Champ_P1NC& vit = ref_cast(Champ_P1NC, ch_vitesse.valeur());
  ch.associer_champ(vit, zone_cl_vef);
  ch.nommer("Taux_cisaillement");
  ch.fixer_nb_comp(1);
  ch.fixer_nb_valeurs_nodales(zone_vef.nb_elem());
  ch.fixer_unite("s-1");
  ch.changer_temps(ch_vitesse.temps());
}

void VEF_discretisation::modifier_champ_tabule(const Zone_dis_base& zone_dis, Champ_Fonc_Tabule& le_champ_tabule, const VECT(REF(Champ_base))& ch_inc) const
{
  le_champ_tabule.le_champ_tabule_discretise().typer("Champ_Fonc_Tabule_P0_VEF");
  Champ_Fonc_Tabule_P0_VEF& le_champ_tabule_dis = ref_cast(Champ_Fonc_Tabule_P0_VEF,le_champ_tabule.le_champ_tabule_discretise().valeur());
  le_champ_tabule_dis.associer_zone_dis_base(zone_dis);
  le_champ_tabule_dis.associer_param(ch_inc,le_champ_tabule.table());
  le_champ_tabule_dis.nommer(le_champ_tabule.le_nom()); // We give a name to this field, help for debug
  le_champ_tabule_dis.fixer_nb_comp(le_champ_tabule.nb_comp());
  le_champ_tabule_dis.fixer_nb_valeurs_nodales(zone_dis.nb_elem());
  le_champ_tabule_dis.changer_temps(ch_inc[0].valeur().temps());
}

void VEF_discretisation::residu( const Zone_dis& z, const Champ_Inc& ch_inco, Champ_Fonc& champ ) const
{
  Nom ch_name(ch_inco.le_nom());
  ch_name += "_residu";
  Cerr << "Discretization of " << ch_name << finl;

  const Zone_VEF& zone_vef = ref_cast( Zone_VEF, z.valeur( ) );
  int nb_comp =ch_inco.valeurs().line_size();
  Discretisation_base::discretiser_champ("champ_face",zone_vef, ch_name,"units_not_defined", nb_comp, ch_inco.temps(), champ);
  Champ_Fonc_base& ch_fonc = ref_cast(Champ_Fonc_base,champ.valeur());
  DoubleTab& tab=ch_fonc.valeurs();
  tab = -10000.0 ;
  Cerr << "[Information] Discretisation_base::residu : the residue is set to -10000.0 at initial time" <<finl;
}

