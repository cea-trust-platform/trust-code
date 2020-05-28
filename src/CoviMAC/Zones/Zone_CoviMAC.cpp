/****************************************************************************
* Copyright (c) 2019, CEA
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
      for (int r = 0; r < 3; r++) xv_(face, r) = xs[r] / S;
    }
  xv_.echange_espace_virtuel();

  detecter_faces_non_planes();

  //MD_vector pour Champ_Face_CoviMAC (faces + dimension * champ_p0)
  MD_Vector_composite mdc_ch_face;
  mdc_ch_face.add_part(md_vector_faces());
  for (int r = 0; r < dimension; r++) mdc_ch_face.add_part(zone().md_vector_elements());
  mdv_ch_face.copy(mdc_ch_face);
  nb_ch_face_tot_ = nb_faces_tot() + dimension * nb_elem_tot();

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

void disp(const DoubleTab& A)
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

void disp(const ArrOfDouble& A)
{
  int i;
  for (i = 0, fprintf(stderr, "{"); i < A.size_array(); i++)
    fprintf(stderr, "%.10E%s ", A.addr()[i], i + 1 < A.size_array() ? "," : "");
  fprintf(stderr, "}\n");
}

void disp(const IntTab& A)
{
  int i, j;
  if (A.nb_dim() == 2) for (i = 0; i < A.dimension(0); i++)
      for (j = 0, fprintf(stderr, i ? "},{" : "{{"); j < A.dimension(1); j++)
        fprintf(stderr, "%d%s ", A.addr()[i * A.dimension(1) + j], j + 1 < A.dimension(1) ? "," : "");
  else for (i = 0, fprintf(stderr, "{"); i < A.dimension(0); i++)
      fprintf(stderr, "%d%s ", A.addr()[i], i + 1 < A.dimension(0) ? "," : "");
  fprintf(stderr, A.nb_dim() == 2 ? "}}\n" : "}\n");
}

void disp(const ArrOfInt& A)
{
  int i;
  for (i = 0, fprintf(stderr, "{"); i < A.size_array(); i++)
    fprintf(stderr, "%d%s ", A.addr()[i], i + 1 < A.size_array() ? "," : "");
  fprintf(stderr, "}\n");
}

void disp(const Matrice_Morse& M)
{
  DoubleTab A(M.nb_lignes(), M.nb_colonnes());
  for (int i = 0; i < A.dimension(0); i++)
    for (int k = M.get_tab1().addr()[i] - 1; k < M.get_tab1().addr()[i + 1] - 1; k++)
      A(i, M.get_tab2().addr()[k] - 1) = M.get_coeff().addr()[k];
  disp(A);
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

/* outils pour l'interpolation aux sommets */
//independant des CLs :pour chaque face, contribution de chaque sommet au flux a la face
//vecteurs dans phifs_v([phifs_d(f), phifs_d(f + 1)[, amont/aval, .); indices dans face_sommets_
void Zone_CoviMAC::init_phifs() const
{
  if (is_init["phifs"]) return;
  const IntTab& f_e = face_voisins(), &f_s = face_sommets();
  const DoubleTab& xs = zone().domaine().coord_sommets(), &vfd = volumes_entrelaces_dir();
  int i, j, k, e, f, n_s;

  phifs_d.set_smart_resize(1), phifs_v.resize(0, 2, dimension), phifs_v.set_smart_resize(1);
  phife.resize(nb_faces_tot(), 2, dimension);

  for (f = 0, phifs_d.append_line(0); f < nb_faces_tot(); f++, phifs_d.append_line(phifs_v.dimension(0)))
    {
      for (n_s = 0; n_s < f_s.dimension(1) && f_s(f, n_s) >= 0; ) n_s++;
      phifs_v.resize(phifs_d(f) + n_s, 2, dimension);
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (j = 0; j < n_s; j++)
          {
            /* vecteurs normes sea (au triangle elem-arete), sfa (triangle CG face-arete) tournes vers l'exterieur du diamant */
            int s0 = f_s(f, j), jb = j + 1 < n_s ? j + 1 : 0, s1 = f_s(f, jb);
            std::array<double, 3> sea, sfa, vecz = {{ 0, 0, 1 }}; //produit normale * surface entre le CG de l'element et le sommet (2D) / arete (3D)
            if (dimension < 3) sea = cross(2, 3, &xs(s0, 0), &vecz[0], &xp_(e, 0)), sfa = cross(2, 3, &xs(s0, 0), &vecz[0], &xv_(f, 0));
            else sea = cross(3, 3, &xs(s0, 0), &xs(s1, 0), &xp_(e, 0), &xp_(e, 0)), sfa = cross(3, 3, &xs(s0, 0), &xs(s1, 0), &xv_(f, 0), &xv_(f, 0));

            //orientation, division par 2 (en 3D) et normalisation
            int sgn_ea = dot(&xp_(e, 0), &sea[0], &xv_(f, 0)) > 0 ? 1 : -1, sgn_fa = dot(&xv_(f, 0), &sfa[0], &xp_(e, 0)) > 0 ? 1 : -1;
            for (k = 0; k < dimension; k++) sea[k] *= sgn_ea * (dimension < 3 ? 1. : 0.5) / vfd(f, i), sfa[k] *= sgn_fa * (dimension < 3 ? 1. : 0.5) / vfd(f, i);

            //contribution
            if (dimension < 3) for (k = 0; k < 2; phife(f, i, k) += sea[k], k++) phifs_v(phifs_d(f) + j, i, k) = sea[k] + 2 * sfa[k];
            else for (k = 0; k < 3; phife(f, i, k) += sea[k], k++)
                phifs_v(phifs_d(f) + j, i, k) += sea[k] + 1.5 * sfa[k], phifs_v(phifs_d(f) + jb, i, k) += sea[k] + 1.5 * sfa[k];
          }
    }
  CRIMP(phifs_d), CRIMP(phifs_v);
  is_init["phifs"] = 1;
}

//independant des CL : elements et faces de bord connectes a chaque sommet
void Zone_CoviMAC::init_sef() const
{
  if (is_init["sef"]) return;
  const IntTab& e_s = zone().les_elems(), &f_s = face_sommets(), &f_e = face_voisins();
  int i, e, f, s, ns_tot = zone().domaine().nb_som_tot(), ok;
  sef_d.resize(0, 2), sef_d.set_smart_resize(1), sef_e.set_smart_resize(1), sef_f.set_smart_resize(1);

  //pour identifer les faces de bord virtuelles avant d'avoir des CLs
  IntTrav is_fb;
  creer_tableau_faces(is_fb);
  for (f = 0; f < premiere_face_int(); f++) is_fb(f) = 1;
  is_fb.echange_espace_virtuel();

  //enumeration des elements et faces de bord autour de s dans le meme ordre sur tous les procs
  std::vector<std::map<std::array<double, 3>, int>> som_e(ns_tot), som_f(ns_tot);
  for (e = 0; e < nb_elem_tot(); e++) for (i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++)
      som_e[s][ {{xp_(e, 0), xp_(e, 1), dimension < 3 ? 0 : xp_(e, 2)}}] = e;
  for (f = 0; f < nb_faces_tot(); f++) if (f_e(f, 1) < 0) for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
        som_f[s][ {{xv_(f, 0), xv_(f, 1), dimension < 3 ? 0 : xv_(f, 2)}}] = f;

  /* remplissage */
  for (s = 0, sef_d.append_line(0, 0), ok = 1; s < ns_tot; s++, sef_d.append_line(sef_e.size(), sef_f.size()), ok = 1)
    {
      //connecte a une face a un seul voisin, mais pas de bord -> sur le joint : on ne fait rien
      for (auto &&c_fa : som_f[s]) ok &= is_fb(c_fa.second);
      if (!ok) continue;
      for (auto &&c_el : som_e[s]) sef_e.append_line(c_el.second);
      for (auto &&c_fa : som_f[s]) sef_f.append_line(c_fa.second);
    }

  CRIMP(sef_d), CRIMP(sef_e), CRIMP(sef_f);
  is_init["sef"] = 1;
}

//dependant des CLs : base du flux en chaque sommet libres / fixes par des CLs (si necessaire)
//vl([deb(s, 0), deb(s + 1, 0)[, .) : vecteurs libres
//vn([deb(s, 1), deb(s + 1, 1)[, .) : vecteurs imposes par un flux a une face de bord (indice fn)
//fd([deb(s, 2), deb(s + 1, 2)[) : faces de Dirichlet en contact avec s
void Zone_CoviMAC::base_flux_som(IntTab& deb, DoubleTab& vl, IntTab& fn, DoubleTab& vn, IntTab& fd, const IntTab& icl, const std::vector<int> is_neu) const
{
  const DoubleVect& fs = face_surfaces();
  const DoubleTab& nf = face_normales();
  int i, j, k, f, s, ns_tot = zone().domaine().nb_som_tot(), n_dir, n_neu;

  deb.resize(0, 3), deb.set_smart_resize(1), vl.resize(0, dimension), vl.set_smart_resize(1);
  vn.resize(0, dimension), vn.set_smart_resize(1), fn.set_smart_resize(1), fd.set_smart_resize(1);

  DoubleTrav M, b, K, x, S;
  M.set_smart_resize(1), b.set_smart_resize(1), K.set_smart_resize(1), x.set_smart_resize(1), S.set_smart_resize(1);
  for (s = 0, deb.append_line(0, 0, 0); s < ns_tot; s++, deb.append_line(vl.dimension(0), fn.size(), fd.size()))
    if (sef_d(s, 0) < sef_d(s + 1, 0)) //saute les sommets sur le joint
      {
        /* faces de Dirichlet? */
        for (i = sef_d(s, 1), n_dir = n_neu = 0; i < sef_d(s + 1, 1); i++)
          if (is_neu[icl(f = sef_f(i), 0)]) n_neu++;
          else n_dir++, fd.append_line(f);
        if (n_dir || !n_neu) continue; //interpolation a partir de CL de Dirichlet ou sans faces de Neumann -> pas de base a mettre

        /* on resout le systeme n_f.Phi = Phi_imp pour chaque face imposee */
        M.resize(n_neu, dimension), b.resize(n_neu, n_neu);
        for (M = 0, b = 0, i = sef_d(s, 1), j = 0; i < sef_d(s + 1, 1); i++, j++)
          for (k = 0, b(j, j) = 1; k < dimension; k++) M(j, k) = nf(sef_f(i), k) / fs(f);
        kersol(M, b, 0.2, &K, x, S); //eps = 0.2 -> angle minimal de 15 deg pour considerer qu'on est dans un coin

        //K : vecteurs libres
        for (i = 0; i < K.dimension(1); i++) dimension < 3 ? vl.append_line(K(0, i), K(1, i)) : vl.append_line(K(0, i), K(1, i), K(2, i));
        //x : dependence en chaque flux
        for (i = sef_d(s, 1), j = 0; i < sef_d(s + 1, 1); i++, j++)
          fn.append_line(sef_f(i)), dimension < 3 ? vn.append_line(x(0, j), x(1, j)) : vn.append_line(x(0, j), x(1, j), x(2, j));
      }

  CRIMP(deb), CRIMP(vl), CRIMP(vn), CRIMP(fn), CRIMP(fd);
}

//dependant de la diffusivite : interpolations
//e_c([deb(s, 0), deb(s + 1, 0)[) : elements (indices dans sef_e)
//fnc([deb(s, 1), deb(s + 1, 1)[) : faces de Neumann (indices dans sef_f)
void Zone_CoviMAC::interp_som(IntTab& deb, DoubleTab& e_c, DoubleTab& f_c, const DoubleTab& inv_nu,
                              const IntTab& f_deb, const DoubleTab& f_vl, const IntTab& f_fn, const DoubleTab& f_vn, const IntTab& fd) const
{
  const DoubleTab& xs = zone().domaine().coord_sommets();
  int i, ib, j, jb, k, l, e, s, ns_tot = zone().domaine().nb_som_tot(), n, N = inv_nu.dimension(1), nocrimp = (deb.nb_dim() == 2), lwork, infoo = 0;
  char trans = 'N';

  deb.set_smart_resize(1), deb.resize(0, 2), e_c.set_smart_resize(1), e_c.resize(0, N), f_c.set_smart_resize(1), f_c.resize(0, N);

  DoubleTrav M, B, S, work(1);
  M.set_smart_resize(1), B.set_smart_resize(1), S.set_smart_resize(1), work.set_smart_resize(1);
  for (s = 0, deb.append_line(0, 0); s < ns_tot; s++, deb.append_line(e_c.dimension(0), f_c.dimension(0)))
    if (sef_d(s, 0) < sef_d(s + 1, 0) && f_deb(s, 2) == f_deb(s + 1, 2)) /* saute les sommets sur le joint ou sur les faces de Dirichlet */
      {
        //elements, faces de Neumann, vecteurs libres
        int n_e = sef_d(s + 1, 0) - sef_d(s, 0), n_f = sef_d(s + 1, 1) - sef_d(s, 1), n_v = f_deb(s + 1, 0) - f_deb(s, 0), n_c = (n_v ? n_v : dimension) + 1, n_rhs = n_e + n_f;
        e_c.resize(deb(s, 0) + n_e, N), f_c.resize(deb(s, 1) + n_f, N);
        M.resize(n_c, n_e), B.resize(n_rhs, n_e), S.resize(n_e);
        //poids relatifs des elements
        for (i = 0, ib = sef_d(s, 0); i < n_e; i++, ib++) e = sef_e(ib), S(i) = 1. / sqrt(dot(&xp_(e, 0), &xp_(e, 0), &xs(s, 0), &xs(s, 0)));

        for (n = 0; n < N; n++) //un systeme lineaire par composante
          {
            /* M : systeme a resoudre par moindres carres !! stockage Fortran pour DGELS */
            for (i = 0, ib = sef_d(s, 0); i < n_e; i++, ib++) for (j = 0, jb = sef_d(s, 1), e = sef_e(ib), M(n_c - 1, i) = S(i); j < n_c - 1; j++, jb++)
                if (inv_nu.nb_dim() == 2) M(j, i) = S(i) * inv_nu(e, n) * (n_v ? dot(&xp_(e, 0), &f_vl(jb, 0), &xs(s, 0)) : xp_(e, j) - xs(s, j)); //isotrope
                else if (inv_nu.nb_dim() == 3) for (k = 0, M(j, i) = 0; k < dimension; k++) //anisotrope diagonal
                    M(j, i) += S(i) * inv_nu(e, n, k) * (xp_(e, k) - xs(s, k)) * (n_v ? f_vl(jb, k) : (k == j));
                else for (k = 0, M(j, i) = 0; k < dimension; k++) for (l = 0; l < dimension; l++) //anisotrope complet
                      M(j, i) += S(i) * inv_nu(e, n, k, l) * (xp_(e, k) - xs(s, k)) * (n_v ? f_vl(jb, l) : (l == j));

            /* b : seconds membres -> un par element et par face de Neumann !! stockage Fortran pour DGELS */
            for (i = 0, ib = sef_d(s, 0); i < n_e; i++, ib++) for (j = 0; j < n_e; j++) B(j, i) = S(i) * (i == j);
            for (i = 0, ib = sef_d(s, 0); i < n_e; i++, ib++) for (j = 0, jb = f_deb(s, 1), e = sef_e(ib); j < n_f; j++, jb++)
                if (inv_nu.nb_dim() == 2) B(n_e + j, i) = S(i) * inv_nu(e, n) * dot(&xs(s, 0), &f_vn(jb, 0), &xp_(e, 0)); //isotrope
                else if (inv_nu.nb_dim() == 3) for (k = 0, B(n_e + j, i) = 0; k < dimension; k++) //anisotrope diagonal
                    B(n_e + j, i) += S(i) * inv_nu(e, n, k) * (xs(s, k) - xp_(e, k)) * f_vn(jb, k);
                else for (k = 0, B(n_e + j, i) = 0; k < dimension; k++) for (l = 0; l < dimension; l++) //anisotrope complet
                      B(n_e + j, i) += S(i) * inv_nu(e, n, k, l) * (xs(s, k) - xp_(e, k)) * f_vn(jb, l);

            /* resolution et stockage des coefficients */
            lwork = -1;
            F77NAME(dgels)(&trans, &n_e, &n_c, &n_rhs, &M(0, 0), &n_e, &B(0, 0), &n_rhs, &work(0), &lwork, &infoo); //"workspace query"
            work.resize(lwork = work(0));
            F77NAME(dgels)(&trans, &n_e, &n_c, &n_rhs, &M(0, 0), &n_e, &B(0, 0), &n_rhs, &work(0), &lwork, &infoo); //"workspace query"


            for (i = 0; i < n_e; i++) e_c(deb(s, 0) + i, n) = B(i      , n_c - 1); //elements
            for (i = 0; i < n_f; i++) f_c(deb(s, 1) + i, n) = B(n_e + i, n_c - 1); //flux de Neumann
          }
      }
  if (nocrimp) return;
  CRIMP(deb), CRIMP(e_c), CRIMP(f_c);
}
