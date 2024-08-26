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

#include <DG_discretisation.h>
#include <Domaine_DG.h>
#include <Champ_Fonc_Tabule.h>
//#include <Champ_Fonc_Elem_DG.h>
//#include <Champ_Fonc_Tabule_Elem_DG.h>
#include <Milieu_base.h>
#include <Equation_base.h>
#include <Champ_Uniforme.h>
#include <Champ_Inc.h>
#include <Schema_Temps_base.h>
#include <Motcle.h>
#include <Domaine_Cl_DG.h>
#include <Domaine_Cl_dis.h>
//#include <grad_U_Champ_Face_DG.h>

Implemente_instanciable(DG_discretisation, "DG", Discret_Thyd);
// XD polymac discretisation_base polymac -1 polymac discretization (polymac discretization that is not compatible with pb_multi).


Entree& DG_discretisation::readOn(Entree& s) { return s; }

Sortie& DG_discretisation::printOn(Sortie& s) const { return s; }

/*! @brief Discretisation d'un champ pour le DG en fonction d'une directive de discretisation.
 *
 * La directive est un Motcle comme "vitesse", "pression",
 *  "temperature", "champ_elem" (cree un champ de type P0), ...
 *  Cette methode determine le type du champ a creer en fonction du type d'element
 *  et de la directive de discretisation. Elle determine ensuite le nombre de ddl
 *  et fixe l'ensemble des parametres du champ (type, nb_compo, nb_ddl, nb_pas_dt,
 *  nom(s), unite(s) et nature du champ) et associe la Domaine_dis au champ.
 *  Voir le code pour avoir la correspondance entre les directives et
 *  le type de champ cree.
 *
 */
void DG_discretisation::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, int nb_pas_dt,
                                          double temps, Champ_Inc& champ, const Nom& sous_type) const
{
//  const Domaine_DG& domaine_DG = ref_cast(Domaine_DG, z);

  Motcles motcles(7);
  motcles[0] = "vitesse";     // Choix standard pour la vitesse
  motcles[1] = "pression";    // Choix standard pour la pression
  motcles[2] = "temperature"; // Choix standard pour la temperature
  motcles[3] = "divergence_vitesse"; // Le type de champ obtenu en calculant div v
  motcles[4] = "gradient_pression";  // Le type de champ obtenu en calculant grad P
  motcles[5] = "champ_elem";    // Creer un champ aux elements (de type P0)
  motcles[6] = "champ_sommets"; // Creer un champ aux sommets (type P1)

  // Le type de champ de vitesse depend du type d'element :
//  Nom type_champ_vitesse("Champ_Face_DG");
  Nom type_elem("Champ_Elem_DG");
  Nom type;
  int default_order = 0; // Valeur par defaut du nombre de composantes
  int rang = motcles.search(directive);
  switch(rang)
    {
    case 0: // TODO for velocity
      throw;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      type = type_elem;
      default_order = 1+Objet_U::dimension;
      break;
    default:
      assert(rang < 0);
      break;
    }

  if (directive == DEMANDE_DESCRIPTION)
    Cerr << "DG_discretisation : " << motcles;

  if (sous_type != NOM_VIDE)
    rang = verifie_sous_type(type, sous_type, directive);

  // Si on n'a pas compris la directive (ou si c'est une demande_description)
  // alors on appelle l'ancetre :
  if (rang < 0)
    {
      Discret_Thyd::discretiser_champ(directive, z, nature, noms, unites, nb_comp, nb_pas_dt, temps, champ);
      return;
    }

  // Calcul du nombre de ddl
  int nb_ddl = 0;
  if (type.debute_par(type_elem))
    nb_ddl = z.nb_elem();
  else
    assert(0);

  creer_champ(champ, z, type, noms[0], unites[0], default_order, nb_ddl, nb_pas_dt, temps, directive, que_suis_je());
  if (nature == multi_scalaire)
    {
      throw;
      champ->fixer_nature_du_champ(nature);
      champ->fixer_unites(unites);
      champ->fixer_noms_compo(noms);
    }
}

/*! @brief Idem que DG_discretisation::discretiser_champ(.
 *
 * .. , Champ_Inc)
 *
 */
void DG_discretisation::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, double temps,
                                          Champ_Fonc& champ) const
{
  discretiser_champ_fonc_don(directive, z, nature, noms, unites, nb_comp, temps, champ);
}

/*! @brief Idem que DG_discretisation::discretiser_champ(.
 *
 * .. , Champ_Inc)
 *
 */
void DG_discretisation::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, double temps,
                                          Champ_Don& champ) const
{
  discretiser_champ_fonc_don(directive, z, nature, noms, unites, nb_comp, temps, champ);
}

/*! @brief Idem que DG_discretisation::discretiser_champ(.
 *
 * .. , Champ_Inc) Traitement commun aux champ_fonc et champ_don.
 *  Cette methode est privee (passage d'un Objet_U pas propre vu
 *  de l'exterieur ...)
 *
 */
void DG_discretisation::discretiser_champ_fonc_don(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, double temps,
                                                   Objet_U& champ) const
{
  // Deux pointeurs pour acceder facilement au champ_don ou au champ_fonc,
  // suivant le type de l'objet champ.
  Champ_Fonc *champ_fonc = 0;
  Champ_Don *champ_don = 0;
  if (sub_type(Champ_Fonc, champ))
    champ_fonc = &ref_cast(Champ_Fonc, champ);
  else
    champ_don = &ref_cast(Champ_Don, champ);

  const Domaine_DG& domaine_DG = ref_cast(Domaine_DG, z);

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
  Nom type_champ_vitesse("Champ_Fonc_Face_DG");
  Nom type_elem("Champ_Fonc_Elem_DG");

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
      type = "Champ_Fonc_Som_DG";
      default_nb_comp = 1;
      break;
    default:
      assert(rang < 0);
      break;
    }

  if (directive == DEMANDE_DESCRIPTION)
    Cerr << "DG_discretisation : " << motcles;

  // Si on n'a pas compris la directive (ou si c'est une demande_description)
  // alors on appelle l'ancetre :
  if (rang < 0)
    {
      if (champ_fonc)
        Discret_Thyd::discretiser_champ(directive, z, nature, noms, unites, nb_comp, temps, *champ_fonc);
      else
        Discret_Thyd::discretiser_champ(directive, z, nature, noms, unites, nb_comp, temps, *champ_don);
      return;
    }

  // Calcul du nombre de ddl
  int nb_ddl = 0;
  if (type == "Champ_Fonc_Elem_DG")
    nb_ddl = z.nb_elem();
  else if (type == type_champ_vitesse)
    nb_ddl = domaine_DG.nb_faces();
  else if (type == "Champ_Fonc_Som_DG")
    nb_ddl = domaine_DG.nb_som();
  else
    assert(0);

  // Si c'est un champ multiscalaire, uh !
  if (nb_comp < 0)
    nb_comp = default_nb_comp;
  if (champ_fonc)
    creer_champ(*champ_fonc, z, type, noms[0], unites[0], nb_comp, nb_ddl, temps, directive, que_suis_je());
  else
    creer_champ(*champ_don, z, type, noms[0], unites[0], nb_comp, nb_ddl, temps, directive, que_suis_je());
  if ((nature == multi_scalaire) && (champ_fonc))
    {
      champ_fonc->valeur().fixer_nature_du_champ(nature);
      champ_fonc->valeur().fixer_unites(unites);
      champ_fonc->valeur().fixer_noms_compo(noms);
    }
  else if ((nature == multi_scalaire) && (champ_don))
    {
      Cerr << "There is no field of type Champ_Don with a multi_scalaire nature." << finl;
      exit();
    }
}

void DG_discretisation::distance_paroi(const Schema_Temps_base& sch, Domaine_dis& z, Champ_Fonc& ch) const
{
  throw;
//  Cerr << "Discretisation de la distance paroi" << finl;
//  Domaine_DG& domaine_DG = ref_cast(Domaine_DG, z.valeur());
//  ch.typer("Champ_Fonc_Elem_DG");
//  Champ_Fonc_Elem_DG& ch_dist_paroi = ref_cast(Champ_Fonc_Elem_DG, ch.valeur());
//  ch_dist_paroi.associer_domaine_dis_base(domaine_DG);
//  ch_dist_paroi.nommer("distance_paroi");
//  ch_dist_paroi.fixer_nb_comp(1);
//  ch_dist_paroi.fixer_nb_valeurs_nodales(domaine_DG.nb_elem());
//  ch_dist_paroi.fixer_unite("m");
//  ch_dist_paroi.changer_temps(sch.temps_courant());
}

void DG_discretisation::domaine_Cl_dis(Domaine_dis& z, Domaine_Cl_dis& zcl) const
{
  Cerr << "discretisation des conditions limites" << finl;
  assert(z.non_nul());
  Domaine_DG& domaine_DG = ref_cast(Domaine_DG, z.valeur());
  zcl.typer("Domaine_Cl_DG");
  assert(zcl.non_nul());
  Domaine_Cl_DG& domaine_cl_DG = ref_cast(Domaine_Cl_DG, zcl.valeur());
  domaine_cl_DG.associer(domaine_DG);
  Cerr << "discretisation des conditions limites OK" << finl;
}

void DG_discretisation::critere_Q(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  throw;
#ifdef dependance
  // On passe la zcl, pour qu'il n y ait qu une methode qqsoit la dsicretisation
  // mais on ne s'en sert pas!!!
  Cerr << "Discretisation du critere Q " << finl;
  const Champ_Som_DG& vit = ref_cast(Champ_Som_DG,ch_vitesse.valeur());
  const Domaine_DG& domaine_DG=ref_cast(Domaine_DG, z.valeur());
  ch.typer("Critere_Q_Champ_Som_DG");
  Critere_Q_Champ_Som_DG& ch_cQ=ref_cast(Critere_Q_Champ_Som_DG,ch.valeur());
  ch_cQ.associer_domaine_dis_base(domaine_DG);
  ch_cQ.associer_champ(vit);
  ch_cQ.nommer("Critere_Q");
  ch_cQ.fixer_nb_comp(1);
  ch_cQ.fixer_nb_valeurs_nodales(domaine_DG.nb_elem());
  ch_cQ.fixer_unite("s-2");
  ch_cQ.changer_temps(ch_vitesse.temps());
#endif
}

void DG_discretisation::y_plus(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  throw;
#ifdef dependance
  Cerr << "Discretisation de y_plus" << finl;
  const Champ_Som_DG& vit = ref_cast(Champ_Som_DG,ch_vitesse.valeur());
  const Domaine_DG& domaine_DG=ref_cast(Domaine_DG, z.valeur());
  const Domaine_Cl_DG& domaine_cl_DG=ref_cast(Domaine_Cl_DG, zcl.valeur());
  ch.typer("Y_plus_Champ_Som_DG");
  Y_plus_Champ_Som_DG& ch_yp=ref_cast(Y_plus_Champ_Som_DG,ch.valeur());
  ch_yp.associer_domaine_dis_base(domaine_DG);
  ch_yp.associer_domaine_Cl_dis_base(domaine_cl_DG);
  ch_yp.associer_champ(vit);
  ch_yp.nommer("Y_plus");
  ch_yp.fixer_nb_comp(1);
  ch_yp.fixer_nb_valeurs_nodales(domaine_DG.nb_elem());
  ch_yp.fixer_unite("adimensionnel");
  ch_yp.changer_temps(ch_vitesse.temps());
#endif
}

void DG_discretisation::grad_T(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_temperature, Champ_Fonc& ch) const
{
  throw;
#ifdef dependance
  Cerr << "Discretisation de gradient_temperature" << finl;
  const Champ_Som_DG& temp = ref_cast(Champ_Som_DG,ch_temperature.valeur());
  const Domaine_DG& domaine_DG=ref_cast(Domaine_DG, z.valeur());
  const Domaine_Cl_DG& domaine_cl_DG=ref_cast(Domaine_Cl_DG, zcl.valeur());
  ch.typer("gradient_temperature_Champ_Som_DG");
  grad_T_Champ_Som_DG& ch_gt=ref_cast(grad_T_Champ_Som_DG,ch.valeur());
  ch_gt.associer_domaine_dis_base(domaine_DG);
  ch_gt.associer_domaine_Cl_dis_base(domaine_cl_DG);
  ch_gt.associer_champ(temp);
  ch_gt.nommer("gradient_temperature");
  ch_gt.fixer_nb_comp(dimension);
  ch_gt.fixer_nb_valeurs_nodales(domaine_DG.nb_elem());
  ch_gt.fixer_unite("K/m");
  ch_gt.changer_temps(ch_temperature.temps());
#endif
}

void DG_discretisation::grad_u(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  throw;
//  const Champ_Face_DG& vit = ref_cast(Champ_Face_DG, ch_vitesse.valeur());
//  const Domaine_DG& domaine_poly = ref_cast(Domaine_DG, z.valeur());
//  const Domaine_Cl_DG& domaine_cl_poly = ref_cast(Domaine_Cl_DG, zcl.valeur());
//  ch.typer("grad_U_Champ_Face_DG");
//  grad_U_Champ_Face_DG& ch_grad_u = ref_cast(grad_U_Champ_Face_DG, ch.valeur());
//  ch_grad_u.associer_domaine_dis_base(domaine_poly);
//  ch_grad_u.associer_domaine_Cl_dis_base(domaine_cl_poly);
//  ch_grad_u.associer_champ(vit);
//  ch_grad_u.nommer("gradient_vitesse");
//  ch_grad_u.fixer_nb_comp(dimension * dimension);
//
//  if (dimension == 2)
//    {
//      ch_grad_u.fixer_nom_compo(0, "dUdX"); // du/dx
//      ch_grad_u.fixer_nom_compo(1, "dUdY"); // du/dy
//      ch_grad_u.fixer_nom_compo(2, "dVdX"); // dv/dx
//      ch_grad_u.fixer_nom_compo(3, "dVdY"); // dv/dy
//    }
//  else
//    {
//      ch_grad_u.fixer_nom_compo(0, "dUdX"); // du/dx
//      ch_grad_u.fixer_nom_compo(1, "dUdY"); // du/dy
//      ch_grad_u.fixer_nom_compo(2, "dUdZ"); // du/dz
//      ch_grad_u.fixer_nom_compo(3, "dVdX"); // dv/dx
//      ch_grad_u.fixer_nom_compo(4, "dVdY"); // dv/dy
//      ch_grad_u.fixer_nom_compo(5, "dVdZ"); // dv/dz
//      ch_grad_u.fixer_nom_compo(6, "dWdX"); // dw/dx
//      ch_grad_u.fixer_nom_compo(7, "dWdY"); // dw/dy
//      ch_grad_u.fixer_nom_compo(8, "dWdZ"); // dw/dz
//    }
//  ch_grad_u.fixer_nature_du_champ(vectoriel);
//  ch_grad_u.fixer_nb_valeurs_nodales(domaine_poly.nb_elem());
//  ch_grad_u.fixer_unite("s-1");
//  ch_grad_u.changer_temps(ch_vitesse.temps());
}

void DG_discretisation::h_conv(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_temperature, Champ_Fonc& ch, Motcle& nom, int temp_ref) const
{
#ifdef dependance
  Cerr << "Discretisation de h_conv" << finl;
  const Champ_Som_DG& temp = ref_cast(Champ_Som_DG,ch_temperature.valeur());
  const Domaine_DG& domaine_DG=ref_cast(Domaine_DG, z.valeur());
  const Domaine_Cl_DG& domaine_cl_DG=ref_cast(Domaine_Cl_DG, zcl.valeur());
  ch.typer("h_conv_Champ_Som_DG");
  h_conv_Champ_Som_DG& ch_gt=ref_cast(h_conv_Champ_Som_DG,ch.valeur());
  ch_gt.associer_domaine_dis_base(domaine_DG);
  ch_gt.associer_domaine_Cl_dis_base(domaine_cl_DG);
  ch_gt.associer_champ(temp);
  ch_gt.temp_ref()=temp_ref;
  ////ch_gt.nommer("h_conv");
  ch_gt.nommer(nom);
  ch_gt.fixer_nb_comp(1);
  ch_gt.fixer_nb_valeurs_nodales(domaine_DG.nb_elem());
  ch_gt.fixer_unite("W/m2.K");
  ch_gt.changer_temps(ch_temperature.temps());
#endif
}
void DG_discretisation::modifier_champ_tabule(const Domaine_dis_base& domaine_poly, Champ_Fonc_Tabule& lambda_tab, const VECT(REF(Champ_base)) &champs_param) const
{
  throw;
//  Champ_Fonc& lambda_tab_dis = lambda_tab.le_champ_tabule_discretise();
//  lambda_tab_dis.typer("Champ_Fonc_Tabule_Elem_DG");
//  Champ_Fonc_Tabule_Elem_DG& ch_tab_lambda_dis = ref_cast(Champ_Fonc_Tabule_Elem_DG, lambda_tab_dis.valeur());
//  //ch_tab_lambda_dis.nommer(nom_champ);
//  ch_tab_lambda_dis.associer_domaine_dis_base(domaine_poly);
//  ch_tab_lambda_dis.associer_param(champs_param, lambda_tab.table());
//  ch_tab_lambda_dis.fixer_nb_comp(lambda_tab.nb_comp());
//  ch_tab_lambda_dis.fixer_nb_valeurs_nodales(domaine_poly.nb_elem());
//  // ch_tab_lambda_dis.fixer_unite(unite);
//  ch_tab_lambda_dis.changer_temps(champs_param[0]->temps());
}

Nom DG_discretisation::get_name_of_type_for(const Nom& class_operateur, const Nom& type_operateur, const Equation_base& eqn, const REF(Champ_base) &champ_sup) const
{
  Nom type;
  if (class_operateur == "Source")
    {
      type = type_operateur;
      Nom champ = (eqn.inconnue()->que_suis_je());
      champ.suffix("Champ");
      type += champ;
      //type+="_DG";
      return type;

    }
  else if (class_operateur == "Solveur_Masse")
    {
      Nom type_ch = eqn.inconnue()->que_suis_je();
      if (type_ch.debute_par("Champ_Elem"))
        type_ch = "_Elem";

      if (type_ch.debute_par("Champ_Face"))
        type_ch = "_Face";

      type = "Masse_DG";
      type += type_ch;
    }
  else if (class_operateur == "Operateur_Grad")
    {
      type = "Op_Grad_DG_Face";
    }
  else if (class_operateur == "Operateur_Div")
    {
      type = "Op_Div_DG";
    }

  else if (class_operateur == "Operateur_Diff")
    {
      Nom type_ch = eqn.inconnue()->que_suis_je();
      if (type_ch.debute_par("Champ_Elem"))
        type_ch = "_Elem";

      if (type_ch.debute_par("Champ_Face"))
        type_ch = "_Face";

      type = "Op_Diff";
      if (type_operateur != "")
        {
          type += "_";
          type += type_operateur;
        }
      type += "_DG";
      type += type_ch;
    }
  else if (class_operateur == "Operateur_Conv")
    {
      type = "Op_Conv_";
      type += type_operateur;
      Nom tiret = "_";
      type += tiret;
      type += que_suis_je();
      Nom type_ch = eqn.inconnue()->que_suis_je();
      if (type_ch.debute_par("Champ_Elem"))
        type += "_Elem";
      if (type_ch.debute_par("Champ_Face"))
        type += "_Face";
      type += "_DG";
    }

  else
    return Discret_Thyd::get_name_of_type_for(class_operateur, type_operateur, eqn);

  return type;
}

void DG_discretisation::distance_paroi_globale(const Schema_Temps_base& sch, Domaine_dis& z, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de distance paroi globale" << finl;
  Noms noms(1), unites(1);
  noms[0] = Nom("distance_paroi_globale");
  unites[0] = Nom("m");
  discretiser_champ(Motcle("champ_elem"), z.valeur(), scalaire, noms, unites, 1, 0, ch);
}
