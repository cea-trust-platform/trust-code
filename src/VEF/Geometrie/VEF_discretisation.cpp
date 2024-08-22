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

#include <Taux_cisaillement_P0_VEF.h>
#include <Champ_Fonc_Tabule_P0_VEF.h>
#include <Correlation_Vec_Sca_VEF.h>
#include <Rotationnel_Champ_P1NC.h>
#include <Rotationnel_Champ_Q1NC.h>
#include <Critere_Q_Champ_P1NC.h>
#include <VEF_discretisation.h>
#include <Y_plus_Champ_P1NC.h>
#include <Navier_Stokes_std.h>
#include <grad_T_Champ_P1NC.h>
#include <grad_U_Champ_P1NC.h>
#include <h_conv_Champ_P1NC.h>
#include <Champ_Ostwald_VEF.h>
#include <Champ_Fonc_Tabule.h>
#include <Schema_Temps_base.h>
#include <Champ_Fonc_Q1NC.h>
#include <Fluide_Ostwald.h>
#include <Champ_Uniforme.h>
#include <Domaine_VEF.h>
#include <Quadri_VEF.h>
#include <Tetra_VEF.h>
#include <Hexa_VEF.h>
#include <Tri_VEF.h>

Implemente_instanciable(VEF_discretisation, "VEFPreP1B|VEF", Discret_Thyd);
// XD vef discretisation_base vefprep1b -1 Finite element volume discretization (P1NC/P1-bubble element). Since the 1.5.5 version, several new discretizations are available thanks to the optional keyword Read. By default, the VEFPreP1B keyword is equivalent to the former VEFPreP1B formulation (v1.5.4 and sooner). P0P1 (if used with the strong formulation for imposed pressure boundary) is equivalent to VEFPreP1B but the convergence is slower. VEFPreP1B dis is equivalent to VEFPreP1B dis Read dis { P0 P1 Changement_de_base_P1Bulle 1 Cl_pression_sommet_faible 0 }

// Par defaut, P0+P1 et changement de base pour faire P0+P1->P1Bulle
Entree& VEF_discretisation::readOn(Entree& is)
{
  alphaE_ = alphaS_ = alphaA_ = alphaRT_ = P1Bulle_ = 0;
  cl_pression_sommet_faible_ = 1;
  modif_div_face_dirichlet_ = 0;

  Param param(que_suis_je());
  param.ajouter("changement_de_base_P1bulle", &P1Bulle_); // XD_ADD_P entier(into=[0,1]) changement_de_base_p1bulle 1 This option may be used to have the P1NC/P0P1 formulation (value set to 0) or the P1NC/P1Bulle formulation (value set to 1, the default).
  param.ajouter_flag("P0", &alphaE_); // XD_ADD_P rien Pressure nodes are added on element centres
  param.ajouter_flag("P1", &alphaS_); // XD_ADD_P rien Pressure nodes are added on vertices
  param.ajouter_flag("Pa", &alphaA_); // XD_ADD_P rien Only available in 3D, pressure nodes are added on bones
  param.ajouter_flag("RT", &alphaRT_); // XD_ADD_P rien For P1NCP1B (in TrioCFD)
  param.ajouter("modif_div_face_dirichlet", &modif_div_face_dirichlet_); // XD_ADD_P entier(into=[0,1]) This option (by default 0) is used to extend control volumes for the momentum equation.
  param.ajouter("CL_pression_sommet_faible", &cl_pression_sommet_faible_); // XD_ADD_P entier(into=[0,1]) This option is used to specify a strong formulation (value set to 0, the default) or a weak formulation (value set to 1) for an imposed pressure boundary condition. The first formulation converges quicker and is stable in general cases. The second formulation should be used if there are several outlet boundaries with Neumann condition (see Ecoulement_Neumann test case for example).
  param.lire_avec_accolades(is);

  // Quelques verifications
  if (dimension != 3 && alphaA_)
    {
      Cerr << "Le support Pa n'est disponible qu'en 3D." << finl;
      Process::exit();
    }
  if (alphaE_ + alphaS_ + alphaA_ == 0)
    {
      Cerr << "Il faut choisir au moins un support parmi P0, P1, Pa." << finl;
      Process::exit();
    }
  if (alphaA_ == 1 && !alphaE_)
    {
      Cerr << "Les discretisations Pa ou P1+Pa ne sont pas encore supportees." << finl;
      Process::exit();
    }
  if (P1Bulle_)
    if (!((alphaE_ == 1) && (alphaS_ == 1) && (alphaA_ == 0)))
      {
        Cerr << "L'option changement_de_base_P1bulle n'est disponible qu'en P0/P1" << finl;
        Process::exit();
      }

  if (alphaRT_)
    {
      if (alphaE_ + alphaS_ + alphaA_ == 0)
        {
          Cerr << "Choose P0 discretization." << finl;
          Process::exit();
        }
      Cerr << "Linke's scheme, OK for steady-state pb. The pressure computed is the Bernoulli pressure P =p+|u|^2" << finl;
    }
  return is;
}

Sortie& VEF_discretisation::printOn(Sortie& s) const
{
  s << "lire " << le_nom() << " { " << finl;
  if (alphaE_) s << " P0";
  if (alphaS_) s << " P1";
  if (alphaA_) s << " Pa";
  if (alphaRT_) s << " RT";
  s << " changement_de_base_P1bulle " << P1Bulle_;
  s << " modif_div_face_dirichlet " << modif_div_face_dirichlet_;
  s << " cl_pression_sommet_faible " << cl_pression_sommet_faible_;
  s << " } " << finl;
  return s;
}

/*! @brief Discretisation d'un champ pour le VEFP1B en fonction d'une directive de discretisation.
 *
 * La directive est un Motcle comme "pression",
 *  "divergence_vitesse" (cree un champ de type P1_isoP1Bulle).
 *  Cette methode determine le type du champ a creer en fonction du type d'element
 *  et de la directive de discretisation. Elle determine ensuite le nombre de ddl
 *  et fixe l'ensemble des parametres du champ (type, nb_compo, nb_ddl, nb_pas_dt,
 *  nom(s), unite(s), nature du champ et attribue un temps) et associe le Domaine_dis au champ.
 *  Voir le code pour avoir la correspondance entre les directives et
 *  le type de champ cree.
 *
 */
void VEF_discretisation::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, int nb_pas_dt, double temps,
                                           Champ_Inc& champ, const Nom& sous_type) const
{
  Motcles motcles(2);
  motcles[0] = "pression";    // Choix standard pour la pression
  motcles[1] = "divergence_vitesse"; // Le type de champ obtenu en calculant div v

  Nom type;
  int default_nb_comp = 0; // Valeur par defaut du nombre de composantes
  int rang = motcles.search(directive);
  switch(rang)
    {
    case 0:
      type = "Champ_P1_isoP1Bulle";
      default_nb_comp = 1;
      break;
    case 1:
      type = "Champ_P1_isoP1Bulle";
      default_nb_comp = 1;
      break;
    default:
      assert(rang < 0);
      break;
    }

  if (directive == DEMANDE_DESCRIPTION)
    Cerr << "VEF_discretisation : " << motcles;

  // Si on n'a pas compris la directive (ou si c'est une demande_description)
  // alors on appelle l'ancetre :
  if (rang < 0)
    {
      VEF_discretisation::discretiser_champ_(directive, z, nature, noms, unites, nb_comp, nb_pas_dt, temps, champ);
      return;
    }

  const int nb_ddl = -1; // c'est le descripteur p1b qui donnera le chiffre
  if (nb_comp < 0)
    nb_comp = default_nb_comp;
  assert(nb_comp > 0);
  creer_champ(champ, z, type, noms[0], unites[0], nb_comp, nb_ddl, nb_pas_dt, temps, directive, que_suis_je());

  if (nature == multi_scalaire)
    {
      Cerr << "There is no field of type Champ_Inc with P1Bulle discretization" << finl;
      Cerr << "and a multi_scalaire nature." << finl;
      exit();
    }
}

void VEF_discretisation::discretiser_champ_(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, int nb_pas_dt, double temps,
                                            Champ_Inc& champ, const Nom& sous_type) const
{
  const Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z);

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
  if (sub_type(Tri_VEF, domaine_vef.type_elem().valeur()) || sub_type(Tetra_VEF, domaine_vef.type_elem().valeur()))
    type_champ_vitesse = "Champ_P1NC";
  else if (sub_type(Quadri_VEF,domaine_vef.type_elem().valeur()) || sub_type(Hexa_VEF, domaine_vef.type_elem().valeur()))
    type_champ_vitesse = "Champ_Q1NC";
  else
    {
      Cerr << "VEF_discretisation::discretiser_champ :\n L'element geometrique ";
      Cerr << domaine_vef.type_elem()->que_suis_je();
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

  if (directive == DEMANDE_DESCRIPTION)
    Cerr << "VEF_discretisation : " << motcles;

  // Si on n'a pas compris la directive (ou si c'est une demande_description)
  // alors on appelle l'ancetre :
  if (rang < 0)
    {
      Discret_Thyd::discretiser_champ(directive, z, nature, noms, unites, nb_comp, nb_pas_dt, temps, champ);
      return;
    }

  // Calcul du nombre de ddl
  int nb_ddl = 0;
  if (type == "Champ_P0_VEF")
    nb_ddl = z.nb_elem();
  else if (type == type_champ_vitesse)
    nb_ddl = domaine_vef.nb_faces();
  else if (type == "Champ_P1_VEF")
    nb_ddl = domaine_vef.nb_som();
  else
    assert(0);

  if (nb_comp < 0)
    nb_comp = default_nb_comp;
  assert(nb_comp > 0);
  creer_champ(champ, z, type, noms[0], unites[0], nb_comp, nb_ddl, nb_pas_dt, temps, directive, que_suis_je());
  if (nature == multi_scalaire)
    {
      champ->fixer_nature_du_champ(nature);
      champ->fixer_unites(unites);
      champ->fixer_noms_compo(noms);
    }

}

/*! @brief Idem que VEF_discretisation::discretiser_champ(.
 *
 * .. , Champ_Inc)
 *
 */
void VEF_discretisation::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, double temps,
                                           Champ_Fonc& champ) const
{
  discretiser_champ_fonc_don(directive, z, nature, noms, unites, nb_comp, temps, champ);
}

/*! @brief Idem que VEF_discretisation::discretiser_champ(.
 *
 * .. , Champ_Inc)
 *
 */
void VEF_discretisation::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, double temps,
                                           Champ_Don& champ) const
{
  discretiser_champ_fonc_don(directive, z, nature, noms, unites, nb_comp, temps, champ);
}

/*! @brief Idem que VEF_discretisation::discretiser_champ(.
 *
 * .. , Champ_Inc) Traitement commun aux champ_fonc et champ_don.
 *  Cette methode est privee (passage d'un Objet_U pas propre vu
 *  de l'exterieur ...)
 *
 */
void VEF_discretisation::discretiser_champ_fonc_don(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, double temps,
                                                    Objet_U& champ) const
{
  // Deux pointeurs pour acceder facilement au champ_don ou au champ_fonc,
  // suivant le type de l'objet champ.
  Champ_Fonc *champ_fonc = nullptr;
  Champ_Don *champ_don = nullptr;
  if (sub_type(Champ_Fonc, champ))
    champ_fonc = &ref_cast(Champ_Fonc, champ);
  else
    champ_don = dynamic_cast<Champ_Don*>(&champ);

  Motcles motcles(2);
  motcles[0] = "pression";    // Choix standard pour la pression
  motcles[1] = "divergence_vitesse"; // Le type de champ obtenu en calculant div v

  Nom type;
  int default_nb_comp = 0; // Valeur par defaut du nombre de composantes
  int rang = motcles.search(directive);
  switch(rang)
    {
    case 0:
      type = "Champ_Fonc_P1_isoP1Bulle";
      default_nb_comp = 1;
      break;
    case 1:
      type = "Champ_Fonc_P1_isoP1Bulle";
      default_nb_comp = 1;
      break;
    default:
      assert(rang < 0);
      break;
    }

  if (directive == DEMANDE_DESCRIPTION)
    Cerr << "VEF_discretisation : " << motcles;

  // Si on n'a pas compris la directive (ou si c'est une demande_description)
  // alors on appelle l'ancetre :
  if (rang < 0)
    {
      if (champ_fonc)
        VEF_discretisation::discretiser_champ_fonc_don_(directive, z, nature, noms, unites, nb_comp, temps, *champ_fonc);
      else
        VEF_discretisation::discretiser_champ_fonc_don_(directive, z, nature, noms, unites, nb_comp, temps, *champ_don);
      return;
    }

  int nb_ddl = -1; // c'est le descripteur p1b qui donnera le chiffre

  if (nb_comp < 0)
    nb_comp = default_nb_comp;
  assert(nb_comp > 0);
  if (champ_fonc)
    creer_champ(*champ_fonc, z, type, noms[0], unites[0], nb_comp, nb_ddl, temps, directive, que_suis_je());
  else
    creer_champ(*champ_don, z, type, noms[0], unites[0], nb_comp, nb_ddl, temps, directive, que_suis_je());

  if (nature == multi_scalaire)
    {
      Cerr << "There is no field of type Champ_Fonc or Champ_Don with P1Bulle discretization" << finl;
      Cerr << "and a multi_scalaire nature." <<finl;
      exit();
    }
}

void VEF_discretisation::discretiser_champ_fonc_don_(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, double temps,
                                                     Objet_U& champ) const
{
  // Deux pointeurs pour acceder facilement au champ_don ou au champ_fonc,
  // suivant le type de l'objet champ.
  Champ_Fonc *champ_fonc = nullptr;
  Champ_Don *champ_don = nullptr;
  if (sub_type(Champ_Fonc, champ))
    champ_fonc = &ref_cast(Champ_Fonc, champ);
  else
    champ_don = dynamic_cast<Champ_Don*>(&champ);

  const Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z);

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
  Nom type_elem_domaine = domaine_vef.domaine().type_elem()->que_suis_je();
  if (Motcle(type_elem_domaine) != "Segment")
    {
      const Elem_VEF_base& elem_vef = domaine_vef.type_elem().valeur();
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

  if (directive == DEMANDE_DESCRIPTION)
    Cerr << "VEF_discretisation : " << motcles;

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
  if (type == "Champ_Fonc_P0_VEF")
    nb_ddl = z.nb_elem();
  else if (type == type_champ_vitesse)
    nb_ddl = domaine_vef.nb_faces();
  else if ((type == "Champ_Fonc_P1_VEF") || (type == "Champ_Fonc_Q1_VEF"))
    nb_ddl = domaine_vef.nb_som();
  else
    assert(0);

  if (nb_comp < 0)
    nb_comp = default_nb_comp;
  assert(nb_comp > 0);
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

void VEF_discretisation::distance_paroi(const Schema_Temps_base& sch, Domaine_dis& z, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de la distance paroi" << finl;
  Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z.valeur());
  ch.typer("Champ_Fonc_P0_VEF");
  Champ_Fonc_P0_VEF& ch_dist_paroi = ref_cast(Champ_Fonc_P0_VEF, ch.valeur());
  ch_dist_paroi.associer_domaine_dis_base(domaine_vef);
  ch_dist_paroi.nommer("distance_paroi");
  ch_dist_paroi.fixer_nb_comp(1);
  ch_dist_paroi.fixer_nb_valeurs_nodales(domaine_vef.nb_elem());
  ch_dist_paroi.fixer_unite("m");
  ch_dist_paroi.changer_temps(sch.temps_courant());
}

void VEF_discretisation::distance_paroi_globale(const Schema_Temps_base& sch, Domaine_dis& z, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de distance paroi globale" << finl;
  Noms noms(1), unites(1);
  noms[0] = Nom("distance_paroi_globale");
  unites[0] = Nom("m");
  discretiser_champ(Motcle("champ_elem"), z.valeur(), scalaire, noms , unites, 1, 0, ch);
}

void VEF_discretisation::vorticite(Domaine_dis& z, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de la vorticite " << finl;
  const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, z.valeur());

  if (sub_type(Tri_VEF,domaine_VEF.type_elem().valeur()) || sub_type(Tetra_VEF, domaine_VEF.type_elem().valeur()))
    {
      ch.typer("Rotationnel_Champ_P1NC");
      const Champ_P1NC& vit = ref_cast(Champ_P1NC, ch_vitesse.valeur());
      Rotationnel_Champ_P1NC& ch_W = ref_cast(Rotationnel_Champ_P1NC, ch.valeur());
      ch_W.associer_domaine_dis_base(domaine_VEF);
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
      ch_W.fixer_nb_valeurs_nodales(domaine_VEF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(ch_vitesse->temps());
    }
  else if (sub_type(Quadri_VEF,domaine_VEF.type_elem().valeur()) || sub_type(Hexa_VEF, domaine_VEF.type_elem().valeur()))
    {
      ch.typer("Rotationnel_Champ_Q1NC");
      const Champ_Q1NC& vit = ref_cast(Champ_Q1NC, ch_vitesse.valeur());
      Rotationnel_Champ_Q1NC& ch_W = ref_cast(Rotationnel_Champ_Q1NC, ch.valeur());
      ch_W.associer_domaine_dis_base(domaine_VEF);
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
      ch_W.fixer_nb_valeurs_nodales(domaine_VEF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(ch_vitesse->temps());
    }
  else
    {
      Cerr << "Pb dans le typage des elements dans VEF_discretisation::vorticite" << finl;
      exit();
    }
}

void VEF_discretisation::creer_champ_vorticite(const Schema_Temps_base& sch, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  if (sub_type(Champ_P1NC, ch_vitesse.valeur()))
    {
      ch.typer("Rotationnel_Champ_P1NC");
      const Champ_P1NC& vit = ref_cast(Champ_P1NC, ch_vitesse.valeur());
      const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, vit.domaine_dis_base());
      Rotationnel_Champ_P1NC& ch_W = ref_cast(Rotationnel_Champ_P1NC, ch.valeur());
      ch_W.associer_domaine_dis_base(domaine_VEF);
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
      ch_W.fixer_nb_valeurs_nodales(domaine_VEF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(sch.temps_courant());
    }
  else if (sub_type(Champ_Q1NC, ch_vitesse.valeur()))
    {
      ch.typer("Rotationnel_Champ_Q1NC");
      const Champ_Q1NC& vit = ref_cast(Champ_Q1NC, ch_vitesse.valeur());
      const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, vit.domaine_dis_base());
      Rotationnel_Champ_Q1NC& ch_W = ref_cast(Rotationnel_Champ_Q1NC, ch.valeur());
      ch_W.associer_domaine_dis_base(domaine_VEF);
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
      ch_W.fixer_nb_valeurs_nodales(domaine_VEF.nb_elem());
      ch_W.fixer_unite("s-1");
      ch_W.changer_temps(sch.temps_courant());
    }
  else
    {
      Cerr << "Pb dans le typage des elements dans VEF_discretisation::creer_champ_vorticite" << finl;
      exit();
    }
}

/*! @brief discretise en VEF le fluide incompressible, donc  K e N
 *
 * @param (Domaine_dis&) domaine a discretiser
 * @param (Fluide_Ostwald&) fluide a discretiser
 * @param (Champ_Inc&) vitesse
 * @param (Champ_Inc&) temperature
 */
void VEF_discretisation::proprietes_physiques_fluide_Ostwald(const Domaine_dis& z, Fluide_Ostwald& le_fluide, const Navier_Stokes_std& eqn_hydr, const Champ_Inc& ch_temper) const
{
  Cerr << "Discretisation VEF du fluide_Ostwald" << finl;
  const Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z.valeur());
  const Champ_Inc& ch_vitesse = eqn_hydr.inconnue();
  const Champ_P1NC& vit = ref_cast(Champ_P1NC, ch_vitesse.valeur());

  Champ_Don& mu = le_fluide.viscosite_dynamique();
  //  mu est toujours un champ_Ostwald_VEF , il faut toujours faire ce qui suit
  mu.typer("Champ_Ostwald_VEF");
  Champ_Ostwald_VEF& ch_mu = ref_cast(Champ_Ostwald_VEF, mu.valeur());
  Cerr << "associe domainedisbase VEF" << finl;
  ch_mu.associer_domaine_dis_base(domaine_vef);
  ch_mu.associer_fluide(le_fluide);
  ch_mu.associer_champ(vit);
  ch_mu.associer_eqn(eqn_hydr);
  Cerr << "associations finies domaine dis base, fluide, champ VEF" << finl;
  ch_mu.fixer_nb_comp(1);

  Cerr << "fait fixer_nb_valeurs_nodales" << finl;
  Cerr << "nb_valeurs_nodales VEF = " << domaine_vef.nb_elem() << finl;
  ch_mu.fixer_nb_valeurs_nodales(domaine_vef.nb_elem());

  Cerr << "fait changer_temps" << finl;
  ch_mu.changer_temps(vit.temps());
  Cerr << "mu VEF est discretise " << finl;
}

void VEF_discretisation::domaine_Cl_dis(Domaine_dis& z, Domaine_Cl_dis& zcl) const
{
  Cerr << "discretisation des conditions limites" << finl;
  assert(z.non_nul());
  Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, z.valeur());
  zcl.typer("Domaine_Cl_VEF");
  assert(zcl.non_nul());
  Domaine_Cl_VEF& domaine_cl_VEF = ref_cast(Domaine_Cl_VEF, zcl.valeur());
  domaine_cl_VEF.associer(domaine_VEF);
  Cerr << "discretisation des conditions limites OK" << finl;
}

void VEF_discretisation::critere_Q(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  // On passe la zcl, pour qu'il n y ait qu une methode qqsoit la dsicretisation
  // mais on ne s'en sert pas!!!
  Cerr << "Discretisation du critere Q " << finl;
  const Champ_P1NC& vit = ref_cast(Champ_P1NC, ch_vitesse.valeur());
  const Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z.valeur());
  ch.typer("Critere_Q_Champ_P1NC");
  Critere_Q_Champ_P1NC& ch_cQ = ref_cast(Critere_Q_Champ_P1NC, ch.valeur());
  ch_cQ.associer_domaine_dis_base(domaine_vef);
  ch_cQ.associer_champ(vit);
  ch_cQ.nommer("Critere_Q");
  ch_cQ.fixer_nb_comp(1);
  ch_cQ.fixer_nb_valeurs_nodales(domaine_vef.nb_elem());
  ch_cQ.fixer_unite("s-2");
  ch_cQ.changer_temps(ch_vitesse->temps());
}

void VEF_discretisation::y_plus(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de y_plus" << finl;
  const Champ_P1NC& vit = ref_cast(Champ_P1NC, ch_vitesse.valeur());
  const Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z.valeur());
  const Domaine_Cl_VEF& domaine_cl_vef = ref_cast(Domaine_Cl_VEF, zcl.valeur());
  ch.typer("Y_plus_Champ_P1NC");
  Y_plus_Champ_P1NC& ch_yp = ref_cast(Y_plus_Champ_P1NC, ch.valeur());
  ch_yp.associer_domaine_dis_base(domaine_vef);
  ch_yp.associer_domaine_Cl_dis_base(domaine_cl_vef);
  ch_yp.associer_champ(vit);
  ch_yp.nommer("Y_plus");
  ch_yp.fixer_nb_comp(1);
  ch_yp.fixer_nb_valeurs_nodales(domaine_vef.nb_elem());
  ch_yp.fixer_unite("adimensionnel");
  ch_yp.changer_temps(ch_vitesse->temps());
}

void VEF_discretisation::grad_u(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_vitesse, Champ_Fonc& ch) const
{

  const Champ_P1NC& vit = ref_cast(Champ_P1NC, ch_vitesse.valeur());
  const Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z.valeur());
  const int nb_comp = dimension * dimension;
  const Domaine_Cl_VEF& domaine_cl_vef = ref_cast(Domaine_Cl_VEF, zcl.valeur());
  ch.typer("grad_u_Champ_P1NC");
  grad_U_Champ_P1NC& ch_grad_u = ref_cast(grad_U_Champ_P1NC, ch.valeur());
  ch_grad_u.fixer_nature_du_champ(vectoriel);
  ch_grad_u.associer_domaine_dis_base(domaine_vef);
  ch_grad_u.associer_domaine_Cl_dis_base(domaine_cl_vef);
  ch_grad_u.associer_champ(vit);
  ch_grad_u.nommer("gradient_vitesse");
  ch_grad_u.fixer_nb_comp(nb_comp);
  ch_grad_u.fixer_nb_valeurs_nodales(domaine_vef.nb_elem());
  ch_grad_u.fixer_unite("s-1");
  ch_grad_u.changer_temps(ch_vitesse->temps());

  if (dimension == 2)
    {
      ch_grad_u.fixer_nom_compo(0, "dUdX"); // du/dx
      ch_grad_u.fixer_nom_compo(1, "dUdY"); // du/dy
      ch_grad_u.fixer_nom_compo(2, "dVdX"); // dv/dx
      ch_grad_u.fixer_nom_compo(3, "dVdY"); // dv/dy
    }
  else
    {
      ch_grad_u.fixer_nom_compo(0, "dUdX"); // du/dx
      ch_grad_u.fixer_nom_compo(1, "dUdY"); // du/dy
      ch_grad_u.fixer_nom_compo(2, "dUdZ"); // du/dz
      ch_grad_u.fixer_nom_compo(3, "dVdX"); // dv/dx
      ch_grad_u.fixer_nom_compo(4, "dVdY"); // dv/dy
      ch_grad_u.fixer_nom_compo(5, "dVdZ"); // dv/dz
      ch_grad_u.fixer_nom_compo(6, "dWdX"); // dw/dx
      ch_grad_u.fixer_nom_compo(7, "dWdY"); // dw/dy
      ch_grad_u.fixer_nom_compo(8, "dWdZ"); // dw/dz
    }

}

void VEF_discretisation::grad_T(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_temperature, Champ_Fonc& ch) const
{
  Cerr << "Discretisation de gradient_temperature" << finl;

  const Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z.valeur());
  creer_champ(ch, domaine_vef, "gradient_temperature_Champ_P1NC", "gradient_temperature", "K/m", dimension, domaine_vef.nb_elem(), ch_temperature->temps());

  grad_T_Champ_P1NC& ch_gt = ref_cast(grad_T_Champ_P1NC, ch.valeur());
  const Champ_P1NC& temp = ref_cast(Champ_P1NC, ch_temperature.valeur());
  const Domaine_Cl_VEF& domaine_cl_vef = ref_cast(Domaine_Cl_VEF, zcl.valeur());
  ch_gt.associer_domaine_Cl_dis_base(domaine_cl_vef);
  ch_gt.associer_champ(temp);
}

void VEF_discretisation::h_conv(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_temperature, Champ_Fonc& ch, Motcle& nom, int temp_ref) const
{
  Cerr << "Discretisation de h_conv" << finl;
  const Champ_P1NC& temp = ref_cast(Champ_P1NC, ch_temperature.valeur());
  const Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z.valeur());
  const Domaine_Cl_VEF& domaine_cl_vef = ref_cast(Domaine_Cl_VEF, zcl.valeur());
  ch.typer("h_conv_Champ_P1NC");
  h_conv_Champ_P1NC& ch_gt = ref_cast(h_conv_Champ_P1NC, ch.valeur());
  ch_gt.associer_domaine_dis_base(domaine_vef);
  ch_gt.associer_domaine_Cl_dis_base(domaine_cl_vef);
  ch_gt.associer_champ(temp);
  ch_gt.temp_ref() = temp_ref;
  ch_gt.nommer(nom);
  ch_gt.fixer_nb_comp(1);
  ch_gt.fixer_nb_valeurs_nodales(domaine_vef.nb_elem());
  ch_gt.fixer_unite("W/m2.K");
  ch_gt.changer_temps(ch_temperature->temps());
}

void VEF_discretisation::taux_cisaillement(const Domaine_dis& z, const Domaine_Cl_dis& zcl, const Champ_Inc& ch_vitesse, Champ_Fonc& champ) const
{
  const Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z.valeur());
  const Domaine_Cl_VEF& domaine_cl_vef = ref_cast(Domaine_Cl_VEF, zcl.valeur());
  champ.typer("Taux_cisaillement_P0_VEF");
  Taux_cisaillement_P0_VEF& ch = ref_cast(Taux_cisaillement_P0_VEF, champ.valeur());
  ch.associer_domaine_dis_base(domaine_vef);
  const Champ_P1NC& vit = ref_cast(Champ_P1NC, ch_vitesse.valeur());
  ch.associer_champ(vit, domaine_cl_vef);
  ch.nommer("Taux_cisaillement");
  ch.fixer_nb_comp(1);
  ch.fixer_nb_valeurs_nodales(domaine_vef.nb_elem());
  ch.fixer_unite("s-1");
  ch.changer_temps(ch_vitesse->temps());
}

void VEF_discretisation::modifier_champ_tabule(const Domaine_dis_base& domaine_dis, Champ_Fonc_Tabule& le_champ_tabule, const VECT(REF(Champ_base)) &ch_inc) const
{
  le_champ_tabule.le_champ_tabule_discretise().typer("Champ_Fonc_Tabule_P0_VEF");
  Champ_Fonc_Tabule_P0_VEF& le_champ_tabule_dis = ref_cast(Champ_Fonc_Tabule_P0_VEF, le_champ_tabule.le_champ_tabule_discretise().valeur());
  le_champ_tabule_dis.associer_domaine_dis_base(domaine_dis);
  le_champ_tabule_dis.associer_param(ch_inc, le_champ_tabule.table());
  le_champ_tabule_dis.nommer(le_champ_tabule.le_nom()); // We give a name to this field, help for debug
  le_champ_tabule_dis.fixer_nb_comp(le_champ_tabule.nb_comp());
  le_champ_tabule_dis.fixer_nb_valeurs_nodales(domaine_dis.nb_elem());
  le_champ_tabule_dis.changer_temps(ch_inc[0]->temps());
}

void VEF_discretisation::residu(const Domaine_dis& z, const Champ_Inc& ch_inco, Champ_Fonc& champ) const
{
  Nom ch_name(ch_inco->le_nom());
  ch_name += "_residu";
  Cerr << "Discretization of " << ch_name << finl;

  const Domaine_VEF& domaine_vef = ref_cast(Domaine_VEF, z.valeur());
  int nb_comp = ch_inco->valeurs().line_size();
  Discretisation_base::discretiser_champ("champ_face", domaine_vef, ch_name, "units_not_defined", nb_comp, ch_inco->temps(), champ);
  Champ_Fonc_base& ch_fonc = ref_cast(Champ_Fonc_base, champ.valeur());
  DoubleTab& tab = ch_fonc.valeurs();
  tab = -10000.0;
  Cerr << "[Information] Discretisation_base::residu : the residue is set to -10000.0 at initial time" << finl;
}

void VEF_discretisation::discretiser(REF(Domaine_dis) &dom) const
{
  Discretisation_base::discretiser(dom);
  Domaine_VEF& zvef = ref_cast(Domaine_VEF, dom->valeur());
  zvef.discretiser_suite(*this);
}
