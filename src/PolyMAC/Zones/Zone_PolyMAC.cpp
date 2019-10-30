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
// File:        Zone_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Zone_PolyMAC.h>
#include <tuple>
#include <unordered_map>
#include <Rectangle.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <Segment.h>
#include <Tetraedre.h>
#include <Quadrangle_VEF.h>
#include <Hexaedre_VEF.h>
#include <Zone_Cl_PolyMAC.h>
#include <Tri_PolyMAC.h>
#include <Segment_PolyMAC.h>
#include <Tetra_PolyMAC.h>
#include <Quadri_PolyMAC.h>
#include <Hexa_PolyMAC.h>
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

#include <LireMED.h>
#include <EcrMED.h>
#include <Champ_implementation_P1.h>
#include <Linear_algebra_tools_impl.h>
#include <IntLists.h>
#include <DoubleLists.h>

//#include <osqp/osqp.h>
#include <unistd.h>

#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <cfloat>
#include <numeric>





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


MD_Vector condensed_md_vector(const IntVect& renum)
{
  MD_Vector mdr;
  const MD_Vector& mds = renum.get_md_vector();
  if (!mds.non_nul()) return mdr;
  const MD_Vector_std& mdstd = ref_cast(MD_Vector_std, mds.valeur());

  //items[proc] = { items a envoyer, items a recevoir, debut/fin de blocs a recevoir }
  std::map<int, std::array<std::vector<int>, 3> > items;
  int i, j, k, l, p;
  for (i = 0; i < mdstd.pe_voisins_.size_array(); i++)
    {
      p = mdstd.pe_voisins_(i);
      //filtre sur les items a envoyer
      for (j = 0; j < mdstd.items_to_send_.get_list_size(i); j++) if ((k = renum(mdstd.items_to_send_(i, j))) >= 0)
          items[p][0].push_back(k);
      //filtre sur les items a recevoir
      for (j = 0; j < mdstd.items_to_recv_.get_list_size(i); j++) if ((k = renum(mdstd.items_to_recv_(i, j))) >= 0)
          items[p][1].push_back(k);
      //filtre sur les blocs a recevoir
      for (j = 0; j < mdstd.blocs_to_recv_.get_list_size(i); j += 2) for (k = mdstd.blocs_to_recv_(i, j); k < mdstd.blocs_to_recv_(i, j + 1); k++)
          if ((l = renum(k)) >= 0)
            {
              if (items[p][2].size() && l == items[p][2].back()) items[p][2].back()++; //on s'ajoute au dernier bloc...
              else items[p][2].insert(items[p][2].end(), { l, l + 1 });                //ou on en cree un autre
            }
    }
  ArrOfInt pe_voisins(items.size());
  VECT(ArrOfInt) items_to_send(items.size()), items_to_recv(items.size()), blocs_to_recv(items.size());
  i = 0;
  for (auto &&kv : items)
    {
      for (j = 0, items_to_send(i).resize(k = kv.second[0].size()); j < k; j++) items_to_send(i)(j) = kv.second[0][j];
      for (j = 0, items_to_recv(i).resize(k = kv.second[1].size()); j < k; j++) items_to_recv(i)(j) = kv.second[1][j];
      for (j = 0, blocs_to_recv(i).resize(k = kv.second[2].size()); j < k; j++) blocs_to_recv(i)(j) = kv.second[2][j];
      pe_voisins(i) = kv.first, i++;
    }
  int nb_items = 0, nb_items_tot = 0;
  for (i = 0; i < renum.size(); i++) nb_items += (renum(i) >= 0);
  for (i = renum.size(), nb_items_tot = nb_items; i < renum.size_totale(); i++) nb_items_tot += (renum(i) >= 0);
  MD_Vector_std mdrs(nb_items_tot, nb_items, pe_voisins, items_to_send, items_to_recv, blocs_to_recv);
  mdr.copy(mdrs);
  return mdr;
}

void Zone_PolyMAC::swap(int fac1, int fac2, int nb_som_faces )
{

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
      for (int r = 0; r < 3; r++) xv_(face, r) = xs[r] / S;
    }
  xv_.echange_espace_virtuel();

  // orthocentrer();

  detecter_faces_non_planes();

  Zone_VF::calculer_porosites();
  Zone_VF::calculer_diametres_hydrauliques();
  calculer_volumes_entrelaces();

  //diverses quantites liees aux aretes
  if (dimension > 2)
    {
      zone().creer_aretes();
      md_vector_aretes_ = zone().aretes_som().get_md_vector();
      //remplissage de som_aretes
      som_arete.resize(zone().nb_som_tot());
      for (int i = 0; i < zone().aretes_som().dimension_tot(0); i++) som_arete[zone().aretes_som()(i, 0)][zone().aretes_som()(i, 1)] = i;
      //remplissage de xa (CGs des aretes), de ta_ (vecteur tangent aux aretes) et de longueur_arete_ (longueurs des aretes)
      xa_.resize(0, 3), ta_.resize(0, 3);
      creer_tableau_aretes(xa_), creer_tableau_aretes(ta_), creer_tableau_aretes(longueur_aretes_);
      const DoubleTab& xs = zone().domaine().coord_sommets();
      for (int i = 0, k; i < zone().nb_aretes_tot(); i++)
        {
          int s1 = zone().aretes_som()(i, 0), s2 = zone().aretes_som()(i, 1), sgn = 0;
          longueur_aretes_(i) = sqrt( dot(&xs(s2, 0), &xs(s2, 0), &xs(s1, 0), &xs(s1, 0)));
          for (k = 0; k < 3 && !sgn; k++) if (dabs(xs(s2, k) - xs(s1, k)) > 1e-8) sgn = xs(s2, k) > xs(s1, k) ? 1 : -1;
          for (k = 0; k < 3; k++) xa_(i, k) = (xs(s1, k) + xs(s2, k)) / 2, ta_(i, k) = sgn * (xs(s2, k) - xs(s1, k)) / longueur_aretes_(i);
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
          d2min = min(d2min, d2 = dot(&xs(s, 0), &xs(s, 0), &xv_(f, 0), &xv_(f, 0))), d2max = max(d2max, d2);
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
                a = som_arete[min(s, s2)].at(max(s, s2));
            std::array<double, 3> vec = cross(3, 3, &xv_(f, 0), &ta_(a, 0), &xs(s, 0));
            r2min = min(r2min, dot(&vec[0], &vec[0]));
          }
        if (dot(&X(0, 0), &X(0, 0)) > 0.25 * r2min) continue; //on s'eloigne trop du CG

        for (i = 0, b_f_ortho(f) = 1; i < dimension; i++) if (dabs(X(i, 0)) > 1e-8) xv_(f, i) += X(i, 0);
      }
  Cerr << 100. * mp_somme_vect(b_f_ortho) / Process::mp_sum(nb_faces()) << "% de faces orthocentrees" << finl;

  /* 2. orthocentrage des elements */
  Cerr << zone().domaine().le_nom() << " : ";
  for (e = 0; e < nb_elem_tot(); e++)
    {
      //l'element est-il deja orthocentre?
      double d2min = DBL_MAX, d2max = 0, d2;
      for (i = 0, np = 0; i < e_s.dimension(1) && (s = e_s(e, i)) >= 0; i++, np++)
        d2min = min(d2min, d2 = dot(&xs(s, 0), &xs(s, 0), &xp_(e, 0), &xp_(e, 0))), d2max = max(d2max, d2);
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
        rmin = min(rmin, dabs(dot(&xp_(e, 0), &nf(f, 0), &xv_(f, 0)) / fs(f)));
      if (dot(&X(0, 0), &X(0, 0)) > 0.25 * rmin * rmin) continue; //on s'eloigne trop du CG

      for (i = 0, b_e_ortho(e) = 1; i < dimension; i++) if (dabs(X(i, 0)) > 1e-8) xp_(e, i) += X(i, 0);
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
      if ((sin2 = std::pow(dot(&xs(s, 0), &nf(f, 0), &xv_(f, 0)) / fs(f), 2) / dot(&xs(s, 0), &xs(s, 0), &xv_(f, 0), &xv_(f, 0))) > val(rk))
        val(rk) = sin2, face(rk) = f, elem1(rk) = f_e(f, 0), elem2(rk) = f_e(f, 1);
  envoyer_all_to_all(val, val), envoyer_all_to_all(face, face), envoyer_all_to_all(elem1, elem1), envoyer_all_to_all(elem2, elem2);

  for (i = j = sin2 = 0; i < Process::nproc(); i++) if (val(i) > sin2) sin2 = val(i), j = i;
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

void Zone_PolyMAC::calculer_volumes_entrelaces()
{
  //  Cerr << "les normales aux faces " << face_normales() << finl;
  // On calcule les volumes entrelaces;
  //  volumes_entrelaces_.resize(nb_faces());

  creer_tableau_faces(volumes_entrelaces_);
  volumes_entrelaces_dir_.resize(nb_faces(), 2);
  creer_tableau_faces(volumes_entrelaces_dir_);
  const DoubleVect& fs = face_surfaces();

  for (int num_face=0; num_face<nb_faces(); num_face++)
    {
      for (int dir=0; dir<2; dir++)
        {
          int elem = face_voisins_(num_face,dir);
          if (elem!=-1)
            {
              volumes_entrelaces_dir_(num_face, dir) = sqrt(dot(&xp_(elem, 0), &xp_(elem, 0), &xv_(num_face, 0), &xv_(num_face, 0))) * fs[num_face];
              volumes_entrelaces_[num_face] += volumes_entrelaces_dir_(num_face, dir);
            }
        }
    }
  volumes_entrelaces_.echange_espace_virtuel();
  volumes_entrelaces_dir_.echange_espace_virtuel();
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

void disp(const DoubleTab& A)
{
  int i, j, k;
  if (A.get_md_vector().non_nul() && sub_type(MD_Vector_composite, A.get_md_vector().valeur()))
    {
      ConstDoubleTab_parts Ap(A);
      for (i = 0; i < Ap.size(); i++) disp(Ap[i]);
    }
  else if (A.nb_dim() == 3) for (i = 0; i < A.dimension(0); i++)
      for (j = 0, fprintf(stderr, i ? "}},{" : "{{"); j < A.dimension(1); j++)
        for (k = 0, fprintf(stderr, j ? "},{" : "{"); k < A.dimension(2); k++)
          fprintf(stderr, "%.10E%s ", A.addr()[A.dimension(2) * (i * A.dimension(1) + j) + k], k + 1 < A.dimension(2) ? "," : "");
  else if (A.nb_dim() == 2) for (i = 0; i < A.dimension(0); i++)
      for (j = 0, fprintf(stderr, i ? "},{" : "{{"); j < A.dimension(1); j++)
        fprintf(stderr, "%.10E%s ", A.addr()[i * A.dimension(1) + j], j + 1 < A.dimension(1) ? "," : "");
  else for (i = 0, fprintf(stderr, "{"); i < A.dimension(0); i++)
      fprintf(stderr, "%.10E%s ", A.addr()[i], i + 1 < A.dimension(0) ? "," : "");
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

//stabilisation des matrices m1 et m2 de PolyMAC
inline void Zone_PolyMAC::ajouter_stabilisation(DoubleTab& M) const
{
  int i, j, k, i1, i2, j1, j2, n_f = M.dimension(0), lwork = 2 + n_f * (6 + 2 * n_f), liwork = 2 * (3 + 5 * n_f), info = 0;
  DoubleTab A, S, b(n_f, 1), N(1, 1), x(1, 1), work(lwork), V;
  IntTab iwork(liwork);

  /* valeurs propres (S) et noyau (N) */
  kersol(M, b, 1e-12, &N, x, S);
  assert(N.dimension(1) == n_f - dimension); //M doit etre de rang d
  double eps = S(dimension - 1); //valeur propre la plus petite de M (hors noyau)

  /* ajout d'une matrice de forme N P n^t, avec P symetrique pour minimiser les termes hors diag de M */
  int n_k = N.dimension(1), n_l = n_f * (n_f - 1) / 2, n_c = n_k * (n_k + 1) / 2; //une ligne par coeff M(i, j > i), une colonne par terme P(i, j >= i)
  A.resize(n_l, n_c), b.resize(n_l, 1);
  for (i1 = i = 0; i1 < n_f; i1++) for (i2 = i1 + 1; i2 < n_f; i2++, i++) //(i1, i2, i) -> numero de ligne
      for (j1 = j = 0, b(i, 0) = -M(i1, i2); j1 < n_k; j1++) for (j2 = j1; j2 < n_k; j2++, j++) //(j1, j2, j) -> numero de colonne
          A(i, j) = N(i1, j1) * N(i2, j2) + (j1 != j2) * N(i1, j2) * N(i2, j1);
  kersol(A, b, 1e-12, NULL, x, S); //minimise la somme des carres des termes hors diag de M
  //modification de M
  for (i1 = 0; i1 < n_f; i1++) for (i2 = 0; i2 < n_f; i2++) for (j1 = j = 0; j1 < n_k; j1++) for (j2 = j1; j2 < n_k; j2++, j++)
          M(i1, i2) += x(j, 0) * (N(i1, j1) * N(i2, j2) + (j1 != j2) * N(i1, j2) * N(i2, j1));

  /* decomposition de Schur de M, puis renfort de la diagonale pour rendre toutes les vp plsu grandes que eps */
  char jobz = 'V', uplo = 'U';
  V = M, S.resize(n_f);
  F77NAME(dsyevd)(&jobz, &uplo, &n_f, &V(0, 0), &n_f, &S(0), &work(0), &lwork, &iwork(0), &liwork, &info);
  assert(info == 0);
  //ajout pour garantir des vp plus grandes que eps
  for (i = 0, M = 0; i < n_f; i++) for (j = 0; j < n_f; j++) for (k = 0; k < n_f; k++) M(i, j) += V(k, i) * max(S(k), eps) * V(k, j);
}

//interpolation normales aux faces -> elements d'ordre 1
void Zone_PolyMAC::init_ve() const
{
  const IntTab& e_f = elem_faces(), &f_e = face_voisins();
  const DoubleVect& ve = volumes_, &fs = face_surfaces();
  int i, k, e, f;

  if (vedeb.dimension(0)) return;
  Cerr << zone().domaine().le_nom() << " : initialisation de ve... ";
  vedeb.resize(1), vedeb.set_smart_resize(1), veji.set_smart_resize(1), veci.resize(0, 3), veci.set_smart_resize(1);
  //formule (1) de Basumatary et al. (2014) https://doi.org/10.1016/j.jcp.2014.04.033 d'apres Perot
  for (e = 0; e < nb_elem_tot(); vedeb.append_line(veji.dimension(0)), e++)
    for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
      {
        double x[3] = { 0, };
        for (k = 0; k < dimension; k++) x[k] = fs(f) * (xv(f, k) - xp(e, k)) * (e == f_e(f, 0) ? 1 : -1) / ve(e);
        veji.append_line(f), veci.append_line(x[0], x[1], x[2]);
      }
  CRIMP(vedeb), CRIMP(veji), CRIMP(veci);
  Cerr << "OK" << finl;
}

//matrice mimetique d'un champ aux faces : (valeur normale aux faces) -> (integrale lineaire sur les lignes brisees, multipliee par la surface de la face)
void Zone_PolyMAC::init_m2() const
{
  const IntTab& f_e = face_voisins(), &e_f = elem_faces();
  const DoubleVect& fs = face_surfaces(), &ve = volumes();
  int i, j, k, e, f, n_f, lwork = 2 + e_f.dimension(1) * (6 + 2 * e_f.dimension(1)), liwork = 2 * (3 + 5 * e_f.dimension(1));

  if (m2deb.dimension(0)) return;
  init_ve();
  Cerr << zone().domaine().le_nom() << " : initialisation de m2... ";
  std::vector<std::map<std::array<int, 2>, double>> m2(nb_faces_tot());

  DoubleTab A(1, 1), M(1, 1), S(1, 1), b(1, 1), N(1, 1), x(1, 1), V(1, 1), work(lwork);
  IntTab iwork(liwork);
  A.set_smart_resize(1), M.set_smart_resize(1), S.set_smart_resize(1), b.set_smart_resize(1), N.set_smart_resize(1), x.set_smart_resize(1), V.set_smart_resize(1);
  std::map<int, int> idxf;
  for (e = 0; e < nb_elem_tot(); e++, idxf.clear())
    {
      /* matrice non stabilisee */
      for (i = 0, n_f = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) idxf[f] = i, n_f++;
      M.resize(n_f, n_f), b.resize(n_f, 1), x.resize(n_f, 1);
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (k = vedeb(e); k < vedeb(e + 1); k++)
          M(i, idxf[veji(k)]) = dot(&xv_(f, 0), &veci(k, 0), &xp_(e, 0)) * fs(f) * (e == f_e(f, 0) ? 1 : -1);

      /* stabilisation et stockage */
      ajouter_stabilisation(M);
      for (i = 0; i < n_f; i++) for (j = 0; j < n_f; j++) if (dabs(M(i, j)) > 1e-12 * ve(e)) m2[e_f(e, i)][ {{e_f(e, j), e }}] += M(i, j);
    }
  //remplissage
  m2deb.resize(1), m2deb.set_smart_resize(1), m2ji.resize(0, 2), m2ji.set_smart_resize(1), m2ci.set_smart_resize(1);
  for (f = 0; f < nb_faces_tot(); f++, m2deb.append_line(m2ji.dimension(0))) for (auto &&kv : m2[f])
      m2ji.append_line(kv.first[0], kv.first[1]), m2ci.append_line(kv.second);
  CRIMP(m2deb), CRIMP(m2ji), CRIMP(m2ci);
  Process::barrier();
  Cerr << "OK" << finl;
}

//rotationnel aux faces d'un champ tangent aux aretes
void Zone_PolyMAC::init_rf() const
{
  const IntTab& f_s = face_sommets();
  const DoubleTab& xs = zone().domaine().coord_sommets(), &nf = face_normales();
  const DoubleVect& la = longueur_aretes(), &fs = face_surfaces();

  if (rfdeb.dimension(0)) return;
  int i, s, f;
  rfdeb.resize(1), rfdeb.set_smart_resize(1), rfji.set_smart_resize(1), rfci.set_smart_resize(1);
  for (f = 0; f < nb_faces_tot(); rfdeb.append_line(rfji.dimension(0)), f++)
    for (i = 0; i < f_s.dimension(1) && (s = f_s(f, i)) >= 0; i++)
      {
        int s2 = f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0),
            a = dimension < 3 ? s : som_arete[min(s, s2)].at(max(s, s2));
        std::array<double, 3> taz = {{ 0, 0, 1 }}, vec; //vecteur tangent a l'arete et produit vectoriel de celui-ci avec xa - xv
        vec = cross(dimension, 3, dimension < 3 ? &xs(a, 0) : &xa_(a, 0), dimension < 3 ? &taz[0] : &ta_(a, 0), &xv_(f, 0));
        int sgn = dot(&vec[0], &nf(f, 0)) > 0 ? 1 : -1;
        rfji.append_line(a), rfci.append_line(sgn * (dimension < 3 ? 1 : la(a)) / fs(f));
      }
  CRIMP(rfdeb), CRIMP(rfji), CRIMP(rfci);
}

//interpolation aux elements d'un champ dont on connait la composante tangente aux aretes (en 3D ), ou la composante verticale aux sommets en 2D
void Zone_PolyMAC::init_we() const
{
  if (wedeb.dimension(0)) return;

  //remplissage de arete_faces_ (liste des faces touchant chaque arete en 3D, chaque sommet en 2D)
  std::vector<std::vector<int> > a_f_vect(dimension < 3 ? nb_som_tot() : zone().nb_aretes_tot());
  const IntTab& f_s = face_sommets_;
  for (int f = 0, i, s1; f < nb_faces_tot(); f++) for (i = 0; i < f_s.dimension(1) && (s1 = f_s(f, i)) >= 0; i++)
      {
        int s2 = f_s(f, i + 1 < f_s.dimension(1) && f_s(f, i + 1) >= 0 ? i + 1 : 0);
        a_f_vect[dimension < 3 ? s1 : som_arete[min(s1, s2)].at(max(s1, s2))].push_back(f);
      }
  int a_f_max = 0;
  for (int i = 0; i < (int) a_f_vect.size(); i++) a_f_max = max(a_f_max, (int) a_f_vect[i].size());
  arete_faces_.resize(a_f_vect.size(), a_f_max), arete_faces_ = -1;
  for (int i = 0, j; i < arete_faces_.dimension(0); i++) for (j = 0; j < (int) a_f_vect[i].size(); j++) arete_faces_(i, j) = a_f_vect[i][j];

  dimension < 3 ? init_we_2d() : init_we_3d();

}

void Zone_PolyMAC::init_we_2d() const
{
  const IntTab& e_f = elem_faces(), &f_s = face_sommets_;
  const DoubleTab& xs = zone().domaine().coord_sommets();
  const DoubleVect& ve = volumes();
  int i, j, e, f, s;

  wedeb.resize(1), wedeb.set_smart_resize(1), weji.set_smart_resize(1), weci.set_smart_resize(1);
  std::map<int, double> wemi;
  for (e = 0; e < nb_elem_tot(); wedeb.append_line(weji.dimension(0)), wemi.clear(), e++)
    {
      //une contribution par "facette" (triangle entre le sommet, le CG de la face et celui de l'element)
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //contrib. de chaque sommet de chaque face
          wemi[s] += dabs(cross(2, 2, &xp_(e, 0), &xv_(f, 0), &xs(s, 0), &xs(s, 0))[2]) / (2 * ve(e));
      for (auto &&kv : wemi) weji.append_line(kv.first), weci.append_line(kv.second);
    }
  CRIMP(wedeb), CRIMP(weji), CRIMP(weci);
}

void Zone_PolyMAC::init_we_3d() const
{
  const IntTab& e_f = elem_faces(), &f_s = face_sommets_;
  const DoubleVect& la = longueur_aretes_, &ve = volumes_;
  int i, j, k, e, f, s;

  wedeb.resize(1), wedeb.set_smart_resize(1), weji.set_smart_resize(1), weci.resize(0, 3), weci.set_smart_resize(1);
  std::map<int, std::array<double, 3> > wemi;
  for (e = 0; e < nb_elem_tot(); wedeb.append_line(weji.dimension(0)), wemi.clear(), e++)
    {
      //une contribution par "facette" (triangle entre CG de l'arete, le CG de la face et celui de l'element)
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //contrib. de chaque sommet de chaque face
          {
            int s2 = f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0),
                a = som_arete[min(s, s2)].at(max(s, s2));
            std::array<double, 3> vec = cross(3, 3, &xp_(e, 0), &xv_(f, 0), &xa_(a, 0), &xa_(a, 0));
            //on tourne la facette dans la bonne direction
            int sgn = dot(&ta_(a, 0), &vec[0]) > 0 ? 1 : -1;
            for (k = 0; k < 3; k++) wemi[a][k] += sgn * vec[k] * la(a) / (2 * ve(e));
          }
      for (auto &&kv : wemi) weji.append_line(kv.first), weci.append_line(kv.second[0], kv.second[1], kv.second[2]);
    }
  CRIMP(wedeb), CRIMP(weji), CRIMP(weci);
}

//matrice mimetique d'un champ aux aretes : (valeur tangente aux aretes) -> (flux a travers l'union des facettes touchant l'arete)
//en 2D, m1 est toujours diagonale! Il suffit de calculer la surface de l'arete duale...
void Zone_PolyMAC::init_m1_2d() const
{
  const IntTab& e_f = elem_faces(), &f_s = face_sommets_;
  const DoubleTab& xs = zone().domaine().coord_sommets();
  int i, j, e, f, s;

  std::vector<std::map<std::array<int, 2>, double>> m1(zone().nb_som_tot()); //m1[a][{ ab, e }] : contribution de (arete ab, element e)
  for (e = 0; e < nb_elem_tot(); e++)
    {
      //une contribution par "facette" (triangle entre le sommet, le CG de la face et celui de l'element)
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
        for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++) //contrib. de chaque sommet de chaque face
          m1[s][ {{ s, e }}] += dabs(cross(2, 2, &xp_(e, 0), &xv_(f, 0), &xs(s, 0), &xs(s, 0))[2]) / 2;
    }

  //remplissage
  m1deb.resize(1), m1deb.set_smart_resize(1), m1ji.resize(0, 2), m1ji.set_smart_resize(1), m1ci.set_smart_resize(1);
  for (i = 0; i < zone().nb_som_tot(); m1deb.append_line(m1ji.dimension(0)), i++) for (auto &&kv : m1[i])
      m1ji.append_line(kv.first[0], kv.first[1]), m1ci.append_line(kv.second);
  CRIMP(m1deb), CRIMP(m1ji), CRIMP(m1ci);
}

//en 3D, c'est moins trivial...
void Zone_PolyMAC::init_m1_3d() const
{
  const IntTab& f_s = face_sommets_, &e_a = zone().elem_aretes(), &e_f = elem_faces_;
  const DoubleVect& la = longueur_aretes_, &ve = volumes();
  int a, i, j, k, e, f, s, s2, n_a;

  Cerr << zone().domaine().le_nom() << " : initialisation de m1... ";
  std::vector<std::map<std::array<int, 2>, double>> m1(zone().nb_aretes_tot()); //m1[a][{ ab, e }] : contribution de (arete ab, element e)
  DoubleTab M;
  M.set_smart_resize(1);
  std::map<int, int> idxa;
  for (e = 0; e < nb_elem_tot(); e++, idxa.clear())
    {
      /* matrice non stabilisee : contribution par facette (couple face/arete) */
      for (i = 0, n_a = 0; i < e_a.dimension(1) && (a = e_a(e, i)) >= 0; i++) idxa[a] = i, n_a++;
      M.resize(n_a, n_a);
      for (i = 0, M = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (j = 0; j < f_s.dimension(1) && (s = f_s(f, j)) >= 0; j++)
          {
            s2 = f_s(f, j + 1 < f_s.dimension(1) && f_s(f, j + 1) >= 0 ? j + 1 : 0), a = som_arete[min(s, s2)].at(max(s, s2));
            std::array<double, 3> vec = cross(3, 3, &xp_(e, 0), &xv_(f, 0), &xa_(a, 0), &xa_(a, 0));
            //on tourne la facette dans la bonne direction
            int sgn = dot(&ta_(a, 0), &vec[0]) > 0 ? 1 : -1;
            for (k = wedeb(e); k < wedeb(e + 1); k++) M(idxa[a], idxa[weji(k)]) += sgn * la(a) * dot(&vec[0], &weci(k, 0)) / 2;
          }

      /* stabilisation et stockage */
      ajouter_stabilisation(M);
      for (i = 0; i < n_a; i++) for (j = 0; j < n_a; j++) if (dabs(M(i, j)) > 1e-12 * ve(e)) m1[e_a(e, i)][ {{ e_a(e, j), e }}] += M(i, j);
    }

  //remplissage
  m1deb.resize(1), m1deb.set_smart_resize(1), m1ji.resize(0, 2), m1ji.set_smart_resize(1), m1ci.set_smart_resize(1);
  for (a = 0; a < zone().nb_aretes_tot(); m1deb.append_line(m1ji.dimension(0)), a++) for (auto &&kv : m1[a])
      m1ji.append_line(kv.first[0], kv.first[1]), m1ci.append_line(kv.second);
  CRIMP(m1deb), CRIMP(m1ji), CRIMP(m1ci);
  Process::barrier();
  Cerr << "OK" << finl;
}

void Zone_PolyMAC::init_m1() const
{
  if (m1deb.dimension(0)) return;
  init_we();
  dimension < 3 ? init_m1_2d() : init_m1_3d();
}

/* initisalisation de solveurs lineaires pour inverser m1 ou m2 */
static void init_mat(const IntTab& deb, const IntTab& ji, const DoubleTab& ci, Matrice_Morse_Sym& mat, SolveurSys& solv, int ntot)
{
  int i, j;
  if (solv.non_nul()) return;
  /* stencil et allocation */
  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  for (i = 0; i + 1 < deb.dimension(0); i++) for (j = deb(i); j < deb(i + 1); j++) if (i <= ji(j, 0)) stencil.append_line(i, ji(j, 0));
  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_symmetric_morse_matrix(ntot, stencil, mat);

  /* remplissage */
  for (i = 0; i + 1 < deb.dimension(0); i++) for (j = deb(i); j < deb(i + 1); j++) if (i <= ji(j, 0)) mat(i, ji(j, 0)) += ci(j);
  mat.set_est_definie(1);

  char lu[] = "Petsc Cholesky { quiet }";
  EChaine ch(lu);
  ch >> solv;
}

void Zone_PolyMAC::init_m2solv() const
{
  init_m2();
  return init_mat(m2deb, m2ji, m2ci, m2mat, m2solv, nb_faces_tot());
}
void Zone_PolyMAC::init_m1solv() const
{
  init_m1();
  return init_mat(m1deb, m1ji, m1ci, m1mat, m1solv, dimension < 3 ? zone().nb_som_tot() : zone().nb_aretes_tot());
}
