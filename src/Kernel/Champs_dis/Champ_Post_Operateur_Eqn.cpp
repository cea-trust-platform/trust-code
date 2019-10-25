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
// File:        Champ_Post_Operateur_Eqn.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs_dis
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Post_Operateur_Eqn.h>
#include <Probleme_base.h>
#include <Postraitement.h>
#include <Zone_VF.h>
#include <Nom.h>
#include <Champ_Generique_refChamp.h>
#include <Discretisation_base.h>
#include <Equation_base.h>
#include <Operateur_base.h>
#include <Param.h>
#include <MD_Vector_composite.h>

Implemente_instanciable_sans_constructeur(Champ_Post_Operateur_Eqn,"Operateur_Eqn|Champ_Post_Operateur_Eqn",Champ_Generique_Operateur_base);

Sortie& Champ_Post_Operateur_Eqn::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}
Champ_Post_Operateur_Eqn::Champ_Post_Operateur_Eqn()
{
  numero_op_=-1;
  numero_source_=-1;
  sans_solveur_masse_=0;
}

void Champ_Post_Operateur_Eqn::set_param(Param& param)
{
  Champ_Generique_Operateur_base::set_param(param);
  param.ajouter("numero_source",&numero_source_);
  param.ajouter("numero_op",&numero_op_);
  param.ajouter_flag("sans_solveur_masse",&sans_solveur_masse_);
}

Entree& Champ_Post_Operateur_Eqn::readOn(Entree& s )
{
  Champ_Generique_Operateur_base::readOn(s);

  return s ;
}

void Champ_Post_Operateur_Eqn::completer(const Postraitement_base& post)
{

  Champ_Gen_de_Champs_Gen::completer(post);

  const Probleme_base& Pb = ref_cast(Postraitement,post).probleme();
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
            //On parcours les equatiosn du probleme et on identifie celle qui correspond au champ inconnu

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
      Cerr<<"Champ_Post_Operateur_Eqn can be applied only to equation unknown."<<finl;
      exit();
    }

  ref_eq_=Pb.equation(numero_eq_);

  int ok=0;
  const Equation_base& eqn=ref_eq_.valeur();
  const MD_Vector& mdf = eqn.inconnue().valeurs().get_md_vector(),
                   md = sub_type(MD_Vector_composite, mdf.valeur()) ? ref_cast(MD_Vector_composite, mdf.valeur()).get_desc_part(0) : mdf;
  const Zone_VF& zvf= ref_cast( Zone_VF,ref_eq_.valeur().zone_dis().valeur());
  if (md== zvf.face_sommets().get_md_vector())
    {
      localisation_inco_=FACE;
      ok=1;
    }
  if (md== zvf.zone().les_elems().get_md_vector())
    {
      localisation_inco_=ELEMENT;
      ok=1;
    }
  if (md == zvf.zone().domaine().les_sommets().get_md_vector())
    {
      ok=1;
      localisation_inco_=NODE;
    }
  if (ok==0)
    {
      Cerr<<"Error in "<<que_suis_je()<<" unknown localisation"<<finl;
      exit();
    }

}

const Champ_base& Champ_Post_Operateur_Eqn::get_champ_without_evaluation(Champ& espace_stockage) const
{

  Champ_Fonc espace_stockage_fonc;
  Champ source_espace_stockage;
  //const Champ_base& source = get_source(0).get_champ(source_espace_stockage);

  //  const Champ_Inc_base& ch_inc=ref_cast(Champ_Inc_base,source);
  double temps=0.;
  Nom directive;
// directive=ref_eq_->inconnue().le_nom();
  // bidouille EF
  //if (directive=="enthalpie") directive="temperature";
  switch (localisation_inco_)
    {
    case ELEMENT:
      directive="CHAMP_ELEM";
      break;
    case NODE:
      directive="CHAMP_SOMMETS";
      break;
    case FACE:
      directive="CHAMP_FACE";
      break;
    default:
      Cerr<<"error in Champ_Post_Operateur_Eqn::get_champ"<<finl;
      exit();

    }
  int nb_comp=ref_eq_.valeur().inconnue().valeur().nb_comp();
  ref_eq_.valeur().discretisation().discretiser_champ(directive,ref_eq_->zone_dis().valeur(),"oooo","unit", nb_comp,temps,espace_stockage_fonc);
  espace_stockage=espace_stockage_fonc;
  espace_stockage.valeur().fixer_nature_du_champ( ref_eq_.valeur().inconnue().valeur().nature_du_champ());

  return espace_stockage;
}
const Champ_base& Champ_Post_Operateur_Eqn::get_champ(Champ& espace_stockage) const
{
  espace_stockage=get_champ_without_evaluation(espace_stockage);
  DoubleTab& es =(espace_stockage.valeurs());
  //if (ref_eq_->schema_temps().temps_courant()!=0)
  {
    if (numero_op_!=-1)
      Operateur().calculer(ref_eq_->inconnue().valeurs(),es);
    else
      ref_eq_->sources()(numero_source_).calculer(es);
    if (!sans_solveur_masse_)
      ref_eq_->solv_masse().appliquer(es);
  }
  es.echange_espace_virtuel();
  // espace_stockage.valeurs()=es;
  return espace_stockage.valeur();
}

const Noms Champ_Post_Operateur_Eqn::get_property(const Motcle& query) const
{
//Creation des composantes serait a faire de maniere dynamique (Eqn_...)

  Motcles motcles(2);
  motcles[0] = "composantes";
  motcles[1] = "unites";

  int rang = motcles.search(query);

  switch(rang)
    {
    case 0:
      {

        int nb_comp= ref_eq_.valeur().inconnue().valeur().nb_comp();
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
        int nb_comp= ref_eq_.valeur().inconnue().valeur().nb_comp();
        Noms unites(nb_comp);
        //Noms source_unites = get_source(0).get_property("unites");

        for (int i=0; i<nb_comp; i++)
          {
            unites[i] = "unit";
          }

        return unites;
        break;
      }

    }
  return Champ_Gen_de_Champs_Gen::get_property(query);
}

Entity Champ_Post_Operateur_Eqn::get_localisation(const int index) const
{

  return localisation_inco_;

}
//Nomme le champ en tant que source par defaut
//"Eqn_" + nom_champ_source
void Champ_Post_Operateur_Eqn::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_source;
      //const Noms nom = get_source(0).get_property("nom");
      nom_post_source = "Eqn_s";
      nom_post_source +=  numero_source_ ;
      nom_post_source += "_o" ;
      nom_post_source += numero_op_ ;
      nommer(nom_post_source);
    }

}
const Operateur_base& Champ_Post_Operateur_Eqn::Operateur() const
{
  return ref_eq_->operateur(numero_op_).l_op_base();
}

Operateur_base& Champ_Post_Operateur_Eqn::Operateur()
{
  return ref_eq_->operateur(numero_op_).l_op_base();
}

