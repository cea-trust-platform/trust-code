/****************************************************************************
* Copyright (c) 2025, CEA
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
#include <Milieu_base.h>
#include <Equation_base.h>
#include <Champ_Uniforme.h>
#include <Champ_Inc_base.h>
#include <Schema_Temps_base.h>
#include <Motcle.h>
#include <Domaine_Cl_DG.h>
#include <Option_DG.h>

Implemente_instanciable(DG_discretisation, "DG", Discret_Thyd);
// XD DG discretisation_base DG -1 DG discretization


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
void DG_discretisation::discretiser_champ(const Motcle& directive, const Domaine_dis_base& dom_dis, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, int nb_pas_dt,
                                          double temps, OWN_PTR(Champ_Inc_base)& champ, const Nom& sous_type) const
{
//  const Domaine_DG& domaine_DG = ref_cast(Domaine_DG, dom_dis);

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
  const int order_DG = Option_DG::Get_order_for(noms[0]);
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
      default_order = Option_DG::Nb_col_from_order(order_DG);
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
      Discret_Thyd::discretiser_champ(directive, dom_dis, nature, noms, unites, nb_comp, nb_pas_dt, temps, champ);
      return;
    }

  // Calcul du nombre de ddl
  int nb_ddl = 0;
  if (type.debute_par(type_elem))
    nb_ddl = dom_dis.nb_elem();
  else
    assert(0);

  creer_champ(champ, dom_dis, type, noms[0], unites[0], default_order, nb_ddl, nb_pas_dt, temps, directive, que_suis_je());
  
	champ->fixer_nature_du_champ(basis_function);

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
                                          OWN_PTR(Champ_Fonc_base)& champ) const
{
  discretiser_champ_fonc_don(directive, z, nature, noms, unites, nb_comp, temps, champ);
}

/*! @brief Idem que DG_discretisation::discretiser_champ(.
 *
 * .. , Champ_Inc)
 *
 */
void DG_discretisation::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, double temps,
                                          OWN_PTR(Champ_Don_base)& champ) const
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
  // Deux pointeurs pour acceder facilement au champ_don ou au champ_fonc, suivant le type de l'objet champ.
  OWN_PTR(Champ_Fonc_base) * champ_fonc = dynamic_cast<OWN_PTR(Champ_Fonc_base)*>(&champ);
  OWN_PTR(Champ_Don_base) * champ_don = dynamic_cast<OWN_PTR(Champ_Don_base)*>(&champ);

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

  Nom type;
  int default_nb_comp = 0; // Valeur par defaut du nombre de composantes
  int rang = motcles.search(directive);
  const Nom& type_elem_geom = domaine_DG.domaine().type_elem()->que_suis_je();
  int nb_pts_integ=-1;
  if (type_elem_geom == "Triangle")
    nb_pts_integ = 3; // 3 for quad 2 7 for quad5
  else if (type_elem_geom == "Quadrangle")
    nb_pts_integ=9; // 9 for quad2 16 for quad5

//  const int order_DG = Option_DG::Get_order_for(directive);
  switch(rang)
    {
    case 0:
    case 1:
      type = "Champ_Fonc_P1_DG";
      default_nb_comp = nb_pts_integ; //Option_DG::Nb_col_from_order(order_DG);;
      break;
    case 2:
    case 3:
      type = "Champ_Fonc_Elem_DG";
      default_nb_comp = 1;
      break;
    case 4:
    case 5:
    case 7:
      type = "Champ_Fonc_P1_DG";
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
  if (type == "Champ_Fonc_Elem_DG" || type == "Champ_Fonc_P1_DG")
    nb_ddl = z.nb_elem();
  else if (type == "Champ_Fonc_Som_DG")
    nb_ddl = domaine_DG.nb_som();
  else
    assert(0);

  // Si c'est un champ multiscalaire, uh !
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

void DG_discretisation::distance_paroi(const Schema_Temps_base& sch, Domaine_dis_base& z, OWN_PTR(Champ_Fonc_base)& ch) const
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


void DG_discretisation::grad_u(const Domaine_dis_base& z, const Domaine_Cl_dis_base& zcl, const Champ_Inc_base& ch_vitesse, OWN_PTR(Champ_Fonc_base)& ch) const
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


void DG_discretisation::modifier_champ_tabule(const Domaine_dis_base& domaine_poly, Champ_Fonc_Tabule& lambda_tab, const VECT(OBS_PTR(Champ_base)) &champs_param) const
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

Nom DG_discretisation::get_name_of_type_for(const Nom& class_operateur, const Nom& type_operateur, const Equation_base& eqn, const OBS_PTR(Champ_base) &champ_sup) const
{
  Nom type;
  if (class_operateur == "Source")
    {
      type = type_operateur;
      Nom champ = (eqn.inconnue().que_suis_je());
      champ.suffix("Champ");
      type += champ;
      //type+="_DG";
      return type;

    }
  else if (class_operateur == "Solveur_Masse")
    {
      Nom type_ch = eqn.inconnue().que_suis_je();
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
      Nom type_ch = eqn.inconnue().que_suis_je();
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
      Nom type_ch = eqn.inconnue().que_suis_je();
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

void DG_discretisation::distance_paroi_globale(const Schema_Temps_base& sch, Domaine_dis_base& z, OWN_PTR(Champ_Fonc_base)& ch) const
{
  Cerr << "Discretisation de distance paroi globale" << finl;
  Noms noms(1), unites(1);
  noms[0] = Nom("distance_paroi_globale");
  unites[0] = Nom("m");
  discretiser_champ(Motcle("champ_elem"), z, scalaire, noms, unites, 1, 0, ch);
}
