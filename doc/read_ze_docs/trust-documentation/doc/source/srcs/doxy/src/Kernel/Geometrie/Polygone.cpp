/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <TRUSTList.h>
#include <Polygone.h>
#include <Triangle.h>
#include <Domaine.h>
#include <algorithm>
using std::swap;

Implemente_instanciable_sans_constructeur(Polygone,"Polygone",Poly_geom_base);

Polygone::Polygone(): FacesIndex_(1),PolygonIndex_(1)
{
  PolygonIndex_[0]=0;
  FacesIndex_[0]=0;
  nb_som_elem_max_=-1;
  nb_face_elem_max_=0;

}

/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
Sortie& Polygone::printOn(Sortie& s ) const
{
  s<< FacesIndex_     <<finl;
  s<< PolygonIndex_ <<finl;
  s<< nb_som_elem_max_ <<finl;
  s<< nb_face_elem_max_ <<finl;
  WARN;
  return s;
}
void Polygone::rebuild_index()
{

  const IntTab& les_elems = mon_dom.valeur().les_elems();
  int nb_elem=les_elems.dimension_tot(0);
  ArrOfInt   PolygonIndex_OK(nb_elem+1);
  PolygonIndex_OK[0]=0;
  for (int ele=0; ele<nb_elem; ele++)
    {
      int nbf=get_nb_som_elem_max();
      while (les_elems(ele,nbf-1)<0)
        nbf--;
      PolygonIndex_OK[ele+1]= PolygonIndex_OK[ele]+nbf;
    }
  ArrOfInt FacesIndex_OK(PolygonIndex_OK[nb_elem]);
  int f=0;
  for (int ele=0; ele<nb_elem; ele++)
    {
      for (int ss=0; ss<PolygonIndex_OK[ele+1]-PolygonIndex_OK[ele]; ss++)

        FacesIndex_OK[f++]= les_elems(ele,ss);
    }
  assert(f==PolygonIndex_OK[nb_elem]);

  FacesIndex_=FacesIndex_OK;
  PolygonIndex_=PolygonIndex_OK;
}

void Polygone::build_reduced(Elem_geom& type_elem, const ArrOfInt& elems_sous_part) const
{
  const IntTab& les_elems = mon_dom.valeur().les_elems();
  type_elem.typer("Polygone");
  Polygone& reduced = ref_cast(Polygone, type_elem.valeur());
  reduced.nb_som_elem_max_  = nb_som_elem_max_;
  reduced.nb_face_elem_max_ = nb_face_elem_max_;

  ArrOfInt& Pi = reduced.PolygonIndex_, &Fi = reduced.FacesIndex_;
  Pi.set_smart_resize(1), Fi.resize(0), Fi.set_smart_resize(1);

  for (int i = 0; i < elems_sous_part.size_array(); i++)
    {
      int e = elems_sous_part[i];
      for (int f = PolygonIndex_[e]; f < PolygonIndex_[e + 1]; f++)
        {
          Fi.append_array(les_elems(e, f - PolygonIndex_[e]));
        }
      Pi.append_array(Fi.size_array());
    }
}


void Polygone::compute_virtual_index()
{
  rebuild_index();
}


int Polygone::get_somme_nb_faces_elem() const
{
  return PolygonIndex_[mon_dom.valeur().nb_elem()];
}
/*! @brief NE FAIT RIEN
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree
 */
Entree& Polygone::readOn(Entree& s )
{
  s>>FacesIndex_;
  s>>PolygonIndex_;
  s>>nb_som_elem_max_;
  s>>nb_face_elem_max_;
  return s;
}

int Polygone::get_nb_som_elem_max() const
{
  if (nb_som_elem_max_>-1)
    return nb_som_elem_max_ ;
  else
    return mon_dom.valeur().les_elems().dimension(1);
}

/*! @brief Renvoie le nom LML d'un polyedre = "POLYEDRE_"+nb_som_max.
 *
 * @return (Nom&) toujours egal a "PRISM6"
 */
const Nom& Polygone::nom_lml() const
{
  static Nom nom;
  nom="POLYEDRE_";
  Nom n(2*get_nb_som_elem_max());
  if (dimension==3) nom="POLYGONE_";
  if (dimension==3) n=Nom(get_nb_som_elem_max());
  nom+=n;
  return nom;
}


// ToDo a mettre dans triangle
int contient_triangle(const ArrOfDouble& pos,int som0,int som1,int som2,const DoubleTab& coord)
{

  double prod,p0,p1,p2;

  // Il faut donc determiner le sens (trigo ou anti trigo) pour la numerotation :
  // Calcul de prod = 01 vectoriel 02 selon z
  // prod > 0 : sens trigo
  // prod < 0 : sens anti trigo
  prod = (coord(som1,0)-coord(som0,0))*(coord(som2,1)-coord(som0,1))
         - (coord(som1,1)-coord(som0,1))*(coord(som2,0)-coord(som0,0));
  double signe;
  if (prod >= 0)
    signe = 1;
  else
    signe = -1;
  // Calcul de p0 = 0M vectoriel 1M selon z
  p0 = (pos[0]-coord(som0,0))*(pos[1]-coord(som1,1))
       - (pos[1]-coord(som0,1))*(pos[0]-coord(som1,0));
  p0 *= signe;
  // Calcul de p1 = 1M vectoriel 2M selon z
  p1 = (pos[0]-coord(som1,0))*(pos[1]-coord(som2,1))
       - (pos[1]-coord(som1,1))*(pos[0]-coord(som2,0));
  p1 *= signe;
  // Calcul de p2 = 2M vectoriel 0M selon z
  p2 = (pos[0]-coord(som2,0))*(pos[1]-coord(som0,1))
       - (pos[1]-coord(som2,1))*(pos[0]-coord(som0,0));
  p2 *= signe;
  double epsilon=std::fabs(prod)*Objet_U::precision_geom;
  if ((p0>-epsilon) && (p1>-epsilon) && (p2>-epsilon))
    return 1;
  else
    return 0;
}

/*! @brief NE FAIT RIEN: A CODER, renvoie toujours 0.
 *
 * Renvoie 1 si l'element "element" du domaine associe a
 *               l'element geometrique contient le point
 *               de coordonnees specifiees par le parametre "pos".
 *     Renvoie 0 sinon.
 *
 * @param (DoubleVect& pos) coordonnees du point que l'on cherche a localiser
 * @param (int element) le numero de l'element du domaine dans lequel on cherche le point.
 * @return (int) 1 si le point de coordonnees specifiees appartient a l'element "element" 0 sinon
 */
int Polygone::contient(const ArrOfDouble& pos_r, int num_poly ) const
{
  const Domaine& domaine=mon_dom.valeur();
  const IntTab& elem=domaine.les_elems();
  const DoubleTab& coord=domaine.coord_sommets();
  //DoubleTab pos(3,dimension);
  // on decoupe le polygone en triangle ayany tous le sommet 0.

  int s0=elem(num_poly,0);
  for (int s=1; s<nb_som_elem_max_-1 ; s++)
    {
      int s1=elem(num_poly,s);
      int s2=elem(num_poly,s+1);
      if (s2<0)
        break;

      if (contient_triangle(pos_r,s0,s1,s2,coord))
        return 1;
    }

  return 0;
}


/*! @brief NE FAIT RIEN: A CODER, renvoie toujours 0 Renvoie 1 si les sommets specifies par le parametre "pos"
 *
 *     sont les sommets de l'element "element" du domaine associe a
 *     l'element geometrique.
 *
 * @param (IntVect& pos) les numeros des sommets a comparer avec ceux de l'elements "element"
 * @param (int element) le numero de l'element du domaine dont on veut comparer les sommets
 * @return (int) 1 si les sommets passes en parametre sont ceux de l'element specifie, 0 sinon
 */
int Polygone::contient(const ArrOfInt& pos, int element ) const
{
  BLOQUE;
  // a coder :
  exit();
  return 0;
}


/*! @brief NE FAIT RIEN: A CODER Calcule les volumes des elements du domaine associe.
 *
 * @param (DoubleVect& volumes) le vecteur contenant les valeurs  des des volumes des elements du domaine
 */
void Polygone::calculer_volumes(DoubleVect& volumes) const
{

  const Domaine& domaine=mon_dom.valeur();
  const IntTab& elem=domaine.les_elems();
  const DoubleTab& coord=domaine.coord_sommets();
  int size = domaine.nb_elem();


  assert(volumes.size_totale()==domaine.nb_elem_tot());
  DoubleTab pos(3,dimension);
  for (int num_poly=0; num_poly<size; num_poly++)
    {
      double aire=0;
      int s0=elem(num_poly,0);
      for (int d=0; d<dimension; d++)
        pos(0,d)=coord(s0,d);
      for (int s=1; s<get_nb_som_elem_max()-1 ; s++)
        {
          int s1=elem(num_poly,s);
          int s2=elem(num_poly,s+1);
          if (s2<0)
            break;
          for (int d=0; d<dimension; d++)
            {
              pos(1,d)=coord(s1,d);
              pos(2,d)=coord(s2,d);
            }
          aire += aire_triangle(pos);
        }
      volumes(num_poly)=aire;
    }
  volumes.echange_espace_virtuel();
  return ;




}
int Polygone::nb_type_face() const
{
  return 1;

}
/*! @brief remplit le tableau faces_som_local(i,j) qui donne pour 0 <= i < nb_faces()  et  0 <= j < nb_som_face(i) le numero local du sommet
 *
 *   sur l'element.
 *   On a  0 <= faces_sommets_locaux(i,j) < nb_som()
 *  Si toutes les faces de l'element n'ont pas le meme nombre de sommets, le nombre
 *  de colonnes du tableau est le plus grand nombre de sommets, et les cases inutilisees
 *  du tableau sont mises a -1
 *  On renvoie 1 si toutes les faces ont le meme nombre d'elements, 0 sinon.
 *
 */
int Polygone::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  return 0;
}
int Polygone::get_tab_faces_sommets_locaux(IntTab& faces_som_local,int ele) const
{
  faces_som_local.resize(nb_face_elem_max_,nb_som_face());
  faces_som_local=-1;
  // on cherche les faces de l'elt

  int nb_face=PolygonIndex_[ele+1]-PolygonIndex_[ele];
  /*
   les elems pas remplis
    int nb_face2=get_nb_som_elem_max();
    const IntTab& les_elems = mon_dom.valeur().les_elems();

    {

    while (les_elems(ele,nb_face2-1)<0)
      nb_face2--;
    }

    assert(nb_face==nb_face2);

  */
  for (int fl=0; fl<nb_face-1; fl++)
    {
      // numerotion locale ou globale ????
      faces_som_local(fl,0)=fl;
      faces_som_local(fl,1)=fl+1;
    }
  {
    int fl=nb_face-1;
    faces_som_local(fl,0)=fl;
    faces_som_local(fl,1)=0;
  }
  /*
    IntTab test;
    get_tab_faces_sommets_locaux(test,ele);
    test-=faces_som_local;
    assert(mp_max_abs_vect(test)==0);
  */
  return 1;

}

// Desctiption : a partir des tableaux d'indirection FacesIndex PolygonIndex
// on calcul les elems, nb_som_face_max_, nb_face_elem_max_ nb_som_elem_max_
void Polygone::affecte_connectivite_numero_global(const ArrOfInt& FacesIndex,const ArrOfInt& PolygonIndex,IntTab& les_elems)
{
  nb_som_elem_max_=0;
  // detremination de nbsom_max
  IntList prov;
  nb_face_elem_max_=0;
  int nelem=PolygonIndex.size_array()-1;
  for (int ele=0; ele<nelem; ele++)
    {
      prov.vide();
      int nbf=PolygonIndex[ele+1]-PolygonIndex[ele];
      if (nbf>nb_face_elem_max_) nb_face_elem_max_=nbf;
      for (int f=PolygonIndex[ele]; f<PolygonIndex[ele+1]; f++)
        {
          //Cerr<<" ici "<<ele << " " <<f <<" "<<FacesIndex(f+1)-FacesIndex(f)<<finl;
          //for (int s=FacesIndex(f); s<FacesIndex(f+1); s++)
          {
            prov.add_if_not(FacesIndex[f]);
          }
        }
      int nbsom=prov.size();
      if (nbsom>nb_som_elem_max_) nb_som_elem_max_=nbsom;
    }
  Cerr<<" Polygon information nb_som_elem_max "<< nb_som_elem_max_<<" nb_face_elem_max "<<nb_face_elem_max_<<finl;
  les_elems.resize(nelem,nb_som_elem_max_);
  les_elems=-1;
  // on refait un tour pour determiiner les elems
  for (int ele=0; ele<nelem; ele++)
    {
      prov.vide();
      for (int f=PolygonIndex[ele]; f<PolygonIndex[ele+1]; f++)
        {
          //for (int s=FacesIndex(f); s<FacesIndex(f+1); s++)
          {
            prov.add_if_not(FacesIndex[f]);
          }
        }
      int nbsom=prov.size();
      for (int s=0; s<nbsom; s++) les_elems(ele,s)=prov[s];
    }
  FacesIndex_=FacesIndex;
  PolygonIndex_=PolygonIndex;
  assert(nb_face_elem_max_==nb_som_elem_max_);
}


/*! @brief Reordonne les sommets du Polygone.
 *
 * NE FAIT RIEN: A CODER
 *
 */
void Polygone::reordonner()
{

}
void Polygone::calculer_centres_gravite(DoubleTab& xp) const
{
  const Domaine& domaine=mon_dom.valeur();
  const IntTab& elem=domaine.les_elems();
  const DoubleTab& coord=domaine.coord_sommets();
  int nb_elem;
  if(xp.dimension(0)==0)
    {
      nb_elem = mon_dom->nb_elem_tot();
      xp.resize(nb_elem,dimension);
    }
  else
    nb_elem=xp.dimension(0);


  xp=0;
  DoubleTab pos(3,dimension);
  ArrOfDouble xpl(dimension);
  for (int num_poly=0; num_poly<nb_elem; num_poly++)
    {
      double aire=0;
      xpl=0;
      int s0=elem(num_poly,0);
      for (int d=0; d<dimension; d++)
        pos(0,d)=coord(s0,d);
      for (int s=1; s<get_nb_som_elem_max()-1 ; s++)
        {
          int s1=elem(num_poly,s);
          int s2=elem(num_poly,s+1);
          if (s2<0)
            break;
          for (int d=0; d<dimension; d++)
            {
              pos(1,d)=coord(s1,d);
              pos(2,d)=coord(s2,d);
            }
          double airel = aire_triangle(pos);
          for (int d=0; d<dimension; d++)
            xpl[d]+=airel*(pos(0,d)+pos(1,d)+pos(2,d));
          aire+=airel;
        }
      aire*=3.;
      for (int d=0; d<dimension; d++)
        xp(num_poly,d)=xpl[d]/(aire);
    }
}

void Polygone::calculer_un_centre_gravite(const int num_poly,DoubleVect& xp) const
{
  const Domaine& domaine=mon_dom.valeur();
  const IntTab& elem=domaine.les_elems();
  const DoubleTab& coord=domaine.coord_sommets();
  xp.resize(dimension);


  xp=0;
  DoubleTab pos(3,dimension);
  ArrOfDouble xpl(dimension);
  {
    double aire=0;
    xpl=0;
    int s0=elem(num_poly,0);
    for (int d=0; d<dimension; d++)
      pos(0,d)=coord(s0,d);
    for (int s=1; s<nb_som_elem_max_-1 ; s++)
      {
        int s1=elem(num_poly,s);
        int s2=elem(num_poly,s+1);
        if (s2<0)
          break;
        for (int d=0; d<dimension; d++)
          {
            pos(1,d)=coord(s1,d);
            pos(2,d)=coord(s2,d);
          }
        double airel = aire_triangle(pos);
        for (int d=0; d<dimension; d++)
          xpl[d]+=airel*(pos(0,d)+pos(1,d)+pos(2,d));
        aire+=airel;
      }
    aire*=3.;
    for (int d=0; d<dimension; d++)
      xp(d)=xpl[d]/(aire);
  }
}

