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
// File:        Champ_Generique_Morceau_Equation.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////
#include <Champ_Generique_Morceau_Equation.h>
#include <Zone_VF.h>
#include <Champ_Generique_refChamp.h>
#include <Discretisation_base.h>
#include <Operateur_base.h>
#include <Operateur.h>
#include <Synonyme_info.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Champ_Generique_Morceau_Equation,"Morceau_Equation",Champ_Gen_de_Champs_Gen);
Add_synonym(Champ_Generique_Morceau_Equation,"Champ_Post_Morceau_Equation");

Champ_Generique_Morceau_Equation::Champ_Generique_Morceau_Equation()
{
  numero_morceau_ = -1;
  compo_=0;
}

Sortie& Champ_Generique_Morceau_Equation::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

//cf Champ_Gen_de_Champs_Gen::readOn
Entree& Champ_Generique_Morceau_Equation::readOn(Entree& s )
{
  Champ_Gen_de_Champs_Gen::readOn(s);
  return s ;
}

//  type          : type du morceau d equation (actuellement "operateur")
//  numero          : numero du morceau d equation
//                   (cas operateur : 0 (diffusion) 1 (convection) 2 (gradient) 3 (divergence))
//  option          : choix de la quantite a postraiter
//                     (actuellement "stabilite" pour dt_stab "flux_bords" pour flux_bords_)
//  compo            : numero de la composante a postraiter pour le cas des "flux_bords"
//                     (si plusieurs composantes)
void Champ_Generique_Morceau_Equation::set_param(Param& param)
{
  Champ_Gen_de_Champs_Gen::set_param(param);
  param.ajouter("type",&type_morceau_,Param::REQUIRED);
  param.ajouter("numero",&numero_morceau_);
  param.ajouter("option",&option_,Param::REQUIRED);
  param.ajouter("compo",&compo_);
}

//Description :
//-Initialisation de ref_eq_
//-Initialisation de localisation_
//-Appel de la methode de la classe mere
void Champ_Generique_Morceau_Equation::completer(const Postraitement_base& post)
{
  const Probleme_base& Pb = get_ref_pb_base();
  int numero_eq_=-1;
  if (sub_type(Champ_Generique_refChamp,get_source(0)))
    {

      Champ espace_stockage;
      const Champ_base& mon_champ = get_source(0).get_champ(espace_stockage);
      if (sub_type(Champ_Inc_base,mon_champ))
        {
          const Champ_Inc_base& mon_champ_inc = ref_cast(Champ_Inc_base,mon_champ);

          {
            //On recupere l equation alors qu elle n est pas encore associee au Champ_Inc
            //On parcours les equations du probleme et on identifie celle qui correspond au champ inconnu

            int nb_eq = Pb.nombre_d_equations();
            int i=0;

            while (i<nb_eq)
              {
                const Equation_base& eq_test = Pb.equation(i);
                if ((eq_test.inconnue().le_nom() == mon_champ_inc.le_nom()))
                  {
                    numero_eq_=i;
                    break;
                  }
                i++;
              }
          }
        }
    }

  if (numero_eq_==-1)
    {
      Cerr<<"We can apply a Champ_Generique_Morceau_Equation only to an unknown field of the problem"<<finl;
      exit();
    }
  ref_eq_ = Pb.equation(numero_eq_);

  localisation_ = morceau().get_localisation_pour_post(option_);

  //Appel de la methode de la classe mere fait apres pour que
  //la methode nommer_source() dispose de ref_eq_ initialise
  Champ_Gen_de_Champs_Gen::completer(post);
}

Champ_Fonc& Champ_Generique_Morceau_Equation::creer_espace_stockage(const Nature_du_champ& nature,
                                                                    const int nb_comp,
                                                                    Champ_Fonc& es_tmp) const
{
  Noms noms;
  Noms unites;
  for (int c=0; c<nb_comp; c++)
    {
      //  noms.add("bidon");
      unites.add("bidon");
    }
  noms.add("bidon");
  double temps;
  temps = get_time();
  const Discretisation_base&  discr = get_discretisation();
  Motcle directive;
  if ((Motcle(option_)=="flux_bords") || (Motcle(option_)=="flux_surfacique_bords"))
    directive = "champ_face"; // Pour avoir les flux_bords aux faces et non aux elements en VDF (ex: temperature)
  else
    directive = get_directive_pour_discr();
  const Zone_dis_base& zone_dis = get_ref_zone_dis_base();

  discr.discretiser_champ(directive,zone_dis,nature,noms,unites,nb_comp,temps,es_tmp);

  if (directive=="pression")
    {
      const  Zone_Cl_dis_base& zcl = get_ref_zcl_dis_base();
      es_tmp->completer(zcl);
    }

  return es_tmp;
}

// Description:
// le morceau d equation lance la discretisation du champ espace_stockage
// et remplit son tableau de valeurs par la methode calculer_pour_post(...)
// Rq : Ce procede differe de celui applique dans les autres Champ_Generique pour lesquels
// le remplissage du tableau de valeurs de espace_stockage n'est pas delegue
//
const Champ_base& Champ_Generique_Morceau_Equation::get_champ(Champ& espace_stockage) const
{
  Champ_Fonc es_tmp;
  Nature_du_champ nature;
  int nb_comp = -1;

  if ((Motcle(option_)=="stabilite") || (Motcle(option_)=="flux_bords") || (Motcle(option_)=="flux_surfacique_bords"))
    {
      nature = scalaire;
      nb_comp = 1;
    }
  else
    {
      Cerr << "Error. Contact TRUST support." << finl;
      Process::exit();
    }

  espace_stockage = creer_espace_stockage(nature,nb_comp,es_tmp);
  morceau().calculer_pour_post(espace_stockage,option_,compo_);
  DoubleTab& es_val = espace_stockage.valeurs();
  es_val.echange_espace_virtuel();
  return espace_stockage.valeur();
}

const Champ_base& Champ_Generique_Morceau_Equation::get_champ_without_evaluation(Champ& espace_stockage) const
{
  Champ_Fonc es_tmp;
  Nature_du_champ nature;
  int nb_comp = -1;

  if ((Motcle(option_)=="stabilite") || (Motcle(option_)=="flux_bords") || (Motcle(option_)=="flux_surfacique_bords"))
    {
      nature = scalaire;
      nb_comp = 1;
    }

  espace_stockage = creer_espace_stockage(nature,nb_comp,es_tmp);
  return espace_stockage.valeur();
}
const Noms Champ_Generique_Morceau_Equation::get_property(const Motcle& query) const
{

  Motcles motcles(2);
  motcles[0] = "composantes";
  motcles[1] = "unites";

  int rang = motcles.search(query);
  switch(rang)
    {
    case 0:
      {
        Noms compo(1);
        Nom nume((int)1);
        compo[0] = nom_post_+nume;

        return compo;
        break;
      }

    case 1:
      {
        Noms unites(1);
        if (Motcle(option_)=="stabilite")
          unites[0] = "s";
        else if (Motcle(option_)=="flux_bords" || Motcle(option_)=="flux_surfacique_bords")
          unites[0] = "bidon";

        return unites;
        break;
      }

    }
  return Champ_Gen_de_Champs_Gen::get_property(query);
}

Entity Champ_Generique_Morceau_Equation::get_localisation(const int index) const
{
  Entity loc;
  //Pour initialisation
  loc = NODE;

  if (localisation_=="elem")
    loc = ELEMENT;
  else if (localisation_=="face")
    loc = FACE;
  else if (localisation_=="som")
    loc = NODE;
  else
    {
      Cerr << "Error of type : localisation should be specified to elem or som or face for the defined field " << nom_post_ << finl;
      throw Champ_Generique_erreur("INVALID");
    }
  return loc;
}

//Description
//Nomme le champ en tant que source par defaut
//nom_eq + "_" + type_morceau + "_" + type_option
void Champ_Generique_Morceau_Equation::nommer_source()
{
  Motcle type_morceau, type_option;
  if (Motcle(type_morceau_)=="operateur")
    {
      if (numero_morceau_==0)
        type_morceau = "diffusion";
      else if (numero_morceau_==1)
        type_morceau = "convection";
      else if (numero_morceau_==2)
        type_morceau = "gradient";
      else if (numero_morceau_==3)
        type_morceau = "divergence";
      else
        {
          Cerr<<"This operateur number is not available."<<finl;
          exit();
        }
    }
  else
    {
      Cerr<<"Currently the only pieces of equations considered for the postprocessing"<<finl;
      Cerr<<"are of type operator"<<finl;
      exit();
    }

  if (Motcle(option_)=="stabilite")
    type_option = "dt";
  else if (Motcle(option_)=="flux_bords")
    {
      type_option = "flux_bords";
    }
  else if (Motcle(option_)=="flux_surfacique_bords")
    {
      type_option = "flux_surfacique_bords";
    }
  else
    {
      Cerr<<"Currently the only options considered for a piece of equation are \"stabilite\" and \"flux_bords\"."<<finl;
      exit();
    }


  if (nom_post_=="??")
    {
      Nom nom_post_source;
      const Nom& nom_eq = ref_eq_->le_nom();
      nom_post_source = nom_eq;
      nom_post_source += "_";
      nom_post_source +=  type_morceau;
      nom_post_source += "_";
      nom_post_source += type_option ;
      if ((Motcle(option_)=="flux_bords") || (Motcle(option_)=="flux_surfacique_bords"))
        {
          Nom nume(compo_);
          nom_post_source += nume;
        }
      nommer(nom_post_source);
    }

}

//Description
//Rend le morceau d equation considere pour ce Champ_Generique
//Actuellement seul type de morceau considere : les operateurs
//Pour considerer  d autres morceaux d equation il faudra tester type_morceau_
const MorEqn& Champ_Generique_Morceau_Equation::morceau() const
{
  int nb_operateurs = ref_eq_->nombre_d_operateurs();
  if (numero_morceau_ >= nb_operateurs)
    return ref_eq_->operateur_fonctionnel(numero_morceau_-nb_operateurs).l_op_base();

  return ref_eq_->operateur(numero_morceau_).l_op_base();
}

MorEqn& Champ_Generique_Morceau_Equation::morceau()
{
  int nb_operateurs = ref_eq_->nombre_d_operateurs();
  if (numero_morceau_ >= nb_operateurs)
    return ref_eq_->operateur_fonctionnel(numero_morceau_-nb_operateurs).l_op_base();

  return ref_eq_->operateur(numero_morceau_).l_op_base();
}

const Motcle Champ_Generique_Morceau_Equation::get_directive_pour_discr() const
{
  Motcle directive;

  if (localisation_=="elem")
    directive = "champ_elem";
  else if (localisation_=="face")
    directive = "champ_face";
  else if (localisation_=="som")
    directive = "champ_sommets";
  else
    {
      Cerr << "Error of type : localisation should be specified to elem or som or face for the defined field " << nom_post_ << finl;
      throw Champ_Generique_erreur("INVALID");
    }

  return directive;
}

