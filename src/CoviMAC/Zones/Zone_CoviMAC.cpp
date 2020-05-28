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

//elements et faces de bord connectes a chaque face
//elements fef_e([fef_d(f, 0), fef_d(s + 1, 0)[) avec l'amont/aval d'abord, faces de bord fef_f([fef_d(s, 1), fef_d(s + 1, 1)[)
void Zone_CoviMAC::init_fef() const
{
  if (is_init["fef"]) return;
  const IntTab& f_s = face_sommets(), &f_e = face_voisins(), &e_f = elem_faces(), &e_s = zone().les_elems();
  int i, j, k, e, eb, f, fb, s, ns_tot = zone().domaine().nb_som_tot(), ok;
  fef_d.resize(0, 2), fef_d.set_smart_resize(1), fef_e.set_smart_resize(1), fef_f.set_smart_resize(1);

  //pour identifer les faces de bord virtuelles avant d'avoir des CLs
  IntTrav is_fb;
  creer_tableau_faces(is_fb);
  for (f = 0; f < premiere_face_int(); f++) is_fb(f) = 1;
  is_fb.echange_espace_virtuel();

  /* connectivite sommets -> elems / faces de bord */
  std::vector<std::set<int>> som_e(ns_tot), som_f(ns_tot);
  if (Option_CoviMAC::vertex_stencil)
    {
      for (e = 0; e < nb_elem_tot(); e++) for (i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++) som_e[s].insert(e);
      for (f = 0; f < nb_faces_tot(); f++) if (is_fb(f)) for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++) som_f[s].insert(f);
    }

  /* pour avoir le meme ordre sur tous les procs */
  std::map<std::array<double, 3>, int> xp_e, xv_f;
  std::set<int> soms; //sommets de la face f
  for (f = 0, fef_d.append_line(0, 0); f < nb_faces_tot(); fef_d.append_line(fef_e.size(), fef_f.size()), f++) if (is_fb(f) || f_e(f, 1) >= 0)
      {
        for (i = 0, xp_e.clear(), xv_f.clear(), soms.clear(); i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++) soms.insert(s);

        if (Option_CoviMAC::vertex_stencil) for (auto &som : soms) //tout ce qui touche un sommet de f
            {
              for (auto &el : som_e[som]) xp_e[ {{ xp_(el, 0), xp_(el, 1), dimension < 3 ? 0 : xp_(el, 2) }}] = el;
              for (auto &fa : som_f[som]) xv_f[ {{ xv_(fa, 0), xv_(fa, 1), dimension < 3 ? 0 : xv_(fa, 2) }}] = fa;
            }
        else for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) //tout ce qui touche un sommet de f et est voisin de l'amont/aval
            {
              xp_e[ {{ xp_(e, 0), xp_(e, 1), dimension < 3 ? 0 : xp_(e, 2) }}] = e;
              for (j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
                {
                  for (k = 0, ok = 0; !ok && k < f_s.dimension(1) && (s = f_s(fb, k)) >= 0; k++) ok |= soms.count(s);
                  if (!ok) continue; //pas de sommet en commun avec f
                  if (is_fb(fb)) xv_f[ {{ xv_(fb, 0), xv_(fb, 1), dimension < 3 ? 0 : xv_(fb, 2) }}] = fb; //on gagne une face de bord
                  else if ((eb = f_e(fb, e == f_e(fb, 0))) >= 0) xp_e[ {{ xp_(eb, 0), xp_(eb, 1), dimension < 3 ? 0 : xp_(eb, 2) }}] = eb; //on gagne un element
                }
            }

        /* remplissage, en commencant par l'amont/aval (pour fef_e) et par la face elle-meme si elle est de bord (pour fef_f) */
        for (auto && c_e : xp_e) if (c_e.second == f_e(f, 0) || c_e.second == f_e(f, 1)) fef_e.append_line(c_e.second);
        for (auto && c_e : xp_e) if (c_e.second != f_e(f, 0) && c_e.second != f_e(f, 1)) fef_e.append_line(c_e.second);
        for (auto && c_f : xv_f) if (c_f.second == f) fef_f.append_line(c_f.second);
        for (auto && c_f : xv_f) if (c_f.second != f) fef_f.append_line(c_f.second);
      }

  CRIMP(fef_d), CRIMP(fef_e), CRIMP(fef_f);
  is_init["fef"] = 1;
}

//pour un champ T aux elements, interpole nu.grad T a la face f; indices donnes par fef_e, fef_f
void Zone_CoviMAC::interp_flux(int f_max, const DoubleTab& nu, int N, const IntTab& icl, const std::vector<int>& is_flux, DoubleTab& fef_ce, DoubleTab& fef_cf, IntTab *tpfa) const
{
  const IntTab& f_e = face_voisins();
  const DoubleTab& nf = face_normales();
  const DoubleVect& fs = face_surfaces(), &vf = volumes_entrelaces();
  int i, ib, j, n, e, f, fb, lwork, infoo = 0;
  char trans = 'N';

  DoubleTrav nu_nf[2], nu_nf2(N, 2), coeff(N), scal(N, 2), corr(N, dimension), M, B, W, nu_nfb(N, dimension), work(1);
  M.set_smart_resize(1), B.set_smart_resize(1), W.set_smart_resize(1), work.set_smart_resize(1);
  nu_nf[0].resize(N, dimension), nu_nf[1].resize(N, dimension);

  for (f = 0; f < f_max; f++)
    {
      int e0 = f_e(f, 0), e1 = f_e(f, 1), el[2] = { e0, e1 };
      if (icl(f, 0))
        {
          if (!is_flux[icl(f, 0)]) //Dirichlet -> flux a deux points
            {
              nu_prod(e0, nu, &nf(f, 0), nu_nf[0]), nu_nf[0] /= fs(f);
              for (n = 0; n < N; n++) coeff(n) = dot(&nu_nf[0](n, 0), &nu_nf[0](n, 0)) / dabs(dot(&xv_(f, 0), &nu_nf[0](n, 0), &xp_(e0, 0)));
              for (n = 0; n < N; n++) fef_ce(fef_d(f, 0), n) = -coeff(n), fef_cf(fef_d(f, 1), n) = coeff(n);
            }
          else for (n = 0; n < N; n++) fef_cf(fef_d(f, 1), n) = -1; //face de bord de Neumann -> flux impose (avec un -)
          if (tpfa) (*tpfa)(f) = 1; //dans les deux cas, flux a deux points
          continue;
        }
      else if (e1 < 0) continue; //face sur le joint

      /* coefficients et partie amont/aval */
      for (i = 0; i < 2; i++) nu_prod(el[i], nu, &nf(f, 0), nu_nf[i]), nu_nf[i] /= fs(f);
      for (n = 0; n < N; n++) for (i = 0; i < 2; i++)
          nu_nf2(n, i) = dot(&nu_nf[i](n, 0), &nu_nf[i](n, 0)), scal(n, i) = dot(&xv_(f, 0), &nu_nf[i](n, 0), &xp_(el[i], 0));
      for (n = 0; n < N; n++) coeff(n) = 1. / (dabs(scal(n, 0)) / nu_nf2(n, 0) + dabs(scal(n, 1)) / nu_nf2(n, 1));

      for (i = fef_d(f, 0); i < fef_d(f + 1, 0); i++) for (n = 0, e = fef_e(i); n < N; n++) fef_ce(i, n) = (e == e1 ? 1 : (e == e0 ? -1 : 0)) * coeff(n);

      /* correction : coeff * vec.grad T, avec vec = x_fam - x_fav le vecteur entre les projections sur la face de l'amont/aval */
      for (n = 0; n < N; n++) for (i = 0; i < dimension; i++)
          corr(n, i) = xp_(e0, i) - xp_(e1, i) + scal(n, 0) * nu_nf[0](n, i) / nu_nf2(n, 0) - scal(n, 1) * nu_nf[1](n, i) / nu_nf2(n, 1);
      if (max_abs_array(corr) < 1e-6 * vf(f) / fs(f)) //correction negligeable -> on peut sortir!
        {
          if (tpfa) (*tpfa)(f) = 1; //TPFA possible
          for (i = fef_d(f, 1); i < fef_d(f + 1, 1); i++) for (n = 0; n < N; n++) fef_cf(i, n) = 0; //mise a 0 de la partie "bords"
          continue;
        }

      /* reconstruction par moindres carres a partir des voisins */
      int n_e = fef_d(f + 1, 0) - fef_d(f, 0), n_f = fef_d(f + 1, 1) - fef_d(f, 1), n_l = n_e + n_f, n_c = dimension + 1, has_neu = 0;
      //convention Fortran pour dgels. On remplit un systeme par composante, mais ce sont tous les memes si il n'y a pas de CL de Neumann
      M.resize(N, n_c, n_l), B.resize(N, n_l, n_l), W.resize(n_l);
      //W : poids des lignes
      for (i = 0, ib = fef_d(f, 0); i < n_e; i++, ib++) e = fef_e(ib), W(i) = 1. / sqrt(dot(&xp_(e, 0), &xp_(e, 0), &xv_(f, 0), &xv_(f, 0)));
      for (i = n_e, ib = fef_d(f, 1); i < n_l; i++, ib++) if (is_flux[icl(fb = fef_f(ib), 0)]) W(i) = 1; //flux -> va savoir
        else W(i) = 1. / sqrt(dot(&xv_(fb, 0), &xv_(fb, 0), &xv_(f, 0), &xv_(f, 0))); //Dirichlet

      //elements : facile
      for (n = 0; n < N; n++) for (i = 0, B = 0, ib = fef_d(f, 0); i < n_e; i++, ib++)
          for (j = 0, e = fef_e(ib), B(n, i, i) = W(i); j < n_c; j++) M(n, j, i) = W(i) * (j < n_c - 1 ? xp_(e, j) - xv_(f, j) : 1);
      //faces de bord : comme un element si Dirichlet, nu.grad T = - flux_imp si Neumann
      for (i = n_e, ib = fef_d(f, 1); i < n_l; i++, ib++) if (is_flux[icl(fb = fef_f(ib), 0)])
          for (n = 0, nu_prod(f_e(fb, 0), nu, &nf(fb, 0), nu_nfb), nu_nfb /= fs(f), has_neu = 1; n < N; n++)
            for (j = 0, B(n, i, i) = -W(i); j < n_c; j++) M(n, j, i) = W(i) * (j < n_c - 1 ? nu_nfb(n, j) : 0);
        else for (n = 0; n < N; n++) for (j = 0, B(n, i, i) = W(i); j < n_c; j++) M(n, j, i) = W(i) * (j < n_c - 1 ? xv_(fb, j) - xv_(f, j) : 1);

      lwork = -1;
      F77NAME(dgels)(&trans, &n_l, &n_c, &n_l, &M(0, 0, 0), &n_l, &B(0, 0, 0), &n_l, &work(0), &lwork, &infoo); //"workspace query"
      work.resize(lwork = work(0));

      for (n = 0; n < (has_neu ? N : 1); n++) //une seule resolution sauf si CL de Neumann
        F77NAME(dgels)(&trans, &n_l, &n_c, &n_l, &M(n, 0, 0), &n_l, &B(n, 0, 0), &n_l, &work(0), &lwork, &infoo);

      /* stockage des coefficients : coeff * corr * coefficients de B */
      for (i = 0, ib = fef_d(f, 0); i < n_e; i++, ib++) for (n = 0; n < N; n++) for (j = 0; j < dimension; j++)
            fef_ce(ib, n) += coeff(n) * corr(n, j) * B(has_neu ? n : 0, i, j);
      for (i = n_e, ib = fef_d(f, 1); i < n_l; i++, ib++) for (n = 0; n < N; n++) for (j = 0, fef_cf(ib, n) = 0; j < dimension; j++)
            fef_cf(ib, n) += coeff(n) * corr(n, j) * B(has_neu ? n : 0, i, j);
    }
}
