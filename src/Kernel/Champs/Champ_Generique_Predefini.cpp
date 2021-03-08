/****************************************************************************
* Copyright (c) 2021, CEA
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
// XD predefini champ_generique_base predefini -1 This keyword is used to post process predefined postprocessing fields.
// XD attr pb_champ deuxmots pb_champ 0 { Pb_champ nom_pb nom_champ } : nom_pb is the problem name and nom_champ is the selected field name. The available keywords for the field name are: energie_cinetique_totale, energie_cinetique_elem, viscosite_turbulente, viscous_force_x, viscous_force_y, viscous_force_z, pressure_force_x, pressure_force_y, pressure_force_z, total_force_x, total_force_y, total_force_z, viscous_force, pressure_force, total_force

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

const Champ_base& Champ_Generique_Predefini::get_champ_without_evaluation(Champ& espace_stockage) const
{
  return champ_->get_champ_without_evaluation(espace_stockage);
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
        else if (Motcle(type_champ_)=="VISCOUS_FORCE_X")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="VISCOUS_FORCE_Y")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="VISCOUS_FORCE_Z")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="PRESSURE_FORCES_X")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="PRESSURE_FORCES_Y")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="PRESSURE_FORCES_Z")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="TOTAL_FORCE_X")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="TOTAL_FORCE_Y")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="TOTAL_FORCE_Z")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="VISCOUS_FORCE")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="PRESSURE_FORCE")
          mots[0] = "kg.m2/s";
        else if (Motcle(type_champ_)=="TOTAL_FORCE")
          mots[0] = "kg.m2/s";
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
  Motcles les_mots(15);
  {
    les_mots[0] = "energie_cinetique_totale";
    les_mots[1] = "energie_cinetique_elem";
    les_mots[2] = "viscosite_turbulente";
    les_mots[3] = "viscous_force_x";
    les_mots[4] = "viscous_force_y";
    les_mots[5] = "viscous_force_z";
    les_mots[6] = "pressure_force_x";
    les_mots[7] = "pressure_force_y";
    les_mots[8] = "pressure_force_z";
    les_mots[9] = "total_force_x";
    les_mots[10] = "total_force_y";
    les_mots[11] = "total_force_z";
    les_mots[12] = "viscous_force";
    les_mots[13] = "pressure_force";
    les_mots[14] = "total_force";
  }

  Nom expression("");
  // on recupere la dimension du problem
  int dim=Objet_U::dimension;
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

    case 3:
      {
        expression  = " Morceau_equation { type operateur numero 0 option flux_bords compo 0 ";
        expression += "  source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } }";
        break;
      }

    case 4:
      {
        expression  = " Morceau_equation { type operateur numero 0 option flux_bords compo 1 ";
        expression += "  source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } }";
        break;
      }

    case 5:
      {
        expression  = " Morceau_equation { type operateur numero 0 option flux_bords compo 2 ";
        expression += "  source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } }";
        break;
      }

    case 6:
      {
        expression  = " Morceau_equation { type operateur numero 2 option flux_bords compo 0 ";
        expression += "  source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } }";
        break;
      }

    case 7:
      {
        expression  = " Morceau_equation { type operateur numero 2 option flux_bords compo 1 ";
        expression += "  source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } }";
        break;
      }

    case 8:
      {
        expression  = " Morceau_equation { type operateur numero 2 option flux_bords compo 2 ";
        expression += "  source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } }";
        break;
      }

    case 9:
      {
        expression  = " Transformation { methode formule expression 1 viscousFX+pressureFX sources { ";
        expression += " Morceau_equation { type operateur numero 0 option flux_bords compo 0 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source viscousFX } , ";
        expression += " Morceau_equation { type operateur numero 2 option flux_bords compo 0 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source pressureFX } ";
        expression += " } }";
        break;
      }

    case 10:
      {
        expression  = " Transformation { methode formule expression 1 viscousFY+pressureFY sources { ";
        expression += " Morceau_equation { type operateur numero 0 option flux_bords compo 1 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source viscousFY } , ";
        expression += " Morceau_equation { type operateur numero 2 option flux_bords compo 1 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source pressureFY } ";
        expression += " } }";
        break;
      }

    case 11:
      {
        expression  = " Transformation { methode formule expression 1 viscousFZ+pressureFZ sources { ";
        expression += " Morceau_equation { type operateur numero 0 option flux_bords compo 2 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source viscousFZ } , ";
        expression += " Morceau_equation { type operateur numero 2 option flux_bords compo 2 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source pressureFZ } ";
        expression += " } }";
        break;
      }

    case 12:
      {
        if (dim==3)
          {
            expression  = " Transformation { methode vecteur expression 3 viscousX viscousY viscousZ sources { ";
            expression += " Morceau_equation { type operateur numero 0 option flux_bords compo 2 source refChamp { Pb_champ ";
            expression += nom_pb_;
            expression += " vitesse } nom_source viscousZ } , ";
          }
        else
          expression  = " Transformation { methode vecteur expression 2 viscousX viscousY sources { ";
        expression += " Morceau_equation { type operateur numero 0 option flux_bords compo 0 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source viscousX } , ";
        expression += " Morceau_equation { type operateur numero 0 option flux_bords compo 1 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source viscousY } ";
        expression += " } }";
        break;
      }

    case 13:
      {
        if (dim==3)
          {
            expression  = " Transformation { methode vecteur expression 3 pressureX pressureY pressureZ sources { ";
            expression += " Morceau_equation { type operateur numero 2 option flux_bords compo 2 source refChamp { Pb_champ ";
            expression += nom_pb_;
            expression += " vitesse } nom_source pressureZ } , ";
          }
        else
          expression  = " Transformation { methode vecteur expression 2 pressureX pressureY sources { ";
        expression += " Morceau_equation { type operateur numero 2 option flux_bords compo 0 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source pressureX } , ";
        expression += " Morceau_equation { type operateur numero 2 option flux_bords compo 1 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source pressureY } ";
        expression += " } }";
        break;
      }

    case 14:
      {
        if (dim==3)
          {
            expression  = " Transformation { methode vecteur expression 3 viscousX+pressureX viscousY+pressureY viscousZ+pressureZ sources { ";
            expression += " Morceau_equation { type operateur numero 0 option flux_bords compo 2 source refChamp { Pb_champ ";
            expression += nom_pb_;
            expression += " vitesse } nom_source viscousZ } , ";
            expression += " Morceau_equation { type operateur numero 2 option flux_bords compo 2 source refChamp { Pb_champ ";
            expression += nom_pb_;
            expression += " vitesse } nom_source pressureZ } , ";
          }
        else
          expression  = " Transformation { methode vecteur expression 2 viscousX+pressureX viscousY+pressureY sources { ";
        expression += " Morceau_equation { type operateur numero 0 option flux_bords compo 0 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source viscousX } , ";
        expression += " Morceau_equation { type operateur numero 0 option flux_bords compo 1 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source viscousY } , ";
        expression += " Morceau_equation { type operateur numero 2 option flux_bords compo 0 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source pressureX } , ";
        expression += " Morceau_equation { type operateur numero 2 option flux_bords compo 1 source refChamp { Pb_champ ";
        expression += nom_pb_;
        expression += " vitesse } nom_source pressureY } ";
        expression += " } }";
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
