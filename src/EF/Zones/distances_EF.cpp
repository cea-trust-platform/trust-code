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
//////////////////////////////////////////////////////////////////////////////
//
// File:        distances_EF.cpp
// Directory:   $TRUST_ROOT/src/EF/Zones
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////
#include <distances_EF.h>
#include <Domaine.h>
#include <Motcle.h>

double norm_3D_vit1(const DoubleTab& vit,int fac,int num1,
                    const Zone_EF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi fixe
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);
  int nsom = zone.nb_som_face();
  ArrOfDouble vit_face(3);
  vit_face=0.;
  for(int comp=0; comp<3; comp++)
    {
      for(int jsom=0; jsom<nsom; jsom++)
        {
          int num_som = zone.face_sommets(num1, jsom);
          vit_face[comp]+=vit(num_som,comp);
        }
    }
  double v1=vit_face[0]/nsom;
  double v2=vit_face[1]/nsom;
  double v3=vit_face[2]/nsom;
  double norm_vit = vitesse_tangentielle(v1,v2,v3,r0,r1,r2);

  double psc = r0*v1+r1*v2+r2*v3;
  // val1,val2 val3 sont les vitesses tangentielles
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);
  val3=(v3-psc*r2)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_2D_vit1_k(const DoubleTab& vit,int fac,int num1,
                      const Zone_EF& zone,
                      double& val1,double& val2)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi fixe
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);
  int nsom = zone.nb_som_face();
  ArrOfDouble vit_face(2);
  vit_face=0.;
  for(int comp=0; comp<2; comp++)
    {
      for(int jsom=0; jsom<nsom; jsom++)
        {
          int num_som = zone.face_sommets(num1, jsom);
          vit_face[comp]+=vit(num_som,comp);
        }
    }
  double v1=vit_face[0]/nsom;
  double v2=vit_face[1]/nsom;
  double norm_vit = vitesse_tangentielle(v1,v2,r0,r1);

  double psc = r0*v1+r1*v2;

  // val1 et val2 sont les vitesses tangetentielles
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_3D_vit2(const DoubleTab& vit,int fac,int num1,
                    const Zone_EF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi defilante
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);

  int nsom = zone.nb_som_face();
  ArrOfDouble vit_face(3);
  ArrOfDouble vit_face2(3);
  vit_face=0.;
  vit_face2=0.;
  for(int comp=0; comp<3; comp++)
    {
      for(int jsom=0; jsom<nsom; jsom++)
        {
          int num_som = zone.face_sommets(num1, jsom);
          vit_face[comp]+=vit(num_som,comp);
          int num_som2 = zone.face_sommets(fac, jsom);
          vit_face2[comp]+=vit(num_som2,comp);
        }
    }
  double v1=(vit_face[0]-vit_face2[0])/nsom;
  double v2=(vit_face[1]-vit_face2[1])/nsom;
  double v3=(vit_face[2]-vit_face2[2])/nsom;
  double norm_vit = vitesse_tangentielle(v1,v2,v3,r0,r1,r2);
  // val1,val2 val3 sont les vitesses tangentielles
  double psc = r0*v1+r1*v2+r2*v3;
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);
  val3=(v3-psc*r2)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_2D_vit2_k(const DoubleTab& vit,int fac,int num1,
                      const Zone_EF& zone,
                      double& val1,double& val2)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi defilante
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);

  int nsom = zone.nb_som_face();
  ArrOfDouble vit_face(2);
  ArrOfDouble vit_face2(2);
  vit_face=0.;
  vit_face2=0.;
  for(int comp=0; comp<2; comp++)
    {
      for(int jsom=0; jsom<nsom; jsom++)
        {
          int num_som = zone.face_sommets(num1, jsom);
          vit_face[comp]+=vit(num_som,comp);
          int num_som2 = zone.face_sommets(fac, jsom);
          vit_face2[comp]+=vit(num_som2,comp);
        }
    }
  double v1=(vit_face[0]-vit_face2[0])/nsom;
  double v2=(vit_face[1]-vit_face2[1])/nsom;
  double norm_vit = vitesse_tangentielle(v1,v2,r0,r1);
  // val1 et val2 sont les vitesses tangetentielles
  double psc = r0*v1+r1*v2;
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double distance_2D(int fac,int elem,const Zone_EF& zone)
{
  const DoubleTab& xp = zone.xp();    // centre de gravite des elements
  const DoubleTab& xv = zone.xv();    // centre de gravite des faces

  const DoubleTab& face_normale = zone.face_normales();
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);

  double x0=xv(fac,0);
  double y0=xv(fac,1);
  double x1=xp(elem,0);
  double y1=xp(elem,1);

  return fabs(r0*(x1-x0)+r1*(y1-y0));
}

double distance_3D(int fac,int elem,const Zone_EF& zone)
{
  const DoubleTab& xp = zone.xp();    // centre de gravite des elements
  const DoubleTab& xv = zone.xv();    // centre de gravite des faces
  const DoubleTab& face_normale = zone.face_normales();
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);
  double x0=xv(fac,0);
  double y0=xv(fac,1);
  double z0=xv(fac,2);
  double x1=xp(elem,0);
  double y1=xp(elem,1);
  double z1=xp(elem,2);

  return fabs(r0*(x1-x0)+r1*(y1-y0)+r2*(z1-z0));
}

double norm_vit_lp(const ArrOfDouble& vit,int face,const Zone_EF& zone,ArrOfDouble& val)
{

  const DoubleTab& face_normale = zone.face_normales();
  int dim=Objet_U::dimension;
  ArrOfDouble r(dim);
  double psc,norm_vit;

  for(int i=0; i<dim; i++) r[i]=face_normale(face,i);

  r/=norme_array(r);
  psc = dotproduct_array(r,vit);

  if(dim==3) norm_vit = vitesse_tangentielle(vit[0],vit[1],vit[2],r[0],r[1],r[2]);
  else             norm_vit = vitesse_tangentielle(vit[0],vit[1],r[0],r[1]);

  for(int i=0; i<dim; i++) val[i]=(vit[i]-psc*r[i])/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_vit_lp_k(const DoubleTab& vit,int face,int face_b,const Zone_EF& zone,ArrOfDouble& val,int is_defilante)
{
  int dimension=Objet_U::dimension;
  //assert(face_b<zone.premiere_face_int()); assert a ameliorer ou faces_virt...
  if (is_defilante==0)
    if (dimension==3)
      return norm_3D_vit1(vit,face_b,face,zone,val[0],val[1],val[2]);
    else
      return norm_2D_vit1_k(vit,face_b,face,zone,val[0],val[1]);

  else
    {
      if (dimension==3)
        return norm_3D_vit2(vit,face_b,face,zone,val[0],val[1],val[2]);
      else
        return norm_2D_vit2_k(vit,face_b,face,zone,val[0],val[1]);
    }
}

double distance_face_elem(int fac,int elem,const Zone_EF& zone)
{
  int dimension=Objet_U::dimension;
  if (dimension==3)
    return distance_3D(fac,elem,zone);
  else
    return distance_2D(fac,elem,zone);
}

