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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Zone_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Linear_algebra_tools_impl.h>
#include <Champ_implementation_P1.h>
#include <Connectivite_som_elem.h>
#include <MD_Vector_composite.h>
#include <Zone_Cl_PolyMAC.h>
#include <Segment_PolyMAC.h>
#include <MD_Vector_tools.h>
#include <TRUSTTab_parts.h>
#include <Quadri_PolyMAC.h>
#include <Quadrangle_VEF.h>
#include <Comm_Group_MPI.h>
#include <communications.h>
#include <Poly_geom_base.h>
#include <Tetra_PolyMAC.h>
#include <Hexaedre_VEF.h>
#include <Hexa_PolyMAC.h>
#include <Matrix_tools.h>
#include <Statistiques.h>
#include <Zone_PolyMAC.h>
#include <Tri_PolyMAC.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <PE_Groups.h>
#include <Rectangle.h>
#include <Tetraedre.h>
#include <EFichier.h>
#include <SFichier.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <Segment.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EChaine.h>
#include <Lapack.h>
#include <unistd.h>
#include <numeric>
#include <cfloat>
#include <vector>
#include <tuple>
#include <cmath>
#include <cfenv>
#include <set>
#include <map>

Implemente_instanciable(Zone_PolyMAC,"Zone_PolyMAC",Zone_VF);


//// printOn
//

Sortie& Zone_PolyMAC::ecrit(Sortie& os) const
{
  Zone_VF::printOn(os);
  os << "____ h_carre "<<finl;
  os << h_carre << finl;
  os << "____ type_elem_ "<<finl;
  os << type_elem_ << finl;
  os << "____ nb_elem_std_ "<<finl;
  os << nb_elem_std_ << finl;
  os << "____ volumes_entrelaces_ "<<finl;
  volumes_entrelaces_.ecrit(os);
  os << "____ face_normales_ "<<finl;
  face_normales_.ecrit(os);
  os << "____ nb_faces_std_ "<<finl;
  os << nb_faces_std_ << finl;
  os << "____ rang_elem_non_std_ "<<finl;
  rang_elem_non_std_.ecrit(os);
  return os;
}

//// printOn
//

Sortie& Zone_PolyMAC::printOn(Sortie& os) const
{
  Zone_VF::printOn(os);

  os << h_carre << finl;
  os << type_elem_ << finl;
  os << nb_elem_std_ << finl;
  os << volumes_entrelaces_ << finl;
  os << face_normales_ << finl;
  os << xp_ << finl;
  os << xv_ << finl;
  os << nb_faces_std_ << finl;
  os << rang_elem_non_std_ << finl;
  return os;
}

//// readOn
//

Entree& Zone_PolyMAC::readOn(Entree& is)
{
  Zone_VF::readOn(is);
  is >> h_carre;
  is >> type_elem_;
  is >> nb_elem_std_ ;
  is >> volumes_entrelaces_ ;
  is >> face_normales_ ;
  is >> xp_;
  is >> xv_;
  is >> nb_faces_std_ ;
  is >> rang_elem_non_std_ ;
  return is;
}

// Description:
//  Methode appelee par Zone_VF::discretiser apres la creation
//  des faces reelles.
//  On reordonne les faces de sorte a placer les faces "non standard"
//  au debut de la liste des faces. Les faces non standard sont celles
//  dont les volumes de controles sont modifies par les conditions aux
//  limites.
void Zone_PolyMAC::reordonner(Faces& les_faces)
{
  if (Process::je_suis_maitre())
    Cerr << "Zone_PolyMAC::reordonner les_faces " << finl;

  // Construction de rang_elem_non_std_ :
  //  C'est un vecteur indexe par les elements du domaine.
  //  size() = nb_elem()
  //  size_tot() = nb_elem_tot()
  //  Valeurs dans le tableau :
  //   rang_elem_non_std_[i] = -1 si l'element i est standard,
  //  sinon
  //   rang_elem_non_std_[i] = j, ou j est l'indice de l'element dans
  //   les tableaux indexes par les elements non standards (par exemple
  //   le tableau Zone_Cl_PolyMAC::type_elem_Cl_).
  // Un element est non standard s'il est voisin d'une face frontiere.
  {
    const Domaine& dom = zone().domaine();
    const int nb_elements = nb_elem();
    const int nb_faces_front = zone().nb_faces_frontiere();
    dom.creer_tableau_elements(rang_elem_non_std_);
    //    rang_elem_non_std_.resize(nb_elements);
    //    Scatter::creer_tableau_distribue(dom, Joint::ELEMENT, rang_elem_non_std_);
    rang_elem_non_std_ = -1;
    int nb_elems_non_std = 0;
    // D'abord on marque les elements non standards avec rang_elem_non_std_[i] = 0
    for (int i_face = 0; i_face < nb_faces_front; i_face++)
      {
        const int elem = les_faces.voisin(i_face, 0);
        if (rang_elem_non_std_[elem] < 0)
          {
            rang_elem_non_std_[elem] = 0;
            nb_elems_non_std++;
          }
      }
    nb_elem_std_ = nb_elements - nb_elems_non_std;
    rang_elem_non_std_.echange_espace_virtuel();
    int count = 0;
    const int size_tot = rang_elem_non_std_.size_totale();
    // On remplace le marqueur "0" par l'indice j.
    for (int elem = 0; elem < size_tot; elem++)
      {
        if (rang_elem_non_std_[elem] == 0)
          {
            rang_elem_non_std_[elem] = count;
            count++;
          }
      }
  }

  // Construction du tableau de renumerotation des faces. Ce tableau,
  // une fois trie dans l'ordre croissant donne l'ordre des faces dans
  // la zone_VF. La cle de tri est construite de sorte a pouvoir retrouver
  // l'indice de la face a partir de la cle par la formule :
  //  indice_face = cle % nb_faces
  const int nbfaces = les_faces.nb_faces();
  ArrOfInt sort_key(nbfaces);
  {
    nb_faces_std_ =0;
    const int nb_faces_front = zone().nb_faces_frontiere();
    // Attention : face_voisins_ n'est pas encore initialise, il
    // faut passer par les_faces.voisins() :
    const IntTab& facevoisins = les_faces.voisins();
    // On place en premier les faces de bord:
    int i_face;
    for (i_face = 0; i_face < nbfaces; i_face++)
      {
        int key = -1;
        if (i_face < nb_faces_front)
          {
            // Si la face est au bord, elle doit etre placee au debut
            // (en fait elle ne doit pas etre renumerotee)
            key = i_face;
          }
        else
          {
            const int elem0 = facevoisins(i_face, 0);
            const int elem1 = facevoisins(i_face, 1);
            // Ces faces ont toujours deux voisins.
            assert(elem0 >= 0 && elem1 >= 0);
            if (rang_elem_non_std_[elem0] >= 0 || rang_elem_non_std_[elem1] >= 0)
              {
                // Si la face est voisine d'un element non standard, elle
                // doit etre classee juste apres les faces de bord:
                key = i_face;
              }
            else
              {
                // Face standard : a la fin du tableau
                key = i_face + nbfaces;
                nb_faces_std_++;
              }
          }
        sort_key[i_face] = key;
      }
    sort_key.ordonne_array();

    // On transforme a nouveau la cle en numero de face:
    for (i_face = 0; i_face < nbfaces; i_face++)
      {
        const int key = sort_key[i_face] % nbfaces;
        sort_key[i_face] = key;
      }
  }
  // On reordonne les faces:
  {
    IntTab& faces_sommets = les_faces.les_sommets();
    IntTab old_tab(faces_sommets);
    const int nb_som_faces = faces_sommets.dimension(1);
    for (int i = 0; i < nbfaces; i++)
      {
        const int old_i = sort_key[i];
        for (int j = 0; j < nb_som_faces; j++)
          faces_sommets(i, j) = old_tab(old_i, j);
      }
  }

  {
    IntTab& faces_voisins = les_faces.voisins();
    IntTab old_tab(faces_voisins);
    for (int i = 0; i < nbfaces; i++)
      {
        const int old_i = sort_key[i];
        faces_voisins(i, 0) = old_tab(old_i, 0);
        faces_voisins(i, 1) = old_tab(old_i, 1);
      }
  }

  // Calcul de la table inversee: reverse_index[ancien_numero] = nouveau numero
  ArrOfInt reverse_index(nbfaces);
  {
    for (int i = 0; i < nbfaces; i++)
      {
        const int j = sort_key[i];
        reverse_index[j] = i;
      }
  }
  // Renumerotation de elem_faces:
  {
    // Nombre d'indices de faces dans le tableau
    const int nb_items = elem_faces_.size();
    ArrOfInt& array = elem_faces_;
    for (int i = 0; i < nb_items; i++)
      {
        const int old = array[i];
        if (old<0)
          array[i] = -1;
        else
          array[i] = reverse_index[old];
      }
  }
  // Mise a jour des indices des faces de joint:
  {
    Joints&      joints    = zone().faces_joint();
    const int nbjoints = joints.size();
    for (int i_joint = 0; i_joint < nbjoints; i_joint++)
      {
        Joint&     un_joint         = joints[i_joint];
        ArrOfInt& indices_faces = un_joint.set_joint_item(Joint::FACE).set_items_communs();
        const int nbfaces2    = indices_faces.size_array();
        assert(nbfaces2 == un_joint.nb_faces()); // renum_items_communs rempli ?
        for (int i = 0; i < nbfaces2; i++)
          {
            const int old = indices_faces[i]; // ancien indice local
            indices_faces[i] = reverse_index[old];
          }
        // Les faces de joint ne sont plus consecutives dans le
        // tableau: num_premiere_face n'a plus ne sens
        un_joint.fixer_num_premiere_face(-1);
      }
  }
}

void Zone_PolyMAC::typer_elem(Zone& zone_geom)
{
  const Elem_geom_base& type_elem_geom = zone_geom.type_elem().valeur();

  if (sub_type(Rectangle,type_elem_geom))
    {
      zone_geom.typer("Quadrangle");
    }
  else if (sub_type(Hexaedre,type_elem_geom))
    zone_geom.typer("Hexaedre_VEF");

  const Elem_geom_base& elem_geom = zone_geom.type_elem().valeur();
  type_elem_.typer(elem_geom.que_suis_je());
}

void Zone_PolyMAC::discretiser()
{

  Zone& zone_geom=zone();
  typer_elem(zone_geom);
  Elem_geom_base& elem_geom = zone_geom.type_elem().valeur();
  Zone_VF::discretiser();

  // Correction du tableau facevoisins:
  //  A l'issue de Zone_VF::discretiser(), les elements voisins 0 et 1 d'une
  //  face sont les memes sur tous les processeurs qui possedent la face.
  //  Si la face est virtuelle et qu'un des deux elements voisins n'est
  //  pas connu (il n'est pas dans l'epaisseur du joint), l'element voisin
  //  vaut -1. Cela peut etre un voisin 0 ou un voisin 1.
  //  On corrige les faces virtuelles pour que, si un element voisin n'est
  //  pas connu, alors il est voisin1. Le voisin0 est donc toujours valide.
  {
    IntTab& face_vois = face_voisins();
    const int debut = nb_faces();
    const int fin   = nb_faces_tot();
    for (int i = debut; i < fin; i++)
      {
        if (face_voisins(i, 0) == -1)
          {
            face_vois(i, 0) = face_vois(i, 1);
            face_vois(i, 1) = -1;
          }
      }
  }

  // Verification de la coherence entre l'element geometrique et
  //l'elemnt de discretisation


  if (sub_type(Segment_PolyMAC,type_elem_.valeur()))
    {
      if (!sub_type(Segment,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          exit();
        }
    }
  else if (sub_type(Tri_PolyMAC,type_elem_.valeur()))
    {
      if (!sub_type(Triangle,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " <<
               elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Triangle est compatible avec la discretisation PolyMAC en dimension 2" << finl;
          Cerr << " Il faut trianguler le domaine lorsqu'on utilise le mailleur interne" ;
          Cerr << " en utilisant l'instruction: Trianguler nom_dom" << finl;
          exit();
        }
    }
  else if (sub_type(Tetra_PolyMAC,type_elem_.valeur()))
    {
      if (!sub_type(Tetraedre,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " <<
               elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Tetraedre est compatible avec la discretisation PolyMAC en dimension 3" << finl;
          Cerr << " Il faut tetraedriser le domaine lorsqu'on utilise le mailleur interne";
          Cerr << " en utilisant l'instruction: Tetraedriser nom_dom" << finl;
          exit();
        }
    }

  else if (sub_type(Quadri_PolyMAC,type_elem_.valeur()))
    {

      if (!sub_type(Quadrangle_VEF,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          exit();
        }
    }
  else if (sub_type(Hexa_PolyMAC,type_elem_.valeur()))
    {

      if (!sub_type(Hexaedre_VEF,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          exit();
        }
    }

  int num_face;


  // On remplit le tableau face_normales_;
  //  Attention : le tableau face_voisins n'est pas exactement un
  //  tableau distribue. Une face n'a pas ses deux voisins dans le
  //  meme ordre sur tous les processeurs qui possedent la face.
  //  Donc la normale a la face peut changer de direction d'un
  //  processeur a l'autre, y compris pour les faces de joint.
  {
    const int n = nb_faces();
    face_normales_.resize(n, dimension);
    // const Domaine & dom = zone().domaine();
    //    Scatter::creer_tableau_distribue(dom, Joint::FACE, face_normales_);
    creer_tableau_faces(face_normales_);
    const IntTab& face_som = face_sommets();
    IntTab& face_vois = face_voisins();
    const IntTab& elem_face = elem_faces();
    const int n_tot = nb_faces_tot();
    for (num_face = 0; num_face < n_tot; num_face++)
      {
        type_elem_.normale(num_face,
                           face_normales_,
                           face_som, face_vois, elem_face, zone_geom);
      }

    DoubleTab old(face_normales_);
    face_normales_.echange_espace_virtuel();
    for (num_face = 0; num_face < n_tot; num_face++)
      {
        int id=1;
        for (int d=0; d<dimension; d++)
          if (!est_egal(old(num_face,d),face_normales_(num_face,d)))
            {
              id=0;
              if (!est_egal(old(num_face,d),-face_normales_(num_face,d)))
                {
                  Cerr<<"pb in faces_normales" <<finl;
                  exit();
                }
            }
        if (id==0)
          {
            // on a change le sens de la normale, on inverse elem1 elem2
            std::swap(face_vois(num_face,0),face_vois(num_face,1));
          }
      }
  }

  /* recalcul de xv pour avoir les vrais CG des faces */
  const DoubleTab& coords = zone().domaine().coord_sommets();
  for (int face = 0; dimension == 3 && face < nb_faces(); face++)
    {
      double xs[3] = { 0, }, S = 0;
      for (int k = 0; k < face_sommets_.dimension(1); k++) if (face_sommets_(face, k) >= 0)
          {
            int s0 = face_sommets_(face, 0), s1 = face_sommets_(face, k),
                s2 = k + 1 < face_sommets_.dimension(1) && face_sommets_(face, k + 1) >= 0 ? face_sommets_(face, k +1) : s0;
            double s = 0;
            for (int r = 0; r < 3; r++) for (int i = 0; i < 2; i++)
                s += (i ? -1 : 1) * face_normales_(face, r) * (coords(s2, (r + 1 +  i) % 3) - coords(s0, (r + 1 +  i) % 3))
                     * (coords(s1, (r + 1 + !i) % 3) - coords(s0, (r + 1 + !i) % 3));
            for (int r = 0; r < 3; r++) xs[r] += s * (coords(s0, r) + coords(s1, r) + coords(s2, r)) / 3;
            S += s;
          }
      if (S == 0 && sub_type(Hexa_PolyMAC,type_elem_.valeur()))
        {
          Cerr << "===============================" << finl;
          Cerr << "Error in your mesh for PolyMAC!" << finl;
          Cerr << "Add this keyword before discretization in your data file to create polyedras:" << finl;
          Cerr << "Polyedriser " << zone().domaine().le_nom() << finl;
          Process::exit();
        }
      for (int r = 0; r < 3; r++) xv_(face, r) = xs[r] / S;
    }
  xv_.echange_espace_virtuel();

  // orthocentrer();

  detecter_faces_non_planes();

  //volumes_entrelaces_ et volumes_entrelaces_dir : par projection de l'amont/aval sur la normale a la face
  creer_tableau_faces(volumes_entrelaces_);
  volumes_entrelaces_dir_.resize(0, 2), creer_tableau_faces(volumes_entrelaces_dir_);
  for (int f = 0, i, e; f < nb_faces(); f++)
    for (i = 0; i < 2 && (e = face_voisins_(f, i)) >= 0; volumes_entrelaces_(f) += volumes_entrelaces_dir_(f, i), i++)
      volumes_entrelaces_dir_(f, i) = dabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0)));
  volumes_entrelaces_.echange_espace_virtuel(), volumes_entrelaces_dir_.echange_espace_virtuel();

  Zone_VF::calculer_porosites();
  Zone_VF::calculer_diametres_hydrauliques();
  calculer_h_carre();

  /* ordre canonique dans elem_faces_ */
  std::map<std::array<double, 3>, int> xv_fsa;
  for (int e = 0, i, j, f; e < nb_elem_tot(); e++)
    {
      for (i = 0, j = 0, xv_fsa.clear(); i < elem_faces_.dimension(1) && (f = elem_faces_(e, i)) >= 0; i++)
        xv_fsa[ {{ xv_(f, 0), xv_(f, 1), dimension < 3 ? 0 : xv_(f, 2) }}] = f;
      for (auto &&c_f : xv_fsa) elem_faces_(e, j) = c_f.second, j++;
    }

  //diverses quantites liees aux aretes
  if (dimension > 2)
    {
      zone().creer_aretes();
      md_vector_aretes_ = zone().aretes_som().get_md_vector();

      /* ordre canonique dans aretes_som */
      IntTab& a_s = zone().set_aretes_som(), &e_a = zone().set_elem_aretes();
      const DoubleTab& xs = zone().domaine().coord_sommets();
      for (int a = 0, i, j, s; a < a_s.dimension_tot(0); a++)
        {
          for (i = 0, j = 0, xv_fsa.clear(); i < a_s.dimension(1) && (s = a_s(a, i)) >= 0; i++) xv_fsa[ {{ xs(s, 0), xs(s, 1), xs(s, 2) }}] = s;
          for (auto &&c_s : xv_fsa) a_s(a, j) = c_s.second, j++;
        }

      //remplissage de som_aretes
      som_arete.resize(zone().nb_som_tot());
      for (int i = 0; i < a_s.dimension_tot(0); i++) for (int j = 0; j < 2; j++) som_arete[a_s(i, j)][a_s(i, !j)] = i;

      //remplissage de xa (CGs des aretes), de ta_ (vecteur tangent aux aretes) et de longueur_arete_ (longueurs des aretes)
      xa_.resize(0, 3), ta_.resize(0, 3);
      creer_tableau_aretes(xa_), creer_tableau_aretes(ta_), creer_tableau_aretes(longueur_aretes_);
      for (int i = 0, k; i < zone().nb_aretes_tot(); i++)
        {
          int s1 = a_s(i, 0), s2 = a_s(i, 1);
          longueur_aretes_(i) = sqrt( dot(&xs(s2, 0), &xs(s2, 0), &xs(s1, 0), &xs(s1, 0)));
          for (k = 0; k < 3; k++) xa_(i, k) = (xs(s1, k) + xs(s2, k)) / 2, ta_(i, k) = (xs(s2, k) - xs(s1, k)) / longueur_aretes_(i);
        }

      /* ordre canonique dans elem_aretes_ */
      for (int e = 0, i, j, a; e < nb_elem_tot(); e++)
        {
          for (i = 0, j = 0, xv_fsa.clear(); i < e_a.dimension(1) && (a = e_a(e, i)) >= 0; i++) xv_fsa[ {{ xa_(a, 0), xa_(a, 1), xa_(a, 2) }}] = a;
          for (auto &&c_a : xv_fsa) e_a(e, j) = c_a.second, j++;
        }
    }

  //MD_vector pour Champ_P0_PolyMAC (elems + faces)
  MD_Vector_composite mdc_ef;
  mdc_ef.add_part(zone().md_vector_elements()), mdc_ef.add_part(md_vector_faces());
  mdv_elems_faces.copy(mdc_ef);

  //MD_vector pour Champ_Face_PolyMAC (faces + aretes)
  MD_Vector_composite mdc_fa;
  mdc_fa.add_part(md_vector_faces()), mdc_fa.add_part(dimension < 3 ? zone().domaine().md_vector_sommets() : md_vector_aretes());
  mdv_faces_aretes.copy(mdc_fa);
}

void Zone_PolyMAC::orthocentrer()
{
  const IntTab& f_s = face_sommets(), &e_s = zone().les_elems(), &e_f = elem_faces();
  const DoubleTab& xs = zone().domaine().coord_sommets(), &nf = face_normales_;
  const DoubleVect& fs = face_surfaces();
  int i, j, e, f, s, np;
  DoubleTab M(0, dimension + 1), X(dimension + 1, 1), S(0, 1), vp; //pour les systemes lineaires
  M.set_smart_resize(1), S.set_smart_resize(1), vp.set_smart_resize(1);
  IntTrav b_f_ortho, b_e_ortho; // b_{f,e}_ortho(f/e) = 1 si la face / l'element est orthocentre
  creer_tableau_faces(b_f_ortho), zone().creer_tableau_elements(b_e_ortho);

  /* 1. orthocentrage des faces (en dimension 3) */
  Cerr << zone().domaine().le_nom() << " : ";
  if (dimension < 3) b_f_ortho = 1; //les faces (segments) sont deja orthcentrees!
  else for (f = 0; f < nb_faces_tot(); f++)
      {
        //la face est-elle deja orthocentree?
        double d2min = DBL_MAX, d2max = 0, d2;
        for (i = 0, np = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++, np++)
          d2min = std::min(d2min, d2 = dot(&xs(s, 0), &xs(s, 0), &xv_(f, 0), &xv_(f, 0))), d2max = std::max(d2max, d2);
        if ((b_f_ortho(f) = (d2max / d2min - 1 < 1e-8))) continue;

        //peut-on l'orthocentrer?
        M.resize(np + 1, 4), S.resize(np + 1, 1);
        for (i = 0; i < np; i++) for (j = 0, S(i, 0) = 0, M(i, 3) = 1; j < 3; j++)
            S(i, 0) += 0.5 * std::pow(M(i, j) = xs(f_s(f, i), j) - xv_(f, j), 2);
        for (j = 0, S(np, 0) = M(np, 3) = 0; j < 3; j++) M(np, j) = nf(f, j) / fs(f);
        if (kersol(M, S, 1e-12, NULL, X, vp) > 1e-8) continue; //la face n'a pas d'orthocentre

        //contrainte : ne pas diminuer la distance entre xv et chaque arete de plus de 50%
        double r2min = DBL_MAX;
        for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
          {
            int s2 = f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0),
                a = som_arete[s].at(s2);
            std::array<double, 3> vec = cross(3, 3, &xv_(f, 0), &ta_(a, 0), &xs(s, 0));
            r2min = std::min(r2min, dot(&vec[0], &vec[0]));
          }
        if (dot(&X(0, 0), &X(0, 0)) > 0.25 * r2min) continue; //on s'eloigne trop du CG

        for (i = 0, b_f_ortho(f) = 1; i < dimension; i++) if (std::fabs(X(i, 0)) > 1e-8) xv_(f, i) += X(i, 0);
      }
  Cerr << 100. * mp_somme_vect(b_f_ortho) / Process::mp_sum(nb_faces()) << "% de faces orthocentrees" << finl;

  /* 2. orthocentrage des elements */
  Cerr << zone().domaine().le_nom() << " : ";
  for (e = 0; e < nb_elem_tot(); e++)
    {
      //l'element est-il deja orthocentre?
      double d2min = DBL_MAX, d2max = 0, d2;
      for (i = 0, np = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++, np++)
        d2min = std::min(d2min, d2 = dot(&xs(s, 0), &xs(s, 0), &xp_(e, 0), &xp_(e, 0))), d2max = std::max(d2max, d2);
      if ((b_e_ortho(e) = (d2max / d2min - 1 < 1e-8))) continue;

      //pour qu'on puisse l'orthocentrer, il faut que ses faces le soient
      for (i = 0, j = 1; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) j = j && b_f_ortho(f);
      if (!j) continue;

      //existe-il un orthocentre?
      M.resize(np, dimension + 1), S.resize(np, 1);
      for (i = 0; i < np; i++) for (j = 0, S(i, 0) = 0, M(i, dimension) = 1; j < dimension; j++)
          S(i, 0) += 0.5 * std::pow(M(i, j) = xs(e_s(e, i), j) - xp_(e, j), 2);
      if (kersol(M, S, 1e-12, NULL, X, vp) > 1e-8) continue; //l'element n'a pas d'orthocentre

      //contrainte : ne pas diminuer la distance entre xp et chaque face de plus de 50%
      double rmin = DBL_MAX;
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        rmin = std::min(rmin, std::fabs(dot(&xp_(e, 0), &nf(f, 0), &xv_(f, 0)) / fs(f)));
      if (dot(&X(0, 0), &X(0, 0)) > 0.25 * rmin * rmin) continue; //on s'eloigne trop du CG

      for (i = 0, b_e_ortho(e) = 1; i < dimension; i++) if (std::fabs(X(i, 0)) > 1e-8) xp_(e, i) += X(i, 0);
    }
  Cerr << 100. * mp_somme_vect(b_e_ortho) / Process::mp_sum(nb_elem()) << "% d'elements orthocentres" << finl;
}

void Zone_PolyMAC::detecter_faces_non_planes() const
{
  const IntTab& f_e = face_voisins(), &f_s = face_sommets_;
  const DoubleTab& xs = zone().domaine().coord_sommets(), &nf = face_normales_;
  const DoubleVect& fs = face_surfaces();
  int i, j, f, s, rk = Process::me(), np = Process::nproc();
  double sin2;
  ArrOfDouble val(np); //sur chaque proc : { cos^2 max, indice de face, indices d'elements }
  IntTab face(np), elem1(np), elem2(np);

  //sur chaque proc : on cherche l'angle le plus grand entre un sommet et le plan de sa face
  for (f = 0; f < nb_faces(); f++) for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
      if ((sin2 = std::pow(dot(&xs(s, 0), &nf(f, 0), &xv_(f, 0)) / fs(f), 2) / dot(&xs(s, 0), &xs(s, 0), &xv_(f, 0), &xv_(f, 0))) > val[rk])
        val[rk] = sin2, face(rk) = f, elem1(rk) = f_e(f, 0), elem2(rk) = f_e(f, 1);
  envoyer_all_to_all(val, val), envoyer_all_to_all(face, face), envoyer_all_to_all(elem1, elem1), envoyer_all_to_all(elem2, elem2);

  for (i = j = sin2 = 0; i < Process::nproc(); i++) if (val[i] > sin2) sin2 = val[i], j = i;
  double theta = asin(sqrt(sin2)) * 180 / M_PI;
  Cerr << "Zone_PolyMAC : angle sommet/face max " << theta << " deg (proc " << j << " , face ";
  Cerr << face(j) << " , elems " << elem1(j) << " / " << elem2(j) << " )" << finl;
  if (theta > 1) Cerr << "Zone_PolyMAC : face non plane detectee! Veuillez reparer votre maillage..." << finl, Process::exit();
}

void Zone_PolyMAC::calculer_h_carre()
{
  // Calcul de h_carre
  h_carre = 1.e30;
  h_carre_.resize(nb_faces());
  // Calcul des surfaces
  Elem_geom_base& elem_geom = zone().type_elem().valeur();
  int is_polyedre = sub_type(Poly_geom_base, elem_geom) ? 1 : 0;
  const ArrOfInt PolyIndex = is_polyedre ? ref_cast(Poly_geom_base, zone().type_elem().valeur()).getElemIndex() : ArrOfInt(0);

  const DoubleVect& surfaces=face_surfaces();
  const int nbe=nb_elem();

  for (int num_elem=0; num_elem<nbe; num_elem++)
    {
      double surf_max = 0;
      const int nb_faces_elem = is_polyedre ? PolyIndex[num_elem+1] - PolyIndex[num_elem] : zone().nb_faces_elem();
      for (int i=0; i<nb_faces_elem; i++)
        {
          double surf = surfaces(elem_faces(num_elem,i));
          surf_max = (surf > surf_max)? surf : surf_max;
        }
      double vol = volumes(num_elem)/surf_max;
      vol *= vol;
      h_carre_(num_elem) = vol;
      h_carre = ( vol < h_carre )? vol : h_carre;
    }
}

void Zone_PolyMAC::remplir_elem_faces()
{
  creer_faces_virtuelles_non_std();
}

void Zone_PolyMAC::modifier_pour_Cl(const Conds_lim& conds_lim)
{


  //if (volumes_sommets_thilde_.size()!=nb_som())
  {

    Cerr << "La Zone_PolyMAC a ete remplie avec succes" << finl;

    //      calculer_h_carre();
    // Calcul des porosites

    // les porosites sommets ne servent pas
    //calculer_porosites_sommets();


  }
  Journal() << "Zone_PolyMAC::Modifier_pour_Cl" << finl;
  int nb_cond_lim=conds_lim.size();
  int num_cond_lim=0;
  for (; num_cond_lim<nb_cond_lim; num_cond_lim++)
    {
      //for cl
      const Cond_lim_base& cl = conds_lim[num_cond_lim].valeur();
      if (sub_type(Periodique, cl))
        {
          //if Perio
          const Periodique& la_cl_period = ref_cast(Periodique,cl);
          int nb_faces_elem = zone().nb_faces_elem();
          const Front_VF& la_front_dis = ref_cast(Front_VF,cl.frontiere_dis());
          int ndeb = 0;
          int nfin = la_front_dis.nb_faces_tot();
#ifndef NDEBUG
          int num_premiere_face = la_front_dis.num_premiere_face();
          int num_derniere_face = num_premiere_face+nfin;
#endif
          int nbr_faces_bord = la_front_dis.nb_faces();
          assert((nb_faces()==0)||(ndeb<nb_faces()));
          assert(nfin>=ndeb);
          int elem1,elem2,k;
          int face;
          // Modification des tableaux face_voisins_ , face_normales_ , volumes_entrelaces_
          // On change l'orientation de certaines normales
          // de sorte que les normales aux faces de periodicite soient orientees
          // de face_voisins(la_face_en_question,0) vers face_voisins(la_face_en_question,1)
          // comme le sont les faces internes d'ailleurs

          DoubleVect C1C2(dimension);
          double vol,psc=0;

          for (int ind_face=ndeb; ind_face<nfin; ind_face++)
            {
              //for ind_face
              face = la_front_dis.num_face(ind_face);
              if  ( (face_voisins_(face,0) == -1) || (face_voisins_(face,1) == -1) )
                {
                  int faassociee = la_front_dis.num_face(la_cl_period.face_associee(ind_face));
                  if (ind_face<nbr_faces_bord)
                    {
                      assert(faassociee>=num_premiere_face);
                      assert(faassociee<num_derniere_face);
                    }

                  elem1 = face_voisins_(face,0);
                  elem2 = face_voisins_(faassociee,0);
                  vol = (volumes_[elem1] + volumes_[elem2])/nb_faces_elem;
                  volumes_entrelaces_[face] = vol;
                  volumes_entrelaces_[faassociee] = vol;
                  face_voisins_(face,1) = elem2;
                  face_voisins_(faassociee,0) = elem1;
                  face_voisins_(faassociee,1) = elem2;
                  psc = 0;
                  for (k=0; k<dimension; k++)
                    {
                      C1C2[k] = xv_(face,k) - xp_(face_voisins_(face,0),k);
                      psc += face_normales_(face,k)*C1C2[k];
                    }

                  if (psc < 0)
                    for (k=0; k<dimension; k++)
                      face_normales_(face,k) *= -1;

                  for (k=0; k<dimension; k++)
                    face_normales_(faassociee,k) = face_normales_(face,k);
                }
            }
        }
    }

  // PQ : 10/10/05 : les faces periodiques etant a double contribution
  //		      l'appel a marquer_faces_double_contrib s'effectue dans cette methode
  //		      afin de pouvoir beneficier de conds_lim.
  Zone_VF::marquer_faces_double_contrib(conds_lim);
}


// Description:
//   creation de l'espace distant pour les faces virtuelles;
//   creation du tableau des faces virtuelles de bord
void Zone_PolyMAC::creer_faces_virtuelles_non_std()

{
  ind_faces_virt_non_std_.resize_array(314);
  ind_faces_virt_non_std_ = -999;
#if 0
  int i,j,id_joint;
  int nb_faces_front=premiere_face_int();
  int nb_faces_virt=zone().ind_faces_virt_bord().size_array();


  // Constitution du tableau des indices de faces
  // virtuelles non standards.
  int prem_face_std=premiere_face_std();
  IntVect ind_faces_nstd(nb_faces_non_std());
  IntVect ind_faces(nb_faces_);
  const VectEsp_Virt& vev_id_f = ind_faces.renvoi_espaces_virtuels();

  for(j=0; j<nb_faces_; j++)
    ind_faces[j]=j;
  for(j=premiere_face_int(); j<prem_face_std; j++)
    ind_faces_nstd[j]=j;

  for(id_joint=0; id_joint<nb_joints(); id_joint++)
    {
      Joint& le_joint = joint(id_joint);
      int PEvoisin=le_joint.PEvoisin();
      const ArrOfInt& edf=le_joint.esp_dist_faces();

      int nbfd = edf.size_array();
      ind_faces.ajoute_espace_distant(PEvoisin,edf);
      ArrOfInt tempo(nbfd);
      int cpt=0;
      for(i=0; i<nbfd; i++)
        if((edf[i]<prem_face_std)&&(edf[i]>=nb_faces_front))
          tempo[cpt++]=edf[i];
      tempo.resize_array(cpt);
      ind_faces_nstd.ajoute_espace_distant(PEvoisin,tempo);
    }
  ind_faces.echange_espace_virtuel();
  ind_faces_nstd.echange_espace_virtuel();

  const VectEsp_Virt& vev_id_fnstd =
    ind_faces_nstd.renvoi_espaces_virtuels();
  ind_faces_virt_non_std_.resize_array(nb_faces_virt);
  for(id_joint=0; id_joint<nb_joints(); id_joint++)
    {
      int deb=vev_id_f[id_joint].deb();
      int fin=deb+vev_id_f[id_joint].nb();
      int deb_b=vev_id_fnstd[id_joint].deb();
      int fin_b=deb_b+vev_id_fnstd[id_joint].nb();
      for(i=deb_b; i<fin_b; i++)
        {
          for(j=deb; j<fin; j++)
            if(ind_faces[j]==ind_faces_nstd[i])
              break;
          assert(j<fin);
          ind_faces_virt_non_std_[j-nb_faces_]=i;
        }
    }
#endif
}

DoubleVect& Zone_PolyMAC::dist_norm_bord(DoubleVect& dist, const Nom& nom_bord) const
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
  return dist;
}

/* "clamping" a 0 des coeffs petits dans M1/W1/M2/W2 */
inline void clamp(DoubleTab &m)
{
  for (int i = 0; i < m.dimension(0); i++) for (int j = 0; j < m.dimension(1); j++) for (int n = 0; n < m.dimension(2); n++)
    if (1e6 * std::abs(m(i, j, n)) < std::abs(m(i, i, n)) + std::abs(m(j, j, n))) m(i, j, n) = 0;
}

//matrices locales par elements (operateurs de Hodge) permettant de faire des interpolations :
//normales aux faces -> tangentes aux faces duales : (nu x_ef.v) = m2 (|f|n_ef.v)
void Zone_PolyMAC::M2(const DoubleTab *nu, int e, DoubleTab& m2) const
{
  int i, j, k, f, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_f, d, D = dimension;
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& xe = xp(), &xf = xv(), &nf = face_normales();
  const DoubleVect& ve = volumes();
  for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++; //nombre de faces de e
  double prefac, fac, beta = n_f == D + 1 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  m2.resize(n_f, n_f, N), m2 = 0;
  DoubleTrav v_e(n_f, D), v_ef(n_f, n_f, D); //interpolations du vecteur complet : non stabilisee en e, stabilisee en (e, f)
  for (i = 0; i < n_f; i++) for (f = e_f(e, i), d = 0; d < D; d++) v_e(i, d) = (xf(f, d) - xe(e, d)) / ve(e);
  for (i = 0; i < n_f; i++) for (f = e_f(e, i), prefac = D * beta / dabs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))), j = 0; j < n_f; j++)
      for (fac = prefac * ((j == i) - (e == f_e(f, 0) ? 1 : -1) * dot(&nf(f, 0), &v_e(j, 0))), d = 0; d < D; d++)
        v_ef(i, j, d) = v_e(j, d) + fac * (xf(f, d) - xe(e, d));
  //matrice!
  for (m2 = 0, i = 0; i < n_f; i++) for (j = 0; j < n_f; j++)
      if (j < i) for (n = 0; n < N; n++) m2(i, j, n) = m2(j, i, n); //sous la diagonale -> avec l'autre cote
      else for (k = 0; k < n_f; k++) for (f = e_f(e, k), fac = dabs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))) / D, n = 0; n < N; n++)
            m2(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ef(k, i, 0), &v_ef(k, j, 0));
  clamp(m2);
}

//tangentes aux faces duales -> normales aux faces : nu|f|n_ef.v = w2.(x_ef.v)
void Zone_PolyMAC::W2(const DoubleTab *nu, int e, DoubleTab& w2) const
{
  int i, j, k, f, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_f, d, D = dimension;
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& xe = xp(), &xf = xv(), &nf = face_normales();
  const DoubleVect& ve = volumes();
  for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++; //nombre de faces de e
  double prefac, fac, beta = n_f == D + 1 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  w2.resize(n_f, n_f, N), w2 = 0;
  DoubleTrav v_e(n_f, D), v_ef(n_f, n_f, D); //interpolations du vecteur complet : non stabilisee en e, stabilisee en (e, f)
  for (i = 0; i < n_f; i++) for (f = e_f(e, i), d = 0; d < D; d++) v_e(i, d) = (e == f_e(f, 0) ? 1 : -1) * nf(f, d) / ve(e);
  for (i = 0; i < n_f; i++) for (f = e_f(e, i), prefac = D * beta * (e == f_e(f, 0) ? 1 : -1) / dabs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))), j = 0; j < n_f; j++)
      for (fac = prefac * ((j == i) - dot(&xf(f, 0), &v_e(j, 0), &xe(e, 0))), d = 0; d < D; d++)
        v_ef(i, j, d) = v_e(j, d) + fac * nf(f, d);
  //matrice!
  for (i = 0; i < n_f; i++) for (j = 0; j < n_f; j++)
      if (j < i) for (n = 0; n < N; n++) w2(i, j, n) = w2(j, i, n); //sous-diagonale -> on copie l'autre cote
      else for (k = 0; k < n_f; k++) for (f = e_f(e, k), fac = dabs(dot(&xf(f, 0), &nf(f, 0), &xe(e, 0))) / D, n = 0; n < N; n++)
            w2(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ef(k, i, 0), &v_ef(k, j, 0));
  clamp(w2);
}

//normales aux aretes duales -> tangentes aux aretes : (nu|a|t_a.v)   = m1 (S_ea.v)
void Zone_PolyMAC::M1(const DoubleTab *nu, int e, DoubleTab& m1) const
{
  int i, j, k, f, a, s, sb, sgn, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_a, d, D = dimension;
  const IntTab& e_f = elem_faces(), &f_s = face_sommets(), &e_a = D < 3 ? e_f : zone().elem_aretes(), &a_s = D < 3 ? f_s : zone().aretes_som(); //en 2D, les aretes sont les faces!
  const DoubleTab& xe = xp(), &xf = xv(), &xs = zone().domaine().coord_sommets();
  const DoubleVect& ve = volumes();
  for (n_a = 0; n_a < e_a.dimension(1) && e_a(e, n_a) >= 0;) n_a++; //nombre d'aretes autour de e
  double prefac, fac, vecz[3] = { 0, 0, 1 }, beta = n_a == 3 * D - 3 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  m1.resize(n_a, n_a, N), m1 = 0;
  DoubleTrav S_ea(n_a, D), v_e(n_a, D), v_ea(n_a, n_a, D); //vecteur "surface duale x normale" (oriente comme a), interpolations non stabilisees / stabilisees
  //construction de S_ea
  if (D < 3) for (i = 0; i < n_a; i++) //2D : arete <-> face, avec orientation du premier au second sommet de f_s
      {
        auto vec = cross(D, 3, &xf(f = e_f(e, i), 0), vecz, &xe(e, 0)); //rotation de (xf - xe)
        for (sgn = dot(&xs(f_s(f, 1), 0), &vec[0], &xs(f_s(f, 0), 0)) > 0 ? 1 : -1, d = 0; d < D; d++) S_ea(i, d) = sgn * vec[d]; //avec la bonne orientation
      }
  else for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //3D: une contribution par couple (f, a)
        {
          sb = f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0); //autre sommet
          a = som_arete[s].at(sb), k = std::find(&e_a(e, 0), &e_a(e, 0) + n_a, a) - &e_a(e, 0); //arete, son indice dans e_a
          auto vec = cross(D, D, &xf(f, 0), &xa_(a, 0), &xe(e, 0), &xe(e, 0)); //non oriente
          for (sgn = dot(&vec[0], &ta_(a, 0)) >= 0 ? 1 : -1, d = 0; d < D; d++) S_ea(k, d) += sgn * vec[d] / 2;
        }
  //v_e (interpolation non stabilisee)
  for (i = 0; i < n_a; i++) for (a = e_a(e, i), s = a_s(a, 0), sb = a_s(a, 1), d = 0; d < D; d++) v_e(i, d) = (xs(sb, d) - xs(s, d)) / ve(e);
  //v_ea (interpolation stabilisee)
  for (i = 0; i < n_a; i++) for (a = e_a(e, i), s = a_s(a, 0), sb = a_s(a, 1), prefac = D * beta / dot(&xs(sb, 0), &S_ea(i, 0), &xs(s, 0)), j = 0; j < n_a; j++)
      for (fac = prefac * ((j == i) - dot(&S_ea(i, 0), &v_e(j, 0))), d = 0; d < D; d++)
        v_ea(i, j, d) = v_e(j, d) + fac * (xs(sb, d) - xs(s, d));
  //matrice!
  for (i = 0; i < n_a; i++) for (j = 0; j < n_a; j++)
      if (j < i) for (n = 0; n < N; n++) m1(i, j, n) = m1(j, i, n); //sous-diagonale -> on copie l'autre cote
      else for (k = 0; k < n_a; k++) for (a = e_a(e, k), s = a_s(a, 0), sb = a_s(a, 1), fac = dot(&xs(sb, 0), &S_ea(k, 0), &xs(s, 0)) / D, n = 0; n < N; n++)
            m1(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ea(k, i, 0), &v_ea(k, j, 0));
  clamp(m1);
}

//tangentes aux aretes -> normales aux aretes duales : (nuS_ea.v) = w1 (|a|t_a.v)
void Zone_PolyMAC::W1(const DoubleTab *nu, int e, DoubleTab& w1) const
{
  int i, j, k, f, a, s, sb, sgn, n, N = nu ? nu->dimension(1) : 1, e_nu = nu && nu->dimension_tot(0) == 1 ? 0 : e, n_a, d, D = dimension;
  const IntTab& e_f = elem_faces(), &f_s = face_sommets(), &e_a = D < 3 ? e_f : zone().elem_aretes(), &a_s = D < 3 ? f_s : zone().aretes_som(); //en 2D, les aretes sont les faces!
  const DoubleTab& xe = xp(), &xf = xv(), &xs = zone().domaine().coord_sommets();
  const DoubleVect& ve = volumes();
  for (n_a = 0; n_a < e_a.dimension(1) && e_a(e, n_a) >= 0;) n_a++; //nombre d'aretes autour de e
  double prefac, fac, vecz[3] = { 0, 0, 1 }, beta = n_a == 3 * D - 3 ? 1. / D : D == 2 ? 1. / sqrt(2) : 1. / sqrt(3); //stabilisation : DGA sur simplexes, SUSHI sinon
  w1.resize(n_a, n_a, N), w1 = 0;
  DoubleTrav S_ea(n_a, D), v_e(n_a, D), v_ea(n_a, n_a, D); //vecteur "surface duale x normale" (oriente comme a), interpolations non stabilisees / stabilisees
  //construction de S_ea
  if (D < 3) for (i = 0; i < n_a; i++) //2D : arete <-> face, avec orientation du premier au second sommet de f_s
      {
        auto vec = cross(D, 3, &xf(f = e_f(e, i), 0), vecz, &xe(e, 0)); //rotation de (xf - xe)
        for (sgn = dot(&xs(f_s(f, 1), 0), &vec[0], &xs(f_s(f, 0), 0)) > 0 ? 1 : -1, d = 0; d < D; d++) S_ea(i, d) = sgn * vec[d]; //avec la bonne orientation
      }
  else for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //3D: une contribution par couple (f, a)
        {
          sb = f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0); //autre sommet
          a = som_arete[s].at(sb), k = std::find(&e_a(e, 0), &e_a(e, 0) + n_a, a) - &e_a(e, 0); //arete, son indice dans e_a
          auto vec = cross(D, D, &xf(f, 0), &xa_(a, 0), &xe(e, 0), &xe(e, 0)); //non oriente
          for (sgn = dot(&vec[0], &ta_(a, 0)) >= 0 ? 1 : -1, d = 0; d < D; d++) S_ea(k, d) += sgn * vec[d] / 2;
        }
  //v_e (interpolation non stabilisee)
  for (i = 0; i < n_a; i++) for (d = 0; d < D; d++) v_e(i, d) = S_ea(i, d) / ve(e);
  //v_ea (interpolation stabilisee)
  for (i = 0; i < n_a; i++) for (a = e_a(e, i), s = a_s(a, 0), sb = a_s(a, 1), prefac = D * beta / dot(&xs(sb, 0), &S_ea(i, 0), &xs(s, 0)), j = 0; j < n_a; j++)
      for (fac = prefac * ((j == i) - dot(&xs(sb, 0), &v_e(j, 0), &xs(s, 0))), d = 0; d < D; d++)
        v_ea(i, j, d) = v_e(j, d) + fac * S_ea(i, d);
  //matrice!
  for (i = 0; i < n_a; i++) for (j = 0; j < n_a; j++)
      if (j < i) for (n = 0; n < N; n++) w1(i, j, n) = w1(j, i, n); //sous-diagonale -> on copie l'autre cote
      else for (k = 0; k < n_a; k++) for (a = e_a(e, k), s = a_s(a, 0), sb = a_s(a, 1), fac = dot(&xs(sb, 0), &S_ea(k, 0), &xs(s, 0)) / D, n = 0; n < N; n++)
            w1(i, j, n) += fac * nu_dot(nu, e_nu, n, &v_ea(k, i, 0), &v_ea(k, j, 0));
  clamp(w1);
}

void Zone_PolyMAC::init_equiv() const
{
  if (is_init["equiv"]) return;
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& nf = face_normales();
  const DoubleVect& fs = face_surfaces();//, &vf = volumes_entrelaces();
  int i, j, e1, e2, f, f1, f2, d, D = dimension, ok;

  IntTrav ntot, nequiv;
  creer_tableau_faces(ntot), creer_tableau_faces(nequiv);
  equiv.resize(nb_faces_tot(), 2, e_f.dimension(1));
  Cerr << zone().domaine().le_nom() << " : initialisation de equiv... ";
  for (f = 0, equiv = -1; f < nb_faces_tot(); f++) if ((e1 = f_e(f, 0)) >= 0 && (e2 = f_e(f, 1)) >= 0)
      for (i = 0; i < e_f.dimension(1) && (f1 = e_f(e1, i)) >= 0; i++)
        for (j = 0, ntot(f)++; j < e_f.dimension(1) && (f2 = e_f(e2, j)) >= 0; j++)
          {
            if (dabs(dabs(dot(&nf(f1, 0), &nf(f2, 0)) / (fs(f1) * fs(f2))) - 1) > 1e-6) continue; //normales colineaires?
            for (ok = 1, d = 0; d < D; d++) ok &= dabs((xv_(f1, d) - xp_(e1, d)) - (xv_(f2, d) - xp_(e2, d))) < 1e-12; //xv - xp identiques?
            if (!ok) continue;
            equiv(f, 0, i) = f2, equiv(f, 1, j) = f1, nequiv(f)++; //si oui, on a equivalence
          }
  Cerr << mp_somme_vect(nequiv) * 100. / mp_somme_vect(ntot) << "% de faces equivalentes!" << finl;
  is_init["equiv"] = 1;
}
