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
// File:        Moyenne_volumique.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Statistiques_temps
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Moyenne_volumique.h>
#include <Param.h>
#include <Equation_base.h>
#include <Postraitement.h>
#include <Zone_VF.h>
#include <Ref_Zone_VF.h>
#include <Octree_Double.h>
#include <communications.h>

Implemente_instanciable(Moyenne_volumique,"Moyenne_volumique",Interprete);

Sortie& Moyenne_volumique::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description: lecture de la fonction de filtrage.
//  {
//    type BOITE|CHAPEAU|QUADRA|GAUSSIENNE|PARSER
//    demie-largeur L
//    [ omega W ]
//    [ expression FORMULE ]
//  }
Entree& Moyenne_volumique::readOn(Entree& is )
{
  l_ = -1;
  expression_parser_ = "??";
  int type;
  Param param(que_suis_je());
  param.ajouter("type", & type, Param::REQUIRED /* obligatoire */);
  param.dictionnaire("BOITE", BOITE);
  param.dictionnaire("CHAPEAU", CHAPEAU);
  param.dictionnaire("GAUSSIENNE", GAUSSIENNE);
  param.dictionnaire("QUADRA", QUADRA);
  param.dictionnaire("PARSER", PARSER);
  param.ajouter("demie-largeur", & box_size_, Param::REQUIRED /* obligatoire */);
  param.ajouter("omega", & l_);
  param.ajouter("expression", & expression_parser_);
  param.lire_avec_accolades_depuis(is);
  switch(type)
    {
    case BOITE:
      type_ = BOITE;
      l_ = box_size_;
      break;
    case CHAPEAU:
      type_ = CHAPEAU;
      l_ = box_size_;
      break;
    case GAUSSIENNE:
      type_ = GAUSSIENNE;
      if (l_ < 0.)
        {
          Cerr << "Error : OMEGA must be set to >= 0" << finl;
          barrier();
          exit();
        }
      break;
    case PARSER:
      type_ = PARSER;
      if (expression_parser_ == "??")
        {
          Cerr << "Error : EXPRESSION must be specified for the parser." << finl;
          barrier();
          exit();
        }
      {
        String2 s(expression_parser_);
        parser_.setString(s);
        parser_.setNbVar(Objet_U::dimension);
        parser_.addVar("x");
        parser_.addVar("y");
        if (Objet_U::dimension == 3)
          parser_.addVar("z");
        parser_.parseString();
      }
      break;
    case QUADRA:
      type_ = QUADRA;
      l_ = box_size_;
      Cerr << "l_ = " << l_ << "box_size_ = " << box_size_ << finl;
      break;
    default:
      exit(); // Erreur interne !
    }
  // Pour que l'octree trouve bien les elements qui sont pile-poil a la limite.
  box_size_ += precision_geom;
  return is;
}
inline double fonction_quadra(double x, double l_)
{
  assert(fabs(x) <= l_);
  double ax = 1. - fabs(x) / l_;
  ax = ax * ax;
  if (fabs(x) < (l_/3.))
    {
      double bx = -3. / l_ * fabs(x) + 1.;
      ax -= bx * bx / 3.; // signifie ax = ax - bx*bx/3
    }
  return ax * (27. / (16. * l_));
}
// Description: Evalue la fonction filtre en chaque coordonnee coord
//  Methode appelee dans la classe Calcul_integrale_locale
void Moyenne_volumique::eval_filtre(const DoubleTab& coords, ArrOfDouble& result) const
{
  const int dim = Objet_U::dimension;
  assert(dim == coords.dimension(1));
  const int n = coords.dimension(0);
  assert(result.size_array() == n);
  switch(type_)
    {
    case PARSER:
      {
        for (int i = 0; i < n; i++)
          {
            for (int j = 0; j < dim; j++)
              parser_.setVar(j, coords(i,j));
            result[i] = parser_.eval();
          }
        break;
      }
    case BOITE:
      {
        double facteur = 0.;
        if (dim == 2)
          facteur = 1. / (l_ * l_ * 4.);
        else
          facteur = 1. / (l_ * l_ * l_ * 8.);

        for (int i = 0; i < n; i++)
          {
            const double x = coords(i,0);
            const double y = coords(i,1);
            const double z = (dim==3) ? coords(i,2) : 0.;
            double r = facteur;
            if (x > l_ || x < -l_
                || y > l_ || y < -l_
                || z > l_ || z < -l_)
              r = 0.;
            result[i] = r;
          }
        break;
      }
    case CHAPEAU:
      {
        const double L2D = l_*l_*l_*l_;
        const double L3D = L2D*l_*l_;
        double facteur;
        if (dim==3)
          facteur = 1. / L3D;
        else
          facteur = 1. / (L2D * l_);
        const int nbis = coords.dimension(0);
        for (int i = 0; i < nbis; i++)
          {
            const double x = coords(i, 0);
            const double y = coords(i, 1);
            const double z = (dim == 3) ? coords(i, 2) : 0.;
            double resu = 0.;
            const double ax = fabs(x);
            const double ay = fabs(y);
            const double az = fabs(z);
            if (ax <= l_ && ay <= l_ && az <= l_)
              resu = (l_-ax) * (l_-ay) * (l_-az) * facteur;
            result[i] = resu;
          }
        break;
      }
    case GAUSSIENNE:
      {
        double facteur1 = - 0.5 / (l_ * l_);
        double facteur2 = 1. / (l_ * sqrt(2 * M_PI));
        if (dim == 2)
          facteur2 = facteur2 * facteur2;
        else
          facteur2 = facteur2 * facteur2 * facteur2;
        for (int i = 0; i < n; i++)
          {
            const double x = coords(i, 0);
            const double y = coords(i, 1);
            const double z = (dim == 3) ? coords(i, 2) : 0.;
            const double k = (x*x + y*y + z*z) * facteur1;
            result[i] = exp(k) * facteur2;
          }
        break;
      }
    case QUADRA:
      {
        for (int i = 0; i < n; i++)
          {
            const double x = coords(i, 0);
            const double y = coords(i, 1);
            const double z = (dim == 3) ? coords(i, 2) : 0.;
            double resu = 0;
            if (fabs(x) < l_ && fabs(y) < l_ && fabs(z) < l_)
              {
                resu = fonction_quadra(x,l_) * fonction_quadra(y,l_);
                if (dim == 3)
                  resu *= fonction_quadra(z,l_);
              }
            result[i] = resu;
          }
        break;
      }
    default:
      {
        Cerr << "Error in Moyenne_volumique::eval() : filter function is not initialized." << finl;
        exit();
      }
    }
}

// Description:
// Cherche le champ de nom "nom_champ" dans le probleme de nom "nom_pb"
//  dans les objers de l'interprete.
// Methode appelee par traiter_champs()
int Moyenne_volumique::get_champ(const Nom& nom_pb,
                                 const Nom& nom_champ,
                                 REF(Champ_base) & ref_champ)
{
  Probleme_base& pb = ref_cast(Probleme_base, objet(nom_pb));
  // Le champ est-il defini dans les postraitements (statistiques) ?
  const int nb_post = pb.postraitements().size();
  Motcle mc_nom_champ(nom_champ);
  for (int i_post = 0; i_post < nb_post; i_post++)
    {
      if (sub_type(Postraitement_base, pb.postraitements()[i_post].valeur()))
        {
          Postraitement& post = ref_cast(Postraitement, pb.postraitements()[i_post].valeur());
          Operateurs_Statistique_tps& stats = post.les_statistiques();
          const int nstat = stats.size();
          for (int i_stat = 0; i_stat < nstat; i_stat++)
            {
              Motcle tmp(stats[i_stat].valeur().le_nom() );

              if (tmp == mc_nom_champ)
                {
                  Operateur_Statistique_tps_base& stat = stats[i_stat].valeur();
                  ref_cast_non_const(DoubleTab, stat.integrale().valeurs()) = stat.calculer_valeurs();
                  ref_champ = stat.integrale();
                  return 1;
                }
            }
        }
    }

  ref_champ = pb.get_champ(nom_champ);
  return 1;
}

// Description: fonction outil permettant de faire les calculs et d'ecrire le resultat
//  dans un fichier lata pour tous les champs d'un type donne de la liste noms_champs.
//  Methode appelee par interpreter()
// type_champ=0 => traiter les champs aux elements
// type_champ=1 => traiter les champs aux faces
void Moyenne_volumique::traiter_champs(const Motcles& noms_champs,
                                       const Nom& nom_pb, const Nom& nom_dom,
                                       const DoubleTab& coords,
                                       Format_Post_base& post,
                                       double temps,
                                       const Motcle& localisation)
{
  const Domaine& dom_post = ref_cast(Domaine, objet(nom_dom));
  const int nb_champs = noms_champs.size();
  if (nb_champs == 0)
    return;

  REF(Champ_base) ref_champ;
  REF(Zone_VF) ref_zone_vf;
  int i_champ;
  // ************************************
  // Calcul du nombre total de composantes et de ref_zone_vf
  int nb_compo_tot = 0;
  for (i_champ = 0; i_champ < nb_champs; i_champ++)
    {
      get_champ(nom_pb, noms_champs[i_champ], ref_champ);
      const Champ_base& champ = ref_champ.valeur();
      const Zone_VF& zvf = ref_cast(Zone_VF, champ.zone_dis_base());
      if (i_champ == 0)
        {
          ref_zone_vf = zvf;
        }
      else
        {
          if (& (ref_zone_vf.valeur()) != & zvf)
            {
              Cerr << "Error in Moyenne_volumique::traiter_champs all the fields must be discretized on the same Zone." << finl;
              barrier();
              exit();
            }
        }
      const int nb_compo = champ.nb_comp();
      nb_compo_tot += nb_compo;
    }

  const Zone_VF& zone_source = ref_zone_vf.valeur();

  // ************************************
  // Construction d'un gros tableau contenant toutes les valeurs a traiter plus la porosite
  DoubleTab valeurs_src;
  const int nb_lignes = zone_source.nb_elem();
  valeurs_src.resize(nb_lignes, nb_compo_tot + 1);
  int count = 0;
  {
    DoubleTab tmp_val;
    IntVect liste_elems(nb_lignes);
    for (int i = 0; i < nb_lignes; i++)
      liste_elems[i] = i;
    const DoubleTab& xp = zone_source.xp();
    for (i_champ = 0; i_champ < nb_champs; i_champ++)
      {
        get_champ(nom_pb, noms_champs[i_champ], ref_champ);
        const Champ_base& champ = ref_champ.valeur();
        const int nb_compo = champ.nb_comp();
        tmp_val.reset();
        tmp_val.resize(nb_lignes, nb_compo);
        champ.valeur_aux_elems(xp, liste_elems, tmp_val);

        for (int i = 0; i < nb_lignes; i++)
          for (int j = 0; j < nb_compo; j++)
            valeurs_src(i, count+j) = tmp_val(i, j);
        count += nb_compo;

        Cout << "Field name = " << ref_champ->le_nom()
             << " Field type = " << ref_champ->que_suis_je() << finl;
      }
  }
  // et une colonne de 1:
  for (int i = 0; i < nb_lignes; i++)
    valeurs_src(i, count) = 1.;

  // Tableau de resultats:
  const int nb_coords = coords.dimension(0);
  DoubleTab resu(nb_coords, nb_compo_tot + 1);

  // ************************************
  // Calcul de tous les produits de convolution

  calculer_convolution_champ_elem(zone_source,
                                  valeurs_src,
                                  coords,
                                  resu);
  Noms nom_dir;
  nom_dir.add("_X");
  nom_dir.add("_Y");
  nom_dir.add("_Z");
  count = 0;
  for (i_champ = 0; i_champ < nb_champs; i_champ++)
    {
      get_champ(nom_pb, noms_champs[i_champ], ref_champ);
      const Champ_base& champ = ref_champ.valeur();
      const int nb_compo = champ.nb_comp();
      DoubleTab extrait(nb_coords, nb_compo);
      for (int i = 0; i < nb_coords; i++)
        for (int j = 0; j < nb_compo; j++)
          extrait(i, j) = resu(i, count + j);

      Cout << "Post writting " << champ.le_nom() << finl;
      Nom nature("scalar");
      if (champ.nature_du_champ()==vectoriel) nature="vector";
      post.ecrire_champ(dom_post,
                        champ.unites(),
                        champ.noms_compo(),
                        -1 /* ecrire toutes les composantes */, temps, temps,
                        noms_champs[i_champ], nom_dom, localisation,nature, extrait);

      count += nb_compo;
    }
  // Derniere colonne (porosite)
  const int nb_compo = 1;
  DoubleTab extrait(nb_coords, nb_compo);
  for (int i = 0; i < nb_coords; i++)
    for (int j = 0; j < nb_compo; j++)
      extrait(i, j) = resu(i, count + j);

  Noms noms_compo;
  Noms unites;
  Nom nom_moyenne;
  unites.add("m3");
  noms_compo.add("porosite");
  nom_moyenne = "porosite";
  Cout << "Porosity post writing" << finl;

  post.ecrire_champ(dom_post, unites, noms_compo, -1 /* ecrire toutes les composantes */,
                    temps, temps,
                    nom_moyenne, nom_dom, localisation, "scalar",extrait);
}

// Description: Lecture des parametres dans le jeu de donnees. Format attendu:
//  Moyenne_volumique {
//    nom_pb NOM_DU_PROBLEME    (ou chercher les champs sources)
//    nom_domaine DOMAINE_CIBLE (on evalue la convolution aux elements de ce domaine)
//    noms_champs N CHAMP1 CHAMP2 ... (noms des champs a filtrer dans le probleme)
//    [ nom_fichier_post NOM_SANS_EXTENSION ] (soit on donne nom_fichier et format_post,
//                                             soit on donne fichier_post)
//    [ format_post lata|meshtv|lml|med|... ] (par defaut lata)
//    [ fichier_post Format_Post_XXX { ... } ] (lecture par readOn du Format_Post_XXX)
//    fonction_filtre ...  (format : voir Moyenne_volumique::readOn() )
//    [ localisation ELEM|SOM ]
//  }
Entree& Moyenne_volumique::interpreter(Entree& is)
{
  Cerr << "Starting of Moyenne_volumique::interpreter" << finl;
  Nom nom_pb, nom_dom;
  Motcles noms_champs;
  Param param(que_suis_je() + Nom("::interpreter()"));
  const int id_elem = 0;
  const int id_som = 1;
  int localisation = id_elem; // par defaut
  Motcle format_post("lata_v1");
  Nom nom_fichier_post;
  DERIV(Format_Post_base) fichier_post;
  param.ajouter("nom_pb", & nom_pb, Param::REQUIRED);
  param.ajouter("nom_domaine", & nom_dom, Param::REQUIRED);
  param.ajouter("noms_champs", & noms_champs, Param::REQUIRED);
  param.ajouter("fichier_post", & fichier_post);
  param.ajouter("format_post", & format_post);
  param.ajouter("nom_fichier_post", & nom_fichier_post);
  // L'objet Moyenne_volumique est un interprete, mais c'est aussi un objet
  // dont la seule propriete est la fonction filtre a utiliser... astuce:
  // on appelle le readOn de la classe pour lire la fonction filtre.
  param.ajouter("fonction_filtre", this, Param::REQUIRED);
  param.ajouter("localisation", & localisation);
  param.dictionnaire("ELEM", id_elem);
  param.dictionnaire("SOM", id_som);
  param.lire_avec_accolades_depuis(is);

  // on recupere le domaine
  const Domaine& dom = ref_cast(Domaine, objet(nom_dom));
  if (noms_champs.size() == 0)
    {
      Cerr << "Moyenne_volumique : no field to treat" << finl;
      return is;
    }
  Cerr << "Writing of the post-processing domain : " << nom_dom << finl;
  REF(Champ_base) ref_champ;
  get_champ(nom_pb, noms_champs[0], ref_champ);
  const double temps = ref_champ.valeur().temps();
  if (!fichier_post.non_nul())
    {
      if (nom_fichier_post == "??")
        {
          Cerr << "Error in Moyenne_volumique::interpreter:\n"
               << " missing NOM_FICHIER_POST or FICHIER_POST keyword" << finl;
          barrier();
          exit();
        }
      if (format_post == "lata")
        format_post = "lata_v1";
      // Astuce pour permettre le cas test de non regression en sequentiel et en parallele:
      //  (il faut que le nom du fichier de sortie soit le nom du cas)
      if (nom_fichier_post == "NOM_DU_CAS")
        {
          Cerr << "Post filename = NOM_DU_CAS => using " << nom_du_cas() << " instead" << finl;
          nom_fichier_post = nom_du_cas();
        }
      fichier_post.typer(Motcle("FORMAT_POST_") + format_post);
      fichier_post.valeur().initialize(nom_fichier_post, 1 /* binaire */, "SIMPLE");
    }
  else
    {
      if (nom_fichier_post != "??")
        {
          Cerr << "Error in Moyenne_volumique::interpreter:\n"
               << " you cannot give NOM_FICHIER_POST and FICHIER_POST. Choose one" << finl;
          barrier();
          exit();
        }
    }
  Format_Post_base& post = fichier_post.valeur();
  post.ecrire_entete(temps, 0 /*reprise*/, 1 /* premier post */);
  post.ecrire_domaine(dom, 1 /* premier_post */);
  post.ecrire_temps(temps);

  // Coordonnees des centres des elements du domaine destination
  DoubleTab coords;
  if (localisation == id_elem)
    {
      dom.zone(0).calculer_centres_gravite(coords);
      // Le tableau contient aussi les elements virtuels et pas d'espace virtuel. bouh.
      coords.resize(dom.zone(0).nb_elem(), coords.dimension(1));
    }
  else
    {
      coords = dom.les_sommets();
    }

  Motcle loc("ELEM");
  if (localisation == id_som)
    loc = "SOM";
  traiter_champs(noms_champs,
                 nom_pb,
                 nom_dom,
                 coords,
                 post,
                 temps,
                 loc);
  int fin=1;
  post.finir(fin);
  return is;
}

// .DESCRIPTION: classe outil utilisee en interne dans calculer_convolution()
class Calcul_integrale_locale
{
public:
  Calcul_integrale_locale(const Zone_VF& zone_source,
                          const Moyenne_volumique& filter,
                          const DoubleTab& champ_source);
  void calculer(double x, double y, double z, ArrOfDouble& resu);

protected:
  const Zone_VF& zone_source_;
  Octree_Double octree_;
  const Moyenne_volumique& filter_;
  const DoubleTab& champ_source_;
  // Tableaux temporaires utilises dans calculer():
  ArrOfInt liste_elems_;
  DoubleTab filter_coords_;
  ArrOfDouble filter_results_;
  int nb_items_reels_;
};

// Description: constructeur de la classe outil...
//  Voir Moyenne_volumique::calculer_convolution()
Calcul_integrale_locale::Calcul_integrale_locale(const Zone_VF& zone_source,
                                                 const Moyenne_volumique& filter,
                                                 const DoubleTab& champ_source) :
  zone_source_(zone_source),
  filter_(filter),
  champ_source_(champ_source)
{
  liste_elems_.set_smart_resize(1);
  filter_coords_.set_smart_resize(1);
  filter_results_.set_smart_resize(1);
  // Construction d'un octree contenant les centres des elements
  // On copie le tableau car il sera retaille :
  DoubleTab coords = zone_source.xp();
  nb_items_reels_ = zone_source.nb_elem();
  // Le tableau xp est dimensionne avec dimension(0)=nb_elem_tot. On le retaille a nb_elem
  coords.resize(nb_items_reels_, coords.dimension(1));
  if (champ_source.dimension(0) != nb_items_reels_)
    {
      Cerr << "Error in Calcul_integrale_locale::Calcul_integrale_locale() :\n"
           << " The source field is not discretized at the elements" << finl;
      Process::barrier();
      Process::exit();
    }
  octree_.build_nodes(coords, 0 /* no virtual items */);
}

// Description: evalue le produit de convolution "filter_ * champ_source_" au point x,y,z
//  et stocke le resultat dans resu. On determine les elements a utiliser en fonction
//  de la taille du filtre en utilisant une octree.
//  On suppose que le champ source est aux elements.
//  Methode appelee par Moyenne_volumique::calculer_convolution()
void Calcul_integrale_locale::calculer(const double x, const double y, const double z,
                                       ArrOfDouble& resu)
{
  const double box_size = filter_.box_size();
  octree_.search_elements_box(x - box_size, y - box_size, z - box_size,
                              x + box_size, y + box_size, z + box_size,
                              liste_elems_);

  const DoubleTab& coord_items = zone_source_.xp();

  const int nb_items = liste_elems_.size_array();
  const int dim = Objet_U::dimension;
  const int dim3 = (dim == 3);
  filter_coords_.resize(nb_items, dim);
  for (int i = 0; i < nb_items; i++)
    {
      const int item = liste_elems_[i];
      assert(item < nb_items_reels_);
      filter_coords_(i, 0) = coord_items(item, 0) - x;
      filter_coords_(i, 1) = coord_items(item, 1) - y;
      if (dim3)
        filter_coords_(i, 2) = coord_items(item, 2) - z;
    }
  filter_results_.resize_array(nb_items);
  filter_.eval_filtre(filter_coords_, filter_results_);

  const DoubleVect& volumes = zone_source_.volumes();
  const int nb_comp = champ_source_.dimension(1);
  resu = 0.;
  for (int i = 0; i < nb_items; i++)
    {
      const int item = liste_elems_[i];
      const double valeur_filtre = filter_results_[i];
      const double volume = volumes(item);
      const double facteur = valeur_filtre * volume;
      for (int j = 0; j < nb_comp; j++)
        {
          // L'integrale est discretisee grossierement comme le produit des
          // valeurs au centre de l'element fois le volume de l'element:
          const double valeur_champ = champ_source_(item, j);
          resu[j] += valeur_champ * facteur;
        }
    }
}

// Description: methode generale pour calculer une convolution a partir d'un
//  champ aux elements ou aux faces. Methode appelee par calculer_convolution_champ_elem()
//  et calculer_convolution_champ_face()
void Moyenne_volumique::calculer_convolution(const Zone_VF& zone_source,
                                             const DoubleTab& champ_source,
                                             const DoubleTab& coords_to_compute,
                                             DoubleTab& resu) const
{
  assert(resu.dimension(0) == coords_to_compute.dimension(0));
  const int dim = Objet_U::dimension;
  const int nbproc = Process::nproc();
  const int nb_coords_to_compute = coords_to_compute.dimension(0);
  const int nb_coords_max = ::mp_max(nb_coords_to_compute);

  int nb_comp;
  nb_comp = champ_source.line_size();
  assert(resu.line_size() == nb_comp);

  DoubleTab coords(nbproc, 3);
  ArrOfInt flag(nbproc);
  ArrOfDouble resu_partiel(nb_comp);
  DoubleTab resu_partiels(nbproc, nb_comp);

  Calcul_integrale_locale integrale_locale(zone_source,
                                           *this,
                                           champ_source);

  // Boucle sur les coordonnees x locales pour lesquelles on veut calculer l'integrale I(x).
  // Si x est pres du bord, le support de la fonction filtre couvre des processeurs voisins
  // qu'il faut ajouter. Pour chaque coordonnees, on demande a tous les autres processeurs
  // de calculer la contribution. Donc on boucle sur le max du nombre de coordonnees pour
  // synchroniser les procs:
  int i, j;
  for (int i_coord = 0; i_coord < nb_coords_max; i_coord++)
    {
      // Chaque processeur envoie la coordonnee a calculer a tous les processeurs:
      if (i_coord < nb_coords_to_compute)
        {
          for (j = 0; j < dim; j++)
            {
              double x = coords_to_compute(i_coord, j);
              for (i = 0; i < nbproc; i++)
                coords(i, j) = x;
            }
          flag = 1;
        }
      else
        {
          flag = 0;
        }
      envoyer_all_to_all(coords, coords);
      envoyer_all_to_all(flag, flag);
      // On a dans coord(pe, i) les coordonnees demandees par chaque processeur et
      // flag[pe] indique si le processeur a demande une coordonnee ou s'il a fini
      // sa liste coords_to_compute.
      // On calcule maintenant la contribution du processeur local aux integrales I(x)
      // pour ces coordonnees. En general, le processeur qui a cette coordonnee chez lui
      // a beaucoup de travail pour celle-ci et presque rien a faire pour les autres coordonnees
      // (si la coordonnee est loin du domaine, l'octree trouve tres rapidement la liste vide).
      // Donc les calculs sont a peu pres equilibres sur les processeurs.
      for (i = 0; i < nbproc; i++)
        {
          if (flag[i])
            {
              integrale_locale.calculer(coords(i, 0), coords(i, 1), coords(i, 2), resu_partiel);
              for (j = 0; j < nb_comp; j++)
                resu_partiels(i, j) = resu_partiel[j];
            }
        }
      // On renvoie a chaque processeur la contribution du processeur local pour la coordonnee
      // qu'il a demande:
      envoyer_all_to_all(resu_partiels, resu_partiels);
      // Le resultat est la somme des contributions de tous les processeurs:
      if (i_coord < nb_coords_to_compute)
        {
          for (j = 0; j < nb_comp; j++)
            {
              double x = 0.;
              for (i = 0; i < nbproc; i++)
                x += resu_partiels(i, j);
              resu(i_coord, j) = x;
            }
        }
    }
}

// Description: Calcule le produit de convolution entre la fonction filtre et
//  le champ "champ_source" qui doit etre discretise aux elements de la "zone_source".
//  Le tableau resu aura le meme nombre de colonnes que le tableau "champ_source", et
//  le meme nombre de lignes que le tableau coords_to_compute.
//  On suppose que la fonction filtre a un support inclu dans un cube de demi-cote box-size
//  centre sur l'origine (on ne calcule pas la contribution des elements hors de ce cube).
void Moyenne_volumique::calculer_convolution_champ_elem(const Zone_VF& zone_source,
                                                        const DoubleTab& champ_source,
                                                        const DoubleTab& coords_to_compute,
                                                        DoubleTab& resu) const
{
  assert(champ_source.dimension(0) == zone_source.zone().nb_elem());
  calculer_convolution(zone_source, champ_source,
                       coords_to_compute, resu);
}

// Description: Idem que calculer_convolution_champ_elem pour un champ VDF aux faces.
//  (on suppose que le champ source est un champ vectoriel contenant pour chaque face
//  la composante normale du champ a cette face)
//  Pour chaque colonne du tableau champ_source, on remplit "dimension" colonnes
//  du tableau resu : la premiere utilisant uniquement les faces de normale X, la deuxieme
//  avec les faces de normale Y, etc...
void Moyenne_volumique::calculer_convolution_champ_face(const Zone_VF& zone_source,
                                                        const DoubleTab& champ_source,
                                                        const DoubleTab& coords_to_compute,
                                                        DoubleTab& resu) const
{
  Cerr << " Moyenne_volumique::calculer_convolution_champ_face is not coded" << finl;
  exit();
}
