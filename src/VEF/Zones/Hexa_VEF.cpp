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

#include <Hexa_VEF.h>
#include <Domaine.h>

Implemente_instanciable_sans_constructeur(Hexa_VEF,"Hexa_VEF",Elem_VEF_base);

// printOn et readOn

Sortie& Hexa_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Hexa_VEF::readOn(Entree& s )
{
  return s ;
}

// Description:
// KEL_(0,fa7),KEL_(1,fa7) sont  les numeros locaux des 2 faces
// qui entourent la facette de numero local fa7
// le numero local de la fa7 est celui du sommet qui la porte
Hexa_VEF::Hexa_VEF()
{
  int tmp[4][12]=
  {
    {1, 3, 4, 0,  5, 5, 5, 5,  0, 1, 3, 4},
    {2, 2, 2, 2,  1, 3, 4, 0,  1, 3, 4, 0},
    {0, 1, 3, 2,  4, 5, 7, 6,  4, 5, 7, 6},
    {1, 3, 2, 0,  5, 7, 6, 4,  0, 1, 3, 2}
  };
  KEL_.resize(4,12);
  for (int i=0; i<4; i++)
    for (int j=0; j<12; j++)
      KEL_(i,j)=tmp[i][j];
}

// Description:
// remplit le tableau face_normales dans la Zone_VEF

void Hexa_VEF::normale(int num_Face,DoubleTab& Face_normales,
                       const  IntTab& Face_sommets,
                       const IntTab& Face_voisins,
                       const IntTab& elem_faces,
                       const Zone& zone_geom) const
{
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();

  double x1,y1,z1,x2,y2,z2;
  double nx,ny,nz;
  int f0,no4;
  int elem1;

  // on a les 4 sommets de la face
  int n0 = Face_sommets(num_Face,0);
  int n1 = Face_sommets(num_Face,1);
  int n2 = Face_sommets(num_Face,2);
  int n3 = Face_sommets(num_Face,3);

  // on va decouper la face en deux triangles
  // on initialise Face_normales(num_face,dimension) a 0
  // car on va sommer les deux face_normale des triangles
  Face_normales(num_Face,0) =  0;
  Face_normales(num_Face,1) =  0;
  Face_normales(num_Face,2) =  0;

  // on prend les sommet S1,S2 et S4

  x1 = les_coords(n0,0) - les_coords(n1,0);
  y1 = les_coords(n0,1) - les_coords(n1,1);
  z1 = les_coords(n0,2) - les_coords(n1,2);

  x2 = les_coords(n3,0) - les_coords(n1,0);
  y2 = les_coords(n3,1) - les_coords(n1,1);
  z2 = les_coords(n3,2) - les_coords(n1,2);

  nx = (y1*z2 - y2*z1)/2;
  ny = (-x1*z2 + x2*z1)/2;
  nz = (x1*y2 - x2*y1)/2;

  // Orientation de la normale de elem1 vers elem2
  // pour cela recherche du sommet de elem1 qui n'est pas sur la Face

  elem1=Face_voisins(num_Face,0);

  if ( (f0 = elem_faces(elem1,0)) == num_Face )
    f0 = elem_faces(elem1,1);

  if ( (no4 = Face_sommets(f0,0)) != n0    &&   no4 != n1
       &&   no4 != n2 &&   no4 != n3)
    ;
  else if ( (no4 = Face_sommets(f0,1)) != n0 && no4 != n1
            && no4 != n2 &&   no4 != n3)
    ;
  else if ( (no4 = Face_sommets(f0,2)) != n0 && no4 != n1
            && no4 != n2 &&   no4 != n3)
    ;
  else
    no4 = Face_sommets(f0,3);

  x1 = les_coords(no4,0) - les_coords(n0,0);
  y1 = les_coords(no4,1) - les_coords(n0,1);
  z1 = les_coords(no4,2) - les_coords(n0,2);

  if ( (nx*x1+ny*y1+nz*z1) > 0 )
    {
      Face_normales(num_Face,0) +=  -nx;
      Face_normales(num_Face,1) +=  -ny;
      Face_normales(num_Face,2) +=  -nz;
    }
  else
    {
      Face_normales(num_Face,0) += nx;
      Face_normales(num_Face,1) += ny;
      Face_normales(num_Face,2) += nz;
    }

  // on prend les sommet S1,S4 et S3

  x1 = les_coords(n0,0) - les_coords(n2,0);
  y1 = les_coords(n0,1) - les_coords(n2,1);
  z1 = les_coords(n0,2) - les_coords(n2,2);

  x2 = les_coords(n3,0) - les_coords(n2,0);
  y2 = les_coords(n3,1) - les_coords(n2,1);
  z2 = les_coords(n3,2) - les_coords(n2,2);

  nx = (y1*z2 - y2*z1)/2;
  ny = (-x1*z2 + x2*z1)/2;
  nz = (x1*y2 - x2*y1)/2;

  // Orientation de la normale de elem1 vers elem2
  // pour cela recherche du sommet de elem1 qui n'est pas sur la Face

  elem1=Face_voisins(num_Face,0);

  if ( (f0 = elem_faces(elem1,0)) == num_Face )
    f0 = elem_faces(elem1,1);

  if ( (no4 = Face_sommets(f0,0)) != n0    &&   no4 != n1
       &&   no4 != n2 &&   no4 != n3)
    ;
  else if ( (no4 = Face_sommets(f0,1)) != n0 && no4 != n1
            && no4 != n2 &&   no4 != n3)
    ;
  else if ( (no4 = Face_sommets(f0,2)) != n0 && no4 != n1
            && no4 != n2 &&   no4 != n3)
    ;
  else
    no4 = Face_sommets(f0,3);

  x1 = les_coords(no4,0) - les_coords(n0,0);
  y1 = les_coords(no4,1) - les_coords(n0,1);
  z1 = les_coords(no4,2) - les_coords(n0,2);

  if ( (nx*x1+ny*y1+nz*z1) > 0 )
    {
      Face_normales(num_Face,0) +=  -nx;
      Face_normales(num_Face,1) +=  -ny;
      Face_normales(num_Face,2) +=  -nz;
    }
  else
    {
      Face_normales(num_Face,0) += nx;
      Face_normales(num_Face,1) += ny;
      Face_normales(num_Face,2) += nz;
    }
}


// Description:
// // calcule les normales des facettes pour des elem standards
void Hexa_VEF::creer_facette_normales(const Zone& zone_geom,
                                      DoubleTab& facette_normales,
                                      const IntVect& rang_elem_non_std) const
{
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();
  const IntTab& les_Polys = zone_geom.les_elems();
  int nb_elem = zone_geom.nb_elem();
  int i, fa7;
  int num_som[8];
  double x[8][3];
  double x_[4][3];
  double xg[3];
  double xj0[3];
  double xjnum1[3];
  double xjnum2[3];
  double u[3];
  double v[3];
  double psc;
  double pv[3];
  facette_normales.reset();
  facette_normales.resize(0, nb_facette(), 3);
  zone_geom.creer_tableau_elements(facette_normales);

  for(i=0; i<nb_elem; i++)
    {
      if (rang_elem_non_std(i)==-1)
        {
          num_som[0]=les_Polys(i,0);
          num_som[1]=les_Polys(i,1);
          num_som[2]=les_Polys(i,2);
          num_som[3]=les_Polys(i,3);
          num_som[4]=les_Polys(i,4);
          num_som[5]=les_Polys(i,5);
          num_som[6]=les_Polys(i,6);
          num_som[7]=les_Polys(i,7);

          x[0][0]=les_coords(num_som[0],0);
          x[0][1]=les_coords(num_som[0],1);
          x[0][2]=les_coords(num_som[0],2);
          x[1][0]=les_coords(num_som[1],0);
          x[1][1]=les_coords(num_som[1],1);
          x[1][2]=les_coords(num_som[1],2);
          x[2][0]=les_coords(num_som[2],0);
          x[2][1]=les_coords(num_som[2],1);
          x[2][2]=les_coords(num_som[2],2);
          x[3][0]=les_coords(num_som[3],0);
          x[3][1]=les_coords(num_som[3],1);
          x[3][2]=les_coords(num_som[3],2);
          x[4][0]=les_coords(num_som[4],0);
          x[4][1]=les_coords(num_som[4],1);
          x[4][2]=les_coords(num_som[4],2);
          x[5][0]=les_coords(num_som[5],0);
          x[5][1]=les_coords(num_som[5],1);
          x[5][2]=les_coords(num_som[5],2);
          x[6][0]=les_coords(num_som[6],0);
          x[6][1]=les_coords(num_som[6],1);
          x[6][2]=les_coords(num_som[6],2);
          x[7][0]=les_coords(num_som[7],0);
          x[7][1]=les_coords(num_som[7],1);
          x[7][2]=les_coords(num_som[7],2);

          xg[0]=(x[0][0]+x[1][0]+x[2][0]+x[3][0]+x[4][0]+x[5][0]+x[6][0]+x[7][0])*0.125;
          xg[1]=(x[0][1]+x[1][1]+x[2][1]+x[3][1]+x[4][1]+x[5][1]+x[6][1]+x[7][1])*0.125;
          xg[2]=(x[0][2]+x[1][2]+x[2][2]+x[3][2]+x[4][2]+x[5][2]+x[6][2]+x[7][2])*0.125;


          for (fa7=0; fa7<nb_facette(); fa7++)
            {
              // la fa7 d'un element standard a pour sommets
              // fa7 et G de coordonnees xg :
              u[0]=x[KEL_(2,fa7)][0]-xg[0];
              u[1]=x[KEL_(2,fa7)][1]-xg[1];
              u[2]=x[KEL_(2,fa7)][2]-xg[2];
              v[0]=x[KEL_(3,fa7)][0]-xg[0];
              v[1]=x[KEL_(3,fa7)][1]-xg[1];
              v[2]=x[KEL_(3,fa7)][2]-xg[2];
              prodvect(u,v,pv);

              // Orientation des normales :

              // recuperation du numero de la face, puis de sommets et calcul du centre de gravite de la face

              int num1 = KEL_(0,fa7);
              int num2 = KEL_(1,fa7);

              switch (num1)
                {
                case 0:
                  {
                    num_som[0]=les_Polys(i,0);
                    num_som[1]=les_Polys(i,2);
                    num_som[2]=les_Polys(i,4);
                    num_som[3]=les_Polys(i,6);
                  }
                  break;
                case 1:
                  {
                    num_som[0]=les_Polys(i,0);
                    num_som[1]=les_Polys(i,1);
                    num_som[2]=les_Polys(i,4);
                    num_som[3]=les_Polys(i,5);
                  }
                  break;
                case 2:
                  {
                    num_som[0]=les_Polys(i,0);
                    num_som[1]=les_Polys(i,1);
                    num_som[2]=les_Polys(i,2);
                    num_som[3]=les_Polys(i,3);
                  }
                  break;
                case 3:
                  {
                    num_som[0]=les_Polys(i,1);
                    num_som[1]=les_Polys(i,3);
                    num_som[2]=les_Polys(i,5);
                    num_som[3]=les_Polys(i,7);
                  }
                  break;
                case 4:
                  {
                    num_som[0]=les_Polys(i,2);
                    num_som[1]=les_Polys(i,3);
                    num_som[2]=les_Polys(i,6);
                    num_som[3]=les_Polys(i,7);
                  }
                  break;
                case 5:
                  {
                    num_som[0]=les_Polys(i,4);
                    num_som[1]=les_Polys(i,5);
                    num_som[2]=les_Polys(i,6);
                    num_som[3]=les_Polys(i,7);
                  }
                  break;
                }

              x_[0][0]=les_coords(num_som[0],0);
              x_[0][1]=les_coords(num_som[0],1);
              x_[0][2]=les_coords(num_som[0],2);
              x_[1][0]=les_coords(num_som[1],0);
              x_[1][1]=les_coords(num_som[1],1);
              x_[1][2]=les_coords(num_som[1],2);
              x_[2][0]=les_coords(num_som[2],0);
              x_[2][1]=les_coords(num_som[2],1);
              x_[2][2]=les_coords(num_som[2],2);
              x_[3][0]=les_coords(num_som[3],0);
              x_[3][1]=les_coords(num_som[3],1);
              x_[3][2]=les_coords(num_som[3],2);

              xjnum1[0]=(x_[0][0]+x_[1][0]+x_[2][0]+x_[3][0])*0.25;
              xjnum1[1]=(x_[0][1]+x_[1][1]+x_[2][1]+x_[3][1])*0.25;
              xjnum1[2]=(x_[0][2]+x_[1][2]+x_[2][2]+x_[3][2])*0.25;

              switch (num2)
                {
                case 0:
                  {
                    num_som[0]=les_Polys(i,0);
                    num_som[1]=les_Polys(i,2);
                    num_som[2]=les_Polys(i,4);
                    num_som[3]=les_Polys(i,6);
                  }
                  break;
                case 1:
                  {
                    num_som[0]=les_Polys(i,0);
                    num_som[1]=les_Polys(i,1);
                    num_som[2]=les_Polys(i,4);
                    num_som[3]=les_Polys(i,5);
                  }
                  break;
                case 2:
                  {
                    num_som[0]=les_Polys(i,0);
                    num_som[1]=les_Polys(i,1);
                    num_som[2]=les_Polys(i,2);
                    num_som[3]=les_Polys(i,3);
                  }
                  break;
                case 3:
                  {
                    num_som[0]=les_Polys(i,1);
                    num_som[1]=les_Polys(i,3);
                    num_som[2]=les_Polys(i,5);
                    num_som[3]=les_Polys(i,7);
                  }
                  break;
                case 4:
                  {
                    num_som[0]=les_Polys(i,2);
                    num_som[1]=les_Polys(i,3);
                    num_som[2]=les_Polys(i,6);
                    num_som[3]=les_Polys(i,7);
                  }
                  break;
                case 5:
                  {
                    num_som[0]=les_Polys(i,4);
                    num_som[1]=les_Polys(i,5);
                    num_som[2]=les_Polys(i,6);
                    num_som[3]=les_Polys(i,7);
                  }
                  break;
                }

              x_[0][0]=les_coords(num_som[0],0);
              x_[0][1]=les_coords(num_som[0],1);
              x_[0][2]=les_coords(num_som[0],2);
              x_[1][0]=les_coords(num_som[1],0);
              x_[1][1]=les_coords(num_som[1],1);
              x_[1][2]=les_coords(num_som[1],2);
              x_[2][0]=les_coords(num_som[2],0);
              x_[2][1]=les_coords(num_som[2],1);
              x_[2][2]=les_coords(num_som[2],2);
              x_[3][0]=les_coords(num_som[3],0);
              x_[3][1]=les_coords(num_som[3],1);
              x_[3][2]=les_coords(num_som[3],2);

              xjnum2[0]=(x_[0][0]+x_[1][0]+x_[2][0]+x_[3][0])*0.25;
              xjnum2[1]=(x_[0][1]+x_[1][1]+x_[2][1]+x_[3][1])*0.25;
              xjnum2[2]=(x_[0][2]+x_[1][2]+x_[2][2]+x_[3][2])*0.25;

              xj0[0]= xjnum1[0]-xjnum2[0];
              xj0[1]= xjnum1[1]-xjnum2[1];
              xj0[2]= xjnum1[2]-xjnum2[2];

              psc=xj0[0]*pv[0] + xj0[1]*pv[1] + xj0[2]*pv[2] ;

              if (psc < 0)
                {
                  facette_normales(i,fa7,0) = pv[0]/2;
                  facette_normales(i,fa7,1) = pv[1]/2;
                  facette_normales(i,fa7,2) = pv[2]/2;
                }
              else
                {
                  facette_normales(i,fa7,0)= -pv[0]/2;
                  facette_normales(i,fa7,1)= -pv[1]/2;
                  facette_normales(i,fa7,2)= -pv[2]/2;
                }
            }
        }
    }
  facette_normales.echange_espace_virtuel();
}

void Hexa_VEF::creer_normales_facettes_Cl(DoubleTab& normales_facettes_Cl,
                                          int fa7,
                                          int num_elem,const DoubleTab& x,
                                          const DoubleVect& xg, const
                                          Zone& zone_geom) const
{
  double u[3];
  double v[3];
  double xj0[3];
  double xjnum1[3];
  double xjnum2[3];

  int num_som[8];
  double x_[4][3];
  double pv[3];
  double psc;

  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();
  const IntTab& les_Polys = zone_geom.les_elems();

  int i2 = KEL_(2,fa7);
  int i3 = KEL_(3,fa7);

  u[0]=x(i2,0)-xg[0];
  u[1]=x(i2,1)-xg[1];
  u[2]=x(i2,2)-xg[2];

  v[0]=x(i3,0)-xg[0];
  v[1]=x(i3,1)-xg[1];
  v[2]=x(i3,2)-xg[2];

  prodvect(u,v,pv);

  // Orientation des normales :

  // recuperation du numero de la face, puis de sommets et calcul du centre de gravite de la face

  int num1 = KEL_(0,fa7);
  int num2 = KEL_(1,fa7);

  switch (num1)
    {
    case 0:
      {
        num_som[0]=les_Polys(num_elem,0);
        num_som[1]=les_Polys(num_elem,2);
        num_som[2]=les_Polys(num_elem,4);
        num_som[3]=les_Polys(num_elem,6);
      }
      break;
    case 1:
      {
        num_som[0]=les_Polys(num_elem,0);
        num_som[1]=les_Polys(num_elem,1);
        num_som[2]=les_Polys(num_elem,4);
        num_som[3]=les_Polys(num_elem,5);
      }
      break;
    case 2:
      {
        num_som[0]=les_Polys(num_elem,0);
        num_som[1]=les_Polys(num_elem,1);
        num_som[2]=les_Polys(num_elem,2);
        num_som[3]=les_Polys(num_elem,3);
      }
      break;
    case 3:
      {
        num_som[0]=les_Polys(num_elem,1);
        num_som[1]=les_Polys(num_elem,3);
        num_som[2]=les_Polys(num_elem,5);
        num_som[3]=les_Polys(num_elem,7);
      }
      break;
    case 4:
      {
        num_som[0]=les_Polys(num_elem,2);
        num_som[1]=les_Polys(num_elem,3);
        num_som[2]=les_Polys(num_elem,6);
        num_som[3]=les_Polys(num_elem,7);
      }
      break;
    case 5:
      {
        num_som[0]=les_Polys(num_elem,4);
        num_som[1]=les_Polys(num_elem,5);
        num_som[2]=les_Polys(num_elem,6);
        num_som[3]=les_Polys(num_elem,7);
      }
      break;
    default:
      {
        exit();
        num_som[0]=-1;
        num_som[1]=-1;
        num_som[2]=-1;
        num_som[3]=-1;
      }
      break;
    }

  x_[0][0]=les_coords(num_som[0],0);
  x_[0][1]=les_coords(num_som[0],1);
  x_[0][2]=les_coords(num_som[0],2);
  x_[1][0]=les_coords(num_som[1],0);
  x_[1][1]=les_coords(num_som[1],1);
  x_[1][2]=les_coords(num_som[1],2);
  x_[2][0]=les_coords(num_som[2],0);
  x_[2][1]=les_coords(num_som[2],1);
  x_[2][2]=les_coords(num_som[2],2);
  x_[3][0]=les_coords(num_som[3],0);
  x_[3][1]=les_coords(num_som[3],1);
  x_[3][2]=les_coords(num_som[3],2);

  xjnum1[0]=(x_[0][0]+x_[1][0]+x_[2][0]+x_[3][0])*0.25;
  xjnum1[1]=(x_[0][1]+x_[1][1]+x_[2][1]+x_[3][1])*0.25;
  xjnum1[2]=(x_[0][2]+x_[1][2]+x_[2][2]+x_[3][2])*0.25;


  switch (num2)
    {
    case 0:
      {
        num_som[0]=les_Polys(num_elem,0);
        num_som[1]=les_Polys(num_elem,2);
        num_som[2]=les_Polys(num_elem,4);
        num_som[3]=les_Polys(num_elem,6);
      }
      break;
    case 1:
      {
        num_som[0]=les_Polys(num_elem,0);
        num_som[1]=les_Polys(num_elem,1);
        num_som[2]=les_Polys(num_elem,4);
        num_som[3]=les_Polys(num_elem,5);
      }
      break;
    case 2:
      {
        num_som[0]=les_Polys(num_elem,0);
        num_som[1]=les_Polys(num_elem,1);
        num_som[2]=les_Polys(num_elem,2);
        num_som[3]=les_Polys(num_elem,3);
      }
      break;
    case 3:
      {
        num_som[0]=les_Polys(num_elem,1);
        num_som[1]=les_Polys(num_elem,3);
        num_som[2]=les_Polys(num_elem,5);
        num_som[3]=les_Polys(num_elem,7);
      }
      break;
    case 4:
      {
        num_som[0]=les_Polys(num_elem,2);
        num_som[1]=les_Polys(num_elem,3);
        num_som[2]=les_Polys(num_elem,6);
        num_som[3]=les_Polys(num_elem,7);
      }
      break;
    case 5:
      {
        num_som[0]=les_Polys(num_elem,4);
        num_som[1]=les_Polys(num_elem,5);
        num_som[2]=les_Polys(num_elem,6);
        num_som[3]=les_Polys(num_elem,7);
      }
      break;
    }


  x_[0][0]=les_coords(num_som[0],0);
  x_[0][1]=les_coords(num_som[0],1);
  x_[0][2]=les_coords(num_som[0],2);
  x_[1][0]=les_coords(num_som[1],0);
  x_[1][1]=les_coords(num_som[1],1);
  x_[1][2]=les_coords(num_som[1],2);
  x_[2][0]=les_coords(num_som[2],0);
  x_[2][1]=les_coords(num_som[2],1);
  x_[2][2]=les_coords(num_som[2],2);
  x_[3][0]=les_coords(num_som[3],0);
  x_[3][1]=les_coords(num_som[3],1);
  x_[3][2]=les_coords(num_som[3],2);

  xjnum2[0]=(x_[0][0]+x_[1][0]+x_[2][0]+x_[3][0])*0.25;
  xjnum2[1]=(x_[0][1]+x_[1][1]+x_[2][1]+x_[3][1])*0.25;
  xjnum2[2]=(x_[0][2]+x_[1][2]+x_[2][2]+x_[3][2])*0.25;

  xj0[0]= xjnum1[0]-xjnum2[0];
  xj0[1]= xjnum1[1]-xjnum2[1];
  xj0[2]= xjnum1[2]-xjnum2[2];

  psc=xj0[0]*pv[0] + xj0[1]*pv[1] + xj0[2]*pv[2] ;

  if (psc < 0)
    {
      normales_facettes_Cl(num_elem,fa7,0) = -pv[0]/2;
      normales_facettes_Cl(num_elem,fa7,1) = -pv[1]/2;
      normales_facettes_Cl(num_elem,fa7,2) = -pv[2]/2;
    }
  else
    {
      normales_facettes_Cl(num_elem,fa7,0) = pv[0]/2;
      normales_facettes_Cl(num_elem,fa7,1) = pv[1]/2;
      normales_facettes_Cl(num_elem,fa7,2) = pv[2]/2;
    }
}

// Description:
// modifie les volumes entrelaces pour la face j d'un elem
// non standard
void Hexa_VEF::modif_volumes_entrelaces(int j,int elem,
                                        const Zone_VEF& la_zone_VEF,
                                        DoubleVect& volumes_entrelaces_Cl,
                                        int type_cl) const
{
  Cerr << "Hexa_VEF::modif_volumes_entrelaces() ne fait rien pour le moment " << finl;

}

// Description:
// modifie les volumes entrelaces pour la face joint j d'un elem
// non standard
void Hexa_VEF::modif_volumes_entrelaces_faces_joints(int j,int elem,
                                                     const Zone_VEF& la_zone_VEF,
                                                     DoubleVect& volumes_entrelaces_Cl,
                                                     int type_cl) const
{


}

// Description:
//
void Hexa_VEF::calcul_vc(const ArrOfInt& Face,ArrOfDouble& vc,
                         const ArrOfDouble& vs,const DoubleTab& vsom,
                         const Champ_Inc_base& vitesse,int type_cl, const DoubleVect& porosite_face) const
{

  vc[0] = vs[0]/6;
  vc[1] = vs[1]/6;
  vc[2] = vs[2]/6;
}

// Description:
// calcule les coord xg du centre d'un element non standard
// calcule aussi idirichlet=nb de faces de Dirichlet de l'element
// si idirichlet=2, n1 est le numero du sommet confondu avec G
void Hexa_VEF::calcul_xg(DoubleVect& xg, const DoubleTab& x,
                         const int type_elem_Cl,int& idirichlet,int& n1,int& ,int& ) const
{
  int dim=xg.size();
  for (int j=0; j<dim; j++)
    xg[j]=(x(0,j)+x(1,j)+x(2,j)+x(3,j)+x(4,j)+x(5,j)+x(6,j)+x(7,j))*0.125;
  idirichlet=0;
}


// Description:
// modifie normales_facettes_Cl quand idirichlet=3
// idirichlet=nb de faces de Dirichlet de l'element
// si idirichlet=3, n1 est le numero du sommet confondu avec G
void Hexa_VEF::modif_normales_facettes_Cl(DoubleTab& normales_facettes_Cl,
                                          int fa7,int num_elem,
                                          int idirichlet,int n1,int ,int ) const
{
  switch (idirichlet)
    {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      {
        //        normales_facettes_Cl(num_elem,n1,0) = 0;
        //        normales_facettes_Cl(num_elem,n1,1) = 0;
        break;
      }
    }
}
