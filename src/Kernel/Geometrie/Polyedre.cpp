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
// File:        Polyedre.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Polyedre.h>
#include <Linear_algebra_tools_impl.h>
#include <Domaine.h>
#include <IntList.h>
#include <algorithm>

using std::swap;

Implemente_instanciable_sans_constructeur(Polyedre,"Polyedre",Poly_geom_base);

Polyedre::Polyedre(): FacesIndex_(1),PolyhedronIndex_(1)
{
  PolyhedronIndex_[0]=0;
  FacesIndex_[0]=0;
  nb_som_elem_max_=-1;
  nb_face_elem_max_=0;
  nb_som_face_max_=0;
}

// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Polyedre::printOn(Sortie& s ) const
{
  s<< Nodes_        <<finl;
  s<< FacesIndex_     <<finl;
  s<< PolyhedronIndex_ <<finl;
  s<< nb_som_elem_max_ <<finl;
  s<< nb_face_elem_max_ <<finl;
  s<< nb_som_face_max_ <<finl;;
  WARN;
  return s;
}


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Polyedre::readOn(Entree& s )
{
  s>> Nodes_;
  s>>FacesIndex_;
  s>>PolyhedronIndex_;
  s>>nb_som_elem_max_;
  s>>nb_face_elem_max_;
  s>>nb_som_face_max_;;
  return s;
}

void Polyedre::calculer_centres_gravite(DoubleTab& xp) const
{

  const Zone& zone=ma_zone.valeur();
  const IntTab& elem=zone.les_elems();
  const DoubleTab& coord=zone.domaine().coord_sommets();


  int nb_elem;
  if(xp.dimension(0)==0)
    {
      nb_elem = ma_zone->nb_elem_tot();
      xp.resize(nb_elem,dimension);
    }
  else
    nb_elem=xp.dimension(0);


  for (int num_poly=0; num_poly<nb_elem; num_poly++)
    {
      double volume=0;
      Vecteur3 xg(0,0,0);
      int nb_som_max=elem.dimension(1);
      int nb_som_reel;
      for (nb_som_reel=0; nb_som_reel<nb_som_max; nb_som_reel++)
        {
          int n=elem(num_poly,nb_som_reel);
          if (n<0)
            break;
          Vecteur3 S(coord(n,0),coord(n,1),coord(n,2));
          xg+=S;
        }
      xg*=1./nb_som_reel;
      Vecteur3 moinsS0(xg);
      moinsS0*=-1;

      Vecteur3 vraixg(0,0,0);

      for (int f=PolyhedronIndex_(num_poly); f<PolyhedronIndex_(num_poly+1); f++)
        {
          int somm_loc3=Nodes_(FacesIndex_(f+1)-1);
          int n3=elem(num_poly,somm_loc3);
          Vecteur3 S3(coord(n3,0),coord(n3,1),coord(n3,2));
          Vecteur3 S3sa(S3);
          S3+=  moinsS0;
          for (int s=FacesIndex_(f); s<FacesIndex_(f+1)-2; s++)
            {
              int somm_loc1=Nodes_(s);
              int n1=elem(num_poly,somm_loc1);
              int somm_loc2=Nodes_(s+1);
              int n2=elem(num_poly,somm_loc2);
              Vecteur3 S1(coord(n1,0),coord(n1,1),coord(n1,2));
              Vecteur3 S2(coord(n2,0),coord(n2,1),coord(n2,2));

              Vecteur3 xgl(xg);
              xgl+=S3sa;
              xgl+=S1;
              xgl+=S2;
              xgl*=0.25;
              S1+=  moinsS0;
              S2+=  moinsS0;
              double vol_l= dabs(
                              S1[0] * ( S2[1] * S3[2] - S3[1] * S2[2] )
                              + S2[0] * ( S3[1] * S1[2] - S1[1] * S3[2] )
                              + S3[0] * ( S1[1] * S2[2] - S2[1] * S1[2] ) );
              volume+=vol_l;
              xgl*=vol_l;
              vraixg+=xgl;
            }
        }
      vraixg*=1./volume;
      xp(num_poly,0)=vraixg[0];
      xp(num_poly,1)=vraixg[1];
      xp(num_poly,2)=vraixg[2];

      // Vecteur3 test= xg -vraixg;
      // Cerr<<num_poly<< "iiii "<<test[0]<< " "<<test[1]<<" "<<test[2]<<finl;
    }

  // Cerr<<" xp "<<xp << finl;
  return ;


  // on a change la methode dans Polygone mais pas dans Polyedre ?
  WARN;
  return Elem_geom_base::calculer_centres_gravite(xp);
}

void Polyedre::calculer_un_centre_gravite(const int num_elem,DoubleVect& xp) const
{
  const IntTab& les_Polys = ma_zone->les_elems();
  const Domaine& le_domaine = ma_zone->domaine();

  xp.resize(dimension);
  int nb_som_reel=nb_som();
  while (les_Polys(num_elem,nb_som_reel-1)==-1)  nb_som_reel--;
  for(int s=0; s<nb_som_reel; s++)
    {
      int num_som = les_Polys(num_elem,s);
      for(int i=0; i<dimension; i++)
        xp(i) += le_domaine.coord(num_som,i)/nb_som_reel;
    }



  WARN;
}
int Polyedre::get_nb_som_elem_max() const
{
  if (nb_som_elem_max_>-1)
    return nb_som_elem_max_ ;
  else
    return ma_zone.valeur().les_elems().dimension(1);
}

// Description:
//    Renvoie le nom LML d'un polyedre = "POLYEDRE_"+nb_som_max.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: toujours egal a "PRISM6"
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Polyedre::nom_lml() const
{
  static Nom nom;
  nom="POLYEDRE_";
  Nom n(get_nb_som_elem_max());
  nom+=n;
  return nom;
}


// Description:
//    NE FAIT RIEN: A CODER, renvoie toujours 0.
//    Renvoie 1 si l'element "element" de la zone associee a
//              l'element geometrique contient le point
//              de coordonnees specifiees par le parametre "pos".
//    Renvoie 0 sinon.
// Precondition:
// Parametre: DoubleVect& pos
//    Signification: coordonnees du point que l'on
//                   cherche a localiser
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int element
//    Signification: le numero de l'element de la zone
//                   dans lequel on cherche le point.
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si le point de coordonnees specifiees
//                   appartient a l'element "element"
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Polyedre::contient(const ArrOfDouble& pos, int num_poly ) const
{

  // on regarde si le point P est du meme cote que xg pour chaque face .
  const Zone& zone=ma_zone.valeur();
  const IntTab& elem=zone.les_elems();
  const DoubleTab& coord=zone.domaine().coord_sommets();
  Vecteur3 P(pos(0),pos(1),pos(2));
  Vecteur3 xg(0,0,0);
  int nb_som_max=elem.dimension(1);
  int nb_som_reel;
  for (nb_som_reel=0; nb_som_reel<nb_som_max; nb_som_reel++)
    {
      int n=elem(num_poly,nb_som_reel);
      if (n<0)
        break;
      Vecteur3 S(coord(n,0),coord(n,1),coord(n,2));
      xg+=S;
    }
  xg*=1./nb_som_reel;

  for (int f=PolyhedronIndex_(num_poly); f<PolyhedronIndex_(num_poly+1); f++)
    {
      Vecteur3 n(0,0,0);
      int somm_loc3=Nodes_(FacesIndex_(f+1)-1);
      int n3=elem(num_poly,somm_loc3);
      Vecteur3 moinsS3(-coord(n3,0),-coord(n3,1),-coord(n3,2));

      for (int s=FacesIndex_(f); s<FacesIndex_(f+1)-2; s++)
        {
          int somm_loc1=Nodes_(s);
          int n1=elem(num_poly,somm_loc1);
          int somm_loc2=Nodes_(s+1);
          int n2=elem(num_poly,somm_loc2);
          Vecteur3 S1(coord(n1,0),coord(n1,1),coord(n1,2));
          Vecteur3 S2(coord(n2,0),coord(n2,1),coord(n2,2));
          S1+=moinsS3;
          S2+=moinsS3;
          Vecteur3 nTr;
          Vecteur3::produit_vectoriel(S1,S2,nTr);
          n+=nTr;

        }




      Vecteur3 S3G(xg);
      S3G+=moinsS3;
      Vecteur3 S3P(P);
      S3P+=moinsS3;
      double prod_scal1=Vecteur3::produit_scalaire(n,S3G);
      double prod_scal2=Vecteur3::produit_scalaire(n,S3P);
      if (prod_scal1*prod_scal2<-Objet_U::precision_geom*prod_scal1*prod_scal1)
        return 0;
    }

  return 1;
}


// Description:
//    NE FAIT RIEN: A CODER, renvoie toujours 0
//    Renvoie 1 si les sommets specifies par le parametre "pos"
//    sont les sommets de l'element "element" de la zone associee a
//    l'element geometrique.
// Precondition:
// Parametre: IntVect& pos
//    Signification: les numeros des sommets a comparer
//                   avec ceux de l'elements "element"
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int element
//    Signification: le numero de l'element de la zone
//                   dont on veut comparer les sommets
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si les sommets passes en parametre
//                   sont ceux de l'element specifie, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Polyedre::contient(const ArrOfInt& pos, int num_poly ) const
{
  BLOQUE;
  return 0;
}


// Description:
//    NE FAIT RIEN: A CODER
//    Calcule les volumes des elements de la zone associee.
// Precondition:
// Parametre: DoubleVect& volumes
//    Signification: le vecteur contenant les valeurs  des
//                   des volumes des elements de la zone
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Polyedre::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=ma_zone.valeur();
  const IntTab& elem=zone.les_elems();
  const DoubleTab& coord=zone.domaine().coord_sommets();

  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      double volume=0;
      Vecteur3 xg(0,0,0);
      int nb_som_max=elem.dimension(1);
      int nb_som_reel;
      for (nb_som_reel=0; nb_som_reel<nb_som_max; nb_som_reel++)
        {
          int n=elem(num_poly,nb_som_reel);
          if (n<0)
            break;
          Vecteur3 S(coord(n,0),coord(n,1),coord(n,2));
          xg+=S;
        }
      xg*=1./nb_som_reel;
      Vecteur3 moinsS0(xg);
      moinsS0*=-1;
      for (int f=PolyhedronIndex_(num_poly); f<PolyhedronIndex_(num_poly+1); f++)
        {
          int somm_loc3=Nodes_(FacesIndex_(f+1)-1);
          int n3=elem(num_poly,somm_loc3);
          Vecteur3 S3(coord(n3,0),coord(n3,1),coord(n3,2));
          S3+=  moinsS0;
          for (int s=FacesIndex_(f); s<FacesIndex_(f+1)-2; s++)
            {
              int somm_loc1=Nodes_(s);
              int n1=elem(num_poly,somm_loc1);
              int somm_loc2=Nodes_(s+1);
              int n2=elem(num_poly,somm_loc2);
              Vecteur3 S1(coord(n1,0),coord(n1,1),coord(n1,2));
              Vecteur3 S2(coord(n2,0),coord(n2,1),coord(n2,2));
              S1+=  moinsS0;
              S2+=  moinsS0;
              volume += dabs(
                          S1[0] * ( S2[1] * S3[2] - S3[1] * S2[2] )
                          + S2[0] * ( S3[1] * S1[2] - S1[1] * S3[2] )
                          + S3[0] * ( S1[1] * S2[2] - S2[1] * S1[2] ) );

            }
        }
      volumes(num_poly)=volume/6.;
    }
//  Cerr<<" volumes "<<volumes << finl;
  return ;
}
int Polyedre::nb_type_face() const
{
  return 1;

}
// Description: remplit le tableau faces_som_local(i,j) qui donne pour
//  0 <= i < nb_faces()  et  0 <= j < nb_som_face(i) le numero local du sommet
//  sur l'element.
//  On a  0 <= faces_sommets_locaux(i,j) < nb_som()
// Si toutes les faces de l'element n'ont pas le meme nombre de sommets, le nombre
// de colonnes du tableau est le plus grand nombre de sommets, et les cases inutilisees
// du tableau sont mises a -1
// On renvoie 1 si toutes les faces ont le meme nombre d'elements, 0 sinon.
int Polyedre::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  return 0;
}
int Polyedre::get_tab_faces_sommets_locaux(IntTab& faces_som_local,int ele) const
{
  faces_som_local.resize(nb_face_elem_max_,nb_som_face_max_);
  faces_som_local=-1;
  if (ele == 0 && PolyhedronIndex_.size_array() == 1) return 1; //pas d'elements!
  // on cherche les faces de l'elt
  int fl=0;
  for (int f=PolyhedronIndex_(ele); f<PolyhedronIndex_(ele+1); f++)
    {
      int sl=0;
      for (int s=FacesIndex_(f); s<FacesIndex_(f+1); s++)
        {
          int somm_loc=Nodes_(s);
          faces_som_local(fl,sl)=somm_loc;
          sl++;
        }
      fl++;
    }

  return 1;

}

// Desctiption : a partir des tableaux d'indirection FacesIndex PolyhedronIndex et Nodes
// on calcul les elems, nb_som_face_max_, nb_face_elem_max_ nb_som_elem_max_
// ainsi que Nodes local...
void Polyedre::affecte_connectivite_numero_global(const ArrOfInt& Nodes,const ArrOfInt& FacesIndex,const ArrOfInt& PolyhedronIndex,IntTab& les_elems)
{
  nb_som_elem_max_=0;
  // detremination de nbsom_max
  IntList prov;
  nb_face_elem_max_=0;
  nb_som_face_max_=0;
  int nelem=PolyhedronIndex.size_array()-1;
  for (int ele=0; ele<nelem; ele++)
    {
      prov.vide();
      int nbf=PolyhedronIndex(ele+1)-PolyhedronIndex(ele);
      if (nbf>nb_face_elem_max_) nb_face_elem_max_=nbf;
      for (int f=PolyhedronIndex(ele); f<PolyhedronIndex(ele+1); f++)
        {
          //Cerr<<" ici "<<ele << " " <<f <<" "<<FacesIndex(f+1)-FacesIndex(f)<<finl;
          int nbs=FacesIndex(f+1)-FacesIndex(f);
          if (nbs>nb_som_face_max_) nb_som_face_max_=nbs;
          for (int s=FacesIndex(f); s<FacesIndex(f+1); s++)
            {
              prov.add_if_not(Nodes(s));
            }
        }
      int nbsom=prov.size();
      if (nbsom>nb_som_elem_max_) nb_som_elem_max_=nbsom;
    }
  Cerr<<" Polyhedron information nb_som_elem_max "<< nb_som_elem_max_<<" nb_som_face_max "<<nb_som_face_max_<<" nb_face_elem_max "<<nb_face_elem_max_<<finl;
  les_elems.resize(nelem,nb_som_elem_max_);
  les_elems=-1;
  // on refait un tour pour determiiner les elems
  for (int ele=0; ele<nelem; ele++)
    {
      prov.vide();
      for (int f=PolyhedronIndex(ele); f<PolyhedronIndex(ele+1); f++)
        {
          for (int s=FacesIndex(f); s<FacesIndex(f+1); s++)
            {
              prov.add_if_not(Nodes(s));
            }
        }
      int nbsom=prov.size();
      // on trie prov dans l'ordre croissant
      // pas strictement necessaire mais permet de garder le meme tableau elem meme si on a effectue des permutation pour les faces
      int perm=1;
      while (perm)
        {
          perm=0;
          for (int i=0; i<nbsom-1; i++)
            if (prov[i]>prov[i+1])
              {
                perm=1;
                int sa=prov[i];
                prov[i]=prov[i+1];
                prov[i+1]=sa;
              }
        }
      for (int s=0; s<nbsom; s++) les_elems(ele,s)=prov[s];
    }
  FacesIndex_=FacesIndex;
  PolyhedronIndex_=PolyhedronIndex;
  // determination de Nodes_...
  Nodes_=Nodes;
  {
    Nodes_=-2;
    for (int ele=0; ele<nelem; ele++)
      {
        for (int f=PolyhedronIndex(ele); f<PolyhedronIndex(ele+1); f++)
          {
            for (int s=FacesIndex(f); s<FacesIndex(f+1); s++)
              {
                int somm_glob=Nodes(s);
                for (int sl=0; sl<nb_som_elem_max_; sl++)
                  if (les_elems(ele,sl)==somm_glob)
                    {
                      Nodes_(s)=sl;
                      break;
                    }
              }
          }
      }
  }
  assert(min_array(Nodes_)>-1);
}

void Polyedre::remplir_Nodes_glob(ArrOfInt& Nodes_glob,const IntTab& les_elems) const
{
  Nodes_glob=Nodes_;
  int nelem=les_elems.dimension_tot(0);
  for (int ele=0; ele<nelem; ele++)
    {
      for (int f=PolyhedronIndex_(ele); f<PolyhedronIndex_(ele+1); f++)
        {
          for (int s=FacesIndex_(f); s<FacesIndex_(f+1); s++)
            {
              int somm_loc=Nodes_(s);
              Nodes_glob(s)=les_elems(ele,somm_loc);
            }
        }
    }
}

// Description:
//    Reordonne les sommets du Polyedre.
//    NE FAIT RIEN: A CODER
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Polyedre::reordonner()
{
}
// Description on va ajouter les elements de type new_elem aux elements deja presents dans les_elems et dans new_elems
void Polyedre::ajouter_elements(const Elem_geom_base& type_elem, const IntTab& new_elems,IntTab& les_elems)
{
  // On a joute les new_elems a les_elems
  int nb_old_elem=les_elems.dimension(0);
  int nb_new_elem=new_elems.dimension(0);
  int nb_som_old_elem=les_elems.dimension(1);
  int nb_som_new_elem=new_elems.dimension(1);
  nb_som_elem_max_ = max(nb_som_old_elem,nb_som_new_elem);
  les_elems.set_smart_resize(1);
  les_elems.resize(nb_old_elem+nb_new_elem,nb_som_elem_max_, ArrOfInt::COPY_NOINIT);
  for (int el=0; el<nb_new_elem; el++)
    {
      for (int s=0; s<nb_som_new_elem; s++)
        les_elems(nb_old_elem+el,s)=new_elems(el,s);

      for (int s=nb_som_new_elem; s<nb_som_old_elem; s++)
        les_elems(nb_old_elem+el,s)=-1;
    }
  // on ajoute les faces pour cela on recupere le tableau de creation des faces
  IntTab faces_som_local;
  type_elem.get_tab_faces_sommets_locaux(faces_som_local);
  int nb_face_new_elem=faces_som_local.dimension(0);
  int nb_som_face_new_elem=faces_som_local.dimension(1);
  nb_face_elem_max_=max(nb_face_elem_max_,nb_face_new_elem);
  nb_som_face_max_=max(nb_som_face_max_,nb_som_face_new_elem);

  PolyhedronIndex_.set_smart_resize(1);
  PolyhedronIndex_.resize_array(1+nb_old_elem+nb_new_elem);

  FacesIndex_.set_smart_resize(1);
  int old_faces_index= FacesIndex_.size_array();
  FacesIndex_.resize_array(old_faces_index+nb_new_elem*nb_face_new_elem);

  Nodes_.set_smart_resize(1);
  int old_nodes_index= Nodes_.size_array();
  Nodes_.resize_array(old_nodes_index+nb_new_elem*nb_face_new_elem*nb_som_face_new_elem);

  int new_s=0;
  for (int el=0; el<nb_new_elem; el++)
    {
      PolyhedronIndex_(nb_old_elem+el+1)=PolyhedronIndex_(nb_old_elem+el)+nb_face_new_elem;
      for (int f=0; f<nb_face_new_elem; f++)
        {
          int nb_som_face_this_elem=0;
          for (int s=0; s<nb_som_face_new_elem; s++)
            if (faces_som_local(f,s)!=-1)
              {
                Nodes_(old_nodes_index+new_s++)=faces_som_local(f,s);
                nb_som_face_this_elem++;
              }
          if (nb_som_face_this_elem==4)
            {
              // on inverse 3 et 4 en cas de quadrangle
              int last=old_nodes_index+new_s-1;
              swap(Nodes_(last),Nodes_(last-1));
            }
          FacesIndex_(old_faces_index+(el*nb_face_new_elem)+f)=
            FacesIndex_(old_faces_index+(el*nb_face_new_elem)+f-1)+nb_som_face_this_elem;

        }
    }
  Nodes_.resize_array(old_nodes_index+new_s);
}

void Polyedre::build_reduced(Elem_geom& type_elem, const ArrOfInt& elems_sous_part) const
{
  type_elem.typer("Polyedre");
  Polyedre& reduced = ref_cast(Polyedre, type_elem.valeur());
  reduced.nb_som_elem_max_  = nb_som_elem_max_;
  reduced.nb_face_elem_max_ = nb_face_elem_max_;
  reduced.nb_som_face_max_  = nb_som_face_max_;
  ArrOfInt& Pi = reduced.PolyhedronIndex_, &Fi = reduced.FacesIndex_, &N = reduced.Nodes_;
  Pi.set_smart_resize(1), Fi.set_smart_resize(1), N.set_smart_resize(1);

  for (int i = 0; i < elems_sous_part.size_array(); i++)
    {
      int e = elems_sous_part(i);
      for (int f = PolyhedronIndex_(e); f < PolyhedronIndex_(e + 1); f++)
        {
          for (int s = FacesIndex_(f); s < FacesIndex_(f + 1); s++) N.append_array(Nodes_(s));
          Fi.append_array(N.size_array());
        }
      Pi.append_array(Fi.size_array() - 1);
    }
}

void Polyedre::compute_virtual_index()
{
  // Methode brutal mais il faut bien commencer ....

  IntTab faces_som(0,nb_face_elem_max_,nb_som_face_max_);
  ma_zone.valeur().creer_tableau_elements(faces_som);

  IntTab faces_som_local;
  int nb_elem=ma_zone.valeur().nb_elem();
  int nb_elem_tot=ma_zone.valeur().nb_elem_tot();
  for (int ele=0; ele<nb_elem; ele++)
    {
      get_tab_faces_sommets_locaux(faces_som_local,ele);
      for (int k=0; k<nb_face_elem_max_; k++)
        for (int l=0; l<nb_som_face_max_; l++)
          faces_som(ele,k,l)=faces_som_local(k,l);
    }
  faces_som.echange_espace_virtuel();
  int nbs=0;

  PolyhedronIndex_.resize(nb_elem_tot+1);
  //Cerr<<"uuu "<< PolyhedronIndex_<<finl;

  for (int ele=nb_elem; ele<nb_elem_tot; ele++)
    {
      int nbf=0;
      for (int k=0; k<nb_face_elem_max_; k++)
        {
          if (faces_som(ele,k,0)!=-1)
            nbf++;
          for (int l=0; l<nb_som_face_max_; l++)
            {
              if (faces_som(ele,k,l)!=-1)
                nbs++;
              // Cerr <<" INFO " << ele<< " k" <<k << " l "<< l<< " iiii "<<faces_som(ele,k,l)<<finl;
            }
        }
      PolyhedronIndex_(ele+1)=PolyhedronIndex_(ele)+nbf;
    }
  FacesIndex_.resize(PolyhedronIndex_(nb_elem_tot)+1);
  int nbs_old=Nodes_.size_array();
  Nodes_.resize(nbs_old+nbs);
  int nbft=PolyhedronIndex_(nb_elem);
  nbs=nbs_old;


  for (int ele=nb_elem; ele<nb_elem_tot; ele++)
    {

      for (int k=0; k<nb_face_elem_max_; k++)
        {
          for (int l=0; l<nb_som_face_max_; l++)
            {
              if (faces_som(ele,k,l)!=-1)
                {
                  Nodes_(nbs)=faces_som(ele,k,l);
                  nbs++;
                }
            }
          if (faces_som(ele,k,0)!=-1)
            {
              FacesIndex_(nbft+1)=nbs;
              nbft++;
            }

        }

    }

  for (int ele=nb_elem; ele<nb_elem_tot; ele++)
    {
      get_tab_faces_sommets_locaux(faces_som_local,ele);
      for (int k=0; k<nb_face_elem_max_; k++)
        for (int l=0; l<nb_som_face_max_; l++)
          {

            int ind1=faces_som(ele,k,l);
            int ind2=faces_som_local(k,l);
            if (ind1!=ind2)
              {
                Cerr << "PPPPB "<< ele<< " k " <<k << " l "<< l<< " iiii "<<ind1<<" "<<ind2<<finl;
                abort();
              }
          }
    }
  //  BLOQUE;

}


int Polyedre::get_somme_nb_faces_elem() const
{

  int titi= PolyhedronIndex_[ma_zone.valeur().nb_elem()];
  return titi;
}
