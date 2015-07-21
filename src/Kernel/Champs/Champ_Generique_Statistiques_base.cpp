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
// File:        Champ_Generique_Statistiques_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_Statistiques_base.h>
#include <Operateur_Statistique_tps_base.h>
#include <Param.h>

Implemente_base_sans_constructeur(Champ_Generique_Statistiques_base,"Champ_Generique_Statistiques_base",Champ_Gen_de_Champs_Gen);

Champ_Generique_Statistiques_base::Champ_Generique_Statistiques_base()
{
  tstat_deb_ = -1.;
  tstat_fin_ = -1;
}

// Description:
//    Imprime le nom et le type de l'operateur sur
//    un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_Generique_Statistiques_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

//cf Champ_Gen_de_Champs_Gen::readOn
Entree& Champ_Generique_Statistiques_base::readOn(Entree& s )
{
  Champ_Gen_de_Champs_Gen::readOn(s);
  return s ;
}

// t_deb : temps de debut des statistiques
// t_fin : temps de fin des statistiques
void Champ_Generique_Statistiques_base::set_param(Param& param)
{
  Champ_Gen_de_Champs_Gen::set_param(param);
  //On ne fixe pas t_deb et t_fin en lecture obligatoire (Param::REQUIRED)
  //car ces attributs peuvent etre fixes par fixer_tdeb_tfin()
  //ex : Postraitement::creer_champ_post_stat()
  //Mais specification obligatoire a conserver dans la doc pour l utilisateur
  //qui specifie son champ statistique de facon standard dans definition_champs.
  param.ajouter("t_deb",&tstat_deb_);
  param.ajouter("t_fin",&tstat_fin_);
}

int Champ_Generique_Statistiques_base::completer_post_statistiques(const Domaine& dom,const int is_axi,Format_Post_base& format)
{

  const Nom nom_post = le_nom();

  Nom localisation;
  const Entity& loc = get_localisation(0);
  if (loc==ELEMENT)
    localisation = "ELEM";
  else if (loc==NODE)
    localisation = "SOM";
  else
    {
      Cerr<<"This localization is not valid for postprocessing"<<finl;
      exit();
    }

  Champ espace_stockage_source;
  const Champ_base& source = integrale().le_champ()->get_champ(espace_stockage_source);
  source.completer_post_champ(dom,is_axi,localisation,nom_post,format);
  return 1;

}

void Champ_Generique_Statistiques_base::fixer_tdeb_tfin(const double& t_deb,const double& t_fin)
{
  tstat_deb_ = t_deb;
  tstat_fin_ = t_fin;
}

int Champ_Generique_Statistiques_base::sauvegarder(Sortie& os) const
{
  int bytes=0;
  bytes += Champ_Gen_de_Champs_Gen::sauvegarder(os);
  bytes += Operateur_Statistique().sauvegarder(os);
  return bytes;
}

int Champ_Generique_Statistiques_base::reprendre(Entree& is)
{
  Champ_Gen_de_Champs_Gen::reprendre(is);
  Nom bidon;
  is >> bidon >> bidon; // On saute l'identificateur et le type des champs
  Operateur_Statistique().reprendre(is);
  return 1;
}

void Champ_Generique_Statistiques_base::mettre_a_jour(double un_temps)
{
  Champ_Gen_de_Champs_Gen::mettre_a_jour(un_temps);
  Operateur_Statistique().mettre_a_jour(un_temps);
}


//Methodes pour changer t_deb et t_fin pour des reprises de statistiques
//et pour des statistiques en serie
void Champ_Generique_Statistiques_base::fixer_serie(const double& t1, const double& t2)
{
  Champ_Gen_de_Champs_Gen::fixer_serie(t1,t2);
  Operateur_Statistique().initialiser(0);
  Operateur_Statistique().fixer_tstat_deb(t1,t1);
  Operateur_Statistique().fixer_tstat_fin(t2);

}

void Champ_Generique_Statistiques_base::fixer_tstat_deb(const double& t1,const double& t2)
{
  Champ_Gen_de_Champs_Gen::fixer_tstat_deb(t1,t2);
  Operateur_Statistique().fixer_tstat_deb(t1,t2);
}

void Champ_Generique_Statistiques_base::lire_bidon(Entree& is) const
{
  Nom bidon;
  double dbidon;
  DoubleTab tab_bidon;

  Champ_Gen_de_Champs_Gen::lire_bidon(is);

  is >> bidon >> bidon;
  is >> dbidon;
  tab_bidon.jump(is);

}
