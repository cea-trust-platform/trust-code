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
#include <Corriger_frontiere_periodique.h>
#include <Octree_Double.h>
#include <Param.h>
#include <Format_Post.h>
#include <ArrOfBit.h>
#include <Reordonner_faces_periodiques.h>
#include <Domaine_bord.h>

Implemente_instanciable(Corriger_frontiere_periodique,"Corriger_frontiere_periodique",Interprete_geometrique_base);

Entree& Corriger_frontiere_periodique::readOn(Entree& is)
{
  return is;
}

Sortie& Corriger_frontiere_periodique::printOn(Sortie& os) const
{
  return os;
}

// Description:
//  Cet interprete permet de corriger les frontieres periodiques pour etre conformes aux
//  besoins de TRUST:
//   - reordonner les faces du bord periodique pour que la face i+n/2 soit en face de la face i,
//     et toutes les faces [0 .. n/2-1] du meme cote et [n/2 .. n-1] de l'autre cote
//   - deplacer les sommets des faces periodiques si besoin (si la CAO est fausse)
// Syntaxe:
//  Corriger_frontiere_periodique {
//     domaine NOMDOMAINE
//     bord    NOMBORDPERIO
//     [ direction DIMENSION dx dy [ dz ] ]
//     [ fichier_post BASENAME ]
//  }
// XD corriger_frontiere_periodique interprete corriger_frontiere_periodique 1 The Corriger_frontiere_periodique keyword is mandatory to first define the periodic boundaries, to reorder the faces and eventually fix unaligned nodes of these boundaries. Faces on one side of the periodic domain are put first, then the faces on the opposite side, in the same order. It must be run in sequential before mesh splitting.
// XD attr domaine chaine domaine 0 Name of domain.
// XD attr bord chaine bord 0 the name of the boundary (which must contain two opposite sides of the domain)
// XD attr direction list direction 1 defines the periodicity direction vector (a vector that points from one node on one side to the opposite node on the other side). This vector must be given if the automatic algorithm fails, that is:NL2 - when the node coordinates are not perfectly periodic NL2 - when the periodic direction is not aligned with the normal vector of the boundary faces
// XD attr fichier_post chaine fichier_post 1 .
Entree& Corriger_frontiere_periodique::interpreter_(Entree& is)
{
  if (nproc() > 1)
    {
      Cerr << "Error in Corriger_frontiere_periodique::interpreter():\n"
           << " this function must be run in sequential before mesh splitting." << finl;
      barrier();
      exit();
    }
  Nom nom_bord;
  ArrOfDouble direction_perio;
  Nom nom_dom, nom_fichier_post;
  Param param(que_suis_je());
  param.ajouter("domaine", &nom_dom, Param::REQUIRED);
  param.ajouter("bord", &nom_bord, Param::REQUIRED);
  param.ajouter("direction", &direction_perio);
  param.ajouter("fichier_post", &nom_fichier_post);
  param.lire_avec_accolades_depuis(is);
  associer_domaine(nom_dom);
  Domaine& dom = domaine();
  const int dim = dom.coord_sommets().dimension(1);
  int direction_perio_set;
  if (direction_perio.size_array() == 0)
    {
      direction_perio_set=0;
      Cerr << "No direction given, searching periodicity direction automatically:" << finl;
      Reordonner_faces_periodiques::chercher_direction_perio(direction_perio, dom, nom_bord);
    }
  else
    {
      direction_perio_set=1;
      if (direction_perio.size_array() != dim)
        {
          Cerr << "Error in Corriger_frontiere_periodique::interpreter: direction should be of size "
               << dim << finl;
          exit();
        }
    }
  Cerr << "Searching and moving periodicity nodes for domain " << nom_dom << " boundary "
       << nom_bord << finl;
  corriger_coordonnees_sommets_perio(dom, nom_bord, direction_perio, nom_fichier_post);
  Bord& bord = dom.zone(0).bord(nom_bord);
  IntTab& faces = bord.faces().les_sommets();
  const double epsilon = Objet_U::precision_geom;
  const int ok = Reordonner_faces_periodiques::reordonner_faces_periodiques(dom, faces, direction_perio, epsilon);
  if (!ok)
    {
      if (direction_perio_set)
        Cerr << "May be you give a wrong vector for the DIRECTION option" << finl;
      else
        Cerr << "May be you could use the DIRECTION option to specify the vector of periodicity" << finl;
      Cerr << "for the keyword:" << finl;
      Cerr << "Corriger_frontiere_periodique { Domaine " << nom_dom << " Bord " << nom_bord << " } " << finl;
      exit();
    }
  return is;
}


// Description:
//  Pour chaque sommet du bord periodique, on cherche son sommet oppose dans la direction
//   + ou - vecteur_perio et dans un rayon search_radius.
//  Parmi le couple de sommets forme, on deplace celui qui se trouve en +vecteur_perio
//  pour le mettre en face de l'autre sommet.
//  Exit en cas d'erreur (si les sommets sont trop eloignes de leur sommet associe)
// Parametre: dom
// Signification: le domaine a modifier. On deplace les coordonnees des sommets periodiques.
// Parametre: nom_bord
// Signification: le nom du bord periodique du domaine a traiter
// Parametre: vecteur_perio
// Signification: un vecteur de taille "dimension" qui contient le delta entre deux sommets
//  periodiques associes (voir methode chercher_direction_perio())
// Parametre: nom_fichier_post
// Signification: si different de "??", on cree un fichier de postraitement contenant les
//  faces du bord initial et un vecteur qui indique le deplacement applique aux sommets.
void Corriger_frontiere_periodique::corriger_coordonnees_sommets_perio(Domaine& dom,
                                                                       const Nom& nom_bord,
                                                                       const ArrOfDouble& vecteur_perio,
                                                                       const Nom& nom_fichier_post)
{
  if (nproc() > 1)
    {
      Cerr << "Error in Reordonner_faces_periodiques::corriger_coordonnees_sommets_perio\n"
           << " this algorithm is sequential (use it before Decouper)" << finl;
      barrier();
      exit();
    }

  Domaine_bord domaine_bord;
  domaine_bord.construire_domaine_bord(dom, nom_bord);
  const ArrOfInt& renum_som = domaine_bord.get_renum_som();
  const DoubleTab& som_bord = domaine_bord.les_sommets();
  Octree_Double octree;
  octree.build_nodes(som_bord, 0 /* do not include virtual nodes */);

  const double epsilon_initial = dom.epsilon();
  if (epsilon_initial == 0.)
    {
      Cerr << "Error in Corriger_frontiere_periodique::corriger_coordonnees_sommets_perio\n"
           << " dom.epsilon = 0." << finl;
      exit();
    }

  int error_flag = 0;

  const int nb_som_bord = som_bord.dimension(0);
  const int dim = som_bord.dimension(1);
  // Un tableau contenant le deplacement applique aux sommets (pour postraitement)
  DoubleTab delta(nb_som_bord, dim);
  // Un tableau pour detecter les erreurs de periodicite: sommets associes plusieurs fois
  ArrOfBit marker(nb_som_bord);
  marker = 0;
  // Tableau temporaire pour l'octree:
  ArrOfInt nodes_list;
  nodes_list.set_smart_resize(1);
  DoubleTab& sommets_src = dom.les_sommets();
  ArrOfDouble coord(dim);
  for (int som = 0; som < nb_som_bord; som++)
    {
      if (marker.testsetbit(som))
        continue; // Sommet deja traite

      // Cherche le sommet oppose dans les deux directions (-1. et +1.)
      double facteur;
      int som2 = -1;
      // Recherche du sommet dans un rayon de plus en plus grand en commencant par epsilon:
      double epsilon = epsilon_initial;
      do
        {
          for (facteur = -1.; facteur < 1.5; facteur += 2.)
            {
              for (int i = 0; i < dim; i++)
                coord[i] = som_bord(som, i) + facteur * vecteur_perio[i];
              octree.search_elements_box(coord, epsilon, nodes_list);
              som2 = octree.search_nodes_close_to(coord, som_bord, nodes_list, epsilon);
              if (som2 >= 0)
                break;
            }
          if (som2 >= 0)
            break;
          // Sommet non trouve, on recommence avec un rayon de recherche plus grand
          epsilon *= 4.;
        }
      while (1);
      if (marker.testsetbit(som2))
        {
          Cerr << "Error in corriger_coordonnees_sommets_perio\n"
               << " Coordinate    " << coord
               << " Closest point [ " << som_bord(som2, 0) << " " << som_bord(som2, 0) << " "
               << ((dim==3)?som_bord(som2, 0):0.) << " ] already used for another point" << finl;
          error_flag = 1;
        }
      else
        {
          // On deplace le sommet vers lequel pointe vecteur_perio:
          const int som_ref = som;
          const int som_deplace = som2;
          // Indice du sommet a deplacer dans le domaine source:
          const int s = renum_som[som_deplace];
          // Deplacement
          for (int i = 0; i < dim; i++)
            {
              double old_x = som_bord(som_deplace, i);
              double new_x = som_bord(som_ref, i) + facteur * vecteur_perio[i];
              sommets_src(s, i) = new_x;
              delta(som_deplace, i) = new_x - old_x;
            }
        }
    }

  if (nom_fichier_post != "??")
    {
      Domaine dom2;
      Cerr << "Writing node displacement into file: " << nom_fichier_post << finl;
      DERIV(Format_Post_base) fichier_post;
      fichier_post.typer("FORMAT_POST_LATA_V1");
      fichier_post.valeur().initialize(nom_fichier_post, 1 /* binaire */, "SIMPLE");
      Format_Post_base& post = fichier_post.valeur();
      post.ecrire_entete(0., 0 /*reprise*/, 1 /* premier post */);
      post.ecrire_domaine(domaine_bord, 1 /* premier_post */);
      post.ecrire_temps(0.);
      Noms unites;
      unites.add("m");
      unites.add("m");
      if (dim==3) unites.add("m");
      Noms compos;
      compos.add("dx");
      compos.add("dy");
      if (dim==3) compos.add("dz");
      post.ecrire_champ(domaine_bord,
                        unites, compos, -1 /* ecrire toutes les composantes */,
                        0., 0., /* temps */
                        "vitesse", domaine_bord.le_nom(), "SOM","vector", delta);
      int fin=1;
      post.finir(fin);
    }
  Cerr << "Max of node displacement (m): " << max_abs_array(delta) << finl;
  if (error_flag)
    {
      Cerr << "Could not correct node coordinates. Aborting." << finl;
      exit();
    }
}
