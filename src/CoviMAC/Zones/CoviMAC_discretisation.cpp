/****************************************************************************
* Copyright (c) 2019, CEA
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

#include <Champ_Fonc_Tabule_P0_CoviMAC.h>
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
#include <grad_U_Champ_Face_CoviMAC.h>

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
  if (type.debute_par(type_elem)) nb_ddl = z.nb_elem() + zone_CoviMAC.premiere_face_int();
  else if (type.debute_par(type_champ_vitesse)) nb_ddl = zone_CoviMAC.nb_faces() + dimension * zone_CoviMAC.nb_elem();
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



void CoviMAC_discretisation::vorticite(Zone_dis& z,const Champ_Inc& ch_vitesse,
                                       Champ_Fonc& ch) const
{
#ifdef dependance
  Cerr << "Discretisation de la vorticite " << finl;
  const Zone_CoviMAC& zone_CoviMAC=ref_cast(Zone_CoviMAC, z.valeur());

  if (sub_type(Tri_CoviMAC,zone_CoviMAC.type_elem().valeur()) || sub_type(Segment_CoviMAC,zone_CoviMAC.type_elem().valeur()) || sub_type(Tetra_CoviMAC,zone_CoviMAC.type_elem().valeur()))
    {
      ch.typer("Rotationnel_Champ_P1_CoviMAC");
      const Champ_P1_CoviMAC& vit = ref_cast(Champ_P1_CoviMAC,ch_vitesse.valeur());
      Rotationnel_Champ_P1_CoviMAC& ch_W=ref_cast(Rotationnel_Champ_P1_CoviMAC,ch.valeur());
      ch_W.associer_zone_dis_base(zone_CoviMAC);
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
      ch_W.fixer_nb_valeurs_nodales(zone_CoviMAC.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(ch_vitesse.temps());
    }
  else if (sub_type(Quadri_CoviMAC,zone_CoviMAC.type_elem().valeur()) || sub_type(Hexa_CoviMAC,zone_CoviMAC.type_elem().valeur()))
    {
      ch.typer("Rotationnel_Champ_Q1_CoviMAC");
      const Champ_Q1_CoviMAC& vit = ref_cast(Champ_Q1_CoviMAC,ch_vitesse.valeur());
      Rotationnel_Champ_Q1_CoviMAC& ch_W=ref_cast(Rotationnel_Champ_Q1_CoviMAC,ch.valeur());
      ch_W.associer_zone_dis_base(zone_CoviMAC);
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
      ch_W.fixer_nb_valeurs_nodales(zone_CoviMAC.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(ch_vitesse.temps());
    }
  else
    {
      Cerr << "Pb dans le typage des elements dans CoviMAC_discretisation::vorticite" << finl;
      exit();
    }
#endif
}

void CoviMAC_discretisation::creer_champ_vorticite(const Schema_Temps_base& sch,
                                                   const Champ_Inc& ch_vitesse,
                                                   Champ_Fonc& ch) const
{

#ifdef dependance
  if (sub_type(Champ_P1_CoviMAC,ch_vitesse.valeur()))
    {
      ch.typer("Rotationnel_Champ_P1_CoviMAC");
      const Champ_P1_CoviMAC& vit = ref_cast(Champ_P1_CoviMAC,ch_vitesse.valeur());
      const Zone_CoviMAC& zone_CoviMAC = ref_cast(Zone_CoviMAC,vit.zone_dis_base());
      Rotationnel_Champ_P1_CoviMAC& ch_W = ref_cast(Rotationnel_Champ_P1_CoviMAC,ch.valeur());
      ch_W.associer_zone_dis_base(zone_CoviMAC);
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
      ch_W.fixer_nb_valeurs_nodales(zone_CoviMAC.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(sch.temps_courant());
    }
  else if (sub_type(Champ_Q1_CoviMAC,ch_vitesse.valeur()))
    {
      ch.typer("Rotationnel_Champ_Q1_CoviMAC");
      const Champ_Q1_CoviMAC& vit = ref_cast(Champ_Q1_CoviMAC,ch_vitesse.valeur());
      const Zone_CoviMAC& zone_CoviMAC = ref_cast(Zone_CoviMAC,vit.zone_dis_base());
      Rotationnel_Champ_Q1_CoviMAC& ch_W = ref_cast(Rotationnel_Champ_Q1_CoviMAC,ch.valeur());
      ch_W.associer_zone_dis_base(zone_CoviMAC);
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
      ch_W.fixer_nb_valeurs_nodales(zone_CoviMAC.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(sch.temps_courant());
    }
  else
    {
      Cerr << "Pb dans le typage des elements dans CoviMAC_discretisation::creer_champ_vorticite" << finl;
      exit();
    }
#endif
}



// Description:
//    discretise en CoviMAC le fluide incompressible, donc  K e N
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
//    Signification: ch_vitesse
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
void CoviMAC_discretisation::proprietes_physiques_fluide_Ostwald(const Zone_dis& z, Fluide_Ostwald& le_fluide,
                                                                 const Navier_Stokes_std& eqn_hydr, const Champ_Inc& ch_temper ) const
{

#ifdef dependance
  Cerr << "Discretisation CoviMAC du fluide_Ostwald" << finl;
  const Zone_CoviMAC& zone_CoviMAC=ref_cast(Zone_CoviMAC, z.valeur());
  const Champ_Inc& ch_vitesse = eqn_hydr.inconnue();
  const Champ_P1_CoviMAC& vit = ref_cast(Champ_P1_CoviMAC,ch_vitesse.valeur());




  Champ_Don& mu = le_fluide.viscosite_dynamique();
  //  mu est toujours un champ_Ostwald_CoviMAC , il faut toujours faire ce qui suit
  mu.typer("Champ_Ostwald_CoviMAC");
  Champ_Ostwald_CoviMAC& ch_mu = ref_cast(Champ_Ostwald_CoviMAC,mu.valeur());
  Cerr<<"associe zonedisbase CoviMAC"<<finl;
  ch_mu.associer_zone_dis_base(zone_CoviMAC);
  ch_mu.associer_fluide(le_fluide);
  ch_mu.associer_champ(vit);
  ch_mu.associer_eqn(eqn_hydr);
  Cerr<<"associations finies zone dis base, fluide, champ CoviMAC"<<finl;
  ch_mu.fixer_nb_comp(1);

  Cerr<<"fait fixer_nb_valeurs_nodales"<<finl;
  Cerr<<"nb_valeurs_nodales CoviMAC = "<<zone_CoviMAC.nb_elem()<<finl;
  ch_mu.fixer_nb_valeurs_nodales(zone_CoviMAC.nb_elem());

  Cerr<<"fait changer_temps"<<finl;
  ch_mu.changer_temps(vit.temps());
  Cerr<<"mu CoviMAC est discretise "<<finl;
#endif
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
#ifdef dependance
  // On passe la zcl, pour qu'il n y ait qu une methode qqsoit la dsicretisation
  // mais on ne s'en sert pas!!!
  Cerr << "Discretisation du critere Q " << finl;
  const Champ_P1_CoviMAC& vit = ref_cast(Champ_P1_CoviMAC,ch_vitesse.valeur());
  const Zone_CoviMAC& zone_CoviMAC=ref_cast(Zone_CoviMAC, z.valeur());
  ch.typer("Critere_Q_Champ_P1_CoviMAC");
  Critere_Q_Champ_P1_CoviMAC& ch_cQ=ref_cast(Critere_Q_Champ_P1_CoviMAC,ch.valeur());
  ch_cQ.associer_zone_dis_base(zone_CoviMAC);
  ch_cQ.associer_champ(vit);
  ch_cQ.nommer("Critere_Q");
  ch_cQ.fixer_nb_comp(1);
  ch_cQ.fixer_nb_valeurs_nodales(zone_CoviMAC.nb_elem());
  ch_cQ.fixer_unite("s-2");
  ch_cQ.changer_temps(ch_vitesse.temps());
#endif
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
#ifdef dependance
  Cerr << "Discretisation de gradient_temperature" << finl;
  const Champ_P1_CoviMAC& temp = ref_cast(Champ_P1_CoviMAC,ch_temperature.valeur());
  const Zone_CoviMAC& zone_CoviMAC=ref_cast(Zone_CoviMAC, z.valeur());
  const Zone_Cl_CoviMAC& zone_cl_CoviMAC=ref_cast(Zone_Cl_CoviMAC, zcl.valeur());
  ch.typer("gradient_temperature_Champ_P1_CoviMAC");
  grad_T_Champ_P1_CoviMAC& ch_gt=ref_cast(grad_T_Champ_P1_CoviMAC,ch.valeur());
  ch_gt.associer_zone_dis_base(zone_CoviMAC);
  ch_gt.associer_zone_Cl_dis_base(zone_cl_CoviMAC);
  ch_gt.associer_champ(temp);
  ch_gt.nommer("gradient_temperature");
  ch_gt.fixer_nb_comp(dimension);
  ch_gt.fixer_nb_valeurs_nodales(zone_CoviMAC.nb_elem());
  ch_gt.fixer_unite("K/m");
  ch_gt.changer_temps(ch_temperature.temps());
#endif
}

void CoviMAC_discretisation::grad_u(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse,Champ_Fonc& ch) const
{
  const Champ_Face_CoviMAC& vit = ref_cast(Champ_Face_CoviMAC,ch_vitesse.valeur());
  const Zone_CoviMAC& zone_poly=ref_cast(Zone_CoviMAC, z.valeur());
  const Zone_Cl_CoviMAC& zone_cl_poly=ref_cast(Zone_Cl_CoviMAC, zcl.valeur());
  ch.typer("grad_U_Champ_Face_CoviMAC");
  grad_U_Champ_Face_CoviMAC& ch_grad_u=ref_cast(grad_U_Champ_Face_CoviMAC,ch.valeur());
  ch_grad_u.associer_zone_dis_base(zone_poly);
  ch_grad_u.associer_zone_Cl_dis_base(zone_cl_poly);
  ch_grad_u.associer_champ(vit);
  ch_grad_u.nommer("gradient_vitesse");
  ch_grad_u.fixer_nb_comp(dimension*dimension);

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
  ch_grad_u.fixer_nature_du_champ(vectoriel);
  ch_grad_u.fixer_nb_valeurs_nodales(zone_poly.nb_elem());
  ch_grad_u.fixer_unite("s-1");
  ch_grad_u.changer_temps(ch_vitesse.temps());
}

void CoviMAC_discretisation::h_conv(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_temperature, Champ_Fonc& ch, Motcle& nom, int temp_ref) const
{
#ifdef dependance
  Cerr << "Discretisation de h_conv" << finl;
  const Champ_P1_CoviMAC& temp = ref_cast(Champ_P1_CoviMAC,ch_temperature.valeur());
  const Zone_CoviMAC& zone_CoviMAC=ref_cast(Zone_CoviMAC, z.valeur());
  const Zone_Cl_CoviMAC& zone_cl_CoviMAC=ref_cast(Zone_Cl_CoviMAC, zcl.valeur());
  ch.typer("h_conv_Champ_P1_CoviMAC");
  h_conv_Champ_P1_CoviMAC& ch_gt=ref_cast(h_conv_Champ_P1_CoviMAC,ch.valeur());
  ch_gt.associer_zone_dis_base(zone_CoviMAC);
  ch_gt.associer_zone_Cl_dis_base(zone_cl_CoviMAC);
  ch_gt.associer_champ(temp);
  ch_gt.temp_ref()=temp_ref;
  ////ch_gt.nommer("h_conv");
  ch_gt.nommer(nom);
  ch_gt.fixer_nb_comp(1);
  ch_gt.fixer_nb_valeurs_nodales(zone_CoviMAC.nb_elem());
  ch_gt.fixer_unite("W/m2.K");
  ch_gt.changer_temps(ch_temperature.temps());
#endif
}
void CoviMAC_discretisation::modifier_champ_tabule(const Zone_dis_base& zone_vdf,Champ_Fonc_Tabule& lambda_tab,const Champ_base&  ch_temper) const
{
  Champ_Fonc& lambda_tab_dis = lambda_tab.le_champ_tabule_discretise();
  lambda_tab_dis.typer("Champ_Fonc_Tabule_P0_CoviMAC");
  Champ_Fonc_Tabule_P0_CoviMAC& ch_tab_lambda_dis =
    ref_cast(Champ_Fonc_Tabule_P0_CoviMAC,lambda_tab_dis.valeur());
  //ch_tab_lambda_dis.nommer(nom_champ);
  ch_tab_lambda_dis.associer_zone_dis_base(zone_vdf);
  ch_tab_lambda_dis.associer_param(ch_temper,lambda_tab.table());
  ch_tab_lambda_dis.fixer_nb_comp(1);
  ch_tab_lambda_dis.fixer_nb_valeurs_nodales(zone_vdf.nb_elem());
// ch_tab_lambda_dis.fixer_unite(unite);
  ch_tab_lambda_dis.changer_temps(ch_temper.temps());
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

  else
    {
      return Discret_Thyd::get_name_of_type_for(class_operateur,type_operateur,eqn);
    }
  return type;
}
