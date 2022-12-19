/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Connectivite_som_elem.h>
#include <EcrFicCollecteBin.h>
#include <Static_Int_Lists.h>
#include <Elem_geom_base.h>
#include <Poly_geom_base.h>
#include <communications.h>
#include <NettoieNoeuds.h>
#include <Faces_builder.h>
#include <Zone.h>
#include <Scatter.h>
#include <Faces2.h>
#include <stdio.h>
#include <vector>
#include <array>
#include <map>

Faces_builder::Faces_builder() :
  les_elements_ptr_(0),
  connectivite_som_elem_ptr_(0),
  is_polyedre_(-1)
{
}

void Faces_builder::reset()
{
  les_elements_ptr_ = 0;
  connectivite_som_elem_ptr_ = 0;
  faces_element_reference_old_.reset();
  ref_zone_.reset();
  faces_sommets_.reset();
  face_elem_.reset();
}

/*! @brief A partir de la description des elements de la zone et des frontieres (bords, raccords, faces internes et joints) :
 *
 *   Remplissage des structures suivantes:
 *   - pour les frontieres de la zone: fixer_num_premiere_face
 *   - les_faces.faces_sommets (faces reeles)
 *   - les_faces.faces_voisins (faces reeles)
 *   - elem_faces              (pour les faces reeles des elements reels)
 *        (on initialise elem_faces de taille nb_elem_reels, nb_faces_par_elem)
 *   - joints.items_communs(FACE)
 *
 */
void Faces_builder::creer_faces_reeles(Zone& zone,
                                       const Static_Int_Lists& connect_som_elem,
                                       Faces&   les_faces,
                                       IntTab& elem_faces)
{
  les_elements_ptr_ = & zone.les_elems();

  connectivite_som_elem_ptr_ = & connect_som_elem;
  // La connectivite doit contenir les sommets virtuels
  assert(connect_som_elem.get_nb_lists() == zone.nb_som_tot());

  // Remplissage du tableau des faces de l'element de reference

  is_polyedre_=0;
  if (sub_type(Poly_geom_base,zone.type_elem().valeur()))
    {
      is_polyedre_=1;
    }
  else
    zone.type_elem().valeur().get_tab_faces_sommets_locaux(faces_element_reference_old_);
  // Tableau de taille (nb_faces, nb_sommets par face),
  // pour chaque face, les indices de ses sommets dans le domaine.
  // L'ordre des sommets est celui donne par l'element de reference,
  // pour celui des elements voisins de la face qui a le plus petit
  // indice.
  IntTab& faces_sommets = les_faces.les_sommets();

  // Tableau de taille (nb_faces, 2) contenant pour chaque face
  // les indices des deux elements voisins. Si "i_face" a un seul voisin,
  // faces_voisins_(i_face, 1) = -1;
  IntTab& faces_voisins = les_faces.voisins();

  // Initialisation des references utilisees dans check_erreur_faces
  faces_sommets_ = faces_sommets;
  face_elem_ = faces_voisins;
  ref_zone_ = zone;

  // Le tableau des faces des elements:
  //  dimension(0) = nombre d'elements,
  //  dimension(1) = nombre de faces par element
  //  elem_faces(i,j) = indice de la face j de l'element i dans les
  //                    tableaux faces_sommets et faces_voisins
  //   (les faces de l'element sont dans l'ordre donne par faces_element_reference)
  //  espaces distants et virtuels appropries pour les elements
  const int nb_elements          = les_elements().dimension(0);
  const int nb_faces_par_element = faces_element_reference(0).dimension(0) ? faces_element_reference(0).dimension(0) : 4;
  elem_faces.resize(nb_elements, nb_faces_par_element);
  elem_faces = -1;

  const int nb_sommets_par_face = faces_element_reference(0).dimension(0) ? faces_element_reference(0).dimension(1) : 3;
  // On ajoute chaque face avec resize(n+1,...), donc smart_resize:
  // Calcul du nombre theorique de faces:
  const int nb_faces_front = zone.nb_faces_frontiere() + zone.nb_faces_joint();
  int nb_faces_prevision = (nb_elements * nb_faces_par_element + nb_faces_front) / 2;
  if (is_polyedre_)
    {
      // les faces sont toutes deja connues....
      const Poly_geom_base& poly=ref_cast(Poly_geom_base,ref_zone_->type_elem().valeur());
      nb_faces_prevision=(poly.get_somme_nb_faces_elem()+ nb_faces_front) / 2;;
    }
  // Allocation memoire pour le nombre de faces prevu pour eviter de reallouer
  // de la memoire n fois (voir set_smart_resize)
  faces_sommets.set_smart_resize(1);
  faces_sommets.resize(nb_faces_prevision, nb_sommets_par_face);
  faces_sommets.resize(0, nb_sommets_par_face);
  faces_voisins.set_smart_resize(1);
  faces_voisins.resize(nb_faces_prevision, 2);
  faces_voisins.resize(0, 2);

  // ******** Traitement des frontieres **********
  //  attention, on initialise "num_premiere_face" pour les frontieres !

  // Creation des faces de bord
  {
    Bords& bords = zone.faces_bord();
    const int n = bords.size();
    for (int i = 0; i < n; i++)
      {
        Frontiere& frontiere = bords[i];
        creer_faces_frontiere(1, /* un element voisin par face */
                              frontiere,
                              faces_sommets,
                              faces_voisins,
                              elem_faces);
      }
  }
  // Raccords
  {
    Raccords& raccords = zone.faces_raccord();
    const int n = raccords.size();
    for (int i = 0; i < n; i++)
      {
        Frontiere& frontiere = raccords[i].valeur();
        creer_faces_frontiere(1, /* un element voisin par face */
                              frontiere,
                              faces_sommets,
                              faces_voisins,
                              elem_faces);
      }
  }

  // Faces de bord "internes"
  {
    Faces_Internes& faces_int = zone.faces_int();
    const int n = faces_int.size();
    for (int i = 0; i < n; i++)
      {
        Frontiere& frontiere = faces_int[i];
        creer_faces_frontiere(2, /* deux elements voisin par face */
                              frontiere,
                              faces_sommets,
                              faces_voisins,
                              elem_faces);
      }

    // On duplique les faces internes : pour chaque face qui a deux
    // voisins, on cree une deuxieme face identique avec le deuxieme voisin,
    // on efface le deuxieme voisin de la face d'origine et on change
    // la face voisins de deuxieme voisin:
    if (n > 0)
      {
        Cerr << "Faces_builder::creer_faces_reeles not coded for the internal faces of boundary" << finl;
        Process::exit();
        // A faire selon l'ancienne version de zone2... et a tester !
      }
  }

  // Faces de joint
  {
    Joints& joints = zone.faces_joint();
    const int n = joints.size();
    for (int i = 0; i < n; i++)
      {
        Frontiere& frontiere = joints[i];
        creer_faces_frontiere(2, /* elements voisins par face */
                              frontiere,
                              faces_sommets,
                              faces_voisins,
                              elem_faces);
        // Remplissage de items_communs(FACE)
        // Les faces de joint sont dans le meme ordre en local et sur le voisin.
        Joint& joint = joints[i];
        ArrOfInt& indices_faces =
          joint.set_joint_item(Joint::FACE).set_items_communs();
        const int nb_faces  = joint.nb_faces();
        indices_faces.resize_array(nb_faces);
        const int num_premiere_face = joint.num_premiere_face();
        for (int i2 = 0; i2 < nb_faces; i2++)
          indices_faces[i2] = num_premiere_face + i2;
      }
  }

  // *********************************************
  // Faces internes

  creer_faces_internes(faces_sommets,
                       elem_faces,
                       faces_voisins);

  // *********************************************
  // C'est fini: on verifie qu'on a bien le nombre de faces prevu
  if (faces_sommets.dimension(0) != nb_faces_prevision)
    {
      Cerr << "Error in Faces_builder::creer_faces_reeles:\n"
           << " number of faces does not match predicted number of faces.\n"
           << " (problem with faces_internes ?)" << finl;
      Process::exit();
    }

  // RAZ attribut smart_resize des tableaux faces_sommets et faces_voisins.
  faces_sommets.set_smart_resize(0);
  faces_voisins.set_smart_resize(0);
  // RAZ des attributs de la classe
  reset();
}

/*! @brief methode outil pour creer_faces_frontiere et creer_faces_internes (si liste non vide sur au moins un processeur, affiche un message et exit()).
 *
 */
void Faces_builder::check_erreur_faces(const char * message,
                                       const ArrOfInt& liste_faces) const
{
  const int nmax = 100;
  int n = liste_faces.size_array();
  if (n > 0)
    {
      Cerr << "==========================" << finl;
      Cerr << "Error!" << finl << message
           << "\nSee log file of this PE for detailed info."
           << finl;
      Sortie& J = Process::Journal();
      J <<  "Error in Faces_builder::creer_faces_*\n"
        << message << finl;
      if (n > nmax)
        {
          J << "Too many faces to display (" << n << ") display only " << nmax << " first faces" << finl;
          n = nmax;
        }
      int i;
      J << "Display format:\n"
        << " facenumber = face index in faces_sommet array\n"
        << " som1..som4 = node index\n"
        << " elem1 elem2 = neighbouring element number\n"
        << "facenumber som1 (x1 y1 z1) som2 (x2 y2 z2) [som3 (x3 y3 z3)...] elem1 elem2" << finl;
      char s[1000];
      const DoubleTab& coord = ref_zone_.valeur().domaine().coord_sommets();
      const IntTab&     faces = faces_sommets_.valeur();
      const IntTab&     face_elem = face_elem_.valeur();
      const int dim = Objet_U::dimension;
      const int nb_som_faces = faces.dimension(1);
      for (i = 0; i < n; i++)
        {
          char *sptr = s;
          const int iface = liste_faces[i];
          sptr += snprintf(sptr, 100, "%4ld ",(long) iface);
          for (int j = 0; j < nb_som_faces; j++)
            {
              const int isom = faces(iface,j);
              sptr += snprintf(sptr, 100, "%5ld(", (long)isom);
              for (int k = 0; k < dim; k++)
                if (isom!=-1)
                  sptr += snprintf(sptr, 100, "%10.6f", coord(isom, k));
              sptr += snprintf(sptr, 100, ")");
            }
          sptr += snprintf(sptr, 100, "%4ld %4ld", (long)face_elem(iface,0),(long) face_elem(iface,1));
          J << s << finl;
        }
      NettoieNoeuds::verifie_noeuds(ref_zone_.valeur().domaine());
      Process::exit();
    }
}

/*! @brief ajoute une face reelle dans faces_sommets et faces_voisins.
 *
 */
int Faces_builder::ajouter_une_face(const ArrOfInt& une_face,
                                    const int elem0,
                                    const int elem1,
                                    IntTab& faces_sommets,
                                    IntTab& faces_voisins)
{
  int i;
  const int num_new_face        = faces_sommets.dimension(0);
  const int nb_sommets_par_face = faces_sommets.dimension(1);
  const int new_size = num_new_face + 1;

  assert(une_face.size_array() == nb_sommets_par_face);
  faces_sommets.resize(new_size, nb_sommets_par_face);
  for (i = 0; i < nb_sommets_par_face; i++)
    faces_sommets(num_new_face, i) = une_face[i];

  faces_voisins.resize(new_size, 2);
  faces_voisins(num_new_face, 0) = elem0;
  faces_voisins(num_new_face, 1) = elem1;

  return num_new_face;
}

int Faces_builder::chercher_face_element(const IntTab&    elem_som,
                                         const IntTab&    faces_element_ref,
                                         const ArrOfInt& une_face,
                                         const int     elem)
{
#ifdef old
#ifndef NDEBUG
  ArrOfInt sommets_element(8);
#else
  int sommets_element[8];
#endif
#endif

  const int nb_faces_element          = faces_element_ref.dimension(0);
  const int nb_sommets_par_face       = faces_element_ref.dimension(1);

  int i_face, i_som, i_som2;
  for (i_face = 0; i_face < nb_faces_element; i_face++)
    {
      for (i_som = 0; i_som < nb_sommets_par_face; i_som++)
        {
          const int sommet_elem_ref = faces_element_ref(i_face, i_som);
          int sommet_domaine ;
          if (sommet_elem_ref==-1)
            sommet_domaine=-1;
          else
            sommet_domaine = elem_som(elem, sommet_elem_ref);
          for (i_som2 = 0; i_som2 < nb_sommets_par_face; i_som2++)
            if (une_face[i_som2] == sommet_domaine) // si sommet trouve, stop
              break;
          if (i_som2 == nb_sommets_par_face) // si sommet non trouve, stop
            break;
        }
      if (i_som == nb_sommets_par_face) // si tous les sommets ont ete trouves, stop
        break;
    }
  if (i_face == nb_faces_element) // si face non trouvee
    return -1;
  else
    return i_face;
}
const IntTab& Faces_builder::faces_element_reference(int elem) const
{
  if (is_polyedre_==1 && les_elements_ptr_->dimension(0))
    {
      assert(is_polyedre_==1);
      const Poly_geom_base& poly=ref_cast(Poly_geom_base,ref_zone_->type_elem().valeur());
      IntTab& elem_ref_mod=ref_cast_non_const(IntTab,faces_element_reference_old_);
      poly.get_tab_faces_sommets_locaux(elem_ref_mod,elem);

      //abort();
      //return faces_element_reference(0);
    }
  return faces_element_reference_old_;
}


/*! @brief Methode outil: on suppose que "une_face" contient les indices des sommets d'une face de l'element d'indice "elem" dans la zone.
 *
 *   On cherche quel est le numero de cette face sur l'element
 *   de reference. Si les sommets ne correspondent a aucune face de
 *   l'element, on renvoie -1.
 *
 */
int Faces_builder::chercher_face_element(const ArrOfInt& une_face,
                                         const int     elem) const
{
  const IntTab& elem_som                = les_elements();
  const IntTab& faces_element_ref       = faces_element_reference(elem);
  int i_face = chercher_face_element(elem_som, faces_element_ref, une_face, elem);
  return i_face;
}

/*! @brief Insere les faces de la frontiere donnee dans les trois tableaux, a la suite des faces deja presentes dans faces_sommets.
 *
 *   Remplissage de :
 *    frontiere.num_premiere_face
 *   Completion de :
 *    faces_sommets
 *    elem_faces
 *    faces_voisins
 *
 */
void Faces_builder::creer_faces_frontiere(const int nb_voisins_attendus,
                                          Frontiere&   frontiere,
                                          IntTab& faces_sommets,
                                          IntTab& faces_voisins,
                                          IntTab& elem_faces) const
{
  assert(nb_voisins_attendus == 1 || nb_voisins_attendus == 2);

  const Static_Int_Lists& som_elem   = connectivite_som_elem();
  const int   nb_sommets_par_face  = faces_element_reference(0).dimension(0) ? faces_element_reference(0).dimension(1) : 3;
  const int   num_premiere_face    = faces_sommets.dimension(0);
  const int   nb_elem_reels        = elem_faces.dimension(0);
  frontiere.fixer_num_premiere_face(num_premiere_face);

  const Faces&   faces_frontiere  = frontiere.faces();
  const IntTab& sommets_faces_fr = faces_frontiere.les_sommets();
  const int   nb_faces         = faces_frontiere.nb_faces();
  ArrOfInt       une_face(nb_sommets_par_face);
  ArrOfInt       voisins;
  voisins.set_smart_resize(1);
  ArrOfInt liste_faces_erreur0;
  liste_faces_erreur0.set_smart_resize(1);
  ArrOfInt liste_faces_erreur1;
  liste_faces_erreur1.set_smart_resize(1);
  ArrOfInt liste_faces_erreur2;
  liste_faces_erreur2.set_smart_resize(1);
  ArrOfInt liste_faces_erreur3;
  liste_faces_erreur3.set_smart_resize(1);

  int i_face;
  for (i_face = 0; i_face < nb_faces; i_face++)
    {
      {
        int nb_sommets_par_face_fr=sommets_faces_fr.dimension(1);
        for (int i = 0; i < std::min(nb_sommets_par_face, nb_sommets_par_face_fr); i++)
          une_face[i] = sommets_faces_fr(i_face, i);
        for (int i = std::min(nb_sommets_par_face, nb_sommets_par_face_fr); i < nb_sommets_par_face; i++)
          une_face[i] = -1;
      }
      // Quels sont les elements voisins de cette face ?
      find_adjacent_elements(som_elem, une_face, voisins);
      const int nb_voisins = voisins.size_array();
      const int elem0 = (nb_voisins > 0) ? voisins[0] : -1;
      const int elem1 = (nb_voisins > 1) ? voisins[1] : -1;
      const int indice_face =
        ajouter_une_face(une_face, elem0, elem1, faces_sommets, faces_voisins);

      switch(nb_voisins)
        {
        case 0:
          {
            // Erreur: la face n'a pas de voisin
            liste_faces_erreur0.append_array(indice_face);
            break;
          }
        case 1:
        case 2:
          {
            if (nb_voisins_attendus == nb_voisins)
              {
                int i_voisin;
                for (i_voisin = 0; i_voisin < nb_voisins; i_voisin++)
                  {
                    const int elem = voisins[i_voisin];
                    // Quelle est la face de l'element ?
                    const int i_face_elem = chercher_face_element(une_face, elem);
                    if (i_face_elem >= 0)
                      {
                        // Si c'est un element reel, on associe la face
                        if (elem < nb_elem_reels)
                          {
                            if (elem_faces(elem, i_face_elem) < 0)
                              {
                                elem_faces(elem, i_face_elem) = indice_face;
                              }
                            else
                              {
                                // Erreur: cette face existe deja (dans cette frontiere ou une autre)
                                liste_faces_erreur3.append_array(indice_face);
                              }
                          }
                      }
                    else
                      {
                        // Erreur: la face n'est pas une face de l'element.
                        liste_faces_erreur0.append_array(indice_face);
                      }
                  }
              }
            else
              {
                // Erreur, on attendait pas ce nombre de voisins.
                liste_faces_erreur1.append_array(indice_face);
              }
            break;
          }
        default:
          // Erreur, plus de deux voisins, c'est n'importe quoi...
          liste_faces_erreur2.append_array(indice_face);
        }
    }
  Nom msg;
  msg = "Boundary \"";
  msg += frontiere.le_nom();
  msg += "\" contains faces which do not belong to any element.";
  check_erreur_faces(msg, liste_faces_erreur0);

  msg = "Boundary \"";
  msg += frontiere.le_nom();
  msg += "\" contains faces that belong to ";
  msg += Nom(3-nb_voisins_attendus);
  msg += " elements.\n";
  switch(nb_voisins_attendus)
    {
    case 1:
      msg += "These faces should have only 1 neighbouring element.";
      break;
    case 2:
      msg += "These faces should have 2 neighbouring elements.";
      break;
    default:
      msg = "Internal error.";
    }
  if (sub_type(Joint, frontiere))
    {
      // Deux sources d'erreur possibles: les faces de joint sont fausses
      // ou bien la zone ne contient pas les elements virtuels (il faut
      // au moins que la zone contienne les elements virtuels voisins des
      // faces de joint).
      msg += "(Error in a Joint object: internal error in the mesh splitter or scatter ? )\n";
    }
  check_erreur_faces(msg, liste_faces_erreur1);

  msg = "Boundary \"";
  msg += frontiere.le_nom();
  msg += "\" contains faces that belong to more than 2 elements.\n";
  check_erreur_faces(msg, liste_faces_erreur2);

  msg = "Boundary \"";
  msg += frontiere.le_nom();
  msg += "\" contains faces that already exist in another boundary or in this one.\n";
  check_erreur_faces(msg, liste_faces_erreur3);
}

/*! @brief Construction des faces interieures au domaine (faces qui ont deux voisins et qui ne sont pas des "faces_bord_internes")
 *
 *   Les faces de joint ont deja ete creees.
 *
 */
void Faces_builder::creer_faces_internes(IntTab& faces_sommets,
                                         IntTab& elem_faces,
                                         IntTab& faces_voisins) const
{
  const IntTab& elem_som             = les_elements();
  const Static_Int_Lists& som_elem   = connectivite_som_elem();
  //  const IntTab & faces_elem_ref       = faces_element_reference();
  const int   nb_elem              = elem_som.dimension(0);
  const int   nb_faces_par_element = faces_element_reference(0).dimension(0);
  const int   nb_sommets_par_face  = nb_faces_par_element ? faces_element_reference(0).dimension(1) : 3;

  // Tableau temporaire dans lequel on stocke les indices des sommets
  // de la face en cours de traitement
  ArrOfInt une_face(nb_sommets_par_face);
  // Tableau temporaire (liste des elements voisins d'une face)
  ArrOfInt voisins;
  voisins.set_smart_resize(1);
  // Liste des faces n'ayant qu'un seul voisin et qui ne figurent pas
  // dans les faces de bord (ce sont des erreurs):
  ArrOfInt liste_faces_frontiere_non_declarees;
  liste_faces_frontiere_non_declarees.set_smart_resize(1);
  ArrOfInt liste_faces_joint_non_declarees;
  liste_faces_joint_non_declarees.set_smart_resize(1);
  // Liste des faces presentant une erreur de connectivite (plus de
  // deux elements voisins, ou connection a des sommets qui ne
  // sont pas une face de l'element:
  ArrOfInt liste_faces_erreurs_connectivite;
  liste_faces_erreurs_connectivite.set_smart_resize(1);

  // Boucle sur les elements
  int i_elem;
  for (i_elem = 0; i_elem < nb_elem; i_elem++)
    {
      int i_face;
      // Boucle sur les faces de l'element
      for (i_face = 0; i_face < nb_faces_par_element; i_face++)
        {

          // L'indice de cette face dans le tableau faces_sommets.
          // Il vaut -1 si la face n'a pas encore ete creee,
          int indice_face = elem_faces(i_elem, i_face);

          // Calcul des indices des sommets de la face dans le domaine:
          int i;
          // Attention il ne faut laisser l'appel ici...
          const IntTab& faces_elem_ref       = faces_element_reference(i_elem);

          for (i = 0; i < nb_sommets_par_face; i++)
            {
              // indice du sommet sur l'element de reference
              const int i_som_ref = faces_elem_ref(i_face, i);
              // indice du sommet dans le domaine
              if (i_som_ref==-1)
                une_face[i] = -1;
              else
                {
                  const int i_som = elem_som(i_elem, i_som_ref);
                  une_face[i] = i_som;
                }
            }
          if (une_face[0]==-1)
            {
              // on a une face bidon on ne fait rien
              elem_faces(i_elem, i_face) = -1;
            }
          else
            {
              // Recherche des elements voisins de cette face.
              // Le tableau "voisins" est classe dans l'ordre croissant.
              find_adjacent_elements(som_elem, une_face, voisins);

              const int nb_voisins = voisins.size_array();
              assert (nb_voisins > 0); // Il devrait au moins y avoir i_elem !!! (ou alors on a une face constitues de -1);

              if (nb_voisins == 1)   // ***** La face a 1 voisin ********
                {

                  assert(voisins[0] == i_elem); // L'element voisin est forcement i_elem
                  // Une face ayant un seul element voisin est une face de frontiere.
                  if (indice_face >= 0)
                    {
                      // Ok, c'est normal, les frontieres ont deja ete traitees
                    }
                  else
                    {
                      // Erreur: la face n'existe pas encore. Elle devrait avoir ete
                      // creee a partir des frontieres (creer_faces_frontiere)
                      indice_face = ajouter_une_face(une_face, i_elem, -1,
                                                     faces_sommets, faces_voisins);
                      liste_faces_frontiere_non_declarees.append_array(indice_face);
                    }

                }
              else if (nb_voisins == 2)     // ***** La face a 2 voisins ********
                {

                  const int elem0 = voisins[0];
                  const int elem1 = voisins[1];
                  assert(elem0 < elem1);
                  if (indice_face >= 0)
                    {
                      // La face a deje ete creee.
                    }
                  else
                    {
                      // La face n'existe pas encore.
                      if (elem0 == i_elem)
                        {
                          // Les voisins sont classes: elem0 < elem1
                          // donc c'est la premiere fois qu'on parcourt cette face dans la boucle
                          // sur les elements.
                          indice_face = ajouter_une_face(une_face, elem0, elem1,
                                                         faces_sommets, faces_voisins);

                          // Ou est cette face sur l'element voisin ?
                          const int i_face_elem1 = chercher_face_element(une_face, elem1);
                          if (i_face_elem1 >= 0)
                            {
                              if (elem1 < nb_elem) // Element voisin reel ?
                                elem_faces(elem1, i_face_elem1) = indice_face;
                            }
                          else
                            {
                              // Erreur, les sommets de la face sont des sommets de l'element elem1
                              // mais ne sont pas sur une face de cet element. Erreur de
                              // connectivite du maillage.
                              liste_faces_erreurs_connectivite.append_array(indice_face);
                            }
                          if (elem1 >= nb_elem)
                            {
                              // Erreur : le voisin est un element virtuel, cette face
                              // devrait etre dans les faces de joint, donc deja creee.
                              liste_faces_joint_non_declarees.append_array(indice_face);
                            }
                        }
                      else
                        {
                          assert(elem1 == i_elem);
                          indice_face = ajouter_une_face(une_face, elem0, elem1,
                                                         faces_sommets, faces_voisins);
                          // On aurait deja du creer cette face car elle est voisine de elem0
                          // qui est deja traite (indice plus petit). Si on arrive ici,
                          // c'est que les sommets de "une_face" appartiennent bien a l'elem0,
                          // mais qu'ils ne sont pas sur une face de cet element. C'est une
                          // erreur de connectivite.
                          liste_faces_erreurs_connectivite.append_array(indice_face);
                        }
                    }

                }
              else                        // ***** La face a > 2 voisins ********
                {
                  if (indice_face < 0)
                    {
                      const int elem0 = voisins[0];
                      const int elem1 = voisins[1];
                      indice_face = ajouter_une_face(une_face, elem0, elem1,
                                                     faces_sommets, faces_voisins);
                    }
                  liste_faces_erreurs_connectivite.append_array(indice_face);
                }

              // Si la face n'existait pas, on l'a creee et on a mis son indice
              // dans indice_face. Sinon on a trouve l'indice de la face existante.
              assert(indice_face >= 0);
              elem_faces(i_elem, i_face) = indice_face; /* WRITE elem_faces */
            }
        }
    }

  // Traitement des erreurs:
  {
    const char * const msg1 = "We found faces which belong to one element/cell only and are not declared in any boundary ! You forgot to define at least one boundary in your mesh. Fix your mesh.\n";
    const char * const msg2 = "Joint faces are incomplete: internal error in the mesh splitter\n";
    const char * const msg3 = "Connectivity error in the mesh elements. Possible errors:\n- one face of one element belongs to more than 2 elements\n- two element have at least 3 common nodes but these nodes are not faces of these elements\n";
    check_erreur_faces(msg1, liste_faces_frontiere_non_declarees);
    check_erreur_faces(msg2, liste_faces_joint_non_declarees);
    check_erreur_faces(msg3, liste_faces_erreurs_connectivite);
  }
}


