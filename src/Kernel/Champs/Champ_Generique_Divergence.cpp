/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Champ_Generique_Divergence.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_Divergence.h>
#include <Champ_Generique_refChamp.h>
#include <Discretisation_base.h>
#include <Synonyme_info.h>

Implemente_instanciable(Champ_Generique_Divergence,"Divergence",Champ_Generique_Operateur_base);
Add_synonym(Champ_Generique_Divergence,"Champ_Post_Operateur_Divergence");

Sortie& Champ_Generique_Divergence::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Champ_Generique_Divergence::readOn(Entree& s )
{
  Champ_Generique_Operateur_base::readOn(s);

  return s ;
}

void Champ_Generique_Divergence::completer(const Postraitement_base& post)
{
  Champ_Gen_de_Champs_Gen::completer(post);
  const Probleme_base& Pb = get_ref_pb_base();
  //On ne complete l operateur que s il sagit du champ de vitesse

  if (sub_type(Champ_Generique_refChamp,get_source(0)))
    {
      Champ espace_stockage;
      if (get_source(0).get_champ(espace_stockage).le_nom()=="vitesse")
        {
          const Equation_base& eqn = Pb.equation(0);
          Op_Div_.associer_eqn(eqn);
          Op_Div_.typer();
          Op_Div_.l_op_base().associer_eqn(eqn);

          //La methode completer() lance l association de la zone_discretisee
          //qui est recuperre par l equation associee a l operateur
          Op_Div_->completer();
        }
    }
}

const Champ_base& Champ_Generique_Divergence::get_champ(Champ& espace_stockage) const
{

  Champ source_espace_stockage;
  const Champ_base& source = get_source(0).get_champ(source_espace_stockage);

  if (Op_Div_.non_nul())
    {
      Champ_Fonc es_tmp;
      espace_stockage = creer_espace_stockage(scalaire,1,es_tmp);
      Op_Div_.calculer(source.valeurs(),espace_stockage.valeurs());
    }
  else
    {
      const Noms nom = get_source(0).get_property("nom");
      Cerr<<"We can apply a Champ_Generique_Divergence only to the velocity field"<<finl;
      Cerr<<"The field treated here "<<nom[0]<<" concerns "<<source.le_nom()<<finl;
      exit();
    }

  DoubleTab& espace_valeurs = espace_stockage->valeurs();
  espace_valeurs.echange_espace_virtuel();
  return espace_stockage.valeur();
}


Entity Champ_Generique_Divergence::get_localisation(const int index) const
{
  Entity loc;
  Nom type_op = Op_Div_.valeur().que_suis_je();
  if (((type_op=="Op_Div_VEF_P1NC") || (type_op=="Op_Div_VDF_Face")) && (index <= 0))
    loc = ELEMENT;
  else
    {
      Cerr << "Error of type : Divergence operator " << type_op << " is not possible for the defined field " << nom_post_ << finl;
      throw Champ_Generique_erreur("INVALID");
    }
  return loc;
}
const Noms Champ_Generique_Divergence::get_property(const Motcle& query) const
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
        Nom nume((int)0);
        compo[0] = nom_post_+nume;

        return compo;
        break;
      }

    case 1:
      {

        Noms unites(1);
        Noms source_unites = get_source(0).get_property("unites");
        unites[0] = source_unites[0]+"/m";

        return unites;
        break;
      }
    }
  return Champ_Gen_de_Champs_Gen::get_property(query);
}

//Nomme le champ en tant que source par defaut
//"Divergence_" + nom_champ_source
void Champ_Generique_Divergence::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_source;
      const Noms nom = get_source(0).get_property("nom");
      nom_champ_source = nom[0];
      nom_post_source =  "Divergence_";
      nom_post_source +=  nom_champ_source;
      nommer(nom_post_source);
    }
}

const Motcle Champ_Generique_Divergence::get_directive_pour_discr() const
{
  Motcle directive;
  directive = "divergence_vitesse";
  return directive;
}
