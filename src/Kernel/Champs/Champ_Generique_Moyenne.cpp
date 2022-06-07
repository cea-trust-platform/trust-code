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

#include <Champ_Generique_Moyenne.h>
#include <Schema_Temps.h>
#include <Postraitement.h>
#include <Discretisation_base.h>
#include <Champs_compris.h>
#include <Synonyme_info.h>
#include <Param.h>

Implemente_instanciable(Champ_Generique_Moyenne,"Champ_Post_Statistiques_Moyenne|Moyenne",Champ_Generique_Statistiques_base);

Sortie& Champ_Generique_Moyenne::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Champ_Generique_Moyenne::readOn(Entree& s )
{
  Champ_Generique_Statistiques_base::readOn(s);

  return s ;
}

//condition_initiale : pour fixer la valeur de l integrale
//de l operateur moyenne en utilisant Champ_Fonc_reprise
//pour lire la moyenne recuperee depuis un fichier .xyz
void Champ_Generique_Moyenne::set_param(Param& param)
{
  Champ_Generique_Statistiques_base::set_param(param);
  param.ajouter_non_std("moyenne_convergee",(this));
}

int Champ_Generique_Moyenne::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="moyenne_convergee")
    {
      Op_Moyenne& operateur = ref_cast(Op_Moyenne,Operateur_Statistique());
      is>>operateur.moyenne_convergee();
      return 1;
    }
  else
    return Champ_Generique_Statistiques_base::lire_motcle_non_standard(mot,is);
  return 1;
}

void Champ_Generique_Moyenne::completer(const Postraitement_base& post)
{
  Champ_Gen_de_Champs_Gen::completer(post);

  const Probleme_base& Pb = get_ref_pb_base();
  const Champ_Generique_base& ch_integre = get_source(0);
  const Zone_dis_base& zdis = get_ref_zone_dis_base();

  Op_Moyenne_.associer(zdis,ch_integre,tstat_deb_,tstat_fin_);
  Op_Moyenne_.completer(Pb);
}

const Champ_base& Champ_Generique_Moyenne::get_champ(Champ& espace_stockage) const
{
  const REF(Champ_Generique_base)& mon_champ = integrale().le_champ();
  Champ espace_stockage_source;
  const Champ_base& source = mon_champ->get_champ(espace_stockage_source);
  Nature_du_champ nature_source = source.nature_du_champ();
  int nb_comp = source.nb_comp();
  Champ_Fonc es_tmp;
  espace_stockage = creer_espace_stockage(nature_source,nb_comp,es_tmp);

  DoubleTab& tab_moy = espace_stockage.valeurs();
  tab_moy = Op_Moyenne_.calculer_valeurs();
  tab_moy.echange_espace_virtuel();
  return espace_stockage.valeur();
}

const Noms Champ_Generique_Moyenne::get_property(const Motcle& query) const
{

  //Creation des composantes serait a faire de maniere dynamique (Moyenne_...)

  Motcles motcles(1);
  motcles[0] = "composantes";

  int rang = motcles.search(query);
  switch(rang)
    {

    case 0:
      {
        //On fixe les composantes specifiques pour le Champ_Generique_Moyenne
        //car dans Format_Post_Lml::ecrire_champ_lml() dans le cas d une localisation ELEM
        //elles sont utilisees

        //Actuellement choix fait d ajouter simplement numero de composante
        /*
          const Noms compo_cibles = get_source(0).get_property("composantes");
          int size = compo_cibles.size();
          Noms mots(size);
          for (int i=0; i<size; i++) {
          mots[i] = "Moyenne_";
          mots[i] += compo_cibles[i];
          }

          return mots;

          break;
        */

        const Noms source_compos = get_source(0).get_property("composantes");
        int nb_comp = source_compos.size();
        Noms compo(nb_comp);

        for (int i=0; i<nb_comp; i++)
          {
            Nom nume(i);
            compo[i] = nom_post_+nume;
          }

        return compo;
        break;
      }
    }
  return Champ_Gen_de_Champs_Gen::get_property(query);
}

//Nomme le champ en tant que source par defaut
//"Moyenne_"+nom_champ_source
void Champ_Generique_Moyenne::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_source;
      const Noms nom = get_source(0).get_property("nom");
      nom_champ_source = nom[0];
      nom_post_source =  "Moyenne_";
      nom_post_source +=  nom_champ_source;
      nommer(nom_post_source);
    }
}

const Champ_Generique_base& Champ_Generique_Moyenne::get_champ_post(const Motcle& nom) const
{
  REF(Champ_Generique_base) ref_champ;
  try
    {
      return Champ_Gen_de_Champs_Gen::get_champ_post(nom) ;
    }
  catch (Champs_compris_erreur)
    {
    }

  //Nom supplementaire teste car c est le nom utilise par :
  //Champ_Generique_Ecart_Type::completer()
  //Champ_Generique_Correlation::completer()

  Noms nom_source = get_source(0).get_property("nom");
  Motcle nom_champ = nom_source[0];
  Motcle nom_champ_moyenne("Moyenne_");
  nom_champ_moyenne += nom_champ;
  nom_champ_moyenne += "_";
  nom_champ_moyenne += Nom(tstat_deb_,"%e");
  nom_champ_moyenne += "_";
  nom_champ_moyenne += Nom(tstat_fin_,"%e");

  if (nom_champ_moyenne==nom)
    {
      ref_champ = *this;
      ref_champ->fixer_identifiant_appel(nom);
      return ref_champ;
    }

  throw Champs_compris_erreur();
  return ref_champ;
}

int Champ_Generique_Moyenne::comprend_champ_post(const Motcle& identifiant) const
{
  if (Champ_Gen_de_Champs_Gen::comprend_champ_post(identifiant))
    return 1;

  //Nom supplementaire teste car c est le nom utilise par :
  //Champ_Generique_Ecart_Type::completer()
  //Champ_Generique_Correlation::completer()
  Noms nom_source = get_source(0).get_property("nom");
  Motcle nom_champ = nom_source[0];
  Motcle nom_champ_moyenne("Moyenne_");
  nom_champ_moyenne += nom_champ;
  nom_champ_moyenne += "_";
  nom_champ_moyenne += Nom(tstat_deb_,"%e");
  nom_champ_moyenne += "_";
  nom_champ_moyenne += Nom(tstat_fin_,"%e");

  if (nom_champ_moyenne==identifiant)
    return 1;

  return 0;
}
