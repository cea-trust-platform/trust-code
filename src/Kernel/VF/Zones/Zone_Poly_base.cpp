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
// File:        Zone_Poly_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/VF/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Linear_algebra_tools_impl.h>
#include <Connectivite_som_elem.h>
#include <MD_Vector_composite.h>
#include <MD_Vector_tools.h>
#include <Comm_Group_MPI.h>
#include <Quadrangle_VEF.h>
#include <communications.h>
#include <Zone_Poly_base.h>
#include <Matrice_Morse.h>
#include <Segment_poly.h>
#include <Statistiques.h>
#include <Hexaedre_VEF.h>
#include <Matrix_tools.h>
#include <unordered_map>
#include <Array_tools.h>
#include <Quadri_poly.h>
#include <Tetra_poly.h>
#include <TRUSTLists.h>
#include <Tetraedre.h>
#include <Rectangle.h>
#include <PE_Groups.h>
#include <Hexa_poly.h>
#include <Tri_poly.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <EFichier.h>
#include <SFichier.h>
#include <Segment.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EChaine.h>
#include <unistd.h>
#include <Lapack.h>
#include <numeric>
#include <vector>
#include <tuple>
#include <cmath>
#include <set>
#include <map>

#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDCouplingMemArray.hxx>

using namespace MEDCoupling;
#endif
#include <Dirichlet_homogene.h>
#include <Dirichlet_paroi_defilante.h>
#include <Navier.h>


Implemente_base(Zone_Poly_base,"Zone_Poly_base",Zone_VF);


//// printOn
//

Sortie& Zone_Poly_base::ecrit(Sortie& os) const
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

Sortie& Zone_Poly_base::printOn(Sortie& os) const
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

Entree& Zone_Poly_base::readOn(Entree& is)
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
void Zone_Poly_base::reordonner(Faces& les_faces)
{
  if (Process::je_suis_maitre())
    Cerr << "Zone_Poly_base::reordonner faces " << finl;

  // Construction de rang_elem_non_std_ :
  //  C'est un vecteur indexe par les elements du domaine.
  //  size() = nb_elem()
  //  size_tot() = nb_elem_tot()
  //  Valeurs dans le tableau :
  //   rang_elem_non_std_[i] = -1 si l'element i est standard,
  //  sinon
  //   rang_elem_non_std_[i] = j, ou j est l'indice de l'element dans
  //   les tableaux indexes par les elements non standards (par exemple
  //   le tableau Zone_Cl_Poly_base::type_elem_Cl_).
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

void Zone_Poly_base::typer_elem(Zone& zone_geom)
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

void Zone_Poly_base::discretiser()
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


  if (sub_type(Segment_poly,type_elem_.valeur()))
    {
      if (!sub_type(Segment,elem_geom))
        {
          Cerr << " The type of the element " << elem_geom.que_suis_je() << " is incorrect" << finl;
          Process::exit();
        }
    }
  else if (sub_type(Tri_poly,type_elem_.valeur()))
    {
      if (!sub_type(Triangle,elem_geom))
        {
          Cerr << " The type of the element " << elem_geom.que_suis_je() << " is incorrect" << finl;
          Cerr << " Only the Triangle type is compatible with the PolyMAC_P0 discretisation in dimension 2" << finl;
          Cerr << " You must triangulate the domain when using the TRUST mesher" ;
          Cerr << " This can be done by adding : Trianguler nom_dom" << finl;
          Process::exit();
        }
    }
  else if (sub_type(Tetra_poly,type_elem_.valeur()))
    {
      if (!sub_type(Tetraedre,elem_geom))
        {
          Cerr << " The type of the element " << elem_geom.que_suis_je() << " is incorrect" << finl;
          Cerr << " Only the Tetrahedral type is compatible with the PolyMAC_P0 discretisation in dimension 3" << finl;
          Cerr << " You must tetrahedrize the domain when using the TRUST mesher" ;
          Cerr << " This can be done by adding : Tetraedriser nom_dom" << finl;
          Process::exit();
        }
    }

  else if (sub_type(Quadri_poly,type_elem_.valeur()))
    {

      if (!sub_type(Quadrangle_VEF,elem_geom))
        {
          Cerr << " The type of the element " << elem_geom.que_suis_je() << " is incorrect" << finl;
          Process::exit();
        }
    }
  else if (sub_type(Hexa_poly,type_elem_.valeur()))
    {

      if (!sub_type(Hexaedre_VEF,elem_geom))
        {
          Cerr << " The type of the element " << elem_geom.que_suis_je() << " is incorrect" << finl;
          Process::exit();
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
                  Process::exit();
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
      if (S == 0 && sub_type(Hexa_poly,type_elem_.valeur()))
        {
          Cerr << "===============================" << finl;
          Cerr << "Error in your mesh for " << que_suis_je() << "!" << finl;
          Cerr << "Add this keyword before discretization in your data file to create polyedras:" << finl;
          Cerr << "Polyedriser " << zone().domaine().le_nom() << finl;
          Process::exit();
        }
      for (int r = 0; r < 3; r++) xv_(face, r) = xs[r] / S;
    }
  xv_.echange_espace_virtuel();

  detecter_faces_non_planes();

  //volumes_entrelaces_ et volumes_entrelaces_dir : par projection de l'amont/aval sur la normale a la face
  creer_tableau_faces(volumes_entrelaces_);
  volumes_entrelaces_dir_.resize(0, 2), creer_tableau_faces(volumes_entrelaces_dir_);
  for (int f = 0, i, e; f < nb_faces(); f++)
    for (i = 0; i < 2 && (e = face_voisins_(f, i)) >= 0; volumes_entrelaces_(f) += volumes_entrelaces_dir_(f, i), i++)
      volumes_entrelaces_dir_(f, i) = std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0)));
  volumes_entrelaces_.echange_espace_virtuel(), volumes_entrelaces_dir_.echange_espace_virtuel();

  Zone_VF::calculer_porosites();
  Zone_VF::calculer_diametres_hydrauliques();
  // calculer_h_carre();
  /* ordre canonique dans elem_faces_ */
  std::map<std::array<double, 3>, int> xv_fsa;
  for (int e = 0, i, j, f; e < nb_elem_tot(); e++)
    {
      for (i = 0, j = 0, xv_fsa.clear(); i < elem_faces_.dimension(1) && (f = elem_faces_(e, i)) >= 0; i++)
        xv_fsa[ {{ xv_(f, 0), xv_(f, 1), dimension < 3 ? 0 : xv_(f, 2) }}] = f;
      for (auto &&c_f : xv_fsa) elem_faces_(e, j) = c_f.second, j++;
    }
}

void Zone_Poly_base::detecter_faces_non_planes() const
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
  Cerr << "Zone_Poly_base : angle sommet/face max " << theta << " deg (proc " << j << " , face ";
  Cerr << face(j) << " , elems " << elem1(j) << " / " << elem2(j) << " )" << finl;
  if (theta > 1) Cerr << "Zone_Poly_base : non-planar face detected ! Please fix your mesh or call 911 ..." << finl, Process::exit();
}

void Zone_Poly_base::calculer_h_carre()
{
  // Calcul de h_carre
  h_carre = 1.e30;
  h_carre_.resize(nb_faces());
  // Calcul des surfaces
  const DoubleVect& surfaces=face_surfaces();
  const int nb_faces_elem=zone().nb_faces_elem();
  const int nbe=nb_elem();
  for (int num_elem=0; num_elem<nbe; num_elem++)
    {
      double surf_max = 0;
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

void disp_dt(const DoubleTab& A)
{
  int i, j, k, l;
  if (A.nb_dim() == 4) for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "}}},{" : "{{"); j < A.dimension(1); j++)
        for (k = 0, fprintf(stderr, j ? "}},{" : "{"); k < A.dimension(2); k++)
          for (l = 0, fprintf(stderr, k ? "},{" : "{"); l < A.dimension(3); l++)
            fprintf(stderr, "%.10E%s ", A.addr()[A.dimension(3) * (A.dimension(2) * (i * A.dimension(1) + j) + k) + l], l + 1 < A.dimension(3) ? "," : "");
  else if (A.nb_dim() == 3) for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "}},{" : "{{"); j < A.dimension(1); j++)
        for (k = 0, fprintf(stderr, j ? "},{" : "{"); k < A.dimension(2); k++)
          fprintf(stderr, "%.10E%s ", A.addr()[A.dimension(2) * (i * A.dimension(1) + j) + k], k + 1 < A.dimension(2) ? "," : "");
  else if (A.nb_dim() == 2) for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "},{" : "{{"); j < A.dimension(1); j++)
        fprintf(stderr, "%.10E%s ", A.addr()[i * A.dimension(1) + j], j + 1 < A.dimension(1) ? "," : "");
  else for (i = 0, fprintf(stderr, "{"); i < A.dimension_tot(0); i++)
      fprintf(stderr, "%.10E%s ", A.addr()[i], i + 1 < A.dimension_tot(0) ? "," : "");
  fprintf(stderr, A.nb_dim() == 4 ? "}}}}\n" : A.nb_dim() == 3 ? "}}}\n" : A.nb_dim() == 2 ? "}}\n" : "}\n");
}

void disp_da(const ArrOfDouble& A)
{
  int i;
  for (i = 0, fprintf(stderr, "{"); i < A.size_array(); i++)
    fprintf(stderr, "%.10E%s ", A.addr()[i], i + 1 < A.size_array() ? "," : "");
  fprintf(stderr, "}\n");
}

void disp_it(const IntTab& A)
{
  int i, j, k, l;
  if (A.nb_dim() == 4) for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "}}},{" : "{{"); j < A.dimension(1); j++)
        for (k = 0, fprintf(stderr, j ? "}},{" : "{"); k < A.dimension(2); k++)
          for (l = 0, fprintf(stderr, k ? "},{" : "{"); l < A.dimension(3); l++)
            fprintf(stderr, "%d%s ", A.addr()[A.dimension(3) * (A.dimension(2) * (i * A.dimension(1) + j) + k) + l], l + 1 < A.dimension(3) ? "," : "");
  else if (A.nb_dim() == 3) for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "}},{" : "{{"); j < A.dimension(1); j++)
        for (k = 0, fprintf(stderr, j ? "},{" : "{"); k < A.dimension(2); k++)
          fprintf(stderr, "%d%s ", A.addr()[A.dimension(2) * (i * A.dimension(1) + j) + k], k + 1 < A.dimension(2) ? "," : "");
  else if (A.nb_dim() == 2) for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "},{" : "{{"); j < A.dimension(1); j++)
        fprintf(stderr, "%d%s ", A.addr()[i * A.dimension(1) + j], j + 1 < A.dimension(1) ? "," : "");
  else for (i = 0, fprintf(stderr, "{"); i < A.dimension_tot(0); i++)
      fprintf(stderr, "%d%s ", A.addr()[i], i + 1 < A.dimension_tot(0) ? "," : "");
  fprintf(stderr, A.nb_dim() == 4 ? "}}}}\n" : A.nb_dim() == 3 ? "}}}\n" : A.nb_dim() == 2 ? "}}\n" : "}\n");
}

void disp_ia(const ArrOfInt& A)
{
  int i;
  for (i = 0, fprintf(stderr, "{"); i < A.size_array(); i++)
    fprintf(stderr, "%d%s ", A.addr()[i], i + 1 < A.size_array() ? "," : "");
  fprintf(stderr, "}\n");
}

void disp_m(const Matrice_Base& M_in)
{
  Matrice_Morse M;
  Matrix_tools::convert_to_morse_matrix(M_in, M);
  DoubleTab A(M.nb_lignes(), M.nb_colonnes());
  for (int i = 0; i < A.dimension(0); i++)
    for (int k = M.get_tab1().addr()[i] - 1; k < M.get_tab1().addr()[i + 1] - 1; k++)
      A(i, M.get_tab2().addr()[k] - 1) = M.get_coeff().addr()[k];
  disp_dt(A);
}

void disp_mt(const tabs_t& mvect)
{
  for (auto &&n_v : mvect)
    fprintf(stderr, "%s:\n", n_v.first.c_str()), disp_dt(n_v.second), fprintf(stderr, "\n");
}

void disp_mm(const std::map<std::string, Matrice_Morse>& mmat)
{
  for (auto &&n_v : mmat)
    fprintf(stderr, "%s:\n", n_v.first.c_str()), disp_m(n_v.second), fprintf(stderr, "\n");
}

void disp_mmp(const matrices_t& mmat)
{
  for (auto &&n_v : mmat)
    fprintf(stderr, "%s:\n", n_v.first.c_str()), disp_m(*n_v.second), fprintf(stderr, "\n");
}

DoubleVect& Zone_Poly_base::dist_norm_bord(DoubleVect& dist, const Nom& nom_bord) const
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

void Zone_Poly_base::init_equiv() const
{
  if (is_init["equiv"]) return;
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& nf = face_normales();
  const DoubleVect& fs = face_surfaces();//, &vf = volumes_entrelaces();
  int i, j, e1, e2, f, f1, f2, d, D = dimension, ok;

  IntTrav ntot, nequiv;
  creer_tableau_faces(ntot), creer_tableau_faces(nequiv);
  equiv.resize(nb_faces_tot(), 2, e_f.dimension(1));
  Cerr << zone().domaine().le_nom() << " : intializing equiv... ";
  for (f = 0, equiv = -1; f < nb_faces_tot(); f++) if ((e1 = f_e(f, 0)) >= 0 && (e2 = f_e(f, 1)) >= 0)
      for (i = 0; i < e_f.dimension(1) && (f1 = e_f(e1, i)) >= 0; i++)
        for (j = 0, ntot(f)++; j < e_f.dimension(1) && (f2 = e_f(e2, j)) >= 0; j++)
          {
            if (std::fabs(std::fabs(dot(&nf(f1, 0), &nf(f2, 0)) / (fs(f1) * fs(f2))) - 1) > 1e-6) continue; //normales colineaires?
            for (ok = 1, d = 0; d < D; d++) ok &= std::fabs((xv_(f1, d) - xp_(e1, d)) - (xv_(f2, d) - xp_(e2, d))) < 1e-12; //xv - xp identiques?
            if (!ok) continue;
            equiv(f, 0, i) = f2, equiv(f, 1, j) = f1, nequiv(f)++; //si oui, on a equivalence
          }
  Cerr << mp_somme_vect(nequiv) * 100. / mp_somme_vect(ntot) << "% equivalent faces !" << finl;
  is_init["equiv"] = 1;
}

void Zone_Poly_base::init_som_elem() const
{
  if (is_init["som_elem"]) return;
  construire_connectivite_som_elem(zone().domaine().nb_som_tot(), zone().les_elems(), som_elem, 1);
  is_init["som_elem"] = 1;
}

void Zone_Poly_base::init_dist_paroi_globale(const Conds_lim& conds_lim) // Methode inspiree de Raccord_distant_homogene::initialise
{
  if(dist_paroi_initialisee_) return;

  const Zone_Poly_base& zone = *this;
  int D=Objet_U::dimension, nf = zone.nb_faces(), ne = zone.nb_elem();
  const IntTab& f_s = face_sommets();
  const DoubleTab& xs = zone.zone().domaine().coord_sommets();

  // On initialise les tables y_faces_ et y_elem_
  zone.creer_tableau_faces(y_faces_);
  zone.zone().creer_tableau_elements(y_elem_);

  n_y_elem_.resize(0,D);
  n_y_faces_.resize(0,D);

  MD_Vector_tools::creer_tableau_distribue(y_elem_.get_md_vector(), n_y_elem_);
  MD_Vector_tools::creer_tableau_distribue(y_faces_.get_md_vector(), n_y_faces_);

  // On va identifier les faces par leur centres de gravite
  int parts = Process::nproc();
  int moi = Process::me();
  DoubleTabs remote_xv(parts);

  // On initialise la table de faces/sommets/aretes de bords locale, on cree une table de sommets locale et on compte les aretes
  int nb_faces_bord = 0;
  int nb_aretes = 0;
  std::set<int> soms;
  for (int ind_cl = 0 ; ind_cl < conds_lim.size() ; ind_cl++)
    if ( sub_type(Dirichlet_paroi_defilante, conds_lim(ind_cl).valeur()) || sub_type(Dirichlet_homogene, conds_lim(ind_cl).valeur()) || sub_type(Navier, conds_lim(ind_cl).valeur()) )
      {
        int num_face_1_cl = conds_lim(ind_cl).frontiere_dis().frontiere().num_premiere_face();
        int nb_faces_cl   = conds_lim(ind_cl).frontiere_dis().frontiere().nb_faces();

        nb_faces_bord += conds_lim(ind_cl).frontiere_dis().frontiere().nb_faces();

        for (int f=num_face_1_cl ; f < nb_faces_cl+num_face_1_cl ; f++)
          {
            int nb_som_loc = 0;
            while ( (nb_som_loc < nb_som_face()) && (f_s(f, nb_som_loc) != -1))
              {
                soms.insert(f_s(f, nb_som_loc));
                nb_som_loc++;
              }
            nb_aretes += (D == 3 ? nb_som_loc : 0)  ; // Autant d'aretes autour d'une face que de sommets !
          }
      }
  remote_xv[moi].resize(nb_faces_bord + soms.size() + nb_aretes,D);

  // On remplit les coordonnes des faces et aretes de bord locales
  int ind_tab = 0 ; // indice de la face/sommet/arete dans le tableau
  for (int ind_cl = 0 ; ind_cl < conds_lim.size() ; ind_cl++)
    if ( sub_type(Dirichlet_paroi_defilante, conds_lim(ind_cl).valeur()) || sub_type(Dirichlet_homogene, conds_lim(ind_cl).valeur()) || sub_type(Navier, conds_lim(ind_cl).valeur()) )
      {
        int num_face_1_cl = conds_lim(ind_cl).frontiere_dis().frontiere().num_premiere_face();
        int nb_faces_cl   = conds_lim(ind_cl).frontiere_dis().frontiere().nb_faces();

        for (int f=num_face_1_cl ; f < nb_faces_cl+num_face_1_cl ; f++)
          {
            for (int d=0 ; d<D ; d++) remote_xv[moi](ind_tab,d) = zone.xv(f, d); // Remplissage des faces
            ind_tab++;

            if (D==3) // Remplissage des aretes
              {
                int id_som = 1 ;
                while ( (id_som < nb_som_face()) && (f_s(f, id_som) != -1))
                  {
                    for (int d=0 ; d<D ; d++) remote_xv[moi](ind_tab,d) = (xs(f_s(f, id_som), d) + xs(f_s(f, id_som-1), d)) / 2;
                    id_som++;
                    ind_tab++;
                  }
                for (int d=0 ; d<D ; d++) remote_xv[moi](ind_tab,d) = (xs(f_s(f, 0), d) + xs(f_s(f, id_som-1), d)) / 2;
                ind_tab++;
              }
          }
      }

  for (auto som:soms) // Remplissage des sommets
    {
      for (int d=0 ; d<D ; d++) remote_xv[moi](ind_tab,d) = xs(som, d);
      ind_tab++;
    }

  // Puis on echange les tableaux des centres de gravites
  // envoi des tableaux
  for (int p = 0; p < parts; p++)
    envoyer_broadcast(remote_xv[p], p);

  VECT(ArrOfInt) racc_vois(parts);
  for (int p = 0; p < parts; p++)
    racc_vois[p].set_smart_resize(1);

#ifdef MEDCOUPLING_
  // On traite les informations, chaque proc connait tous les XV

  // On boucle sur toutes les faces puis tous les elems
  const DoubleTab& local_xv = zone.xv(),
                   & local_xp = zone.xp();

  //DataArrayDoubles des xv locaux et de tous les remote_xv (a la suite)
  std::vector<MCAuto<DataArrayDouble> > vxv(parts);
  std::vector<const DataArrayDouble*> cvxv(parts);
  for (int p = 0; p < parts; p++)
    {
      vxv[p] = DataArrayDouble::New();
      vxv[p]->useExternalArrayWithRWAccess(remote_xv[p].addr(), remote_xv[p].dimension(0), remote_xv[p].dimension(1));
      cvxv[p] = vxv[p];
    }
  MCAuto<DataArrayDouble> remote_xvs(DataArrayDouble::Aggregate(cvxv)), local_xs(DataArrayDouble::New());
  local_xs->alloc(nf+ne, D);
  for (int f = 0; f < nf; f++) for (int d = 0; d < D; d++)
      local_xs->setIJ(f, d, local_xv(f, d));
  for (int e = 0; e < ne; e++) for (int d = 0; d < D; d++)
      local_xs->setIJ(nf+e, d, local_xp(e, d));

  //indices des points de remote_xvs les plus proches de chaque point de local_xv
  MCAuto<DataArrayInt> glob_idx(DataArrayInt::New());
  glob_idx = remote_xvs->findClosestTupleId(local_xs);

  //pour chaque element et face de local_xs : remplissage des tableaux
  for (int fe = 0; fe<nf+ne; fe++)
    {
      //retour de l'indice global (glob_idx(ind_face)) au couple (proc, ind_face2)
      int proc = 0, fe2 = glob_idx->getIJ(fe, 0);
      while (fe2 >= remote_xv[proc].dimension(0)) fe2 -= remote_xv[proc].dimension(0), proc++;
      assert(fe2 <  remote_xv[proc].dimension(0));

      double distance2 = 0;
      for (int d=0; d<D; d++)
        {
          double x1 = 0 ;
          if (fe<nf) x1=local_xv(fe,d);
          else if (fe<nf+ne) x1=local_xp(fe-nf,d);
          else { Cerr<<"Zone_Poly_base::init_dist_bord : problem in the ditance to the edge calculation. Contact TRUST support."<<finl; Process::exit();}
          double x2=remote_xv[proc](fe2,d);
          distance2 += (x1-x2)*(x1-x2);
        }
      if (fe<nf)
        {
          y_faces_(fe) = std::sqrt(distance2);
          if (y_faces_(fe)>1.e-8) for (int d = 0 ; d<D ; d++) n_y_faces_(fe, d) = ( local_xv(fe,d)-remote_xv[proc](fe2,d) )/ y_faces_(fe);
        }
      else
        {
          y_elem_(fe-nf)  = std::sqrt(distance2);
          for (int d = 0 ; d<D ; d++) n_y_elem_(fe-nf, d) = ( local_xp(fe-nf,d)-remote_xv[proc](fe2,d) )/ y_elem_(fe-nf);
        }
    }

#else
  Cerr<<"Zone_Poly_base::init_dist_bord needs TRUST compiled with MEDCoupling."<<finl;
  exit();
#endif

  // Pour les elems de bord, on calcule la distance de facon propre avec le produit scalaire
  for (int ind_cl = 0 ; ind_cl < conds_lim.size() ; ind_cl++)
    if ( sub_type(Dirichlet_paroi_defilante, conds_lim(ind_cl).valeur()) || sub_type(Dirichlet_homogene, conds_lim(ind_cl).valeur()) || sub_type(Navier, conds_lim(ind_cl).valeur()) )
      {
        int num_face_1_cl = conds_lim(ind_cl).frontiere_dis().frontiere().num_premiere_face();
        int nb_faces_cl   = conds_lim(ind_cl).frontiere_dis().frontiere().nb_faces();

        for (int f=num_face_1_cl ; f < nb_faces_cl+num_face_1_cl ; f++)
          if ( dist_face_elem0(f, face_voisins(f, 0)) < y_elem_(face_voisins(f, 0)) ) // Prise en compte du cas ou l'element a plusieurs faces de bord
            {
              y_elem_(face_voisins(f, 0)) = dist_face_elem0(f, face_voisins(f, 0)) ;
              for (int d = 0 ; d<D ; d++)
                {
                  n_y_elem_(face_voisins(f, 0), d) = -face_normales(f,  d)/face_surfaces(f);
                  n_y_faces_ = -face_normales(f,  d)/face_surfaces(f);
                }
            }
      }

  y_faces_.echange_espace_virtuel();
  y_elem_.echange_espace_virtuel();
  dist_paroi_initialisee_ = 1;
  Cerr <<"Initialize the y table " << zone.zone().domaine().le_nom();
}
