/****************************************************************************
* Copyright (c) 2025, CEA
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
#include <Reordonner_faces_periodiques.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Octree_Double.h>
#include <Param.h>

namespace
{
inline void message()
{
  Cerr << "You need to use the Corriger_frontiere_periodique keyword on the periodic boundaries." << finl;
  Cerr << "See the reference manual to use this keyword on your data file." << finl;
}

template <typename _SIZE_>
inline void calculer_vecteur_2faces(const DoubleTab_T<_SIZE_>& coord,
                                    const IntTab_T<_SIZE_>& faces,
                                    const _SIZE_ i_face1,
                                    const _SIZE_ i_face2,
                                    ArrOfDouble& vect)
{
  const int nb_som_faces = faces.dimension_int(1);
  const int dim = coord.dimension_int(1);
  assert(vect.size_array() == dim);
  vect = 0.;
  // Calcul du vecteur entre le centre de la face i et le centre de la face i+n
  for (int j = 0; j < nb_som_faces; j++)
    {
      const _SIZE_ sommet1 = faces(i_face1, j),
                   sommet2 = faces(i_face2, j);
      for (int compo = 0; compo < dim; compo++)
        vect[compo] += coord(sommet2, compo) - coord(sommet1, compo);
    }
  vect /= nb_som_faces;
}


template <typename _SIZE_>
double local_norme_vect(const DoubleVect_T<_SIZE_>& dv)
{
  using int_t = _SIZE_;
  double x=0.0;
  for(int_t i=0; i< dv.size_reelle(); i++)
    x += dv(i)*dv(i);
  x = sqrt(x);
  return x;
}

template <typename _SIZE_>
void build_trad_space(const Domaine_32_64<_SIZE_>& domaine, IntTab_T<_SIZE_>& renum)
{
  // If we reach this point, we are already in parallel, and hence we should be in 32b only,
  // never in 64b
  assert(Process::is_parallel());

  const MD_Vector& md_sommets = domaine.les_sommets().get_md_vector();
  Scatter::construire_espace_virtuel_traduction(md_sommets, md_sommets, renum, 1 /* erreurs fatales */);
}

#if INT_is_64_ == 2
template <>
void build_trad_space(const Domaine_32_64<trustIdType>& domaine, IntTab_T<trustIdType>& renum)
{
  Cerr << "Reordonner_faces_periodiques::renum_som_perio() was invoked from a parallel environment with" << finl;
  Cerr << "a 64b object. Did you use 'Domaine_64' keyword instead of 'Domaine' after a Scatter keyword??" << finl;
  Process::exit(-1);
}
#endif

}

template<typename _SIZE_>
void Reordonner_faces_periodiques_32_64<_SIZE_>::chercher_direction_perio(ArrOfDouble& direction_perio, const Domaine_32_64<_SIZE_>& dom, const Nom& bord)
{
  using DoubleTab_t = DoubleTab_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using ArrOfDouble_t = ArrOfDouble_T<_SIZE_>;
  using Frontiere_t = Frontiere_32_64<_SIZE_>;

  const DoubleTab_t& sommets = dom.coord_sommets();
  const int dim = static_cast<int>(sommets.dimension(1));
  direction_perio.resize_array(dim);
  direction_perio = 0.;
  const Frontiere_t& front = dom.frontiere(bord);
  const int_t nb_faces = front.nb_faces();
  if (nb_faces == 0)
    return;
  const IntTab_t& faces = front.faces().les_sommets();
  const int nb_som_face = static_cast<int>(faces.dimension(1));
  DoubleTab_t normale(1, dim);
  IntTab_t une_face(1, nb_som_face);
  for (int i = 0; i < nb_som_face; i++)
    une_face(0, i) = faces(0, i);
  dom.type_elem()->calculer_normales(une_face, normale);
  normale /= local_norme_vect<_SIZE_>(normale);

  ArrOfDouble_t delta(nb_faces);
  ArrOfDouble vect(dim);
  for (int_t i = 1; i < nb_faces; i++)
    {
      calculer_vecteur_2faces<_SIZE_>(sommets, faces, 0, i, vect);
      double x = 0.;
      for (int j = 0; j < dim; j++)
        x += vect[j] * normale(0,j);
      delta[i] = x;
    }
  const double min = min_array(delta);
  const double max = max_array(delta);
  double facteur = (std::fabs(min) > std::fabs(max)) ? min : max;
  for (int i = 0; i < dim; i++)
    direction_perio[i] = normale(0, i) * facteur;
  Cerr << "Periodicity direction for " << dom.le_nom() << "/" << bord << " " << direction_perio;
}

/*! @brief Reordonne le tableau "faces" selon la convention des faces periodiques: D'abord les faces d'une extremite, puis dans le meme ordre, les faces jumelles.
 *
 *   Attention, l'algorithme est en n carre (lent), et ne fonctionne qu'en sequentiel.
 *
 * @param (domaine) le domaine a laquelle appartiennent les faces
 * @param (direction_perio) le vecteur qui separe le centre d'une face au centre de la face opposee
 * @param (faces) le tableau des faces (pour chaque face, indices de ses sommets) a reordonner Valeur de retour: 1 si ok, 0 si on n'a pas trouve de face jumelle a une face a precision_geom pres.
 */
template<typename _SIZE_>
int Reordonner_faces_periodiques_32_64<_SIZE_>::reordonner_faces_periodiques(const Domaine_32_64<_SIZE_>& domaine,
                                                                             IntTab_T<_SIZE_>& faces,
                                                                             const ArrOfDouble& direction_perio,
                                                                             const double epsilon)
{
  // Modif B.M. 04/06/2010: j'autorise l'operation en parallele car c'est utilise par
  // l'interprete MaillerParallel...
  // PL 18/11/2010: Je deplace neanmoins l'interdiction de l'utilisation de l'interprete en // dans le jeu de donnees (voir ::interpreter_)
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;
  using Octree_Double_t = Octree_Double_32_64<_SIZE_>;

  const int_t nb_faces = faces.dimension(0);
  const int nb_som_faces = static_cast<int>(faces.dimension(1));
  const int dim = static_cast<int>(domaine.les_sommets().dimension(1));
  // Calcul des coordonnees des centres des faces:
  DoubleTab_t centres(nb_faces, 3);
  {
    const DoubleTab_t& coord = domaine.les_sommets();
    const double inv_nb_som = 1. / (double) nb_som_faces;
    for (int_t i = 0; i < nb_faces; i++)
      {
        for (int j = 0; j < nb_som_faces; j++)
          {
            const int_t sommet = faces(i, j);
            for (int k = 0; k < dim; k++)
              centres(i, k) += coord(sommet, k) * inv_nb_som;
          }
      }
  }

  // Construction d'un octree contenant les centres des faces:
  Octree_Double_t octree;
  octree.build_nodes(centres, 0 /* do not include virtual nodes */);

  // Pour chaque face, on cherche sa face periodique associee (dont le centre
  // est decale de direction_perio).

  // Pour chaque face, son nouvel indice dans le tableau des faces
  ArrOfInt_t renum_faces(nb_faces);
  renum_faces= -1;
  ArrOfInt_t nodes_list;

  ArrOfDouble coord(dim);
  int count = 0;
  for (int_t i_face = 0; i_face < nb_faces; i_face++)
    {
      if (renum_faces[i_face] >= 0)
        continue; // Face deja traitee, on passe
      // Cherche la face opposee dans les deux directions (-1. et +1.)
      double facteur;
      int_t i_face2 = -1;
      for (facteur = -1.; facteur < 1.5; facteur += 2.)
        {
          for (int i = 0; i < dim; i++)
            coord[i] = centres(i_face, i) + facteur * direction_perio[i];
          octree.search_elements_box(coord, epsilon, nodes_list);
          i_face2 = octree.search_nodes_close_to(coord, centres, nodes_list, epsilon);
          if (i_face2 >= 0)
            break;
        }
      if (i_face2 >= 0)
        {
          if (renum_faces[i_face2] >= 0)
            {
              Cerr << "====================================================" << finl;
              Cerr << "Error in reordonner_faces_periodiques: the face " << i_face
                   << " of " << centres(i_face,0) << " " << centres(i_face,1) << " "
                   << ((dim==3)?centres(i_face,2):0.)
                   << " center already has a face twin."
                   << finl;
              Cerr << "Possible problem: the boundary is not periodic. Check your mesh." << finl;
              return 0;
            }
          int_t f0 = (facteur > 0.) ? i_face : i_face2;
          int_t f1 = (facteur > 0.) ? i_face2: i_face;
          renum_faces[f0] = count;
          renum_faces[f1] = count + nb_faces / 2;
          count++;
        }
      else
        {
          Cerr << "====================================================" << finl;
          Cerr << "Error in reordonner_faces_periodiques: the face " << i_face << " of " << centres(i_face,0) << " " << centres(i_face,1) << " "
               << ((dim==3)?centres(i_face,2):0.) << finl;
          Cerr << "center has no face twin into the specified direction in the list of faces." << finl;
          return 0;
        }
    }
  // Reordonner les faces:
  const IntTab_t oldfaces(faces);
  for (int_t i = 0; i < nb_faces; i++)
    {
      const int_t new_i = renum_faces[i];
      for (int j = 0; j < nb_som_faces; j++)
        faces(new_i, j) = oldfaces(i, j);
    }
  return 1;
}

/*! @brief essaie de verifier si les faces du bord num_bord sont ordonnees suivant la convention des faces periodiques.
 *
 * On stocke dans vecteur_delta la direction de periodicite presumee (intervalle
 *  mesure entre la premiere face et la face jumelle), et erreur le max de l'erreur par rapport a cette
 *  mesure pour les autres faces.
 *  En parallele, l'erreur est le max sur tous les processeurs
 *  Valeur de retour: 1 si ok, 0 si l'erreur est superieure a precision_geom.
 *
 */
template <typename _SIZE_>
int Reordonner_faces_periodiques_32_64<_SIZE_>::check_faces_periodiques(const Frontiere_32_64<_SIZE_>& frontiere,
                                                                        ArrOfDouble& vecteur_delta, ArrOfDouble& erreur,
                                                                        bool verbose)
{
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;

  const int dim = Objet_U::dimension;
  vecteur_delta.resize_array(dim);
  vecteur_delta = 0.;
  erreur.resize_array(dim);
  erreur = 0.;

  if (verbose && Process::je_suis_maitre())
    Cerr << "Check periodic faces to the boundary : " << frontiere.le_nom() << finl;

  const IntTab_t& faces = frontiere.faces().les_sommets();
  const int_t nb_faces = faces.dimension(0);
  if (nb_faces % 2 != 0)
    {
      Cerr << "Error in Check_faces_periodiques to the boundary " << frontiere.le_nom()
           << "\n The number of faces is odd : " << nb_faces << finl;
      Cerr << "You probably forgot to define periodicity on some boundaries during partition:" << finl;
      Cerr << "Partition domain { ... periodique 1 " << frontiere.le_nom() << " }" << finl;
      Process::Process::exit();
    }
  const int_t n = nb_faces / 2;
  const DoubleTab_t coord = frontiere.domaine().les_sommets();

  int i;
  // Calculer un vecteur delta (tous les procs n'ont pas forcement des faces de ce bord)
  vecteur_delta = -1.e37;
  if (n > 0)
    calculer_vecteur_2faces<_SIZE_>(coord, faces, 0, n, vecteur_delta);
  for (i = 0; i < dim; i++)
    vecteur_delta[i] = Process::mp_max(vecteur_delta[i]);

  // Calculer pour chaque face l'erreur par rapport a ce vecteur delta.
  ArrOfDouble vect(dim);
  for (i = 0; i < n; i++)
    {
      calculer_vecteur_2faces<_SIZE_>(coord, faces, i, i+n, vect);
      // Calcul de la difference entre vect et vecteur_delta:
      for (int compo = 0; compo < dim; compo++)
        erreur[compo] = std::max(erreur[compo], std::fabs(vecteur_delta[compo] - vect[compo]));
    }
  double maxerr = 0.;
  // Calcul du max sur tous les procs:
  for (i = 0; i < dim; i++)
    {
      erreur[i] = Process::mp_max(erreur[i]);
      maxerr = std::max(maxerr, erreur[i]);
    }

  if (verbose && Process::je_suis_maitre())
    {
      Cerr << " Delta vector = [ ";
      for (i = 0; i < dim; i++) Cerr << vecteur_delta[i] << " ";
      Cerr << "]  error = [ ";
      for (i = 0; i < dim; i++) Cerr << erreur[i] << " ";
      Cerr << "]" << finl;
    }
  if (!(maxerr < Objet_U::precision_geom))
    {
      if (Process::je_suis_maitre())
        {
          Cerr << " This boundary is not detected as periodic (geometric error > precision_geom)" << finl;
          message();
          if (Process::is_parallel()) Cerr << "Or you forgot to define the periodic boundary in the Decouper keyword." << finl;
        } // attendre qu'on ait ecrit pour continuer (sinon risque de exit() avant d'avoir affiche le message)
      Process::barrier();
      return 0;
    }
  return 1;
}

template<typename _SIZE_>
void Reordonner_faces_periodiques_32_64<_SIZE_>::renum_som_perio(const Domaine_32_64<_SIZE_>& domaine, const Noms& liste_bords_periodiques,
                                                                 ArrOfInt_T<_SIZE_>& renum_som_perio, bool calculer_espace_virtuel)
{
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;

  const int_t nb_som = domaine.nb_som();
  IntTab_t renum(nb_som);
  for (int_t i = 0; i < nb_som; i++)
    renum[i] = renum_som_perio[i];

  const DoubleTab_t coord = domaine.coord_sommets();
  const int dim = coord.dimension_int(1);

  // Etape 1: pour chaque sommet reel, trouver un sommet associe (si plusieurs directions
  //  de periodicite, un sommet peut etre associe a plusieurs autres).
  for (auto& itr : liste_bords_periodiques)
    {
      const Nom& nom_bord = itr;
      const Frontiere_32_64<_SIZE_>& front = domaine.bord(nom_bord);
      // Direction periodique de ce bord:
      ArrOfDouble delta;
      ArrOfDouble erreur;
      if (!check_faces_periodiques(front, delta, erreur, true /* verbose */))
        Process::exit();
      // Tableau pointant vers tous les sommets de toutes les faces
      // (on cast le IntTab en ArrOfInt)
      const IntTab_t& faces_sommets = front.les_sommets_des_faces();
      const int nb_som_face = faces_sommets.dimension_int(1);
      const int_t nb_faces = faces_sommets.dimension(0) / 2;
      // Boucle sur les faces d'un cote du domaine (premiere moitie des faces)
      for (int_t i_face = 0; i_face < nb_faces; i_face++)
        {
          for (int i_som = 0; i_som < nb_som_face; i_som++)
            {
              const int_t sommet = faces_sommets(i_face, i_som);

              // Trouver le sommet associe
              // Comme les frontieres sont ordonnees (voir check_faces_periodiques),
              // le sommet est forcement un des sommets de la face opposee.
              // Le vecteur qui va de "sommet" a "sommet_oppose" doit etre egal a "delta"
              // la direction de periodicite.
              const int_t i_face_opposee = i_face + nb_faces;
              int_t sommet_opp = -1;
              int i_som_opp = 0;
              for (; i_som_opp < nb_som_face; i_som_opp++)
                {
                  sommet_opp = faces_sommets(i_face_opposee, i_som_opp);
                  int i = 0;
                  for (; i < dim; i++)
                    {
                      double epsilon = std::fabs((coord(sommet_opp, i) - coord(sommet, i)) - delta[i]);
                      if (epsilon > Objet_U::precision_geom)
                        break;
                    }
                  // On a trouve le sommet oppose
                  if (i == dim) break;
                }
              if (i_som_opp >= nb_som_face)
                {
                  Cerr << "[PE" << Process::me() << "] Error in Reordonner_faces_periodiques::renum_som_perio\n"
                       << " An opposite node has not been found\n"
                       << " Boundary " << nom_bord << "\n Face 1: " << i_face << "\n Node: " << sommet << finl;
                  Cerr << " May be you should define the periodic boundary " << nom_bord << finl;
                  message();
                  Process::exit();
                }
              renum[sommet_opp] = sommet;
            }
        }
    }

  // Deuxieme etape: faire pointer tous les sommets periodiques lies entre eux vers le meme sommet

  for (int_t i = 0; i < nb_som; i++)
    {
      int_t j = renum[i];
      // Parcourir les sommet relies pour cette chaine:
      while (j != renum[j])
        j = renum[j];
      renum[i] = j;
    }
  // Calcul des valeurs pour les sommets virtuels.
  // Les sommets opposes aux sommets virtuels doivent etre connus, donc erreurs fatales.
  if (Process::is_parallel() && calculer_espace_virtuel)
    ::build_trad_space(domaine, renum);

  // Recopie du resultat dans le tableau renum_som_perio
  assert(renum.dimension_tot(0) == domaine.nb_som_tot());
  renum_som_perio = renum;
}

// Explicit instanciations
template class Reordonner_faces_periodiques_32_64<int>;
#if INT_is_64_ == 2
template class Reordonner_faces_periodiques_32_64<trustIdType>;
#endif

