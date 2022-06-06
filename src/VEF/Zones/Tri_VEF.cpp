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

#include <Tri_VEF.h>
#include <Domaine.h>
#include <Zone_VEF.h>
#include <Champ_P1NC.h>

Implemente_instanciable_sans_constructeur(Tri_VEF,"Tri_VEF",Elem_VEF_base);

// printOn et readOn


Sortie& Tri_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Tri_VEF::readOn(Entree& s )
{
  return s ;
}

Tri_VEF::Tri_VEF()
{
  // Description:
  // KEL_(0,fa7),KEL_(1,fa7) sont  les numeros locaux des 2 faces
  // qui entourent la facette de numero local fa7
  int tmp[3][3]=
  {
    {1, 2, 0},
    {2, 0, 1},
    {0, 1, 2}
  };
  KEL_.resize(3,3);
  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      KEL_(i,j)=tmp[i][j];
}

// Description:
// remplit le tableau face_normales dans la Zone_VEF
void Tri_VEF::normale(int num_Face,DoubleTab& Face_normales,
                      const  IntTab& Face_sommets,
                      const IntTab& Face_voisins,
                      const IntTab& elem_faces,
                      const Zone& zone_geom) const
{
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();

  double x1,y1;
  double nx,ny;
  int no3;
  int f0;
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

  if ( (nx*x1+ny*y1) > 0 )
    {
      Face_normales(num_Face,0) = - nx;
      Face_normales(num_Face,1) = - ny;
    }
  else
    {
      Face_normales(num_Face,0) = nx;
      Face_normales(num_Face,1) = ny;
    }
}

// Description:
// calcule les normales des facettes pour des elem standards
void Tri_VEF::creer_facette_normales(const Zone& zone_geom,
                                     DoubleTab& facette_normales,
                                     const IntVect& rang_elem_non_std) const
{
  const DoubleTab& les_coords = zone_geom.domaine().coord_sommets();
  const IntTab& les_Polys = zone_geom.les_elems();
  int nb_elem = zone_geom.nb_elem();

  int i, fa7;
  int i0,i1;
  int num_som[3];
  double x[3][2];
  double xg[2];
  double xj0[2];
  double u[2];
  double v[2];
  double psc;

  if (facette_normales.get_md_vector() != zone_geom.md_vector_elements())
    {
      facette_normales.reset();
      facette_normales.resize(0,3,2);
      zone_geom.creer_tableau_elements(facette_normales);
    }

  for(i=0; i<nb_elem; i++)
    {
      if (rang_elem_non_std(i)==-1)
        {
          num_som[0]=les_Polys(i,0);
          num_som[1]=les_Polys(i,1);
          num_som[2]=les_Polys(i,2);
          x[0][0]=les_coords(num_som[0],0);
          x[0][1]=les_coords(num_som[0],1);
          x[1][0]=les_coords(num_som[1],0);
          x[1][1]=les_coords(num_som[1],1);
          x[2][0]=les_coords(num_som[2],0);
          x[2][1]=les_coords(num_som[2],1);
          xg[0]=(x[0][0]+x[1][0]+x[2][0])/3;
          xg[1]=(x[0][1]+x[1][1]+x[2][1])/3;
          for (fa7=0; fa7<3; fa7++)
            {
              // la fa7 a pour sommets fa7 et "G" de coordonnees xg
              u[0]= x[fa7][0]-xg[0];
              u[1]= x[fa7][1]-xg[1];
              v[0]= -u[1];
              v[1]= u[0];
              i0 = KEL_(0,fa7);
              i1 = KEL_(1,fa7);
              // Orientation des normales :
              xj0[0]= x[i0][0]-x[i1][0];
              xj0[1]= x[i0][1]-x[i1][1];
              psc=xj0[0]*v[0] + xj0[1]*v[1];
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
            }
        }
    }
  facette_normales.echange_espace_virtuel();
}

// Description:
// remplit le tableau normales_facettes_Cl dans la Zone_Cl_VEF
// pour la facette fa7 de l'element num_elem
void Tri_VEF::creer_normales_facettes_Cl(DoubleTab& normales_facettes_Cl,
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

  int i0 = KEL_(0,fa7);
  int i1 = KEL_(1,fa7);

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

}

// Description:
// modifie les volumes entrelaces pour la face j d'un elem
// non standard
void Tri_VEF::modif_volumes_entrelaces(int j,int elem,
                                       const Zone_VEF& la_zone_VEF,
                                       DoubleVect& volumes_entrelaces_Cl,
                                       int type_cl) const
{
  double surf_mod;
  const DoubleVect& volumes_entrelaces = la_zone_VEF.volumes_entrelaces();
  const IntTab& elem_faces = la_zone_VEF.elem_faces();

  switch(type_cl)
    {

      // pas de Face de Dirichlet : impossible
    case 0:
      {
        Cerr << "Tri_VEF::modif_volumes_entrelaces() type 0 impossible!\n";
        break;
      }

    case 1: // une Face de Dirichlet : Face 2
      {
        surf_mod = volumes_entrelaces[j]/2 ;
        volumes_entrelaces_Cl[elem_faces(elem,0)]  += surf_mod;
        volumes_entrelaces_Cl[elem_faces(elem,1)]  += surf_mod;
        break;
      }

    case 2: // une Face de Dirichlet : Face 1
      {
        surf_mod = volumes_entrelaces[j]/2 ;
        volumes_entrelaces_Cl[elem_faces(elem,0)]  += surf_mod;
        volumes_entrelaces_Cl[elem_faces(elem,2)]  += surf_mod;
        break;
      }

    case 4: // une Face de Dirichlet : Face 0
      {
        surf_mod = volumes_entrelaces[j]/2 ;
        volumes_entrelaces_Cl[elem_faces(elem,1)]  += surf_mod;
        volumes_entrelaces_Cl[elem_faces(elem,2)]  += surf_mod;
        break;
      }

    case 6: // deux Faces de Dirichlet : Faces 0,1
      {
        surf_mod = volumes_entrelaces[elem_faces(elem,0)]
                   +  volumes_entrelaces[elem_faces(elem,1)];
        volumes_entrelaces_Cl[elem_faces(elem,2)]  += surf_mod;
        break;
      }

    case 3: // deux Faces de Dirichlet : Faces 1,2
      {
        surf_mod = volumes_entrelaces[elem_faces(elem,2)]
                   +  volumes_entrelaces[elem_faces(elem,1)];
        volumes_entrelaces_Cl[elem_faces(elem,0)]  += surf_mod;
        break;
      }

    case 5: // deux Faces de Dirichlet : Faces 0,2
      {
        surf_mod = volumes_entrelaces[elem_faces(elem,0)]
                   +  volumes_entrelaces[elem_faces(elem,2)];
        volumes_entrelaces_Cl[elem_faces(elem,1)] += surf_mod;
        break;
      }

    default :
      {
        Cerr << "\n  type inconnu Tri_VEF::modif_volumes_entrelaces: " << type_cl ;
        exit();
      }

    } // fin du switch

}

// Description:
// modifie les volumes entrelaces pour la face joint j d'un elem
// non standard
void Tri_VEF::modif_volumes_entrelaces_faces_joints(int j,int elem,
                                                    const Zone_VEF& la_zone_VEF,
                                                    DoubleVect& volumes_entrelaces_Cl,
                                                    int type_cl) const
{
  double surf_mod;
  const DoubleVect& volumes_entrelaces = la_zone_VEF.volumes_entrelaces();
  const IntTab& elem_faces = la_zone_VEF.elem_faces();

  int face;
  int nb_faces_cl = volumes_entrelaces_Cl.size();
  switch(type_cl)
    {

      // pas de Face de Dirichlet : impossible
    case 0:
      {
        Cerr << "Tri_VEF::modif_volumes_entrelaces() type 0 impossible!\n";
        break;
      }

    case 1: // une Face de Dirichlet : Face 2
      {
        surf_mod = volumes_entrelaces[j]/2 ;
        face=elem_faces(elem,0);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += surf_mod;
        face=elem_faces(elem,1);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += surf_mod;
        break;
      }

    case 2: // une Face de Dirichlet : Face 1
      {
        surf_mod = volumes_entrelaces[j]/2 ;
        face=elem_faces(elem,0);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += surf_mod;
        face=elem_faces(elem,2);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += surf_mod;
        break;
      }

    case 4: // une Face de Dirichlet : Face 0
      {
        surf_mod = volumes_entrelaces[j]/2 ;
        face=elem_faces(elem,1);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += surf_mod;
        face=elem_faces(elem,2);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += surf_mod;
        break;
      }

    case 6: // deux Faces de Dirichlet : Faces 0,1
      {
        surf_mod = volumes_entrelaces[elem_faces(elem,0)]
                   +  volumes_entrelaces[elem_faces(elem,1)];
        face=elem_faces(elem,2);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += surf_mod;
        break;
      }

    case 3: // deux Faces de Dirichlet : Faces 1,2
      {
        surf_mod = volumes_entrelaces[elem_faces(elem,2)]
                   +  volumes_entrelaces[elem_faces(elem,1)];
        face=elem_faces(elem,0);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face]  += surf_mod;
        break;
      }

    case 5: // deux Faces de Dirichlet : Faces 0,2
      {
        surf_mod = volumes_entrelaces[elem_faces(elem,0)]
                   +  volumes_entrelaces[elem_faces(elem,2)];
        face=elem_faces(elem,1);
        if(face<nb_faces_cl)
          volumes_entrelaces_Cl[face] += surf_mod;
        break;
      }

    default :
      {
        Cerr << "\n  type inconnu Tri_VEF::modif_volumes_entrelaces: " << type_cl ;
        exit();
      }

    } // fin du switch

}

// Description:
//
void Tri_VEF::calcul_vc(const ArrOfInt& Face,ArrOfDouble& vc,
                        const ArrOfDouble& vs,const DoubleTab& vsom,
                        const Champ_Inc_base& vitesse,int type_cl, const DoubleVect& porosite_face) const
{
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
void Tri_VEF::calcul_xg(DoubleVect& xg, const DoubleTab& x,
                        const int type_elem_Cl,int& idirichlet,int& n1,int& ,int& ) const
{
  int dim=xg.size();
  switch(type_elem_Cl)
    {

    case 0:  // le triangle n'a pas de Face de dirichlet: il a 3 Facettes
      //  le point G est le barycentre des sommets du triangle
      {
        for (int j=0; j<dim; j++)
          xg[j]=(x(0,j)+x(1,j)+x(2,j))/3;

        idirichlet=0;
        break;
      }

    case 1:  // le triangle a une Face de dirichlet: la Face 2
      // le point G est le barycentre des sommets de la Face 2

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x(0,j)+x(1,j))/2;

        idirichlet=1;
        break;
      }

    case 2:  // le triangle a une Face de dirichlet: la Face 1
      // le point G est le barycentre des sommets de la Face 1

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x(0,j)+x(2,j))/2;

        idirichlet=1;
        break;
      }

    case 4:  // le triangle a une Face de dirichlet: la Face 0
      // le point G est le barycentre des sommets de la Face 0

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x(1,j)+x(2,j))/2;

        idirichlet=1;
        break;
      }

    case 6 : // le triangle a deux faces de Dirichlet : les faces 0,1
      // le point G est le sommet 2 du triangle

      {
        for (int j=0; j<dim; j++)
          xg[j]=x(2,j);

        idirichlet=2;
        n1 = 2;
        break;

      }

    case 5 : // le triangle a deux faces de Dirichlet : les faces 0,2
      // le point G est le sommet 1 du triangle

      {
        for (int j=0; j<dim; j++)
          xg[j]=x(1,j);

        idirichlet=2;
        n1 = 1;
        break;

      }

    case 3 : // le triangle a deux faces de Dirichlet : les faces 1,2
      // le point G est le sommet 0 du triangle

      {
        for (int j=0; j<dim; j++)
          xg[j]=x(0,j);

        idirichlet=2;
        n1 = 0;
        break;

      }

    } // fin du switch

}

// Description:
// modifie normales_facettes_Cl quand idirichlet=2
// idirichlet=nb de faces de Dirichlet de l'element
// si idirichlet=2, n1 est le numero du sommet confondu avec G
void Tri_VEF::modif_normales_facettes_Cl(DoubleTab& normales_facettes_Cl,
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
      {

        //            fa7=n1;
        //normales_facettes_Cl(num_elem,fa7,0) = 0;
        //normales_facettes_Cl(num_elem,fa7,1) = 0;
        // l'appel semble inutile en 2D
        normales_facettes_Cl(num_elem,n1,0) = 0;
        normales_facettes_Cl(num_elem,n1,1) = 0;
        break;
      }
    }
}
