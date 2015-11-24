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
// File:        Op_Correlation.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Statistiques_temps
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Correlation.h>
#include <Schema_Temps.h>
#include <Discretisation_base.h>
#include <Champ_Generique_refChamp.h>

Implemente_instanciable(Op_Correlation,"Op_Correlation",Operateur_Statistique_tps_base);

//// printOn
//

Sortie& Op_Correlation::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Op_Correlation::readOn(Entree& s )
{
  return s ;
}

//////////////////////////////////////////////////////////////////
//
//         Fonctions de la classe Ecart_type_Champ_Face
//
/////////////////////////////////////////////////////////////////

void Op_Correlation::completer(const Probleme_base& Pb)
{
  const REF(Champ_Generique_base)& a = la_moyenne_a().integrale().le_champ();
  const REF(Champ_Generique_base)& b = la_moyenne_b().integrale().le_champ();
  integrale_tps_a_ = la_moyenne_a().integrale();
  integrale_tps_b_ = la_moyenne_b().integrale();

  // On recupere la zone discretisee
  const Zone_dis_base& zone = a->get_ref_zone_dis_base();

  // Dimensionnement du champ integrale_champ a la meme taille que mon_champ
  const DoubleTab& tab_a = valeurs_a();
  const DoubleTab& tab_b = valeurs_b();

  Champ espace_stockage_source_a, espace_stockage_source_b;
  const Champ_base& source_a = a->get_champ(espace_stockage_source_a);
  const Champ_base& source_b = b->get_champ(espace_stockage_source_b);

  const int& nb_compo_a = source_a.nb_comp();
  const int& nb_compo_b = source_b.nb_comp();

  //Pour utiliser nom_cible uniquement si a et b sont des Champ_Generique_refChamp
  //pour pouvoir reproduire le nom des variables dans le lml
  Noms noms_a, noms_b;

  if (!(sub_type(Champ_Generique_refChamp,a.valeur()) && (sub_type(Champ_Generique_refChamp,b.valeur()))))
    {
      noms_a = a->get_property("nom");
      noms_b = b->get_property("nom");
    }
  else
    {
      noms_a = a->get_property("nom_cible");
      noms_b = b->get_property("nom_cible");
    }

  const Nom nom_a = noms_a[0];
  const Nom nom_b = noms_b[0];

  //Cas particulier si a et b sont des Champ_Generique_refChamp
  //pour pouvoir reproduire le nom des variables dans le lml
  Noms compo_a, compo_b;

  if (!(sub_type(Champ_Generique_refChamp,a.valeur()) && (sub_type(Champ_Generique_refChamp,b.valeur()))))
    {
      compo_a = a->get_property("composantes");
      compo_b = b->get_property("composantes");
    }
  else
    {
      compo_a = source_a.noms_compo();
      compo_b = source_b.noms_compo();
    }

  const Noms unites_a = a->get_property("unites");
  const Noms unites_b = b->get_property("unites");

  int nb_valeurs_nodales = -1;

  // Note B.M.: integrale_tps_ab_ est deja type dans Correlation::associer
  // En revanche fixer_nb_valeurs_nodales n'est pas fait.

  if ( ( tab_a.get_md_vector() != tab_b.get_md_vector() ) ||
       ( tab_a.dimension_tot(0) != tab_b.dimension_tot(0) ) ||
       ( nb_compo_a > 1 && ( tab_a.nb_dim() == 1 || tab_a.dimension(1) != nb_compo_a ) ) ||
       ( nb_compo_b > 1 && ( tab_b.nb_dim() == 1 || tab_b.dimension(1) != nb_compo_b ) ) )
    {
      // Support different pour les champs a et b ou pour leurs composantes
      // Alors on ramene aux elements en creant un Champ_Fonc_PO_VDF ou VEF
      integrale_tps_ab_.support_different()=1;
      Nom Champ_Fonc_P0="Champ_Fonc_P0_";
      Champ_Fonc_P0+=Pb.discretisation().que_suis_je().substr_old(1,3);
      integrale_tps_ab_.typer(Champ_Fonc_P0);
      nb_valeurs_nodales = zone.nb_elem();
    }
  else
    {
      // Meme support pour les champs a et b
      integrale_tps_ab_.support_different()=0;
      if (tab_a.size_reelle_ok())
        nb_valeurs_nodales = tab_a.dimension(0);
      else
        nb_valeurs_nodales = -1; // B.M.: quoi donner a fixer_nb_valeurs_nodales ???
    }
  int nb_comp= nb_compo_a * nb_compo_b;
  DoubleTab& val = valeurs_ab();
  Noms noms_composantes;
  if (nb_compo_a == 1)
    if (nb_compo_b == 1)
      {
        // A et B champs scalaires
      }
    else
      {
        // A champ scalaire, B champ vectoriel
        noms_composantes.dimensionner(nb_comp);
        Nom debut("Correlation_");
        debut += nom_a+"_";
        for (int i=0; i<nb_comp; i++)
          noms_composantes[i]=debut+compo_b[i];
      }
  else if (nb_compo_b == 1)
    {
      // A champ vectoriel, B champ scalaire
      noms_composantes.dimensionner(nb_comp);
      Nom debut("Correlation_");
      debut+=nom_b+"_";
      for (int i=0; i<nb_comp; i++)
        noms_composantes[i]=debut+compo_a[i];
    }
  else
    {
      // A et B champs vectoriels
      noms_composantes.dimensionner(nb_comp);
      Nom debut("Correlation_");
      for (int i=0; i<nb_compo_a; i++)
        for (int j=0; j<nb_compo_b; j++)
          noms_composantes[i*nb_compo_b+j]=debut+compo_a[i]+"_"+compo_b[j];
    }

  if (nb_comp > 1)
    val.resize(0, nb_comp);

  Nom nom_pour_post("Correlation_");
  nom_pour_post+=nom_a+"_"+nom_b;
  integrale_tps_ab_->nommer(nom_pour_post);

  integrale_tps_ab_.associer_zone_dis_base(zone);
  integrale_tps_ab_->fixer_nb_comp(nb_comp);

  // BM: le parametre de fixer_nb_valeurs_nodales est inutile,
  //  le champ sait determiner tout seul combien de ddl il a, mais
  //  il y a un test sur le parametre: il faut que la valeur soit la bonne.
  integrale_tps_ab_.fixer_nb_valeurs_nodales(nb_valeurs_nodales);

  // On fixe les unites
  Nom unite(unites_a[0]);
  unite+=".";
  unite+=unites_b[0];

  if (nb_comp == 1)
    integrale_tps_ab_->fixer_unite(unite);
  else
    {
      Noms unites(nb_comp);
      for (int i=0; i<unites.size(); i++)
        unites[i]=unite;
      integrale_tps_ab_->fixer_noms_compo(noms_composantes);
      integrale_tps_ab_->fixer_unites(unites);
    }
  integrale_tps_ab_.changer_temps(Pb.schema_temps().temps_courant());
}

DoubleTab Op_Correlation::calculer_valeurs() const
{
  Integrale_tps_produit_champs correlation(integrale_tps_ab_);
  const double& dt_ab = dt_integration_ab();
  if ( dt_ab > 0 )
    {
      // On calcule Moyenne(a'b')=Moyenne(ab)-Moyenne(a)*Moyenne(b)
      correlation.valeurs() /= dt_ab;
      const double& dt_a = dt_integration_a();
      const double& dt_b = dt_integration_b();
      assert(est_egal(dt_a,dt_ab));
      assert(est_egal(dt_b,dt_ab));
      correlation.ajoute_produit_tensoriel(-1/(dt_a*dt_b),integrale_tps_a_.valeur().valeur(),integrale_tps_b_.valeur().valeur());
    }
  return correlation.valeurs();
}

int Op_Correlation::completer_post_statistiques(const Domaine& dom,const int is_axi,Format_Post_base& format)
{
  return 1;
}
