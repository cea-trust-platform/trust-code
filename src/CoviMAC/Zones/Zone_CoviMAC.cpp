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
// File:        Zone_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Zone_CoviMAC.h>
#include <tuple>
#include <unordered_map>
#include <Rectangle.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <Segment.h>
#include <Tetraedre.h>
#include <Quadrangle_VEF.h>
#include <Hexaedre_VEF.h>
#include <Zone_Cl_CoviMAC.h>
#include <Tri_CoviMAC.h>
#include <Segment_CoviMAC.h>
#include <Tetra_CoviMAC.h>
#include <Quadri_CoviMAC.h>
#include <Hexa_CoviMAC.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EFichier.h>
#include <SFichier.h>
#include <IntList.h>
#include <DoubleList.h>
#include <Connectivite_som_elem.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <EChaine.h>
#include <Comm_Group_MPI.h>
#include <PE_Groups.h>
#include <communications.h>
#include <Statistiques.h>
#include <MD_Vector_composite.h>
#include <MD_Vector_tools.h>
#include <ConstDoubleTab_parts.h>
#include <Lapack.h>
#include <Option_CoviMAC.h>
#include <Echange_contact_CoviMAC.h>
#include <Dirichlet_homogene.h>

#include <LireMED.h>
#include <EcrMED.h>
#include <Champ_implementation_P1.h>
#include <Linear_algebra_tools_impl.h>
#include <IntLists.h>
#include <DoubleLists.h>

#include <unistd.h>

#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <cfloat>
#include <numeric>

#include <cfenv>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <osqp/osqp.h>
#pragma GCC diagnostic pop

Implemente_instanciable(Zone_CoviMAC,"Zone_CoviMAC",Zone_VF);


//// printOn
//

Sortie& Zone_CoviMAC::ecrit(Sortie& os) const
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

Sortie& Zone_CoviMAC::printOn(Sortie& os) const
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

Entree& Zone_CoviMAC::readOn(Entree& is)
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
void Zone_CoviMAC::reordonner(Faces& les_faces)
{
  if (Process::je_suis_maitre())
    Cerr << "Zone_CoviMAC::reordonner les_faces " << finl;

  // Construction de rang_elem_non_std_ :
  //  C'est un vecteur indexe par les elements du domaine.
  //  size() = nb_elem()
  //  size_tot() = nb_elem_tot()
  //  Valeurs dans le tableau :
  //   rang_elem_non_std_[i] = -1 si l'element i est standard,
  //  sinon
  //   rang_elem_non_std_[i] = j, ou j est l'indice de l'element dans
  //   les tableaux indexes par les elements non standards (par exemple
  //   le tableau Zone_Cl_CoviMAC::type_elem_Cl_).
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

void Zone_CoviMAC::typer_elem(Zone& zone_geom)
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

void Zone_CoviMAC::discretiser()
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


  if (sub_type(Segment_CoviMAC,type_elem_.valeur()))
    {
      if (!sub_type(Segment,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          exit();
        }
    }
  else if (sub_type(Tri_CoviMAC,type_elem_.valeur()))
    {
      if (!sub_type(Triangle,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " <<
               elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Triangle est compatible avec la discretisation CoviMAC en dimension 2" << finl;
          Cerr << " Il faut trianguler le domaine lorsqu'on utilise le mailleur interne" ;
          Cerr << " en utilisant l'instruction: Trianguler nom_dom" << finl;
          exit();
        }
    }
  else if (sub_type(Tetra_CoviMAC,type_elem_.valeur()))
    {
      if (!sub_type(Tetraedre,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " <<
               elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Tetraedre est compatible avec la discretisation CoviMAC en dimension 3" << finl;
          Cerr << " Il faut tetraedriser le domaine lorsqu'on utilise le mailleur interne";
          Cerr << " en utilisant l'instruction: Tetraedriser nom_dom" << finl;
          exit();
        }
    }

  else if (sub_type(Quadri_CoviMAC,type_elem_.valeur()))
    {

      if (!sub_type(Quadrangle_VEF,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          exit();
        }
    }
  else if (sub_type(Hexa_CoviMAC,type_elem_.valeur()))
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
      if (S == 0 && sub_type(Hexa_CoviMAC,type_elem_.valeur()))
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

  detecter_faces_non_planes();

  //MD_vector pour Champ_Face_CoviMAC (faces + dimension * champ_p0)
  MD_Vector_composite mdc_ch_face;
  mdc_ch_face.add_part(md_vector_faces());
  mdc_ch_face.add_part(zone().md_vector_elements(), dimension);
  mdv_ch_face.copy(mdc_ch_face);

  //volumes_entrelaces_ et volumes_entrelaces_dir : par projection de l'amont/aval sur la normale a la face
  creer_tableau_faces(volumes_entrelaces_);
  volumes_entrelaces_dir_.resize(0, 2), creer_tableau_faces(volumes_entrelaces_dir_);
  for (int f = 0, i, e; f < nb_faces(); f++)
    for (i = 0; i < 2 && (e = face_voisins_(f, i)) >= 0; volumes_entrelaces_(f) += volumes_entrelaces_dir_(f, i), i++)
      volumes_entrelaces_dir_(f, i) = dabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0)));
  volumes_entrelaces_.echange_espace_virtuel(), volumes_entrelaces_dir_.echange_espace_virtuel();

  Zone_VF::calculer_porosites();
  Zone_VF::calculer_diametres_hydrauliques();
}

void Zone_CoviMAC::detecter_faces_non_planes() const
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
      if ((sin2 = std::pow(dot(&xs(s, 0), &nf(f, 0), &xv_(f, 0)) / fs(f), 2) / dot(&xs(s, 0), &xs(s, 0), &xv_(f, 0), &xv_(f, 0))) > val(rk))
        val(rk) = sin2, face(rk) = f, elem1(rk) = f_e(f, 0), elem2(rk) = f_e(f, 1);
  envoyer_all_to_all(val, val), envoyer_all_to_all(face, face), envoyer_all_to_all(elem1, elem1), envoyer_all_to_all(elem2, elem2);

  for (i = j = sin2 = 0; i < Process::nproc(); i++) if (val(i) > sin2) sin2 = val(i), j = i;
  double theta = asin(sqrt(sin2)) * 180 / M_PI;
  Cerr << "Zone_CoviMAC : angle sommet/face max " << theta << " deg (proc " << j << " , face ";
  Cerr << face(j) << " , elems " << elem1(j) << " / " << elem2(j) << " )" << finl;
  if (theta > 1) Cerr << "Zone_CoviMAC : face non plane detectee! Veuillez reparer votre maillage..." << finl, Process::exit();
}

void Zone_CoviMAC::calculer_h_carre()
{
  // Calcul de h_carre
  h_carre = 1.e30;
  h_carre_.resize(nb_faces());
  // Calcul des surfaces
  const DoubleVect& surfaces=face_surfaces();
  const int& nb_faces_elem=zone().nb_faces_elem();
  const int& nbe=nb_elem();
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
  int i, j, k;
  if (A.nb_dim() == 3) for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "}},{" : "{{"); j < A.dimension(1); j++)
        for (k = 0, fprintf(stderr, j ? "},{" : "{"); k < A.dimension(2); k++)
          fprintf(stderr, "%.10E%s ", A.addr()[A.dimension(2) * (i * A.dimension(1) + j) + k], k + 1 < A.dimension(2) ? "," : "");
  else if (A.nb_dim() == 2) for (i = 0; i < A.dimension_tot(0); i++)
      for (j = 0, fprintf(stderr, i ? "},{" : "{{"); j < A.dimension(1); j++)
        fprintf(stderr, "%.10E%s ", A.addr()[i * A.dimension(1) + j], j + 1 < A.dimension(1) ? "," : "");
  else for (i = 0, fprintf(stderr, "{"); i < A.dimension_tot(0); i++)
      fprintf(stderr, "%.10E%s ", A.addr()[i], i + 1 < A.dimension_tot(0) ? "," : "");
  fprintf(stderr, A.nb_dim() == 3 ? "}}}\n" : (A.nb_dim() == 2 ? "}}\n" : "}\n"));
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
  int i, j;
  if (A.nb_dim() == 2) for (i = 0; i < A.dimension(0); i++)
      for (j = 0, fprintf(stderr, i ? "},{" : "{{"); j < A.dimension(1); j++)
        fprintf(stderr, "%d%s ", A.addr()[i * A.dimension(1) + j], j + 1 < A.dimension(1) ? "," : "");
  else for (i = 0, fprintf(stderr, "{"); i < A.dimension(0); i++)
      fprintf(stderr, "%d%s ", A.addr()[i], i + 1 < A.dimension(0) ? "," : "");
  fprintf(stderr, A.nb_dim() == 2 ? "}}\n" : "}\n");
}

void disp_ia(const ArrOfInt& A)
{
  int i;
  for (i = 0, fprintf(stderr, "{"); i < A.size_array(); i++)
    fprintf(stderr, "%d%s ", A.addr()[i], i + 1 < A.size_array() ? "," : "");
  fprintf(stderr, "}\n");
}

void disp_m(const Matrice_Morse& M)
{
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

DoubleVect& Zone_CoviMAC::dist_norm_bord(DoubleVect& dist, const Nom& nom_bord) const
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


void Zone_CoviMAC::init_equiv() const
{
  if (is_init["equiv"]) return;
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleTab& nf = face_normales();
  const DoubleVect& fs = face_surfaces();
  int i, j, e1, e2, f, f1, f2;

  IntTrav ntot, nequiv;
  creer_tableau_faces(ntot), creer_tableau_faces(nequiv);
  equiv.resize(nb_faces_tot(), 2, e_f.dimension(1));
  Cerr << zone().domaine().le_nom() << " : initialisation de equiv... ";
  for (f = 0, equiv = -1; f < nb_faces_tot(); f++) if ((e1 = f_e(f, 0)) >= 0 && (e2 = f_e(f, 1)) >= 0)
      for (i = 0; i < e_f.dimension(1) && (f1 = e_f(e1, i)) >= 0; i++)
        for (j = 0, ntot(f)++; j < e_f.dimension(1) && (f2 = e_f(e2, j)) >= 0; j++)
          {
            if (dabs(dabs(dot(&nf(f1, 0), &nf(f2, 0)) / (fs(f1) * fs(f2))) - 1) > 1e-6) continue; //normales non colineaires
            if (dot(&xv_(f1, 0), &xv_(f2, 0), &xp_(e1, 0), &xp_(e2, 0)) < 0) continue; //vecteurs (xp - xv) opposes
            auto v = cross(dimension, dimension, &xv_(f1, 0), &xv_(f2, 0), &xp_(e1, 0), &xp_(e2, 0));
            if ((dimension < 3 ? v[2] * v[2] : dot(&v[0], &v[0])) > 1e-12 * fs(f) * fs(f)) continue; //vecteurs (xp - xv) non colineaires
            equiv(f, 0, i) = f2, equiv(f, 1, j) = f1, nequiv(f)++; //si oui, on a equivalence
          }
  Cerr << mp_somme_vect(nequiv) * 100. / mp_somme_vect(ntot) << "% de faces equivalentes!" << finl;
  is_init["equiv"] = 1;
}

//interpolation normales aux faces -> elements d'ordre 1
void Zone_CoviMAC::init_ve() const
{
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleVect& ve = volumes_, &fs = face_surfaces();
  int i, k, e, f;

  if (is_init["ve"]) return;
  Cerr << zone().domaine().le_nom() << " : initialisation de ve... ";
  ved.resize(1), ved.set_smart_resize(1), vej.set_smart_resize(1), vec.resize(0, dimension), vec.set_smart_resize(1);
  //formule (1) de Basumatary et al. (2014) https://doi.org/10.1016/j.jcp.2014.04.033 d'apres Perot
  for (e = 0; e < nb_elem_tot(); ved.append_line(vej.dimension(0)), e++)
    for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
      {
        double x[3] = { 0, };
        for (k = 0; k < dimension; k++) x[k] = fs(f) * (xv(f, k) - xp(e, k)) * (e == f_e(f, 0) ? 1 : -1) / ve(e);
        vej.append_line(f), dimension < 3 ? vec.append_line(x[0], x[1]) : vec.append_line(x[0], x[1], x[2]);
      }
  CRIMP(ved), CRIMP(vej), CRIMP(vec);
  is_init["ve"] = 1, Cerr << "OK" << finl;
}


//stencil face/face : fsten_f([fsten_d(f, 0), fsten_d(f + 1, 0)[)
void Zone_CoviMAC::init_stencils() const
{
  if (is_init["stencils"]) return;
  const IntTab& f_s = face_sommets(), &f_e = face_voisins(), &e_f = elem_faces(), &e_s = zone().les_elems();
  int i, j, k, e, f, fb, s, ns_tot = zone().domaine().nb_som_tot(), ok;
  fsten_d.set_smart_resize(1), fsten_d.resize(1), fsten_f.set_smart_resize(1);

  //is_fb(f) = 1 pour les faces de bord
  IntTrav is_fb(nb_faces_tot());
  for (f = 0; f < premiere_face_int(); f++) is_fb(f) = 1;
  for (f = nb_faces(); f < nb_faces_tot(); f++) is_fb(f) = (zone().ind_faces_virt_bord()(f - nb_faces()) >= 0);

  /* connectivite sommets -> elems / faces de bord */
  std::vector<std::set<int>> som_e(ns_tot), som_f(ns_tot);
  for (e = 0; e < nb_elem_tot(); e++) for (i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++) som_e[s].insert(e);
  for (f = 0; f < nb_faces_tot(); f++) if (is_fb(f)) for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++) som_f[s].insert(f);

  std::set<int> soms, sf_f; //sommets de la face f, elems connectes a e par soms, sommets / faces connectes par une face commune
  for (f = 0; f < nb_faces_tot(); fsten_d.append_line(fsten_f.size()), f++) if (f_e(f, 0) >= 0 && (is_fb(f) || f_e(f, 1) >= 0))
      {
        soms.clear(), sf_f.clear();
        for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++) soms.insert(s);

        /* connectivite par faces + sommets */
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) //tout ce qui touche un sommet de f et est voisin de l'amont/aval
          for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
            {
              for (k = 0, ok = 0; !ok && k < f_s.dimension(1) && (s = f_s(fb, k)) >= 0; k++) ok |= soms.count(s);
              if (!ok) continue; //pas de sommet en commun avec f
              if (fb != f) sf_f.insert(fb); //face
            }
        /* remplissage */
        for (auto && fa : sf_f) fsten_f.append_line(fa);
      }

  CRIMP(fsten_d), CRIMP(fsten_f);
  is_init["stencils"] = 1;
}


//construction des "points harmoniques" aux faces lies au gradient d'un champ aux elements
//entrees : cls          : conditions aux limites
//          is_p         : cas de la pression dans Navier-Stokes (CL de Neumann -> Dirichlet, le reste -> Neumann homogene)
//          nu_grad      : 1 si on veut nf.(nu.grad T), 0 si on veut nf.grad T
//          nu(e, n, ..) : diffusivite aux elements (optionnel)
//          invh(f, n)   : resistivite (1 / h) aux faces de bord (optionnel)
//sorties : xh(f, n, d)           : point harmonique a la face f pour la composante n
//          wh(f, n)              : pour les faces internes : Th(f, n) = wh(f, n) * Te(amont, n) + (1 - wh(f, n)) * Te(aval, n) si interne
//          whm(f_ech, n, 0/1, m) : pour les faces Echange_contact : Th(f, n) = sum_m,i whm(f_ech, n, i, m) T(f_e(f, i), m)
void Zone_CoviMAC::harmonic_points(const Conds_lim& cls, int is_p, int nu_grad, const DoubleTab *nu, const DoubleTab *invh, DoubleTab& xh, DoubleTab& wh, DoubleTab *whm) const
{
  const IntTab& f_e = face_voisins();
  const DoubleTab& nf = face_normales();
  const DoubleVect& fs = face_surfaces();
  int i, j, e, f, fb, n, N = xh.dimension(1), nf_tot = nb_faces_tot(), d, D = dimension;

  DoubleTrav lambda(2, N), xef(2, D), def(2), lambda_t(2, N, D);
  double i3[3][3] = { { 1, 0, 0 }, { 0, 1, 0}, { 0, 0, 1 }};

  /* faces de bord : on delegue les CLs monolithique a Echange_contact_CoviMAC et on traite les autres */
  wh = -1; //valeur par defaut : a la fin, wh = -1 que les faces au bord des joints
  for (i = 0; i < cls.size(); i++)
    if (sub_type(Echange_contact_CoviMAC, cls[i].valeur())) ref_cast(Echange_contact_CoviMAC, cls[i].valeur()).harmonic_points(xh, wh, *whm);
    else /* vraie face de bord */
      {
        const Cond_lim_base& cl = cls[i].valeur();
        const Front_VF& fvf = ref_cast(Front_VF, cl.frontiere_dis());
        int neu  = sub_type(Neumann, cl), neu_h = sub_type(Neumann_homogene, cl), sym = sub_type(Symetrie, cl),
            dir  = sub_type(Dirichlet, cl), dir_h = sub_type(Dirichlet_homogene, cl),
            echg = sub_type(Echange_impose_base, cl);
        for (j = 0; j < fvf.nb_faces_tot(); j++)
          {
            f = fvf.num_face(j), e = f_e(f, 0), fb = fbord(f); //numero de l'elem en face, de face de bord
            //diffusion : partie normale (nf.nu.nf), partie tangente (nu.nf - (nf.nu.nf)nf )
            for (n = 0; n < N; n++) for (lambda(0, n) = nu_dot(nu, e, n, &nf(f, 0), &nf(f, 0)) / (fs(f) * fs(f)), d = 0; d < D; d++)
                lambda_t(0, n, d) = (nu_dot(nu, e, n, i3[d], &nf(f, 0)) - lambda(0, n) * nf(f, d)) / fs(f);
            for (def(0) = dot(&xv_(f, 0), &nf(f, 0), &xp_(e, 0)) / fs(f), d = 0; d < D; d++) //projection elem->face
              xef(0, d) = xp_(e, d) + def(0) * nf(f, d) / fs(f), xef(1, d) = xv_(f, d);

            if (is_p ? (neu || neu_h) : (dir || dir_h || echg)) for (n = 0; n < N; n++) //Dirichlet / Echange_impose_base : pt harmonique avec def(1) -> 0
                {
                  double r = (invh ? (*invh)(fb, n) : 0) + (echg ? 1. / ref_cast(Echange_impose_base, cl).h_imp(j, n) : 0); //resistance thermique totale
                  for (wh(f, n) = 1 - 1. / (1 + r * lambda(0, n) / def(0)), d = 0; d < D; d++) //poids de l'amont
                    xh(f, n, d) = (def(0) * xef(1, d) + r * (lambda(0, n) * xef(0, d) + def(0) * lambda_t(0, n, d))) / (def(0) + r * lambda(0, n)); //position
                }
            else if (is_p ? (dir || dir_h || sym) : (neu || neu_h || sym)) for (n = 0; n < N; n++) //Neumann : projection sur la face selon nu.nf
                {
                  double scal = dot(&xv_(f, 0), &nf(f, 0), &xp_(e, 0)) / (fs(f) * (nu_grad ? lambda(0, n) : 1));
                  for (wh(f, n) = 1, d = 0; d < D; d++) xh(f, n, d) = xp_(e, d) + scal * (nu_grad ? nu_dot(nu, e, n, &nf(f, 0), i3[d]) : nf(f, d)) / fs(f);
                }
            else abort();
          }
      }

  /* faces internes */
  for (f = premiere_face_int(); f < nf_tot; f++) if (f_e(f, 0) >= 0 && f_e(f, 1) >= 0) /* on doit avoir les deux voisins */
      {
        for (i = 0; i < 2; i++) for (e = f_e(f, i), n = 0; n < N; n++) lambda(i, n) = nu_dot(nu, e, n, &nf(f, 0), &nf(f, 0)) / (fs(f) * fs(f));
        for (i = 0; i < 2; i++) for (e = f_e(f, i), def(i) = (i ? -1 : 1) * dot(&xv_(f, 0), &nf(f, 0), &xp_(e, 0)) / fs(f), d = 0; d < D; d++)
            xef(i, d) = xp_(e, d) + (i ? -1 : 1) * def(i) * nf(f, d) / fs(f);
        for (i = 0; i < 2; i++) for (e = f_e(f, i), n = 0; n < N; n++) for (d = 0; d < D; d++)
              lambda_t(i, n, d) = (nu_dot(nu, e, n, i3[d], &nf(f, 0)) - lambda(i, n) * nf(f, d)) / fs(f);
        for (n = 0; n < N; n++) for (d = 0; d < D; d++) /* position du "harmonic average point "*/
            xh(f, n, d) = (lambda(1, n) * def(0) * xef(1, d) + lambda(0, n) * def(1) * xef(0, d) + def(0) * def(1) * (lambda_t(1, n, d) - lambda_t(0, n, d)))
                          / (lambda(1, n) * def(0) + lambda(0, n) * def(1));
        for (n = 0; n < N; n++) /* poids de l'amont dans la valeur a ce point */
          wh(f, n) = lambda(0, n) * def(1) / (lambda(1, n) * def(0) + lambda(0, n) * def(1));
      }
}

//pour u.n champ T aux elements, interpole [n_f.grad T]_f (si nu_grad = 0) ou [n_f.nu.grad T]_f
//en preservant exactement les champs verifiant [nu grad T]_e = cte.
//Entrees : cls           : conditions aux limites
//          fcl(f, 0/1/2) : donnes sur les CLs (type de CL, indice de CL, indice dans la CL) (cf. Champ_{P0,Face}_CoviMAC)
//          nu(e, n, ..)  : diffusivite aux elements (optionnel)
//          invh(f, n)    : resistivite (1/h) aux bords (optionnel)
//          xh, wh, whm   : donnees sur les points harmoniques retournees par harmonic_points()
//          pe_ext        : donnees sur les CL Echange_contact retournees par Op_Diff_CoviMAC_base
//          nu_grad       : 1 si on veut nf.(nu.grad T), 0 si on veut nf.grad T
//Sorties : phif_w(f, n)                         : poids de l'amont dans la compo n du flux
//          phif_d(f, 0/1)                       : indices dans phif_{e,c} / phif_{pe,pc} du flux a f dans [phif_d(f, 0/1), phif_d(f + 1, 0/1)[
//          phif_e(i), phif_c(i, n, c)           : indices/coefficients locaux (pas d'Echange_contact) et diagonaux (composantes independantes)
//          phif_pe(i, 0/1), phif_pc(i, n, m, c) : indices (pb, elem) /coefficients distants et/ou non diagonaux
void Zone_CoviMAC::fgrad(const Conds_lim& cls, const IntTab& fcl, const DoubleTab *nu, const DoubleTab *invh,
                         const DoubleTab& xh, const DoubleTab& wh, const DoubleTab *whm, const IntTab *pe_ext, int nu_grad,
                         DoubleTab& phif_w, IntTab& phif_d, IntTab& phif_e, DoubleTab& phif_c, IntTab *phif_pe, DoubleTab *phif_pc) const
{
  const IntTab& f_e = face_voisins();
  const DoubleTab& nf = face_normales();
  const DoubleVect& fs = face_surfaces();
  int i, j, k, l, e, f, e_s, f_s, f_sb, m, n, N = xh.dimension(1), M = whm ? whm->dimension(2) : N, ne_tot = nb_elem_tot(), nw, infoo, d, D = dimension, f_max = phif_w.dimension(0), nnz, hdiag;
  char trans = 'N';
  double i3[3][3] = { { 1, 0, 0 }, { 0, 1, 0}, { 0, 0, 1 }};
  phif_d.set_smart_resize(1), phif_e.set_smart_resize(1), phif_e.resize(0), phif_c.set_smart_resize(1), phif_c.resize(0, N, 2);
  if (phif_pe) phif_d.resize(1, 2), phif_pe->set_smart_resize(1), phif_pe->resize(0, 2), phif_pc->set_smart_resize(1), phif_pc->resize(0, N, M, 2);
  else phif_d.resize(1);

  DoubleTrav base, A, B, interp, phi, W(1), r_int(N, 2), lambda(2);
  base.set_smart_resize(1), A.set_smart_resize(1), B.set_smart_resize(1), interp.set_smart_resize(1), phi.set_smart_resize(1), W.set_smart_resize(1);
  std::vector<std::pair<int, int>> p_e; //stencil aux elements
  init_stencils();

  for (f = 0; f < f_max; (phif_pe ? phif_d.append_line(phif_e.size(), phif_pe->dimension(0)) : phif_d.append_line(phif_e.size())), f++)
    if (fcl(f, 0) >= 0 && sub_type(Echange_contact_CoviMAC, cls[fcl(f, 1)].valeur())) //Echange_contact -> delegation
      {
        ref_cast(Echange_contact_CoviMAC, cls[fcl(f, 1)].valeur()).fgrad(phif_w, phif_d, phif_e, phif_c, *phif_pe, *phif_pc); //remplit toutes les faces de la CL
        phif_d.resize(phif_d.dimension(0) - 1, 2), f = phif_d.dimension(0) - 1; //pour que f et phif_d soient bons a la prochaine iteration de la boucle
      }
    else if (wh(f, 0) >= 0) //faces internes ou de bord "normales" -> il faut etre assez loin du joint pour que tous les ooints harmoniques soient definis
      {
        int ok = 1, n_f = fsten_d(f + 1) - fsten_d(f), nc = n_f + 1, nl = D + 1, nrhs = 1 + (f_e(f, 1) >= 0); //nombre de faces dans le stencil, indice de face de bord
        const int *fa = &fsten_f(fsten_d(f)); //pointeur vers la premiere face du stencil
        for (i = 0; i < n_f; i++) ok &= (wh(fa[i], 0) >= 0);
        if (!ok) continue; //il manque un point harmonique -> on sort

        base.resize(nrhs, D, D), A.resize(nc, nl), B.resize(nrhs, nc), interp.resize(nc, N, nrhs); //systeme Ax = B -> format LAPACK
        for (interp = 0, n = 0; n < N; n++) for (i = 0; i < nrhs; i++) interp(n_f, n, i) = 1; //interpolation de depart : celle au point harmonique
        for (n = 0; n < N; n++)
          {
            /* base : base de "vecteurs tests" amont/aval d'un gradient verifiant nf.nu_am.vec_am = nf.nu_av.vec_av */
            for (i = 0; i < nrhs; i++) lambda(i) = nu_dot(nu, f_e(f, i), n, &nf(f, 0), &nf(f, 0)) / (fs(f) * fs(f));
            for (i = 0; i < nrhs; i++) for (j = 0; j < D; j++) for (d = 0; d < D; d++)
                  base(i, j, d) = i3[j][d] + (i ? -1 : 1) * (nu_dot(nu, f_e(f, f_e(f, 1) >= 0), n, &nf(f, 0), i3[j]) - nu_dot(nu, f_e(f, 0), n, &nf(f, 0), i3[j])) * nf(f, d) / (2 * lambda(i) * fs(f) * fs(f));

            /* seconds membres : au passage, on calcule la resistance interne r = d / lambda */
            for (B = 0, i = 0; i < nrhs; i++)
              {
                e = f_e(f, i), r_int(n, i) = (i ? -1 : 1) * dot(&xv_(f, 0), &nf(f, 0), &xp_(e, 0)) / (fs(f) * (nu_grad ? lambda(i) : 1));
                for (d = 0; d < D; d++) B(i, d) = xp_(e, d) + (i ? -1 : 1) * r_int(n, i) * (nu_grad ? nu_dot(nu, e, n, &nf(f, 0), i3[d]) : nf(f, d)) / fs(f) - xh(f, n, d);
              }

            /* interpolation des points projetes amont / aval */
            if (std::pow(1e6 * local_max_abs_vect(B), D - 1) > fs(f)) //si les pts projetes ne sont pas dessus -> corrections
              {
                /* matrice A (format LAPACK) */
                for (i = 0; i < n_f; i++) for (k = (dot(&xh(fa[i], n, 0), &nf(f, 0), &xh(f, n, 0)) > 0 && nrhs > 1), d = 0; d < nl; d++) //faces du stencil
                    A(i, d) = d < D ? dot(&xh(fa[i], n, 0), &base(k, d, 0), &xh(f, n, 0)) : 1;
                for (d = 0; d < nl; d++) A(n_f, d) = d < D ? 0 : 1; //face elle-meme

                /* resolution */
                nw = -1, F77NAME(dgels)(&trans, &nl, &nc, &nrhs, &A(0, 0), &nl, &B(0, 0), &nc, &W(0), &nw, &infoo);
                W.resize(nw = W(0)), F77NAME(dgels)(&trans, &nl, &nc, &nrhs, &A(0, 0), &nl, &B(0, 0), &nc, &W(0), &nw, &infoo);
                /* correction de interp (avec ecretage) */
                for (i = 0; i < nrhs; i++) for (j = 0; j < nc; j++) interp(j, n, i) += dabs(B(i, j)) > 1e-8 ? B(i, j) : 0;
              }
          }

        /* construction du stencil aux elements */
        for (p_e.clear(), i = 0; i <= n_f; i++) for (f_s = i < n_f ? fa[i] : f, f_sb = fbord(f_s), j = 0; j < 2; j++)
            if ((e_s = f_e(f_s, j)) >= 0) p_e.push_back(std::make_pair(0, e_s)); //element normal
            else if (f_sb >= 0 && pe_ext && (e_s = (*pe_ext)(f_sb, 1)) >= 0) p_e.push_back(std::make_pair((*pe_ext)(f_sb, 0), e_s)); //de l'autre cote d'un Echange_contact
            else if (f_sb >= 0) p_e.push_back(std::make_pair(0, ne_tot + f_s)); //CL non Echange_contact -> dependance en les valeurs aux bords
        std::sort(p_e.begin(), p_e.end()), p_e.erase(std::unique(p_e.begin(), p_e.end()), p_e.end()); //classement + deduplication

        /* passage au flux */
        phi.resize(p_e.size(), N, M, 2), phi = 0;
        for (n = 0; n < N; n++) phif_w(f, n) = nrhs < 2 ? 1 : r_int(n, 0) * wh(f, n) / (r_int(n, 0) * wh(f, n) + r_int(n, 1) * (1 - wh(f, n))); //poids de l'amont

        for (i = 0; i <= n_f; i++) for (f_s = i < n_f ? fa[i] : f, f_sb = fbord(f_s), j = 0; j < 2; j++)
            if ((e_s = f_e(f_s, j) >= 0 ? f_e(f_s, j) : f_sb >= 0 && pe_ext && (*pe_ext)(f_sb, 0) >= 0 ? (*pe_ext)(f_sb, 1) : -1) >= 0) //il existe un element source (interne ou externe)
              {
                k = std::lower_bound(p_e.begin(), p_e.end(), std::make_pair(f_e(f_s, j) >= 0 ? 0 : (*pe_ext)(f_sb, 0), e_s)) - p_e.begin();//position dans le stencil
                if (f_sb < 0 || !pe_ext || (*pe_ext)(f_sb, 0) < 0) for (n = 0; n < N; n++) for (l = 0; l < nrhs; l++) //point harmonique standard -> composantes separees
                      phi(k, n, n, l) += (l ? -1 : 1) / r_int(n, l) * interp(i, n, l) * (j ? 1 - wh(f_s, n) : wh(f_s, n));
                else for (n = 0; n < N; n++) for (m = 0; m < M; m++) for (l = 0; l < nrhs; l++) //pt harmonique a une Echange_contact -> melange des composantes
                        phi(k, n, m, l) += (l ? -1 : 1) / r_int(n, l) * interp(i, n, l) * (*whm)((*pe_ext)(f_sb, 2), n, m, j);
              }
            else for (k = std::lower_bound(p_e.begin(), p_e.end(), std::make_pair(0, ne_tot + f_s)) - p_e.begin(), n = 0; n < N; n++) //face de bord -> dependance en la CL
                for (l = 0; l < nrhs; l++) phi(k, n, n, l) += (l ? -1 : 1) / r_int(n, l) * interp(i, n, l)
                                                                * (wh(f_s, n) < 1 ? 1 - wh(f_s, n) : dist_norm_bord(f_s) / (nu_grad ? -nu_dot(nu, f_e(f_s, 0), n, &nf(f_s, 0), &nf(f_s, 0)) / (fs(f_s) * fs(f_s)) : 1));
        //amont/aval
        for (i = 0; i < nrhs; i++) for (k = std::lower_bound(p_e.begin(), p_e.end(), std::make_pair(0, f_e(f, i))) - p_e.begin(), n = 0; n < N; n++)
            phi(k, n, n, i) += (i ? 1 : -1) / r_int(n, i);

        /* remplissage de phif_e/pe/c/pc */
        for (i = 0; i < (int) p_e.size(); i++)
          {
            for (nnz = 0, hdiag = 0, n = 0; n < N; n++) for (m = 0; m < M; m++) for (l = 0; l < nrhs; l++) if (phi(i, n, m, l) != 0) nnz++, hdiag |= (m != n);
            if (!nnz) continue; //on peut sauter ce point
            if (p_e[i].first == 0 && !hdiag) //contrib diagonale d'un elem local -> dans phif_e/c
              for (phif_e.append_line(p_e[i].second), k = phif_c.dimension(0), phif_c.resize(k + 1, N, 2), n = 0; n < N; n++)
                for (l = 0; l < 2; l++) phif_c(k, n, l) = phi(i, n, n, l);
            else for (phif_pe->append_line(p_e[i].first, p_e[i].second), k = phif_pc->dimension(0), phif_pc->resize(k + 1, N, M, 2), n = 0; n < N; n++)
                for (m = 0; m < M; m++) for (l = 0; l < 2; l++) (*phif_pc)(k, n, m, l) = phi(i, n, m, l);
          }
      }
}
