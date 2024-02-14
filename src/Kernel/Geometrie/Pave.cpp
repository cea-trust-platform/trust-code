/****************************************************************************
* Copyright (c) 2024, CEA
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
#include <Domaine.h>

Implemente_instanciable(Pave,"Pave",Domaine);
// XD pave mailler_base pave 0 Class to create a pave (block) with boundaries.
// XD  attr name chaine name 0 Name of the pave (block).
// XD  attr bloc bloc_pave bloc 0 Definition of the pave (block).
// XD  attr list_bord list_bord list_bord 0 Domain boundaries definition.

// XD bloc_pave objet_lecture nul -1 Class to create a pave.
// XD attr Origine listf Origine 1 Keyword to define the pave (block) origin, that is to say one of the 8 block points (or 4 in a 2D coordinate system).
// XD attr longueurs listf longueurs 1 Keyword to define the block dimensions, that is to say knowing the origin, length along the axes.
// XD attr nombre_de_noeuds listentierf nombre_de_noeuds 1 Keyword to define the discretization (nodenumber) in each direction.
// XD attr facteurs listf facteurs 1 Keyword to define stretching factors for mesh discretization in each direction. This is a real number which must be positive (by default 1.0). A stretching factor other than 1 allows refinement on one edge in one direction.
// XD attr symx rien symx 1 Keyword to define a block mesh that is symmetrical with respect to the YZ plane (respectively Y-axis in 2D) passing through the block centre.
// XD attr symy rien symy 1 Keyword to define a block mesh that is symmetrical with respect to the XZ plane (respectively X-axis in 2D) passing through the block centre.
// XD attr symz rien symz 1 Keyword defining a block mesh that is symmetrical with respect to the XY plane passing through the block centre.
// XD attr xtanh floattant xtanh 1 Keyword to generate mesh with tanh (hyperbolic tangent) variation in the X-direction.
// XD attr xtanh_dilatation entier(into=[-1,0,1]) xtanh_dilatation 1 Keyword to generate mesh with tanh (hyperbolic tangent) variation in the X-direction. xtanh_dilatation: The value may be -1,0,1 (0 by default): 0: coarse mesh at the middle of the channel and smaller near the walls -1: coarse mesh at the left side of the channel and smaller at the right side 1: coarse mesh at the right side of the channel and smaller near the left side of the channel.
// XD attr xtanh_taille_premiere_maille floattant xtanh_taille_premiere_maille 1 Size of the first cell of the mesh with tanh (hyperbolic tangent) variation in the X-direction.
// XD attr ytanh floattant ytanh 1 Keyword to generate mesh with tanh (hyperbolic tangent) variation in the Y-direction.
// XD attr ytanh_dilatation entier(into=[-1,0,1]) ytanh_dilatation 1 Keyword to generate mesh with tanh (hyperbolic tangent) variation in the Y-direction. ytanh_dilatation: The value may be -1,0,1 (0 by default): 0: coarse mesh at the middle of the channel and smaller near the walls -1: coarse mesh at the bottom of the channel and smaller near the top 1: coarse mesh at the top of the channel and smaller near the bottom.
// XD attr ytanh_taille_premiere_maille floattant ytanh_taille_premiere_maille 1 Size of the first cell of the mesh with tanh (hyperbolic tangent) variation in the Y-direction.
// XD attr ztanh floattant ztanh 1 Keyword to generate mesh with tanh (hyperbolic tangent) variation in the Z-direction.
// XD attr ztanh_dilatation entier(into=[-1,0,1]) ztanh_dilatation 1 Keyword to generate mesh with tanh (hyperbolic tangent) variation in the Z-direction. tanh_dilatation: The value may be -1,0,1 (0 by default): 0: coarse mesh at the middle of the channel and smaller near the walls -1: coarse mesh at the back of the channel and smaller near the front 1: coarse mesh at the front of the channel and smaller near the back.
// XD attr ztanh_taille_premiere_maille floattant ztanh_taille_premiere_maille 1 Size of the first cell of the mesh with tanh (hyperbolic tangent) variation in the Z-direction.

Sortie& Pave::printOn(Sortie& s ) const
{
  return Domaine::printOn(s) ;
}

/*! @brief Lit les specifications d'un pave a partir d'un flot d'entree.
 *
 *     Le format de lecture d'un pave dans le jeu de donnee est le suivant:
 *      Pave nom_pave
 *      {
 *      Origine OX OY (OZ)
 *      Longueurs LX LY (LZ)
 *      Nombre_de_noeuds NX NY (NZ)
 *      Facteurs Fx Fy (Fz)
 *      (Symx)
 *      (Symy)
 *      (Symz)
 *      }
 *      {
 *      (Bord)  nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
 *      ...
 *      (Raccord)  local homogene nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
 *      ...
 *      (Internes)  nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
 *      ...
 *      (Joint)  nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1 PE_voisin
 *      ...
 *	    (Groupe_Faces) nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
 *	    ...
 *      }
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws dimension d'espace necessaire pour mailler
 * @throws accolade ouvrante attendue
 * @throws Symy n'a de sens que pour une dimension >= 2
 * @throws Symz n'a de sens qu'en dimension 3
 * @throws Les facteurs de progression doivent etre positifs
 * @throws Il doit y avoir au moins deux mailles en x
 * @throws accolade ouvrante attendue avant lecture des bords
 * @throws mot cle non reconnu
 */
Entree& Pave::readOn(Entree& is)
{
  if(dimension<0)
    {
      Cerr << "You must give the dimension before to mesh" << finl;
      Cerr << " The syntax is \"Dimension dim\" " << finl ;
      exit();
    }
  facteurs_.resize(dimension);
  facteurs_ = 1.;
  symetrique_.resize(dimension);
  symetrique_=0;
  origine_.resize(dimension);
  longueurs_.resize(dimension);
  nb_noeuds_.resize(dimension);
  Motcle motlu;
  int rang;

  typer_();

  is >> nom_;
  Cerr << "Reading of the block " << nom_ << finl;
  is >> motlu;
  if (motlu!="{")
    {
      Cerr << "We expected a { after " << nom_ << finl;
      exit();
    }
  {
    Motcles les_mots(18);
    {
      les_mots[0]="Origine";
      les_mots[1]="Longueurs";
      les_mots[2]="Nombre_de_noeuds";
      les_mots[3]="Facteurs";
      les_mots[4]="Symx";
      les_mots[5]="Symy";
      les_mots[6]="Symz";
      les_mots[7]="xtanh";
      les_mots[8]="xtanh_dilatation";
      les_mots[9]="xtanh_taille_premiere_maille";
      les_mots[10]="ytanh";
      les_mots[11]="ytanh_dilatation";
      les_mots[12]="ytanh_taille_premiere_maille";
      les_mots[13]="ztanh";
      les_mots[14]="ztanh_dilatation";
      les_mots[15]="ztanh_taille_premiere_maille";
      les_mots[16]="reprise_VEF";
      les_mots[17]="}";
    }
    while(motlu != "}")
      {
        is >> motlu;
        rang = les_mots.search(motlu);
        switch(rang)
          {
          case 0:
            for(int i=0; i< dimension; i++)
              is >> origine_(i);
            break;
          case 1:
            lire_longueurs(is);
            break;
          case 2:
            lire_noeuds(is);
            break;
          case 3:
            for(int i=0; i< dimension; i++)
              is >> facteurs_(i);
            break;
          case 4:      // Symx
            symetrique_(0)=1;
            break;
          case 5:      // Symy
            if(dimension<2)
              {
                Cerr << "Symy has meaning only in dimension >= 2" << finl;
                exit();
              }
            symetrique_(1)=1;
            break;
          case 6:      // Symz
            if(dimension<3)
              {
                Cerr << "Symz has meaning only in dimension >= 3" << finl;
                exit();
              }
            symetrique_(2)=1;
            break;
          case 7:
            {
              is >> xa_tanh;
              break;
            }
          case 8:
            {
              is >> xtanh_dilatation;
              break;
            }
          case 9:
            {
              double x1, x_tmp;
              is >> x1;
              xa_tanh=.1;
              double fac_sym;
              assert(xtanh_dilatation!=-123);
              if (xtanh_dilatation != 0 )
                fac_sym=1.;
              else
                fac_sym=2.;
              assert(!est_egal(xa_tanh,-123.));
              for(int decimale=1; decimale<7; decimale++)
                {
                  x_tmp=longueurs_(0)/fac_sym*(1.+tanh((-1.+fac_sym*1./((Mx-1)*1.))*atanh(xa_tanh))/xa_tanh);
                  while ( x_tmp > x1 )
                    {
                      xa_tanh+=pow(10.,-decimale);
                      xa_tanh = std::min(xa_tanh,1-Objet_U::precision_geom);
                      if ( std::fabs(xa_tanh) < Objet_U::precision_geom)
                        {
                          Cerr << "Error: The coefficient xa_tanh has a value of : " << xa_tanh << finl ;
                          Cerr << "So the mesh can't be generated with tanh (hyperbolic tangent) variation!" << finl ;
                          Cerr << "You must decrease either the xtanh_taille_premiere_maille size of the first cell of the mesh " << finl;
                          Cerr << "or the nombre_de_noeuds Nx node number in the X direction." << finl;
                          exit();
                        }
                      x_tmp=longueurs_(0)/fac_sym*(1.+tanh((-1.+fac_sym*1./((Mx-1)*1.))*atanh(xa_tanh))/xa_tanh);
                    }
                  xa_tanh-=pow(10.,-decimale);
                  xa_tanh = std::min(xa_tanh,1-Objet_U::precision_geom);
                  if ( std::fabs(xa_tanh) < Objet_U::precision_geom)
                    {
                      Cerr << "Error: The coefficient xa_tanh has a value of : " << xa_tanh << finl ;
                      Cerr << "So the mesh can't be generated with tanh (hyperbolic tangent) variation!" << finl ;
                      Cerr << "You must decrease either the xtanh_taille_premiere_maille size of the first cell of the mesh " << finl;
                      Cerr << "or the nombre_de_noeuds Nx node number in the X direction." << finl;
                      exit();
                    }
                }
              Cerr << "The coefficient xa_tanh has a value of : " << xa_tanh << finl ;
              break;
            }
          case 10:
            {
              is >> a_tanh;
              break;
            }
          case 11:
            {
              is >> tanh_dilatation;
              break;
            }
          case 12:
            {
              double y1, y_tmp;
              is >> y1;
              a_tanh=.1;
              double fac_sym;
              assert(tanh_dilatation!=-123);
              if (tanh_dilatation != 0 )
                fac_sym=1.;
              else
                fac_sym=2.;
              assert(!est_egal(a_tanh,-123.));
              for(int decimale=1; decimale<7; decimale++)
                {
                  y_tmp=longueurs_(1)/fac_sym*(1.+tanh((-1.+fac_sym*1./((My-1)*1.))*atanh(a_tanh))/a_tanh);
                  while ( y_tmp > y1 )
                    {
                      a_tanh+=pow(10.,-decimale);
                      a_tanh = std::min(a_tanh,1-Objet_U::precision_geom);
                      if ( std::fabs(a_tanh) < Objet_U::precision_geom)
                        {
                          Cerr << "Error: The coefficient ya_tanh has a value of : " << a_tanh << finl ;
                          Cerr << "So the mesh can't be generated with tanh (hyperbolic tangent) variation!" << finl ;
                          Cerr << "You must decrease either the ytanh_taille_premiere_maille size of the first cell of the mesh " << finl;
                          Cerr << "or the nombre_de_noeuds Ny node number in the Y direction." << finl;
                          exit();
                        }
                      y_tmp=longueurs_(1)/fac_sym*(1.+tanh((-1.+fac_sym*1./((My-1)*1.))*atanh(a_tanh))/a_tanh);
                    }
                  a_tanh-=pow(10.,-decimale);
                  a_tanh = std::min(a_tanh,1-Objet_U::precision_geom);
                  if ( std::fabs(a_tanh) < Objet_U::precision_geom)
                    {
                      Cerr << "Error: The coefficient ya_tanh has a value of : " << a_tanh << finl ;
                      Cerr << "So the mesh can't be generated with tanh (hyperbolic tangent) variation!" << finl ;
                      Cerr << "You must decrease either the ytanh_taille_premiere_maille size of the first cell of the mesh " << finl;
                      Cerr << "or the nombre_de_noeuds Ny node number in the Y direction." << finl;
                      exit();
                    }
                }
              Cerr << "The coefficient ya_tanh has a value of : " << a_tanh << finl ;
              break;
            }
          case 13:
            {
              is >> za_tanh;
              break;
            }
          case 14:
            {
              is >> ztanh_dilatation;
              break;
            }
          case 15:
            {
              double z1, z_tmp;
              is >> z1;
              za_tanh=.1;
              double fac_sym;
              assert(ztanh_dilatation!=-123);
              if (ztanh_dilatation != 0 )
                fac_sym=1.;
              else
                fac_sym=2.;
              assert(!est_egal(za_tanh,-123.));
              for(int decimale=1; decimale<7; decimale++)
                {
                  z_tmp=longueurs_(2)/fac_sym*(1.+tanh((-1.+fac_sym*1./((Mz-1)*1.))*atanh(za_tanh))/za_tanh);
                  while ( z_tmp > z1 )
                    {
                      za_tanh+=pow(10.,-decimale);
                      za_tanh = std::min(za_tanh,1-Objet_U::precision_geom);
                      if ( std::fabs(za_tanh) < Objet_U::precision_geom)
                        {
                          Cerr << "Error: The coefficient za_tanh has a value of : " << za_tanh << finl ;
                          Cerr << "So the mesh can't be generated with tanh (hyperbolic tangent) variation!" << finl ;
                          Cerr << "You must decrease either the ztanh_taille_premiere_maille size of the first cell of the mesh " << finl;
                          Cerr << "or the nombre_de_noeuds Nz node number in the Z direction." << finl;
                          exit();
                        }
                      z_tmp=longueurs_(2)/fac_sym*(1.+tanh((-1.+fac_sym*1./((Mz-1)*1.))*atanh(za_tanh))/za_tanh);
                    }
                  za_tanh-=pow(10.,-decimale);
                  za_tanh = std::min(za_tanh,1-Objet_U::precision_geom);
                  if ( std::fabs(za_tanh) < Objet_U::precision_geom)
                    {
                      Cerr << "Error: The coefficient za_tanh has a value of : " << za_tanh << finl ;
                      Cerr << "So the mesh can't be generated with tanh (hyperbolic tangent) variation!" << finl ;
                      Cerr << "You must decrease either the ztanh_taille_premiere_maille size of the first cell of the mesh " << finl;
                      Cerr << "or the nombre_de_noeuds Nz node number in the Z direction." << finl;
                      exit();
                    }
                }
              Cerr << "The coefficient za_tanh has a value of : " << za_tanh << finl ;
              break;
            }
          case 16:
            {
              rep_VEF=true;
              break;
            }
          case 17: // Fin
            break;
          default:
            if (motlu == "tanh" || motlu == "tanh_dilatation" || motlu == "tanh_taille_premiere_maille")
              {
                Cerr << "Error: '" << motlu << "' keyword is obsolete since V1.7.9. We renamed it to 'Y"<< motlu <<"'" << finl;
              }
            else
              {
                Cerr << motlu << "  is not understood " << finl;
                Cerr << les_mots;
              }
            exit();
          }
      }
    pas_.resize(dimension);
    if(min_array(facteurs_)<=0.)
      {
        Cerr << "The progression factors must be positives" << finl;
        exit();
      }
    switch(dimension)
      {
      case 1:
        maille1D();
        break;
      case 2:
        maille2D();
        break;
      case 3:
        maille3D();
        break;
      }
  }
  if(Mx<2)
    {
      Cerr << "There must be at least two cells on x" << finl;
      exit();
    }
  if (axi)
    {
      // Les coordonnees sont modulo 2 pi
      double deux_pi=M_PI*2.0 ;
      {
        int les_noeuds__dimension0_opt=sommets_.dimension(0);
        for(int i=0; i<les_noeuds__dimension0_opt; i++)
          {
            sommets_(i, 1)-=(int) sommets_(i, 1);
            sommets_(i, 1)*=deux_pi;
          }
      }

      longueurs_(1)*=deux_pi;
      origine_(1)*=deux_pi;
    }
  {
    is >> motlu;
    if (motlu!="{")
      {
        Cerr << "We expected a { before reading the boundaries" << finl;
        exit();
      }
    Motcles les_mots(7);
    {
      les_mots[0]="Bord";
      les_mots[1]="Raccord";
      les_mots[2]="Internes";
      les_mots[3]="Trou";
      les_mots[4]="Joint";
      les_mots[5]="Groupe_Faces";
      les_mots[6]="}";
    }
    while(motlu != "}")
      {
        is >> motlu;
        rang = les_mots.search(motlu);
        switch(rang)
          {
          case 0:      // Bord
            {
              Bord& newbord=mes_faces_bord_.add(Bord());
              lire_front(is , newbord );
            }
            break;
          case 1:      // Raccord
            {
              Raccord& newraccord=mes_faces_raccord_.add(Raccord());
              Nom type="Raccord_";
              Nom local, homogene;
              is >> local >> homogene;
              type+=local;
              type+="_";
              type+=homogene;
              if (type == "local")
                {
                  Cerr << "Do not use sequential local connection!" << finl;
                  exit();
                }
              newraccord.typer(type);
              lire_front(is , newraccord.valeur() );
            }
            break;
          case 2:      // Plaques
            {
              Bord_Interne& faces=mes_bords_int_.add(Bord_Interne());
              lire_front(is , faces );
            }
            break;
          case 5:      // Groupe de Faces
            {
              Groupe_Faces& faces=mes_groupes_faces_.add(Groupe_Faces());
              lire_front(is , faces );
            }
            break;
          case 3:      // Trous
            break;
          case 4:      // Joint
            {
              Joint& newjoint=mes_faces_joint_.add(Joint());
              lire_front(is , newjoint );
            }
            break;
          case 6:      // Fin
            break;
          default:
            Cerr << motlu << "  is not understood " << finl;
            Cerr << les_mots;
            exit();
          }
      }
  }

  return is;
}

/*! @brief Effectue un maillage 1D, du pave avec les valeurs des parametres lus par ReadOn.
 *
 */
void Pave::maille1D()
{
  Cerr << "Step of 1D mesh in progress... " << finl;
  double epsilon_geom=Objet_U::precision_geom;
  int i;
  if(longueurs_(0)<0)
    {
      origine_(0)+=longueurs_(0);
      longueurs_(0)=-longueurs_(0);
      facteurs_(0)=1./facteurs_(0);
    }
  if( (!symetrique_(0)) || (std::fabs(facteurs_(0)-1.)<epsilon_geom)   )
    {
      if(std::fabs(facteurs_(0)-1.)>epsilon_geom)
        pas_(0)=longueurs_(0)*(facteurs_(0)-1)/(pow(facteurs_(0),Nx)-1);
      else
        pas_(0)=longueurs_(0)/Nx;
      double dx=pas_(0);
      double x=origine_(0);
      for (i=0; i<Nx; i++)
        {
          coord_noeud(i)=x;
          x+=dx;
          dx*=facteurs_(0);
        }
      coord_noeud(Nx)=origine_(0)+longueurs_(0);
    }
  else
    {
      int Ix=Nx/2;
      if( (Ix*2)==Nx )
        pas_(0)=0.5*longueurs_(0)*(facteurs_(0)-1)/(pow(facteurs_(0),Ix)-1);
      else
        {
          if(Ix)
            {
              Ix+=1;
              pas_(0)=0.5*longueurs_(0)/( (pow(facteurs_(0),Ix-1)-1)/(facteurs_(0)-1)
                                          +0.5*pow(facteurs_(0),Ix) );
            }
          else
            {
              Ix+=1;
              pas_(0)=0;
            }
        }
      double dx=pas_(0);
      double x=0;
      for (i=0; i<Ix; i++)
        {
          coord_noeud(i)=origine_(0)+x;
          coord_noeud(Nx-i)=origine_(0)+longueurs_(0)-x;
          x+=dx;
          dx*=facteurs_(0);
        }
      if( (Ix*2)==Nx )
        coord_noeud(Ix)=0.5*(coord_noeud(Ix+1)+coord_noeud(Ix-1));
    }
  Cerr << finl;
  Cerr << "Step of mesh ended " << finl;
}


/*! @brief Effectue un maillage 2D, du pave avec les valeurs des parametres lus par ReadOn.
 *
 */
void Pave::maille2D()
{
  Cerr << "Step of 2D mesh in progress... " << finl;
  double epsilon_geom=Objet_U::precision_geom;
  int i, j;
  int pourcent=0;
  int tmp;
  if(longueurs_(0)<0)
    {
      origine_(0)+=longueurs_(0);
      longueurs_(0)=-longueurs_(0);
      facteurs_(0)=1./facteurs_(0);
    }
  if(longueurs_(1)<0)
    {
      origine_(1)+=longueurs_(1);
      longueurs_(1)=-longueurs_(1);
      facteurs_(1)=1./facteurs_(1);
    }
  if( (!symetrique_(0)) || (std::fabs(facteurs_(0)-1.)<epsilon_geom) )
    {
      assert(!est_egal(xa_tanh,-123.));
      if ((xa_tanh>epsilon_geom)&&(xa_tanh<1.) )
        {
          ///  Maillage en x en tanh
          Cerr << "In if( (xa_tanh>epsilon_geom)&&(xa_tanh<1.) )" << finl;
          pourcent =0;
          double fac_sym;
          assert(xtanh_dilatation!=-123);
          if (xtanh_dilatation != 0)
            fac_sym=1.;
          else
            fac_sym=2.;

          if (xtanh_dilatation != -1 )
            {
              for (i=0; i<Mx; i++)
                {
                  Cerr << "i=" << i << finl;
                  tmp=(i*100)/Mx;
                  if(tmp > pourcent)
                    {
                      pourcent=tmp;
                      Cerr << "\rIt was created " << pourcent << "% of coordx" << flush;
                    }
                  assert(!est_egal(xa_tanh,-123.));
                  for (j=0; j<My; j++)
                    coord_noeud(i,j,0)=origine_(0)+longueurs_(0)/fac_sym*(1.+tanh((-1.+fac_sym*i/((Mx-1)*1.))*atanh(xa_tanh))/xa_tanh);
                  Cerr << "coord_noeud(0,j,1)=" << coord_noeud(0,j,0) << finl;
                  Cerr << "It was created " << pourcent << "% of coordx" << flush;
                }
            }
          else
            {
              for (i=0; i<Mx; i++)
                {
                  Cerr << "i=" << i << finl;
                  tmp=(i*100)/Mx;
                  if(tmp > pourcent)
                    {
                      pourcent=tmp;
                      Cerr << "\rIt was created " << pourcent << "% of coordx" << flush;
                    }
                  assert(!est_egal(xa_tanh,-123.));
                  for (j=0; j<My; j++)
                    coord_noeud(i,j,0)=origine_(0)+longueurs_(0)-longueurs_(0)/fac_sym*(1.+tanh((-1.+fac_sym*(Mx-1-i)/((Mx-1)*1.))*atanh(xa_tanh))/xa_tanh);
                  Cerr << "coord_noeud(0,j,0)=" << coord_noeud(0,j,0) << finl;
                  Cerr << "It was created " << pourcent << "% of coordx" << flush;
                }
            }
          ///////////////////////////////
          for (j=0; j<My; j++)
            coord_noeud(0,j,0)=origine_(0);
        }
      else
        {
          double x;
          for (i=0; i<Nx; i++)
            {
              tmp=(i*100)/Nx;
              if(tmp > pourcent)
                {
                  pourcent=tmp;
                  Cerr << "It was created " << pourcent << "% of coordx" << flush;
                }
              if (std::fabs(facteurs_(0)-1.)>epsilon_geom)
                x=origine_(0)+longueurs_(0)*(pow(facteurs_(0),i)-1)/(pow(facteurs_(0),Nx)-1);
              else
                x=origine_(0)+longueurs_(0)*i/Nx;
              for (j=0; j<My; j++)
                coord_noeud(i,j,0)=x;
            }
          for (j=0; j<My; j++)
            coord_noeud(Nx,j,0)=origine_(0)+longueurs_(0);
        }
    }
  else
    {
      int Ix=Nx/2;
      if( (Ix*2)==Nx )
        pas_(0)=0.5*longueurs_(0)*(facteurs_(0)-1)/(pow(facteurs_(0),Ix)-1);
      else
        {
          if(Ix)
            {
              Ix+=1;
              pas_(0)=0.5*longueurs_(0)/( (pow(facteurs_(0),Ix-1)-1)/(facteurs_(0)-1)
                                          +0.5*pow(facteurs_(0),Ix) );
            }
          else
            {
              Ix+=1;
              pas_(0)=longueurs_(0);
            }
        }
      double dx=pas_(0);
      double x=0;
      for (i=0; i<Ix; i++)
        {
          tmp=(i*100)/Ix;
          if(tmp > pourcent)
            {
              pourcent=tmp;
              Cerr << "It was created " << pourcent << "% of coordx" << flush;
            }
          for (j=0; j<My; j++)
            {
              coord_noeud(i,j,0)=origine_(0)+x;
              coord_noeud(Nx-i,j,0)=origine_(0)+longueurs_(0)-x;
            }
          x+=dx;
          dx*=facteurs_(0);
        }
      if( (Ix*2)==Nx )
        for (j=0; j<My; j++)
          coord_noeud(Ix,j,0)=0.5*(coord_noeud(Ix+1,j,0)
                                   +coord_noeud(Ix-1,j,0));
    }
  pourcent=0;
  //
  if( (!symetrique_(1)) || (std::fabs(facteurs_(1)-1.)<epsilon_geom) )
    {
      assert(!est_egal(a_tanh,-123.));
      if ((a_tanh>epsilon_geom)&&(a_tanh<1.) )
        {
          ///  Maillage en y en tanh
          Cerr << "In if( (a_tanh>epsilon_geom)&&(a_tanh<1.) )" << finl;
          pourcent =0;
          double fac_sym;
          assert(tanh_dilatation!=-123);
          if (tanh_dilatation != 0)
            fac_sym=1.;
          else
            fac_sym=2.;

          if (tanh_dilatation != -1 )
            {
              for (j=0; j<My; j++)
                {
                  Cerr << "j=" << j << finl;
                  tmp=(j*100)/My;
                  if(tmp > pourcent)
                    {
                      pourcent=tmp;
                      Cerr << "\rIt was created " << pourcent << "% of coordy" << flush;
                    }
                  assert(!est_egal(a_tanh,-123.));
                  for (i=0; i<Mx; i++)
                    coord_noeud(i,j,1)=origine_(1)+longueurs_(1)/fac_sym*(1.+tanh((-1.+fac_sym*j/((My-1)*1.))*atanh(a_tanh))/a_tanh);
                  Cerr << "coord_noeud(0,j,1)=" << coord_noeud(0,j,1) << finl;
                  Cerr << "It was created " << pourcent << "% of coordy" << flush;
                }
            }
          else
            {
              for (j=0; j<My; j++)
                {
                  Cerr << "j=" << j << finl;
                  tmp=(j*100)/My;
                  if(tmp > pourcent)
                    {
                      pourcent=tmp;
                      Cerr << "\rIt was created " << pourcent << "% of coordy" << flush;
                    }
                  assert(!est_egal(a_tanh,-123.));
                  for (i=0; i<Mx; i++)
                    coord_noeud(i,j,1)=origine_(1)+longueurs_(1)-longueurs_(1)/fac_sym*(1.+tanh((-1.+fac_sym*(My-1-j)/((My-1)*1.))*atanh(a_tanh))/a_tanh);
                  Cerr << "coord_noeud(0,j,1)=" << coord_noeud(0,j,1) << finl;
                  Cerr << "It was created " << pourcent << "% of coordy" << flush;
                }
            }
          ///////////////////////////////
          for (i=0; i<Mx; i++)
            coord_noeud(i,0,1)=origine_(1);
        }
      else
        {
          double y;
          for (j=0; j<Ny; j++)
            {
              tmp=(j*100)/Ny;
              if(tmp > pourcent)
                {
                  pourcent=tmp;
                  Cerr << "It was created " << pourcent << "% of coordy" << flush;
                }
              if (std::fabs(facteurs_(1)-1.)>epsilon_geom)
                y=origine_(1)+longueurs_(1)*(pow(facteurs_(1),j)-1)/(pow(facteurs_(1),Ny)-1);
              else
                y=origine_(1)+longueurs_(1)*j/Ny;
              for (i=0; i<Mx; i++)
                coord_noeud(i,j,1)=y;
            }
          for (i=0; i<Mx; i++)
            coord_noeud(i,j,1)=origine_(1)+longueurs_(1);
        }
    }
  else
    {
      int Iy=Ny/2;
      if( (Iy*2)==Ny )
        pas_(1)=0.5*longueurs_(1)*(facteurs_(1)-1)/(pow(facteurs_(1),Iy)-1);
      else
        {
          if(Iy)
            {
              Iy+=1;
              pas_(1)=0.5*longueurs_(1)/( (pow(facteurs_(1),Iy-1)-1)/(facteurs_(1)-1)
                                          +0.5*pow(facteurs_(1),Iy) );
            }
          else
            {
              Iy+=1;
              pas_(1)=longueurs_(1);
            }
        }
      double dy=pas_(1);
      double y=0;
      for (j=0; j<Iy; j++)
        {
          tmp=(j*100)/Iy;
          if(tmp > pourcent)
            {
              pourcent=tmp;
              Cerr << "It was created " << pourcent << "% of coordy" << flush;
            }
          for (i=0; i<Mx; i++)
            {
              coord_noeud(i,j,1)=origine_(1)+y;
              coord_noeud(i,Ny-j,1)=origine_(1)+longueurs_(1)-y;
            }
          y+=dy;
          dy*=facteurs_(1);
        }
      if( (Iy*2)==Ny )
        for (i=0; i<Mx; i++)
          coord_noeud(i,Iy,1)=0.5*(coord_noeud(i,Iy-1,1)
                                   +coord_noeud(i,Iy+1,1));
    }
  Cerr << finl;
  Cerr << "Step of mesh ended " << finl;
}

/*! @brief Effectue un maillage 3D, du pave avec les valeurs des parametres lus par ReadOn.
 *
 */
void Pave::maille3D()
{
  Cerr << "Step of 3D mesh in progress... " << finl;
  double epsilon_geom=Objet_U::precision_geom;
  int i, j, k;
  int pourcent=0;
  int tmp;
  if(longueurs_(0)<0)
    {
      origine_(0)+=longueurs_(0);
      longueurs_(0)=-longueurs_(0);
      facteurs_(0)=1./facteurs_(0);
    }
  if(longueurs_(1)<0)
    {
      origine_(1)+=longueurs_(1);
      longueurs_(1)=-longueurs_(1);
      facteurs_(1)=1./facteurs_(1);
    }
  if(longueurs_(2)<0)
    {
      origine_(2)+=longueurs_(2);
      longueurs_(2)=-longueurs_(2);
      facteurs_(2)=1./facteurs_(2);
    }
  if( (!symetrique_(0)) || (std::fabs(facteurs_(0)-1.)<epsilon_geom) )
    {
      assert(!est_egal(xa_tanh,-123.));
      if ((xa_tanh>epsilon_geom)&&(xa_tanh<1.) )
        {
          ///  Maillage en x en tanh
          Cerr << "In if( (xa_tanh>epsilon)&&(xa_tanh<1.) )" << finl;
          pourcent =0;
          double fac_sym;
          assert(xtanh_dilatation!=-123);
          if (xtanh_dilatation != 0)
            fac_sym=1.;
          else
            fac_sym=2.;

          if (xtanh_dilatation != -1 )
            {
              for (i=0; i<Mx; i++)
                {
                  Cerr << "i=" << i << finl;
                  tmp=(i*100)/Mx;
                  if(tmp > pourcent)
                    {
                      pourcent=tmp;
                      Cerr << "\rIt was created " << pourcent << "% of coordx" << flush;
                    }
                  assert(!est_egal(xa_tanh,-123.));
                  for (j=0; j<My; j++)
                    for (k=0; k<Mz; k++)
                      coord_noeud(i,j,k,0)=origine_(0)+longueurs_(0)/fac_sym*(1.+tanh((-1.+fac_sym*i/((Mx-1)*1.))*atanh(xa_tanh))/xa_tanh);
                  Cerr << "coord_noeud(i,0,0,0)=" << coord_noeud(i,0,0,0) << finl;
                }
            }
          else
            {
              for (i=0; i<Mx; i++)
                {
                  Cerr << "i=" << i << finl;
                  tmp=(i*100)/Mx;
                  if(tmp > pourcent)
                    {
                      pourcent=tmp;
                      Cerr << "It was created " << pourcent << "% of coordx" << flush;
                    }
                  assert(!est_egal(xa_tanh,-123.));
                  for (j=0; j<My; j++)
                    for (k=0; k<Mz; k++)
                      coord_noeud(i,j,k,0)=origine_(0)+longueurs_(0)-longueurs_(0)/fac_sym*(1.+tanh((-1.+fac_sym*(Mx-1-i)/((Mx-1)*1.))*atanh(xa_tanh))/xa_tanh);

                  Cerr << "coord_noeud(i,0,0,0)=" << coord_noeud(i,0,0,0) << finl;
                }
            }
          for (j=0; j<My; j++)
            for (k=0; k<Mz; k++)
              coord_noeud(0,j,k,0)=origine_(0);
        }
      ///////////////////////////////
      else
        {

          double x;
          for (i=0; i<Nx; i++)
            {
              tmp=(i*100)/Nx;
              if(tmp > pourcent)
                {
                  pourcent=tmp;
                  Cerr << "It was created " << pourcent << "% of coordx" << flush;
                }
              if(std::fabs(facteurs_(0)-1.)>epsilon_geom)
                x=origine_(0)+longueurs_(0)*(pow(facteurs_(0),i)-1)/(pow(facteurs_(0),Nx)-1);
              else
                x=origine_(0)+longueurs_(0)*i/Nx;
              for (j=0; j<My; j++)
                for (k=0; k<Mz; k++)
                  coord_noeud(i,j,k,0)=x;
            }
          for (j=0; j<My; j++)
            for (k=0; k<Mz; k++)
              coord_noeud(Nx,j,k,0)=origine_(0)+longueurs_(0);
        }
    }
  else
    {
      int Ix=Nx/2;
      if( (Ix*2)==Nx )
        pas_(0)=0.5*longueurs_(0)*(facteurs_(0)-1)/(pow(facteurs_(0),Ix)-1);
      else
        {
          if(Ix)
            {
              Ix+=1;
              pas_(0)=0.5*longueurs_(0)/( (pow(facteurs_(0),Ix-1)-1)/(facteurs_(0)-1)
                                          +0.5*pow(facteurs_(0),Ix) );
            }
          else
            {
              Ix+=1;
              pas_(0)=longueurs_(0);
            }
        }
      double dx=pas_(0);
      double x=0;
      for (i=0; i<Ix; i++)
        {
          tmp=(i*100)/Ix;
          if(tmp > pourcent)
            {
              pourcent=tmp;
              Cerr << "It was created " << pourcent << "% of coordx" << flush;
            }
          for (j=0; j<My; j++)
            for (k=0; k<Mz; k++)
              {
                coord_noeud(i,j,k,0)=origine_(0)+x;
                coord_noeud(Nx-i,j,k,0)=origine_(0)+longueurs_(0)-x;
              }
          x+=dx;
          dx*=facteurs_(0);
        }
      if( (Ix*2)==Nx )
        for (j=0; j<My; j++)
          for (k=0; k<Mz; k++)
            coord_noeud(Ix,j,k,0)=0.5*(coord_noeud(Ix+1,j,k,0)+
                                       coord_noeud(Ix-1,j,k,0));
    }
  //
  pourcent=0;
  Cerr << finl;
  if( (!symetrique_(1)) || (std::fabs(facteurs_(1)-1.)<epsilon_geom) )
    {
      assert(!est_egal(a_tanh,-123.));
      if ((a_tanh>epsilon_geom)&&(a_tanh<1.) )
        {
          ///  Maillage en y en tanh
          Cerr << "In if( (a_tanh>epsilon)&&(a_tanh<1.) )" << finl;
          pourcent =0;
          double fac_sym;
          assert(tanh_dilatation!=-123);
          if (tanh_dilatation != 0)
            fac_sym=1.;
          else
            fac_sym=2.;

          if (tanh_dilatation != -1 )
            {
              for (j=0; j<My; j++)
                {
                  Cerr << "j=" << j << finl;
                  tmp=(j*100)/My;
                  if(tmp > pourcent)
                    {
                      pourcent=tmp;
                      Cerr << "\rIt was created " << pourcent << "% of coordy" << flush;
                    }
                  assert(!est_egal(a_tanh,-123.));
                  for (i=0; i<Mx; i++)
                    for (k=0; k<Mz; k++)
                      coord_noeud(i,j,k,1)=origine_(1)+longueurs_(1)/fac_sym*(1.+tanh((-1.+fac_sym*j/((My-1)*1.))*atanh(a_tanh))/a_tanh);
                  Cerr << "coord_noeud(0,j,0,1)=" << coord_noeud(0,j,0,1) << finl;
                }
            }
          else
            {
              for (j=0; j<My; j++)
                {
                  Cerr << "j=" << j << finl;
                  tmp=(j*100)/My;
                  if(tmp > pourcent)
                    {
                      pourcent=tmp;
                      Cerr << "It was created " << pourcent << "% of coordy" << flush;
                    }
                  assert(!est_egal(a_tanh,-123.));
                  for (i=0; i<Mx; i++)
                    for (k=0; k<Mz; k++)
                      coord_noeud(i,j,k,1)=origine_(1)+longueurs_(1)-longueurs_(1)/fac_sym*(1.+tanh((-1.+fac_sym*(My-1-j)/((My-1)*1.))*atanh(a_tanh))/a_tanh);

                  Cerr << "coord_noeud(0,j,0,1)=" << coord_noeud(0,j,0,1) << finl;
                }
            }
          for (i=0; i<Mx; i++)
            for (k=0; k<Mz; k++)
              coord_noeud(i,0,k,1)=origine_(1);
        }
      ///////////////////////////////
      else
        {
          double y;
          for (j=0; j<Ny; j++)
            {
              tmp=(j*100)/Ny;
              if(tmp > pourcent)
                {
                  pourcent=tmp;
                  Cerr << "It was created " << pourcent << "% of coordy" << flush;
                }
              if(std::fabs(facteurs_(1)-1.)>epsilon_geom)
                y=origine_(1)+longueurs_(1)*(pow(facteurs_(1),j)-1)/(pow(facteurs_(1),Ny)-1);
              else
                y=origine_(1)+longueurs_(1)*j/Ny;

              for (i=0; i<Mx; i++)
                for (k=0; k<Mz; k++)
                  coord_noeud(i,j,k,1)=y;
            }
          for (i=0; i<Mx; i++)
            for (k=0; k<Mz; k++)
              coord_noeud(i,j,k,1)=origine_(1)+longueurs_(1);
        }
    }
  else
    {
      int Iy=Ny/2;
      if( (Iy*2)==Ny )
        pas_(1)=0.5*longueurs_(1)*(facteurs_(1)-1)/(pow(facteurs_(1),Iy)-1);
      else
        {
          if(Iy)
            {
              Iy+=1;
              pas_(1)=0.5*longueurs_(1)/( (pow(facteurs_(1),Iy-1)-1)/(facteurs_(1)-1)
                                          +0.5*pow(facteurs_(1),Iy) );
            }
          else
            {
              Iy+=1;
              pas_(1)=longueurs_(1);
            }
        }
      double dy=pas_(1);
      double y=0;
      for (j=0; j<Iy; j++)
        {
          tmp=(j*100)/Iy;
          if(tmp > pourcent)
            {
              pourcent=tmp;
              Cerr << "It was created " << pourcent << "% of coordy" << flush;
            }
          for (i=0; i<Mx; i++)
            for (k=0; k<Mz; k++)
              {
                coord_noeud(i,j,k,1)=origine_(1)+y;
                coord_noeud(i,Ny-j,k,1)=origine_(1)+longueurs_(1)-y;
              }
          y+=dy;
          dy*=facteurs_(1);
        }
      if( (Iy*2)==Ny )
        for (i=0; i<Mx; i++)
          for (k=0; k<Mz; k++)
            coord_noeud(i,Iy,k,1)=0.5*(coord_noeud(i,Iy-1,k,1)
                                       +coord_noeud(i,Iy+1,k,1));
    }
  //
  pourcent=0;
  Cerr << finl;
  if( (!symetrique_(2)) ||( std::fabs(facteurs_(2)-1.)<epsilon_geom)    )
    {
      assert(!est_egal(za_tanh,-123.));
      if ((za_tanh>epsilon_geom)&&(za_tanh<1.) )
        {
          ///  Maillage en z en tanh
          Cerr << "In if( (za_tanh>epsilon)&&(za_tanh<1.) )" << finl;
          pourcent =0;
          double fac_sym;
          assert(ztanh_dilatation!=-123);
          if (ztanh_dilatation != 0)
            fac_sym=1.;
          else
            fac_sym=2.;

          if (ztanh_dilatation != -1 )
            {
              for (k=0; k<Mz; k++)
                {
                  Cerr << "k=" << k << finl;
                  tmp=(k*100)/Mz;
                  if(tmp > pourcent)
                    {
                      pourcent=tmp;
                      Cerr << "\rIt was created " << pourcent << "% of coordz" << flush;
                    }
                  assert(!est_egal(za_tanh,-123.));
                  for (i=0; i<Mx; i++)
                    for (j=0; j<My; j++)
                      coord_noeud(i,j,k,2)=origine_(2)+longueurs_(2)/fac_sym*(1.+tanh((-1.+fac_sym*k/((Mz-1)*1.))*atanh(za_tanh))/za_tanh);
                  Cerr << "coord_noeud(0,0,k,2)=" << coord_noeud(0,0,k,2) << finl;
                }
            }
          else
            {
              for (k=0; k<Mz; k++)
                {
                  Cerr << "k=" << k << finl;
                  tmp=(k*100)/Mz;
                  if(tmp > pourcent)
                    {
                      pourcent=tmp;
                      Cerr << "It was created " << pourcent << "% of coordz" << flush;
                    }
                  assert(!est_egal(za_tanh,-123.));
                  for (i=0; i<Mx; i++)
                    for (j=0; j<My; j++)
                      coord_noeud(i,j,k,2)=origine_(2)+longueurs_(2)-longueurs_(2)/fac_sym*(1.+tanh((-1.+fac_sym*(Mz-1-k)/((Mz-1)*1.))*atanh(za_tanh))/za_tanh);

                  Cerr << "coord_noeud(0,0,k,2)=" << coord_noeud(0,0,k,2) << finl;
                }
            }
          for (i=0; i<Mx; i++)
            for (j=0; j<My; j++)
              coord_noeud(i,j,0,2)=origine_(2);
        }
      ///////////////////////////////
      else
        {
          double z;
          for (k=0; k<Nz; k++)
            {
              tmp=(k*100)/Nz;
              if(tmp > pourcent)
                {
                  pourcent=tmp;
                  Cerr << "It was created " << pourcent << "% of coordz" << flush;
                }
              if(std::fabs(facteurs_(2)-1.)>epsilon_geom)
                z=origine_(2)+longueurs_(2)*(pow(facteurs_(2),k)-1)/(pow(facteurs_(2),Nz)-1);
              else
                z=origine_(2)+longueurs_(2)*k/Nz;
              for (i=0; i<Mx; i++)
                for (j=0; j<My; j++)
                  coord_noeud(i,j,k,2)=z;
            }
          for (i=0; i<Mx; i++)
            for (j=0; j<My; j++)
              coord_noeud(i,j,Nz,2)=origine_(2)+longueurs_(2);
        }
    }
  else
    {
      int Iz=Nz/2;
      if( (Iz*2)==Nz )
        pas_(2)=0.5*longueurs_(2)*(facteurs_(2)-1)/(pow(facteurs_(2),Iz)-1);
      else
        {
          if(Iz)
            {
              Iz+=1;
              pas_(2)=0.5*longueurs_(2)/( (pow(facteurs_(2),Iz-1)-1)/(facteurs_(2)-1)
                                          +0.5*pow(facteurs_(2),Iz) );
            }
          else
            {
              Iz+=1;
              pas_(2)=longueurs_(2);
            }
        }
      double dz=pas_(2);
      double z=0;
      for (k=0; k<Iz; k++)
        {
          tmp=(k*100)/Iz;
          if(tmp > pourcent)
            {
              pourcent=tmp;
              Cerr << "It was created " << pourcent << "% of coordz" << flush;
            }
          for (i=0; i<Mx; i++)
            for (j=0; j<My; j++)
              {
                coord_noeud(i,j,k,2)=origine_(2)+z;
                coord_noeud(i,j,Nz-k,2)=origine_(2)+longueurs_(2)-z;
              }
          z+=dz;
          dz*=facteurs_(2);
        }
      if( (Iz*2)==Nz )
        for (i=0; i<Mx; i++)
          for (j=0; j<My; j++)
            coord_noeud(i,j,Iz,2)=0.5*(coord_noeud(i,j,Iz+1,2)
                                       +coord_noeud(i,j,Iz-1,2));
    }


  // PaQ : pour pouvoir passer d'un maillage VDF a VEF suivant tetraedriser homogene

  if(rep_VEF)
    {
      for (j=1; j<My; j+=2)
        for (i=0; i<Mx; i++)
          for (k=0; k<Mz; k++)
            coord_noeud(i,j,k,1)=coord_noeud(i,j-1,k,1)+0.5*(coord_noeud(i,j+1,k,1)-coord_noeud(i,j-1,k,1));
    }


  Cerr << finl;
  Cerr << "Step of mesh ended " << finl;
}


/*! @brief Type le pave suivant la dimension d'espace et suivant le repere de coordonnees demande (cylindrique
 *
 *     ou cartesien)
 *     En dimension 1      : le pave est maille avec des segments
 *     En dimension 2 (axi): la pave est maille avec des rectangle (axi)
 *     En dimension 3 (axi): la pave est maille avec des hexaedres (axi)
 *
 */
void Pave::typer_()
{
  switch(dimension)
    {
    case 1:
      Domaine::typer("Segment");
      break;
    case 2:
      if (axi)
        Domaine::typer("Rectangle_Axi");
      else if (bidim_axi)
        Domaine::typer("Rectangle_2D_Axi");
      else
        Domaine::typer("Rectangle");
      break;
    case 3:
      if (axi)
        Domaine::typer("Hexaedre_Axi");
      else
        Domaine::typer("Hexaedre");
      break;
    default :
      throw;
    }
}

/*! @brief Lit les longueurs LX LY [LZ] du jeu de donnee a partir d'un flot d'entree.
 *
 *     Methode appelee par readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @throws La Longueur est en nombre de tour en axi, comprise entre 0 et 1
 */
void Pave::lire_longueurs(Entree& is)
{
  int i;
  for(i=0; i< dimension; i++)
    is >> longueurs_(i);
  if(axi)
    {
      if(longueurs_(1) >1.+epsilon())
        {
          Cerr << "The length \"Longueurs\" is in number of turns in axisymmetric" << finl;
          Cerr << "It must be between 0 and 1" << finl;
          exit();
        }
      else if(longueurs_(1)==1.)
        tour_complet=true;
    }
}

/*! @brief Lit le nombre de noeuds dans le jeu de donnee a partir d'un flot d'entree et construit les noeuds.
 *
 *     Methode appelee par readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @throws en coordonnees axi il faut lire les longueurs d'abord
 * @throws dimension d'espace non prevue
 */
void Pave::lire_noeuds(Entree& is)
{
  int i, j, k;
  if( (axi) && (longueurs_(0)==0.))
    {
      Cerr<< "You must first read the length in axisymmetric!" << finl;
      exit();
    }
  for(i=0; i< dimension; i++)
    is >> nb_noeuds_(i);
  if (min_array(nb_noeuds_)<2)
    {
      Cerr << "\nError: The number of nodes in directions Nx and Ny (and Nz) for 'Pave " << nom_ << "' must be greater than 1." << finl;
      Cerr << "If you want to define a unique cell in a given direction, set the number of nodes to 2 in that direction." << finl;
      exit();
    }
  if(dimension==1)
    {
      Mx=nb_noeuds_(0);
      Nx=nb_noeuds_(0)-1;
      mes_elems_.resize(Nx,2);
      sommets_.resize(Mx);
      for (i=0; i<Nx; i++)
        {
          maille_sommet(i,0)=numero_sommet(i);
          maille_sommet(i,1)=numero_sommet(i+1);
        }
    }
  else if(dimension==2)
    {
      Mx=nb_noeuds_(0);
      My=nb_noeuds_(1);
      Nx=nb_noeuds_(0)-1;
      Ny=nb_noeuds_(1)-1;
      if(tour_complet) Ny++;
      mes_elems_.resize(Nx*Ny,4);
      sommets_.resize(Mx*My,2);
      for (i=0; i<Nx; i++)
        for (j=0; j<Ny; j++)
          {
            maille_sommet(i,j,0)=numero_sommet(i, j);
            maille_sommet(i,j,1)=numero_sommet(i+1, j);
            maille_sommet(i,j,2)=numero_sommet(i, j+1);
            maille_sommet(i,j,3)=numero_sommet(i+1, j+1);
          }
    }
  else if(dimension==3)
    {
      Mx=nb_noeuds_(0);
      My=nb_noeuds_(1);
      Mz=nb_noeuds_(2);
      Nx=nb_noeuds_(0)-1;
      Ny=nb_noeuds_(1)-1;
      Nz=nb_noeuds_(2)-1;
      if(tour_complet) Ny++;
      mes_elems_.resize(Nx*Ny*Nz,8);
      sommets_.resize(Mx*My*Mz,3);
      for (i=0; i<Nx; i++)
        for ( j=0; j<Ny; j++)
          for ( k=0; k<Nz; k++)
            {
              maille_sommet(i,j,k,0)=numero_sommet(i, j, k);
              maille_sommet(i,j,k,1)=numero_sommet(i+1, j, k);
              maille_sommet(i,j,k,2)=numero_sommet(i, j+1, k);
              maille_sommet(i,j,k,3)=numero_sommet(i+1, j+1, k);
              maille_sommet(i,j,k,4)=numero_sommet(i, j, k+1);
              maille_sommet(i,j,k,5)=numero_sommet(i+1, j, k+1);
              maille_sommet(i,j,k,6)=numero_sommet(i, j+1, k+1);
              maille_sommet(i,j,k,7)=numero_sommet(i+1, j+1, k+1);
            }
    }
  else
    {
      Cerr << "dimension = " << dimension << "not provided " << finl;
      exit();
    }
}

