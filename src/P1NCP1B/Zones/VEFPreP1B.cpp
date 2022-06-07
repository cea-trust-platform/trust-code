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

#include <VEFPreP1B.h>
#include <Domaine_dis.h>
#include <Zone_Cl_VEFP1B.h>
#include <Champ_P1_isoP1Bulle.h>
#include <Schema_Temps_base.h>
#include <Champ_Inc.h>
#include <Zone_Cl_dis.h>
#include <Motcle.h>
#include <Champ_Don.h>
#include <Champ_Fonc.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(VEFPreP1B,"VEFPreP1B",VEF_discretisation);
// XD vefprep1b vef vefprep1b -1 Finite element volume discretization (P1NC/P1-bubble element). Since the 1.5.5 version, several new discretizations are available thanks to the optional keyword Read. By default, the VEFPreP1B keyword is equivalent to the former VEFPreP1B formulation (v1.5.4 and sooner). P0P1 (if used with the strong formulation for imposed pressure boundary) is equivalent to VEFPreP1B but the convergence is slower. VEFPreP1B dis is equivalent to VEFPreP1B dis Read dis { P0 P1 Changement_de_base_P1Bulle 1 Cl_pression_sommet_faible 0 }

VEFPreP1B::VEFPreP1B() : P1Bulle_(1), alphaE_(1), alphaS_(1), alphaA_(0), modif_div_face_dirichlet_(0), cl_pression_sommet_faible_(0)
{
  // Par defaut, P0+P1 et changement de base pour faire P0+P1->P1Bulle
}
// printOn et readOn

Sortie& VEFPreP1B::printOn(Sortie& s) const
{
  s<<"lire "<<le_nom()<<" { " <<finl;
  if (alphaE_) s<<" P0";
  if (alphaS_) s<<" P1";
  if (alphaA_) s<<" Pa";
  s<<" changement_de_base_P1bulle "<<P1Bulle_;
  s<<" modif_div_face_dirichlet "<<modif_div_face_dirichlet_;
  s<<" cl_pression_sommet_faible " << cl_pression_sommet_faible_;
  s<<" } "<<finl;
  return s;
}

Entree& VEFPreP1B::readOn(Entree& is )
{
  alphaE_ = alphaS_ = alphaA_ = P1Bulle_ = 0;
  cl_pression_sommet_faible_ = 1;
  modif_div_face_dirichlet_=0;

  Param param(que_suis_je());
  param.ajouter("changement_de_base_P1bulle", &P1Bulle_); // XD_ADD_P int (into=[0,1]) changement_de_base_p1bulle 1 This option may be used to have the P1NC/P0P1 formulation (value set to 0) or the P1NC/P1Bulle formulation (value set to 1, the default).
  param.ajouter_flag("P0", &alphaE_); // XD_ADD_P rien Pressure nodes are added on element centres
  param.ajouter_flag("P1", &alphaS_); // XD_ADD_P rien Pressure nodes are added on vertices
  param.ajouter_flag("Pa", &alphaA_); // XD_ADD_P rien Only available in 3D, pressure nodes are added on bones
  param.ajouter("modif_div_face_dirichlet",&modif_div_face_dirichlet_); // XD_ADD_P  int (into=[0,1]) This option (by default 0) is used to extend control volumes for the momentum equation.
  param.ajouter("CL_pression_sommet_faible", &cl_pression_sommet_faible_); // XD_ADD_P  int (into=[0,1]) This option is used to specify a strong formulation (value set to 0, the default) or a weak formulation (value set to 1) for an imposed pressure boundary condition. The first formulation converges quicker and is stable in general cases. The second formulation should be used if there are several outlet boundaries with Neumann condition (see Ecoulement_Neumann test case for example).
  param.lire_avec_accolades(is);

  // Quelques verifications
  if (dimension!=3 && alphaA_)
    {
      Cerr << "Le support Pa n'est disponible qu'en 3D." << finl;
      Process::exit();
    }
  if (alphaE_+alphaS_+alphaA_==0)
    {
      Cerr << "Il faut choisir au moins un support parmi P0, P1, Pa." << finl;
      Process::exit();
    }
  if (alphaA_==1 && !alphaE_)
    {
      Cerr << "Les discretisations Pa ou P1+Pa ne sont pas encore supportees." << finl;
      Process::exit();
    }
  if (P1Bulle_)
    if (!( (alphaE_==1) && (alphaS_==1) && (alphaA_==0)))
      {
        Cerr << "L'option changement_de_base_P1bulle n'est disponible qu'en P0/P1" << finl;
        Process::exit();
      }
  return is;
}

// Description:
// Discretisation d'un champ pour le VEFP1B en fonction d'une directive
// de discretisation. La directive est un Motcle comme "pression",
// "divergence_vitesse" (cree un champ de type P1_isoP1Bulle).
// Cette methode determine le type du champ a creer en fonction du type d'element
// et de la directive de discretisation. Elle determine ensuite le nombre de ddl
// et fixe l'ensemble des parametres du champ (type, nb_compo, nb_ddl, nb_pas_dt,
// nom(s), unite(s), nature du champ et attribue un temps) et associe la Zone_dis au champ.
// Voir le code pour avoir la correspondance entre les directives et
// le type de champ cree.
void VEFPreP1B::discretiser_champ(
  const Motcle& directive, const Zone_dis_base& z,
  Nature_du_champ nature,
  const Noms& noms, const Noms& unites,
  int nb_comp, int nb_pas_dt, double temps,
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

  if (directive == demande_description)
    Cerr << "VEFPreP1B : " << motcles;

  // Si on n'a pas compris la directive (ou si c'est une demande_description)
  // alors on appelle l'ancetre :
  if (rang < 0)
    {
      VEF_discretisation::discretiser_champ(directive, z,
                                            nature, noms, unites,
                                            nb_comp, nb_pas_dt, temps, champ);
      return;
    }

  const int nb_ddl = -1; // c'est le descripteur p1b qui donnera le chiffre
  if (nb_comp < 0)
    nb_comp = default_nb_comp;
  assert(nb_comp > 0);
  creer_champ(champ, z, type, noms[0], unites[0], nb_comp, nb_ddl, nb_pas_dt, temps,
              directive, que_suis_je());

  if (nature == multi_scalaire)
    {
      Cerr<<"There is no field of type Champ_Inc with P1Bulle discretization"<<finl;
      Cerr<<"and a multi_scalaire nature."<<finl;
      exit();
    }

}

// Description:
// Idem que VEFPreP1B::discretiser_champ(... , Champ_Inc)
void VEFPreP1B::discretiser_champ(
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
// Idem que VEFPreP1B::discretiser_champ(... , Champ_Inc)
void VEFPreP1B::discretiser_champ(const Motcle& directive, const Zone_dis_base& z,
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
void VEFPreP1B::discretiser_champ_fonc_don(
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

  if (directive == demande_description)
    Cerr << "VEFPreP1B : " << motcles;

  // Si on n'a pas compris la directive (ou si c'est une demande_description)
  // alors on appelle l'ancetre :
  if (rang < 0)
    {
      if (champ_fonc)
        VEF_discretisation::discretiser_champ(directive, z, nature, noms, unites,
                                              nb_comp, temps, *champ_fonc);
      else
        VEF_discretisation::discretiser_champ(directive, z, nature, noms, unites,
                                              nb_comp, temps, *champ_don);
      return;
    }

  int nb_ddl = -1; // c'est le descripteur p1b qui donnera le chiffre

  if (nb_comp < 0)
    nb_comp = default_nb_comp;
  assert(nb_comp > 0);
  if (champ_fonc)
    creer_champ(*champ_fonc, z, type, noms[0], unites[0], nb_comp, nb_ddl, temps,
                directive, que_suis_je());
  else
    creer_champ(*champ_don, z, type, noms[0], unites[0], nb_comp, nb_ddl, temps,
                directive, que_suis_je());

  if (nature == multi_scalaire)
    {
      Cerr<<"There is no field of type Champ_Fonc or Champ_Don with P1Bulle discretization"<<finl;
      Cerr<<"and a multi_scalaire nature."<<finl;
      exit();
    }
}

void VEFPreP1B::zone_Cl_dis(Zone_dis& z,
                            Zone_Cl_dis& zcl) const
{
  Cerr << "discretisation des conditions limites" << finl;
  assert(z.non_nul());
  Zone_VEF& zone_VEF=ref_cast(Zone_VEF, z.valeur());
  zcl.typer("Zone_Cl_VEFP1B");
  assert(zcl.non_nul());
  Zone_Cl_VEFP1B& zone_Cl_VEF=ref_cast(Zone_Cl_VEFP1B, zcl.valeur());
  zone_Cl_VEF.associer(zone_VEF);
  Cerr << "discretisation des conditions limites OK" << finl;
}

void VEFPreP1B::discretiser(Domaine_dis& dom) const
{
  Discretisation_base::discretiser(dom);
  Zone_VEF_PreP1b& zvef = ref_cast(Zone_VEF_PreP1b, dom.zone_dis(0).valeur());
  zvef.discretiser_suite(*this);
}
