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
// File:        Champ_Generique_Predefini.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_Predefini.h>
#include <Param.h>
#include <Entree_complete.h>

Implemente_instanciable(Champ_Generique_Predefini,"Predefini",Champ_Gen_de_Champs_Gen);

Sortie& Champ_Generique_Predefini::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

//-cf Champ_Gen_de_Champs_Gen::readOn
//-Creation de l expression en fonction de type_champ_ qui a ete lu
//-Lecture de champ_
Entree& Champ_Generique_Predefini::readOn(Entree& s )
{
  Champ_Gen_de_Champs_Gen::readOn(s);
  Nom expression;
  expression = construit_expression();
  Entree_complete s_complete(expression,s);
  s_complete>>champ_;
  return s ;
}

//  methode : indique le nom du probleme a considerer
//              et le champ predefini a construire
//
void Champ_Generique_Predefini::set_param(Param& param)
{
  param.ajouter_non_std("Pb_champ",(this),Param::REQUIRED);
}

int Champ_Generique_Predefini::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="Pb_champ")
    {
      // Lecture du nom du probleme et du champ a construire
      is >> nom_pb_ >> type_champ_;
      return 1;
    }
  else
    return Champ_Generique_base::lire_motcle_non_standard(mot,is);
  return 1;
}

void Champ_Generique_Predefini::completer(const Postraitement_base& post)
{
  champ_->completer(post);
}

const Champ_Generique_base& Champ_Generique_Predefini::get_source(int i) const
{
  return ref_cast(Champ_Gen_de_Champs_Gen,champ_.valeur()).get_source(i);
}

const Champ_base& Champ_Generique_Predefini::get_champ(Champ& espace_stockage) const
{
  return champ_->get_champ(espace_stockage);
}

const Noms Champ_Generique_Predefini::get_property(const Motcle& query) const
{
  Motcles motcles(1);
  motcles[0] = "unites";
  int rang = motcles.search(query);
  switch(rang)
    {
    case 0:
      {
        Noms mots(1);
        if (Motcle(type_champ_)=="ENERGIE_CINETIQUE_TOTALE")
          mots[0] = "kg.m2/s2";
        else if (Motcle(type_champ_)=="ENERGIE_CINETIQUE_ELEM")
          mots[0] = "kg/(m.s2)";
        else if (Motcle(type_champ_)=="VISCOSITE_TURBULENTE")
          mots[0] = "m2/s";
        return mots;
        break;
      }
    }
  return champ_->get_property(query);
}

void Champ_Generique_Predefini::nommer(const Nom& nom)
{
  champ_->nommer(nom);
}

const Nom& Champ_Generique_Predefini::get_nom_post() const
{
  return champ_->get_nom_post();
}

void Champ_Generique_Predefini::nommer_source()
{
  ref_cast(Champ_Gen_de_Champs_Gen,champ_.valeur()).nommer_source();
}

Nom Champ_Generique_Predefini::construit_expression()
{
  Motcles les_mots(3);
  {
    les_mots[0] = "energie_cinetique_totale";
    les_mots[1] = "energie_cinetique_elem";
    les_mots[2] = "viscosite_turbulente";
  }

  Nom expression("");
  int rang = les_mots.search(type_champ_);
  switch(rang)
    {
    case 0:
      {
        expression  = " Reduction_0D { methode somme_ponderee ";
        expression += " source Transformation { methode formule expression 1 0.5*rho*norme_u*norme_u ";
        expression += " sources { Transformation { methode norme  localisation elem  source RefChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse }  nom_source norme_u } , refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " masse_volumique nom_source rho } } } } ";
        break;
      }

    case 1:
      {
        expression  = " Transformation { methode formule expression 1 0.5*rho*norme_u*norme_u ";
        expression += " sources { Transformation { methode norme  localisation elem  source RefChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse }  nom_source norme_u } , refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " masse_volumique nom_source rho } } } ";
        break;
      }

    case 2:
      {
        expression  = " modifier_pour_QC { division source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " viscosite_dynamique_turbulente } } ";
        break;
      }
    default :
      {
        Cerr<<"Only keywords among "<<les_mots<<" are allowed."<<finl;
        exit();
      }
    }
  return expression;
}
