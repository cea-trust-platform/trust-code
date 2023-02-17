/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Discretisation_base.h>
#include <Domaine_dis_cache.h>
#include <Schema_Temps_base.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Interprete.h>
#include <Domaine_VF.h>

Implemente_base(Discretisation_base,"Discretisation_base",Objet_U);
Implemente_instanciable(Discretisation,"Discretisation",DERIV(Discretisation_base));

const Motcle Discretisation_base::DEMANDE_DESCRIPTION = Motcle("demande_description");
const Nom    Discretisation_base::NOM_VIDE = Nom("-");

Sortie& Discretisation_base::printOn(Sortie& os) const
{
  return os;
}

Entree& Discretisation_base::readOn(Entree& is)
{
  return is;
}

void Discretisation_base::associer_domaine(const Domaine& dom)
{
  le_domaine_ = dom;
}

// Construction d'objets en fontion de la discretisation :
// Champs,
// Matrices de pression,
// ...

// On passe une directive qui doit etre reconnue par le type
//  de discretisation. Les directives sont des mots cles :
//  la casse est ignoree et il ne doit pas y avoir d'espace.
// Exemples de directives :
//  "CHAMP_P0",
//  "CHAMP_FACES",
//  "VITESSE",
//  "PRESSION",
//  "TEMPERATURE",
// Parfois, le nombre de composantes depend de la discretisation
// (exemple pour la vitesse : 1 composante en VDF, 3 en VEF)
// Si on met nb_comp = -1, la discretisation choisit le nombre
// approprie, sinon elle utilise la valeur fournie.
void Discretisation_base::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, const Nom& nom, const Nom& unite, int nb_comp, int nb_pas_dt, double temps, Champ_Inc& champ,
                                            const Nom& sous_type) const

{
  Noms noms;
  Noms unites;
  noms.add(nom);
  unites.add(unite);

  discretiser_champ(directive, z, scalaire, noms, unites, nb_comp, nb_pas_dt, temps, champ, sous_type);
}

void Discretisation_base::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, const Nom& nom, const Nom& unite, int nb_comp, double temps, Champ_Fonc& champ) const
{
  Noms noms;
  Noms unites;
  noms.add(nom);
  unites.add(unite);

  discretiser_champ(directive, z, scalaire, noms, unites, nb_comp, temps, champ);
}
void Discretisation_base::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, const Nom& nom, const Nom& unite, int nb_comp, double temps, Champ_Don& champ) const
{
  Noms noms;
  Noms unites;
  noms.add(nom);
  unites.add(unite);

  discretiser_champ(directive, z, scalaire, noms, unites, nb_comp, temps, champ);
}

/*! @brief Cette fonction est un outil pour les trois methodes suivantes.
 *
 * ..
 *
 */
void Discretisation_base::test_demande_description(const Motcle& directive,
                                                   const Nom& type_objet) const
{
  if (directive == Discretisation_base::DEMANDE_DESCRIPTION)
    {
      Cerr << "Discr_base : none directive understood" << finl;
      // On a fait le tour, les descendants on ecrit toutes les directives
      // comprises, on s'arrete.
      assert(0);
      exit();
    }
  // Si on arrive ici, c'est qu'aucun descendant de la methode n'a compris
  // la directive, on provoque l'affichage des directives comprises :
  Cerr << "\nError in Discr_base::discretiser_(..., ";
  Cerr << type_objet << ")\n";
  Cerr << " The discretization " << que_suis_je();
  Cerr << " does not understood the following directive :\n  " << directive;
  Cerr << "\n The understood directives are :\n";
}

/*! @brief Discretisation d'un champ en fonction de la directive et des autres parametres.
 *
 * Voir les classes derivees. Cette methode
 *  ne traite aucune directive, elle affiche la liste des directives comprises
 *  par les classes derivees.
 *  Voir par exemple VDF_discretisation.cpp et VEF...
 *
 */
void Discretisation_base::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, int nb_pas_dt, double temps,
                                            Champ_Inc& champ, const Nom& sous_type) const
{
  test_demande_description(directive, champ.que_suis_je());
  // Appel recursif pour produire l'affichage des directives :
  if (directive == DEMANDE_DESCRIPTION)
    {
      exit();
      throw;
    }
  discretiser_champ(DEMANDE_DESCRIPTION, z, nature, noms, unites, nb_comp, nb_pas_dt, temps, champ, sous_type);
}

/*! @brief idem
 *
 */
void Discretisation_base::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, double temps,
                                            Champ_Fonc& champ) const
{
  test_demande_description(directive, champ.que_suis_je());
  // Appel recursif pour produire l'affichage des directives :
  if (directive == DEMANDE_DESCRIPTION)
    {
      exit();
      throw;
    }
  discretiser_champ(DEMANDE_DESCRIPTION, z, nature, noms, unites, nb_comp, temps, champ);
}

/*! @brief idem
 *
 */
void Discretisation_base::discretiser_champ(const Motcle& directive, const Domaine_dis_base& z, Nature_du_champ nature, const Noms& noms, const Noms& unites, int nb_comp, double temps,
                                            Champ_Don& champ) const
{
  test_demande_description(directive, champ.que_suis_je());
  // Appel recursif pour produire l'affichage des directives :
  if (directive == DEMANDE_DESCRIPTION)
    {
      exit();
      throw;
    }
  discretiser_champ(DEMANDE_DESCRIPTION, z, nature, noms, unites, nb_comp, temps, champ);
}

void Discretisation_base::discretiser_variables() const
{
  Cerr << "Discretisation_base::discretiser_variables() does nothing" << finl;
  Cerr << "and must be overloaded " << finl;
  exit();
}

void Discretisation_base::discretiser_Domaine_Cl_dis(const Domaine_dis& ,Domaine_Cl_dis& ) const
{
  Cerr << "Discretisation_base::discretiser_Domaine_Cl_dis does nothing" << finl;
  Cerr << "and must be overloaded " << finl;
  exit();
}

/*! @brief Fonction outil pour fixer les membres communs a tous les types de champs (utilisee dans creer_champ)
 *
 */
void Discretisation_base::champ_fixer_membres_communs(
  Champ_base& ch,
  const Domaine_dis_base& z,
  const Nom& type,
  const Nom& nom,
  const Nom& unite,
  int nb_comp,
  int nb_ddl,
  double temps)
{
  ch.associer_domaine_dis_base(z);
  ch.nommer(nom);
  ch.fixer_nb_comp(nb_comp);
  if ((nb_comp>1)&& (nb_comp==dimension))
    {
      Noms noms(nb_comp);
      if (!axi)
        {
          noms[0]=nom+"X";
          noms[1]=nom+"Y";
          if (nb_comp>2)
            noms[2]=nom+"Z";
        }
      else
        {
          noms[0]=nom+"R";
          noms[1]=nom+"teta";
          if (nb_comp>2)
            noms[2]=nom+"Z";
        }
      ch.fixer_noms_compo(noms);
    }
  ch.fixer_unite(unite);
  ch.fixer_nb_valeurs_nodales(nb_ddl);
  ch.changer_temps(temps);
}

/*! @brief Methode statique qui cree un Champ_Inc du type specifie.
 *
 * Les parametres "directive" et "nom_discretisation" sont
 *  utilises pour l'affichage uniquement et sont optionnels
 *
 */
void Discretisation_base::creer_champ(Champ_Inc& ch, const Domaine_dis_base& z, const Nom& type, const Nom& nom, const Nom& unite, int nb_comp, int nb_ddl, int nb_pas_dt, double temps,
                                      const Nom& directive, const Nom& nom_discretisation)
{
  Nom nomd = nom_discretisation; // Pour contourner le probleme du "static" dans type_info::nom()
  ch.typer(type);
  Champ_Inc_base& chb = ch.valeur();
  chb.fixer_nb_valeurs_temporelles(nb_pas_dt);
  champ_fixer_membres_communs(chb, z, type, nom, unite, nb_comp, nb_ddl, temps);
}

/*! @brief Methode statique qui cree un Champ_Fonc du type specifie.
 *
 * Les parametres "directive" et "nom_discretisation" sont
 *  utilises pour l'affichage uniquement et sont optionnels
 *
 */
void Discretisation_base::creer_champ(Champ_Fonc& ch, const Domaine_dis_base& z, const Nom& type, const Nom& nom, const Nom& unite, int nb_comp, int nb_ddl, double temps, const Nom& directive,
                                      const Nom& nom_discretisation)
{
  Nom nomd = nom_discretisation; // Pour contourner le probleme du "static" dans type_info::nom()
  ch.typer(type);
  Champ_Fonc_base& chb = ch.valeur();
  champ_fixer_membres_communs(chb, z, type, nom, unite, nb_comp, nb_ddl, temps);
}

/*! @brief Methode statique qui cree un Champ_Don du type specifie.
 *
 * Les parametres "directive" et "nom_discretisation" sont
 *  utilises pour l'affichage uniquement et sont optionnels
 *
 */
void Discretisation_base::creer_champ(Champ_Don& ch, const Domaine_dis_base& z, const Nom& type, const Nom& nom, const Nom& unite, int nb_comp, int nb_ddl, double temps, const Nom& directive,
                                      const Nom& nom_discretisation)
{
  Nom nomd = nom_discretisation; // Pour contourner le probleme du "static" dans type_info::nom()
  ch.typer(type);
  Champ_Don_base& chb = ch.valeur();
  champ_fixer_membres_communs(chb, z, type, nom, unite, nb_comp, nb_ddl, temps);
}

Entree& Discretisation::readOn(Entree& is)
{
  return DERIV(Discretisation_base)::readOn(is);
}

Sortie& Discretisation::printOn(Sortie& os) const
{
  return DERIV(Discretisation_base)::printOn(os);
}

void Discretisation_base::discretiser(Domaine_dis& dom_dis) const
{
  Nom type="Domaine_";
  type+=que_suis_je();
  const Domaine& dom = le_domaine_.valeur();
  dom_dis = Domaine_dis_cache::Build_or_get(type, dom);
}

void Discretisation_base::volume_maille(const Schema_Temps_base& sch,

                                        const Domaine_dis& z,
                                        Champ_Fonc& ch) const
{
  Cerr << "Discretization of the field 'volume of meshes'" << finl;
  const Domaine_VF& domaine_VF=ref_cast(Domaine_VF, z.valeur());
  discretiser_champ("champ_elem",domaine_VF,"volume_maille","m3",1,sch.temps_courant(),ch);
  Champ_Fonc_base& ch_fonc = ref_cast(Champ_Fonc_base,ch.valeur());
  DoubleVect& tab=ch_fonc.valeurs();
  tab = domaine_VF.volumes();
}

void Discretisation_base::residu(const Domaine_dis& , const Champ_Inc&, Champ_Fonc& ) const
{
  Cerr << "\nDiscret_Thyd::residu() does nothing" << finl;
  Cerr <<  que_suis_je() << " needs to overload it !" << finl;
  exit();
}

void Discretisation_base::modifier_champ_tabule(const Domaine_dis_base& domaine_dis,Champ_Fonc_Tabule& ch_tab,const VECT(REF(Champ_base))& ch_inc) const
{
  Cerr<<que_suis_je()<<" must overload Discretisation_base::modifier_champ_tabule"<<finl;
  Cerr<<__FILE__<<(int)__LINE__<<" uncoded"<<finl;
  Process::exit();

}

void Discretisation_base::nommer_completer_champ_physique(const Domaine_dis_base& domaine_dis, const Nom& nom_champ, const Nom& unite, Champ_base& le_champ,
                                                          const Probleme_base& pb) const
{
  // on nomme le champ et l'unite
  le_champ.nommer(nom_champ);
  le_champ.fixer_unite(unite);
  if (sub_type(Champ_Fonc_Tabule,le_champ))
    {
      Noms& noms_variables = ref_cast(Champ_Fonc_Tabule, le_champ).noms_champs_parametre();
      Noms& noms_pbs = ref_cast(Champ_Fonc_Tabule, le_champ).noms_problemes();
      VECT(REF(Champ_base)) les_ch_eq;
      for (int i = 0; i < noms_variables.size(); i++)
        {
          REF(Champ_base) champ;
          const Probleme_base& pb_ch = noms_pbs.size() == 0 ? pb : ref_cast(Probleme_base, Interprete::objet(noms_pbs[i]));
          champ = pb_ch.get_champ(Motcle(noms_variables[i]));
          les_ch_eq.add(champ);
        }
      modifier_champ_tabule(domaine_dis,ref_cast(Champ_Fonc_Tabule,le_champ), les_ch_eq);
    }
}

/*! @brief remplit le Nom type en focntion de la classe de operateur, du type de l'operateur et de l'equation
 *
 * @param (class_operateur) Ce nom correspond au type de la classe de base de l'objet que l'on veut construire exemples: "source","Op_conv","Op_diff","Op_div","Op_grad","solveur_masse" Exemple get_name_of_type_for("Op_conv","amont",eqn,type); get_name_of_type_for("Op_diff"," ",eqn,type,champ_diffusisivite);
 */


// cela permet de specifier le comportement pour chaque discretisation
Nom Discretisation_base::get_name_of_type_for(const Nom& class_operateur, const Nom& type_operateur,const Equation_base& eqn ,const REF(Champ_base)& champ_sup) const
{
  Nom type;
  if (class_operateur == "Source")
    {
      type = type_operateur;
      Nom disc = eqn.discretisation().que_suis_je();

      int isQC = eqn.probleme().is_dilatable();

      if ((isQC) && ((eqn.que_suis_je() != "Transport_K_Eps") && (eqn.que_suis_je() != "Transport_K_Eps_Bas_Reynolds") &&
                     (eqn.que_suis_je() != "Transport_K_Eps_Realisable") && (eqn.que_suis_je() != "Transport_K_Eps_V2")))
        type += "_QC";

      // sauf pour le term boussinesq
      if (disc == "VEFPreP1B")
        {
          if ((Motcle(type_operateur) == "boussinesq_temperature") || (Motcle(type_operateur) == "boussinesq_concentration") || (Motcle(type_operateur) == "boussinesq"))
            disc = "VEFPreP1B";
          else
            disc = "VEF";
        }

      Nom type_ch = (eqn.inconnue()->que_suis_je());
      if (type_ch == "Champ_Q1NC") type_ch = "Champ_P1NC";

      type_ch.suffix("Champ_");
      type += "_";
      type += disc;
      type += "_";
      type += type_ch;
      return type;
    }
  else if (class_operateur == "Solveur_Masse")
    {
      type = "Masse_";

      Nom discr = eqn.discretisation().que_suis_je();
      // le Solveur est commun aux discretisations VEF et VEFP1B
      if (discr == "VEFPreP1B") discr = "VEF";

      type += discr;

      Nom type_ch = eqn.inconnue()->que_suis_je();
      if (type_ch == "Champ_Q1NC")  type_ch = "Champ_P1NC";
      if (type_ch.debute_par("Champ_P0_VDF")) type_ch = "Champ_P0_VDF";
      if (type_ch.debute_par("Champ_Face")) type_ch = "Champ_Face";

      type_ch.suffix("Champ");
      type += type_ch;
      return type;
    }
  else if (class_operateur == "Operateur_Grad")
    {
      type = "Op_Grad_";
      Nom type_pb = eqn.probleme().que_suis_je();
      if (type_pb == "Probleme_SG")
        {
          type += (type_pb.suffix("Probleme_"));
          type += "_";
        }

      Nom discr = eqn.discretisation().que_suis_je();

      type += discr;
      type += "_";
      Nom type_inco = eqn.inconnue()->que_suis_je();

      if (type_inco == "Champ_Q1NC") type_inco = "Champ_P1NC";

      type += (type_inco.suffix("Champ_"));

      //Test pour appliquer un gradient a un Champ_P1NC a une composante en VEF
      //Typage a revoir (revison des operateurs)
      if ((eqn.inconnue()->le_nom() != "vitesse") && (eqn.inconnue()->que_suis_je() == "Champ_P1NC"))
        type = "Op_Grad_P1NC_to_P0";

      //Test pour appliquer un gradient a un Champ_P0 a une composante en VDF
      //Typage a revoir (revison des operateurs)
      if ((eqn.inconnue()->le_nom() != "vitesse") && (eqn.inconnue()->que_suis_je() == "Champ_P0_VDF"))
        type = "Op_Grad_P0_to_Face";

      return type;
    }
  else if (class_operateur=="Operateur_Div")
    {
      type = "Op_Div_";

      Nom discr = eqn.discretisation().que_suis_je();
      Nom type_inco = eqn.inconnue()->que_suis_je();
      type += discr;

      type += "_";
      if (type_inco == "Champ_Q1NC") type_inco = "Champ_P1NC";

      type += (type_inco.suffix("Champ_"));
      return type;
    }
  else if (class_operateur=="Operateur_Diff")
    {
      Nom typ(type_operateur);
      if (typ == "standard") typ = "";

      Cerr << "We treat the diffusive operator of : " << eqn.que_suis_je() << finl;
      type = "Op_Diff_";
      //const Discretisation_base& discr=eqn.discretisation();
      Nom nom_discr = que_suis_je();
      Cerr << "The discretization used is : " << nom_discr << finl;
      assert(champ_sup.non_nul());
      const Champ_base& diffusivite = champ_sup.valeur();
      // Typage en fonction de la discretisation

      // les operateurs de diffusion sont communs aux discretisations VEF et VEFP1B
      if (nom_discr == "VEFPreP1B")
        nom_discr = "VEF";
      type += nom_discr;
      type += typ;

      // MODIF ELI LAUCOIN (10/12/2007) :
      // Je conserve le comportement normal pour le VEF et le VDF
      if ((nom_discr == "VDF") || (nom_discr == "VEF"))
        {
          Nom nb_inc;
          // Modif Elie Saikali (Nov 2020)
          if (eqn.inconnue()->que_suis_je() == "Champ_Face" || (diffusivite.nb_comp() == 1 && nom_discr == "VDF"))
            nb_inc = "_";
          else if (diffusivite.nb_comp() > 1 && diffusivite.le_nom() == "conductivite")
            nb_inc = "ANISOTROPE_";
          else
            {
              if (nom_discr == "VEF") nb_inc = "_";
              else nb_inc = "_Multi_inco_";
            }

          type += nb_inc;

          Nom type_diff;
          if (nom_discr == "VDF") type_diff = ""; /* pas de const/var en VDF */
          else
            {
              if (sub_type(Champ_Uniforme, diffusivite)) type_diff = "const_";
              else type_diff = "var_";
            }

          type += type_diff;
        }
      else
        {
          // par contre, je modifie le cas general pour eviter de rajouter des cas particuliers
          type += "_";
        }

      Nom type_inco = eqn.inconnue()->que_suis_je();

      type += (type_inco.suffix("Champ_"));
      if (axi == 1) type += "_Axi";

      return type;
    }
  else if (class_operateur == "Operateur_Conv")
    {
      if (Motcle(type_operateur) == Motcle("ALE"))
        {
          type = "Op_Conv_";
          type += type_operateur;
          Nom tiret = "_";
          type += tiret;
          Nom discr = que_suis_je();

          if (discr == "VEFPreP1B") discr = "VEF";
          type += discr;
          return type;
        }
      else
        {
          type = "Op_Conv_";
          type += type_operateur;
          Nom tiret = "_";
          type += tiret;
          Nom discr = que_suis_je();

          // les operateurs de diffusion sont communs aux discretisations VEF et VEFP1B
          if (discr == "VEFPreP1B") discr = "VEF";

          type += discr;
          if (type_operateur != "KEps_Comp")
            {
              type += tiret;
              Nom type_inco = eqn.inconnue()->que_suis_je();
              if (type_inco == "Champ_Q1NC") type_inco = "Champ_P1NC";
              if (type_inco.debute_par("Champ_P0_VDF")) type_inco = "Champ_P0_VDF";
              if (type_inco.debute_par("Champ_Face")) type_inco = "Champ_Face";

              type += (type_inco.suffix("Champ_"));

              if (axi == 1)
                if (type_operateur == "quick") type += "_Axi";
            }
          return type;
        }
    }
  else if (class_operateur == "Operateur_Evanescence")
    {
      Nom type_inco = eqn.inconnue()->que_suis_je();
      if (type_inco == "Champ_Q1NC") type_inco = "Champ_P1NC";
      if (type_inco.debute_par("Champ_P0_VDF")) type_inco = "Champ_P0_VDF";
      if (type_inco.debute_par("Champ_Face")) type_inco = "Champ_Face";
      type_inco.suffix("Champ");
      type = Nom("Op_Evanescence") + (type_operateur != "" ? "_" : "") + type_operateur + "_" + que_suis_je() + type_inco;
    }
  else
    {
      Cerr << class_operateur << " not understood in get_name_of_type_for of  " << que_suis_je() << finl;
      Cerr << __FILE__ << " " << (int) __LINE__ << finl;
      Process::exit();
    }
  return type;
}
/*! @brief donne l'extension a utiliser pour la discretisation en fonction de class_oprateur par defaut renvoie type=que_suis_je()
 *
 *  Mais par exemple on renverra souvent VEF au lieu de VEFPreP1b (champ_post_interpolation)
 *
 */
/*
const Nom& Discretisation_base::get_name_of_discretisation_for(const Nom& class_operateur,Nom& type) const
{
  type=que_suis_je();
  return type;
}

*/

int Discretisation_base::verifie_sous_type(Nom& type, const Nom& sous_type, const Motcle& directive) const
{
  const Type_info * base_info = Type_info::type_info_from_name(sous_type);

  if (base_info)
    {
      if (base_info->has_base(type))
        {
          type = sous_type;
          return 0;
        }

      // Elie Saikali : je fais mieux le jour ou on fait du vrai C++ pas comme ca ... FAUT TESTER SI LE CAST MARCHE ET PAS NOMMMMM !!!!!!!!!!!!!!!!!!
      if (sous_type.debute_par("Champ_Face_dep_expr") && type == "Champ_Face_VDF")
        {
          type = sous_type;
          return 0;
        }

      Cerr << "Error in " << que_suis_je() << " ::discretiser_champ" << finl;
      Cerr << sous_type << " is not a sub type of " << type << " for " << que_suis_je() << " discretization";
      Cerr << "( directive : \""<< directive << "\")"<<finl;
      Process::exit();
    }
  else
    {
      Cerr << "Error in " << que_suis_je() << "::discretiser_champ" << finl;
      Cerr << "Unknown class type " << sous_type << finl;
      Process::exit();
    }
  return -1;
}
