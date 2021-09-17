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
// File:        EF_discretisation.cpp
// Directory:   $TRUST_ROOT/src/EF/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <EF_discretisation.h>
#include <Zone_EF.h>
#include <Champ_P1_EF.h>
#include <Champ_Q1_EF.h>
#include <Champ_Fonc_P0_EF.h>
#include <Rotationnel_Champ_P1_EF.h>
#include <Rotationnel_Champ_Q1_EF.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Fonc_Tabule_P0_EF.h>
#include <Milieu_base.h>
#include <Equation_base.h>
#include <Tri_EF.h>
#include <Tetra_EF.h>
#include <Segment_EF.h>
#include <Point_EF.h>
#include <Quadri_EF.h>
#include <Hexa_EF.h>
#include <Champ_Uniforme.h>
#include <DescStructure.h>
#include <Champ_Inc.h>
#include <Schema_Temps.h>
#include <Schema_Temps_base.h>
#include <Motcle.h>
#include <Zone_Cl_EF.h>
#include <Zone_Cl_dis.h>

Implemente_instanciable(EF_discretisation,"EF",Discret_Thyd);



Entree& EF_discretisation::readOn(Entree& s)
{
  return s ;
}

Sortie& EF_discretisation::printOn(Sortie& s) const
{
  return s ;
}

// Description:
// Discretisation d'un champ pour le EF en fonction d'une directive
// de discretisation. La directive est un Motcle comme "vitesse", "pression",
// "temperature", "champ_elem" (cree un champ de type P0), ...
// Cette methode determine le type du champ a creer en fonction du type d'element
// et de la directive de discretisation. Elle determine ensuite le nombre de ddl
// et fixe l'ensemble des parametres du champ (type, nb_compo, nb_ddl, nb_pas_dt,
// nom(s), unite(s) et nature du champ) et associe la Zone_dis au champ.
// Voir le code pour avoir la correspondance entre les directives et
// le type de champ cree.
void EF_discretisation::discretiser_champ(
  const Motcle& directive, const Zone_dis_base& z,
  Nature_du_champ nature,
  const Noms& noms, const Noms& unites,
  int nb_comp, int nb_pas_dt, double temps,
  Champ_Inc& champ, const Nom& sous_type ) const
{
  const Zone_EF& zone_EF = ref_cast(Zone_EF, z);

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
  if (sub_type(Tri_EF, zone_EF.type_elem().valeur())
      || sub_type(Segment_EF,zone_EF.type_elem().valeur())
      || sub_type(Tetra_EF,zone_EF.type_elem().valeur()))
    type_champ_vitesse = "Champ_P1_EF";
  else if (sub_type(Quadri_EF,zone_EF.type_elem().valeur())
           || sub_type(Hexa_EF,zone_EF.type_elem().valeur()))
    type_champ_vitesse = "Champ_Q1_EF";
  else
    {
      Cerr << "EF_discretisation::discretiser_champ :\n L'element geometrique ";
      Cerr << zone_EF.type_elem().valeur().que_suis_je();
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
      type = "Champ_P0_EF";
      default_nb_comp = 1;
      break;
    case 2:
      type = type_champ_vitesse;
      default_nb_comp = 1;
      break;
    case 3:
      type = "Champ_P0_EF";
      default_nb_comp = 1;
      break;
    case 4:
      type = type_champ_vitesse;
      default_nb_comp = dimension;
      break;
    case 5:
      type = "Champ_P0_EF";
      default_nb_comp = 1;
      break;
    case 6:
      type = "Champ_P1_EF";
      default_nb_comp = 1;
      break;
    default:
      assert(rang < 0);
      break;
    }

  if (directive == demande_description)
    Cerr << "EF_discretisation : " << motcles;

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
  if (type == "Champ_P0_EF")
    nb_ddl = z.nb_elem();
  else if (type == type_champ_vitesse)
    nb_ddl = zone_EF.nb_som();
  else if (type == "Champ_P1_EF")
    nb_ddl = zone_EF.nb_som();
  else
    assert(0);

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
// Idem que EF_discretisation::discretiser_champ(... , Champ_Inc)
void EF_discretisation::discretiser_champ(
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
// Idem que EF_discretisation::discretiser_champ(... , Champ_Inc)
void EF_discretisation::discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
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
// Idem que EF_discretisation::discretiser_champ(... , Champ_Inc)
// Traitement commun aux champ_fonc et champ_don.
// Cette methode est privee (passage d'un Objet_U pas propre vu
// de l'exterieur ...)
void EF_discretisation::discretiser_champ_fonc_don(
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

  const Zone_EF& zone_EF = ref_cast(Zone_EF, z);

  Motcles motcles(7);
  motcles[0] = "pression";    // Choix standard pour la pression
  motcles[1] = "temperature"; // Choix standard pour la temperature
  motcles[2] = "divergence_vitesse"; // Le type de champ obtenu en calculant div v
  motcles[3] = "champ_elem";  // Creer un champ aux elements (de type P0)
  motcles[6] = "champ_sommets";  // Creer un champ aux elements (de type P1)
  motcles[4] = "vitesse";     // Choix standard pour la vitesse
  motcles[5] = "gradient_pression";  // Le type de champ obtenu en calculant grad P

  // Le type de champ de vitesse depend du type d'element :
  Nom type_champ_vitesse;
  {
    const Elem_EF_base& elem_EF = zone_EF.type_elem().valeur();
    if (sub_type(Tri_EF, elem_EF) || sub_type(Segment_EF,elem_EF) ||sub_type(Tetra_EF, elem_EF) ||  sub_type(Point_EF,elem_EF) )
      type_champ_vitesse = "Champ_Fonc_P1_EF";
    else if (sub_type(Quadri_EF, elem_EF) || sub_type(Hexa_EF, elem_EF))
      type_champ_vitesse = "Champ_Fonc_Q1_EF";
    else
      {
        Cerr << "EF_discretisation::discretiser_champ :\n L'element geometrique ";
        Cerr << elem_EF.que_suis_je();
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
      type = "Champ_Fonc_P0_EF";
      default_nb_comp = 1;
      break;
    case 1:
      type = type_champ_vitesse;
      default_nb_comp = 1;
      break;
    case 2:
      type = "Champ_Fonc_P0_EF";
      default_nb_comp = 1;
      break;
    case 3:
      type = "Champ_Fonc_P0_EF";
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
      type = type_champ_vitesse;
      default_nb_comp = 1;
      break;
    default:
      assert(rang < 0);
      break;
    }

  if (directive == demande_description)
    Cerr << "EF_discretisation : " << motcles;

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
  if (type == "Champ_Fonc_P0_EF")     nb_ddl = z.nb_elem();
  else if (type == type_champ_vitesse) nb_ddl = zone_EF.nb_som();
  else assert(0);

  /* // Si c'est un champ multiscalaire, uh !
  if (nature == multi_scalaire)
    {
      // Pas encore code
      Cerr << "Champ multi_scalaire pas code" << finl;
      assert(0);
      exit();
    }
    else */
  {
    if (nb_comp < 0)
      nb_comp = default_nb_comp;
    assert(nb_comp > 0);
    if (champ_fonc)
      creer_champ(*champ_fonc, z, type, noms[0], unites[0], nb_comp, nb_ddl, temps,
                  directive, que_suis_je());
    else
      creer_champ(*champ_don, z, type, noms[0], unites[0], nb_comp, nb_ddl, temps,
                  directive, que_suis_je());
  }

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



void EF_discretisation::distance_paroi(const Schema_Temps_base& sch,
                                       Zone_dis& z, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de la distance paroi" << finl;
  Zone_EF& zone_EF=ref_cast(Zone_EF, z.valeur());
  ch.typer("Champ_Fonc_P0_EF");
  Champ_Fonc_P0_EF& ch_dist_paroi=ref_cast(Champ_Fonc_P0_EF,ch.valeur());
  ch_dist_paroi.associer_zone_dis_base(zone_EF);
  ch_dist_paroi.nommer("distance_paroi");
  ch_dist_paroi.fixer_nb_comp(1);
  ch_dist_paroi.fixer_nb_valeurs_nodales(zone_EF.nb_elem());
  ch_dist_paroi.fixer_unite("m");
  ch_dist_paroi.changer_temps(sch.temps_courant());
}



void EF_discretisation::vorticite(Zone_dis& z,const Champ_Inc& ch_vitesse,
                                  Champ_Fonc& ch) const
{
  Cerr << "Discretisation de la vorticite " << finl;
  const Zone_EF& zone_EF=ref_cast(Zone_EF, z.valeur());

  if (sub_type(Tri_EF,zone_EF.type_elem().valeur()) || sub_type(Segment_EF,zone_EF.type_elem().valeur()) || sub_type(Tetra_EF,zone_EF.type_elem().valeur()))
    {
      ch.typer("Rotationnel_Champ_P1_EF");
      const Champ_P1_EF& vit = ref_cast(Champ_P1_EF,ch_vitesse.valeur());
      Rotationnel_Champ_P1_EF& ch_W=ref_cast(Rotationnel_Champ_P1_EF,ch.valeur());
      ch_W.associer_zone_dis_base(zone_EF);
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
      ch_W.fixer_nb_valeurs_nodales(zone_EF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(ch_vitesse.temps());
    }
  else if (sub_type(Quadri_EF,zone_EF.type_elem().valeur()) || sub_type(Hexa_EF,zone_EF.type_elem().valeur()))
    {
      ch.typer("Rotationnel_Champ_Q1_EF");
      const Champ_Q1_EF& vit = ref_cast(Champ_Q1_EF,ch_vitesse.valeur());
      Rotationnel_Champ_Q1_EF& ch_W=ref_cast(Rotationnel_Champ_Q1_EF,ch.valeur());
      ch_W.associer_zone_dis_base(zone_EF);
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
      ch_W.fixer_nb_valeurs_nodales(zone_EF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(ch_vitesse.temps());
    }
  else
    {
      Cerr << "Pb dans le typage des elements dans EF_discretisation::vorticite" << finl;
      exit();
    }
}

void EF_discretisation::creer_champ_vorticite(const Schema_Temps_base& sch,
                                              const Champ_Inc& ch_vitesse,
                                              Champ_Fonc& ch) const
{
  if (sub_type(Champ_P1_EF,ch_vitesse.valeur()))
    {
      ch.typer("Rotationnel_Champ_P1_EF");
      const Champ_P1_EF& vit = ref_cast(Champ_P1_EF,ch_vitesse.valeur());
      const Zone_EF& zone_EF = ref_cast(Zone_EF,vit.zone_dis_base());
      Rotationnel_Champ_P1_EF& ch_W = ref_cast(Rotationnel_Champ_P1_EF,ch.valeur());
      ch_W.associer_zone_dis_base(zone_EF);
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
      ch_W.fixer_nb_valeurs_nodales(zone_EF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(sch.temps_courant());
    }
  else if (sub_type(Champ_Q1_EF,ch_vitesse.valeur()))
    {
      ch.typer("Rotationnel_Champ_Q1_EF");
      const Champ_Q1_EF& vit = ref_cast(Champ_Q1_EF,ch_vitesse.valeur());
      const Zone_EF& zone_EF = ref_cast(Zone_EF,vit.zone_dis_base());
      Rotationnel_Champ_Q1_EF& ch_W = ref_cast(Rotationnel_Champ_Q1_EF,ch.valeur());
      ch_W.associer_zone_dis_base(zone_EF);
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
      ch_W.fixer_nb_valeurs_nodales(zone_EF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(sch.temps_courant());
    }
  else
    {
      Cerr << "Pb dans le typage des elements dans EF_discretisation::creer_champ_vorticite" << finl;
      exit();
    }
}



// Description:
//    discretise en EF le fluide incompressible, donc  K e N
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
void EF_discretisation::proprietes_physiques_fluide_Ostwald(const Zone_dis& z, Fluide_Ostwald& le_fluide,
                                                            const Navier_Stokes_std& eqn_hydr, const Champ_Inc& ch_temper ) const
{

#ifdef dependance
  Cerr << "Discretisation EF du fluide_Ostwald" << finl;
  const Zone_EF& zone_EF=ref_cast(Zone_EF, z.valeur());
  const Champ_Inc& ch_vitesse = eqn_hydr.inconnue();
  const Champ_P1_EF& vit = ref_cast(Champ_P1_EF,ch_vitesse.valeur());




  Champ_Don& mu = le_fluide.viscosite_dynamique();
  //  mu est toujours un champ_Ostwald_EF , il faut toujours faire ce qui suit
  mu.typer("Champ_Ostwald_EF");
  Champ_Ostwald_EF& ch_mu = ref_cast(Champ_Ostwald_EF,mu.valeur());
  Cerr<<"associe zonedisbase EF"<<finl;
  ch_mu.associer_zone_dis_base(zone_EF);
  ch_mu.associer_fluide(le_fluide);
  ch_mu.associer_champ(vit);
  ch_mu.associer_eqn(eqn_hydr);
  Cerr<<"associations finies zone dis base, fluide, champ EF"<<finl;
  ch_mu.fixer_nb_comp(1);

  Cerr<<"fait fixer_nb_valeurs_nodales"<<finl;
  Cerr<<"nb_valeurs_nodales EF = "<<zone_EF.nb_elem()<<finl;
  ch_mu.fixer_nb_valeurs_nodales(zone_EF.nb_elem());

  Cerr<<"fait changer_temps"<<finl;
  ch_mu.changer_temps(vit.temps());
  Cerr<<"mu EF est discretise "<<finl;
#endif
}


void EF_discretisation::zone_Cl_dis(Zone_dis& z,
                                    Zone_Cl_dis& zcl) const
{
  Cerr << "discretisation des conditions limites" << finl;
  assert(z.non_nul());
  Zone_EF& zone_EF=ref_cast(Zone_EF, z.valeur());
  zcl.typer("Zone_Cl_EF");
  assert(zcl.non_nul());
  Zone_Cl_EF& zone_cl_EF=ref_cast(Zone_Cl_EF, zcl.valeur());
  zone_cl_EF.associer(zone_EF);
  Cerr << "discretisation des conditions limites OK" << finl;
}


void EF_discretisation::critere_Q(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
#ifdef dependance
  // On passe la zcl, pour qu'il n y ait qu une methode qqsoit la dsicretisation
  // mais on ne s'en sert pas!!!
  Cerr << "Discretisation du critere Q " << finl;
  const Champ_P1_EF& vit = ref_cast(Champ_P1_EF,ch_vitesse.valeur());
  const Zone_EF& zone_EF=ref_cast(Zone_EF, z.valeur());
  ch.typer("Critere_Q_Champ_P1_EF");
  Critere_Q_Champ_P1_EF& ch_cQ=ref_cast(Critere_Q_Champ_P1_EF,ch.valeur());
  ch_cQ.associer_zone_dis_base(zone_EF);
  ch_cQ.associer_champ(vit);
  ch_cQ.nommer("Critere_Q");
  ch_cQ.fixer_nb_comp(1);
  ch_cQ.fixer_nb_valeurs_nodales(zone_EF.nb_elem());
  ch_cQ.fixer_unite("s-2");
  ch_cQ.changer_temps(ch_vitesse.temps());
#endif
}


void EF_discretisation::y_plus(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{

#ifdef dependance
  Cerr << "Discretisation de y_plus" << finl;
  const Champ_P1_EF& vit = ref_cast(Champ_P1_EF,ch_vitesse.valeur());
  const Zone_EF& zone_EF=ref_cast(Zone_EF, z.valeur());
  const Zone_Cl_EF& zone_cl_EF=ref_cast(Zone_Cl_EF, zcl.valeur());
  ch.typer("Y_plus_Champ_P1_EF");
  Y_plus_Champ_P1_EF& ch_yp=ref_cast(Y_plus_Champ_P1_EF,ch.valeur());
  ch_yp.associer_zone_dis_base(zone_EF);
  ch_yp.associer_zone_Cl_dis_base(zone_cl_EF);
  ch_yp.associer_champ(vit);
  ch_yp.nommer("Y_plus");
  ch_yp.fixer_nb_comp(1);
  ch_yp.fixer_nb_valeurs_nodales(zone_EF.nb_elem());
  ch_yp.fixer_unite("adimensionnel");
  ch_yp.changer_temps(ch_vitesse.temps());
#endif
}

void EF_discretisation::grad_T(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_temperature, Champ_Fonc& ch) const
{
#ifdef dependance
  Cerr << "Discretisation de gradient_temperature" << finl;
  const Champ_P1_EF& temp = ref_cast(Champ_P1_EF,ch_temperature.valeur());
  const Zone_EF& zone_EF=ref_cast(Zone_EF, z.valeur());
  const Zone_Cl_EF& zone_cl_EF=ref_cast(Zone_Cl_EF, zcl.valeur());
  ch.typer("gradient_temperature_Champ_P1_EF");
  grad_T_Champ_P1_EF& ch_gt=ref_cast(grad_T_Champ_P1_EF,ch.valeur());
  ch_gt.associer_zone_dis_base(zone_EF);
  ch_gt.associer_zone_Cl_dis_base(zone_cl_EF);
  ch_gt.associer_champ(temp);
  ch_gt.nommer("gradient_temperature");
  ch_gt.fixer_nb_comp(dimension);
  ch_gt.fixer_nb_valeurs_nodales(zone_EF.nb_elem());
  ch_gt.fixer_unite("K/m");
  ch_gt.changer_temps(ch_temperature.temps());
#endif
}

void EF_discretisation::h_conv(const Zone_dis& z,const Zone_Cl_dis& zcl,const Champ_Inc& ch_temperature, Champ_Fonc& ch, Motcle& nom, int temp_ref) const
{
#ifdef dependance
  Cerr << "Discretisation de h_conv" << finl;
  const Champ_P1_EF& temp = ref_cast(Champ_P1_EF,ch_temperature.valeur());
  const Zone_EF& zone_EF=ref_cast(Zone_EF, z.valeur());
  const Zone_Cl_EF& zone_cl_EF=ref_cast(Zone_Cl_EF, zcl.valeur());
  ch.typer("h_conv_Champ_P1_EF");
  h_conv_Champ_P1_EF& ch_gt=ref_cast(h_conv_Champ_P1_EF,ch.valeur());
  ch_gt.associer_zone_dis_base(zone_EF);
  ch_gt.associer_zone_Cl_dis_base(zone_cl_EF);
  ch_gt.associer_champ(temp);
  ch_gt.temp_ref()=temp_ref;
  ////ch_gt.nommer("h_conv");
  ch_gt.nommer(nom);
  ch_gt.fixer_nb_comp(1);
  ch_gt.fixer_nb_valeurs_nodales(zone_EF.nb_elem());
  ch_gt.fixer_unite("W/m2.K");
  ch_gt.changer_temps(ch_temperature.temps());
#endif
}
void EF_discretisation::modifier_champ_tabule(const Zone_dis_base& zone_vdf,Champ_Fonc_Tabule& lambda_tab,const VECT(REF(Champ_base))&  champs_param) const
{
  Champ_Fonc& lambda_tab_dis = lambda_tab.le_champ_tabule_discretise();
  lambda_tab_dis.typer("Champ_Fonc_Tabule_P0_EF");
  Champ_Fonc_Tabule_P0_EF& ch_tab_lambda_dis =
    ref_cast(Champ_Fonc_Tabule_P0_EF,lambda_tab_dis.valeur());
  //ch_tab_lambda_dis.nommer(nom_champ);
  ch_tab_lambda_dis.associer_zone_dis_base(zone_vdf);
  ch_tab_lambda_dis.associer_param(champs_param, lambda_tab.table());
  ch_tab_lambda_dis.fixer_nb_comp(lambda_tab.nb_comp());
  ch_tab_lambda_dis.fixer_nb_valeurs_nodales(zone_vdf.nb_elem());
// ch_tab_lambda_dis.fixer_unite(unite);
  ch_tab_lambda_dis.changer_temps(champs_param[0].valeur().temps());
}

Nom  EF_discretisation::get_name_of_type_for(const Nom& class_operateur, const Nom& type_operateur,const Equation_base& eqn,  const REF(Champ_base)& champ_sup) const
{
  Nom type;
  if (class_operateur=="Source")
    {
      type=type_operateur;
      type+="_EF";
      return type;

    }
  else if (class_operateur=="Solveur_Masse")
    {
      type="Masse_EF";
    }
  else if (class_operateur=="Operateur_Grad")
    {
      type="Op_Grad_EF";
    }
  else if (class_operateur=="Operateur_Div")
    {
      type="Op_Div_EF";
    }

  else if (class_operateur=="Operateur_Diff")
    {
      type="Op_Diff" ;
      if (type_operateur!="" )
        {
          type+="_";
          type+=type_operateur;
        }
      type+="_EF";
    }
  else if (class_operateur=="Operateur_Conv")
    {
      type="Op_Conv_";
      type+=type_operateur;
      Nom tiret="_";
      type+= tiret;
      type+=que_suis_je();
    }

  else
    {
      return Discret_Thyd::get_name_of_type_for(class_operateur,type_operateur,eqn);
    }
  return type;
}
