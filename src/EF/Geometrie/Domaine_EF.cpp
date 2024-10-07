/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <interface_INITGAUSS.h>
#include <interface_CALCULBIJ.h>
#include <Domaine_Cl_dis_base.h>
#include <Quadrangle_VEF.h>
#include <Domaine_Cl_EF.h>
#include <Equation_base.h>
#include <Hexaedre_VEF.h>
#include <Milieu_base.h>
#include <Domaine_EF.h>
#include <Periodique.h>
#include <Segment_EF.h>
#include <Rectangle.h>
#include <Tetraedre.h>
#include <Quadri_EF.h>
#include <Hexaedre.h>
#include <Triangle.h>
#include <Tetra_EF.h>
#include <EFichier.h>
#include <Segment.h>
#include <Hexa_EF.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Tri_EF.h>

Implemente_instanciable(Domaine_EF, "Domaine_EF", Domaine_VF);

Sortie& Domaine_EF::ecrit(Sortie& os) const
{
  Domaine_VF::printOn(os);
  os << "____ h_carre "<<finl;
  os << h_carre << finl;
  os << "____ type_elem_ "<<finl;
  os << type_elem_.valeur() << finl;
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

Sortie& Domaine_EF::printOn(Sortie& os) const
{
  Domaine_VF::printOn(os);

  os << h_carre << finl;
  os << type_elem_.valeur() << finl;
  os << nb_elem_std_ << finl;
  os << volumes_entrelaces_ << finl;
  os << face_normales_ << finl;
  os << xp_ << finl;
  os << xv_ << finl;
  os << nb_faces_std_ << finl;
  os << rang_elem_non_std_ << finl;
  return os;
}

Entree& Domaine_EF::readOn(Entree& is)
{
  Domaine_VF::readOn(is);
  is >> h_carre;

  /* read type_elem */
  {
    Nom type;
    is >> type;
    if (type == "Tri_EF")
      type_elem_ = Tri_EF();
    else if (type == "Tetra_EF")
      type_elem_ = Tetra_EF();
    else if (type == "Quadri_EF")
      type_elem_ = Quadri_EF();
    else if (type == "Hexa_EF")
      type_elem_ = Hexa_EF();
    else
      {
        Cerr << type << " n'est pas un Elem_EF !" << finl;
        Process::exit();
      }
  }

  is >> nb_elem_std_ ;
  is >> volumes_entrelaces_ ;
  is >> face_normales_ ;
  is >> xp_;
  is >> xv_;
  is >> nb_faces_std_ ;
  is >> rang_elem_non_std_ ;
  return is;
}

void Domaine_EF::calculer_IPhi(const Domaine_Cl_dis_base& zcl)
{
  int nbelem=domaine().nb_elem();
  int nb_som_elem=domaine().nb_som_elem();
  IPhi_.resize(nbelem,nb_som_elem);
  domaine().creer_tableau_elements(IPhi_);
  //  Scatter::creer_tableau_distribue(domaine().domaine(), JOINT_ITEM::ELEMENT, IPhi_);
  IPhi_thilde_=IPhi_;
  double rap=1./domaine().nb_som_elem();

  // calcul de IPhi_ version valable poour les carres...
  for (int elem=0; elem<nbelem; elem++)
    {
      double vol=volumes(elem)*rap;
      for (int s=0; s<nb_som_elem; s++)
        {
          IPhi_(elem,s)=vol;
          IPhi_thilde_(elem,s)=vol*zcl.equation().milieu().porosite_elem(elem);
        }
    }
  IPhi_.echange_espace_virtuel();
  IPhi_thilde_.echange_espace_virtuel();
}

void Domaine_EF::calculer_volumes_sommets(const Domaine_Cl_dis_base& zcl)
{
  //  volumes_sommets_thilde_.resize(nb_som());
  //Scatter::creer_tableau_distribue(domaine().domaine(), JOINT_ITEM::SOMMET, volumes_sommets_thilde_);
//  domaine().creer_tableau_sommets(volumes_sommets_thilde_);

  calculer_IPhi(zcl);

  // calcul de volumes_sommets_thilde
  const IntTab& les_elems=domaine().les_elems() ;

  DoubleVect volumes_som_prov(volumes_sommets_thilde_);
  for (int elem=0; elem<domaine().nb_elem_tot(); elem++)
    {

      for (int s=0; s<domaine().nb_som_elem(); s++)
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
      for (int elem=0; elem<domaine().nb_elem_tot(); elem++)
        {

          for (int s=0; s<domaine().nb_som_elem(); s++)
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
      for (int elem=0; elem<domaine().nb_elem_tot(); elem++)
        for (int s=0; s<domaine().nb_som_elem(); s++)
          IPhi_thilde_(elem,s)= IPhi_thilde_(elem,s)*porosite_sommets_(les_elems(elem,s));
    }
#endif

  volumes_thilde_=volumes_;
  for (int elem=0; elem<domaine().nb_elem_tot(); elem++)
    {
      volumes_thilde_(elem)=volumes_(elem)*zcl.equation().milieu().porosite_elem(elem);
    }
}

void Domaine_EF::swap(int fac1, int fac2, int nb_som_faces )
{

}

/*! @brief Methode appelee par Domaine_VF::discretiser apres la creation des faces reelles.
 *
 *   On reordonne les faces de sorte a placer les faces "non standard"
 *   au debut de la liste des faces. Les faces non standard sont celles
 *   dont les volumes de controles sont modifies par les conditions aux
 *   limites.
 *
 */
void Domaine_EF::reordonner(Faces& les_faces)
{
  Cerr << "Domaine_EF::reordonner les_faces " << finl;

  // Construction de rang_elem_non_std_ :
  //  C'est un vecteur indexe par les elements du domaine.
  //  size() = nb_elem()
  //  size_tot() = nb_elem_tot()
  //  Valeurs dans le tableau :
  //   rang_elem_non_std_[i] = -1 si l'element i est standard,
  //  sinon
  //   rang_elem_non_std_[i] = j, ou j est l'indice de l'element dans
  //   les tableaux indexes par les elements non standards (par exemple
  //   le tableau Domaine_Cl_EF::type_elem_Cl_).
  // Un element est non standard s'il est voisin d'une face frontiere.
  {
    const Domaine& dom = domaine();
    const int nb_elements = nb_elem();
    const int nb_faces_front = domaine().nb_faces_frontiere();
    dom.creer_tableau_elements(rang_elem_non_std_);
    //    rang_elem_non_std_.resize(nb_elements);
    //    Scatter::creer_tableau_distribue(dom, JOINT_ITEM::ELEMENT, rang_elem_non_std_);
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

  renumeroter(les_faces);

}

void Domaine_EF::typer_elem(Domaine& domaine_geom)
{
  const Elem_geom_base& elem_geom = domaine_geom.type_elem().valeur();
  if (sub_type(Rectangle, elem_geom))
    domaine_geom.typer("Quadrangle");
  else if (sub_type(Hexaedre, elem_geom))
    domaine_geom.typer("Hexaedre_VEF");

  const Nom& type_elem_geom = domaine_geom.type_elem()->que_suis_je();
  Nom type;

  if (type_elem_geom == "Triangle")
    type = "Tri_EF";
  else if (type_elem_geom == "Tetraedre")
    type = "Tetra_EF";
  else if (type_elem_geom == "Quadrangle")
    type = "Quadri_EF";
  else if (type_elem_geom == "Hexaedre_VEF")
    type = "Hexa_EF";
  else if (type_elem_geom == "Segment")
    type = "Segment_EF";
  else if (type_elem_geom == "Segment_axi")
    type = "Segment_EF_axi";
  else if (type_elem_geom == "Point")
    type = "Point_EF";
  else
    {
      Cerr << "probleme de typage dans Domaine_EF::typer_elem => type geometrique : " << type_elem_geom << finl;
      Process::exit();
    }
  type_elem_.typer(type);
}

void Domaine_EF::verifie_compatibilite_domaine()
{
  if (domaine().axi1d())
    {
      Cerr << "*****************************************************************************" << finl;
      Cerr << " Error in " << que_suis_je() << " : the type of domain " << domaine().que_suis_je();
      Cerr << " is not compatible" << finl;
      Cerr << " with the discretisation EF. " << finl;
      Cerr << " Please use the discretization EF_axi or define a domain of type Domaine." << finl;
      Cerr << "*****************************************************************************" << finl;
      Process::exit();
    }
}

void Domaine_EF::discretiser()
{
  verifie_compatibilite_domaine();

  Domaine& domaine_geom=domaine();
  typer_elem(domaine_geom);
  Elem_geom_base& elem_geom = domaine_geom.type_elem().valeur();
  Domaine_VF::discretiser();

  // Correction du tableau facevoisins:
  //  A l'issue de Domaine_VF::discretiser(), les elements voisins 0 et 1 d'une
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
    // const Domaine & dom = domaine();
    //    Scatter::creer_tableau_distribue(dom, JOINT_ITEM::FACE, face_normales_);
    creer_tableau_faces(face_normales_);
    const IntTab& face_som = face_sommets();
    const IntTab& face_vois = face_voisins();
    const IntTab& elem_face = elem_faces();
    const int n_tot = nb_faces_tot();
    for (num_face = 0; num_face < n_tot; num_face++)
      {
        type_elem_->normale(num_face,
                            face_normales_,
                            face_som, face_vois, elem_face, domaine_geom);
      }
  }

  domaine().creer_tableau_sommets(volumes_sommets_thilde_);
}

void Domaine_EF::calculer_Bij_gen(DoubleTab& bij)
{
  const IntTab& les_elems=domaine().les_elems() ;
  int nbsom_face=nb_som_face();
  int nbelem=nb_elem_tot();
  int nbsom_elem=domaine().nb_som_elem();
  const IntTab& elemfaces=elem_faces_;
  int nbface_elem=domaine().nb_faces_elem();

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
}

void Domaine_EF::calculer_Bij(DoubleTab& bij)
{
  int nbsom_face=nb_som_face();
  int nbelem=nb_elem_tot();
  int nbsom_elem=domaine().nb_som_elem();
  bij.resize(nbelem,nbsom_elem,dimension);
  const IntTab& les_elems=domaine().les_elems() ;
  if (nbsom_elem!=8)
    {
      calculer_Bij_gen(bij);

      bij/=nbsom_face;
      Bij_thilde_=bij;
      {
        //int nbelem=nb_elem_tot();
        //int nbsom_elem=domaine().nb_som_elem();
        const IntTab& elems=domaine().les_elems() ;
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
      DoubleTab xl(dimension,nbnn),bijl(dimension,nbnn);
      DoubleTab detj(npgau),ajm1(npgau*9),aj(npgau*9),df(npgau*nbnn*3),iphi(nbnn);
      int ip=0;
      const DoubleTab& coord=domaine().coord_sommets();
      for (int elem=0; elem<nbelem; elem++)
        {
          for (int i=0; i<nbnn; i++)
            num[i]=les_elems(elem,filter[i]);
          for (int i=0; i<nbnn; i++)
            for (int d=0; d<dimension; d++)
              {
                xl(d,i)=coord(num[i],d);
              }
          num+=1;
          ip=0;
          double vol;
          int nbsom_tot=volumes_sommets_.size_totale();
          CALCULBIJ.Compute(nbnn,nbsom_tot, xl, num, bijl,
                            porosite_sommets_, ip,
                            npgau, xgau, frgau, dfrgau,
                            poigau,detj, ajm1, aj, df,vol,volumes_sommets_,iphi);
          if (!est_egal(volumes_(elem),vol,1e-5))
            {
              Cerr<<"Erreur volume "<<elem<< " vol: "<<volumes_(elem)<<" new "<<vol<<" delta "<<volumes_(elem)-vol;
              exit();
            }
          for (int i=0; i<nbnn; i++)
            {
              for (int d=0; d<dimension; d++)
                {
                  bij(elem,filter[i],d)=bijl(d,(i));
                  IPhi_(elem,filter[i])=iphi(i);
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
                  Bij_thilde_(elem,filter[i],d)=bijl(d,(i));
                  IPhi_thilde_(elem,filter[i])=iphi(i);
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
        //Cerr<<"vol elem "<<volumes_(elem)<< " "<<volumes_thilde_(elem)<<finl;
      }
  if (err) exit();

}
void Domaine_EF::calculer_porosites_sommets()
{


  porosite_sommets_.resize(nb_som());
  porosite_sommets_=1;
}
void Domaine_EF::calculer_h_carre()
{
  // Calcul de h_carre
  h_carre = 1.e30;
  h_carre_.resize(nb_faces());
  // Calcul des surfaces
  const DoubleVect& surfaces=face_surfaces();
  const int nb_faces_elem=domaine().nb_faces_elem();
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

void Domaine_EF::calculer_volumes_entrelaces()
{
  //  Cerr << "les normales aux faces " << face_normales() << finl;
  // On calcule les volumes entrelaces;
  //  volumes_entrelaces_.resize(nb_faces());
  creer_tableau_faces(volumes_entrelaces_);
  int elem1,elem2;
  int nb_faces_elem=domaine().nb_faces_elem();
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

void Domaine_EF::modifier_pour_Cl(const Conds_lim& conds_lim)
{


  //if (volumes_sommets_thilde_.size()!=nb_som())
  if (Bij_.nb_dim()==1)
    {

      calculer_volumes_sommets(conds_lim[0]->domaine_Cl_dis());
      Cerr << "le Domaine_EF a ete rempli avec succes" << finl;

      calculer_h_carre();
      // Calcul des porosites

      // les porosites sommets ne servent pas
      //calculer_porosites_sommets();
      calculer_Bij(Bij_);



    }
  Journal() << "Domaine_EF::Modifier_pour_Cl" << finl;
  for (auto& itr : conds_lim)
    {
      //for cl
      const Cond_lim_base& cl = itr.valeur();
      if (sub_type(Periodique, cl))
        {
          //if Perio
          const Periodique& la_cl_period = ref_cast(Periodique,cl);
          int nb_faces_elem = domaine().nb_faces_elem();
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
  Domaine_VF::marquer_faces_double_contrib(conds_lim);
}

