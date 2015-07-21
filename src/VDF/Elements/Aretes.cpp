/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Aretes.cpp
// Directory:   $TRUST_ROOT/src/VDF/Elements
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Aretes.h>
#include <Zone_VDF.h>

Implemente_instanciable(Aretes,"Aretes",Objet_U);


// printOn et readOn

Sortie& Aretes::printOn(Sortie& s ) const
{
  s << "Aretes " << finl;
  s << faces_ << finl;
  s << type1_ << finl;
  s << type2_ << finl;
  return s;
}

Entree& Aretes::readOn(Entree& s )
{
  return s ;
}

// Description :
// affecte a l'arete numero
// les faces f1, f2, f3, f4
// l'arete est de type2_ type ou :
// type2_ = -1 si arete coin
// type2_ = 0 si arete bord
// type2_ = 1 si arete mixte
// type2_ = 2 si arete interne
// l'arete est de type1_ dir ou :
// type1_ = 0 si arete XY
// type1_ = 1 si arete XZ
// type1_ = 2 si arete YZ
// En dimension 2 il n'y a que des aretes XY
void Aretes::affecter(int& numero_a, int dir, int type, int nb_face,
                      int f1, int f2, int f3, int f4)
{
  int coin = -1 ;
  int bord = 0 ;
  if (type>0)
    {
      if( (f1<nb_face) || (f2<nb_face) || (f3<nb_face) || (f4<nb_face) )
        numero_a++;
      else
        return;
    }
  if (type==bord)
    {
      if( (f1<nb_face) || (f2<nb_face) || (f3<nb_face) )
        numero_a++;
      else
        return;
    }
  if (type==coin)
    {
      if( (f1<nb_face) && (f2<nb_face) && (f3<nb_face) && (f4<nb_face) )
        numero_a++;
      else
        return;
    }

  faces_(numero_a, 0)=f1;
  faces_(numero_a, 1)=f2;
  faces_(numero_a, 2)=f3;
  faces_(numero_a, 3)=f4;
  type1_(numero_a)=dir;
  type2_(numero_a)=type;

  if(type >0)
    {
      assert(faces_(numero_a, 0) !=-1);
      assert(faces_(numero_a, 1) !=-1);
      assert(faces_(numero_a, 2) !=-1);
      assert(faces_(numero_a, 3) !=-1);
    }
}

void Aretes::calculer_centre_de_gravite(Zone_VDF& zone)
{
  const IntTab& so = zone.face_sommets();
  const DoubleTab& co = zone.zone().domaine().les_sommets();
  DoubleTab& xa_ = zone.xa();
  int i,j,k;
  int f0=-1,f1=-1,s00,s01,s10,s11;
  int type;
  int nb_aretes = faces_.dimension(0);
  //const IntVect& orient =  zone.orientation();
  // Calcul des ccordonnees de l'arete
  if(dimension==2)
    {
      xa_.resize(nb_aretes,2);
      for(i=0; i<nb_aretes; i++)
        {
          type = type2_(i);
          if((type==2)||(type==1)) // arete interne ou mixte
            {
              f0 = faces_(i,0);
              s00 = so(f0,0);
              s01 = so(f0,1);
              f1 = faces_(i,1);
              s10 = so(f1,0);
              s11 = so(f1,1);
              if((s00==s10)||(s00==s11))
                {
                  xa_(i,0) = co(s00,0);
                  xa_(i,1) = co(s00,1);
                }
              else if((s01==s10)||(s01==s11))
                {
                  xa_(i,0) = co(s01,0);
                  xa_(i,1) = co(s01,1);
                }
              else
                {
                  Cerr<<"Erreur on a pas trouve de sommets communs"<<finl;
                  exit();
                }
            }
          else if((type == 0)||(type == -1)) // arete bord ou coin
            {
              for(j=0; j<4; j++)
                {
                  f0=faces_(i,j);
                  if(f0!=-1) break;
                }
              assert(j<4);
              k=j+1;
              s00 = so(f0,0);
              s01 = so(f0,1);
              for(j=k; j<4; j++)
                {
                  f1=faces_(i,j);
                  if(f1!=-1) break;
                }
              assert(j<4);
              s10 = so(f1,0);
              s11 = so(f1,1);
              if((s00==s10)||(s00==s11))
                {
                  xa_(i,0) = co(s00,0);
                  xa_(i,1) = co(s00,1);
                }
              else if((s01==s10)||(s01==s11))
                {
                  xa_(i,0) = co(s01,0);
                  xa_(i,1) = co(s01,1);
                }
              else
                {
                  Cerr<<"Erreur on a pas trouve de sommets communs"<<finl;
                  exit();
                }
            }
        }
    }
  else if(dimension==3)
    {
      xa_.resize(nb_aretes,3);
      for(i=0; i<nb_aretes; i++)
        {
          type = type2_(i);
          if((type==2)||(type==1)) // arete interne ou mixte
            {
              f0 = faces_(i,0);
              f1 = faces_(i,1);
              int s0,s1,s0j;
              int deux;
              for(j=0; j<4; j++)
                {
                  s0j=so(f0,j);
                  for(k=0; k<4; k++)
                    if(s0j==so(f1,k)) break;
                  if(k<4) break;
                }
              assert(j<4);
              s0 = s0j;
              deux = j+1;
              for(j=deux; j<4; j++)
                {
                  s0j=so(f0,j);
                  for(k=0; k<4; k++)
                    if(s0j==so(f1,k)) break;
                  if(k<4) break;
                }
              assert(j<4);
              s1 = s0j;
              xa_(i,0) = (co(s0,0)+co(s1,0))/2.0;
              xa_(i,1) = (co(s0,1)+co(s1,1))/2.0;
              xa_(i,2) = (co(s0,2)+co(s1,2))/2.0;
            }
          else if((type == 0)||(type == -1)) // arete bord ou coin
            {
              int f,fdeux;
              int s0,s1,s0j;
              int deux;
              for(f=0; f<4; f++)
                {
                  f0=faces_(i,f);
                  if(f0!=-1) break;
                }
              assert(f<4);
              fdeux=f+1;
              for(f=fdeux; f<4; f++)
                {
                  f1=faces_(i,f);
                  if(f1!=-1) break;
                }
              for(j=0; j<4; j++)
                {
                  s0j=so(f0,j);
                  for(k=0; k<4; k++)
                    if(s0j==so(f1,k)) break;
                  if(k<4) break;
                }
              assert(j<4);
              s0 = s0j;
              deux = j+1;
              for(j=deux; j<4; j++)
                {
                  s0j=so(f0,j);
                  for(k=0; k<4; k++)
                    if(s0j==so(f1,k)) break;
                  if(k<4) break;
                }
              assert(j<4);
              s1 = s0j;
              xa_(i,0) = (co(s0,0)+co(s1,0))/2.0;
              xa_(i,1) = (co(s0,1)+co(s1,1))/2.0;
              xa_(i,2) = (co(s0,2)+co(s1,2))/2.0;
            }
        }
    }
}

// Description :
// Dimensionne les tableaux.
void Aretes::dimensionner(int n)
{
  faces_.resize(n,4);
  type1_.resize(n);
  type2_.resize(n) ;
}

// Description :
// appelee par trier
// Echange les aretes a1 et a2
void Aretes::swap(int a1, int a2)
{
  int tmp;
  tmp = faces_(a1, 0);
  faces_(a1, 0) = faces_(a2, 0);
  faces_(a2, 0)=tmp;
  tmp = faces_(a1, 1);
  faces_(a1, 1) = faces_(a2, 1);
  faces_(a2, 1)=tmp;
  tmp = faces_(a1, 2);
  faces_(a1, 2) = faces_(a2, 2);
  faces_(a2, 2)=tmp;
  tmp = faces_(a1, 3);
  faces_(a1, 3) = faces_(a2, 3);
  faces_(a2, 3)=tmp;
  tmp = type1_(a1);
  type1_(a1)=type1_(a2);
  type1_(a2)=tmp;
  tmp = type2_(a1);
  type2_(a1)=type2_(a2);
  type2_(a2)=tmp;
}
// Description :
// reoordonne le tableaux des aretes
// avec d'abord les aretes coins (elles n'ont que deux faces)
// puis les aretes bord (elles ont trois faces dont deux de bord)
// puis les aretes mixte (elles ont quatre faces dont deux de bord)
// puis les aretes_internes (elles ont quatre faces internes)
void Aretes::trier(int& nb_aretes_coin, int& nb_aretes_bord,
                   int& nb_aretes_mixte, int& nb_aretes_interne)
{
  //
  nb_aretes_coin=nb_aretes_bord=nb_aretes_mixte=nb_aretes_interne=0;
  int coin = -1 ;
  int bord = 0 ;
  int mixte = 1 ;
  int interne = 2 ;
  int nb_aretes = type1_.size();
  int courante=0;
  int arete;
  while( (courante<nb_aretes)&&(type2_(courante)==coin) )
    {
      courante++;
      nb_aretes_coin++;
    }
  for(arete=courante; arete<nb_aretes; arete++)
    {
      if(type2_(arete)==coin)
        {
          swap(arete, courante);
          while( (courante<nb_aretes)&&(type2_(courante)==coin) )
            {
              courante++;
              nb_aretes_coin++;
            }
          //arete=courante;
        }
    }
  while( (courante<nb_aretes)&&(type2_(courante)==bord) )
    {
      courante++;
      nb_aretes_bord++;
    }
  for(arete=courante; arete<nb_aretes; arete++)
    {
      if(type2_(arete)==bord)
        {
          swap(arete, courante);
          assert(type2_(courante) == bord);
          while( (courante<nb_aretes)&&(type2_(courante)==bord) )
            {
              courante++;
              nb_aretes_bord++;
            }
          //arete=courante;
        }
    }
  while( (courante<nb_aretes)&&(type2_(courante)==mixte) )
    {
      courante++;
      nb_aretes_mixte++;
    }
  for(arete=courante; arete<nb_aretes; arete++)
    {
      if(type2_(arete)==mixte)
        {
          swap(arete, courante);
          assert(faces_(courante, 0) !=-1);
          assert(faces_(courante, 1) !=-1);
          assert(faces_(courante, 2) !=-1);
          assert(faces_(courante, 3) !=-1);
          while( (courante<nb_aretes)&&(type2_(courante)==mixte) )
            {
              courante++;
              nb_aretes_mixte++;
            }
          //arete=courante;
        }
    }
  while( (courante<nb_aretes)&&(type2_(courante)==interne) )
    {
      courante++;
      nb_aretes_interne++;
    }
  for(arete=courante; arete<nb_aretes; arete++)
    {
      if(type2_(arete)==interne)
        {
          swap(arete, courante);
          assert(faces_(courante, 0) !=-1);
          assert(faces_(courante, 1) !=-1);
          assert(faces_(courante, 2) !=-1);
          assert(faces_(courante, 3) !=-1);
          while( (courante<nb_aretes)&&(type2_(courante)==interne) )
            {
              courante++;
              nb_aretes_interne++;
            }
          //arete=courante;
        }
    }
}

void Aretes::trier_pour_debog(int& nb_aretes_coin, int& nb_aretes_bord,
                              int& nb_aretes_mixte, int& nb_aretes_interne,const DoubleTab&
                              xv)
{

  ArrOfDouble XVref(dimension),XVref2(dimension);
  int nb_aretes = type1_.size();
  int arete;
  for (int boucle=0; boucle<3; boucle++)
    {
      int deb=nb_aretes - nb_aretes_interne;
      int fin=nb_aretes;
      if (boucle==1)
        {
          fin=deb;
          deb-=nb_aretes_mixte;
        }
      else if (boucle==2)
        {
          fin=deb;
          deb-=  nb_aretes_bord;
        }
      for ( arete=deb; arete<fin; arete++)
        {
          if ((boucle==2)&&(type2_(arete)!=0))
            {
              Cerr<<"gros pb "<<arete<<finl;
              exit();
            }
          int ref=faces_(arete,0);
          for (int i=0; i<dimension; i++) XVref(i)=xv(ref,i);
          int marq=-1;
          int ref2;
          for (int arete2=arete; arete2<fin; arete2++)
            {
              ref2=faces_(arete2,0);
              for (int i=0; i<dimension; i++) XVref2(i)=xv(ref2,i);
              int test=-1;
              //              int testsa=-1;
              if (dimension==2)
                {
                  if (sup_strict(XVref2(1),XVref(1))) test=1;
                  else if ((XVref2(1)==XVref(1))&&(sup_strict(XVref2(0),XVref(0)))) test=0;
                }
              else if (dimension==3)
                {
                  if (XVref2(2)>XVref(2)) test=2;
                  else if  (XVref2(2)==XVref(2))
                    {
                      if (XVref2(1)>XVref(1)) test=1;
                      else if ((XVref2(1)==XVref(1))&&(XVref2(0)>XVref(0))) test=0;
                    }
                }
              if (test!=-1)
                {
                  marq=arete2;
                  XVref=XVref2;
                }
            }
          if (marq!=-1) swap(arete,marq);
        }
    }
  for (arete=nb_aretes - nb_aretes_interne; arete<nb_aretes*0; arete++)
    {
      Cerr <<me()<<" arete "<<arete<<" "<< faces_(arete,0)<<" "<< faces_(arete,1)<<" "<< faces_(arete,2)<<" "<< faces_(arete,3)<<finl;
    }
}
