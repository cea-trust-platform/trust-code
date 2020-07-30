/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Polyedriser.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#include <Polyedriser.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Polyedre.h>
#include <vector>



// Convention de numerotation
//    sommets         faces         4(face z=1)
//      6------7            *------*
//     /|     /|           /| 3   /|
//    2------3 |          *------* |
//    | |    | |          |0|    |1|
//    | 4----|-5          | *----|-*
//    |/     |/           |/  2  |/
//    0------1            *------*
//                       5(face z=0)

static int faces_sommets_poly[6][4] =
{
  { 0, 4, 6, 2 },
  { 1, 5, 7, 3 },
  { 0, 2, 3, 1 },
  { 4, 6, 7, 5 },
  { 2, 6, 7, 3 },
  { 0, 4, 5, 1 }
};

Implemente_instanciable(Polyedriser,"Polyedriser",Interprete_geometrique_base);
// XD polyedriser interprete polyedriser -1 cast hexahedra into polyhedra so that the indexing of the mesh vertices is compatible with PolyMAC discretization. Must be used in PolyMAC discretization if a hexahedral mesh has been produced with TRUST's internal mesh generator.
// XD attr domain_name ref_domaine domain_name 0 Name of domain.

Sortie& Polyedriser::printOn(Sortie& os) const
{
  return os;
}

Entree& Polyedriser::readOn(Entree& is)
{
  return is;
}


// compute the angle between the vectors u and v (which are coplanar)
static double computeAngleBetweenCoplanarVectors(std::vector<double> u, std::vector<double> v,
                                                 std::vector<double> normale)
{
  double dot = u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
  double det =   u[0]*v[1]*normale[2]
                 +   v[0]*normale[1]*u[2]
                 +   normale[0]*u[1]*v[2]
                 -   u[2]* v[1] *normale[0]
                 -   v[2]* normale[1] *u[0]
                 -   normale[2]* u[1] *v[0];

  double angle = atan2(det, dot);
  if(det == 0.0)
    {
      int dir = 0;
      while(v[dir] == 0.0) dir++;
      double quotient = u[dir] / v[dir];
      if( quotient > 0)
        angle = 0.0;
      else if( quotient < 0 )
        angle = M_PI;
    }
  else if(det < 0)
    angle += 2*M_PI;

  return angle;
}

// reordering vertices inside the faces in trigonometric order
// (or anti-trigonometric, depending on the orientation of the normal)
static void reorder_vertices(Faces& faces, DoubleTab coords)
{
  int nb_sommets = faces.les_sommets().dimension(1);
  int nb_faces = faces.nb_faces();
  for(int face = 0; face < nb_faces; face++)
    {
      // computing the center of gravity of the face
      std::vector<double> center(3,0.0);
      for(int s=0; s < nb_sommets; s++)
        for(int dir=0; dir<3; dir++)
          center[dir] += coords(faces.sommet(face,s),dir);
      for(int dir=0; dir<3; dir++)
        center[dir] /= nb_sommets;

      // computing the normal to the face
      int s0 = faces.sommet(face, 0);
      int s1 = faces.sommet(face, 1);
      int s2 = faces.sommet(face, 2);
      std::vector<double> u =
      {
        coords(s1,0) - coords(s0,0),
        coords(s1,1) - coords(s0,1),
        coords(s1,2) - coords(s0,2),
      };
      std::vector<double> v =
      {
        coords(s2,0) - coords(s0,0),
        coords(s2,1) - coords(s0,1),
        coords(s2,2) - coords(s0,2),
      };
      std::vector<double> normale =
      {
        u[1]*v[2] - u[2]*v[1] ,
        u[2]*v[0] - u[0]*v[2] ,
        u[0]*v[1] - u[1]*v[0] ,
      };
      double normale_norm = sqrt( normale[0]*normale[0] + normale[1]*normale[1] + normale[2]*normale[2] );
      for(int dir=0; dir<3; dir++)  normale[dir] /= normale_norm;

      //reference vector used to compute angles
      std::vector<double> vec_ref = { coords(faces.sommet(face,0),0) - center[0],
                                      coords(faces.sommet(face,0),1) - center[1],
                                      coords(faces.sommet(face,0),2) - center[2]
                                    };

      //sorting the vertices in the face according to their angle formed with the reference vector
      //in ascending order
      for (int i = 0; i < nb_sommets-1; i++)
        {
          for (int j = 0; j < nb_sommets-i-1; j++)
            {
              std::vector<double> vec1 = {coords(faces.sommet(face,j),0)-center[0],
                                          coords(faces.sommet(face,j),1)-center[1],
                                          coords(faces.sommet(face,j),2)-center[2]
                                         };
              std::vector<double> vec2 = {coords(faces.sommet(face,j+1),0)-center[0],
                                          coords(faces.sommet(face,j+1),1)-center[1],
                                          coords(faces.sommet(face,j+1),2)-center[2]
                                         };
              double angle1 = computeAngleBetweenCoplanarVectors(vec1, vec_ref, normale);
              double angle2 = computeAngleBetweenCoplanarVectors(vec2, vec_ref, normale);

              if (angle1 > angle2)
                {
                  int tmp = faces.sommet(face,j);
                  faces.sommet(face, j) = faces.sommet(face,j+1);
                  faces.sommet(face, j+1) = tmp;
                }
            }
        }
    }
  return;
}

void Polyedriser::polyedriser(Zone& zone) const
{
  if(zone.type_elem()->que_suis_je() == "Hexaedre")
    {
      zone.typer("Polyedre");
      Polyedre& p = ref_cast(Polyedre, zone.type_elem().valeur());

      ArrOfInt Pi, Fi, N;
      const IntTab& elements = zone.les_elems();
      int nb_elems = elements.dimension(0);
      Pi.resize_array(nb_elems+1);
      Fi.resize_array(nb_elems*6+1);
      N.resize_array(nb_elems*24);

      int face = 0;
      int node = 0;
      for (int e = 0; e < nb_elems; e++)
        {
          Pi(e) = face; // Index des polyedres

          for(int f=0; f<6; f++)
            {
              Fi(face) = face*4; // Index des faces:
              for(int s=0; s<4; s++)
                {
                  int som_loc = faces_sommets_poly[f][s];
                  N(node) = elements(e, som_loc);
                  node++;
                }
              face++;
            }
        }
      Fi(nb_elems*6) = node;
      Pi(nb_elems) = face;
      p.affecte_connectivite_numero_global(N, Fi, Pi, zone.les_elems());
    }
  else
    {
      Cerr << "We do not yet know how to Polyedriser the "
           << zone.type_elem()->que_suis_je() <<"s"<<finl;
      exit();
    }

  const DoubleTab coords = zone.domaine().coord_sommets();
  {
    LIST_CURSEUR(Bord) curseur(zone.faces_bord());
    while(curseur)
      {
        Faces& les_faces=curseur->faces();
        les_faces.typer(Faces::polygone_3D);
        reorder_vertices(les_faces, coords);
        ++curseur;
      }
  }

  {
    // Les Raccords
    LIST_CURSEUR(Raccord) curseur(zone.faces_raccord());
    while(curseur)
      {
        Faces& les_faces=curseur.valeur()->faces();
        les_faces.typer(Faces::polygone_3D);
        reorder_vertices(les_faces, coords);
        ++curseur;
      }
  }
  {
    // Les Faces internes
    LIST_CURSEUR(Faces_Interne) curseur(zone.faces_int());
    while(curseur)
      {
        Faces& les_faces=curseur->faces();
        les_faces.typer(Faces::polygone_3D);
        reorder_vertices(les_faces, coords);
        ++curseur;
      }
  }
  return;
}

Entree& Polyedriser::interpreter_(Entree& is)
{
  if(dimension !=dimension_application())
    {
      Cerr << "Interpreter "<<que_suis_je()<<" cannot be applied for dimension " << dimension <<finl;
      exit();
    }
  if (Process::nproc()>1)
    {
      Cerr << "Interpreter "<<que_suis_je()<<" cannot be applied during a parallel calculation !" << finl;
      Cerr << "The mesh can't be changed after it has been partitioned." << finl;
      Process::exit();
    }

  associer_domaine(is);
  Scatter::uninit_sequential_domain(domaine());
  for(int i=0; i<domaine().nb_zones() ; i++)
    {
      polyedriser(domaine().zone(i));
    }
  Scatter::init_sequential_domain(domaine());
  return is;
}

