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
// File:        Champ_Generique_Gradient.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_Gradient.h>
#include <Zone_VF.h>
#include <Champ_Generique_refChamp.h>
#include <Discretisation_base.h>
#include <Synonyme_info.h>

Implemente_instanciable(Champ_Generique_Gradient,"Gradient",Champ_Generique_Operateur_base);
Add_synonym(Champ_Generique_Gradient,"Champ_Post_Operateur_Gradient");

Sortie& Champ_Generique_Gradient::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Champ_Generique_Gradient::readOn(Entree& s )
{
  Champ_Generique_Operateur_base::readOn(s);
  return s ;
}

void Champ_Generique_Gradient::completer(const Postraitement_base& post)
{
  Champ_Gen_de_Champs_Gen::completer(post);
  const Probleme_base& Pb = get_ref_pb_base();
  if (sub_type(Champ_Generique_refChamp,get_source(0)))
    {

      Champ espace_stockage;
      const Champ_base& mon_champ = get_source(0).get_champ(espace_stockage);
      if (sub_type(Champ_Inc_base,mon_champ))
        {
          const Champ_Inc_base& mon_champ_inc = ref_cast(Champ_Inc_base,mon_champ);

          if ( (mon_champ_inc.le_nom()=="pression") || ((mon_champ_inc.le_nom()!="vitesse") && (mon_champ_inc.que_suis_je()=="Champ_P1NC"))
               || ((mon_champ_inc.le_nom()!="vitesse") && (mon_champ_inc.que_suis_je()=="Champ_P0_VDF")) )
            {

              //On recupere l equation alors qu elle n est pas encore associee au Champ_Inc
              //On parcours les equatiosn du probleme et on identifie celle qui correspond au champ inconnu

              int nb_eq = Pb.nombre_d_equations();
              int i=0;

              while (i<nb_eq)
                {
                  const Equation_base& eq_test = Pb.equation(i);
                  if ((eq_test.inconnue().le_nom() == mon_champ_inc.le_nom()) || ((i==0) && (mon_champ_inc.le_nom()=="pression") &&
                                                                                  (eq_test.le_nom()!="Conduction")))
                    break;
                  i++;
                }
              const Equation_base& eqn = Pb.equation(i);

              Op_Grad_.associer_eqn(eqn);
              Op_Grad_.typer();
              Op_Grad_.l_op_base().associer_eqn(eqn);

              const Zone_dis& zdis = eqn.zone_dis();
              const Zone_Cl_dis& zcl = eqn.zone_Cl_dis();
              const Champ_Inc& inco = eqn.inconnue();
              Op_Grad_->associer(zdis, zcl, inco);
            }
          else
            {
              Cerr<<"We can apply a Champ_Generique_Gradient only to the pressure field"<<finl;
              Cerr<<"or to a field of type Champ_P1NC or Champ_P0_VDF at one component"<<finl;
              exit();
            }
        }
      else
        {
          Cerr<<"We can apply a Champ_Generique_Gradient only to an unknown field of the problem"<<finl;
          exit();
        }
    }
}

const Champ_base& Champ_Generique_Gradient::get_champ(Champ& espace_stockage) const
{
  Champ source_espace_stockage;
  const Champ_base& source = get_source(0).get_champ(source_espace_stockage);

  if (Op_Grad_.non_nul())
    {
      Champ_Fonc es_tmp;
      espace_stockage = creer_espace_stockage(vectoriel,dimension,es_tmp);

      Op_Grad_.calculer(source.valeurs(),espace_stockage.valeurs());

      Nom type_op = Op_Grad_.valeur().que_suis_je();
      if ((type_op!="Op_Grad_P1NC_to_P0") && (type_op!="Op_Grad_P0_to_Face"))
        Op_Grad_.equation().solv_masse().appliquer(espace_stockage.valeurs());
    }
  else
    {
      const Noms nom = get_source(0).get_property("nom");
      Cerr<<"We can apply a Champ_Generique_Gradient only to the pressure field"<<finl;
      Cerr<<"or to a field of type Champ_P1NC or Champ_P0_VDF at one component"<<finl;
      Cerr<<"The space of storage of field treated here "<<nom[0]<<" is of type"<<source.que_suis_je()<<finl;
      exit();
    }

  DoubleTab& espace_valeurs = espace_stockage->valeurs();
  espace_valeurs.echange_espace_virtuel();
  return espace_stockage.valeur();
}

const Noms Champ_Generique_Gradient::get_property(const Motcle& query) const
{

  //Creation des composantes serait a faire de maniere dynamique (Gradient_...)

  Motcles motcles(2);
  motcles[0] = "composantes";
  motcles[1] = "unites";

  int rang = motcles.search(query);
  switch(rang)
    {
    case 0:
      {
        //Actuellement choix fait d ajouter simplement numero de composante
        /*
          const Noms compo_cibles = get_source(0).get_property("composantes");
          ///int size = compo_cibles.size();
          ///const Noms nom = get_source(0).get_property("nom");
          ///Nom nom_champ = nom[0];
          Nom nom_champ = "pression";

          //Valable si Champ_Fonc_P1NC
          Noms mots(dimension);
          Nom compo_temp="gradient_";

          for (int i=0; i<dimension; i++) {
          if (i==0) {
          mots[i] = compo_temp+nom_champ +"X";
          }
          else if (i==1) {
          mots[i] = compo_temp+nom_champ +"Y";;
          }
          else if (i==2) {
          mots[i] = compo_temp+nom_champ +"Z";
          }
          }

          return mots;
          break;
        */

        int nb_comp = dimension;
        Noms compo(nb_comp);
        for (int i=0; i<nb_comp; i++)
          {
            Nom nume(i);
            compo[i] = nom_post_+nume;
          }

        return compo;
        break;
      }

    case 1:
      {
        Noms unites(dimension);
        Noms source_unites = get_source(0).get_property("unites");

        for (int i=0; i<dimension; i++)
          {
            unites[i] = source_unites[0]+"/m";
          }

        return unites;
        break;
      }
    }

  return Champ_Gen_de_Champs_Gen::get_property(query);
}

Entity Champ_Generique_Gradient::get_localisation(const int index) const
{
  Entity loc;
  Nom type_op = Op_Grad_.valeur().que_suis_je();
  if ((type_op=="Op_Grad_P1NC_to_P0") && (index <= 0))
    loc = ELEMENT;
  else if ((type_op=="Op_Grad_P0_to_Face") && (index <= 0))
    loc=FACE;
  else
    {
      Cerr << "Error of type : Gradient operator " << type_op << " is not possible for the defined field " << nom_post_ << finl;
      throw Champ_Generique_erreur("INVALID");
    }
  return loc;
}
//Nomme le champ en tant que source par defaut
//"Gradient_" + nom_champ_source
void Champ_Generique_Gradient::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_source;
      const Noms nom = get_source(0).get_property("nom");
      nom_champ_source = nom[0];
      nom_post_source =  "Gradient_";
      nom_post_source +=  nom_champ_source;
      nommer(nom_post_source);
    }
}

const Motcle Champ_Generique_Gradient::get_directive_pour_discr() const
{
  Motcle directive;

  Nom type_op = Op_Grad_.valeur().que_suis_je();
  if (type_op=="Op_Grad_P1NC_to_P0")
    directive = "champ_elem";
  else
    directive = "gradient_pression";

  return directive;
}
