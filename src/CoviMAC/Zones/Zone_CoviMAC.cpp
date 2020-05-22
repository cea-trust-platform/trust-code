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

  //MD_Vector pour Champ_P0_CoviMAC (elems + faces de bord)
  MD_Vector_composite mdc_ch_p0;
  mdc_ch_p0.add_part(zone().md_vector_elements());
  IntVect renum;
  creer_tableau_faces(renum, ArrOfInt::NOCOPY_NOINIT);
  for (int i = 0; i < nb_faces(); i++) renum[i] = (i < premiere_face_int() ? 0 : -1);
  renum.echange_espace_virtuel();
  MD_Vector md_fb;
  MD_Vector_tools::creer_md_vect_renum_auto(renum, md_fb);
  mdc_ch_p0.add_part(md_fb);
  mdv_ch_p0.copy(mdc_ch_p0);

  /* ajout de ces nouveaux items dans faces_voisins */
  IntTab proc_face(0, 2), proc_face_bord(0, 2);
  creer_tableau_faces(proc_face), MD_Vector_tools::creer_tableau_distribue(mdv_ch_p0, proc_face_bord, Array_base::NOCOPY_NOINIT);
  for (int f = 0; f < premiere_face_int(); f++) for (int i = 0; i < 2; i++)
      proc_face(f, i) = proc_face_bord(nb_elem_tot() + f, i) = i ? f : Process::me();
  proc_face.echange_espace_virtuel(), proc_face_bord.echange_espace_virtuel();
  std::map<std::array<int, 2>, int> pf_idx; //indice du couple (processeur, face de bord) dans l'espace virtuel mdv_ch_p0
  for (int idx = nb_elem_tot(); idx < proc_face_bord.dimension_tot(0); idx++)
    pf_idx[ {{ proc_face_bord(idx, 0), proc_face_bord(idx, 1) }}] = idx;
  for (int f = 0; f < nb_faces_tot(); f++) if (face_voisins_(f, 1) < 0 && pf_idx.count({{ proc_face(f, 0), proc_face(f, 1) }}))
  face_voisins_(f, 1) = pf_idx.at({{ proc_face(f, 0), proc_face(f, 1) }});
  nb_ch_p0_tot_ = proc_face_bord.dimension_tot(0);

  /* et ajout dans xp_ */
  DoubleTab xp_new(0, dimension);
  MD_Vector_tools::creer_tableau_distribue(mdv_ch_p0, xp_new, Array_base::NOCOPY_NOINIT);
  for (int e = 0; e < nb_elem(); e++) for (int r = 0; r < dimension; r++) xp_new(e, r) = xp_(e, r);
  for (int f = 0; f < premiere_face_int(); f++) for (int r = 0; r < dimension; r++) xp_new(nb_elem_tot() + f, r) = xv_(f, r);
  xp_new.echange_espace_virtuel(), xp_ = xp_new;

  //MD_vector pour Champ_Face_CoviMAC (faces + dimension * champ_p0)
  MD_Vector_composite mdc_ch_face;
  mdc_ch_face.add_part(md_vector_faces());
  for (int r = 0; r < dimension; r++) mdc_ch_face.add_part(mdv_ch_p0);
  mdv_ch_face.copy(mdc_ch_face);
  nb_ch_face_tot_ = nb_faces_tot() + dimension * nb_ch_p0_tot_;

  //volumes_entrelaces_ et volumes_entrelaces_dir : par projection de l'amont/aval sur la normale a la face
  creer_tableau_faces(volumes_entrelaces_);
  volumes_entrelaces_dir_.resize(0, 2), creer_tableau_faces(volumes_entrelaces_dir_);
  for (int f = 0; f < nb_faces(); f++) for (int i = 0; i < 2; volumes_entrelaces_(f) += volumes_entrelaces_dir_(f, i), i++)
      volumes_entrelaces_dir_(f, i) = dabs(dot(&xp_(face_voisins_(f, i), 0), &face_normales_(f, 0), &xv_(f, 0)));
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

/* recherche d'une matrice W verifiant W.R = N avec sum_{i!=j} w_{ij}^2 minimal et un spectre acceptable */
/* en entree,W contient le stencil admissible  */
void Zone_CoviMAC::W_stabiliser(DoubleTab& W, DoubleTab& R, DoubleTab& N, int *ctr, double *spectre) const
{
  int i, j, k, l, n_f = R.dimension(0), nv = 0, d = R.dimension(1), infoo = 0, lwork = -1, sym, diag, it, cv;

  /* idx : numero de l'inconnue W(i, j) dans le probleme d'optimisation */
  //si NtR est symetrique, alors on cherche a avoir W symetrique
  Matrice33 NtR(0, 0, 0, 0, d < 2, 0, 0, 0, d < 3), iNtR;
  for (i = 0; i < d; i++) for (j = 0; j < d; j++) for (k = 0; k < n_f; k++) NtR(i, j) += N(k, i) * R(k, j);
  for (i = 0, sym = 1; i < d; i++) for (j = i + 1; j < d; j++) sym &= (dabs(NtR(i, j) - NtR(j, i)) < 1e-8);
  //remplissage de idx(i, j) : numero de W_{ij} dans le pb d'optimisation
  IntTrav idx(n_f, n_f);
  if (sym) for (i = 0; i < n_f; i++) for (j = i; j < n_f; j++) idx(i, j) = idx(j, i) = (W(i, j) ? nv++ : -1);
  else for (i = 0; i < n_f; i++) for (j = 0; j < n_f; j++) idx(i, j) = (W(i, j) ? nv++ : -1);

  /* version non stabilisee : W0 = N.(NtR)^-1.Nt */
  Matrice33::inverse(NtR, iNtR); //crash si NtR non inversible
  for (i = 0, W = 0; i < n_f; i++) for (j = 0; j < d; j++) for (k = 0; k < d; k++) for (l = 0; l < n_f; l++) W(i, l) += N(i, j) * iNtR(j, k) * N(l, k);
  //spectre de Ws (partie symetrique de W)
  DoubleTrav Ws(n_f, n_f), S(n_f), work(1);
  for (i = 0; i < n_f; i++) for (j = 0; j < n_f; j++) Ws(i, j) = (W(i, j) + W(j, i)) / 2;
  char jobz = 'N', uplo = 'U';
  F77NAME(dsyev)(&jobz, &uplo, &n_f, &Ws(0, 0), &n_f, &S(0), &work(0), &lwork, &infoo);//"workspace query"
  work.resize(lwork = work(0));
  F77NAME(dsyev)(&jobz, &uplo, &n_f, &Ws(0, 0), &n_f, &S(0), &work(0), &lwork, &infoo);//vrai appel
  assert(S(0) > -1e-8 && S(n_f - dimension) > 0);
  if (spectre) spectre[0] = min(spectre[0], S(n_f - dimension)), spectre[2] = max(spectre[2], S(n_f - 1));
  //bornes sur le spectre de la matrice finale
  double l_min = S(n_f - dimension) / 4, l_max = S(n_f - 1) * 4;

  /* probleme d'optimisation : sur les W_{ij} autorises */
  OSQPData data;
  OSQPSettings settings;
  osqp_set_default_settings(&settings);
  settings.scaled_termination = 1, settings.polish = 1, settings.eps_abs = settings.eps_rel = 1e-8, settings.max_iter = 1e5;

  /* contrainte : W.R = N */
  std::vector<std::map<int, double>> C(nv); //stockage CSC : C[j][i] = M_{ij}
  std::vector<double> lb, ub; //bornes inf/sup
  int il = 0; //suivi de la ligne qu'on est en train de creer
  for (i = 0; i < n_f; i++) for (j = 0; j < d; j++, il++) for (k = 0, lb.push_back(N(i, j)), ub.push_back(N(i, j)); k < n_f; k++)
        if (idx(i, k) >= 0) C[idx(i, k)][il] += R(k, j);

  /* objectif: minimiser la norme l2 des termes hors diagonale */
  std::vector<int> P_c(nv + 1), P_l(nv), A_c, A_l; //coeffs de la colonne c : indices [P_c(c), P_c(c + 1)[ dans P_l, P_v
  std::vector<double> P_v(nv), A_v, Q(nv, 0.);
  for (i = 0; i < nv; i++) P_l[i] = i, P_c[i + 1] = i + 1;
  for (i = 0; i < n_f; i++) for (j = 0; j < n_f; j++) if (idx(i, j) >= 0) P_v[idx(i, j)] += (i == j ? 1e-2 : 1);
  data.n = nv, data.P = csc_matrix(nv, nv, P_v.size(), P_v.data(), P_l.data(), P_c.data()), data.q = Q.data();

  //iterations : on resout et on ajoute des contraintes tant que W a un spectre hors de [l_min, l_max]
  std::fenv_t fenv;
  std::feholdexcept(&fenv); //suspend les exceptions FP
  std::vector<double> sol(nv);
  for (cv = 0, it = 0; !cv && it < 100; it++)
    {
      /* assemblage de A : matrice des contraintes */
      for (j = 0, A_c.resize(1), A_l.resize(0), A_v.resize(0); j < nv; j++, A_c.push_back(A_l.size()))
        for (auto && l_v : C[j]) A_l.push_back(l_v.first), A_v.push_back(l_v.second);
      data.A = csc_matrix(lb.size(), nv, A_v.size(), A_v.data(), A_l.data(), A_c.data());
      data.l = lb.data(), data.u = ub.data(), data.m = lb.size();

      /* resolution  */
      OSQPWorkspace *osqp = osqp_setup(&data, &settings);
      if (it) osqp_warm_start_x(osqp, sol.data()); //repart de l'iteration precedente
      osqp_solve(osqp);
      if (osqp->info->status_val == OSQP_PRIMAL_INFEASIBLE || osqp->info->status_val == OSQP_PRIMAL_INFEASIBLE_INACCURATE) abort();
      sol.assign(osqp->solution->x, osqp->solution->x + nv);
      for (i = 0; i < n_f; i++) for (j = 0; j < n_f; j++) W(i, j) = (idx(i, j) >= 0 ? sol[idx(i, j)] : 0);

      /* calcul du spectre : Ws recupere les vecteurs propres */
      for (i = 0; i < n_f; i++) for (j = 0; j < n_f; j++) Ws(i, j) = (W(i, j) + W(j, i)) / 2;
      jobz = 'V', F77NAME(dsyev)(&jobz, &uplo, &n_f, &Ws(0, 0), &n_f, &S(0), &work(0), &lwork, &infoo);

      /* pour chaque vp sortant de [ l_min, l_max ], on ajoute une contrainte pour la restreindre a [l_min * 1.1, l_max / 1.1 ] */
      for (i = 0, cv = (osqp->info->status_val == OSQP_SOLVED); i < n_f; i++) if (S(i) < l_min || S(i) > l_max)
          {
            for (j = 0; j < n_f; j++) for (k = 0; k < n_f; k++) if(idx(j, k) >= 0) C[idx(j, k)][il]  += Ws(i, j) * Ws(i, k);
            lb.push_back(l_min * (S(i) < l_min ? 1.1 : 1)), ub.push_back(l_max / (S(i) > l_max ? 1.1 : 1));
            cv = 0, il++; //on repart avec une ligne en plus
          }
      osqp_cleanup(osqp), free(data.A);
    }
  std::fesetenv(&fenv);     //remet les exceptions FP

  if (!cv) abort(); //ca passe ou ca casse
  if (spectre) for (i = 0; i < n_f; i++) spectre[1] = min(spectre[1], S(i)), spectre[3] = max(spectre[3], S(i));

  //statistiques
  if (!ctr) return;
  //ctr[0] : diagonale, ctr[1] : symetrique
  for (i = 0, diag = 1; i < n_f; i++) for (j = 0; j < n_f; j++) diag &= (i == j || dabs(W(i, j)) < 1e-6);
  ctr[0] += diag, ctr[1] += sym && !diag;
}


//matrice mimetique W_2 : valeurs tangentes aux lignes element-faces -> valeurs normales aux faces
void Zone_CoviMAC::init_w2() const
{
  const IntTab& f_e = face_voisins(), &e_f = elem_faces(), &f_s = face_sommets();
  const DoubleVect& fs = face_surfaces(), &ve = volumes();
  const DoubleTab& nf = face_normales();
  int i, j, e, f, s, n_f, ctr[2] = {0, 0 }, n_tot = Process::mp_sum(nb_elem());
  double spectre[4] = { DBL_MAX, DBL_MAX, 0, 0 }; //vp min (partie consistante, partie stab), vp max (partie consistante, partie stab)

  if (is_init["w2"]) return;
  w2d.set_smart_resize(1), w2i.set_smart_resize(1), w2j.set_smart_resize(1), w2c.set_smart_resize(1);
  Cerr << zone().domaine().le_nom() << " : initialisation de w2...";

  DoubleTab W, R, N;
  W.set_smart_resize(1), R.set_smart_resize(1), N.set_smart_resize(1);

  /* pour le partage entre procs */
  DoubleTrav w2e(0, e_f.dimension(1), e_f.dimension(1));
  zone().creer_tableau_elements(w2e);


  /* calcul sur les elements reels */
  std::map<int, std::vector<int>> som_face; //som_face[s] : faces de l'element e touchant le sommet s
  IntTrav nnz, nef;//par elements : nombre de coeffs non nuls, nombre de faces (lignes)
  zone().creer_tableau_elements(nnz), zone().creer_tableau_elements(nef);
  for (e = 0; e < nb_elem(); e++)
    {
      for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++;
      W.resize(n_f, n_f), R.resize(n_f, dimension), N.resize(n_f, dimension);

      /* matrices R (lignes elements -> faces) et N (normales sortantes aux faces) */
      for (i = 0; i < n_f; i++) for (j = 0, f = e_f(e, i); j < dimension; j++)
          N(i, j) = nf(f, j) / fs(f) * (e == f_e(f, 0) ? 1 : -1), R(i, j) = (xv_(f, j) - xp_(e, j)) * fs(f) / ve(e);

      /* faces connectees a chaque sommet, puis stencil admissible */
      for (i = 0, W = 0, som_face.clear(); i < n_f; i++)
        for (j = 0, f = e_f(e, i); j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) som_face[s].push_back(i);
      for (auto &s_fs : som_face) for (auto i1 : s_fs.second) for (auto i2 : s_fs.second) W(i1, i2) = 1;

      /* matrice stabilisee et stockage */
      W_stabiliser(W, R, N, ctr, spectre);
      for (i = 0; i < n_f; i++) for (j = 0, nef(e)++; j < n_f; j++) w2e(e, i, j) = W(i, j), nnz(e) += (dabs(W(i, j)) > 1e-6);
    }

  /* echange et remplissage */
  w2e.echange_espace_virtuel();
  for (e = 0, w2d.append_line(0), w2i.append_line(0); e < nb_elem_tot(); e++, w2d.append_line(w2i.size() - 1))
    {
      for (n_f = 0; n_f < e_f.dimension(1) && e_f(e, n_f) >= 0; ) n_f++;
      for (i = 0; i < n_f; i++, w2i.append_line(w2j.size())) for (j = 0, w2j.append_line(i), w2c.append_line(w2e(e, i, i)); j < n_f; j++)
          if (j != i && dabs(w2e(e, i, j)) > 1e-6) w2j.append_line(j), w2c.append_line(w2e(e, i, j));
    }

  CRIMP(w2d), CRIMP(w2i), CRIMP(w2j), CRIMP(w2c);
  Cerr << 100. * Process::mp_sum(ctr[0]) / n_tot << "/" << 100. * Process::mp_sum(ctr[1]) / n_tot << "% diag/sym lambda : "
       << Process::mp_min(spectre[0]) << " / " << Process::mp_min(spectre[1]) << " -> " << Process::mp_max(spectre[2])
       << " / " << Process::mp_max(spectre[3]) << " width : " << mp_somme_vect(nnz) * 1. / mp_somme_vect(nef) << finl;
  is_init["w2"] = 1;
}


/* interpolation elements/face de bord -> faces d'ordre 2. On stocke plusieurs candidats -> pour le cas heterogene */
// void Zone_CoviMAC::init_finterp() const
// {
//   if (is_init["finterp"]) return; //deja initialisa
//   const IntTab& f_e = face_voisins(), &f_s = face_sommets(), &e_s = zone().les_elems();
//   int i, e, f, s, ne_tot = nb_elem_tot(), nf_tot = nb_faces_tot();
//
//   std::vector<std::set<int>> som_elem(zone().nb_som_tot()); //connectivite sommets-elements avec les faces de bord en plus
//   for (e = 0; e < ne_tot; e++) for (i = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++) som_elem[s].insert(e);
//   for (f = 0; f < nf_tot; f++) if ((e = f_e(f, 1)) >= ne_tot)
//       for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++) som_elem[s].insert(e);
//
//   fid.set_smart_resize(1), fie.set_smart_resize(1);
//   std::map<std::array<double, 3>, int> xp_e; //pour traiter les elements dans un ordre canonique
//   for (f = 0, fid.append_line(0); f < nb_faces_tot(); fid.append_line(fie.size()), f++)
//     {
//       if ((e = f_e(f, 1)) >= ne_tot) fie.append_line(e); //face de bord, geree directement
//       if (e < 0 || e >= ne_tot) continue; //cas ci-dessus ou face de bord trop loin -> plus rien a faire
//       for (i = 0, xp_e.clear(); i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
//         for (auto &&el : som_elem[s]) xp_e[ {{ xp_(el, 0), xp_(el, 1), dimension < 3 ? 0 : xp_(el, 2) }}] = el;
//
//       //on met en premier l'amont/aval de la face, puis le reste
//       for (auto &&c_e : xp_e) if (c_e.second == f_e(f, 0) || c_e.second == f_e(f, 1)) fie.append_line(c_e.second);
//       for (auto &&c_e : xp_e) if (c_e.second != f_e(f, 0) && c_e.second != f_e(f, 1)) fie.append_line(c_e.second);
//     }
//
//   CRIMP(fid), CRIMP(fie);
//   is_init["finterp"] = 1;
// }

void Zone_CoviMAC::init_finterp() const
{
  if (is_init["finterp"]) return; //deja initialisa
  const IntTab& e_f = elem_faces(), &f_e = face_voisins(), &f_s = face_sommets();
  int i, j, k, e, eb, f, fb, s, ne_tot = nb_elem_tot(), ok;

  fid.set_smart_resize(1), fie.set_smart_resize(1);
  std::map<std::array<double, 3>, int> xp_e; //pour traiter les elements dans un ordre canonique
  std::set<int> soms; //liste de sommets d'une face
  for (f = 0, fid.append_line(0); f < nb_faces_tot(); fid.append_line(fie.size()), f++)
    {
      if ((e = f_e(f, 1)) >= ne_tot) fie.append_line(e); //face de bord, geree directement
      if (e < 0 || e >= ne_tot) continue; //cas ci-dessus ou face de bord trop loin -> plus rien a faire
      for (i = 0, soms.clear(); i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++) soms.insert(s);
      for (i = 0, xp_e.clear(); i < 2; i++) for (e = f_e(f, i), j = 0; j < e_f.dimension(1) && (fb = e_f(e, j)) >= 0; j++)
          {
            for (k = 0, ok = 0; !ok && k < f_s.dimension(1) && (s = f_s(fb, k)) >= 0; k++) ok += soms.count(s);
            if (ok && (eb = f_e(fb, e == f_e(fb, 0))) >= 0)
              xp_e[ {{ xp_(eb, 0), xp_(eb, 1), dimension < 3 ? 0 : xp_(eb, 2) }}] = eb;
          }
      //on met en premier l'amont/aval de la face, puis le reste
      for (auto &&c_e : xp_e) if (c_e.second == f_e(f, 0) || c_e.second == f_e(f, 1)) fie.append_line(c_e.second);
      for (auto &&c_e : xp_e) if (c_e.second != f_e(f, 0) && c_e.second != f_e(f, 1)) fie.append_line(c_e.second);
    }

  CRIMP(fid), CRIMP(fie);
  is_init["finterp"] = 1;
}

/* utilise les points identifies ci-dessus pour interpoler "harmoniquement" aux faces */
Zone_CoviMAC::interp_t Zone_CoviMAC::finterp(int f, int dnu, double *inu_am, double *inu_av) const
{
  assert(fid(f) < fid(f + 1)); //si on tombe ici, alors joint pas assez large...
  if (fid(f + 1) == fid(f) + 1) //variable geree directement -> trivial
    return { {{ fie(fid(f)), -1, -1, -1 }}, {{ 1, 0 , 0, 0 }}};

  const DoubleTab& nf = face_normales();
  int i4[4], i, j, k, l, dp1 = dimension + 1, lwork = dp1 * dp1, infoo = 0;

  /* cas heterogene : interpolation harmonique */
  IntTrav piv(dp1);
  DoubleTrav iNam(dimension, dimension), iNav(dimension, dimension), M(dp1, dp1), work(lwork);
  if (dnu == 1) for (i = 0; i < dimension; i++) iNam(i, i) = inu_am[0], iNav(i, i) = inu_av[0]; //isotrope
  else if (dnu == dimension) for (i = 0; i < dimension; i++) iNam(i, i) = inu_am[i], iNav(i, i) = inu_av[i]; //anisotrope diagonal
  else for (i = 0; i < dimension; i++) for (j = 0; j < dimension; j++) /* anisotrope complet */
        iNam(i, j) = inu_am[dimension * i + j], iNav(i, j) = inu_av[dimension * i + j];

  interp_t resu;
  double a_min, a_min_max = -DBL_MAX;
  /* on cherche a maximiser a_min : sans compter a_min = 0 (degenerescence) */
  // for (i4[0] = fid(f); i4[0] < fid(f + 1); i4[0]++) for (i4[1] = i4[0] + 1; i4[1] < fid(f + 1); i4[1]++)
  for (i4[0] = fid(f), i4[1] = i4[0] + 1,i4[2] = i4[1] + 1; i4[2] < fid(f + 1); i4[2]++) for (i4[3] = i4[2] + 1; i4[3] < (dimension < 3 ? i4[2] + 2 : fid(f + 1)); i4[3]++)
      {
        /* remplissage de la matrice */
        for (j = 0, M = 0; j < dp1; M(j, dimension) = 1, j++)
          {
            DoubleTrav& iN = dot(&xp_(fie(i4[j]), 0), &nf(f, 0), &xv_(f, 0)) > 0 ? iNav : iNam; //matrice du bon cote
            for (k = 0; k < dimension; k++) for (l = 0; l < dimension; l++) M(j, k) += iN(k, l) * (xp_(fie(i4[j]), l) - xv_(f, l));
          }

        /* inversion */
        F77NAME(dgetrf)(&dp1, &dp1, &M(0, 0), &dp1, &piv(0), &infoo);//decomposition LU
        if (infoo > 0) continue; //matrice singuliere -> pas la peine
        F77NAME(dgetri)(&dp1, &M(0, 0), &dp1, &piv(0), &work(0), &lwork, &infoo); //inverse

        for (i = 0, a_min = DBL_MAX; i < dp1; i++) if (dabs(M(dp1 - 1, i)) > 1e-6) a_min = min(a_min, M(dp1 - 1, i));
        if (a_min < a_min_max + 1e-8) continue; //on n'a pas trouve mieux
        for (i = 0, j = 0; i < dp1; i++) if (dabs(M(dp1 - 1, i)) > 1e-6) resu.first[j] = fie(i4[i]), resu.second[j] = M(dp1 - 1, i), j++;
        while (j < 4) resu.first[j] = -1, resu.second[j] = 0, j++;
        a_min_max = a_min;
      }

  return resu;
}
