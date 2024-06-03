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

#include <Faces.h>
#include <Faces2.h>
#include <DomaineAxi1d.h>
#include <communications.h>
#include <Linear_algebra_tools_impl.h>


Implemente_instanciable_sans_constructeur(Faces,"Faces",Objet_U);
Faces::Faces():nb_som_face(0) { }


/*! @brief Ecrit les faces sur un flots de sortie.
 *
 * On ecrit:
 *       - le type des faces
 *       - les sommets
 *       - les voisins
 *     On ecrit juste le type Faces::vide_0D, si le nombre de
 *     faces est nul.
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Faces::printOn(Sortie& s ) const
{
  if(nb_faces()==0)
    s << "vide_0D" << finl;
  else
    {
      s << (type(type_face_)) << finl;
      s <<  sommets;
      s << faces_voisins;
    }
  return s ;
}

/*! @brief Ecrit les faces sur un flots de sortie.
 *
 * On ecrit:
 *       - le type des faces
 *       - les sommets
 *       - les voisins
 *     On ecrit juste le type Faces::vide_0D, si le nombre de
 *     faces est nul.
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Faces::ecrit(Sortie& s ) const
{
  if(nb_faces()==0)
    s << "vide_0D" << finl;
  else
    {
      s << (type(type_face_)) << finl;
      sommets.ecrit(s);
      faces_voisins.ecrit(s);
    }
  return s ;
}

/*! @brief Lit les specifications d'un objet face a partir d'un flot d'entree.
 *
 *     On lit:
 *       - le type des faces
 *       - les sommets
 *       - les voisins
 *     Et on reordonne les sommets.
 *     Cree un objets Faces avec 0 faces si le type est "vide_0D"
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Faces::readOn(Entree& s)
{
  Motcle typ;
  s >> typ;
  typer(typ);
  if (typ != "vide_0D")
    {
      // Les sommets et faces ne sont ecrits que si le type n'est pas vide:
      s >> sommets;
      s >> faces_voisins;
    }
  else
    {
      // pour ne pas planter betement la suite qui suppose nb_dim()==2 et line_size_ > 0
      sommets.resize(0,1);
      faces_voisins.resize(0,2);
    }
  return s;
}


/*! @brief Lit les specifications d'un objet face a partir d'un flot d'entree.
 *
 *     On lit:
 *       - le type des faces
 *       - les sommets
 *       - les voisins
 *     Et on reordonne les sommets.
 *     Ne fait rien si le type lu est "vide_0D"
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Faces::lit(Entree& s)
{
  Motcle typ;
  s >> typ;
  if(typ!="vide_0D")
    {
      typer(typ);
      sommets.lit(s);
      faces_voisins.lit(s);
      reordonner();
    }
  else
    {
      typer(typ);
      sommets.resize(0,0);
      faces_voisins.resize(0,2);
    }
  return s ;
}

/*! @brief Renvoie un objet de Type_Face representant le type de nom specifie.
 *
 *     Les noms de types reconnus sont:
 *              "vide_0D"
 *       "point_1D"
 *       "point_1D_axi"
 *       "segment_2D"
 *       "triangle_3D"
 *       "quadrangle_3D"
 *       "segment_2D_axi"
 *       "quadrangle_3D_axi"
 *       "quadrilatere_2D_axi"
 *
 * @param (Motcle& mot) le mot clef representant un type de face
 * @return (Type_Face) le type de face correspondant au parametre mo
 * @throws type de face non reconnu
 */
Type_Face Faces::type(const Motcle& mot) const
{
  Motcles les_mots(10);
  {
    les_mots[0]="vide_0D";
    les_mots[1]="point_1D";
    les_mots[2]="point_1D_axi";
    les_mots[3]="segment_2D";
    les_mots[4]="triangle_3D";
    les_mots[5]="quadrangle_3D";
    les_mots[6]="segment_2D_axi";
    les_mots[7]="quadrangle_3D_axi";
    les_mots[8]="quadrilatere_2D_axi";
    les_mots[9]="polygone_3d";
  }
  int rang=les_mots.search(mot);
  switch(rang)
    {
    case  0 :
      return Faces::vide_0D;
    case  1 :
      return Faces::point_1D;
    case  2 :
      return Faces::point_1D_axi;
    case  3 :
      return Faces::segment_2D;
    case  4 :
      return Faces::triangle_3D;
    case  5 :
      return Faces::quadrangle_3D;
    case  6 :
      return Faces::segment_2D_axi;
    case  7 :
      return Faces::quadrangle_3D_axi;
    case  8 :
      return Faces::quadrilatere_2D_axi;
    case 9 :
      return Faces::polygone_3D;
    default :
      {
        Cerr << "In the area number " << Process::me() << " " << mot << " is not a type of face." << finl;
        Cerr << "Check your splitting." << finl;
        // Si mot est vide c'est que l'on tente de relire des Domaines creees avec une version anterieure a la 1.5.1
        if (mot=="")
          {
            Cerr << "Your splitting seems to have been done with a TRUST version 1.5 or earlier. Since" << finl;
            Cerr << "the 1.5.1, the files format of .Zones containing the splitting of your mesh has evolved." << finl;
            Cerr << "You must therefore rebuild the splitting of your mesh with TRUST version 1.5.1 or newer." <<
                 finl;
          }
        exit();
      }
    }
  // pour le compilo :
  return Faces::point_1D;
}

/*! @brief Renvoie le nom associe a un type de face.
 *
 * (inverse de Type_Face Faces::type(const Motcle& ) const)
 *
 * @param (Type_Face& typ) un type de face
 * @return (Motcle&) le nom correspondant au type speficie en parametre
 * @throws type de face non reconnu
 */
Motcle& Faces::type(const Type_Face& typ) const
{
  static Motcle mot;
  switch(typ)
    {
    case  Faces::vide_0D :
      mot="vide_0D";
      break;
    case  Faces::point_1D :
      mot="point_1D";
      break;
    case  Faces::point_1D_axi :
      mot="point_1D_axi";
      break;
    case  Faces::segment_2D :
      mot="segment_2D";
      break;
    case  Faces::segment_2D_axi :
      mot="segment_2D_axi";
      break;
    case  Faces::triangle_3D :
      mot="triangle_3D";
      break;
    case  Faces::quadrangle_3D :
      mot="quadrangle_3D";
      break;
    case  Faces::quadrangle_3D_axi :
      mot="quadrangle_3D_axi";
      break;
    case Faces::quadrilatere_2D_axi :
      mot="quadrilatere_2D_axi";
      break;
    case Faces::polygone_3D:
      mot="polygone_3D";
      break;
    default :
      {
        Cerr << "Error TRUST in Motcle& Faces::type(const Type_Face& typ)" << finl;
        exit();
      }
    }
  return mot;
}

/*! @brief Ajoute des faces.
 *
 * Ajouter des faces revient a ajouter des sommets.
 *
 * @param (IntTab& sommet) le tableau des sommets a ajouter
 * @throws les sommets specifies n'ont pas la bonne dimension (1D,2D,3D)
 */
void Faces::ajouter(const IntTab& tab_sommet)
{
  assert(sommets.dimension(1)==tab_sommet.dimension(1));
  int oldsz=sommets.dimension(0);
  int addsz=tab_sommet.dimension(0);
  dimensionner(oldsz+addsz);
  for(int i=0; i<addsz; i++)
    for(int j=0; j<tab_sommet.dimension(1); j++)
      sommets(oldsz+i,j)=tab_sommet(i,j);
}

/*! @brief (Re-)dimensionne les faces On redimensionne les voisins en consequence.
 *
 *     Les sommets implicitement ajoutes sont initialises a -1.
 *
 * @param (int i) le nouveau nombre de face
 * @return (int) le nouveau nombre de face
 */
int Faces::dimensionner(int i)
{
  int oldsz=nb_faces();
  int nombre_som_faces=nb_som_faces();
  if (sommets.size()!=0) sommets.detach_vect(); // Fixed bug: To have the possibility of merging borders after discretization
  sommets.resize(i,nombre_som_faces);
  faces_voisins.resize(i, 2);
  for(int j=oldsz; j<i; j++)
    {
      for(int k=0; k<nombre_som_faces; k++)
        sommets(j,k)=-1;
      faces_voisins(j,0)=faces_voisins(j,1)=-1;
    }
  return i;
}

/*! @brief Initialise les voisins des faces joints a -1
 *
 * @param (int nb_faces_joint) le nombre de faces representant des Joints
 */
void Faces::initialiser_faces_joint(int nb_faces_joint)
{
  int i;
  for(i=0; i<nb_faces_joint; i++)
    //      faces_voisins(i,0)=faces_voisins(i,1)=-2;
    faces_voisins(i,0)=faces_voisins(i,1)=-1;
}

/*! @brief Initialise les sommets des faces joints a -1
 *
 * @param (int nb_faces_joint) le nombre de faces representant des Joints
 */
void Faces::initialiser_sommets_faces_joint(int nb_faces_joint)
{
  int i;
  for(i=0; i<nb_faces_joint; i++)
    for(int k=0; k<nb_som_faces(); k++)
      //         sommets(i,k)=-2;
      sommets(i,k)=-1;
}

/*! @brief Type les faces
 *
 * @param (Motcle& typ) le type a donner aux faces
 */
void Faces::typer(const Motcle& typ)
{
  typer(type(typ));
}

/*! @brief Type les faces
 *
 * @param (Type_Face& typ) le type a donner aux faces
 * @throws type de face inconnu
 */
void Faces::typer(const Type_Face& typ)
{
  type_face_ = typ;
  // int axi_ = 0;
  int max_dim = 3;
  switch(typ)
    {
    case  Faces::vide_0D :
      nb_som_face=0;
      break;
    case  Faces::point_1D :
    case  Faces::point_1D_axi :
      nb_som_face=1;
      break;
    case  Faces::segment_2D :
      nb_som_face=2;
      break;
    case  Faces::segment_2D_axi :
      nb_som_face=2; //axi_=1;
      break;
    case  Faces::triangle_3D :
      nb_som_face=3;
      break;
    case  Faces::quadrangle_3D :
      nb_som_face=4;
      break;
    case  Faces::quadrangle_3D_axi :
      nb_som_face=4; //axi_=1;
      break;
    case  Faces::quadrilatere_2D_axi :
      nb_som_face=2; //axi_=1;
      break;
    case Faces::polygone_3D:
      nb_som_face=-1; // sera fixe plus tard
      break;
    default :
      {
        Cerr << "Error TRUST in Faces::typer(const Motcle& typ)" << finl;
        exit();
      }
    }
  /* TroisDto2D marche pas
     if (axi_==1 && bidim_axi!=1 && axi!=1)
     {
     Cerr << "The mesh contains faces that show that is a revolution mesh." << finl;
     Cerr << "Add Bidim_axi or Axi in your data file." << finl;
     exit();
     } */
  if (dimension<std::min(max_dim,nb_som_face))
    {
      Cerr << "You are in dimension " << dimension << finl;
      Cerr << "and the mesh contains faces with " << nb_som_face << " nodes." << finl;
      Cerr << "and this seems to be a mesh of dimension " << std::min(max_dim,nb_som_face) << " ..." << finl;
      exit();
    }
}


/*! @brief Complete la face specifie: met a jour ses voisins.
 *
 * @param (int face) l'indice de la face a completer
 * @param (int num_elem) le numero de l'element voisin de la face
 * @throws face deja complete
 */
void Faces::completer(int face, int num_elem)
{
  if ( voisin(face,0) == -1)
    voisin(face, 0) = num_elem;
  else if( voisin(face,1) == -1)
    voisin(face, 1) = num_elem;
  else
    {
      Cerr << finl;
      Cerr << "Problem for the face number " << face << " and the cell " << num_elem << finl;
      Cerr << "Indeed, the face already belongs to the cells " << voisin(face, 0) << " and " << voisin(face, 1) << finl;
      Cerr << "The nodes of this face are:" << finl;
      for (int i=0; i<nb_som_face; i++)
        Cerr << "Node " << sommet(face,i) << finl;
      Cerr << "Check your mesh. Perhaps some cells " << finl;
      Cerr << "are defined 2 times." << finl;
      exit();
    }
}

/*! @brief Calcule la surface des faces
 *
 * @param (DoubleVect& surfaces) le vecteur contenant la surface de chacune des faces.
 * @throws type de face non reconnu
 * @throws calcul de la surface de ce type de face non code
 * @throws calcul de surface errone (surface <= 0)
 * @throws type de face ne correspondant pas a la dimension d'espace
 */
void Faces::calculer_surfaces(DoubleVect& surfaces) const
{
  surfaces.resize(nb_faces_tot());
  const Domaine& dom=domaine();
  // Verification qu'en coordonnees cylindriques, r est bien positif
  if (axi || bidim_axi)
    {
      int nb_som = dom.les_sommets().dimension(0);
      for (int i=0; i<nb_som; i++)
        {
          if (dom.coord(i,0)<0)
            {
              Cerr << "In axisymmetric, the coordinates of the mesh according to radius" << finl;
              Cerr << "ie along X, can not be negatives." << finl;
              Cerr << "The revolution axis must be at x=0." << finl;
              exit();
            }
        }
    }
  switch(type_face_)
    {
    case Faces::segment_2D :
      {
        assert(dimension==2);
        double delta0, delta1;
        for(int face=0; face <nb_faces_tot(); face++)
          {
            delta0=(dom.coord(sommet(face ,0), 0) - dom.coord(sommet(face ,1), 0));
            delta0*=delta0;
            delta1=(dom.coord(sommet(face ,0), 1) - dom.coord(sommet(face ,1), 1));
            delta1*=delta1;
            surfaces(face)=sqrt(delta0+delta1);
            if(surfaces(face)==0.)
              {
                Cerr << "area("<<face<<")=0 ! Check your mesh." << finl;
                exit();
              }
          }
        break;
      }
    case Faces::quadrilatere_2D_axi :
      {
        assert(dimension==2);
        double r0,r1,z0,z1,delta_r,delta_z;
        for(int face=0; face <nb_faces_tot(); face++)
          {
            r0 = dom.coord(sommet(face ,0), 0);
            r1 = dom.coord(sommet(face ,1), 0);
            delta_r=std::fabs(r1-r0);
            if ( est_egal(delta_r,0) ) //  faces de direction r
              {
                z0 = dom.coord(sommet(face ,0), 1);
                z1 = dom.coord(sommet(face ,1), 1);
                delta_z=std::fabs(z1-z0);
                surfaces(face) = 2*M_PI*r0*delta_z;
              }
            else //face de direction z
              {
                surfaces(face) = 2*M_PI*0.5*(r1+r0)*delta_r;
              }
          }
        break;
      }

    case  Faces::segment_2D_axi :
      {
        assert(dimension==2);
        double r0,r1,teta0,teta1,d_teta;
        for(int face=0; face <nb_faces_tot(); face++)
          {
            r0 = dom.coord(sommet(face ,0), 0);
            r1 = dom.coord(sommet(face ,1), 0);
            if ( est_egal(r0,r1) ) // surface = r0*abs(teta1-teta0)
              {
                teta0 = dom.coord(sommet(face ,0), 1);
                teta1 = dom.coord(sommet(face ,1), 1);
                d_teta = std::fabs(teta1-teta0);
                if(d_teta > M_PI) d_teta=2.*M_PI-d_teta;
                surfaces(face)=r0*d_teta;
              }
            else //  surface = r1-r2
              surfaces(face)=std::fabs(r1-r0);
          }
        break;
      }
    case  Faces::triangle_3D :
      {
        assert(dimension==3);

        double delta0, delta1, delta2;
        double longueur0, longueur1;
        double prod,sa;

        for(int face=0; face <nb_faces_tot(); face++)
          {
            prod=0;
            delta0=(dom.coord(sommet(face ,1), 0) - dom.coord(sommet(face ,0), 0));
            delta1=(dom.coord(sommet(face ,1), 1) - dom.coord(sommet(face ,0), 1));
            delta2=(dom.coord(sommet(face ,1), 2) - dom.coord(sommet(face ,0), 2));
            longueur0=(delta0*delta0+delta1*delta1+delta2*delta2);
            sa=delta0;
            delta0=(dom.coord(sommet(face ,2), 0) - dom.coord(sommet(face ,0), 0));
            prod=sa*delta0;
            sa=delta1;
            delta1=(dom.coord(sommet(face ,2), 1) - dom.coord(sommet(face ,0), 1));
            prod+=sa*delta1;
            sa=delta2;
            delta2=(dom.coord(sommet(face ,2), 2) - dom.coord(sommet(face ,0), 2));
            prod+=sa*delta2;
            longueur1=(delta0*delta0+delta1*delta1+delta2*delta2);
            surfaces(face)=0.5*(sqrt(longueur0*longueur1-prod*prod));
            if(surfaces(face)==0.)
              {
                Cerr << "area("<<face<<")=0 ! Check your mesh." << finl;
                exit();
              }
          }
        break;
      }
    case  Faces::quadrangle_3D :
      {
        // On se base sur Hexa_VEF::normale():
        for(int face=0; face <nb_faces_tot(); face++)
          {
            int n0 = sommet(face, 0);
            int n1 = sommet(face, 1);
            int n2 = sommet(face, 2);
            int n3 = sommet(face, 3);
            // NB: Dans un prisme, il y'a aussi des triangles comme faces... Donc:
            if (n3<0) n3 = n2;

            double x1 = dom.coord(n0, 0) - dom.coord(n1, 0);
            double y1 = dom.coord(n0, 1) - dom.coord(n1, 1);
            double z1 = dom.coord(n0, 2) - dom.coord(n1, 2);

            double x2 = dom.coord(n3, 0) - dom.coord(n1, 0);
            double y2 = dom.coord(n3, 1) - dom.coord(n1, 1);
            double z2 = dom.coord(n3, 2) - dom.coord(n1, 2);

            double nx = (y1*z2 - y2*z1)/2;
            double ny = (-x1*z2 + x2*z1)/2;
            double nz = (x1*y2 - x2*y1)/2;

            x1 = dom.coord(n0,0) - dom.coord(n2,0);
            y1 = dom.coord(n0,1) - dom.coord(n2,1);
            z1 = dom.coord(n0,2) - dom.coord(n2,2);

            x2 = dom.coord(n3,0) - dom.coord(n2,0);
            y2 = dom.coord(n3,1) - dom.coord(n2,1);
            z2 = dom.coord(n3,2) - dom.coord(n2,2);

            nx -= (y1*z2 - y2*z1)/2;
            ny -= (-x1*z2 + x2*z1)/2;
            nz -= (x1*y2 - x2*y1)/2;

            surfaces(face)=sqrt(nx*nx+ny*ny+nz*nz);
          }
        break;
      }
    case Faces::quadrangle_3D_axi :
      {
        assert(dimension==3);
        double r0,r1,teta0,teta1,teta2,z0,z2,d_teta,delta_r;
        for(int face=0; face <nb_faces_tot(); face++)
          {
            r0 = dom.coord(sommet(face ,0), 0);
            r1 = dom.coord(sommet(face ,1), 0);
            delta_r = std::fabs(r1 - r0);
            if ( est_egal(r0,r1) )
              {
                teta0 = dom.coord(sommet(face ,0), 1);
                teta1 = dom.coord(sommet(face ,1), 1);
                z0 = dom.coord(sommet(face ,0), 2);
                z2 = dom.coord(sommet(face ,2), 2);
                d_teta = std::fabs(teta1-teta0);
                if(d_teta > M_PI) d_teta=2.*M_PI-d_teta;
                surfaces(face)=r0*d_teta*std::fabs(z2-z0);
              }
            else
              {
                teta0 = dom.coord(sommet(face ,0), 1);
                teta2 = dom.coord(sommet(face ,2), 1);
                if (teta0 == teta2)
                  {
                    z0 = dom.coord(sommet(face ,0), 2);
                    z2 = dom.coord(sommet(face ,2), 2);
                    surfaces(face) = delta_r*std::fabs(z2-z0);
                  }
                else
                  {
                    d_teta=std::fabs(teta2-teta0);
                    if(d_teta > M_PI) d_teta=2.*M_PI-d_teta;
                    surfaces(face) = 0.5*(r0+r1)*d_teta*delta_r;
                  }
              }
          }
        break;
      }
    case Faces::point_1D:
    case Faces::vide_0D :
      {
        for(int face=0; face <nb_faces_tot(); face++)
          surfaces(face) = 1.0;

        break;
      }
    case Faces::point_1D_axi:
      {
        assert(dimension==3);

        const DomaineAxi1d& domax = ref_cast(DomaineAxi1d,dom);

        for(int face=0; face <nb_faces_tot(); face++)
          {
            int elem = voisin(face,0)==-1 ? voisin(face,1) : voisin(face,0);

            double x0 = domax.origine_repere(elem,0);
            double y0 = domax.origine_repere(elem,1);
            double x = dom.coord(sommet(face ,0), 0);
            double y = dom.coord(sommet(face ,0), 1);

            double r = sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0));
            surfaces(face) = 2.*M_PI*r;
          }
        break;
      }
    case Faces::polygone_3D:
      {
        const DoubleTab& coord=dom.coord_sommets();
        int nmax=les_sommets().dimension(1);
        for(int face=0; face <nb_faces_tot(); face++)
          {
            double n0=0,n1=0,n2=0;
            int n=nmax-1;
            while (n >= 0 && sommet(face,n)==-1) n--;
            for (int i0=0; i0<=n; i0++)
              {

                int ip1_0=0;
                if (i0<n) ip1_0=i0+1;
                int i=sommet(face,i0);
                int ip1=sommet(face,ip1_0);
                n0+=coord(i,1)*coord(ip1,2)-coord(i,2)*coord(ip1,1);
                n1+=coord(i,2)*coord(ip1,0)-coord(i,0)*coord(ip1,2);
                n2+=coord(i,0)*coord(ip1,1)-coord(i,1)*coord(ip1,0);
              }
            surfaces(face)=sqrt(n0*n0+n1*n1+n2*n2)/2.;
          }
        break;
      }
    default :
      {
        Cerr << "Error TRUST in type of Faces not recognized " << finl;
        exit();
      }
    }
}


/*! @brief Calcule les centres de gravite de chaque face.
 *
 * @param (DoubleTab& xv) tableau contenant les coordonnees des centres de gravite de chaque face. xv(i,j) contient la coordonnee j du centre de gravite de la i-ieme face. On resize le tableau xv et on lui affecte le descripteur parallele du tableau des sommets avant de le remplir.
 */
void Faces::calculer_centres_gravite(DoubleTab& xv) const
{
  // Le tableau xv est dimensionne dans ::calculer_centres_gravite
  const Domaine& dom=domaine();
  const DoubleTab& coord=dom.coord_sommets();
  ::calculer_centres_gravite(xv, type_face_,
                             coord, sommets);
}


void calculer_centres_gravite(DoubleTab& xv,
                              Type_Face type_face_,
                              const DoubleTab& coord,
                              const IntTab& sommet)
{
  int nb_faces_tot = sommet.dimension_tot(0);
  int dim = coord.dimension(1);
  xv.resize(nb_faces_tot, dim);
  // Copie le descripteur parallele du tableau sommet:
  xv.set_md_vector(sommet.get_md_vector());

  if(nb_faces_tot!=0)
    {
      int nb_som_faces=sommet.dimension(1);
      double inv=1./nb_som_faces;
      int k, som;
      xv = 0;
      if(Objet_U::axi)
        {
          switch(type_face_)
            {
            case  Faces::segment_2D_axi :
              {
                for (int face=0; face<nb_faces_tot; face++)
                  {
                    for(som=0; som < nb_som_faces; som++)
                      xv(face, 0)+=coord(sommet(face ,som), 0);
                    double teta_0=coord(sommet(face ,0), 1);
                    double teta_1=coord(sommet(face ,1), 1);
                    double teta_min=std::min(teta_1, teta_0);
                    double teta_max=std::max(teta_1, teta_0);
                    double d_teta=teta_max-teta_min;
                    if( (teta_min==0.) && (d_teta>M_PI) )
                      {
                        teta_min+=2.*M_PI;
                      }
                    {
                      xv(face, 1)+=(teta_min+teta_max);
                    }
                  }
                break;
              }
            case Faces::quadrangle_3D_axi :
              {
                for (int face=0; face<nb_faces_tot; face++)
                  {
                    for(som=0; som < nb_som_faces; som++)
                      xv(face, 0)+=coord(sommet(face ,som), 0);
                    for(som=0; som < nb_som_faces; som++)
                      xv(face, 2)+=coord(sommet(face ,som), 2);
                    double teta_0=coord(sommet(face ,0), 1);
                    double teta_1=coord(sommet(face ,1), 1);
                    double teta_2=coord(sommet(face ,2), 1);
                    double teta_3=coord(sommet(face ,3), 1);
                    double teta_min=std::min(teta_1, teta_0);
                    teta_min=std::min(teta_min, teta_2);
                    teta_min=std::min(teta_min, teta_3);
                    double teta_max=std::max(teta_1, teta_0);
                    teta_max=std::max(teta_min, teta_2);
                    teta_max=std::max(teta_min, teta_3);
                    double d_teta=teta_max-teta_min;
                    if( (teta_min==0.) && (d_teta>M_PI) )
                      {
                        if(teta_0==0.) teta_0+=2.*M_PI;
                        if(teta_1==0.) teta_1+=2.*M_PI;
                        if(teta_2==0.) teta_2+=2.*M_PI;
                        if(teta_3==0.) teta_3+=2.*M_PI;
                      }
                    {
                      xv(face, 1)+=(teta_0+teta_1+teta_2+teta_3) ;
                    }
                  }
                break;
              }
            default:
              {
                Cerr << "Face type number " << (int)type_face_ << " not provided in Faces::calculer_centres_gravite" << finl;
                break;
              }
            }
          xv*=inv;
        }
      else
        {
          for (int face=0; face<nb_faces_tot; face++)
            {
              int nb_som=nb_som_faces;
              while (sommet(face,nb_som-1)==-1) nb_som--;

              inv=1./nb_som;
              for(k=0; k<dim; k++)
                for(som=0; som < nb_som; som++)
                  xv(face, k)+=coord(sommet(face ,som), k);
              for(k=0; k<dim; k++)
                xv(face, k)*=inv;
            }
        }
    }
}

/*! @brief Reordonne les faces.
 *
 * (on ne reordonne que les quadrangles)
 *
 * @throws type de face non reconnu
 */
void Faces::reordonner()
{
  //Cerr << "Faces::reordonner()" << finl;
  switch(type_face_)
    {
    case  Faces::point_1D :
    case  Faces::point_1D_axi :
      {
        break;
      }
    case  Faces::segment_2D :
      {
        // on peut avoir des bords de maillage triangulaire en 3D
        assert(dimension>=2);
        break;
      }
    case Faces::quadrilatere_2D_axi :
      {
        assert(dimension==2);
        const Domaine& madomaine=domaine();
        const Domaine& dom=madomaine;
        ArrOfInt S(2);
        ArrOfInt NS(2);
        int i;
        const int nombre_faces=nb_faces();
        for(int face=0; face < nombre_faces; face++)
          {
            NS=-1;
            for(i=0; i<2; i++)
              S[i] = sommet(face, i);
            assert( S[0] >=0 );
            assert( S[1] >=0 );
            if (dom.coord(S[0], 0) > dom.coord(S[1], 0))
              {
                NS[0]=S[1];
                NS[1]=S[0];
              }
            else
              {
                NS[0]=S[0];
                NS[1]=S[1];
              }

            for(i=0; i<2; i++)
              {
                assert(NS[i]!=-1);
                sommet(face, i)=NS[i];
              }
          }
        break;
      }
    case  Faces::segment_2D_axi :
      {
        assert(dimension==2);
        break;
      }
    case  Faces::triangle_3D :
      {
        assert(dimension==3);
        break;
      }
    case  Faces::quadrangle_3D :
      {
        assert(dimension==3);
        const Domaine& madomaine=domaine();
        const Domaine& dom=madomaine;
        ArrOfInt S(4);
        ArrOfInt NS(4);
        int i;
        double xmin, ymin, zmin;
        double xmax, ymax, zmax;
        const int nombre_faces=nb_faces();
        for(int face=0; face < nombre_faces; face++)
          {
            NS=-1;
            for(i=0; i<4; i++)
              {
                S[i] = sommet(face, i);
                assert( S[i] >=0 );
              }
            if (1)
              {
                // adaptation de Hexaedre_VEF::reordonne
                const DoubleTab& coord=dom.les_sommets();
                int s0=S[0];
                int s1=S[1];
                int s2=S[2];
                int s3=S[3];
                double x03=coord(s3,0)-coord(s0,0);
                double y03=coord(s3,1)-coord(s0,1);
                double z03=coord(s3,2)-coord(s0,2);
                double x02=coord(s2,0)-coord(s0,0);
                double y02=coord(s2,1)-coord(s0,1);
                double z02=coord(s2,2)-coord(s0,2);

                double x01=coord(s1,0)-coord(s0,0);
                double y01=coord(s1,1)-coord(s0,1);
                double z01=coord(s1,2)-coord(s0,2);

                Vecteur3 OA(x01,y01,z01);
                Vecteur3 OB(x03,y03,z03);
                Vecteur3 OC(x02,y02,z02);

                Vecteur3 nplan, nmedian;
                // rectangle C B
                //           O A
                // calcul de n= OA ^ OC
                Vecteur3::produit_vectoriel(OA,OC,nplan);
                // calcul de la normale nmedian du plan (OB,n)
                Vecteur3::produit_vectoriel(OB,nplan,nmedian);

                // on regarde si les points A et C sont bien de part et d'autre du plan
                double psC=Vecteur3::produit_scalaire(nmedian,OA);
                double psA=Vecteur3::produit_scalaire(nmedian,OC);

                if (psA*psC>0)
                  {
                    // inversion des sommets 2 et 3
                    sommet(face,2)=S[3];
                    sommet(face,3)=S[2];
                    Cerr << "Permutation of local nodes 2 and 3 on the face " <<face<<finl;

                  }
              }
            else
              {

                xmin=std::min(dom.coord(S[0], 0), dom.coord(S[1], 0));
                xmin=std::min(xmin, dom.coord(S[2], 0));
                xmax=std::max(dom.coord(S[0], 0), dom.coord(S[1], 0));
                xmax=std::max(xmax, dom.coord(S[2], 0));
                ymin=std::min(dom.coord(S[0], 1), dom.coord(S[1], 1));
                ymin=std::min(ymin, dom.coord(S[2], 1));
                ymax=std::max(dom.coord(S[0], 1), dom.coord(S[1], 1));
                ymax=std::max(ymax, dom.coord(S[2], 1));
                zmin=std::min(dom.coord(S[0], 2), dom.coord(S[1], 2));
                zmin=std::min(zmin, dom.coord(S[2], 2));
                zmax=std::max(dom.coord(S[0], 2), dom.coord(S[1], 2));
                zmax=std::max(zmax, dom.coord(S[2], 2));

                if(est_egal(zmin, zmax))
                  {
                    for(i=0; i<4; i++)
                      if ( est_egal(dom.coord(S[i], 0),xmin) && est_egal(dom.coord(S[i], 1),ymin))
                        NS[0]=S[i];
                    for(i=0; i<4; i++)
                      if ( !est_egal(dom.coord(S[i], 0),xmin) && est_egal(dom.coord(S[i], 1),ymin))
                        NS[1]=S[i];
                    for(i=0; i<4; i++)
                      if ( est_egal(dom.coord(S[i], 0),xmin) && !est_egal(dom.coord(S[i], 1),ymin))
                        NS[2]=S[i];
                    for(i=0; i<4; i++)
                      if ( !est_egal(dom.coord(S[i], 0),xmin) && !est_egal(dom.coord(S[i], 1),ymin))
                        NS[3]=S[i];
                  }
                if(est_egal(ymin, ymax))
                  {
                    for(i=0; i<4; i++)
                      if ( est_egal(dom.coord(S[i], 0),xmin) && est_egal(dom.coord(S[i], 2),zmin))
                        NS[0]=S[i];
                    for(i=0; i<4; i++)
                      if ( !est_egal(dom.coord(S[i], 0),xmin) && est_egal(dom.coord(S[i], 2),zmin))
                        NS[1]=S[i];
                    for(i=0; i<4; i++)
                      if ( est_egal(dom.coord(S[i], 0),xmin) && !est_egal(dom.coord(S[i], 2),zmin))
                        NS[2]=S[i];
                    for(i=0; i<4; i++)
                      if ( !est_egal(dom.coord(S[i], 0),xmin) && !est_egal(dom.coord(S[i], 2),zmin))
                        NS[3]=S[i];
                  }
                if(est_egal(xmin, xmax))
                  {
                    for(i=0; i<4; i++)
                      if ( est_egal(dom.coord(S[i], 1),ymin) && est_egal(dom.coord(S[i], 2),zmin))
                        NS[0]=S[i];
                    for(i=0; i<4; i++)
                      if ( !est_egal(dom.coord(S[i], 1),ymin) && est_egal(dom.coord(S[i], 2),zmin))
                        NS[1]=S[i];
                    for(i=0; i<4; i++)
                      if ( est_egal(dom.coord(S[i], 1),ymin) && !est_egal(dom.coord(S[i], 2),zmin))
                        NS[2]=S[i];
                    for(i=0; i<4; i++)
                      if ( !est_egal(dom.coord(S[i], 1),ymin) && !est_egal(dom.coord(S[i], 2),zmin))
                        NS[3]=S[i];
                  }

                for(i=0; i<4; i++)
                  {
                    assert(NS[i]!=-1);
                    sommet(face, i)=NS[i];
                  }
              }
          }
        break;
      }
    case Faces::quadrangle_3D_axi :
      {
        assert(dimension==3);
        break;
      }
    case Faces::polygone_3D:
      assert(dimension==3);
      break;
    default :
      {
        Cerr << "Error TRUST in type of Faces not recognized " << finl;
        exit();
      }
    }
//  Cerr << "Faces::reordonner() ok" << finl;
}

/*! @brief Compare l'objet Faces passe en parametre avec l'objet Faces lui-meme (this)
 *
 * @param (Faces& faces) les faces avec lesquel on compare l'objet
 * @param (IntVect& renum) le vecteur renumerotation - renum est de taille 1 et renum contient -1 si il n'y pas le meme nombre de face dans l'objet que dans le parametre faces. - renum autant d'elements que de nombre de face sinon. et renum(i) = le rang de la face i du parametre faces dans l'objet courant
 * @return (IntVect&)
 */
IntVect& Faces::compare(const Faces& faces, IntVect& renum)
{
  const Domaine& le_dom=domaine();
  const Domaine& sa_domaine=faces.domaine();
  const Domaine& domaine=le_dom;
  const Domaine& son_domaine=sa_domaine;
  if ( (nb_faces() != faces.nb_faces()) || ( type_face_ != faces.type_face_) )
    {
      renum.resize(1);
      renum=-1;
      return renum;
    }
  if(domaine.le_nom() == son_domaine.le_nom())
    {
      // il suffit de comparer les numeros des sommets :
      IntLists listes;
      int premier=0;
      int numerol, face, ok, rang;
      for(face=0; face<nb_faces(); face++)
        {
          numerol=ppsf(*this, face, nb_som_face);
          listes[numerol].add(premier++);
        }
      for(face=0; face<nb_faces(); face++)
        {
          numerol=ppsf(faces, face, nb_som_face);
          IntList_Curseur curseur(listes[numerol]);
          ok=1;
          while (curseur && ok)
            {
              rang=curseur.valeur();
              ok=!same(faces, face, *this, rang, nb_som_face);
              if(!ok)
                {
                  // "face==rang"
                  renum(face)=rang;
                }
              else
                ++curseur;
            }
          if(!curseur)
            {
              renum.resize(1);
              renum=-1;
              return renum;
            }
        }
    }
  else
    {
      // il faut comparer les coordonnees des sommets :
    }
  return renum;
}


