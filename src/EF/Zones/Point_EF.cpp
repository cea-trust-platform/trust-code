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
// File:        Point_EF.cpp
// Directory:   $TRUST_ROOT/src/EF/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Point_EF.h>
#include <Domaine.h>
#include <Zone_EF.h>
#include <Champ_P1_EF.h>
// Tri pour voire si champ_P1...
Implemente_instanciable(Point_EF,"Point_EF",Elem_EF_base);

// printOn et readOn


Sortie& Point_EF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Point_EF::readOn(Entree& s )
{
  return s ;
}


// Description:
// remplit le tableau face_normales dans la Zone_EF
void Point_EF::normale(int num_Face,DoubleTab& Face_normales,
                       const  IntTab& Face_sommets,
                       const IntTab& Face_voisins,
                       const IntTab& elem_faces,
                       const Zone& zone_geom) const
{
  abort();
  // pas de sens simple a normale
  Face_normales(num_Face,0) = 1;

  {

    //int n0 = Face_sommets(num_Face,0);

    int elem1 = Face_voisins(num_Face,0);
    // int elem2 = Face_voisins(num_Face,1);

    int n2=elem_faces(elem1,0);
    if (n2==num_Face) Face_normales(num_Face,0) = -1;
    /*
    const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();
    //     const IntTab& elem = zone_geom.elems();

       {
    int n2=elem_faces(elem1,0);
    if ( n2 == num_Face )
    n2 = elem_faces(elem1,1);
    n2=Face_sommets(n2,0);
    for (int i=0;i<dim;i++)
    d(i) =les_coords(n0,i)- les_coords(n2,i);
    d/=d.norm();

    Face_normales(num_Face,0) = -1;

    }

    //int n1 = Face_sommets(num_Face,1);

    // Orientation de la normale de elem1 vers elem2
    // pour cela recherche du sommet de elem1 qui n'est pas sur la Face
    int f0,no3;
    int elem1 = Face_voisins(num_Face,0);
    Cerr<<num_Face<<" iii "<<elem1<< " "<<Face_voisins(num_Face,1)<<" "<<(Face_voisins(num_Face,0)==elem1)	  <<finl;
    if ( (f0 = elem_faces(elem1,0)) == num_Face )
    f0 = elem_faces(elem1,1);
    if ( (no3 = Face_sommets(f0,0)) != n0  )
    Cerr<<"oo "<<finl;
    else
    {
    Cerr<<"ii"<<finl;
    no3 = Face_sommets(f0,1);
    }
    Cerr<<n0 << " "<<finl;
    */
  }
  return;
  /*


  double x1,y1;
  double nx,ny;
  int no3; int f0;
  int n0 = Face_sommets(num_Face,0);
  int n1 = Face_sommets(num_Face,1);
  x1 = les_coords(n0,0)-les_coords(n1,0);
  y1 = les_coords(n0,1)-les_coords(n1,1);
  nx = -y1;
  ny = x1;

  // Orientation de la normale de elem1 vers elem2
  // pour cela recherche du sommet de elem1 qui n'est pas sur la Face
  int elem1 = Face_voisins(num_Face,0);
  if ( (f0 = elem_faces(elem1,0)) == num_Face )
    f0 = elem_faces(elem1,1);
  if ( (no3 = Face_sommets(f0,0)) != n0 && no3 != n1 )
    ;
  else
    no3 = Face_sommets(f0,1);

  x1 = les_coords(no3,0) - les_coords(n0,0);
  y1 = les_coords(no3,1) - les_coords(n0,1);

  if ( (nx*x1+ny*y1) > 0 ) {
    Face_normales(num_Face,0) = - nx;
    Face_normales(num_Face,1) = - ny;
  }
  else {
    Face_normales(num_Face,0) = nx;
    Face_normales(num_Face,1) = ny;
  }
  */
}

// Description:
//
void Point_EF::calcul_vc(const ArrOfInt& Face,ArrOfDouble& vc,
                         const ArrOfDouble& vs,const DoubleTab& vsom,
                         const Champ_Inc_base& vitesse,int type_cl) const
{
  abort();
  const Zone_EF& zone_EF = ref_cast(Zone_EF,vitesse.zone_dis_base());
  const DoubleVect& porosite_face = zone_EF.porosite_face();
  //Cerr << " type_cl " <<  type_cl << finl;
  switch(type_cl)
    {
    case 0: // le triangle n'a pas de Face de Dirichlet
      {
        vc[0] = vs[0]/3;
        vc[1] = vs[1]/3;
        break;
      }

    case 1: // le triangle a une Face de Dirichlet :la Face 2
      {
        vc[0]= vitesse(Face[2],0)*porosite_face[Face[2]];
        vc[1]= vitesse(Face[2],1)*porosite_face[Face[2]];
        //vc[0]= vitesse(Face[2],0);
        //vc[1]= vitesse(Face[2],1);
        break;
      }

    case 2: // le triangle a une Face de Dirichlet :la Face 1
      {
        vc[0]= vitesse(Face[1],0)*porosite_face[Face[1]];
        vc[1]= vitesse(Face[1],1)*porosite_face[Face[1]];
        //vc[0]= vitesse(Face[1],0);
        //vc[1]= vitesse(Face[1],1);
        break;
      }

    case 4: // le triangle a une Face de Dirichlet :la Face 0
      {
        vc[0]= vitesse(Face[0],0)*porosite_face[Face[0]];
        vc[1]= vitesse(Face[0],1)*porosite_face[Face[0]];
        // vc[0]= vitesse(Face[0],0);
        //vc[1]= vitesse(Face[0],1);
        break;
      }

    case 3: // le triangle a deux faces de Dirichlet :les faces 1 et 2
      {
        vc[0]= vsom(0,0);
        vc[1]= vsom(0,1);
        break;
      }

    case 5: // le triangle a deux faces de Dirichlet :les faces 0 et 2
      {
        vc[0]= vsom(1,0);
        vc[1]= vsom(1,1);
        break;
      }

    case 6: // le triangle a deux faces de Dirichlet :les faces 0 et 1
      {
        vc[0]= vsom(2,0);
        vc[1]= vsom(2,1);
        break;
      }

    } // fin du switch

}

// Description:
// calcule les coord xg du centre d'un element non standard
// calcule aussi idirichlet=nb de faces de Dirichlet de l'element
// si idirichlet=2, n1 est le numero du sommet confondu avec G
void Point_EF::calcul_xg(DoubleVect& xg, const DoubleTab& x,
                         const int type_elem_Cl,int& idirichlet,int& n1,int& ,int& ) const
{
  abort();
  int j,dim=xg.size();
  switch(type_elem_Cl)
    {

    case 0:  // le triangle n'a pas de Face de dirichlet: il a 3 Facettes
      //  le point G est le barycentre des sommets du triangle
      {
        for (j=0; j<dim; j++)
          xg[j]=(x(0,j)+x(1,j)+x(2,j))/3;

        idirichlet=0;
        break;
      }

    case 1:  // le triangle a une Face de dirichlet: la Face 2
      // le point G est le barycentre des sommets de la Face 2

      {
        for (j=0; j<dim; j++)
          xg[j]=(x(0,j)+x(1,j))/2;

        idirichlet=1;
        break;
      }

    case 2:  // le triangle a une Face de dirichlet: la Face 1
      // le point G est le barycentre des sommets de la Face 1

      {
        for (j=0; j<dim; j++)
          xg[j]=(x(0,j)+x(2,j))/2;

        idirichlet=1;
        break;
      }

    case 4:  // le triangle a une Face de dirichlet: la Face 0
      // le point G est le barycentre des sommets de la Face 0

      {
        for (j=0; j<dim; j++)
          xg[j]=(x(1,j)+x(2,j))/2;

        idirichlet=1;
        break;
      }

    case 6 : // le triangle a deux faces de Dirichlet : les faces 0,1
      // le point G est le sommet 2 du triangle

      {
        for (j=0; j<dim; j++)
          xg[j]=x(2,j);

        idirichlet=2;
        n1 = 2;
        break;

      }

    case 5 : // le triangle a deux faces de Dirichlet : les faces 0,2
      // le point G est le sommet 1 du triangle

      {
        for (j=0; j<dim; j++)
          xg[j]=x(1,j);

        idirichlet=2;
        n1 = 1;
        break;

      }

    case 3 : // le triangle a deux faces de Dirichlet : les faces 1,2
      // le point G est le sommet 0 du triangle

      {
        for (j=0; j<dim; j++)
          xg[j]=x(0,j);

        idirichlet=2;
        n1 = 0;
        break;

      }

    } // fin du switch

}
