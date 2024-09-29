/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Domaine_VDF.h>
#include <Domaine_Cl_VDF.h>
#include <Rectangle.h>
#include <Hexaedre.h>
#include <Faces_VDF.h>
#include <Aretes.h>
#include <Periodique.h>
#include <Dirichlet_entree_fluide_leaves.h>
#include <Neumann_sortie_libre.h>
#include <EcrFicCollecte.h>
#include <math.h>
#include <TRUSTTrav.h>

Implemente_instanciable(Domaine_VDF,"Domaine_VDF",Domaine_VF);

//#define SORT_POUR_DEBOG

Sortie& Domaine_VDF::printOn(Sortie& os) const
{
  Domaine_VF::printOn(os);
  os << "orientation_" << orientation_ << finl;
  os << "nb_faces_X_" << nb_faces_X_ << finl;
  os << "nb_faces_Y_" << nb_faces_Y_ << finl;
  os << "nb_faces_Z_" << nb_faces_Z_ << finl;
  os << "nb_aretes_" << nb_aretes_ << finl;
  os << "nb_aretes_joint_" << nb_aretes_joint_ << finl;
  os << "nb_aretes_coin_" << nb_aretes_coin_ << finl;
  os << "nb_aretes_bord_" << nb_aretes_bord_ << finl;
  os << "nb_aretes_mixtes_" << nb_aretes_mixtes_ << finl;
  os << "nb_aretes_internes_" << nb_aretes_internes_ << finl;
  Qdm_.ecrit(os << "Qdm : ");
  os << h_x_ << " " << h_y_ << " " << h_z_ << finl;
  return os;
}

Entree& Domaine_VDF::readOn(Entree& is)
{
  Domaine_VF::readOn(is);
  is >> orientation_;
  is >> nb_faces_X_;
  is >> nb_faces_Y_;
  is >> nb_faces_Z_;
  is >> nb_aretes_;
  is >> nb_aretes_joint_;
  is >> nb_aretes_coin_;
  is >> nb_aretes_bord_;
  is >> nb_aretes_mixtes_;
  is >> nb_aretes_internes_;
  Qdm_.lit(is);
  is >> h_x_ >> h_y_ >> h_z_;
  return is;
}
/*! @brief renvoie un Faces_VDF* !
 *
 */
Faces* Domaine_VDF::creer_faces()
{
  Faces_VDF* les_faces_vdf=new(Faces_VDF);
  return les_faces_vdf;
}

/*! @brief Reordonne les faces internes par orientation, on doit mettre a jour tous les tableaux qui dependent des indices de faces, soit:
 *
 *    - faces_sommets
 *    - faces_voisins
 *    - elem_faces
 *    - orientation
 *    - Domaine.faces_joint().items_communs(FACE)
 *
 */

void Domaine_VDF::reordonner(Faces& les_faces)
{
  //  Cerr << "Faces : " << les_faces << finl;
  // Cerr << "On reordonne les faces " << finl;

  // Calcul de l'orientation des faces reeles
  Faces_VDF& les_faces_vdf=ref_cast(Faces_VDF, les_faces);
  les_faces_vdf.calculer_orientation(orientation_, nb_faces_X_,
                                     nb_faces_Y_, nb_faces_Z_);

  Joints&      joints     = domaine().faces_joint();
  const int nb_faces_front = domaine().nb_faces_frontiere();

  // Construction d'un int selon lequel on va trier les faces:
  //  orientation * nb_faces + indice_face
  // Quand on trie par ordre croissant de cet int, on trie selon l'orientation
  // en preservant l'ordre initial des faces de meme orientation
  const int nb_faces = les_faces_vdf.nb_faces();
  int i, j;

  ArrOfInt sort_key(nb_faces);
  // On ne trie pas les faces de bord, qui restent au debut:
  for (i = 0; i < nb_faces_front; i++)
    sort_key[i] = i;

  for (; i < nb_faces; i++)
    {
      const int ori = orientation(i);
      sort_key[i] = ori * nb_faces + i;
    }

  sort_key.ordonne_array();

  // Il suffit de revenir a l'index initial pour avoir les indices des
  // faces triees par orientation : sort_key[nouveau numero] = ancien numero
  for (i = nb_faces_front; i < nb_faces; i++)
    {
      const int key = sort_key[i];
      orientation_[i] = key / nb_faces;
      sort_key[i]    = key % nb_faces;
    }

  // On reordonne les faces:
  {
    IntTab& faces_sommets = les_faces_vdf.les_sommets();
    IntTab old_tab(faces_sommets);
    const int nb_som_faces = faces_sommets.dimension(1);
    for (i = 0; i < nb_faces; i++)
      {
        const int old_i = sort_key[i];
        for (j = 0; j < nb_som_faces; j++)
          faces_sommets(i, j) = old_tab(old_i, j);
      }
  }

  {
    IntTab& faces_voisins = les_faces_vdf.voisins();
    IntTab old_tab(faces_voisins);
    for (i = 0; i < nb_faces; i++)
      {
        const int old_i = sort_key[i];
        faces_voisins(i, 0) = old_tab(old_i, 0);
        faces_voisins(i, 1) = old_tab(old_i, 1);
      }
  }

  // Calcul de la table inversee: reverse_index[ancien_numero] = nouveau numero
  ArrOfInt reverse_index(nb_faces);
  {
    for (i = 0; i < nb_faces; i++)
      {
        const int jj = sort_key[i];
        reverse_index[jj] = i;
      }
  }
  // Renumerotation de elem_faces:
  {
    // Nombre d'indices de faces dans le tableau
    const int nb_items = elem_faces_.size();
    ArrOfInt& array = elem_faces_;
    for (i = 0; i < nb_items; i++)
      {
        const int old = array[i];
        array[i] = reverse_index[old];
      }
  }
  // Mise a jour des indices des faces de joint:
  {
    const int nb_joints = joints.size();
    for (int i_joint = 0; i_joint < nb_joints; i_joint++)
      {
        Joint&     joint         = joints[i_joint];
        ArrOfInt& indices_faces = joint.set_joint_item(JOINT_ITEM::FACE).set_items_communs();
        const int nb_faces_bis    = indices_faces.size_array();
        assert(nb_faces_bis == joint.nb_faces()); // items_communs rempli ?
        for (i = 0; i < nb_faces_bis; i++)
          {
            const int old = indices_faces[i]; // ancien indice local
            indices_faces[i] = reverse_index[old];
          }
        // Les faces de joint ne sont plus consecutives dans le
        // tableau: num_premiere_face n'a plus ne sens
        joint.fixer_num_premiere_face(-1);
      }
  }
  // Mise a jour des indices des groupes de faces:
  Groupes_Faces&      groupes_faces    = domaine().groupes_faces();
  groupes_faces.renumerote(reverse_index);
}

/*! @brief appel a  Domaine_VF::discretiser() calcul des centres de gravite des elements
 *
 *  remplissage des connectivites elements/faces
 *  on reordonne les connectivites faces/elements
 *  de sorte que el1 est a gauche et el2 a droite.
 *  calcul des porosites volumiques et surfaciques
 *  generation des aretes
 *  calcul des pas du maillage
 *
 */
void Domaine_VDF::discretiser()
{
  Domaine_VF::discretiser();

  Domaine& domaine_geom=domaine();

  // Verification de la coherence entre l'element geometrique et la discretisation

  const Elem_geom_base& elem_geom = domaine_geom.type_elem().valeur();

  if (dimension == 2)
    {
      if (!sub_type(Rectangle,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Rectangle et Rectangle_Axi sont compatibles avec la discretisation VDF en dimension 2" << finl;
          exit();
        }
    }
  else if (dimension == 3)
    {
      if (!sub_type(Hexaedre,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Hexaedre ou Hexadre_Axi sont compatibles avec la discretisation VDF en dimension 3" << finl;
          exit();
        }
    }
  // Calcul de l'orientation des faces virtuelles
  // Note BM: pour compatibilite avec la version 1.5, orientation_
  //  n'a pas d'espace virtuel.
  MD_Vector md_nul;
  creer_tableau_faces(orientation_);
  orientation_.echange_espace_virtuel();
  orientation_.set_md_vector(md_nul); // Detache la structure parallele

  // Application de la convention VDF sur face_voisin:
  // L'element face_voisin(i,0) doit avoir une coordonnee "ori" plus petite que la face
  // et l'element face_voisin(i,1) doit avoir une coordonnee plus grande.
  {
    const int nbr_faces_tot = face_voisins_.dimension_tot(0);
    for (int i_face = 0; i_face < nbr_faces_tot; i_face++)
      {
        const int elem0 = face_voisins_(i_face, 0);
        const int elem1 = face_voisins_(i_face, 1);
        const int ori = orientation_[i_face];
        const double x_face = xv(i_face, ori);
        double delta = 0.;
        // L'element 0 doit avoir une coordonnee "ori" plus petite que la face
        // et l'element 1 doit avoir une coordonnee plus grande.
        if (elem0 >= 0)
          {
            delta = x_face - xp(elem0, ori);
          }
        else
          {
            assert(elem1 >= 0); // Sinon, grosse erreur: elements voisins non renseignes
            delta = xp(elem1, ori) - x_face;
          }
        if (delta < 0)
          {
            // On inverse les deux elements
            face_voisins_(i_face, 0) = elem1;
            face_voisins_(i_face, 1) = elem0;
          }
      }
  }

  calculer_volumes_entrelaces();
  genere_aretes();
  calcul_h();
  remplir_face_normales();
  Cerr << "L'objet de type Domaine_VDF a ete rempli avec succes " << finl;

}

void Domaine_VDF::remplir_face_normales()
{
  // On remplit le tableau face_normales_;
  //  Attention : le tableau face_voisins n'est pas exactement un tableau distribue. Une face n'a pas ses deux voisins dans le
  //  meme ordre sur tous les processeurs qui possedent la face.
  //  Donc la normale a la face peut changer de direction d'un processeur a l'autre, y compris pour les faces de joint.
  face_normales_ = xv_; // already has // structure
  face_normales_ = 0.;

  for (int f = 0; f < nb_faces_tot(); f++)
    {
      int ori = orientation(f);
      face_normales_(f,ori) = face_surfaces(f);
    }
}

/*! @brief remplissage des volumes entrelaces
 *
 */
void Domaine_VDF::calculer_volumes_entrelaces()
{
  Cerr << "On calcule les volumes entrelaces" << finl;
  creer_tableau_faces(volumes_entrelaces_);
  volumes_entrelaces_dir_.resize(nb_faces(), 2);
  creer_tableau_faces(volumes_entrelaces_dir_);

  const int nb_faces_front = premiere_face_int();
  const int nbf = nb_faces();
  for (int num_face = 0; num_face<nbf; num_face++)
    {
      const double f = (num_face < nb_faces_front) ? 2. : 1.;
      for (int dir = 0; dir < 2; dir ++)
        {
          int elem = face_voisins_(num_face, dir);
          if (elem != -1)
            {
              if ((axi) && (orientation_[num_face]==0))
                volumes_entrelaces_dir_(num_face, dir) = f * 0.5 * xv_(num_face, 0) * volumes(elem) / xp(elem, 0);
              else if ((bidim_axi) && (orientation_[num_face]==0))
                {
                  const double r1 = xv(num_face, 0);
                  const double r2 = xp(elem, 0);
                  const double dz = dim_elem(elem, 1);
                  const double dr = std::fabs(r1 - r2);
                  const double r = std::min(r1, r2);
                  volumes_entrelaces_dir_(num_face, dir) = 2. * M_PI * (r * dr + 0.5 * dr * dr) * dz;
                }
              else
                volumes_entrelaces_dir_(num_face, dir) = f * 0.5 * volumes(elem);

              volumes_entrelaces_[num_face] += volumes_entrelaces_dir_(num_face, dir);
            }
        }
    }
  volumes_entrelaces_.echange_espace_virtuel();
  volumes_entrelaces_dir_.echange_espace_virtuel();
}

static inline int face_vois(const Domaine_VDF& zvdf, const Domaine& domaine, int face, int i)
{
  int elem=zvdf.face_voisins(face,i);
  if(elem==-1)
    {
      face=domaine.face_bords_interne_conjuguee(face);
      if(face!=-1)
        {
          elem=zvdf.face_voisins(face,i);
          assert(elem!=-1);
        }
    }
  return elem;
}

/*! @brief generation des aretes
 *
 */
void Domaine_VDF::genere_aretes()
{
  Cerr << "On genere les aretes" << finl;

  Domaine& mon_dom=domaine();
  //  int nb_poly = le_dom.nb_elem();
  int nb_poly_tot = mon_dom.nb_elem_tot();
  // Estimation avec majoration du nombre d'aretes : nb_aretes_plus
  nb_aretes_=-1; // cf Aretes::affecter
  int nb_aretes_plus=-1;
  if (dimension == 2)
    nb_aretes_plus = mon_dom.nb_som();
  else if (dimension == 3)
    nb_aretes_plus = mon_dom.nb_som()*3;
  Cerr << "Creation des aretes " << finl;
  Aretes les_aretes(nb_aretes_plus);

  // On balaie les elements :
  int el1, el2, el3, el4;
  int face12, face13, face34, face24;

  int nb_dir;
  if(dimension==2) nb_dir=1;
  else nb_dir=3;
  IntVect gauche(nb_dir);
  gauche(0)=0;
  if(dimension==3)
    {
      gauche(1)=0;
      gauche(2)=1;
    }
  IntVect droite(nb_dir);
  droite(0)=dimension;
  if(dimension==3)
    {
      droite(1)=dimension;
      droite(2)=dimension+1;
    }
  IntVect haut(nb_dir);
  haut(0)=dimension+1;
  if(dimension==3)
    {
      haut(1)=dimension+2;
      haut(2)=dimension+2;
    }
  IntVect bas(nb_dir);
  bas(0)=1;
  if(dimension==3)
    {
      bas(1)=2;
      bas(2)=2;
    }

  int coin=-1;
  int bord=0;
  int mixte=1;
  int interne=2;
  // Detection des plaques (2 faces frontieres se superposent):
  IntVect est_une_plaque(nb_faces());
  creer_tableau_faces(est_une_plaque);
  est_une_plaque=0;
  // Boucles sur les faces frontieres
  ArrOfDouble P1(3), P2(3);
  P1=0;
  P2=0;
  double eps = 10.0*Objet_U::precision_geom;
  for (int face=0; face<premiere_face_int(); face++)
    {
      int ori = orientation(face);
      for (int i = 0; i < dimension; i++)
        {
          P1[i] = xv(face, i) + (ori == i ? eps : 0);
          P2[i] = xv(face, i) - (ori == i ? eps : 0);
        }
      int elem1 = domaine().chercher_elements(P1[0], P1[1], P1[2]);
      int elem2 = domaine().chercher_elements(P2[0], P2[1], P2[2]);
      if (elem1 >= 0 && elem2 >= 0 && elem1 != elem2)
        {
          // Find 2 points P1 and P2 in cells so:
          est_une_plaque(face) = 1;
          //Journal() << "We detect an internal boundary on face " << face << " between elements " << elem1 << " and " << elem2 << finl;
        }
    }
  est_une_plaque.echange_espace_virtuel();

  for(int dir=0; dir<nb_dir; dir++)
    for (el1=0; el1<nb_poly_tot; el1++)
      {
        // On doit generer l'arete en haut a droite de el1
        face12=elem_faces(el1,droite(dir));
        face13=elem_faces(el1,haut(dir));

        el2=face_vois(*this, mon_dom, face12, 1);

        if(el2>-1)
          face24=elem_faces(el2,haut(dir));
        else
          face24=-1;

        el3=face_vois(*this, mon_dom, face13, 1);

        if(el3>-1)
          face34=elem_faces(el3,droite(dir));
        else
          face34=-1;

        if( (el2>-1) && (el3>-1))
          el4=face_vois(*this, mon_dom, face24, 1);
        else if((el2>-1))
          el4=face_vois(*this, mon_dom, face24, 1);
        else if((el3>-1))
          el4=face_vois(*this, mon_dom, face34, 1);
        else
          el4=-1;

        if ( (el2==-1) && (el4>=0) )
          face24=elem_faces(el4,bas(dir));
        if ( (el3==-1) && (el4>=0) )
          face34=elem_faces(el4,gauche(dir));

        const int nb_f = nb_faces();
        if (el2 > -1 && el3 > -1 && el4 > -1) // arete interne
          les_aretes.affecter(nb_aretes_, dir, interne, nb_f, face13, face24, face12, face34, est_une_plaque);
        else if ( (el3 > -1 && el4 > -1) ||
                  (el2 > -1 && el4 > -1) ||
                  (el2 > -1 && el3 > -1) ) // arete mixte
          les_aretes.affecter(nb_aretes_, dir, mixte, nb_f, face13, face24, face12, face34, est_une_plaque);
        else if (el2 > -1) // arete bord
          les_aretes.affecter(nb_aretes_, dir, bord, nb_f, face13, face24, face12, 1, est_une_plaque);
        else if (el3 > -1) // arete bord
          les_aretes.affecter(nb_aretes_, dir, bord, nb_f, face12, face34, face13, 1, est_une_plaque);
        else // arete coin
          les_aretes.affecter(nb_aretes_, dir, coin, nb_f, face13, face24, face12, face34, est_une_plaque);
        //Cerr << "elements_haut_droit " << el1 << " " << el2 << " " << el3 << " " << el4 << finl;
        //Journal() << "Provisoire faces arete: " << face12 << " " << face13 << " " << face24 << " " << face34 << finl;

        // Pour les coins ou bords :
        face13 = elem_faces(el1, bas(dir));
        el3 = face_vois(*this, mon_dom, face13, 0);
        if (el3 < 0) // On doit generer l'arete en bas a droite de el1
          // si la maille en bas de el1 n'existe pas
          {
            if (el2 >= 0)
              {
                face24 = elem_faces(el2, bas(dir));
                el4 = face_vois(*this, mon_dom, face24, 0);
                if (el4 < 0) // arete bord
                  les_aretes.affecter(nb_aretes_, dir, bord, nb_f, face13, face24, face12, -1, est_une_plaque);
                else // arete mixte
                  {
                    face34 = elem_faces(el4, gauche(dir));
                    if (face_vois(*this, mon_dom, face34, 0) == -1)
                      les_aretes.affecter(nb_aretes_, dir, mixte, nb_f, face13, face24, face34, face12, est_une_plaque);
                  }
              }
            else   // arete coin
              les_aretes.affecter(nb_aretes_, dir, coin, nb_f, face13, -1, -1, face12, est_une_plaque);
            //Cerr << "elements_bas_droit " << el1 << " " << el2 << " " << el3 << " " << el4 << finl;
          }


        face13 = elem_faces(el1, gauche(dir));
        el3 = face_vois(*this, mon_dom, face13, 0);
        if (el3 < 0) // On doit generer l'arete en haut a gauche de el1
          // si la maille en haut a gauche n'existe pas
          {
            face12 = elem_faces(el1, haut(dir));
            el2 = face_vois(*this, mon_dom, face12, 1);
            if (el2 >= 0)
              {
                face24 = elem_faces(el2, gauche(dir));
                el4 = face_vois(*this, mon_dom, face24, 0);
                if (el4 < 0) // arete bord
                  les_aretes.affecter(nb_aretes_, dir, bord, nb_f, face13, face24, face12, -1, est_une_plaque);
              }
            else   // arete coin
              les_aretes.affecter(nb_aretes_, dir, coin, nb_f, -1, face12, -1, face13, est_une_plaque);
            //Cerr << "elements_haut_gauche " << el1 << " " << el2 << " " << el3 << " " << el4 << finl;
          }

        // On doit generer l'arete en bas a gauche de el1
        face12 = elem_faces(el1, gauche(dir));
        el2 = face_vois(*this, mon_dom, face12, 0);
        face13 = elem_faces(el1, bas(dir));
        el3 = face_vois(*this, mon_dom, face13, 0);
        if ((el2 < 0) && (el3 < 0)) // arete coin
          {
            les_aretes.affecter(nb_aretes_, dir, coin, nb_f, face13, -1, face12, -1, est_une_plaque);
            //Cerr << "elements_bas_gauche " << el1 << " " << el2 << " " << el3 << " " << el4 << finl;
          }
      }
  nb_aretes_++;
  les_aretes.dimensionner(nb_aretes_);
  // Cerr << "Tri des aretes " << finl;
  les_aretes.trier(nb_aretes_coin_,
                   nb_aretes_bord_,
                   nb_aretes_mixtes_,
                   nb_aretes_internes_);
#ifdef SORT_POUR_DEBOG

  les_aretes.trier_pour_debog(nb_aretes_coin_,
                              nb_aretes_bord_,
                              nb_aretes_mixtes_,
                              nb_aretes_internes_,xv());
#endif
  nb_aretes_joint_=0;
  assert(nb_aretes_==nb_aretes_coin_+nb_aretes_bord_+nb_aretes_mixtes_
         +nb_aretes_internes_+nb_aretes_joint_);
  Qdm_.ref(les_aretes.faces());
  /*
  // Boucle pour verifier ou sont les parois internes
  for (int i=0; i<Qdm_.dimension(0); i++)
    {
      int nb_plaques = 0;
      for (int j = 0; j < Qdm_.dimension(1); j++)
        nb_plaques += Qdm_(i, j) >= 0 ? est_une_plaque(Qdm_(i, j)) : 0;
      if (nb_plaques > 0)
      Journal() << "Provisoire arete " << i << " a " << nb_plaques << " paroi internes." << finl;
    }
  Journal() << "Aretes coin   = " << nb_aretes_coin_ << finl;
    Journal() << "Aretes bord   = " << nb_aretes_bord_ << finl;
    Journal() << "Aretes mixte  = " << nb_aretes_mixtes_ << finl;
    Journal() << "Aretes interne= " << nb_aretes_internes_ << finl;
    Journal() << "Aretes joint  = " << nb_aretes_joint_ << finl;
     */
  //Cerr << "Qdm : " << Qdm_ << finl;
}

/*! @brief calcul des pas du maillage
 *
 */
void Domaine_VDF::calcul_h()
{
  Domaine& domaine_geom=domaine();
  IntVect numfa(domaine_geom.nb_faces_elem());
  const double deux_pi = M_PI * 2.0;
  const int D = dimension;

  for (int e = 0; e < domaine_geom.nb_elem(); e++)
    {
      for (int j = 0; j < domaine_geom.nb_faces_elem(); j++)
        numfa[j] = elem_faces(e, j);

      h_x_ = std::min(h_x_, xv_(numfa[D], 0) - xv_(numfa[0], 0));
      double hy_loc;
      if (axi)
        {
          double d_teta = xv_(numfa[D + 1], 1) - xv_(numfa[1], 1);
          if (d_teta < 0) d_teta += deux_pi;
          hy_loc = d_teta * xv_(numfa[1], 0);
        }
      else hy_loc = xv_(numfa[D + 1], 1) - xv_(numfa[1], 1);
      h_y_ = std::min(h_y_, hy_loc);
      if (dimension == 3) h_z_ = std::min(h_z_, xv_(numfa[5], 2) - xv_(numfa[2], 2));
    }
  h_x_ = mp_min(h_x_);
  h_y_ = mp_min(h_y_);
  h_z_ = mp_min(h_z_);
}

void Domaine_VDF::modifier_pour_Cl(const Conds_lim& conds_lim)
{
  // Cerr << "Domaine_VDF::Modifier_pour_Cl" << finl;
  Domaine_VF::modifier_pour_Cl(conds_lim);

  int fac3;
  int nb_cond_lim=conds_lim.size();
  IntTab aretes_coin_traitees(nb_aretes_coin());
  aretes_coin_traitees = -1;

  for (int num_cond_lim=0; num_cond_lim<nb_cond_lim; num_cond_lim++)
    {
      // for cl ...
      //Journal() << "On traite la cl num : " << num_cond_lim << finl;
      const Cond_lim_base& cl = conds_lim[num_cond_lim].valeur();
      if (sub_type(Periodique, cl))
        {
          // if cl perio

          // Modification du tableau Qdm_ pour les aretes de type periodicite

          const Domaine_Cl_VDF& domaine_Cl_VDF = ref_cast(Domaine_Cl_VDF,cl.domaine_Cl_dis());

          int ndeb_arete = premiere_arete_bord();
          int fac1,fac2,sign,num_face,elem1,n_type;
          for (int n_arete=ndeb_arete; n_arete<ndeb_arete+nb_aretes_bord(); n_arete++)
            {
              // for n_arete
              n_type=domaine_Cl_VDF.type_arete_bord(n_arete-ndeb_arete);

              if (n_type == TypeAreteBordVDF::PERIO_PERIO) // arete de type periodicite
                {
                  //if arete perio
                  fac1 = Qdm_(n_arete,0);
                  fac2 = Qdm_(n_arete,1);
                  fac3 = Qdm_(n_arete,2);
                  sign = Qdm_(n_arete,3);

                  const Front_VF& la_frontiere_dis = ref_cast(Front_VF,cl.frontiere_dis());
                  int ndeb = la_frontiere_dis.num_premiere_face();
                  int nfin = ndeb + la_frontiere_dis.nb_faces();
                  if ( ( ( ndeb <= fac1) && (fac1 < nfin) ) ||
                       ( ( ndeb <= fac2) && (fac2 < nfin) )
                     )
                    {
                      if (sign == 1)
                        elem1 = face_voisins(fac1,1);
                      else
                        elem1 = face_voisins(fac1,0);
                      num_face = dimension+orientation_[fac3];

                      if (sign == -1)
                        {
                          Qdm_(n_arete,3) = fac3;
                          Qdm_(n_arete,2) = elem_faces(elem1,num_face);
                        }
                      else
                        Qdm_(n_arete,3) = elem_faces(elem1,num_face);
                    }
                }
            }
          // prise  en compte des aretes coin
          int ndeb_arete_coin = premiere_arete_coin();
          Cerr << "Modifications de Qdm pour les aretes coins  num_cond_lim=" << num_cond_lim <<  finl;
          int fac4;

          for (int n_arete=ndeb_arete_coin; n_arete<ndeb_arete_coin+nb_aretes_coin(); n_arete++)
            {
              if (aretes_coin_traitees[n_arete] != 1)
                {
                  fac1 = Qdm_(n_arete,0);
                  fac2 = Qdm_(n_arete,1);
                  fac3 = Qdm_(n_arete,2);
                  fac4 = Qdm_(n_arete,3);

                  // On recupere le numero des faces qui ne sont pas egales a -1
                  IntVect f(2);
                  f = -2;
                  int i=0;
                  if (fac1 != -1)
                    {
                      f(i) = fac1;
                      i++;
                    }
                  if (fac2 != -1)
                    {
                      f(i) = fac2;
                      i++;
                    }
                  if (fac3 != -1)
                    {
                      f(i) = fac3;
                      i++;
                    }
                  if (fac4 != -1)
                    {
                      f(i) = fac4;
                      i++;
                    }

                  // On regarde si la face est une face de periodicite

                  const Front_VF& la_frontiere_dis = ref_cast(Front_VF,cl.frontiere_dis());
                  int ndeb = la_frontiere_dis.num_premiere_face();
                  int nfin = ndeb + la_frontiere_dis.nb_faces();
                  int  elem, fac,dim0,dim1,indic_f0=-100,indic_f1=-100;

                  n_type=domaine_Cl_VDF.type_arete_coin(n_arete-ndeb_arete_coin);
                  dim1 = orientation_[f(1)];
                  dim0 = orientation_[f(0)];

                  for (int j=0; j<2; j++)
                    for (int k=0; k<2; k++)
                      if ((face_voisins(f(0),j) == face_voisins(f(1),k))  && (face_voisins(f(0),j)!=-1) )
                        {
                          indic_f0 = j;
                          indic_f1 = k;
                        }

                  if ((n_type == TypeAreteCoinVDF::PERIO_PAROI) || (n_type == TypeAreteCoinVDF::PERIO_FLUIDE))// arete coin perio-paroi
                    {
                      if ((f(0) >= ndeb)&&(f(0) < nfin))
                        {
                          Qdm_(n_arete,2)=f(0);
                          Qdm_(n_arete,indic_f0)=f(1);

                          elem = face_voisins(f(0),1-indic_f0);
                          fac = elem_faces(elem,dim1+(1-indic_f1)*dimension);
                          Qdm_(n_arete,1-indic_f0)=fac;

                          Qdm_(n_arete,3)=1-2*indic_f1;

                          //                          Cerr << "n_arete=" << n_arete << "  OK!!" << finl;
                          aretes_coin_traitees[n_arete] = 1;
                        }
                      else
                        {
                          if ((f(1) >= ndeb)&&(f(1) < nfin))
                            {
                              Qdm_(n_arete,2)=f(1);
                              Qdm_(n_arete,indic_f1)=f(0);

                              elem = face_voisins(f(1),1-indic_f1);
                              fac = elem_faces(elem,dim0+(1-indic_f0)*dimension);
                              Qdm_(n_arete,1-indic_f1)=fac;

                              Qdm_(n_arete,3)=1-2*indic_f0;

                              //                              Cerr << "n_arete=" << n_arete << "  OK!!" << finl;
                              aretes_coin_traitees[n_arete] = 1;
                            }
                          else
                            {
                              // Cerr<<"Attention cas non traite lors du remplissage du tableau Qdm"<<finl;
                              // exit();
                              ;
                            }
                        }
                    }
                  else if (n_type == TypeAreteCoinVDF::PERIO_PERIO) // arete coin perio-perio
                    {
                      if ((f(0) >= ndeb)&&(f(0) < nfin))
                        {
                          Qdm_(n_arete,2+indic_f1)=f(0);
                          Qdm_(n_arete,indic_f0)=f(1);

                          elem = face_voisins(f(0),1-indic_f0);
                          fac = elem_faces(elem,dim1+(1-indic_f1)*dimension);
                          Qdm_(n_arete,1-indic_f0)=fac;

                          elem = face_voisins(f(1),1-indic_f1);
                          fac = elem_faces(elem,dim0+(1-indic_f0)*dimension);
                          Qdm_(n_arete,3-indic_f1)=fac;

                          //                            Cerr << "n_arete=" << n_arete << "  OK!!" << finl;
                          aretes_coin_traitees[n_arete] = 1;
                        }
                      else
                        {
                          if ((f(1) >= ndeb)&&(f(1) < nfin))
                            {
                              Qdm_(n_arete,2+indic_f0)=f(1);
                              Qdm_(n_arete,indic_f1)=f(0);

                              elem = face_voisins(f(1),1-indic_f1);
                              fac = elem_faces(elem,dim0+(1-indic_f0)*dimension);
                              Qdm_(n_arete,1-indic_f1)=fac;

                              elem = face_voisins(f(0),1-indic_f0);
                              fac = elem_faces(elem,dim1+(1-indic_f1)*dimension);
                              Qdm_(n_arete,3-indic_f0)=fac;

                              //                                Cerr << "n_arete=" << n_arete << "  OK!!" << finl;
                              aretes_coin_traitees[n_arete] = 1;
                            }
                        }
                    }

                  else
                    {
                      Cerr << "Attention : les cas concernant d autres types d aretes coin "  << finl;
                      Cerr << "que perio-perio ou perio-paroi ne sont pas traites!!" << finl;
                    }
                }
            }
        }

      // Modif OC 01/2005 pour traiter les coins de type paroi/fluide
      else if (sub_type(Dirichlet_entree_fluide, cl) || sub_type(Neumann_sortie_libre, cl) )
        {
          // if cl de type paroi

          const Domaine_Cl_VDF& domaine_Cl_VDF = ref_cast(Domaine_Cl_VDF,cl.domaine_Cl_dis());

          int fac1,fac2,n_type;
          int ndeb_arete_coin = premiere_arete_coin();
          Cerr << "Modifications de Qdm pour les aretes coins touchant une paroi num_cond_lim=" << num_cond_lim <<  finl;
          int fac4;

          for (int n_arete=ndeb_arete_coin; n_arete<ndeb_arete_coin+nb_aretes_coin(); n_arete++)
            {
              if (aretes_coin_traitees[n_arete] != 1)
                {
                  fac1 = Qdm_(n_arete,0);
                  fac2 = Qdm_(n_arete,1);
                  fac3 = Qdm_(n_arete,2);
                  fac4 = Qdm_(n_arete,3);

                  // On recupere le numero des faces qui ne sont pas egales a -1
                  IntVect f(2);
                  f = -2;
                  int i=0;
                  if (fac1 != -1)
                    {
                      f(i) = fac1;
                      i++;
                    }
                  if (fac2 != -1)
                    {
                      f(i) = fac2;
                      i++;
                    }
                  if (fac3 != -1)
                    {
                      f(i) = fac3;
                      i++;
                    }
                  if (fac4 != -1)
                    {
                      f(i) = fac4;
                      i++;
                    }

                  // On regarde si la face est une face de type paroi

                  const Front_VF& la_frontiere_dis = ref_cast(Front_VF,cl.frontiere_dis());
                  int ndeb = la_frontiere_dis.num_premiere_face();
                  int nfin = ndeb + la_frontiere_dis.nb_faces();
                  int indic_f0=-100,indic_f1=-100;

                  n_type=domaine_Cl_VDF.type_arete_coin(n_arete-ndeb_arete_coin);

                  for (int j=0; j<2; j++)
                    for (int k=0; k<2; k++)
                      if ((face_voisins(f(0),j) == face_voisins(f(1),k)) && (face_voisins(f(0),j)!=-1) )
                        {
                          indic_f0 = j;
                          indic_f1 = k;
                        }

                  if ((n_type == TypeAreteCoinVDF::PAROI_FLUIDE) || (n_type == TypeAreteCoinVDF::FLUIDE_PAROI) || (n_type == TypeAreteCoinVDF::FLUIDE_FLUIDE))// arete coin paroi-fluide
                    {
                      if ((f(0) >= ndeb)&&(f(0) < nfin))
                        {
                          Qdm_(n_arete,0)=f(1);
                          Qdm_(n_arete,1)=f(1);
                          Qdm_(n_arete,2)=f(0);
                          Qdm_(n_arete,3)=1-2*indic_f1;
                          aretes_coin_traitees[n_arete] = 1;
                        }
                      else
                        {
                          if ((f(1) >= ndeb)&&(f(1) < nfin))
                            {
                              Qdm_(n_arete,0)=f(0);
                              Qdm_(n_arete,1)=f(0);
                              Qdm_(n_arete,2)=f(1);
                              Qdm_(n_arete,3)=1-2*indic_f0;
                              aretes_coin_traitees[n_arete] = 1;
                            }
                          else
                            {
                              // Cerr<<"Attention cas non traite lors du remplissage du tableau Qdm"<<finl;
                              // exit();
                              ;
                            }
                        }
                    }
                }
            }
        }

      //   Cerr << "Qdm : " << Qdm_ << finl;
      //   Cerr << "aretes_coin_traitees : " << aretes_coin_traitees << finl;
    }
  //Journal() << "le_dom apres modif pour Cl : " << *this << finl;


  // PQ : 10/10/05 : les faces periodiques etant a double contribution
  //                      l'appel a marquer_faces_double_contrib s'effectue dans cette methode
  //                      afin de pouvoir beneficier de conds_lim.


  Domaine_VF::marquer_faces_double_contrib(conds_lim);
}

/*! @brief remplit le tableau face_voisins_fictifs_ ne CREE PAS d elts fictifs!!!
 *
 */

void Domaine_VDF::creer_elements_fictifs(const Domaine_Cl_dis_base& zcldisbase)
{
  Cerr << "Domaine_VDF::creer_elements_fictifs()" << finl;
  const Domaine_Cl_VDF& zclvdf = ref_cast(Domaine_Cl_VDF,zcldisbase);
  if (face_voisins_fictifs_.size() == 0)
    {
      face_voisins_fictifs_.resize(nb_faces_bord(),2);
      face_voisins_fictifs_ = -1;
      int compteur = nb_elem_tot();
      int face,ndeb,nfin;

      for (int n_bord=0; n_bord<nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          if ( (sub_type(Dirichlet_entree_fluide,la_cl.valeur())) ||
               (sub_type(Neumann_sortie_libre,la_cl.valeur())) )
            {
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (face=ndeb; face<nfin; face++)
                if (face_voisins(face,0) != -1)
                  face_voisins_fictifs_(face,1) = compteur++;
                else
                  face_voisins_fictifs_(face,0) = compteur++ ;
            }
        }
    }
  Cerr << "taille "<<face_voisins_fictifs_.size()-nb_elem()<<finl;
}

// Remplit le tableau dist avec les valeurs des distances normales
// au bord des faces du bord de nom nom_bord
// Le tableau dist est dimensionne par la methode
DoubleVect& Domaine_VDF::dist_norm_bord(DoubleVect& dist, const Nom& nom_bord) const
{
  if (axi)
    {
      for (int n_bord=0; n_bord<les_bords_.size(); n_bord++)
        {
          const Front_VF& fr_vf = front_VF(n_bord);
          if (fr_vf.le_nom() == nom_bord)
            {
              dist.resize(fr_vf.nb_faces());
              int ndeb = fr_vf.num_premiere_face();
              for (int face=ndeb; face<ndeb+fr_vf.nb_faces(); face++)
                dist(face-ndeb) = dist_norm_bord_axi(face);
            }
        }
    }
  else
    {
      for (int n_bord=0; n_bord<les_bords_.size(); n_bord++)
        {
          const Front_VF& fr_vf = front_VF(n_bord);
          if (fr_vf.le_nom() == nom_bord)
            {
              dist.resize(fr_vf.nb_faces());
              int ndeb = fr_vf.num_premiere_face();
              for (int face=ndeb; face<ndeb+fr_vf.nb_faces(); face++)
                dist(face-ndeb) = dist_norm_bord(face);
            }
        }
    }
  return dist;
}

void Domaine_VDF::init_virt_e_map() const
{
  IntTrav p_e(0, 2);
  domaine().creer_tableau_elements(p_e);
  for (int e = 0; e < nb_elem() ; e++) p_e(e, 0) = Process::me(), p_e(e, 1) = e;
  p_e.echange_espace_virtuel();
  for (int e = nb_elem() ; e < nb_elem_tot() ; e++) virt_e_map[ {{ p_e(e, 0), p_e(e, 1) }}] = e;
}
