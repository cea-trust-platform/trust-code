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

#include <Quadri_poly.h>
#include <Domaine.h>

Implemente_instanciable_sans_constructeur(Quadri_poly,"Quadri_poly",Elem_poly_base);

// printOn et readOn

Sortie& Quadri_poly::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Quadri_poly::readOn(Entree& s )
{
  return s ;
}

// Description:
// KEL_(0,fa7),KEL_(1,fa7) sont  les numeros locaux des 2 faces
// qui entourent la facette de numero local fa7
// le numero local de la fa7 est celui du sommet qui la porte
Quadri_poly::Quadri_poly()
{
}

// Description:
// remplit le tableau face_normales dans la Zone_poly
void Quadri_poly::normale(int num_Face,DoubleTab& Face_normales,
                          const  IntTab& Face_sommets,
                          const IntTab& Face_voisins,
                          const IntTab& elem_faces,
                          const Zone& zone_geom) const
{
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();
  double x1,y1;
  double nx,ny;
  double x1g=0,y1g=0;
  double x2g=0,y2g=0;
  double grx,gry,psc;
  int sign=1,i;
  int n0 = Face_sommets(num_Face,0);
  int n1 = Face_sommets(num_Face,1);
  x1 = les_coords(n0,0)-les_coords(n1,0);
  y1 = les_coords(n0,1)-les_coords(n1,1);
  nx = -y1;
  ny = x1;
  int elem1=Face_voisins(num_Face,0);
  int elem2=Face_voisins(num_Face,1);

  //Orientation de la normale vers le plus grand numero d'elem
  //pour cela on teste d'abord si l'on est sur le bord
  if (elem2!=-1)
    {
      //on oriente a partir du centre de gravite
      //calcul du centre de gravite de chaque element
      for(i=0; i<4; i++)
        {
          x1g+=les_coords(Face_sommets(elem_faces(elem1,i),0),0);
          x1g+=les_coords(Face_sommets(elem_faces(elem1,i),1),0);
          y1g+=les_coords(Face_sommets(elem_faces(elem1,i),0),1);
          y1g+=les_coords(Face_sommets(elem_faces(elem1,i),1),1);
          x2g+=les_coords(Face_sommets(elem_faces(elem2,i),0),0);
          x2g+=les_coords(Face_sommets(elem_faces(elem2,i),1),0);
          y2g+=les_coords(Face_sommets(elem_faces(elem2,i),0),1);
          y2g+=les_coords(Face_sommets(elem_faces(elem2,i),1),1);
        }

      grx=(x2g-x1g)*0.125;
      gry=(y2g-y1g)*0.125;

      //on regarde le signe du produit scalaire
      psc=grx*nx+gry*ny;
      if(psc<0)
        {
          if(elem1<elem2)
            sign=-1;
        }
      else if(elem2<elem1)
        sign=-1;
    }
  else
    {
      //on oriente a partir du centre de gravite et du milieu de la
      //face courante

      for(i=0; i<4; i++)
        {
          x1g+=les_coords(Face_sommets(elem_faces(elem1,i),0),0);
          x1g+=les_coords(Face_sommets(elem_faces(elem1,i),1),0);
          y1g+=les_coords(Face_sommets(elem_faces(elem1,i),0),1);
          y1g+=les_coords(Face_sommets(elem_faces(elem1,i),1),1);
        }
      // Cerr << "xg et yg de Face_normales: " << x1g << " " << y1g << finl;

      x2g = les_coords(n0,0)+les_coords(n1,0);
      y2g = les_coords(n0,1)+les_coords(n1,1);
      grx=x2g*0.5-x1g*0.125;
      gry=y2g*0.5-y1g*0.125;

      //   Cerr << "grx et gry : " << grx << " " << gry << finl;
      //on regarde le signe du produit scalaire
      psc=grx*nx+gry*ny;
      if(psc<0)
        sign=-1;
    }
  Face_normales(num_Face,0)=sign*nx;
  Face_normales(num_Face,1)=sign*ny;
}

// Description:
//
void Quadri_poly::calcul_vc(const ArrOfInt& Face,ArrOfDouble& vc,
                            const ArrOfDouble& vs,const DoubleTab& vsom,
                            const Champ_Inc_base& vitesse,int type_cl) const
{
  //Cerr << " DANS Quadri_poly::calcul_vc , type_cl = " << type_cl << finl;
  //Cerr << "vs " << vs << " et vsom " << vsom << " et vitesse " << vitesse << finl;

//   switch(type_cl) {
//   case 0: //  pas de Face de Dirichlet
//     {
  vc[0] = vs[0]*0.25;
  vc[1] = vs[1]*0.25;
//       break;
//     }

//   case 1: // une Face de Dirichlet : Face 3
//     {
//       vc[0] = vitesse(Face[3],0);
//       vc[1] = vitesse(Face[3],1);
//       break;
//     }

//   case 3: // une Face de Dirichlet : Face 2
//     {
//       vc[0] = vitesse(Face[2],0);
//       vc[1] = vitesse(Face[2],1);
//       break;
//     }

//   case 9: // une Face de Dirichlet : Face 1
//     {
//       vc[0] = vitesse(Face[1],0);
//       vc[1] = vitesse(Face[1],1);
//       break;
//     }

//   case 27: // une Faces de Dirichlet :Face 0
//     {
//       vc[0] = vitesse(Face[0],0);
//       vc[1] = vitesse(Face[0],1);
//       break;
//     }

//   case 4: // deux Faces de Dirichlet : Faces 2,3
//     {
//       vc[0]= vsom(3,0);
//       vc[1]= vsom(3,1);
//       break;
//     }

//   case 28: // deux Faces de Dirichlet : Faces 0,3
//     {
//       vc[0]= vsom(2,0);
//       vc[1]= vsom(2,1);
//       break;
//     }

//   case 12: // deux Faces de Dirichlet : Faces 1,2
//     {
//       vc[0]= vsom(1,0);
//       vc[1]= vsom(1,1);
//       break;
//     }

//   case 36: // deux Faces de Dirichlet : Faces 0,1
//     {
//       vc[0]= vsom(0,0);
//       vc[1]= vsom(0,1);
//       break;
//     }


//   case 10: // deux Faces de Dirichlet : Faces 1,3
//     {
//       vc[0] = vs[0]*0.25;
//       vc[1] = vs[1]*0.25;
//       break;
//     }

//   case 30: // deux Faces de Dirichlet : Faces 0,2
//     {
//       vc[0] = vs[0]*0.25;
//       vc[1] = vs[1]*0.25;
//       break;
//     }

//   case 13: //trois Faces de Dirichlet : Faces 3,2,1
//     {
//       vc[0]= vitesse(Face[2],0);
//       vc[1]= vitesse(Face[2],1);
//       break;
//     }

//   case 31: //trois Faces de Dirichlet : Faces 0,3,2
//     {
//       vc[0]= vitesse(Face[3],0);
//       vc[1]= vitesse(Face[3],1);
//       break;
//     }

//   case 37: //trois Faces de Dirichlet : Faces 1,0,3
//     {
//       vc[0]= vitesse(Face[0],0);
//       vc[1]= vitesse(Face[0],1);
//       break;
//     }

//   case 39: //trois Faces de Dirichlet : Faces 2,1,0
//     {
//       vc[0]= vitesse(Face[1],0);
//       vc[1]= vitesse(Face[1],1);
//       break;
//     }

//   default :
//     {
//       Cerr << "\n  type inconnu : " << type_cl ;
//       exit();
//     }

//   } // fin du switch

}

// Description:
// calcule les coord xg du centre d'un element non standard
// calcule aussi idirichlet=nb de faces de Dirichlet de l'element
// si idirichlet=2, n1 est le numero du sommet confondu avec G
void Quadri_poly::calcul_xg(DoubleVect& xg, const DoubleTab& x,
                            const int type_elem_Cl,int& idirichlet,int& n1,int& ,int& ) const
{
  int j,dim=xg.size();
//   switch(type_elem_Cl) {

//   case 0:  //  pas de Face de dirichlet: il a 4 Facettes
//     //  le point G est le barycentre des sommets du triangle
//     {
  for (j=0; j<dim; j++)
    xg[j]=(x(0,j)+x(1,j)+x(2,j)+x(3,j))*0.25;
  idirichlet=0;
//       break;
//     }

//   case 1: // une Face de Dirichlet : Face 3
//     // le point G est le centre de la face 3
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=(x(2,j)+x(3,j))*0.5;
//       idirichlet=1;
//       break;
//     }

//   case 3: // une Face de Dirichlet : Face 2
//     // le point G est le centre de la face 2
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=(x(1,j)+x(3,j))*0.5;
//       idirichlet=1;
//       break;
//     }

//   case 9: // une Face de Dirichlet : Face 1
//     // le point G est le centre de la face 1
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=(x(0,j)+x(1,j))*0.5;
//       idirichlet=1;
//       break;
//     }

//   case 27: // une Faces de Dirichlet :Face 0
//     // le point G est le centre de la face 0
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=(x(0,j)+x(2,j))*0.5;
//       idirichlet=1;
//       break;
//     }

//   case 4: // deux Faces de Dirichlet : Faces 2,3
//     //le point G est le sommet commun au deux faces de dirichlet
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=x(3,j);
//       idirichlet=2;
//       break;
//     }

//   case 28: // deux Faces de Dirichlet : Faces 0,3
//     //le point G est le sommet commun au deux faces de dirichlet
//      {
//       for (j=0; j<dim; j++)
// 	xg[j]=x(2,j);
//       idirichlet=2;
//       break;
//     }

//   case 12: // deux Faces de Dirichlet : Faces 1,2
//     //le point G est le sommet commun au deux faces de dirichlet
//      {
//       for (j=0; j<dim; j++)
// 	xg[j]=x(1,j);
//       idirichlet=2;
//       break;
//     }

//   case 36: // deux Faces de Dirichlet : Faces 0,1
//     //le point G est le sommet commun au deux faces de dirichlet
//      {
//       for (j=0; j<dim; j++)
// 	xg[j]=x(0,j);
//       idirichlet=2;
//       break;
//     }

//   case 10: // deux Faces de Dirichlet : Faces 1,3
//      // on garde les memes volumes de controles que pour des faces internes
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=(x(0,j)+x(1,j)+x(2,j)+x(3,j))*0.25;
//       idirichlet=0;
//       break;
//     }

//   case 30: // deux Faces de Dirichlet : Faces 0,2
//      // on garde les memes volumes de controles que pour des faces internes
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=(x(0,j)+x(1,j)+x(2,j)+x(3,j))*0.25;
//       idirichlet=0;
//       break;
//     }

//   case 13: //trois Faces de Dirichlet : Faces 3,2,1
//     // le point G est le centre de la face de dirichlet opposee a la face non-dirichlet
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=(x(1,j)+x(3,j))*0.5;
//       idirichlet=3;
//       break;
//     }

//   case 31: //trois Faces de Dirichlet : Faces 0,3,2
//     // le point G est le centre de la face de dirichlet opposee a la face non-dirichlet
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=(x(2,j)+x(3,j))*0.5;
//       idirichlet=3;
//       break;
//     }

//   case 37: //trois Faces de Dirichlet : Faces 1,0,3
//     // le point G est le centre de la face de dirichlet opposee a la face non-dirichlet
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=(x(0,j)+x(2,j))*0.5;
//       idirichlet=3;
//       break;
//     }

//   case 39: //trois Faces de Dirichlet : Faces 2,1,0
//     // le point G est le centre de la face de dirichlet opposee a la face non-dirichlet
//     {
//       for (j=0; j<dim; j++)
// 	xg[j]=(x(0,j)+x(1,j))*0.5;
//       idirichlet=3;
//       break;
//     }

//   } // fin du switch

}

