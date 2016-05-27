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
// File:        Champ_Generique_Correlation.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_Correlation.h>
#include <Champ_Generique_Moyenne.h>
#include <Schema_Temps.h>
#include <Postraitement.h>
#include <Discretisation_base.h>
#include <Synonyme_info.h>

Implemente_instanciable(Champ_Generique_Correlation,"Champ_Post_Statistiques_Correlation|Correlation",Champ_Generique_Statistiques_base);

Sortie& Champ_Generique_Correlation::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Champ_Generique_Correlation::readOn(Entree& s )
{
  Champ_Generique_Statistiques_base::readOn(s);
  return s ;
}

void Champ_Generique_Correlation::completer(const Postraitement_base& post)
{
  Champ_Gen_de_Champs_Gen::completer(post);
  const Probleme_base& Pb = get_ref_pb_base();

  Noms nom = get_source(0).get_property("nom");
  Motcle nom_champ = nom[0];
  Noms nom2 = get_source(1).get_property("nom");
  Motcle nom_champ2 = nom2[0];

  const Champ_Generique_base& ch_integre = get_source(0);
  const Champ_Generique_base& ch_integre2 = get_source(1);
  const Zone_dis_base& zdis = get_ref_zone_dis_base();

  Op_Correlation_.associer(zdis,ch_integre,ch_integre2,tstat_deb_,tstat_fin_);
  const Postraitement& mon_post = ref_cast(Postraitement,post);

  Motcle nom_champ_moyenne("Moyenne_");
  nom_champ_moyenne += nom_champ;
  nom_champ_moyenne += "_";
  nom_champ_moyenne += Nom(tstat_deb_,"%e");
  nom_champ_moyenne += "_";
  nom_champ_moyenne += Nom(tstat_fin_,"%e");

  Motcle nom_champ_moyenne2("Moyenne_");
  nom_champ_moyenne2 += nom_champ2;
  nom_champ_moyenne2 += "_";
  nom_champ_moyenne2 += Nom(tstat_deb_,"%e");
  nom_champ_moyenne2 += "_";
  nom_champ_moyenne2 += Nom(tstat_fin_,"%e");

  if ((mon_post.comprend_champ_post(nom_champ_moyenne)) && (mon_post.comprend_champ_post(nom_champ_moyenne2)))
    {
      const Champ_Generique_base& champ_stat_base = mon_post.get_champ_post(nom_champ_moyenne);
      const Operateur_Statistique_tps_base& operateur = ref_cast(Champ_Generique_Moyenne,champ_stat_base).Operateur_Statistique();
      const Op_Moyenne& op_moyenne = ref_cast(Op_Moyenne,operateur);
      Op_Correlation_.associer_op_stat(op_moyenne);

      const Champ_Generique_base& champ_stat_base2 = mon_post.get_champ_post(nom_champ_moyenne2);
      const Operateur_Statistique_tps_base& operateur2 = ref_cast(Champ_Generique_Moyenne,champ_stat_base2).Operateur_Statistique();
      const Op_Moyenne& op_moyenne2 = ref_cast(Op_Moyenne,operateur2);
      Op_Correlation_.associer_op_stat(op_moyenne2);
    }
  else
    {
      Cerr<<"The specification of the post-processing correlation field "<<finl;
      Cerr<<get_property("nom")[0]<<finl;
      Cerr<<"implies to specify before the corresponding "<<finl;
      Cerr<<"post-processing statistics average fields."<<finl;
      Cerr<<"Please, modify your data set to specify the post-processing statistics"<<finl;
      Cerr<<"average fields before (instead of after) the post-processing correlation field"<<finl;
      Cerr<<get_property("nom")[0]<<finl;
      exit();
    }
  Op_Correlation_.completer(Pb);
}

const Champ_base& Champ_Generique_Correlation::get_champ(Champ& espace_stockage) const
{
  int nb_comp = integrale().valeur().nb_comp();
  //Nature_du_champ nature_source = source.nature_du_champ();
  //Pas completement exact car il y a le cas de la correlation vecteur-vecteur et dans
  //ce cas c est un tenseur qui est manipule (la nature n est pas scalaire ou vectorielle)
  Nature_du_champ nature_source = (nb_comp==1)?scalaire:vectoriel;
  Champ_Fonc es_tmp;
  espace_stockage = creer_espace_stockage(nature_source,nb_comp,es_tmp);

  DoubleTab& tab_correlation = espace_stockage.valeurs();
  tab_correlation = Op_Correlation_.calculer_valeurs();
  tab_correlation.echange_espace_virtuel();
  return espace_stockage.valeur();
}

const Noms Champ_Generique_Correlation::get_property(const Motcle& query) const
{

  //Creation des composantes serait a faire de maniere dynamique (Correlation_...)

  Motcles motcles(2);
  motcles[0] = "unites";
  motcles[1] = "composantes";

  int rang = motcles.search(query);
  switch(rang)
    {
    case 0:
      {
        const Noms noms_unites = integrale()->unites();
        return noms_unites;
        break;
      }
    case 1:
      {
        //On fixe les composantes specifiques pour le Champ_Generique_Moyenne
        //car dans Format_Post_Lml::ecrire_champ_lml() dans le cas d une localisation ELEM
        //elles sont utilisees

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

        /*
          const Noms source_compos = get_source(0).get_property("composantes");
          int nb_comp = source_compos.size();
          Noms compo(nb_comp);

          for (int i=0; i<nb_comp; i++) {
          Nom nume(i);
          compo[i] = nom_post_+nume;
          }
        */

        const Noms noms_compo = integrale()->noms_compo();
        return noms_compo;
        break;

      }
    }
  return Champ_Gen_de_Champs_Gen::get_property(query);
}

//Nomme le champ en tant que source par defaut
//"Correlation_"+nom_champ_source_1+nom_champ_source_2
void Champ_Generique_Correlation::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_source_1, nom_champ_source_2;
      const Noms nom_1 = get_source(0).get_property("nom");
      const Noms nom_2 = get_source(1).get_property("nom");
      nom_champ_source_1 = nom_1[0];
      nom_champ_source_2 = nom_2[0];

      nom_post_source =  "Correlation_";
      nom_post_source += nom_champ_source_1;
      nom_post_source += "_";
      nom_post_source += nom_champ_source_2;
      nommer(nom_post_source);
    }
}
//Renvoie 1 si tenseur a post-traiter, sinon 0
int Champ_Generique_Correlation::get_info_type_post() const
{
  const Noms compo = get_property("composantes");
  if (compo.size()>1)
    return 1;
  else
    return 0;
}

const Motcle Champ_Generique_Correlation::get_directive_pour_discr() const
{
  Motcle directive;

  int support_corr = ref_cast(Integrale_tps_produit_champs,integrale()).get_support_different();

  if (support_corr==1)
    directive = "champ_elem";
  else
    directive = Op_Correlation_.la_moyenne_a().integrale().le_champ().valeur().get_directive_pour_discr();

  return directive;
}

