/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Quadri_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Zones
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Quadri_VEF.h>
#include <Domaine.h>

Implemente_instanciable_sans_constructeur(Quadri_VEF,"Quadri_VEF",Elem_VEF_base);

// printOn et readOn

Sortie& Quadri_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Quadri_VEF::readOn(Entree& s )
{
  return s ;
}

// Description:
// KEL_(0,fa7),KEL_(1,fa7) sont  les numeros locaux des 2 faces
// qui entourent la facette de numero local fa7
// le numero local de la fa7 est celui du sommet qui la porte
Quadri_VEF::Quadri_VEF()
{
  int tmp[3][4]=
  {
    {0, 1, 0, 2},
    {1, 2, 3, 3},
    {0, 1, 2, 3}
  };
  KEL_.resize(3,4);
  for (int i=0; i<3; i++)
    for (int j=0; j<4; j++)
      KEL_(i,j)=tmp[i][j];
}

// Description:
// remplit le tableau face_normales dans la Zone_VEF
void Quadri_VEF::normale(int num_Face,DoubleTab& Face_normales,
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
          else if(elem2<elem1)
            sign=-1;
        }
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
// calcule les normales des facettes pour des elem standards
void Quadri_VEF::creer_facette_normales(const Zone& zone_geom,
                                        DoubleTab& facette_normales,
                                        const IntVect& rang_elem_non_std) const
{
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();
  const IntTab& les_Polys = zone_geom.les_elems();
  int nb_elem = zone_geom.nb_elem();

  int i, fa7;
  int i0=-1,i1=-1;
  int num_som[4];
  double x[4][2];
  double xg[2];
  double xj0[2];
  double u[2];
  double v[2];
  double psc;

  facette_normales.resize(0, nb_facette(), 2);
  zone_geom.creer_tableau_elements(facette_normales);

  for(i=0; i<nb_elem; i++)
    {
      if (rang_elem_non_std(i)==-1)
        {
          num_som[0]=les_Polys(i,0);
          num_som[1]=les_Polys(i,1);
          num_som[2]=les_Polys(i,2);
          num_som[3]=les_Polys(i,3);
          //        Cerr << "numero de sommet dans Quadri_VEF : "
          //              << num_som[0] << " " << num_som[1] << " "
          //              << num_som[2] << " " << num_som[3] << finl;
          x[0][0]=les_coords(num_som[0],0);
          x[0][1]=les_coords(num_som[0],1);
          x[1][0]=les_coords(num_som[1],0);
          x[1][1]=les_coords(num_som[1],1);
          x[2][0]=les_coords(num_som[2],0);
          x[2][1]=les_coords(num_som[2],1);
          x[3][0]=les_coords(num_som[3],0);
          x[3][1]=les_coords(num_som[3],1);
          xg[0]=(x[0][0]+x[1][0]+x[2][0]+x[3][0])*0.25;
          xg[1]=(x[0][1]+x[1][1]+x[2][1]+x[3][1])*0.25;

          for (fa7=0; fa7<4; fa7++)
            {
              // la fa7 d'un element standard a pour sommets
              // fa7 et G de coordonnees xg :
              u[0]= x[fa7][0]-xg[0];
              u[1]= x[fa7][1]-xg[1];
              v[0]= -u[1];
              v[1]= u[0];

              // Orientation des normales :
              switch (fa7)
                {
                case 0:
                  i0=1;
                  i1=2;
                  break;
                case 1:
                  i0=3;
                  i1=0;
                  break;
                case 2:
                  i0=3;
                  i1=0;
                  break;
                case 3:
                  i0=2;
                  i1=1;
                  break;
                default:
                  i0=-1;
                  i1=-1;
                  assert(0);
                  exit();
                  break;
                }

              // i0 = KEL_(0,fa7);
              // i1 = KEL_(1,fa7);

              xj0[0]= x[i0][0]-x[i1][0];
              xj0[1]= x[i0][1]-x[i1][1];

              psc=xj0[0]*v[0] + xj0[1]*v[1];
              //  Cerr << "facette_normales : " << facette_normales << finl;
              //Cerr << "numero d'element : " << i << finl;
              //Cerr << "numero de fa7 : " << fa7 << finl;
              if (psc < 0)
                {
                  facette_normales(i,fa7,0) = -v[0];
                  facette_normales(i,fa7,1) = -v[1];
                }
              else
                {
                  facette_normales(i,fa7,0) = v[0];
                  facette_normales(i,fa7,1) = v[1];
                }
              //Cerr << "facette_normales : " << facette_normales << finl;
            }
        }
    }
  facette_normales.echange_espace_virtuel();

  //  Cerr << "facette_normales internes (Quadri_VEF::creer_normales_facettes) : " << facette_normales << finl;
}

// Description:
// remplit le tableau normales_facettes_Cl dans la Zone_Cl_VEF
// pour la facette fa7 de l'element num_elem
void Quadri_VEF::creer_normales_facettes_Cl(DoubleTab& normales_facettes_Cl,
                                            int fa7,
                                            int num_elem,const DoubleTab& x,
                                            const DoubleVect& xg, const Zone& zone_geom) const
{
  double u[2];
  double v[2];
  double xj0[2];
  double psc;

  u[0]= x(fa7,0)-xg[0];
  u[1]= x(fa7,1)-xg[1];
  v[0]= -u[1];
  v[1]= u[0];

  int i0=-1;
  int i1=-1;

  switch (fa7)
    {
    case 0:
      i0=1;
      i1=2;
      break;
    case 1:
      i0=3;
      i1=0;
      break;
    case 2:
      i0=3;
      i1=0;
      break;
    case 3:
      i0=2;
      i1=1;
      break;
    default:
      i0=-1;
      i1=-1;
      assert(0);
      exit();
      break;
    }

  // Orientation des normales :
  xj0[0]= x(i0,0)-x(i1,0);
  xj0[1]= x(i0,1)-x(i1,1);

  psc=xj0[0]*v[0] + xj0[1]*v[1];
  if (psc < 0)
    {
      normales_facettes_Cl(num_elem,fa7,0) = -v[0];
      normales_facettes_Cl(num_elem,fa7,1) = -v[1];
    }
  else
    {
      normales_facettes_Cl(num_elem,fa7,0) = v[0];
      normales_facettes_Cl(num_elem,fa7,1) = v[1];
    }
  // Cerr << "normales_facettes_Cl " << normales_facettes_Cl << finl;
}

// Description:
// modifie les volumes entrelaces pour la face j d'un elem
// non standard
void Quadri_VEF::modif_volumes_entrelaces(int j,int elem,
                                          const Zone_VEF& la_zone_VEF,
                                          DoubleVect& volumes_entrelaces_Cl,
                                          int type_cl) const
{

  Cerr << "Quadri_VEF::modif_volumes_entrelaces() ne fait rien pour le moment " << finl;
  //  double surf_mod;
  //   const DoubleVect& volumes_entrelaces = la_zone_VEF.volumes_entrelaces();
  //   const IntTab& elem_faces = la_zone_VEF.elem_faces();
  //   Cerr << "elem " << elem << finl;
  //   Cerr << "type_cl " << type_cl << finl;

  //   switch(type_cl) {

  //     // pas de Face de Dirichlet : impossible
  //   case 0:
  //     {
  //       Cerr << "Quadri_VEF::modif_volumes_entrelaces() type 0 impossible!\n";
  //       break;
  //     }

  //   case 1: // une Face de Dirichlet : Face 3
  //     {
  //       DoubleTab coord(3,2);
  //       for(int i=0;i<3;i++)
  //         for(int k=0;k<2;k++)
  //           coord(i,k)=la_zone_VEF.zone().domaine().coord(la_zone_VEF.zone().sommet_elem(elem,i),k);
  //       volumes_entrelaces_Cl[elem_faces(elem,0)]  = std::fabs((coord(0,0)-coord(2,0))*((coord(2,1)+coord(3,1))/2-coord(2,2)) - ((coord(2,0)+coord(3,0))/2-coord(2,0))*(coord(0,1)-coord(2,2)))/2;
  //       volumes_entrelaces_Cl[elem_faces(elem,2)]  = std::fabs((coord(1,0)-coord(3,0))*((coord(2,1)+coord(3,1))/2-coord(3,2)) - ((coord(2,0)+coord(3,0))/2-coord(3,0))*(coord(1,1)-coord(3,2)))/2;
  //       volumes_entrelaces_Cl[elem_faces(elem,1)]  = std::fabs((coord(1,0)-coord(0,0))*((coord(2,1)+coord(3,1))/2-coord(0,2)) - ((coord(2,0)+coord(3,0))/2-coord(0,0))*(coord(1,1)-coord(0,2)))/2;
  //       break;
  //     }

  //   case 3: // une Face de Dirichlet : Face 2
  //     {
  //       DoubleTab coord(3,2);
  //       for(int i=0;i<3;i++)
  //         for(int k=0;k<2;k++)
  //           coord(i,k)=la_zone_VEF.zone().domaine().coord(la_zone_VEF.zone().sommet_elem(elem,i),k);
  //       volumes_entrelaces_Cl[elem_faces(elem,1)]  = std::fabs((coord(2,0)-coord(3,0))*((coord(1,1)+coord(3,1))/2-coord(3,2)) - ((coord(1,0)+coord(3,0))/2-coord(3,0))*(coord(2,1)-coord(3,2)))/2;
  //       volumes_entrelaces_Cl[elem_faces(elem,3)]  = std::fabs((coord(0,0)-coord(1,0))*((coord(1,1)+coord(3,1))/2-coord(1,2)) - ((coord(1,0)+coord(3,0))/2-coord(1,0))*(coord(0,1)-coord(1,2)))/2;
  //       volumes_entrelaces_Cl[elem_faces(elem,0)]  = std::fabs((coord(0,0)-coord(2,0))*((coord(1,1)+coord(3,1))/2-coord(2,2)) - ((coord(1,0)+coord(3,0))/2-coord(2,0))*(coord(0,1)-coord(2,2)))/2;
  //       break;
  //     }

  //   case 9: // une Face de Dirichlet : Face 1
  //     {
  //       DoubleTab coord(3,2);
  //       for(int i=0;i<3;i++)
  //         for(int k=0;k<2;k++)
  //           coord(i,k)=la_zone_VEF.zone().domaine().coord(la_zone_VEF.zone().sommet_elem(elem,i),k);
  //       volumes_entrelaces_Cl[elem_faces(elem,0)]  = std::fabs((coord(2,0)-coord(0,0))*((coord(0,1)+coord(1,1))/2-coord(0,2)) - ((coord(0,0)+coord(1,0))/2-coord(0,0))*(coord(2,1)-coord(0,2)))/2;
  //       volumes_entrelaces_Cl[elem_faces(elem,2)]  = std::fabs((coord(3,0)-coord(1,0))*((coord(0,1)+coord(1,1))/2-coord(1,2)) - ((coord(0,0)+coord(1,0))/2-coord(1,0))*(coord(3,1)-coord(1,2)))/2;
  //       volumes_entrelaces_Cl[elem_faces(elem,3)]  = std::fabs((coord(3,0)-coord(2,0))*((coord(0,1)+coord(1,1))/2-coord(2,2)) - ((coord(0,0)+coord(1,0))/2-coord(2,0))*(coord(3,1)-coord(2,2)))/2;
  //       break;
  //     }

  //   case 27: // une Face de Dirichlet :Face 0
  //     {

  //       DoubleTab coord(3,2);
  //       for(int i=0;i<3;i++) {
  //         for(int k=0;k<2;k++) {
  //           Cerr << "la_zone_VEF.zone().sommet_elem(elem,i) " << la_zone_VEF.zone().sommet_elem(elem,i) << finl;
  //           coord(i,k)=la_zone_VEF.zone().domaine().coord(la_zone_VEF.zone().sommet_elem(elem,i),k);
  //           Cerr << "coord(i,k) " <<  coord(i,k) << finl;
  //         }
  //       }
  //       Cerr << " elem_faces(elem,1) " << elem_faces(elem,1) << finl;
  //       Cerr << " elem_faces(elem,3) " << elem_faces(elem,3) << finl;
  //       Cerr << " elem_faces(elem,2) " << elem_faces(elem,2) << finl;

  //       Cerr << " coord(0,0) " << coord(0,0) << " coord(0,1) " << coord(0,1)  << finl;
  //       Cerr << " coord(1,0) " << coord(1,0) << " coord(1,1) " << coord(1,1)  << finl;
  //       Cerr << " coord(2,0) " << coord(2,0) << " coord(2,1) " << coord(2,1)  << finl;

  //       volumes_entrelaces_Cl[elem_faces(elem,1)]  = std::fabs((coord(3,0)-coord(2,0))*((coord(0,1)+coord(2,1))/2-coord(2,2)) - ((coord(0,0)+coord(2,0))/2-coord(2,0))*(coord(3,1)-coord(2,2)))/2;
  //       volumes_entrelaces_Cl[elem_faces(elem,3)]  = std::fabs((coord(1,0)-coord(0,0))*((coord(0,1)+coord(2,1))/2-coord(0,2)) - ((coord(0,0)+coord(2,0))/2-coord(0,0))*(coord(1,1)-coord(0,2)))/2;
  //       volumes_entrelaces_Cl[elem_faces(elem,2)]  = std::fabs((coord(3,0)-coord(1,0))*((coord(0,1)+coord(2,1))/2-coord(1,2)) - ((coord(0,0)+coord(2,0))/2-coord(1,0))*(coord(3,1)-coord(1,2)))/2;

  //       break;
  //     }

  //   case 4: // deux Faces de Dirichlet : Faces 2,3
  //     {
  //       Cerr << " elem_faces(elem,0) " << elem_faces(elem,0) << finl;
  //       Cerr << " elem_faces(elem,1) " << elem_faces(elem,1) << finl;
  //       volumes_entrelaces_Cl[elem_faces(elem,0)]  +=volumes_entrelaces[elem_faces(elem,3)];
  //       volumes_entrelaces_Cl[elem_faces(elem,1)]  +=volumes_entrelaces[elem_faces(elem,2)];
  //       break;
  //     }

  //   case 28: // deux Faces de Dirichlet : Faces 0,3
  //     {
  //       volumes_entrelaces_Cl[elem_faces(elem,1)]  +=volumes_entrelaces[elem_faces(elem,0)];
  //       volumes_entrelaces_Cl[elem_faces(elem,2)]  +=volumes_entrelaces[elem_faces(elem,3)];
  //       break;
  //     }

  //   case 12: // deux Faces de Dirichlet : Faces 1,2
  //     {
  //       volumes_entrelaces_Cl[elem_faces(elem,0)]  +=volumes_entrelaces[elem_faces(elem,1)];
  //       volumes_entrelaces_Cl[elem_faces(elem,3)]  +=volumes_entrelaces[elem_faces(elem,2)];
  //       break;
  //     }

  //   case 36: // deux Faces de Dirichlet : Faces 0,1
  //     {
  //       volumes_entrelaces_Cl[elem_faces(elem,3)]  +=volumes_entrelaces[elem_faces(elem,0)];
  //       volumes_entrelaces_Cl[elem_faces(elem,2)]  +=volumes_entrelaces[elem_faces(elem,1)];
  //       break;
  //     }

  //   case 10: // deux Faces de Dirichlet : Faces 1,3
  //     {
  //       volumes_entrelaces_Cl[elem_faces(elem,0)]  +=volumes_entrelaces[elem_faces(elem,0)];
  //       volumes_entrelaces_Cl[elem_faces(elem,2)]  +=volumes_entrelaces[elem_faces(elem,2)];
  //       break;
  //     }

  //   case 30: // deux Faces de Dirichlet : Faces 0,2
  //     {
  //       volumes_entrelaces_Cl[elem_faces(elem,1)]  +=volumes_entrelaces[elem_faces(elem,1)];
  //       volumes_entrelaces_Cl[elem_faces(elem,3)]  +=volumes_entrelaces[elem_faces(elem,3)];
  //       break;
  //     }

  //   case 13: //trois Faces de Dirichlet : Faces 3,2,1
  //     {
  //       volumes_entrelaces_Cl[elem_faces(elem,0)]  +=volumes_entrelaces[elem_faces(elem,3)]+volumes_entrelaces[elem_faces(elem,2)]
  //         +volumes_entrelaces[elem_faces(elem,1)];
  //       break;
  //     }

  //   case 31: //trois Faces de Dirichlet : Faces 0,3,2
  //     {
  //       volumes_entrelaces_Cl[elem_faces(elem,1)]  +=volumes_entrelaces[elem_faces(elem,3)]+volumes_entrelaces[elem_faces(elem,2)]
  //         +volumes_entrelaces[elem_faces(elem,0)];
  //       break;
  //     }

  //   case 37: //trois Faces de Dirichlet : Faces 1,0,3
  //     {
  //       volumes_entrelaces_Cl[elem_faces(elem,2)]  +=volumes_entrelaces[elem_faces(elem,3)]+volumes_entrelaces[elem_faces(elem,1)]
  //         +volumes_entrelaces[elem_faces(elem,0)];
  //       break;
  //     }

  //   case 39: //trois Faces de Dirichlet : Faces 2,1,0
  //     {
  //       volumes_entrelaces_Cl[elem_faces(elem,3)]  +=volumes_entrelaces[elem_faces(elem,2)]+volumes_entrelaces[elem_faces(elem,1)]
  //         +volumes_entrelaces[elem_faces(elem,0)];
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
// modifie les volumes entrelaces pour la face joint j d'un elem
// non standard
void Quadri_VEF::modif_volumes_entrelaces_faces_joints(int j,int elem,
                                                       const Zone_VEF& la_zone_VEF,
                                                       DoubleVect& volumes_entrelaces_Cl,
                                                       int type_cl) const
{
  // const DoubleVect& volumes_entrelaces = la_zone_VEF.volumes_entrelaces();
  //   const IntTab& elem_faces = la_zone_VEF.elem_faces();

  //   switch(type_cl) {

  //     // pas de Face de Dirichlet : impossible
  //   case 0:
  //     {
  //       Cerr << "Quadri_VEF::modif_volumes_entrelaces() type 0 impossible!\n";
  //       break;
  //     }


  //   } // fin du switch

}

// Description:
//
void Quadri_VEF::calcul_vc(const ArrOfInt& Face,ArrOfDouble& vc,
                           const ArrOfDouble& vs,const DoubleTab& vsom,
                           const Champ_Inc_base& vitesse,int type_cl, const DoubleVect& porosite_face) const
{
  //Cerr << " DANS Quadri_VEF::calcul_vc , type_cl = " << type_cl << finl;
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
void Quadri_VEF::calcul_xg(DoubleVect& xg, const DoubleTab& x,
                           const int type_elem_Cl,int& idirichlet,int& n1,int& ,int& ) const
{
  int dim=xg.size();
  //   switch(type_elem_Cl) {

  //   case 0:  //  pas de Face de dirichlet: il a 4 Facettes
  //     //  le point G est le barycentre des sommets du triangle
  //     {
  for (int j=0; j<dim; j++)
    xg[j]=(x(0,j)+x(1,j)+x(2,j)+x(3,j))*0.25;
  idirichlet=0;
  //       break;
  //     }

  //   case 1: // une Face de Dirichlet : Face 3
  //     // le point G est le centre de la face 3
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=(x(2,j)+x(3,j))*0.5;
  //       idirichlet=1;
  //       break;
  //     }

  //   case 3: // une Face de Dirichlet : Face 2
  //     // le point G est le centre de la face 2
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=(x(1,j)+x(3,j))*0.5;
  //       idirichlet=1;
  //       break;
  //     }

  //   case 9: // une Face de Dirichlet : Face 1
  //     // le point G est le centre de la face 1
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=(x(0,j)+x(1,j))*0.5;
  //       idirichlet=1;
  //       break;
  //     }

  //   case 27: // une Faces de Dirichlet :Face 0
  //     // le point G est le centre de la face 0
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=(x(0,j)+x(2,j))*0.5;
  //       idirichlet=1;
  //       break;
  //     }

  //   case 4: // deux Faces de Dirichlet : Faces 2,3
  //     //le point G est le sommet commun au deux faces de dirichlet
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=x(3,j);
  //       idirichlet=2;
  //       break;
  //     }

  //   case 28: // deux Faces de Dirichlet : Faces 0,3
  //     //le point G est le sommet commun au deux faces de dirichlet
  //      {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=x(2,j);
  //       idirichlet=2;
  //       break;
  //     }

  //   case 12: // deux Faces de Dirichlet : Faces 1,2
  //     //le point G est le sommet commun au deux faces de dirichlet
  //      {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=x(1,j);
  //       idirichlet=2;
  //       break;
  //     }

  //   case 36: // deux Faces de Dirichlet : Faces 0,1
  //     //le point G est le sommet commun au deux faces de dirichlet
  //      {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=x(0,j);
  //       idirichlet=2;
  //       break;
  //     }

  //   case 10: // deux Faces de Dirichlet : Faces 1,3
  //      // on garde les memes volumes de controles que pour des faces internes
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=(x(0,j)+x(1,j)+x(2,j)+x(3,j))*0.25;
  //       idirichlet=0;
  //       break;
  //     }

  //   case 30: // deux Faces de Dirichlet : Faces 0,2
  //      // on garde les memes volumes de controles que pour des faces internes
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=(x(0,j)+x(1,j)+x(2,j)+x(3,j))*0.25;
  //       idirichlet=0;
  //       break;
  //     }

  //   case 13: //trois Faces de Dirichlet : Faces 3,2,1
  //     // le point G est le centre de la face de dirichlet opposee a la face non-dirichlet
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=(x(1,j)+x(3,j))*0.5;
  //       idirichlet=3;
  //       break;
  //     }

  //   case 31: //trois Faces de Dirichlet : Faces 0,3,2
  //     // le point G est le centre de la face de dirichlet opposee a la face non-dirichlet
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=(x(2,j)+x(3,j))*0.5;
  //       idirichlet=3;
  //       break;
  //     }

  //   case 37: //trois Faces de Dirichlet : Faces 1,0,3
  //     // le point G est le centre de la face de dirichlet opposee a la face non-dirichlet
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=(x(0,j)+x(2,j))*0.5;
  //       idirichlet=3;
  //       break;
  //     }

  //   case 39: //trois Faces de Dirichlet : Faces 2,1,0
  //     // le point G est le centre de la face de dirichlet opposee a la face non-dirichlet
  //     {
  //       for (int j=0; j<dim; j++)
  //         xg[j]=(x(0,j)+x(1,j))*0.5;
  //       idirichlet=3;
  //       break;
  //     }

  //   } // fin du switch

}


// Description:
// modifie normales_facettes_Cl quand idirichlet=3
// idirichlet=nb de faces de Dirichlet de l'element
// si idirichlet=3, n1 est le numero du sommet confondu avec G
void Quadri_VEF::modif_normales_facettes_Cl(DoubleTab& normales_facettes_Cl,
                                            int fa7,int num_elem,
                                            int idirichlet,int n1,int ,int ) const
{
  // Cerr << "Quadri_VEF::modif_normales_facettes_Cl, fa7 " << fa7 << "num_elem " << num_elem << "idirichlet " << idirichlet << "n1 " << n1 << "normales_facettes_Cl " << normales_facettes_Cl <<  finl;
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
