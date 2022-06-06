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

#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Rectangle.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <Tetraedre.h>
#include <Quadrangle_VEF.h>
#include <Hexaedre_VEF.h>
#include <Periodique.h>
#include <Tri_VEF.h>
#include <Tetra_VEF.h>
#include <Quadri_VEF.h>
#include <Hexa_VEF.h>
#include <Domaine.h>
#include <MD_Vector_tools.h>

Implemente_instanciable(Zone_VEF,"Zone_VEF",Zone_VF);


//// printOn
//

Sortie& Zone_VEF::ecrit(Sortie& os) const
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
  os << "____ facette_normales_ "<<finl;
  facette_normales_.ecrit(os);
  os << "____ nb_faces_std_ "<<finl;
  os << nb_faces_std_ << finl;
  os << "____ rang_elem_non_std_ "<<finl;
  rang_elem_non_std_.ecrit(os);
  return os;
}

//// printOn
//

Sortie& Zone_VEF::printOn(Sortie& os) const
{
  Zone_VF::printOn(os);

  os << h_carre << finl;
  os << type_elem_ << finl;
  os << nb_elem_std_ << finl;
  os << volumes_entrelaces_ << finl;
  os << face_normales_ << finl;
  os << facette_normales_ << finl;
  os << xp_ << finl;
  os << xv_ << finl;
  os << nb_faces_std_ << finl;
  os << rang_elem_non_std_ << finl;
  return os;
}

//// readOn
//

Entree& Zone_VEF::readOn(Entree& is)
{
  Zone_VF::readOn(is);
  is >> h_carre;
  is >> type_elem_;
  is >> nb_elem_std_ ;
  is >> volumes_entrelaces_ ;
  is >> face_normales_ ;
  is >> facette_normales_ ;
  is >> xp_;
  is >> xv_;
  is >> nb_faces_std_ ;
  is >> rang_elem_non_std_ ;
  return is;
}

void Zone_VEF::swap(int fac1, int fac2, int nb_som_faces )
{

}

// Description:
//  Methode appelee par Zone_VF::discretiser apres la creation
//  des faces reelles.
//  On reordonne les faces de sorte a placer les faces "non standard"
//  au debut de la liste des faces. Les faces non standard sont celles
//  dont les volumes de controles sont modifies par les conditions aux
//  limites.
void Zone_VEF::reordonner(Faces& les_faces)
{
  if (Process::je_suis_maitre())
    Cerr << "Zone_VEF::reordonner les_faces " << finl;

  // Construction de rang_elem_non_std_ :
  //  C'est un vecteur indexe par les elements du domaine.
  //  size() = nb_elem()
  //  size_tot() = nb_elem_tot()
  //  Valeurs dans le tableau :
  //   rang_elem_non_std_[i] = -1 si l'element i est standard,
  //  sinon
  //   rang_elem_non_std_[i] = j, ou j est l'indice de l'element dans
  //   les tableaux indexes par les elements non standards (par exemple
  //   le tableau Zone_Cl_VEF::type_elem_Cl_).
  // Un element est non standard s'il est voisin d'une face frontiere.
  {
    const int nb_faces_front = zone().nb_faces_frontiere();
    zone().creer_tableau_elements(rang_elem_non_std_, ArrOfInt::NOCOPY_NOINIT);
    rang_elem_non_std_ = -1;
    // D'abord on marque les elements non standards avec rang_elem_non_std_[i] = 0
    for (int i_face = 0; i_face < nb_faces_front; i_face++)
      {
        const int elem = les_faces.voisin(i_face, 0);
        rang_elem_non_std_[elem] = 0;
      }
    rang_elem_non_std_.echange_espace_virtuel();

    // On construit le md_vector des elements non standards, mais il n'est pas utilise.
    // L'important, c'est que rang_elem_non_std_ soit rempli correctement pour etre
    //  utilise par creer_md_vect_renum() dans Zone_Cl_VEF::associer()
    MD_Vector md_vect_elems_non_std;
    MD_Vector_tools::creer_md_vect_renum_auto(rang_elem_non_std_, md_vect_elems_non_std);
  }

  // Construction du tableau de renumerotation des faces. Ce tableau,
  // une fois trie dans l'ordre croissant donne l'ordre des faces dans
  // la zone_VF. La cle de tri est construite de sorte a pouvoir retrouver
  // l'indice de la face a partir de la cle par la formule :
  //  indice_face = cle % nb_faces
  const int nbr_faces = les_faces.nb_faces();
  ArrOfInt sort_key(nbr_faces);
  {
    nb_faces_std_ =0;
    const int nb_faces_front = zone().nb_faces_frontiere();
    // Attention : face_voisins_ n'est pas encore initialise, il
    // faut passer par les_faces.voisins() :
    const IntTab& facevoisins = les_faces.voisins();
    // On place en premier les faces de bord:
    int i_face;
    for (i_face = 0; i_face < nbr_faces; i_face++)
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
                key = i_face + nbr_faces;
                nb_faces_std_++;
              }
          }
        sort_key[i_face] = key;
      }
    sort_key.ordonne_array();

    // On transforme a nouveau la cle en numero de face:
    for (i_face = 0; i_face < nbr_faces; i_face++)
      {
        const int key = sort_key[i_face] % nbr_faces;
        sort_key[i_face] = key;
      }
  }
  // On reordonne les faces:
  {
    IntTab& faces_sommets = les_faces.les_sommets();
    IntTab old_tab(faces_sommets);
    const int nb_som_faces = faces_sommets.line_size();
    for (int i = 0; i < nbr_faces; i++)
      {
        const int old_i = sort_key[i];
        for (int j = 0; j < nb_som_faces; j++)
          faces_sommets(i, j) = old_tab(old_i, j);
      }
  }

  {
    IntTab& faces_voisins = les_faces.voisins();
    IntTab old_tab(faces_voisins);
    for (int i = 0; i < nbr_faces; i++)
      {
        const int old_i = sort_key[i];
        faces_voisins(i, 0) = old_tab(old_i, 0);
        faces_voisins(i, 1) = old_tab(old_i, 1);
      }
  }

  // Calcul de la table inversee: reverse_index[ancien_numero] = nouveau numero
  ArrOfInt reverse_index(nbr_faces);
  {
    for (int i = 0; i < nbr_faces; i++)
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
        array[i] = reverse_index[old];
      }
  }
  // Mise a jour des indices des faces de joint:
  {
    Joints&      joints    = zone().faces_joint();
    const int nbr_joints = joints.size();
    for (int i_joint = 0; i_joint < nbr_joints; i_joint++)
      {
        Joint&     un_joint         = joints[i_joint];
        ArrOfInt& indices_faces = un_joint.set_joint_item(Joint::FACE).set_items_communs();
        const int nbr_indfaces    = indices_faces.size_array();
        assert(nbr_indfaces == un_joint.nb_faces()); // renum_items_communs rempli ?
        for (int i = 0; i < nbr_indfaces; i++)
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

void Zone_VEF::discretiser()
{
  Zone& zone_geom=zone();
  typer_elem(zone_geom);
  Elem_geom_base& elem_geom = zone_geom.type_elem().valeur();
  Zone_VF::discretiser();

  // Correction du tableau face_voisins:
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



  if (sub_type(Tri_VEF,type_elem_.valeur()))
    {
      if (!sub_type(Triangle,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " <<
               elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Triangle est compatible avec la discretisation VEF en dimension 2" << finl;
          Cerr << " Il faut trianguler le domaine lorsqu'on utilise le mailleur interne" ;
          Cerr << " en utilisant l'instruction: Trianguler nom_dom" <<
               finl;
          exit();
        }
    }
  else if (sub_type(Tetra_VEF,type_elem_.valeur()))
    {
      if (!sub_type(Tetraedre,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " <<
               elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Tetraedre est compatible avec la discretisation VEF en dimension 3" << finl;
          Cerr << " Il faut tetraedriser le domaine lorsqu'on utilise le mailleur interne";
          Cerr << " en utilisant l'instruction: Tetraedriser nom_dom" <<
               finl;
          exit();
        }
    }

  else if (sub_type(Quadri_VEF,type_elem_.valeur()))
    {

      if (!sub_type(Quadrangle_VEF,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          exit();
        }
    }
  else if (sub_type(Hexa_VEF,type_elem_.valeur()))
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
    creer_tableau_faces(face_normales_, Array_base::NOCOPY_NOINIT);
    const IntTab& face_som = face_sommets();
    const IntTab& face_vois = face_voisins();
    const IntTab& elem_face = elem_faces();
    const int n_tot = nb_faces_tot();
    for (num_face = 0; num_face < n_tot; num_face++)
      {
        type_elem_.normale(num_face,
                           face_normales_,
                           face_som, face_vois, elem_face, zone_geom);
      }
  }

  // Calcul de facette_normales_
  type_elem_.creer_facette_normales(zone_geom,
                                    facette_normales(),
                                    rang_elem_non_std());

  calculer_volumes_entrelaces();
  Cerr << "Informations of the Zone VEF of the domain " << zone().domaine().le_nom() << " : " << finl;

  calculer_h_carre();
  // Calcul des porosites
  Zone_VF::calculer_porosites();
}

void Zone_VEF::calculer_h_carre()
{
  const int nbe=nb_elem();
  // Calcul de h_carre
  h_carre = 1.e30;
  h_carre_.resize(nbe);
  // Calcul des surfaces
  const DoubleVect& surfaces=face_surfaces();
  const int nb_faces_elem=zone().nb_faces_elem();
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
  // Modif BM: je pense qu'avec mp_min(h_carre) on va eviter beaucoup de differences seq/par
  h_carre = mp_min(h_carre);
  Cerr << "Lowest cell size h=(Volume/max(Surface))= " << sqrt(h_carre) << finl;
  double moyenne = 0.;
  for (int i = 0; i < nbe; i++)
    moyenne += h_carre_(i);
  moyenne = mp_sum(moyenne);
  double h_carre_moyen = moyenne / mp_sum((double)nbe);
  Cerr << "Average cell size <h>= " << sqrt(h_carre_moyen) << finl;
  if (h_carre*1e6<h_carre_moyen)
    Cerr << "Warning, a cell size is more than 1000 times smaller than the average cell size of the domain. Check your mesh." << finl;
  Cerr<<"=============================================="<<finl;
}

void Zone_VEF::calculer_volumes_entrelaces()
{
  //  Cerr << "les normales aux faces " << face_normales() << finl;
  // On calcule les volumes entrelaces;

  // Si domaine dynamique, le tableau peut eventuellement deja avoir la bonne structure, ou pas.
  // S'il n'est pas deformable, on n'est pas cense passer ici deux fois.
  assert(zone().domaine().deformable()
         || !(volumes_entrelaces_.get_md_vector().non_nul()));
  if (!(volumes_entrelaces_.get_md_vector() == md_vector_faces()))
    {
      volumes_entrelaces_.reset();
      creer_tableau_faces(volumes_entrelaces_, Array_base::NOCOPY_NOINIT);
    }
  int nb_faces_elem=zone().nb_faces_elem();
  int num_face;
  int nmax = premiere_face_int();
  const double facteur = 1. / nb_faces_elem;
  for (num_face = 0; num_face < nmax; num_face++)
    {
      int elem1 = face_voisins_(num_face,0);
      volumes_entrelaces_[num_face] = volumes(elem1) * facteur;
    }
  nmax = nb_faces();
  for ( ; num_face < nmax; num_face++)
    {
      int elem1 = face_voisins_(num_face,0);
      int elem2 = face_voisins_(num_face,1);
      volumes_entrelaces_[num_face] = (volumes(elem1) + volumes(elem2)) * facteur;
    }
  volumes_entrelaces_.echange_espace_virtuel();
}

void Zone_VEF::remplir_elem_faces()
{
  creer_faces_virtuelles_non_std();
}

void Zone_VEF::modifier_pour_Cl(const Conds_lim& conds_lim)
{
  Journal() << "Zone_VEF::Modifier_pour_Cl" << finl;
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
                  vol = (volumes(elem1) + volumes(elem2))/nb_faces_elem;
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
  //                      l'appel a marquer_faces_double_contrib s'effectue dans cette methode
  //                      afin de pouvoir beneficier de conds_lim.
  Zone_VF::marquer_faces_double_contrib(conds_lim);
  // Construction du tableau num_fac_loc_
  construire_num_fac_loc();
}


void Zone_VEF::typer_elem(Zone& zone_geom)
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

// Description:
//   creation de l'espace distant pour les faces virtuelles;
//   creation du tableau des faces virtuelles de bord
void Zone_VEF::creer_faces_virtuelles_non_std()

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

DoubleTab& Zone_VEF::vecteur_face_facette()
{
  // On construit si de taille nul
  // ou si le maillage est deformable
  if (vecteur_face_facette_.size()==0 || zone().domaine().deformable())
    {
      // Taille 8*n*4*3*2=192n
      const int nfa7 = type_elem().nb_facette();
      const int nb_poly_tot = nb_elem_tot();
      vecteur_face_facette_.resize(nb_poly_tot,nfa7,dimension,2);
      const IntTab& KEL=type_elem().valeur().KEL();
      const IntTab& les_Polys = zone().les_elems();
      const DoubleTab& coord =  zone().domaine().coord_sommets();
      const DoubleTab& xg = xp();
      //const int nb_som_facette = zone().type_elem().nb_som_face();
      int nb_som_facette=dimension;
      int num1,num2;
      for (int poly=0; poly<nb_poly_tot; poly++)
        {
          // Boucle sur les facettes du polyedre non standard:
          for (int fa7=0; fa7<nfa7; fa7++)
            {
              num1 = elem_faces(poly,KEL(0,fa7));
              num2 = elem_faces(poly,KEL(1,fa7));

              // Calcul des rx0 et rx1 :
              for (int i=0; i<dimension; i++)
                {
                  // Calcul de la ieme coordonnee du centre de la fa7
                  double coord_centre_fa7 = xg(poly,i);
                  for (int num_som_fa7=0; num_som_fa7<nb_som_facette-1; num_som_fa7++)
                    {
                      int isom_loc = KEL(num_som_fa7+2,fa7);
                      int isom_glob = les_Polys(poly,isom_loc);
                      coord_centre_fa7 += coord(isom_glob,i);
                    }
                  coord_centre_fa7 /= nb_som_facette;
                  // Fin calcul de la ieme coordonnee du centre de la fa7

                  // Calcul de la ieme coordonnee du centre des faces num1 et num2
                  double coord_centre_num1 = 0;
                  double coord_centre_num2 = 0;
                  for (int num_som_face=0; num_som_face<nb_som_facette; num_som_face++)
                    {
                      int s1 = face_sommets(num1,num_som_face);
                      int s2 = face_sommets(num2,num_som_face);
                      coord_centre_num1 += coord(s1,i);
                      coord_centre_num2 += coord(s2,i);
                    }
                  coord_centre_num1 /= nb_som_facette;
                  coord_centre_num2 /= nb_som_facette;
                  // Fin de Calcul de la ieme coordonnee du centre des faces num1 et num2
                  vecteur_face_facette_(poly,fa7,i,0) = coord_centre_fa7 - coord_centre_num1;
                  vecteur_face_facette_(poly,fa7,i,1) = coord_centre_fa7 - coord_centre_num2;
                }
              // Fin de Calcul des rx0 et rx1
            }
        }
    }
  return vecteur_face_facette_;
}
