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

#include <Faces_VDF.h>
#include <Domaine.h>

Implemente_instanciable(Faces_VDF,"Faces_VDF",Faces);

//// printOn
//
Sortie& Faces_VDF::printOn(Sortie& s) const
{
  return Faces::printOn(s) ;
}


//// readOn
//

Entree& Faces_VDF::readOn(Entree& s)
{
  return Faces::readOn(s) ;
}


void Faces_VDF::calculer_orientation(IntVect& tab_orientation,
                                     int& nx, int& ny, int& nz)
{
  tab_orientation.resize(nb_faces_tot());
  orientation_.ref(tab_orientation);
  nx=ny=nz=0;

  const Zone& mazone=zone();
  const Domaine& dom=mazone.domaine();
  double dx=0, dy=0, dz=1.e30;

  for(int face=0; face<nb_faces_tot(); face++)
    {
      if(dimension == 2)
        {
          dx = std::fabs(dom.coord(sommet(face, 1), 0)-dom.coord(sommet(face, 0), 0));
          dy = std::fabs(dom.coord(sommet(face, 1), 1)-dom.coord(sommet(face, 0), 1));
        }
      else if(dimension == 3)
        {
          if (nb_som_faces() != 4 )
            {
              Cerr << "We found a VDF face with nb_som_faces = " << nb_som_faces() << ". Aren't you trying to read a VEF mesh in VDF ?" << finl;
              Cerr << "There is a problem with mesh or with chosen discretization." << finl;
              exit();
            }
          dx = std::fabs(dom.coord(sommet(face, 3), 0)-dom.coord(sommet(face, 0), 0));
          dy = std::fabs(dom.coord(sommet(face, 3), 1)-dom.coord(sommet(face, 0), 1));
          dz = std::fabs(dom.coord(sommet(face, 3), 2)-dom.coord(sommet(face, 0), 2));
        }
      if(axi)
        {
          if(dy<0) dy+=2.*M_PI;
        }
      if ( (dx<=dy) && (dx<=dz) )
        {
          tab_orientation(face)=0;
          nx++;
        }
      else if ( (dy<=dx) && (dy<=dz) )
        {
          tab_orientation(face)=1;
          ny++;
        }
      else if ( (dz<=dx) && (dz<=dy) )
        {
          tab_orientation(face)=2;
          nz++;
        }
    }
}

void Faces_VDF::reordonner(IntVect& tab_orientation)
{
  const Zone& mazone=zone();
  int face, derniere, courante, ori;

  int nb_faces_bord=mazone.nb_faces_bord();
  int nb_faces_raccord=mazone.nb_faces_raccord();
  int nb_faces_int=mazone.nb_faces_int();

  // On ne trie que les faces interieures :
  //#     derniere+=(nb_faces_bord+nb_faces_raccord);
  derniere=(nb_faces_bord+nb_faces_raccord);
  derniere+=nb_faces_int;
  courante=derniere;
  derniere=nb_faces();
  // On trie les  faces non frontieres en fonction de leur orientation

  for(ori=0; ori<dimension; ori++)
    {
      while( (courante<derniere)&&(tab_orientation(courante)==ori) )
        courante++;
      for(face=courante; face<derniere; face++)
        {
          if(tab_orientation(face)==ori)
            {
              swap(face, courante);
              while( (courante<derniere)&&(tab_orientation(courante)==ori) )
                courante++;
              //face=courante;
            }
        }
    }
}

void Faces_VDF::reordonner_pour_debog(IntVect& tab_orientation)
{
  const Zone& mazone=zone();
  int face, derniere;

  int nb_faces_bord=mazone.nb_faces_bord();
  int nb_faces_raccord=mazone.nb_faces_raccord();
  int nb_faces_int=mazone.nb_faces_int();

  // On ne trie que les faces interieures :
  //#     derniere+=(nb_faces_bord+nb_faces_raccord);
  derniere=(nb_faces_bord+nb_faces_raccord);
  derniere+=nb_faces_int;

  derniere=nb_faces();
  /*
  // On trie les  faces non frontieres en fonction de leur orientation

  for(ori=0; ori<dimension; ori++)
  {
  while( (courante<derniere)&&(orientation(courante)==ori) )
  courante++;
  for(face=courante; face<derniere; face++)
  {
  if(orientation(face)==ori)
  {
  swap(face, courante);
  while( (courante<derniere)&&(orientation(courante)==ori) )
  courante++;
  //face=courante;
  }
  }
  }
  */
  Cerr << "ON FAIT UN TRI EXTREMENT CHER POUR AVOIR DES FACES COMME EN SEQUENTIEL"<<finl;
  DoubleTab xv;
  calculer_centres_gravite(xv);
  int ok=0,ordre;
  int marq=0;
  //for(marq=(dimension-1)*0;marq>-1*0;marq--)
  for(marq=0; marq<dimension; marq++)
    {
      ok=0;
      Cerr << " ON recommence direction"<<marq<<finl;
      while(ok==0)
        {
          Cerr<<"on recommence"<<finl;
          ok=1;
          for (face=nb_faces_bord+nb_faces_raccord; face<derniere-1; face++)
            {
              //assert(dimension==3);
              ordre=-1;
              if (xv(face,marq)>xv(face+1,marq))
                {
                  ordre=marq;
                  //for (int marq2=dimension-1;marq2<marq;marq2++)
                  for (int marq2=marq+1; marq2<dimension; marq2++)
                    if (xv(face,marq2)<xv(face+1,marq2)) ordre=-1;
                }
              //  else if ((xv(face,1)>xv(face+1,1))&&(marq>0)) ordre=1;
              //else if ((xv(face,0)>xv(face+1,0))&&(marq>1)) ordre=0;
              if (ordre!=-1)
                {
                  // il faut echanger face et face+1
                  //Cerr<<"on echange "<<face <<" et " <<face+1 <<" ordre "<<ordre<<" "<<xv(face,ordre)<<" "<<xv(face+1,ordre)<<finl;
                  ok=0;
                  swap(face,face+1);
                  double x;
                  for (int yy=0; yy<dimension; yy++)
                    {
                      x=xv(face,yy);
                      xv(face,yy)=xv(face+1,yy);
                      xv(face+1,yy)=x;
                    }
                }
            }
        }
    }
}

void Faces_VDF::swap(int fac1, int fac2)
{
  assert(fac1 < nb_faces());
  assert(fac2 < nb_faces());
  int tmp=voisin(fac1,0);
  voisin(fac1,0)=voisin(fac2,0);
  voisin(fac2,0)=tmp;
  tmp=voisin(fac1,1);
  voisin(fac1,1)=voisin(fac2,1);
  voisin(fac2,1)=tmp;
  for(int som=0; som<nb_som_faces(); som++)
    {
      tmp=sommet(fac1, som) ;
      sommet(fac1,som)=sommet(fac2,som);
      sommet(fac2,som)=tmp;
    }
  tmp=orientation_(fac1);
  orientation_(fac1)=orientation_(fac2);
  orientation_(fac2)=tmp;
}
