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
// File:        Zone_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Frottement_externe_impose.h>
#include <Linear_algebra_tools_impl.h>
#include <Frottement_global_impose.h>
#include <Champ_implementation_P1.h>
#include <Echange_contact_CoviMAC.h>
#include <Connectivite_som_elem.h>
#include <MD_Vector_composite.h>
#include <Dirichlet_homogene.h>
#include <MD_Vector_tools.h>
#include <Zone_Cl_CoviMAC.h>
#include <Segment_CoviMAC.h>
#include <TRUSTTab_parts.h>
#include <Option_CoviMAC.h>
#include <Comm_Group_MPI.h>
#include <Quadrangle_VEF.h>
#include <Quadri_CoviMAC.h>
#include <communications.h>
#include <Tetra_CoviMAC.h>
#include <Statistiques.h>
#include <Zone_CoviMAC.h>
#include <Hexa_CoviMAC.h>
#include <Hexaedre_VEF.h>
#include <Matrix_tools.h>
#include <Tri_CoviMAC.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Tetraedre.h>
#include <Rectangle.h>
#include <PE_Groups.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <EFichier.h>
#include <SFichier.h>
#include <Segment.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EChaine.h>
#include <Lapack.h>
#include <numeric>
#include <vector>
#include <tuple>
#include <cmath>
#include <set>
#include <map>
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
      volumes_entrelaces_dir_(f, i) = std::fabs(dot(&xp_(e, 0), &face_normales_(f, 0), &xv_(f, 0)));
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
      if ((sin2 = std::pow(dot(&xs(s, 0), &nf(f, 0), &xv_(f, 0)) / fs(f), 2) / dot(&xs(s, 0), &xs(s, 0), &xv_(f, 0), &xv_(f, 0))) > val[rk])
        val[rk] = sin2, face(rk) = f, elem1(rk) = f_e(f, 0), elem2(rk) = f_e(f, 1);
  envoyer_all_to_all(val, val), envoyer_all_to_all(face, face), envoyer_all_to_all(elem1, elem1), envoyer_all_to_all(elem2, elem2);

  for (i = j = sin2 = 0; i < Process::nproc(); i++) if (val[i] > sin2) sin2 = val[i], j = i;
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
            if (std::fabs(std::fabs(dot(&nf(f1, 0), &nf(f2, 0)) / (fs(f1) * fs(f2))) - 1) > 1e-6) continue; //normales colineaires?
            for (ok = 1, d = 0; d < D; d++) ok &= std::fabs((xv_(f1, d) - xp_(e1, d)) - (xv_(f2, d) - xp_(e2, d))) < 1e-12; //xv - xp identiques?
            if (!ok) continue;
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
  const IntTab& f_s = face_sommets(), &f_e = face_voisins(), &e_s = zone().les_elems();
  int i, e, f, s, ne_tot = nb_elem_tot(), ns_tot = zone().domaine().nb_som_tot();
  fsten_d.set_smart_resize(1), fsten_d.resize(1), fsten_eb.set_smart_resize(1);

  /* connectivite sommets -> elems / faces de bord */
  std::vector<std::set<int>> som_eb(ns_tot);
  for (e = 0; e < nb_elem_tot(); e++) for (i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++) som_eb[s].insert(e);
  for (f = 0; f < nb_faces_tot(); f++) if (fbord(f) >= 0) for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++) som_eb[s].insert(ne_tot + f);

  std::set<int> f_eb; //sommets de la face f, elems connectes a e par soms, sommets / faces connectes par une face commune
  for (f = 0; f < nb_faces_tot(); fsten_d.append_line(fsten_eb.size()), f++) if (f_e(f, 0) >= 0 && (fbord(f) >= 0 || f_e(f, 1) >= 0))
      {
        /* connectivite par un sommet de f */
        for (f_eb.clear(), i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++) for (auto &&el : som_eb[s]) f_eb.insert(el);

        /* remplissage */
        for (auto && el : f_eb) fsten_eb.append_line(el);
      }

  CRIMP(fsten_d), CRIMP(fsten_eb);
  is_init["stencils"] = 1;
}

void Zone_CoviMAC::init_som_elem() const
{
  if (is_init["som_elem"]) return;
  construire_connectivite_som_elem(zone().domaine().nb_som_tot(), zone().les_elems(), som_elem, 1);
  is_init["som_elem"] = 1;
}

//pour u.n champ T aux elements, interpole [n_f.grad T]_f (si nu_grad = 0) ou [n_f.nu.grad T]_f
//en preservant exactement les champs verifiant [nu grad T]_e = cte.
//Entrees : N             : nombre de composantes
//          is_p          : 1 si on traite le champ de pression (inversion Neumann / Dirichlet)
//          cls           : conditions aux limites
//          fcl(f, 0/1/2) : donnes sur les CLs (type de CL, indice de CL, indice dans la CL) (cf. Champ_{P0,Face}_CoviMAC)
//          nu(e, n, ..)  : diffusivite aux elements (optionnel)
//          som_ext       : liste de sommets a ne pas traiter (ex. : traitement direct des Echange_Contact dans Op_Diff_CoviMAC_Elem)
//          virt          : 1 si on veut aussi le flux aux faces virtuelles
//          full_stencil  : 1 si on veut le stencil complet (pour dimensionner())
//Sorties : phif_d(f, 0/1)                       : indices dans phif_{e,c} / phif_{pe,pc} du flux a f dans [phif_d(f, 0/1), phif_d(f + 1, 0/1)[
//          phif_e(i), phif_c(i, n, c)           : indices/coefficients locaux (pas d'Echange_contact) et diagonaux (composantes independantes)
void Zone_CoviMAC::fgrad(int N, int is_p, const Conds_lim& cls, const IntTab& fcl, const DoubleTab *nu, const IntTab *som_ext,
                         int virt, int full_stencil, IntTab& phif_d, IntTab& phif_e, DoubleTab& phif_c) const
{
  const IntTab& f_e = face_voisins(), &e_f = elem_faces(), &f_s = face_sommets();
  const DoubleTab& nf = face_normales(), &xs = zone().domaine().coord_sommets(), &vfd = volumes_entrelaces_dir();
  const DoubleVect& fs = face_surfaces(), &vf = volumes_entrelaces();
  int i, i_s, j, k, l, e, f, s, sb, n_f, n_m, n_ef, n_e, n_eb, m, n, ne_tot = nb_elem_tot(), sgn, nw, infoo, d, db, D = dimension, rk, nl, nc, un = 1, il, ok, essai;
  double x, eps_g = 1e-6, eps = 1e-10, i3[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }}, fac[3], vol_s;
  init_som_elem(), init_stencils();
  phif_d.set_smart_resize(1), phif_e.set_smart_resize(1), phif_e.resize(0), phif_c.resize(fsten_eb.dimension(0), N), phif_c = 0;

  std::vector<int> s_eb, s_f; //listes d'elements/bord, de faces autour du sommet
  std::vector<double> surf_fs, vol_es; //surfaces partielles des faces connectees au sommet (meme ordre que s_f)
  std::vector<std::array<std::array<double, 3>,2>> vec_fs;//pour chaque facette, base de (D-1) vecteurs permettant de la parcourir
  std::vector<std::vector<int>> se_f; /* se_f[i][.] : faces connectees au i-eme element connecte au sommet s */
  DoubleTrav M, B, X, Ff, Feb, Mf, Meb, W(1), x_fs, A, S; //systeme M.(grad u) = B dans chaque element, flux a la face Ff.u_fs + Feb.u_eb, equations Mf.u_fs = Meb.u_eb
  IntTrav piv, ctr[3];
  for (i = 0; first_fgrad_ && i < 3; i++) zone().domaine().creer_tableau_sommets(ctr[i]);
  M.set_smart_resize(1), B.set_smart_resize(1), X.set_smart_resize(1), Ff.set_smart_resize(1), Feb.set_smart_resize(1), Mf.set_smart_resize(1), Meb.set_smart_resize(1);
  W.set_smart_resize(1), x_fs.set_smart_resize(1), A.set_smart_resize(1), piv.set_smart_resize(1), S.set_smart_resize(1);

  /* contributions aux sommets : en evitant ceux de som_ext */
  for (i_s = 0; i_s <= (som_ext ? som_ext->size() : 0); i_s++)
    for (s = (som_ext && i_s ? (*som_ext)(i_s - 1) + 1 : 0); s < (som_ext && i_s < som_ext->size() ? (*som_ext)(i_s) : (virt ? nb_som_tot() : nb_som())); s++)
      {
        /* elements connectes a s : a partir de som_elem (deja classes) */
        for (s_eb.clear(), n_e = 0; n_e < som_elem.get_list_size(s); n_e++) s_eb.push_back(som_elem(s, n_e));
        /* faces et leurs surfaces partielles */
        for (s_f.clear(), surf_fs.clear(), vec_fs.clear(), se_f.resize(std::max(int(se_f.size()), n_e)), i = 0, ok = 1; i < n_e; i++) for (se_f[i].clear(), e = s_eb[i], j = 0; j < e_f.dimension(1) && (f = e_f(e, j)) >= 0; j++)
            {
              for (k = 0, sb = 0; k < f_s.dimension(1) && (sb = f_s(f, k)) >= 0; k++) if (sb == s) break;
              if (sb != s) continue; /* face de e non connectee a s -> on saute */
              if (fbord(f) >= 0) s_eb.insert(std::lower_bound(s_eb.begin(), s_eb.end(), ne_tot + f), ne_tot + f); //si f est de bord, on ajoute l'indice correspondant a s_eb
              else ok &= (f_e(f, 0) >= 0 && f_e(f, 1) >= 0); //si f est interne, alors l'amont/aval doivent etre presents
              se_f[i].push_back(f); //faces connectees a e et s
              if ((l = std::lower_bound(s_f.begin(), s_f.end(), f) - s_f.begin()) == (int) s_f.size() || s_f[l] != f) /* si f n'est pas dans s_f, on l'ajoute */
                {
                  s_f.insert(s_f.begin() + l, f); //face -> dans s_f
                  if (D < 3) surf_fs.insert(surf_fs.begin() + l, fs(f) / 2), vec_fs.insert(vec_fs.begin() + l, {{{ xs(s, 0) - xv_(f, 0), xs(s, 1) - xv_(f, 1), 0}, { 0, 0, 0 }}}); //2D -> facile
                  else for (surf_fs.insert(surf_fs.begin() + l, 0), vec_fs.insert(vec_fs.begin() + l, {{{ 0, 0, 0}, {0, 0, 0 }}}), m = 0; m < 2; m++) //3D -> deux sous-triangles
                  {
                    if (m == 1 || k > 0) sb = f_s(f, m ? (k + 1 < f_s.dimension(1) && f_s(f, k + 1) >= 0 ? k + 1 : 0) : k - 1); //sommet suivant (m = 1) ou precedent avec k > 0 -> facile
                    else for (n = f_s.dimension(1) - 1; (sb = f_s(f, n)) == -1; ) n--; //sommet precedent avec k = 0 -> on cherche a partir de la fin
                    auto v = cross(D, D, &xs(s, 0), &xs(sb, 0), &xv_(f, 0), &xv_(f, 0));//produit vectoriel (xs - xf)x(xsb - xf)
                    surf_fs[l] += std::fabs(dot(&v[0], &nf(f, 0))) / fs(f) / 4; //surface a ajouter
                    for (d = 0; d < D; d++) vec_fs[l][m][d] = (xs(s, d) + xs(sb, d)) / 2 - xv_(f, d); //vecteur face -> arete
                  }
                }
            }
        if (!ok) continue; //au moins un voisin manquant
        n_eb = s_eb.size(), n_f = s_f.size();

        /* conversion de se_f en indices dans s_f */
        for (i = 0; i < n_e; i++) for (j = 0; j < (int) se_f[i].size(); j++) se_f[i][j] = std::lower_bound(s_f.begin(), s_f.end(), se_f[i][j]) - s_f.begin();
        for (vol_es.resize(n_e), vol_s = 0, i = 0; i < n_e; vol_s += vol_es[i], i++) for (e = s_eb[i], vol_es[i] = 0, j = 0; j < (int) se_f[i].size(); j++)
            f = s_f[k = se_f[i][j]], vol_es[i] += surf_fs[k] * vfd(f, e != f_e(f, 0)) / fs(f) / D;

        for (essai = 0; essai < 3; essai++) /* essai 0 : MPFA O -> essai 1 : MPFA O avec x_fs mobiles -> essai 2 : MPFA symetrique (corecive, mais pas tres consistante) */
          {
            if (essai == 1) /* essai 1 : choix des points x_fs de continuite aux facettes pour obtenir un schema symetrique */
              {
                /* systeme lineaire */
                for (M.resize(N, nc = (D - 1) * n_f, nl = D * (D - 1) / 2 * n_e), B.resize(N, n_m = std::max(nc, nl)), M = 0, B = 0, i = 0, il = 0; i < n_e; i++)
                  for (d = 0; d < D; d++) for (db = 0; db < d; db++, il++) for (e = s_eb[i], j = 0; j < (int) se_f[i].size(); j++) for (sgn = e == f_e(f = s_f[k = se_f[i][j]], 0) ? 1 : -1, n = 0; n < N; n++)
                          {
                            for (l = 0; l < D; l++) fac[l] = sgn * nu_dot(nu, e, n, &nf(f, 0), i3[l]) * surf_fs[k] / fs(f) / vol_es[i]; //vecteur lambda_e nf sortant * facteur commun
                            B(n, il) += fac[d] * (xv_(f, db) - xp_(e, db)) - fac[db] * (xv_(f, d) - xp_(e, d)); //second membre
                            for (l = 0; l < D - 1; l++) M(n, (D - 1) * k + l, il) += fac[db] * vec_fs[k][l][d] - fac[d] * vec_fs[k][l][db]; //matrice
                          }

                /* resolution -> DEGLSY */
                nw = -1, piv.resize(nc), F77NAME(dgelsy)(&nl, &nc, &un, &M(0, 0, 0), &nl, &B(0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                for (W.resize(nw = W(0)), n = 0; n < N; n++) piv = 0, F77NAME(dgelsy)(&nl, &nc, &un, &M(n, 0, 0), &nl, &B(n, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                /* x_fs = xf + corrections */
                for (x_fs.resize(N, n_f, D), n = 0; n < N; n++) for (i = 0; i < n_f; i++) for (f = s_f[i], d = 0; d < D; d++)
                      for (x_fs(n, i, d) = xv_(f, d), k = 0; k < D - 1; k++) x_fs(n, i, d) += std::min(std::max(B(n, (D - 1) * i + k), 0.), 0.5) * vec_fs[i][k][d];
              }

            /* gradients par maille en fonctions des (u_eb, u_fs), flux F = Ff.u_fs + Feb.u_eb, et systeme Mf.u_fs = Feb.u_eb */
            Ff.resize(n_f, n_f, N), Feb.resize(n_f, n_eb, N), Mf.resize(N, n_f, n_f), Meb.resize(N, n_eb, n_f);
            for (Ff = 0, Feb = 0, Mf = 0, Meb = 0, i = 0; i < n_e; i++)
              for (e = s_eb[i], M.resize(n_ef = se_f[i].size(), D), B.resize(D, n_m = std::max(D, n_ef)), X.resize(n_ef, D), piv.resize(n_ef), n = 0; n < N; n++)
                {
                  if (essai < 2) /* essais 0 et 1 : gradient consistant donne par (u_e, (u_fs)_{f v e, s})*/
                    {
                      /* gradient dans (e, s) -> matrice / second membre M.x = B du systeme (grad u)_i = sum_f b_{fi} (x_fs_i - x_e), avec x_fs le pt de continuite de u_fs */
                      for (j = 0; j < n_ef; j++) for (f = s_f[k = se_f[i][j]], d = 0; d < D; d++) M(j, d) = (essai ? x_fs(n, k, d) : xv_(f, d)) - xp_(e, d);
                      for (B = 0, d = 0; d < D; d++) B(d, d) = 1;
                      nw = -1, piv = 0, F77NAME(dgelsy)(&D, &n_ef, &D, &M(0, 0), &D, &B(0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                      W.resize(nw = W(0)), F77NAME(dgelsy)(&D, &n_ef, &D, &M(0, 0), &D, &B(0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                      for (j = 0; j < n_ef; j++) for (d = 0; d < D; d++) X(j, d) = B(d, j); /* pour pouvoir utiliser nu_dot */
                    }
                  else for (j = 0; j < n_ef; j++) for (sgn = e == f_e(f = s_f[k = se_f[i][j]], 0) ? 1 : -1, d = 0; d < D; d++) /* essai 2 : gradient non consistant */
                        X(j, d) = surf_fs[k] / vol_es[i] * sgn * nf(f, d) / fs(f);

                  /* flux et equation. Remarque : les CLs complexes des equations scalaires sont gerees directement dans Op_Diff_CoviMAC_Elem */
                  for (j = 0; j < n_ef; j++)
                    {
                      k = se_f[i][j], f = s_f[k], sgn = e == f_e(f, 0) ? 1 : -1; //face et son indice
                      const Cond_lim_base *cl = fcl(f, 0) ? &cls[fcl(f, 1)].valeur() : NULL; //si on est sur une CL, pointeur vers celle-ci
                      int is_dir = cl && (is_p ? sub_type(Neumann, *cl) : sub_type(Dirichlet, *cl) || sub_type(Dirichlet_homogene, *cl)); //est-elle de Dirichlet?
                      for (l = 0; l < n_ef; l++)
                        {
                          x = sgn * nu_dot(nu, e, n, &nf(f, 0), &X(l, 0)) * surf_fs[k] / fs(f); //contribution au flux
                          if (sgn > 0) Ff(k, se_f[i][l], n) += x, Feb(k, i, n) -= x; //flux amont->aval
                          if (!is_dir) Mf(n, se_f[i][l], k) += x, Meb(n, i, k) += x; //equation sur u_fs (sauf si CL Dirichlet)
                        }
                      if (!cl) continue; //rien de l'autre cote
                      else if (is_dir) Mf(n, k, k) = Meb(n, std::find(s_eb.begin(), s_eb.end(), ne_tot + f) - s_eb.begin(), k) = 1; //Dirichlet -> equation u_fs = u_b
                      else if (is_p ? !is_dir : sub_type(Neumann, *cl)) //Neumann -> ajout du flux au bord
                        Meb(n, std::find(s_eb.begin(), s_eb.end(), ne_tot + f) - s_eb.begin(), k) -= surf_fs[k];
                      else if (sub_type(Frottement_global_impose, *cl)) //Frottement_global_impose -> flux =  - coeff * v_e
                        Meb(n, i, k) -= surf_fs[k] * ref_cast(Frottement_global_impose, *cl).coefficient_frottement(fcl(f, 2), n);
                      else if (sub_type(Frottement_externe_impose, *cl)) //Frottement_externe_impose -> flux =  - coeff * v_f
                        Mf(n, k, k) += surf_fs[k] * ref_cast(Frottement_global_impose, *cl).coefficient_frottement(fcl(f, 2), n);
                    }
                }
            /* resolution de Mf.u_fs = Meb.u_eb : DGELSY, au cas ou */
            nw = -1, piv.resize(n_f), F77NAME(dgelsy)(&n_f, &n_f, &n_eb, &Mf(0, 0, 0), &n_f, &Meb(0, 0, 0), &n_f, &piv(0), &eps, &rk, &W(0), &nw, &infoo);
            for (W.resize(nw = W(0)), n = 0; n < N; n++)
              piv = 0, F77NAME(dgelsy)(&n_f, &n_f, &n_eb, &Mf(n, 0, 0), &n_f, &Meb(n, 0, 0), &n_f, &piv(0), &eps, &rk, &W(0), &nw, &infoo);

            /* substitution dans Feb */
            for (i = 0; i < n_f; i++) for (j = 0; j < n_eb; j++) for (n = 0; n < N; n++) for (k = 0; k < n_f; k++)
                    Feb(i, j, n) += Ff(i, k, n) * Meb(n, j, k);

            /* A : forme bilineaire */
            if (essai == 2) break;//pas la peine pour VFSYM
            for (A.resize(N, n_e, n_e), A = 0, i = 0; i < n_e; i++) for (e = s_eb[i], j = 0; j < (int) se_f[i].size(); j++)
                for (sgn = e == f_e(f = s_f[k = se_f[i][j]], 0) ? 1 : -1, l = 0; l < n_e; l++) for (n = 0; n < N; n++)
                    A(n, i, l) -= sgn * Feb(k, l, n);
            /* symmetrisation */
            for (n = 0; n < N; n++) for (i = 0; i < n_e; i++) for (j = 0; j <= i; j++) A(n, i, j) = A(n, j, i) = (A(n, i, j) + A(n, j, i)) / 2;
            /* v.p. la plus petite : DSYEV */
            nw = -1, F77NAME(DSYEV)("N", "U", &n_e, &A(0, 0, 0), &n_e, S.addr(), &W(0), &nw, &infoo);
            for (W.resize(nw = W(0)), S.resize(n_e), n = 0, ok = 1; n < N; n++)
              F77NAME(DSYEV)("N", "U", &n_e, &A(n, 0, 0), &n_e, &S(0), &W(0), &nw, &infoo), ok &= S(0) > -1e-8 * vol_s;
            if (ok) break; //pour qu' "essai" ait la bonne valeur en sortie
          }
        if (first_fgrad_) ctr[essai](s) = 1;

        /* stockage dans phif_c */
        for (i = 0; i < n_f; i++) for (f = s_f[i], j = 0; j < n_eb; j++)
            for (k = std::lower_bound(fsten_eb.addr() + fsten_d(f), fsten_eb.addr() + fsten_d(f + 1), s_eb[j]) - fsten_eb.addr(), n = 0; n < N; n++)
              phif_c(k, n) += Feb(i, j, n) / fs(f);
      }


  /* simplification du stencil */
  int skip;
  DoubleTrav scale(N);
  for (phif_d.resize(1), phif_d = 0, phif_e.resize(0), f = 0, i = 0; f < nb_faces_tot(); f++, phif_d.append_line(i)) if (fbord(f) >= 0 || (f_e(f, 0) >= 0 && f_e(f, 1) >= 0))
      {
        for (n = 0; n < N; n++) scale(n) = nu_dot(nu, f_e(f, 0), n, &nf(f, 0), &nf(f, 0)) / (fs(f) * vf(f)); //ordre de grandeur des coefficients
        for (j = fsten_d(f); j < fsten_d(f + 1); j++)
          {
            for (skip = !full_stencil && fsten_eb(j) != f_e(f, 0), n = 0; n < N; n++) skip &= std::fabs(phif_c(j, n)) < 1e-8 * scale(n); //que mettre ici?
            if (skip) continue;
            for (n = 0; n < N; n++) phif_c(i, n) = phif_c(j, n);
            phif_e.append_line(fsten_eb(j)), i++;
          }
      }
  /* comptage */
  if (!first_fgrad_) return;
  int count[3] = { mp_somme_vect(ctr[0]), mp_somme_vect(ctr[1]), mp_somme_vect(ctr[2]) }, tot = count[0] + count[1] + count[2];
  if (tot)
    Cerr << zone().domaine().le_nom() << "::fgrad(): " << 100. * count[0] / tot << "% MPFA-O "
         << 100. * count[1] / tot << "% MPFA-O(h) " << 100. * count[2] / tot << "% MPFA-SYM" << finl;
  first_fgrad_ = 0;
}
