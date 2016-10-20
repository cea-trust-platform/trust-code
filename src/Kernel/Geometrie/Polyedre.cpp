/****************************************************************************
* Copyright (c) 2015, CEA
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

// fonctions pour polyedre contient:

void r8vec_zero ( int n, double a1[] );
double *r8vec_cross_3d ( double v1[3], double v2[3] );
void r8vec_copy ( int n, double a1[], double a2[] );
double r8vec_length ( int dim_num, double x[] );
double r8vec_dot ( int n, double a1[], double a2[] );
double r8vec_triple_product ( double v1[3], double v2[3], double v3[3] );
double arc_cosine ( double c )
{
  double angle;
  double pi = 3.141592653589793;
  if ( c <= -1.0 )
    {
      angle = pi;
    }
  else if ( 1.0 <= c )
    {
      angle = 0.0;
    }
  else
    {
      angle = acos ( c );
    }
  return angle;
}
int i4_max ( int i1, int i2 )
{
  if ( i2 < i1 )
    {
      return i1;
    }
  else
    {
      return i2;
    }
}
int i4_min ( int i1, int i2 )
{
  if ( i1 < i2 )
    {
      return i1;
    }
  else
    {
      return i2;
    }
}
int i4_modp ( int i, int j )
{
  int value;
  if ( j == 0 )
    {
      Cout << "\n";
      Cout << "I4_MODP - Fatal error!\n";
      Cout << "  I4_MODP ( I, J ) called with J = " << j << "\n";
      Process::exit();
    }
  value = i % j;
  if ( value < 0 )
    {
      value = value + abs ( j );
    }
  return value;
}
int i4_wrap ( int ival, int ilo, int ihi )
{
  int jhi;
  int jlo;
  int value;
  int wide;
  jlo = i4_min ( ilo, ihi );
  jhi = i4_max ( ilo, ihi );
  wide = jhi + 1 - jlo;
  if ( wide == 1 )
    {
      value = jlo;
    }
  else
    {
      value = jlo + i4_modp ( ival - jlo, wide );
    }
  return value;
}
double *polygon_normal_3d ( int n, double v[] )
{
  int i;
  int j;
  double *normal;
  double normal_norm;
  double *p;
  double *v1;
  double *v2;
  normal = new double[3];
  v1 = new double[3];
  v2 = new double[3];
  r8vec_zero ( 3, normal );
  for ( i = 0; i < 3; i++ )
    {
      v1[i] = v[i+1*3] - v[i+0*3];
    }
  for ( j = 2; j < n; j++ )
    {
      for ( i = 0; i < 3; i++ )
        {
          v2[i] = v[i+j*3] - v[i+0*3];
        }
      p = r8vec_cross_3d ( v1, v2 );
      for ( i = 0; i < 3; i++ )
        {
          normal[i] = normal[i] + p[i];
        }
      r8vec_copy ( 3, v2, v1 );
      delete [] p;
    }
  normal_norm = r8vec_length ( 3, normal );
  if ( normal_norm != 0.0 )
    {
      for ( i = 0; i < 3; i++ )
        {
          normal[i] = normal[i] / normal_norm;
        }
    }
  delete [] v1;
  delete [] v2;
  return normal;
}
double polygon_solid_angle_3d ( int n, double v[], const double p[3] )
{
  // Cerr<<" toto "<<n ;for (int i=0;i<n;i++) Cerr<<" "<<v[i];  Cerr<<" "<<p[0]<<" "<<p[1]<<" "<<p[2]<<finl;
  double a[3];
  double angle;
  double area = 0.0;
  double b[3];
  int j;
  int jp1;
  double *normal1;
  double normal1_norm;
  double *normal2;
  double normal2_norm;
  double pi = 3.141592653589793;
  double *plane;
  double r1[3];
  double s;
  double value;
  if ( n < 3 )
    {
      return 0.0;
    }
  plane = polygon_normal_3d ( n, v );
  a[0] = v[0+(n-1)*3] - v[0+0*3];
  a[1] = v[1+(n-1)*3] - v[1+0*3];
  a[2] = v[2+(n-1)*3] - v[2+0*3];
  for ( j = 0; j < n; j++ )
    {
      r1[0] = v[0+j*3] - p[0];
      r1[1] = v[1+j*3] - p[1];
      r1[2] = v[2+j*3] - p[2];
      jp1 = i4_wrap ( j + 1, 0, n - 1 );
      b[0] = v[0+jp1*3] - v[0+j*3];
      b[1] = v[1+jp1*3] - v[1+j*3];
      b[2] = v[2+jp1*3] - v[2+j*3];
      normal1 = r8vec_cross_3d ( a, r1 );
      normal1_norm = r8vec_length ( 3, normal1 );
      normal2 = r8vec_cross_3d ( r1, b );
      normal2_norm = r8vec_length ( 3, normal2 );
      s = r8vec_dot ( 3, normal1, normal2 )
          / ( normal1_norm * normal2_norm );
      angle = arc_cosine ( s );
      s = r8vec_triple_product ( b, a, plane );
      if ( 0.0 < s )
        {
          area = area + pi - angle;
        }
      else
        {
          area = area + pi + angle;
        }
      a[0] = -b[0];
      a[1] = -b[1];
      a[2] = -b[2];
      delete [] normal1;
      delete [] normal2;
    }
  area = area - pi * ( double ) ( n - 2 );
  if ( 0.0 < r8vec_dot ( 3, plane, r1 ) )
    {
      value = -area;
    }
  else
    {
      value = area;
    }
  delete [] plane;
  return value;
}
void r8vec_copy ( int n, double a1[], double a2[] )
{
  int i;
  for ( i = 0; i < n; i++ )
    {
      a2[i] = a1[i];
    }
  return;
}
double *r8vec_cross_3d ( double v1[3], double v2[3] )
{
  double *v3;
  v3 = new double[3];
  v3[0] = v1[1] * v2[2] - v1[2] * v2[1];
  v3[1] = v1[2] * v2[0] - v1[0] * v2[2];
  v3[2] = v1[0] * v2[1] - v1[1] * v2[0];
  return v3;
}
double r8vec_dot ( int n, double a1[], double a2[] )
{
  int i;
  double value;
  value = 0.0;
  for ( i = 0; i < n; i++ )
    {
      value = value + a1[i] * a2[i];
    }
  return value;
}
double r8vec_length ( int dim_num, double x[] )
{
  int i;
  double value;
  value = 0.0;
  for ( i = 0; i < dim_num; i++ )
    {
      value = value + pow ( x[i], 2 );
    }
  value = sqrt ( value );
  return value;
}
double r8vec_triple_product ( double v1[3], double v2[3], double v3[3] )
{
  double *v4;
  double value;
  v4 = r8vec_cross_3d ( v2, v3 );
  value = r8vec_dot ( 3, v1, v4 );
  delete [] v4;
  return value;
}
void r8vec_zero ( int n, double a1[] )
{
  int i;
  for ( i = 0; i < n; i++ )
    {
      a1[i] = 0.0;
    }
  return;
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
int Polyedre::contient(const ArrOfDouble& pos, int ele ) const
{


  double area=0;
  int face_order_max=get_nb_som_face_max();
  ArrOfDouble v_face(3*face_order_max);
  const Zone& zone=ma_zone.valeur();
  const IntTab& elem=zone.les_elems();
  const DoubleTab& coord=zone.domaine().coord_sommets();
  for (int f=PolyhedronIndex_(ele); f<PolyhedronIndex_(ele+1); f++)
    {
      int node_num_face=FacesIndex_(f+1)-FacesIndex_(f);
      int s0=FacesIndex_(f);
      for (int s=FacesIndex_(f); s<FacesIndex_(f+1); s++)
        {

          int somm_loc=Nodes_(s);
          int som_glob=elem(ele,somm_loc);
          for (int i=0; i<3; i++)
            v_face(i+(s-s0)*3)=coord(som_glob,i);
        }
      // calcul angle solid
      //double solid_angle=0;

      area = area + polygon_solid_angle_3d ( node_num_face, v_face.addr(), pos.addr() );


    }

  //  AREA should be -4*PI, 0, or 4*PI.
  //  So this test should be quite safe!
  double pi= 3.141592653589793;
  if (( area < -2.0 * pi) || ( 2.0 * pi) < area )
    return 1;
  else
    return 0;
  /*
    BLOQUE;

    // a coder :
    // est-ce que le polyedre de numero element contient le point de
    // coordonnees pos ?
    // adaptation de tetraedre contient
    assert(pos.size_array()==3);
    const Zone& zone=ma_zone.valeur();
    const Domaine& dom=zone.domaine();
    double prod1,prod2,xn,yn,zn;
    int som0, som1, som2, som3,som4,som5;

    // On regarde tout d'abord si le pintr cherche n'est pas un des
    // sommets du triangle
    som0 = zone.sommet_elem(ielem,0);
    som1 = zone.sommet_elem(ielem,1);
    som2 = zone.sommet_elem(ielem,2);
    som3 = zone.sommet_elem(ielem,3);
    som4 = zone.sommet_elem(ielem,4);
    som5 = zone.sommet_elem(ielem,5);
    if( ( est_egal(dom.coord(som0,0),pos(0)) && est_egal(dom.coord(som0,1),pos(1)) && est_egal(dom.coord(som0,2),pos(2)) )
    || (est_egal(dom.coord(som1,0),pos(0)) && est_egal(dom.coord(som1,1),pos(1)) && est_egal(dom.coord(som1,2),pos(2)))
    || (est_egal(dom.coord(som2,0),pos(0)) && est_egal(dom.coord(som2,1),pos(1)) && est_egal(dom.coord(som2,2),pos(2)))
    || (est_egal(dom.coord(som3,0),pos(0)) && est_egal(dom.coord(som3,1),pos(1)) && est_egal(dom.coord(som3,2),pos(2)))
    || (est_egal(dom.coord(som4,0),pos(0)) && est_egal(dom.coord(som4,1),pos(1)) && est_egal(dom.coord(som4,2),pos(2)))
    || (est_egal(dom.coord(som5,0),pos(0)) && est_egal(dom.coord(som5,1),pos(1)) && est_egal(dom.coord(som5,2),pos(2)))
    )
    return 1;

    for (int j=0; j<5; j++)
    {
    switch(j) {
    case 0 :
    som0 = zone.sommet_elem(ielem,0);
    som1 = zone.sommet_elem(ielem,1);
    som2 = zone.sommet_elem(ielem,3);
    som3 = zone.sommet_elem(ielem,2);
    break;
    case 1 :
    som0 = zone.sommet_elem(ielem,0);
    som1 = zone.sommet_elem(ielem,2);
    som2 = zone.sommet_elem(ielem,3);
    som3 = zone.sommet_elem(ielem,4);
    break;
    case 2 :
    som0 = zone.sommet_elem(ielem,1);
    som1 = zone.sommet_elem(ielem,2);
    som2 = zone.sommet_elem(ielem,4);
    som3 = zone.sommet_elem(ielem,0);
    break;
    case 3 :
    som0 = zone.sommet_elem(ielem,0);
    som1 = zone.sommet_elem(ielem,1);
    som2 = zone.sommet_elem(ielem,2);
    som3 = zone.sommet_elem(ielem,3);
    break;
    case 4 :
    som0 = zone.sommet_elem(ielem,3);
    som1 = zone.sommet_elem(ielem,4);
    som2 = zone.sommet_elem(ielem,5);
    som3 = zone.sommet_elem(ielem,0);
    break;
    }

    // Algorithme : le sommet 3 et le point M doivent pour j=0 a 3 du meme cote
    // que le plan formes par les points som0,som1,som2.
    // calcul de la normale au plan som0,som1,som2 :
    xn = (dom.coord(som1,1)-dom.coord(som0,1))*(dom.coord(som2,2)-dom.coord(som0,2))
    - (dom.coord(som1,2)-dom.coord(som0,2))*(dom.coord(som2,1)-dom.coord(som0,1));
    yn = (dom.coord(som1,2)-dom.coord(som0,2))*(dom.coord(som2,0)-dom.coord(som0,0))
    - (dom.coord(som1,0)-dom.coord(som0,0))*(dom.coord(som2,2)-dom.coord(som0,2));
    zn = (dom.coord(som1,0)-dom.coord(som0,0))*(dom.coord(som2,1)-dom.coord(som0,1))
    - (dom.coord(som1,1)-dom.coord(som0,1))*(dom.coord(som2,0)-dom.coord(som0,0));
    prod1 = xn * ( dom.coord(som3,0) - dom.coord(som0,0) )
    + yn * ( dom.coord(som3,1) - dom.coord(som0,1) )
    + zn * ( dom.coord(som3,2) - dom.coord(som0,2) );
    prod2 = xn * ( pos[0] - dom.coord(som0,0) )
    + yn * ( pos[1] - dom.coord(som0,1) )
    + zn * ( pos[2] - dom.coord(som0,2) );
    // Si le point est sur le plan (prod2 quasi nul) : on ne peut pas conclure...
    if (prod1*prod2 < 0 && dabs(prod2)>dabs(prod1)*Objet_U::precision_geom) return 0;
    }
  */
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
int Polyedre::contient(const ArrOfInt& pos, int element ) const
{
  BLOQUE;
  // a coder :
  exit();
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
      for (int f=PolyhedronIndex_(num_poly); f<PolyhedronIndex_(num_poly+1); f++)
        {
          int somm_loc3=Nodes_(FacesIndex_(f+1)-1);
          int n3=elem(num_poly,somm_loc3);
          for (int s=FacesIndex_(f); s<FacesIndex_(f+1)-2; s++)
            {
              int somm_loc1=Nodes_(s);
              int n1=elem(num_poly,somm_loc1);
              int somm_loc2=Nodes_(s+1);
              int n2=elem(num_poly,somm_loc2);
              volume += fabs(
                          coord(n1,0) * ( coord(n2,1) * coord(n3,2) - coord(n3,1) * coord(n2,2) )
                          + coord(n2,0) * ( coord(n3,1) * coord(n1,2) - coord(n1,1) * coord(n3,2) )
                          + coord(n3,0) * ( coord(n1,1) * coord(n2,2) - coord(n2,1) * coord(n1,2) ) );

            }
        }
      volumes(num_poly)=volume/6.;
    }

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
  BLOQUE;
  return 1;
}
int Polyedre::get_tab_faces_sommets_locaux(IntTab& faces_som_local,int ele) const
{
  faces_som_local.resize(nb_face_elem_max_,nb_som_face_max_);
  faces_som_local=-1;
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
  //  exit();
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
  int face_order_max=get_nb_som_face_max();
  ArrOfDouble v_face(3*face_order_max);
  const Zone& zone=ma_zone.valeur();
  const IntTab& elem=zone.les_elems();
  const DoubleTab& coord=zone.domaine().coord_sommets();
  ArrOfDouble pos(3);
  int nelem=zone.nb_elem();

  // pour reordonner on reordonne les sommets des faces, de facon a avoir toutes les normales sortantes ou rentrantes (necessaire pour contient).
  // pour cela on regarde le signe de l'angle solide de chaque face vue du cdg.
  for (int ele=0; ele<nelem; ele++)
    {
      int sm=elem.dimension(1);
      while(elem(ele,sm-1)==-1) sm--;
      for (int d=0; d<3; d++)
        {
          for (int s=0; s<sm; s++)
            pos(d)+=coord(elem(ele,s),d);
          pos(d)/=sm;
        }
      double area;
      for (int f=PolyhedronIndex_(ele); f<PolyhedronIndex_(ele+1); f++)
        {
          int node_num_face=FacesIndex_(f+1)-FacesIndex_(f);
          int s0=FacesIndex_(f);
          for (int s=FacesIndex_(f); s<FacesIndex_(f+1); s++)
            {

              int somm_loc=Nodes_(s);
              int som_glob=elem(ele,somm_loc);
              for (int i=0; i<3; i++)
                v_face(i+(s-s0)*3)=coord(som_glob,i);
            }
          // calcul angle solid
          //double solid_angle=0;

          area =  polygon_solid_angle_3d ( node_num_face, v_face.addr(), pos.addr() );
          if (area>0)
            {
              Cerr<<" The face "<<f-PolyhedronIndex_(ele)<<"/"<< PolyhedronIndex_(ele+1)-PolyhedronIndex_(ele)<< " of the element "<<ele<<" not well oriented, we straighten it "<<finl;
              ArrOfInt save(node_num_face);
              for (int s=0; s<node_num_face; s++) save(s)=Nodes_(FacesIndex_(f)+s);
              for (int s=0; s<node_num_face; s++) Nodes_(FacesIndex_(f+1)-s-1)=save(s);
            }

        }
    }
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

