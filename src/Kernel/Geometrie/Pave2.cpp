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

#include <Pave.h>
#include <math.h>


// Description:
//    Lit les specifications d'une frontiere du jeu de donnee
//    a partir d'un flot d'entree et la construit.
//    Format:
//    nom_front X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: Frontiere& front
//    Signification: la frontiere lue
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception: mot clef "X" attendu
// Exception: mot clef "=" attendu
// Exception: extremite en X invalide
// Exception: mot clef "X" ou "Y" attendu
// Exception: mot clef "<=" attendu
// Exception: mot clef "Y" attendu
// Exception: extremite en Y invalide
// Exception: il n'y a pas de bord en teta, vous avez maille
//            une couronne complete
// Exception: mot clef "X" ou "Y" ou "Z" attendu
// Exception: mot clef "Z" attendu
// Exception: extremite en Z invalide
// Effets de bord:
// Postcondition:
void Pave::lire_front(Entree& is, Frontiere& front)
{
  int i, j, k;
  Nom nom_front;
  is >> nom_front;
  Cerr << "Reading of the boundary " << nom_front << finl;
  front.nommer(nom_front);
  front.typer_faces(elem.type_face());
  int internes=sub_type(Faces_Interne, front);
  if(dimension==1)
    {
      Nom X, egal;
      double coupe;
      is  >> X >> egal >> coupe ;
      if( X!="X")
        {
          Cerr << "We expected a = X instead of" << X  <<
               " before " << egal << " " << coupe << finl;
          exit();
        }
      if( egal!="=")
        {
          Cerr << "We expected a = after " << X << finl;
          exit();
        }
      front.dimensionner(0);
      IntTab som(1,1);
      if ( (coupe != Origine(0)) && (coupe != (Origine(0)+Longueurs(0))) )
        {
          Cerr << coupe << " is not an extremity" << finl;
          Cerr << "The extremities are : "
               << Origine(0) << " " << Origine(0)+Longueurs(0) << finl;
          exit();
        }
      if(std::fabs(coupe - Origine(0))<epsilon())
        som(0,0)=0;
      else
        som(0,0)=Mx;
      front.ajouter_faces(som);
      front.associer_zone(*this);
    }
  else if(dimension==2)
    {
      Nom X, Y, egal, infegal1, infegal2;
      double coupe;
      double xmin, xmax;
      is  >> X >> egal >> coupe ;
      if( (X!="X") && (X!="Y") )
        {
          Cerr << "We expected a = X or Y instead of" << X <<
               " before " << egal << " " << coupe << finl;
          exit();
        }
      if( egal!="=")
        {
          Cerr << "We expected a = after " << X << finl;
          exit();
        }
      is >> xmin >> infegal1 >> Y >> infegal2 >> xmax;
      if( infegal1!="<=")
        {
          Cerr << "We expected a <= after " << xmin << " and not " << infegal1 << finl;
          exit();
        }
      if (X=="X")
        {
          if( Y!="Y" )
            {
              Cerr << "We expected a Y after " << infegal1 << finl;
              exit();
            }
          if (axi)
            {
              double deux_pi=M_PI*2.0 ;
              xmin*=deux_pi;
              xmax*=deux_pi;
            }
        }
      else
        {
          assert(tour_complet!=-123);
          if(tour_complet)
            {
              Cerr << "There is no boundary in teta! " << finl;
              Cerr << "You have meshed a complete crown! " << finl;
              exit();
            }
          if( Y!="X" )
            {
              Cerr << "We expected a X after " << infegal1 << finl;
              exit();
            }
        }
      if( infegal2!="<=")
        {
          Cerr << "We expected a <= after " << Y << " and not " << infegal2 <<finl;
          exit();
        }
      front.dimensionner(0);
      IntTab som;
      if (X=="X")
        {
          if ( (std::fabs(Origine(0) - coupe)>epsilon()) &&
               (std::fabs(coupe - Origine(0)-Longueurs(0))>epsilon()) && (!internes))
            {
              Cerr << "X = " << coupe << " is not a boundary" << finl;
              exit();
            }
          if ( ((std::fabs(Origine(0) - coupe)<epsilon()) ||
                (std::fabs(coupe - Origine(0)-Longueurs(0))<epsilon())) && (internes))
            {
              Cerr << "X = " << coupe << " is a boundary" << finl;
              exit();
            }
          int jmin, jmax;
          if (std::fabs(Origine(0) - coupe)<epsilon())
            i=0;
          else
            i=Nx;
          if(internes)
            for(i=0; coord_noeud(i,0,0)+epsilon()<coupe; i++) {};
          jmin=0;
          for(; coord_noeud(0,jmin,1)+epsilon()<xmin; jmin++) {};
          if(std::fabs(xmax-(Origine(1)+Longueurs(1)))<epsilon())
            {
              assert(tour_complet!=-123);
              jmax=My-1+tour_complet;
            }
          else
            {
              jmax = jmin;
              for(; coord_noeud(0,jmax,1)+epsilon()<xmax; jmax++) {};
              //            if(jmax-jmin==0)
              //            {
              //               Cerr << "le bord de nom " << nom_front << "est vide !" << finl;
              //               exit();
              //            }
            }
          som.resize(jmax-jmin,2);
          for(j=jmin; j<jmax; j++)
            {
              som(j-jmin,0)=numero_sommet(i, j);
              som(j-jmin,1)=numero_sommet(i, j+1);
            }
        }
      else
        {
          if (axi)
            {
              double deux_pi=M_PI*2.0 ;
              coupe*=deux_pi;
            }
          if ( (std::fabs(Origine(1) - coupe)>epsilon()) &&
               (std::fabs(coupe - Origine(1)-Longueurs(1))>epsilon()) && (!internes))
            {
              Cerr << "Y = " << coupe << " is not a boundary" << finl;
              exit();
            }
          if ( ((std::fabs(Origine(1) - coupe)<epsilon()) ||
                (std::fabs(coupe - Origine(1)-Longueurs(1))<epsilon())) && (internes))
            {
              Cerr << "Y = " << coupe <<" is a boundary" << finl;
              exit();
            }
          int imin, imax;
          if (std::fabs(coupe-Origine(1))<epsilon())
            j=0;
          else
            j=Ny;
          if(internes)
            for(j=0; coord_noeud(0,j,1)+epsilon()<coupe; j++) {};
          imin=0;
          for(; coord_noeud(imin,0,0)+epsilon()<xmin; imin++) {};
          if(std::fabs(xmax-(Origine(0)+Longueurs(0)))<epsilon())
            imax=Mx-1;
          else
            {
              imax = imin;
              for(; coord_noeud(imax,0,0)+epsilon()<xmax; imax++) {};
              //            if(imax-imin==0)
              //            {
              //               Cerr << "le bord de nom " << nom_front << "est vide !" << finl;
              //               exit();
              //            }
            }
          som.resize(imax-imin,2);
          for(i=imin; i<imax; i++)
            {
              som(i-imin,0)=numero_sommet(i, j);
              som(i-imin,1)=numero_sommet(i+1, j);
            }
        }
      front.ajouter_faces(som);
      front.associer_zone(*this);
    }
  else if(dimension==3)
    {
      Nom  X, Y, Z, egal, infegal1, infegal2, infegal3, infegal4;
      double coupe;
      double xmin, xmax, ymin, ymax;
      is  >> X >> egal >> coupe ;
      if( (X!="X") && (X!="Y") && (X!="Z"))
        {
          Cerr << "We expected a = X or Y or Z instead of" << X  <<
               " before " << egal << " " << coupe << finl;
          exit();
        }
      if( egal!="=")
        {
          Cerr << "We expected a = after " << X << finl;
          exit();
        }
      is >> xmin >> infegal1 >> Y >> infegal2 >> xmax;
      if( infegal1!="<=")
        {
          Cerr << "We expected a <= after " << xmin << " and not " << infegal1 <<finl;
          exit();
        }
      if( infegal2!="<=")
        {
          Cerr << "We expected a <= after " << Y << " and not " << infegal2 << finl;
          exit();
        }
      is >> ymin >> infegal3 >> Z >> infegal4 >> ymax;
      if( infegal3!="<=")
        {
          Cerr << "We expected a <= after " << ymin << " and not " << infegal3 << finl;
          exit();
        }
      if( infegal4!="<=")
        {
          Cerr << "We expected a <= after " << Z << " and not " << infegal4 << finl;
          exit();
        }
      if (X=="X")
        {
          if( Y!="Y" )
            {
              Cerr << "We expected a Y and not " << Y  << finl;
              exit();
            }
          if (axi)
            {
              double deux_pi=M_PI*2.0 ;
              xmin*=deux_pi;
              xmax*=deux_pi;
            }
          if( Z!="Z")
            {
              Cerr << "We expected a Z and not " << Z  << finl;
              exit();
            }
        }
      else if(X=="Y")
        {
          assert(tour_complet!=-123);
          if(tour_complet)
            {
              Cerr << "There is no boundary in teta! " << finl;
              Cerr << "You have meshed a complete crown! " << finl;
              exit();
            }
          if( Y!="X" )
            {
              Cerr << "We expected a X and not " << Y  << finl;
              exit();
            }
          if( Z!="Z")
            {
              Cerr << "We expected a Z and not " << Z  << finl;
              exit();
            }
        }
      else
        {
          if( Y!="X" )
            {
              Cerr << "We expected a X and not " << Y  << finl;
              exit();
            }
          if( Z!="Y" )
            {
              Cerr << "We expected a Z and not " << Z  << finl;
              exit();
            }
          if (axi)
            {
              double deux_pi=M_PI*2.0 ;
              ymin*=deux_pi;
              ymax*=deux_pi;
            }
        }
      front.dimensionner(0);
      IntTab som;
      if (X=="X")
        {
          if ( (std::fabs(Origine(0)-coupe)>epsilon()) &&
               (std::fabs(Origine(0)+Longueurs(0)-coupe)>epsilon()) && (!internes) )
            {
              Cerr << "X = " << coupe << " is not a boundary " << finl;
              exit();
            }
          if ( ((std::fabs(Origine(0)-coupe)<epsilon()) ||
                (std::fabs(Origine(0)+Longueurs(0)-coupe)<epsilon())) && (internes) )
            {
              Cerr << "X = " << coupe << " is a boundary " << finl;
              exit();
            }
          int jmin, jmax, kmin, kmax;
          if (std::fabs(Origine(0)-coupe)<epsilon())
            i=0;
          else
            i=Nx;
          if(internes)
            for(i=0; coord_noeud(i,0,0,0)+epsilon()<coupe; i++) {};
          jmin=kmin=0;
          for(; coord_noeud(0,jmin,0,1)+epsilon()<xmin; jmin++) {};
          for(; coord_noeud(0,0,kmin,2)+epsilon()<ymin; kmin++) {};
          jmax = jmin;
          kmax=kmin;
          if(std::fabs(xmax-(Origine(1)+Longueurs(1)))<epsilon())
            {
              assert(tour_complet!=-123);
              jmax=My-1+tour_complet;
            }
          else
            for(; coord_noeud(0,(jmax),0,1)+epsilon()<xmax; jmax++) {};
          for(; coord_noeud(0,0,(kmax),2)+epsilon()<ymax; kmax++) {};
          //         if((jmax-jmin)*(kmax-kmin)==0)
          //         {
          //            Cerr << "le bord de nom " << nom_front << "est vide !" << finl;
          //            exit();
          //         }
          som.resize((jmax-jmin)*(kmax-kmin),4);
          for(j=jmin; j<jmax; j++)
            for(k=kmin; k<kmax; k++)
              {
                som((k-kmin)*(jmax-jmin)+j-jmin,0)=numero_sommet(i, j, k);
                som((k-kmin)*(jmax-jmin)+j-jmin,1)=numero_sommet(i, j+1, k);
                som((k-kmin)*(jmax-jmin)+j-jmin,2)=numero_sommet(i, j, k+1);
                som((k-kmin)*(jmax-jmin)+j-jmin,3)=numero_sommet(i, j+1, k+1);
              }
        }
      else if(X=="Y")
        {
          if (axi)
            {
              double deux_pi=M_PI*2.0 ;
              coupe*=deux_pi;
            }
          if ( (std::fabs(Origine(1) - coupe)>epsilon()) &&
               (std::fabs(coupe - Origine(1)-Longueurs(1))>epsilon()) && (!internes)        )
            {
              Cerr << "Y = " << coupe << " is not a boundary " << finl;
              exit();
            }
          if ( ((std::fabs(Origine(1) - coupe)<epsilon()) ||
                (std::fabs(coupe - Origine(1)-Longueurs(1))<epsilon())) && (internes)        )
            {
              Cerr << "Y = " << coupe << " is a boundary " << finl;
              exit();
            }
          int imin, imax, kmin, kmax;
          if (std::fabs(coupe-Origine(1))<epsilon())
            j=0;
          else
            j=Ny;
          if(internes)
            for(j=0; coord_noeud(0,j,0,1)+epsilon()<coupe; j++) {};
          imin=kmin=0;
          for(; coord_noeud(imin,0,0,0)+epsilon()<xmin; imin++) {};
          for(; coord_noeud(0,0,kmin,2)+epsilon()<ymin; kmin++) {};
          imax = imin;
          kmax=kmin;
          for(; coord_noeud(imax,0,0,0)+epsilon()<xmax; imax++) {};
          for(; coord_noeud(0,0,(kmax),2)+epsilon()<ymax; kmax++) {};
          //         if((imax-imin)*(kmax-kmin)==0)
          //         {
          //            Cerr << "le bord de nom " << nom_front << "est vide !" << finl;
          //            exit();
          //         }
          som.resize((imax-imin)*(kmax-kmin),4);
          for(i=imin; i<imax; i++)
            for(k=kmin; k<kmax; k++)
              {
                som((k-kmin)*(imax-imin)+i-imin,0)=numero_sommet(i, j, k);
                som((k-kmin)*(imax-imin)+i-imin,1)=numero_sommet(i+1, j, k);
                som((k-kmin)*(imax-imin)+i-imin,2)=numero_sommet(i, j, k+1);
                som((k-kmin)*(imax-imin)+i-imin,3)=numero_sommet(i+1, j, k+1);
              }
        }
      else
        {
          if ( (std::fabs(Origine(2)-coupe)>epsilon()) &&
               (std::fabs(coupe-Origine(2)-Longueurs(2))>epsilon()) && (!internes))
            {
              Cerr << "Z = " << coupe << " is not a boundary " << finl;
              exit();
            }
          if ( ((std::fabs(Origine(2)-coupe)<epsilon()) ||
                (std::fabs(coupe-Origine(2)-Longueurs(2))<epsilon())) && (internes))
            {
              Cerr << "Z = " << coupe << " is a boundary " << finl;
              exit();
            }
          int imin, imax, jmin, jmax;
          if (std::fabs(coupe-Origine(2))<epsilon())
            k=0;
          else
            k=Nz;
          if(internes)
            for(k=0; coord_noeud(0,0,k,3)+epsilon()<coupe; k++) {};
          imin=jmin=0;
          for(; coord_noeud(imin,0,0,0)+epsilon()<xmin; imin++) {};
          for(; coord_noeud(0,jmin,0,1)+epsilon()<ymin; jmin++) {};
          imax = imin;
          jmax=jmin;
          for(; coord_noeud(imax,0,0,0)+epsilon()<xmax; imax++) {};
          if(std::fabs(ymax-(Origine(1)+Longueurs(1)))<epsilon())
            {
              assert(tour_complet!=-123);
              jmax=My-1+tour_complet;
            }
          else
            for(; coord_noeud(0,jmax,0,1)+epsilon()<ymax; jmax++) {};
          //         if((imax-imin)*(jmax-jmin)==0)
          //         {
          //            Cerr << "le bord de nom " << nom_front << "est vide !" << finl;
          //            exit();
          //         }
          som.resize((imax-imin)*(jmax-jmin),4);
          for(i=imin; i<imax; i++)
            for(j=jmin; j<jmax; j++)
              {
                som((j-jmin)*(imax-imin)+i-imin,0)=numero_sommet(i, j, k);
                som((j-jmin)*(imax-imin)+i-imin,1)=numero_sommet(i+1, j, k);
                som((j-jmin)*(imax-imin)+i-imin,2)=numero_sommet(i, j+1, k);
                som((j-jmin)*(imax-imin)+i-imin,3)=numero_sommet(i+1, j+1, k);
              }
        }
      front.ajouter_faces(som);
      front.associer_zone(*this);
    }
  else
    {
      exit();
    }
  Cerr << "End of reading of the boundary : " << nom_front << finl;
}

