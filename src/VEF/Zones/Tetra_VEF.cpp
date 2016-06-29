/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Tetra_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Zones
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Tetra_VEF.h>
#include <Domaine.h>
#include <Zone_VEF.h>
#include <Champ_P1NC.h>

Implemente_instanciable_sans_constructeur(Tetra_VEF,"Tetra_VEF",Elem_VEF_base);

// printOn et readOn


Sortie& Tetra_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Tetra_VEF::readOn(Entree& s )
{
  return s ;
}
// Description:
// renvoie pour la facette fa7 :
// pour j=0,j=1 : les numeros locaux des 2 faces qui entourent fa7
// pour j=2,j=3 : les numeros locaux des sommets du tetraedre qui
//                appartiennent a fa7
Tetra_VEF::Tetra_VEF()
{
  int tmp[4][6]=
  {
    {0, 0, 0, 1, 1, 2},
    {1, 2, 3, 2, 3, 3},
    {2, 1, 1, 3, 2, 1},
    {3, 3, 2, 0, 0, 0}
  };
  KEL_.resize(4,6);
  for (int i=0; i<4; i++)
    for (int j=0; j<6; j++)
      KEL_(i,j)=tmp[i][j];
}

void Tetra_VEF::normale(int num_Face,DoubleTab& Face_normales,
                        const IntTab& Face_sommets,
                        const IntTab& Face_voisins,
                        const IntTab& elem_faces,
                        const Zone& zone_geom) const
{

  //Cerr << " num_Face " << num_Face << finl;
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();

  // Cerr << "les face sommet " << Face_sommets << finl;
  double x1,y1,z1,x2,y2,z2;
  double nx,ny,nz;
  int f0,no4;

  int n0 = Face_sommets(num_Face,0);
  int n1 = Face_sommets(num_Face,1);
  int n2 = Face_sommets(num_Face,2);


  x1 = les_coords(n0,0) - les_coords(n1,0);
  y1 = les_coords(n0,1) - les_coords(n1,1);
  z1 = les_coords(n0,2) - les_coords(n1,2);

  x2 = les_coords(n2,0) - les_coords(n1,0);
  y2 = les_coords(n2,1) - les_coords(n1,1);
  z2 = les_coords(n2,2) - les_coords(n1,2);

  nx = (y1*z2 - y2*z1)/2;
  ny = (-x1*z2 + x2*z1)/2;
  nz = (x1*y2 - x2*y1)/2;
  // Cerr << "nx " << nx << " ny " << ny << " nz " << nz << finl;

  // Orientation de la normale de elem1 vers elem2
  // pour cela recherche du sommet de elem1 qui n'est pas sur la Face
  int elem1 = Face_voisins(num_Face,0);
  if ( (f0 = elem_faces(elem1,0)) == num_Face )
    f0 = elem_faces(elem1,1);

  if ( (no4 = Face_sommets(f0,0)) != n0    &&   no4 != n1
       &&   no4 != n2)
    ;
  else if ( (no4 = Face_sommets(f0,1)) != n0 && no4 != n1
            && no4 != n2 )
    ;
  else
    no4 = Face_sommets(f0,2);

  x1 = les_coords(no4,0) - les_coords(n0,0);
  y1 = les_coords(no4,1) - les_coords(n0,1);
  z1 = les_coords(no4,2) - les_coords(n0,2);

  if ( (nx*x1+ny*y1+nz*z1) > 0 )
    {
      Face_normales(num_Face,0) = - nx;
      Face_normales(num_Face,1) = - ny;
      Face_normales(num_Face,2) = - nz;
    }
  else
    {
      Face_normales(num_Face,0) = nx;
      Face_normales(num_Face,1) = ny;
      Face_normales(num_Face,2) = nz;
    }

  // Cerr << "Face_normales " << Face_normales << finl;

}

// Description:
// remplit le tableau face_normales dans la Zone_VEF
void Tetra_VEF::creer_facette_normales(const Zone& zone_geom,
                                       DoubleTab& facette_normales,
                                       const IntVect& rang_elem_non_std) const
{
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();
  const IntTab& les_Polys = zone_geom.les_elems();
  int nb_elem = zone_geom.nb_elem();

  int i,fa7;
  int num_som[4];
  double x[4][3];
  double xg[3];
  double xj0[3];
  double u[3];
  double v[3];
  double psc;
  double pv[3];

  facette_normales.resize(0,6,3);
  // valgrind with MPICH says a unitialised here so we initialize:
  // zone_geom.creer_tableau_elements(facette_normales, Array_base::NOCOPY_NOINIT);
  zone_geom.creer_tableau_elements(facette_normales);

  for(i=0; i<nb_elem; i++)
    {
      if (rang_elem_non_std(i)==-1)
        {
          num_som[0]=les_Polys(i,0);
          num_som[1]=les_Polys(i,1);
          num_som[2]=les_Polys(i,2);
          num_som[3]=les_Polys(i,3);
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
          xg[0]=0.25*(x[0][0]+x[1][0]+x[2][0]+x[3][0]);
          xg[1]=0.25*(x[0][1]+x[1][1]+x[2][1]+x[3][1]);
          xg[2]=0.25*(x[0][2]+x[1][2]+x[2][2]+x[3][2]);
          for (fa7=0; fa7<6; fa7++)
            {
              // la fa7 a pour sommets KEL_(2,fa7), KEL_(3,fa7), "G"
              u[0]=x[KEL_(2,fa7)][0]-xg[0];
              u[1]=x[KEL_(2,fa7)][1]-xg[1];
              u[2]=x[KEL_(2,fa7)][2]-xg[2];
              v[0]=x[KEL_(3,fa7)][0]-xg[0];
              v[1]=x[KEL_(3,fa7)][1]-xg[1];
              v[2]=x[KEL_(3,fa7)][2]-xg[2];
              prodvect(u,v,pv);
              // Orientation des normales :
              xj0[0]= x[KEL_(0,fa7)][0]-x[KEL_(1,fa7)][0];
              xj0[1]= x[KEL_(0,fa7)][1]-x[KEL_(1,fa7)][1];
              xj0[2]= x[KEL_(0,fa7)][2]-x[KEL_(1,fa7)][2];
              psc=xj0[0]*pv[0] + xj0[1]*pv[1] + xj0[2]*pv[2] ;
              if (psc < 0)
                {
                  facette_normales(i,fa7,0) = -pv[0]/2;
                  facette_normales(i,fa7,1) = -pv[1]/2;
                  facette_normales(i,fa7,2) = -pv[2]/2;
                }
              else
                {
                  facette_normales(i,fa7,0)  = pv[0]/2;
                  facette_normales(i,fa7,1)= pv[1]/2;
                  facette_normales(i,fa7,2)= pv[2]/2;
                }
            }
        }
    }
  facette_normales.echange_espace_virtuel();
}


// Description:
// remplit le tableau normales_facettes_Cl dans la Zone_Cl_VEF
// pour la facette fa7 de l'element num_elem
void Tetra_VEF::creer_normales_facettes_Cl(DoubleTab& normales_facettes_Cl,
                                           int fa7,
                                           int num_elem,const DoubleTab& x,
                                           const DoubleVect& xg, const Zone& zone_geom) const
{
  // x contient les coord des sommets du tetraedre
  // xg contient les coord du "centre" du tetraedre
  double u[3];
  double v[3];
  double xj0[3];
  double psc;
  // Cerr << "fa7 " << fa7 << " et le num_elem " << num_elem << finl;
  int i0 = KEL_(0,fa7);
  int i1 = KEL_(1,fa7);
  int i2 = KEL_(2,fa7);
  int i3 = KEL_(3,fa7);
  //  Cerr << "i0 " << i0 << " i1 " << i1 << " i2 " << i2 << " i3 " << i3 << finl;
  u[0]=x(i2,0)-xg[0];
  u[1]=x(i2,1)-xg[1];
  u[2]=x(i2,2)-xg[2];
  v[0]=x(i3,0)-xg[0];
  v[1]=x(i3,1)-xg[1];
  v[2]=x(i3,2)-xg[2];
  double pv[3];
  prodvect(u,v,pv);

  // Orientation des normales :
  xj0[0]= x(i0,0)-x(i1,0);
  xj0[1]= x(i0,1)-x(i1,1);
  xj0[2]= x(i0,2)-x(i1,2);

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


void Tetra_VEF::modif_volumes_entrelaces(int j,int elem,
                                         const Zone_VEF& la_zone_VEF,
                                         DoubleVect& volumes_entrelaces_Cl,
                                         int type_cl) const
{
  double vol_mod;
  const DoubleVect& volumes_entrelaces = la_zone_VEF.volumes_entrelaces();
  const IntTab& elem_faces = la_zone_VEF.elem_faces();

  switch(type_cl)
    {

      // pas de Face de Dirichlet : impossible
    case 0:
      {
        Cerr << "Tetra_VEF::modif_volumes_entrelaces() type 0 impossible!\n";
        break;
      }

    case 1: // une Face de Dirichlet : Face 3
      {
        vol_mod = volumes_entrelaces[j]/3 ;
        volumes_entrelaces_Cl[elem_faces(elem,0)]  += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,1)]  += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,2)]  += vol_mod;
        break;
      }

    case 2: // une Face de Dirichlet : Face 2
      {
        vol_mod = volumes_entrelaces[j]/3 ;
        volumes_entrelaces_Cl[elem_faces(elem,0)]  += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,1)]  += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,3)]  += vol_mod;
        break;
      }

    case 4: // une Face de Dirichlet : Face 1
      {
        vol_mod = volumes_entrelaces[j]/3 ;
        volumes_entrelaces_Cl[elem_faces(elem,0)]  += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,2)]  += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,3)]  += vol_mod;
        break;
      }

    case 8: // une Face de Dirichlet : Face 0
      {
        vol_mod = volumes_entrelaces[j]/3 ;
        volumes_entrelaces_Cl[elem_faces(elem,1)] += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,2)] += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,3)] += vol_mod;
        break;
      }

    case 3: // deux faces de Dirichlet : faces 2,3
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,2)]
                   + volumes_entrelaces[elem_faces(elem,3)])/2;
        volumes_entrelaces_Cl[elem_faces(elem,0)] += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,1)] += vol_mod;
        break;
      }

    case 5: // deux faces de Dirichlet : faces 1,3
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,1)]
                   + volumes_entrelaces[elem_faces(elem,3)])/2;
        volumes_entrelaces_Cl[elem_faces(elem,0)] += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,2)] += vol_mod;
        break;
      }

    case 6: // le tetraedre a deux faces de Dirichlet : faces 1,2
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,1)]
                   + volumes_entrelaces[elem_faces(elem,2)])/2;
        volumes_entrelaces_Cl[elem_faces(elem,0)] += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,3)] += vol_mod;
        break;
      }

    case 9: // deux faces de Dirichlet :faces 0,3
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,3)])/2;
        volumes_entrelaces_Cl[elem_faces(elem,1)] += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,2)] += vol_mod;
        break;
      }

    case 10: // deux faces de Dirichlet :faces 0,2
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,2)])/2;
        volumes_entrelaces_Cl[elem_faces(elem,1)] += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,3)] += vol_mod;
        break;
      }

    case 12: // deux faces de Dirichlet :faces 0,1
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,1)])/2;
        volumes_entrelaces_Cl[elem_faces(elem,2)] += vol_mod;
        volumes_entrelaces_Cl[elem_faces(elem,3)] += vol_mod;
        break;
      }



    case 7:  // trois faces de Dirichlet : faces 1,2,3
      {
        vol_mod =  volumes_entrelaces[elem_faces(elem,1)]
                   + volumes_entrelaces[elem_faces(elem,2)]
                   + volumes_entrelaces[elem_faces(elem,3)];
        volumes_entrelaces_Cl[elem_faces(elem,0)] += vol_mod;
        break;
      }

    case 11: // trois faces de Dirichlet : faces 0,2,3
      {
        vol_mod =  volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,2)]
                   + volumes_entrelaces[elem_faces(elem,3)];
        volumes_entrelaces_Cl[elem_faces(elem,1)] += vol_mod;
        break;
      }

    case 13: // trois faces de Dirichlet : faces 0,1,3
      {

        vol_mod =  volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,1)]
                   + volumes_entrelaces[elem_faces(elem,3)];
        volumes_entrelaces_Cl[elem_faces(elem,2)] += vol_mod;
        break;
      }

    case 14: // trois faces de Dirichlet : faces 0,1,2
      {
        vol_mod =  volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,1)]
                   + volumes_entrelaces[elem_faces(elem,2)];
        volumes_entrelaces_Cl[elem_faces(elem,3)] += vol_mod;
        break;
      }
    default :
      {
        Cerr << "\n  type inconnu Tetra_VEF::modif_volumes_entrelaces: " << type_cl;
        exit();
      }

    } // fin du switch
}

void Tetra_VEF::modif_volumes_entrelaces_faces_joints(int j,int elem,
                                                      const Zone_VEF& la_zone_VEF,
                                                      DoubleVect& volumes_entrelaces_Cl,
                                                      int type_cl) const
{
  double vol_mod;
  const DoubleVect& volumes_entrelaces = la_zone_VEF.volumes_entrelaces();
  const IntTab& elem_faces = la_zone_VEF.elem_faces();

  int face;
  int nb_faces_cl = volumes_entrelaces_Cl.size();
  switch(type_cl)
    {

      // pas de Face de Dirichlet : impossible
    case 0:
      {
        Cerr << "Tetra_VEF::modif_volumes_entrelaces() type 0 impossible!\n";
        break;
      }

    case 1: // une Face de Dirichlet : Face 3
      {
        vol_mod = volumes_entrelaces[j]/3 ;
        face=elem_faces(elem,0);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,1);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,2);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 2: // une Face de Dirichlet : Face 2
      {
        vol_mod = volumes_entrelaces[j]/3 ;
        face=elem_faces(elem,0);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,1);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,3);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 4: // une Face de Dirichlet : Face 1
      {
        vol_mod = volumes_entrelaces[j]/3 ;
        face=elem_faces(elem,0);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,2);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,3);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 8: // une Face de Dirichlet : Face 0
      {
        vol_mod = volumes_entrelaces[j]/3 ;
        face=elem_faces(elem,1);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,2);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,3);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 3: // deux faces de Dirichlet : faces 2,3
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,2)]
                   + volumes_entrelaces[elem_faces(elem,3)])/2;
        face=elem_faces(elem,0);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,1);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 5: // deux faces de Dirichlet : faces 1,3
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,1)]
                   + volumes_entrelaces[elem_faces(elem,3)])/2;
        face=elem_faces(elem,0);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,2);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 6: // le tetraedre a deux faces de Dirichlet : faces 1,2
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,1)]
                   + volumes_entrelaces[elem_faces(elem,2)])/2;
        face=elem_faces(elem,0);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,3);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 9: // deux faces de Dirichlet :faces 0,3
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,3)])/2;
        face=elem_faces(elem,1);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,2);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 10: // deux faces de Dirichlet :faces 0,2
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,2)])/2;
        face=elem_faces(elem,1);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,3);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 12: // deux faces de Dirichlet :faces 0,1
      {
        vol_mod = (volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,1)])/2;
        face=elem_faces(elem,2);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        face=elem_faces(elem,3);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }



    case 7:  // trois faces de Dirichlet : faces 1,2,3
      {
        vol_mod =  volumes_entrelaces[elem_faces(elem,1)]
                   + volumes_entrelaces[elem_faces(elem,2)]
                   + volumes_entrelaces[elem_faces(elem,3)];
        face=elem_faces(elem,0);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 11: // trois faces de Dirichlet : faces 0,2,3
      {
        vol_mod =  volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,2)]
                   + volumes_entrelaces[elem_faces(elem,3)];
        face=elem_faces(elem,1);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 13: // trois faces de Dirichlet : faces 0,1,3
      {

        vol_mod =  volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,1)]
                   + volumes_entrelaces[elem_faces(elem,3)];
        face=elem_faces(elem,2);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }

    case 14: // trois faces de Dirichlet : faces 0,1,2
      {
        vol_mod =  volumes_entrelaces[elem_faces(elem,0)]
                   + volumes_entrelaces[elem_faces(elem,1)]
                   + volumes_entrelaces[elem_faces(elem,2)];
        face=elem_faces(elem,3);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += vol_mod;
        break;
      }
    default :
      {
        Cerr << "\n  type inconnu Tetra_VEF::modif_volumes_entrelaces: " << type_cl;
        exit();
      }

    } // fin du switch
}

void Tetra_VEF::calcul_vc(const ArrOfInt& Face,ArrOfDouble& vc,
                          const ArrOfDouble& vs,const DoubleTab& vsom,
                          const Champ_Inc_base& vitesse,int type_cl, const DoubleVect& porosite_face) const
{
  int comp;
  switch(type_cl)
    {

    case 0: // le tetraedre n'a pas de Face de Dirichlet
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.25*vs[comp];
        break;
      }

    case 1: // le tetraedre a une Face de Dirichlet : KEL3
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vitesse(Face[3],comp)*porosite_face(Face[3]);
        break;
      }

    case 2: // le tetraedre a une Face de Dirichlet : KEL2
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vitesse(Face[2],comp)*porosite_face(Face[2]);
        break;
      }

    case 4: // le tetraedre a une Face de Dirichlet : KEL1
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vitesse(Face[1],comp)*porosite_face(Face[1]);
        break;
      }

    case 8: // le tetraedre a une Face de Dirichlet : KEL0
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vitesse(Face[0],comp)*porosite_face(Face[0]);
        break;
      }

    case 3: // le tetraedre a deux faces de Dirichlet : KEL3 et KEL2
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5* (vsom(0,comp) + vsom(1,comp));
        break;
      }

    case 5: // le tetraedre a deux faces de Dirichlet : KEL3 et KEL1
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5* (vsom(0,comp) + vsom(2,comp));
        break;
      }

    case 6: // le tetraedre a deux faces de Dirichlet : KEL1 et KEL2
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5* (vsom(0,comp) + vsom(3,comp));
        break;
      }

    case 9: // le tetraedre a deux faces de Dirichlet : KEL0 et KEL3
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5* (vsom(1,comp) + vsom(2,comp));
        break;
      }

    case 10: // le tetraedre a deux faces de Dirichlet : KEL0 et KEL2
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5* (vsom(1,comp) + vsom(3,comp));
        break;
      }

    case 12: // le tetraedre a deux faces de Dirichlet : KEL0 et KEL1
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5*(vsom(2,comp) + vsom(3,comp));
        break;
      }

    case 7: // le tetraedre a trois faces de Dirichlet : KEL1, KEL2 et KEL3
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vsom(0,comp);
        break;
      }

    case 11: // le tetraedre a trois faces de Dirichlet : KEL0,KEL2 et KEL3
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vsom(1,comp);
        break;
      }

    case 13: // le tetraedre a trois faces de Dirichlet : KEL0, KEL1 et KEL3
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vsom(2,comp);
        break;
      }

    case 14: // le tetraedre a trois faces de Dirichlet : KEL0, KEL1 et KEL2
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vsom(3,comp);
        break;
      }

    } // fin du switch
}

// Description:
// calcule les coord xg du centre d'un element non standard
// calcule aussi idirichlet=nb de faces de Dirichlet de l'element
void Tetra_VEF::calcul_xg(DoubleVect& xg,const DoubleTab& x, const int type_elem_Cl,
                          int& idirichlet,int& n1,int& n2,int& n3) const
{
  int dim=xg.size();

  switch(type_elem_Cl)
    {

    case 0:  // le tetraedre n'a pas de Face de Dirichlet. Il a 6 Facettes
      {
        for (int j=0; j<dim; j++)
          xg[j]=0.25*(x(0,j)+x(1,j)+x(2,j)+x(3,j));

        idirichlet=0;
        break;
      }

    case 1:  // le tetraedre a une Face de Dirichlet.  Le 'centre'
      // du tetraedre est au milieu de la Face 3 qui a pour sommets
      // 0, 1, 2
      // Il a 3 Facettes reelles : 0   aux noeuds 2 3 xg
      //                           1   aux noeuds 1 3 xg
      //                           3   aux noeuds 3 0 xg
      // les 3 autres Facettes sont sur la Face 3

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x(0,j)+x(1,j)+x(2,j))/3.;

        idirichlet=1;
        break;

      }

    case 2:  // le tetraedre a une Face de Dirichlet.  Le 'centre'
      // du tetraedre est au milieu de la Face 2 qui a pour sommets
      // 0, 1, 3
      // Il a 3 Facettes reelles : 0   aux noeuds 2 3 xg
      //                           2   aux noeuds 1 2 xg
      //                           4   aux noeuds 2 0 xg

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x(0,j)+x(1,j)+x(3,j))/3.;

        idirichlet=1;
        break;
      }

    case 4:  // le tetraedre a une Face de Dirichlet.  Le 'centre'
      // du tetraedre est au milieu de la Face 1 qui a pour sommets
      // 0, 2, 3
      // Il a 3 Facettes reelles : 1   aux noeuds 1 3 xg
      //                           2   aux noeuds 1 2 xg
      //                           5   aux noeuds 1 0 xg

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x(0,j)+x(2,j)+x(3,j))/3.;

        idirichlet=1;
        break;
      }

    case 8:  // le tetraedre a une Face de Dirichlet.  Le 'centre'
      // du tetraedre est au milieu de la Face 0 qui a pour sommets
      // 1, 2, 3
      // Il a 3 Facettes reelles : 3   aux noeuds 3 0 xg
      //                           4   aux noeuds 2 0 xg
      //                           5   aux noeuds 1 0 xg

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x(1,j)+x(2,j)+x(3,j))/3.;

        idirichlet=1;
        break;
      }

    case 3:  // le tetraedre a deux faces de Dirichlet 2 et 3. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour extremites 0, 1.
      // Il a 1 Facette nulle: 5

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x(0,j)+x(1,j));

        n1=5;
        idirichlet=2;
        break;
      }


    case 5:  // le tetraedre a deux faces de Dirichlet 3 et 1. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour extremites 0, 2.
      // Il a 1 Facette  nulle  : 4

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x(0,j)+x(2,j));

        n1=4;
        idirichlet=2;
        break;
      }

    case 6:  // le tetraedre a deux faces de Dirichlet 1 et 2. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour extremites 0, 3.
      // Il a 1 Facette  nulle: 3

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x(0,j)+x(3,j));

        n1=3;
        idirichlet=2;
        break;
      }

    case 9:  // le tetraedre a deux faces de Dirichlet 0 et 3. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour extremites 1, 2
      // Il a 1 Facette  nulle: 2

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x(1,j)+x(2,j));

        n1=2;
        idirichlet=2;
        break;
      }

    case 10:  // le tetraedre a deux faces de Dirichlet 0 et 2. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour extremites 1, 3.
      // Il a 1 Facette  nulle: 1

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x(1,j)+x(3,j));

        n1=1;
        idirichlet=2;
        break;
      }


    case 12:  // le tetraedre a deux faces de Dirichlet 0 et 1. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour sommets 2, 3.
      // Il a 1 Facette  nulle

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x(2,j)+x(3,j));

        n1=0;
        idirichlet=2;
        break;
      }

    case 7:  // trois faces de Dirichlet : 1,2,3. Le centre est au sommet 0
      // il y 3 Facettes nulles: 3,4,5

      {
        for (int j=0; j<dim; j++)
          xg[j]= x(0,j);

        n1=3;
        n2=4;
        n3=5;
        idirichlet=3;
        break;

      }

    case 11:  // trois faces de Dirichlet : 0,2,3. Le centre est au sommet 1
      // il y 3 Facettes nulles: 1, 2, 5

      {
        for (int j=0; j<dim; j++)
          xg[j]= x(1,j);

        n1=1;
        n2=2;
        n3=5;
        idirichlet=3;
        break;

      }

    case 13:  // trois faces de Dirichlet : 0,1,3. Le centre est au sommet 2
      // il y 3 Facettes nulles: 0, 2, 4

      {
        for (int j=0; j<dim; j++)
          xg[j]= x(2,j);

        n1=0;
        n2=2;
        n3=4;
        idirichlet=3;
        break;

      }
    case 14:  // trois faces de Dirichlet : 0,1,2. Le centre est au sommet 3
      // il y 3 Facettes nulles: 0, 1, 3

      {
        for (int j=0; j<dim; j++)
          xg[j]= x(3,j);

        n1=0;
        n2=1;
        n3=3;
        idirichlet=3;
        break;

      }
    }
}


void Tetra_VEF::modif_normales_facettes_Cl(DoubleTab& normales_facettes_Cl,
                                           int fa7,int num_elem,int idirichlet,
                                           int n1,int n2,int n3) const
{
  switch (idirichlet)
    {

    case 0:
      break;

    case 1:
      break;

    case 2:  // une facette nulle n1
      {
        fa7=n1;
        normales_facettes_Cl(num_elem,fa7,0) = 0;
        normales_facettes_Cl(num_elem,fa7,1) = 0;
        normales_facettes_Cl(num_elem,fa7,2) = 0;
        break;
      }

    case 3:
      {
        fa7=n1;
        normales_facettes_Cl(num_elem,fa7,0) = 0;
        normales_facettes_Cl(num_elem,fa7,1) = 0;
        normales_facettes_Cl(num_elem,fa7,2) = 0;
        fa7=n2;
        normales_facettes_Cl(num_elem,fa7,0) = 0;
        normales_facettes_Cl(num_elem,fa7,1) = 0;
        normales_facettes_Cl(num_elem,fa7,2) = 0;
        fa7=n3;
        normales_facettes_Cl(num_elem,fa7,0) = 0;
        normales_facettes_Cl(num_elem,fa7,1) = 0;
        normales_facettes_Cl(num_elem,fa7,2) = 0;
        break;
      }

    }  //fin du switch
}



