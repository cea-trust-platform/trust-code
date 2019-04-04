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
// File:        Zone_EF.cpp
// Directory:   $TRUST_ROOT/src/EF/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Zone_EF.h>
#include <Zone_Cl_EF.h>
#include <Rectangle.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <Segment.h>
#include <Tetraedre.h>
#include <Quadrangle_VEF.h>
#include <Hexaedre_VEF.h>
#include <Periodique.h>
#include <Tri_EF.h>
#include <Segment_EF.h>
#include <Tetra_EF.h>
#include <Quadri_EF.h>
#include <Hexa_EF.h>
#include <Domaine.h>
#include <Scatter.h>
#include <EFichier.h>

#include <interface_INITGAUSS.h>
#include <interface_CALCULBIJ.h>

Implemente_instanciable(Zone_EF,"Zone_EF",Zone_VF);


//// printOn
//

Sortie& Zone_EF::ecrit(Sortie& os) const
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

Sortie& Zone_EF::printOn(Sortie& os) const
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

Entree& Zone_EF::readOn(Entree& is)
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


void  Zone_EF::calculer_volumes_sommets()
{
  //  volumes_sommets_thilde_.resize(nb_som());
  //Scatter::creer_tableau_distribue(zone().domaine(), Joint::SOMMET, volumes_sommets_thilde_);
//  zone().domaine().creer_tableau_sommets(volumes_sommets_thilde_);
  int nbelem=zone().nb_elem();
  int nb_som_elem=zone().nb_som_elem();
  IPhi_.resize(nbelem,nb_som_elem);
  zone().domaine().creer_tableau_elements(IPhi_);
  //  Scatter::creer_tableau_distribue(zone().domaine(), Joint::ELEMENT, IPhi_);
  IPhi_thilde_=IPhi_;
  double rap=1./zone().nb_som_elem();

  // calcul de IPhi_ version valable poour les carres...
  for (int elem=0; elem<nbelem; elem++)
    {
      double vol=volumes(elem)*rap;
      for (int s=0; s<nb_som_elem; s++)
        {
          IPhi_(elem,s)=vol;
          IPhi_thilde_(elem,s)=vol*porosite_elem_(elem);
        }
    }
  IPhi_.echange_espace_virtuel();
  IPhi_thilde_.echange_espace_virtuel();
  // calcul de volumes_sommets_thilde
  const IntTab& les_elems=zone().les_elems() ;

  DoubleVect volumes_som_prov(volumes_sommets_thilde_);
  for (int elem=0; elem<zone().nb_elem_tot(); elem++)
    {

      for (int s=0; s<zone().nb_som_elem(); s++)
        {
          double vol=IPhi_thilde_(elem,s);
          int s_glob=les_elems(elem,s);
          volumes_sommets_thilde_(s_glob)+=vol;
          volumes_som_prov(s_glob)+=IPhi_(elem,s);
        }
    }

//  volumes_sommets_thilde_.resize(nb_som());
  volumes_sommets_thilde_.echange_espace_virtuel();
  if (porosite_sommets_.size_array()==0)
    {
      // abort();
      porosite_sommets_=volumes_sommets_thilde_;

      for (int som=0; som<nb_som(); som++)
        {
          porosite_sommets_(som)=volumes_sommets_thilde_(som)/volumes_som_prov(som);
        }
      porosite_sommets_.echange_espace_virtuel();
    }
#if 0
  if (0)
    {
      porosite_sommets_.resize(0);
      DoubleVect volumes_som_prov(volumes_sommets_thilde_);
      DoubleVect volumes_som_prov_b(volumes_sommets_thilde_);
      volumes_som_prov=0;
      volumes_som_prov_b=0;
      for (int elem=0; elem<zone().nb_elem_tot(); elem++)
        {

          for (int s=0; s<zone().nb_som_elem(); s++)
            {
              double vol=IPhi_thilde_(elem,s);
              double v2=IPhi_(elem,s);
              int s_glob=les_elems(elem,s);
              volumes_som_prov(s_glob)+=v2/vol;
              volumes_som_prov_b(s_glob)+=1;
            }
        }
      Cerr<<mp_max_vect(porosite_elem_)<<" "<<mp_min_vect( porosite_elem_)<<finl;
      Cerr<<mp_max_vect(volumes_som_prov)<<" "<<mp_min_vect( volumes_som_prov)<<finl;
      Cerr<<mp_max_vect(volumes_som_prov_b)<<" "<<mp_min_vect( volumes_som_prov_b)<<finl;

      if (porosite_sommets_.size_array()==0)
        {
          // abort();
          porosite_sommets_=volumes_sommets_thilde_;

          for (int som=0; som<nb_som(); som++)
            {
              porosite_sommets_(som)=1./volumes_som_prov(som)*volumes_som_prov_b(som);

            }
          porosite_sommets_.echange_espace_virtuel();
        }
      Cerr<<mp_max_vect( porosite_sommets_)<<" "<<mp_min_vect( porosite_sommets_)<<finl;
      for (int elem=0; elem<zone().nb_elem_tot(); elem++)
        for (int s=0; s<zone().nb_som_elem(); s++)
          IPhi_thilde_(elem,s)= IPhi_thilde_(elem,s)*porosite_sommets_(les_elems(elem,s));
    }
#endif

  volumes_thilde_=volumes_;
  for (int elem=0; elem<zone().nb_elem_tot(); elem++)
    {
      volumes_thilde_(elem)=volumes_(elem)*porosite_elem_(elem);
    }
}

void Zone_EF::swap(int fac1, int fac2, int nb_som_faces )
{

}

// Description:
//  Methode appelee par Zone_VF::discretiser apres la creation
//  des faces reelles.
//  On reordonne les faces de sorte a placer les faces "non standard"
//  au debut de la liste des faces. Les faces non standard sont celles
//  dont les volumes de controles sont modifies par les conditions aux
//  limites.
void Zone_EF::reordonner(Faces& les_faces)
{
  if (Process::je_suis_maitre())
    Cerr << "Zone_EF::reordonner les_faces " << finl;

  // Construction de rang_elem_non_std_ :
  //  C'est un vecteur indexe par les elements du domaine.
  //  size() = nb_elem()
  //  size_tot() = nb_elem_tot()
  //  Valeurs dans le tableau :
  //   rang_elem_non_std_[i] = -1 si l'element i est standard,
  //  sinon
  //   rang_elem_non_std_[i] = j, ou j est l'indice de l'element dans
  //   les tableaux indexes par les elements non standards (par exemple
  //   le tableau Zone_Cl_EF::type_elem_Cl_).
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

void Zone_EF::typer_elem(Zone& zone_geom)
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

void Zone_EF::discretiser()
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


  if (sub_type(Segment_EF,type_elem_.valeur()))
    {
      if (!sub_type(Segment,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          exit();
        }
    }
  else if (sub_type(Tri_EF,type_elem_.valeur()))
    {
      if (!sub_type(Triangle,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " <<
               elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Triangle est compatible avec la discretisation EF en dimension 2" << finl;
          Cerr << " Il faut trianguler le domaine lorsqu'on utilise le mailleur interne" ;
          Cerr << " en utilisant l'instruction: Trianguler nom_dom" << finl;
          exit();
        }
    }
  else if (sub_type(Tetra_EF,type_elem_.valeur()))
    {
      if (!sub_type(Tetraedre,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " <<
               elem_geom.que_suis_je() << " est incorrect" << finl;
          Cerr << " Seul le type Tetraedre est compatible avec la discretisation EF en dimension 3" << finl;
          Cerr << " Il faut tetraedriser le domaine lorsqu'on utilise le mailleur interne";
          Cerr << " en utilisant l'instruction: Tetraedriser nom_dom" << finl;
          exit();
        }
    }

  else if (sub_type(Quadri_EF,type_elem_.valeur()))
    {

      if (!sub_type(Quadrangle_VEF,elem_geom))
        {
          Cerr << " Le type de l'element geometrique " << elem_geom.que_suis_je() << " est incorrect" << finl;
          exit();
        }
    }
  else if (sub_type(Hexa_EF,type_elem_.valeur()))
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

  Zone_VF::calculer_porosites();
  //calculer_volumes_entrelaces();

  zone().domaine().creer_tableau_sommets(volumes_sommets_thilde_);
}

void Zone_EF::calculer_Bij(DoubleTab& bij)
{
  int nbsom_face=nb_som_face();
  int nbelem=nb_elem_tot();
  int nbsom_elem=zone().nb_som_elem();
  const IntTab& elemfaces=elem_faces_;
  int nbface_elem=zone().nb_faces_elem();
  bij.resize(nbelem,nbsom_elem,dimension);
  const IntTab& les_elems=zone().les_elems() ;
  if (nbsom_elem!=8)
    {
      for (int elem=0; elem<nbelem; elem++)
        for (int i=0; i<nbsom_elem; i++)
          {
            // on cherche les faces contribuantes ,ce n'est pas optimal
            for (int f=0; f<nbface_elem; f++)
              {
                int face=elemfaces(elem,f);
                for (int s=0; s<nbsom_face; s++)
                  if (face_sommets_(face,s)==les_elems(elem,i))

                    // on cherche les faces contribuantes ,ce n'est pas optimal
                    for (int j=0; j<dimension; j++)
                      {
                        bij(elem,i,j)+=face_normales(face,j)*oriente_normale(face,elem);
                      }
              }
          }
      bij/=nbsom_face;
      Bij_thilde_=bij;
      {
        //int nbelem=nb_elem_tot();
        //int nbsom_elem=zone().nb_som_elem();
        const IntTab& elems=zone().les_elems() ;
        //Cerr<<min(porosite_elem_)<<finl;abort();
        for (int elem=0; elem<nbelem; elem++)
          for (int i=0; i<nbsom_elem; i++)
            {
              int som_glob=elems(elem,i);;
              for (int j=0; j<dimension; j++)
                {
                  Bij_thilde_(elem,i,j)=bij(elem,i,j)*porosite_sommets_(som_glob);
                }
            }

        // essai
        if(0)
          {
            EFichier bthilde("Bthilde");
            for (int elem=0; elem<nbelem; elem++)
              for (int i=0; i<nbsom_elem; i++)
                for (int j=0; j<dimension; j++)
                  {
                    int ii,jj,ele;
                    //Cout<< elem<<" "<<i<<" "<<j<<" "<<Bij_thilde_(elem,i,j)<<" uuu ";
                    int i2=i;
                    if (i2==2) i2=3;
                    if (i2==3) i2=2;
                    if (i2==7) i2=6;
                    if (i2==6) i2=7;
                    Cout<< elem<<" "<<i<<" "<<j<<" "<<Bij_thilde_(elem,i2,j)<<" uuu ";
                    bthilde >> ele>> ii >> jj >> Bij_thilde_(elem,i2,j);
                    Cout<< Bij_thilde_(elem,i2,j)<<finl;
                    assert(ele==elem+1);
                    assert(ii==i+1);
                    assert(jj==j+1);
                  }
          }
      }
    }
  else
    {
      volumes_sommets_thilde_=0;

      Bij_thilde_.resize(nbelem,nbsom_elem,dimension);
      ArrOfInt filter(8) ;


      filter[0] = 0 ;
      filter[1] = 2 ;
      filter[2] = 3 ;
      filter[3] = 1 ;
      filter[4] = 4 ;
      filter[5] = 6 ;
      filter[6] = 7 ;
      filter[7] = 5 ;
      interface_INITGAUSS init_gauss;
      int npgau=27;
      // npgau=1;
      int nbnn=nbsom_elem;
      DoubleTab xgau(npgau,3),frgau(npgau,nbnn),dfrgau(npgau,nbnn,(int)3),poigau(npgau);
      DoubleVect volumes_sommets_(volumes_sommets_thilde_);
      int dim=3;
      //      init_gauss.compute(&dim,&nbnn,&npgau,xgau.addr(),frgau.addr(),dfrgau.addr(),poigau.addr());
      init_gauss.Compute(dim,nbnn,npgau,xgau,frgau,dfrgau,poigau);
      ArrOfInt num(nbnn);
      interface_CALCULBIJ CALCULBIJ;
      DoubleTab xl(dimension,nbnn),bijl(dimension,nbnn),poro;
      DoubleTab detj(npgau),ajm1(npgau*9),aj(npgau*9),df(npgau*nbnn*3),iphi(nbnn);
      int ip=0;
      const DoubleTab& coord=zone().domaine().coord_sommets();
      for (int elem=0; elem<nbelem; elem++)
        {
          for (int i=0; i<nbnn; i++)
            num(i)=les_elems(elem,filter(i));
          for (int i=0; i<nbnn; i++)
            for (int d=0; d<dimension; d++)
              {
                xl(d,i)=coord(num(i),d);
              }
          num+=1;
          ip=0;
          double vol;
          int nbsom_tot=volumes_sommets_.size_totale();
          CALCULBIJ.Compute(nbnn,nbsom_tot, xl, num, bijl,
                            porosite_sommets_, ip,
                            npgau, xgau, frgau, dfrgau,
                            poigau,detj, ajm1, aj, df,vol,volumes_sommets_,iphi);
          if (!est_egal(volumes_(elem),vol,1e-6))
            {
              Cerr<<"Erreur volume "<<elem<< " vol: "<<volumes_(elem)<<" new "<<vol<<" delta "<<volumes_(elem)-vol;
              exit();
            }
          for (int i=0; i<nbnn; i++)
            {
              for (int d=0; d<dimension; d++)
                {
                  bij(elem,filter(i),d)=bijl(d,(i));
                  IPhi_(elem,filter(i))=iphi(i);
                }
            }
          ip=1;

          CALCULBIJ.Compute(nbnn, nbsom_tot, xl, num, bijl,
                            porosite_sommets_, ip,
                            npgau, xgau, frgau, dfrgau,
                            poigau,detj, ajm1, aj, df,volumes_thilde_(elem),volumes_sommets_thilde_,iphi);

          for (int i=0; i<nbnn; i++)
            {
              for (int d=0; d<dimension; d++)
                {
                  Bij_thilde_(elem,filter(i),d)=bijl(d,(i));
                  IPhi_thilde_(elem,filter(i))=iphi(i);
                }

            }
          //exit();
        }
    }
  if (0)
    {
      for (int elem=0; elem<nbelem; elem++)
        {
          for (int i=0; i<nbsom_elem; i++)
            {
              for (int d=0; d<dimension; d++)
                Cerr<<" BIJ "<<elem<<" "<< i<< " "<<d<<" "<<bij(elem,i,d)<<" "<<Bij_thilde_(elem,i,d)<<finl;
              Cerr<< " Iphi " << IPhi_(elem,i)<<" "<<IPhi_thilde_(elem,i)<<finl;
            }
          Cerr<<"vol elem "<<volumes_(elem)<< " "<<volumes_thilde_(elem)<<finl;
        }
      for (int elem=0; elem<volumes_sommets_thilde_.size_array(); elem++)
        Cerr<<elem<<" vol som "<< volumes_sommets_thilde_(elem)<<finl;
    }
  // verif
  Cerr<<"max/min/max_abs bij "<<mp_max_vect(bij)<< " "<<mp_min_vect(bij)<<" "<<mp_max_abs_vect(bij)<<finl;
  int err=0;
  for (int elem=0; elem<nbelem; elem++)
    for (int j=0; j<dimension; j++)
      {
        double tot=0;
        for (int i=0; i<nbsom_elem; i++)
          tot+=bij(elem,i,j);
        if (!est_egal(tot,0))
          {
            err++;
            Cerr<<" pb elem "<< elem<<" direction "<<j<<" : "<<tot<<finl;
          }
      }
  if (err) exit();

}
void Zone_EF::calculer_porosites_sommets()
{


  porosite_sommets_.resize(nb_som());
  porosite_sommets_=1;
}
void Zone_EF::calculer_h_carre()
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

void Zone_EF::calculer_volumes_entrelaces()
{
  //  Cerr << "les normales aux faces " << face_normales() << finl;
  // On calcule les volumes entrelaces;
  //  volumes_entrelaces_.resize(nb_faces());
  creer_tableau_faces(volumes_entrelaces_);
  int elem1,elem2;
  int nb_faces_elem=zone().nb_faces_elem();
  int num_face;
  for (num_face=0; num_face<premiere_face_int(); num_face++)
    {
      elem1 = face_voisins_(num_face,0);
      volumes_entrelaces_[num_face] = volumes_[elem1]/nb_faces_elem;
    }
  for (num_face=premiere_face_int(); num_face<nb_faces(); num_face++)
    {
      elem1 = face_voisins_(num_face,0);
      elem2 = face_voisins_(num_face,1);
      volumes_entrelaces_[num_face] = (volumes_[elem1] + volumes_[elem2])
                                      /
                                      nb_faces_elem;
    }
  volumes_entrelaces_.echange_espace_virtuel();
}
void Zone_EF::remplir_elem_faces()
{
  creer_faces_virtuelles_non_std();
}

void Zone_EF::modifier_pour_Cl(const Conds_lim& conds_lim)
{


  //if (volumes_sommets_thilde_.size()!=nb_som())
  if (Bij_.nb_dim()==1)
    {

      calculer_volumes_sommets();
      Cerr << "La Zone_EF a ete remplie avec succes" << finl;

      calculer_h_carre();
      // Calcul des porosites

      // les porosites sommets ne servent pas
      //calculer_porosites_sommets();
      calculer_Bij(Bij_);



    }
  Journal() << "Zone_EF::Modifier_pour_Cl" << finl;
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
void Zone_EF::creer_faces_virtuelles_non_std()

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

