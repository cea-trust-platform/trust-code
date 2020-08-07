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
// File:        distances_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Zones
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////
#include <distances_VEF.h>
#include <Domaine.h>
#include <Motcle.h>

double norm_2D_vit1(const DoubleTab& vit,int num1,int num2,int fac,const Zone_VEF& zone,double& u)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();

  // fac numero de la face a paroi fixe
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);

  double v1=(vit(num1,0)+vit(num2,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1))/nfac;
  double v = vitesse_tangentielle(v1,v2,r0,r1);
  if (v==0)
    u = 0;
  else
    u = 1;
  return v;
}

double norm_2D_vit1(const DoubleTab& vit,int num1,int num2,int fac,const Zone_VEF& zone,double& val1, double& val2)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();

  // fac numero de la face a paroi fixe
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);

  double v1=(vit(num1,0)+vit(num2,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1))/nfac;
  double v = vitesse_tangentielle(v1,v2,r0,r1);
  double psc = r0*v1+r1*v2;

  // val1 et val2 sont les vitesses tangentielles
  val1=(v1-psc*r0)/(v+DMINFLOAT);
  val2=(v2-psc*r1)/(v+DMINFLOAT);

  return v;
}


double norm_2D_vit1_lp(const DoubleTab& vit,int fac,int num1,int num2,const Zone_VEF& zone,double& val1, double& val2)
{
  // PQ : 03/03 : Definition de la vitesse tangentielle a une paroi
  // obtenue par "moyennage" des vitesses aux faces associees a des elements standards
  // (dont on suppose le bon comportement suivant la loi log)

  const DoubleTab& face_normale = zone.face_normales();
  double c1,c2;
  c1=0.;
  c2=0.;

  //  int rang1 = rang_elem_non_std(num1);
  //  if (rang1<0) c1=1.;
  //  int rang2 = rang_elem_non_std(num2);
  //  if (rang2<0) c2=1.;

  c1=1.;
  c2=1.;

  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);

  double v0=(c1*vit(num1,0)+c2*vit(num2,0))/(c1+c2);
  double v1=(c1*vit(num1,1)+c2*vit(num2,1))/(c1+c2);

  double psc = r0 * v0 + r1 * v1;
  double norm_vit = vitesse_tangentielle(v0,v1,r0,r1);
  // composantes du vecteur tangent normalise
  val1 = (v0-psc*r0)/(norm_vit+DMINFLOAT);
  val2 = (v1-psc*r1)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_2D_vit1(const DoubleTab& vit,int num1,int num2,int num3,int fac,const Zone_VEF& zone,double& u)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();

  // fac numero de la face a paroi fixe
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1))/nfac;
  double v = vitesse_tangentielle(v1,v2,r0,r1);
  if (v==0)
    u = 0;
  else
    u = 1;
  return v;
}

double norm_2D_vit1(const DoubleTab& vit,int num1,int num2,int num3,int num4,int fac,const Zone_VEF& zone,double& u)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();

  // fac numero de la face a paroi fixe
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0)+vit(num4,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1)+vit(num4,1))/nfac;
  double v = vitesse_tangentielle(v1,v2,r0,r1);
  if (v==0)
    u = 0;
  else
    u = 1;
  return v;
}


double norm_2D_vit2(const DoubleTab& vit,int num1,int num2,int fac,const Zone_VEF& zone, double& u)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();
  // fac numero de la face a paroi defilante
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);
  double v1=(vit(num1,0)+vit(num2,0)-2.*vit(fac,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)-2.*vit(fac,1))/nfac;
  double v = vitesse_tangentielle(v1,v2,r0,r1);
  if (v==0)
    u = 0;
  else
    u = 1;
  return v;
}

double norm_2D_vit2(const DoubleTab& vit,int num1,int num2,int num3,int fac,const Zone_VEF& zone, double& u)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();
  // fac numero de la face a paroi defilante
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0)-3.*vit(fac,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1)-3.*vit(fac,1))/nfac;
  double v = vitesse_tangentielle(v1,v2,r0,r1);
  if (v==0)
    u = 0;
  else
    u = 1;
  return v;
}

double norm_2D_vit2(const DoubleTab& vit,int num1,int num2,int num3,int num4,int fac,const Zone_VEF& zone,double& u)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();

  // fac numero de la face a paroi fixe
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0)+vit(num4,0)-4.*vit(fac,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1)+vit(num4,1)-4.*vit(fac,1))/nfac;
  double v = vitesse_tangentielle(v1,v2,r0,r1);
  if (v==0)
    u = 0;
  else
    u = 1;
  return v;
}


double distance_2D(int fac,int elem,const Zone_VEF& zone)
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

  return dabs(r0*(x1-x0)+r1*(y1-y0));
}

double norm_2D_vit1_k(const DoubleTab& vit,int fac,int num1,
                      const Zone_VEF& zone,
                      double& val1,double& val2)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi fixe
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);
  double v1=vit(num1,0);
  double v2=vit(num1,1);
  double norm_vit = vitesse_tangentielle(v1,v2,r0,r1);

  double psc = r0*v1+r1*v2;

  // val1 et val2 sont les vitesses tangetentielles
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);


  return norm_vit;
}
double norm_3D_vit1(const DoubleTab& vit,int fac,int num1,
                    const Zone_VEF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi fixe
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);
  double v1=vit(num1,0);
  double v2=vit(num1,1);
  double v3=vit(num1,2);
  double norm_vit = vitesse_tangentielle(v1,v2,v3,r0,r1,r2);

  double psc = r0*v1+r1*v2+r2*v3;
  // val1,val2 val3 sont les vitesses tangentielles
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);
  val3=(v3-psc*r2)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_3D_vit1(const DoubleTab& vit,int fac,int num1,int num2,int num3,
                    const Zone_VEF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();
  // fac numero de la face a paroi fixe
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1))/nfac;
  double v3=(vit(num1,2)+vit(num2,2)+vit(num3,2))/nfac;
  double norm_vit = vitesse_tangentielle(v1,v2,v3,r0,r1,r2);
  // val1,val2 val3 sont les vitesses tangentielles
  double psc = r0*v1+r1*v2+r2*v3;
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);
  val3=(v3-psc*r2)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_3D_vit1(const DoubleTab& vit,int fac,int num1,int num2,int num3,int num4,
                    const Zone_VEF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();
  // fac numero de la face a paroi fixe
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0)+vit(num4,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1)+vit(num4,1))/nfac;
  double v3=(vit(num1,2)+vit(num2,2)+vit(num3,2)+vit(num4,2))/nfac;
  double norm_vit = vitesse_tangentielle(v1,v2,v3,r0,r1,r2);
  // val1,val2 val3 sont les vitesses tangentielles
  double psc = r0*v1+r1*v2+r2*v3;
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);
  val3=(v3-psc*r2)/(norm_vit+DMINFLOAT);
  return norm_vit;
}
double norm_3D_vit1(const DoubleTab& vit,int fac,int num1,int num2,int num3,int num4,
                    int num5,const Zone_VEF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();
  // fac numero de la face a paroi fixe
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0)+vit(num4,0)+vit(num5,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1)+vit(num4,1)+vit(num5,1))/nfac;
  double v3=(vit(num1,2)+vit(num2,2)+vit(num3,2)+vit(num4,2)+vit(num5,2))/nfac;
  double norm_vit = vitesse_tangentielle(v1,v2,v3,r0,r1,r2);
  // val1,val2 val3 sont les vitesses tangentielles
  double psc = r0*v1+r1*v2+r2*v3;
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);
  val3=(v3-psc*r2)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_2D_vit2_k(const DoubleTab& vit,int fac,int num1,
                      const Zone_VEF& zone,
                      double& val1,double& val2)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi defilante
  double r0,r1;
  calcule_r0r1(face_normale,fac,r0,r1);

  double v1=vit(num1,0)-vit(fac,0);
  double v2=vit(num1,1)-vit(fac,1);
  double norm_vit = vitesse_tangentielle(v1,v2,r0,r1);
  // val1 et val2 sont les vitesses tangetentielles
  double psc = r0*v1+r1*v2;
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);

  return norm_vit;
}
double norm_3D_vit2(const DoubleTab& vit,int fac,int num1,
                    const Zone_VEF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  // fac numero de la face a paroi defilante
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);
  double v1=vit(num1,0)-vit(fac,0);
  double v2=vit(num1,1)-vit(fac,1);
  double v3=vit(num1,2)-vit(fac,2);
  double norm_vit = vitesse_tangentielle(v1,v2,v3,r0,r1,r2);
  // val1,val2 val3 sont les vitesses tangentielles
  double psc = r0*v1+r1*v2+r2*v3;
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);
  val3=(v3-psc*r2)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_3D_vit2(const DoubleTab& vit,int fac,int num1,int num2,int num3,
                    const Zone_VEF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();
  // fac numero de la face a paroi defilante
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);
  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0)-(nfac-1)*vit(fac,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1)-(nfac-1)*vit(fac,1))/nfac;
  double v3=(vit(num1,2)+vit(num2,2)+vit(num3,2)-(nfac-1)*vit(fac,2))/nfac;
  double norm_vit = vitesse_tangentielle(v1,v2,v3,r0,r1,r2);
  // val1,val2 val3 sont les vitesses tangentielles
  double psc = r0*v1+r1*v2+r2*v3;
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);
  val3=(v3-psc*r2)/(norm_vit+DMINFLOAT);
  return norm_vit;
}

double norm_3D_vit1_lp(const DoubleTab& vit,int fac,int num1,int num2,int num3,
                       const Zone_VEF& zone,
                       double& val1,double& val2,double& val3)
{
  // PQ : 03/03 : Definition de la vitesse tangentielle a une paroi
  // obtenue par "moyennage" des vitesses aux faces associees a des elements standards
  // (dont on suppose le bon comportement suivant la loi log)

  const DoubleTab& face_normale = zone.face_normales();
  //  const IntVect& rang_elem_non_std = zone.rang_elem_non_std();
  double c1,c2,c3;
  c1=0.;
  c2=0.;
  c3=0.;

  //  int rang1 = rang_elem_non_std(num1);
  //  if (rang1<0) c1=1.;
  //  int rang2 = rang_elem_non_std(num2);
  //  if (rang2<0) c2=1.;
  //  int rang3 = rang_elem_non_std(num3);
  //  if (rang3<0) c3=1.;

  c1=1.;
  c2=1.;
  c3=1.;

  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);
  double v0=(c1*vit(num1,0)+c2*vit(num2,0)+c3*vit(num3,0))/(c1+c2+c3);
  double v1=(c1*vit(num1,1)+c2*vit(num2,1)+c3*vit(num3,1))/(c1+c2+c3);
  double v2=(c1*vit(num1,2)+c2*vit(num2,2)+c3*vit(num3,2))/(c1+c2+c3);

  double psc = r0 * v0 + r1 * v1 + r2 * v2;
  double norm_vit = vitesse_tangentielle(v0,v1,v2,r0,r1,r2);
  // composantes du vecteur tangent normalise
  val1 = (v0-psc*r0)/(norm_vit+DMINFLOAT);
  val2 = (v1-psc*r1)/(norm_vit+DMINFLOAT);
  val3 = (v2-psc*r2)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double norm_3D_vit2(const DoubleTab& vit,int fac,int num1,int num2,int num3,int num4,
                    const Zone_VEF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();
  // fac numero de la face a paroi defilante
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);

  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0)+vit(num4,0)-nfac*vit(fac,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1)+vit(num4,1)-nfac*vit(fac,1))/nfac;
  double v3=(vit(num1,2)+vit(num2,2)+vit(num3,2)+vit(num4,2)-nfac*vit(fac,2))/nfac;
  double norm_vit = vitesse_tangentielle(v1,v2,v3,r0,r1,r2);
  // val1,val2 val3 sont les vitesses tangentielles
  double psc = r0*v1+r1*v2+r2*v3;
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);
  val3=(v3-psc*r2)/(norm_vit+DMINFLOAT);

  return norm_vit;
}
double norm_3D_vit2(const DoubleTab& vit,int fac,int num1,int num2,int num3,int num4,
                    int num5, const Zone_VEF& zone,
                    double& val1,double& val2,double& val3)
{
  const DoubleTab& face_normale = zone.face_normales();
  const Zone& zone_geom = zone.zone();
  int nfac = zone_geom.nb_faces_elem();
  // fac numero de la face a paroi defilante
  double r0,r1,r2;
  calcule_r0r1r2(face_normale,fac,r0,r1,r2);

  double v1=(vit(num1,0)+vit(num2,0)+vit(num3,0)+vit(num4,0)+vit(num5,0)-nfac*vit(fac,0))/nfac;
  double v2=(vit(num1,1)+vit(num2,1)+vit(num3,1)+vit(num4,1)+vit(num5,1)-nfac*vit(fac,1))/nfac;
  double v3=(vit(num1,2)+vit(num2,2)+vit(num3,2)+vit(num4,2)+vit(num5,2)-nfac*vit(fac,2))/nfac;
  double norm_vit = vitesse_tangentielle(v1,v2,v3,r0,r1,r2);
  // val1,val2 val3 sont les vitesses tangentielles
  double psc = r0*v1+r1*v2+r2*v3;
  val1=(v1-psc*r0)/(norm_vit+DMINFLOAT);
  val2=(v2-psc*r1)/(norm_vit+DMINFLOAT);
  val3=(v3-psc*r2)/(norm_vit+DMINFLOAT);

  return norm_vit;
}

double distance_3D(int fac,int elem,const Zone_VEF& zone)
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

  return dabs(r0*(x1-x0)+r1*(y1-y0)+r2*(z1-z0));
}

DoubleVect& calcul_longueur_filtre(DoubleVect& longueur_filtre, const Motcle& methode, const Zone_VEF& zone)
{
  int nbr_element=zone.nb_elem_tot();
  int element;
  int dim=Objet_U::dimension;
  const Zone& zone_geom = zone.zone();

  if (longueur_filtre.size() != nbr_element)
    {
      Cerr << "erreur dans la taille du DoubleVect valeurs de la longueur du filtre" << finl;
      Process::exit();
    }

  if (methode == Motcle("volume") || methode == Motcle("volume_sans_lissage"))  // racine cubique du volume
    {
      longueur_filtre=-1.;

      const DoubleVect& volume = zone.volumes();
      for (element=0; element<nbr_element; element ++)
        {
          longueur_filtre(element) = exp(log(volume[element])/double(dim));
        }
    }
  else if (methode == Motcle("arete"))  // recherche de la plus longue arete d'un element
    {
      longueur_filtre=-1.;

      const IntTab& les_sommets = zone_geom.les_elems();
      int som1,som2,som_1,som_2;
      double distance;

      for (element=0; element<nbr_element; element ++)
        for (som1=0; som1<dim; som1++)
          for (som2=som1; som2<dim+1; som2++)
            {
              som_1 = les_sommets(element, som1);
              som_2 = les_sommets(element, som2);

              distance = distance_sommets(som_1, som_2, zone);
              distance/=sqrt(2.);
              longueur_filtre(element) = max(longueur_filtre(element), distance);
            }
    }
  else if (methode ==  Motcle("Scotti"))  // application de Scotti a un pseudo-cube
    {
      longueur_filtre=-1.;

      const IntTab& les_sommets = zone_geom.les_elems();
      int som0,som1,som2,som3;
      int som_0,som_1,som_2,som_3;

      if(Objet_U::dimension==2)  // On revient a la racine carre du volume
        {
          int nbr_elementb=zone.nb_elem_tot();
          int elementb;
          //int dim=Objet_U::dimension;
          const DoubleVect& volume = zone.volumes();

          for (elementb=0; elementb<nbr_elementb; elementb ++)
            {
              longueur_filtre(elementb) = exp(log(volume[elementb])/double(dim));
            }
        }
      else
        {
          ArrOfDouble psc(4);
          ArrOfDouble dist(3);
          double dist_tot,dist_min,dist_max,dist_moy;
          double a1,a2,f_scotti;

          som_0=0;
          som_1=0;
          som_2=0;
          som_3=0;

          for (element=0; element<nbr_element; element ++)
            {
              som0 = les_sommets(element, 0);
              som1 = les_sommets(element, 1);
              som2 = les_sommets(element, 2);
              som3 = les_sommets(element, 3);

              psc(0) = som_pscal(som0,som1,som2,som3,zone);
              psc(1) = som_pscal(som1,som0,som2,som3,zone);
              psc(2) = som_pscal(som2,som0,som1,som3,zone);
              psc(3) = som_pscal(som3,som0,som1,som2,zone);

              const int indice_min = imin_array(psc);
              if(indice_min==0)
                {
                  som_0=som0;
                  som_1=som1;
                  som_2=som2;
                  som_3=som3;
                }
              if(indice_min==1)
                {
                  som_0=som1;
                  som_1=som0;
                  som_2=som2;
                  som_3=som3;
                }
              if(indice_min==2)
                {
                  som_0=som2;
                  som_1=som0;
                  som_2=som1;
                  som_3=som3;
                }
              if(indice_min==3)
                {
                  som_0=som3;
                  som_1=som0;
                  som_2=som1;
                  som_3=som2;
                }

              dist(0)=distance_sommets(som_0,som_1,zone);
              dist(1)=distance_sommets(som_0,som_2,zone);
              dist(2)=distance_sommets(som_0,som_3,zone);

              dist_tot=dist(0)+dist(1)+dist(2);

              dist_min=min_array(dist);
              dist_max=max_array(dist);
              dist_moy=dist_tot-dist_min-dist_max;

              a1=dist_min/dist_max;
              a2=dist_moy/dist_max;

              f_scotti=cosh(sqrt((4./27.)*( log(a1)*log(a1) - log(a1)*log(a2) + log(a2)*log(a2) )));

              longueur_filtre(element) = f_scotti * exp(log(dist_min*dist_max*dist_moy)/3.);
            }
        }//3D
    }
  else
    {
      Cerr << "calcul_longueur_filtre.cpp n'a pas reconnu l'argument : " << methode << finl;
      Cerr << "les arguments possibles sont : \"volume\", \"volume_sans_lissage\", \"Scotti\", \"arete\"." << finl;
      Process::exit();

    }


  if ( ! (methode == Motcle("volume_sans_lissage")) )  // processus de "regularisation"
    {
      const Domaine& dom=zone.zone().domaine();
      const IntTab& les_sommets = zone_geom.les_elems();
      int nb_sommet = zone.nb_som_tot();
      ArrOfDouble longueur_filtre_sommet(nb_sommet);
      int som1;
      int som_0,som_1;

      longueur_filtre_sommet=-1.;

      for (element=0; element<nbr_element; element ++)
        for (som1=0; som1<dim+1; som1++)
          {
            som_0 = les_sommets(element, som1);
            som_1 = dom.get_renum_som_perio(som_0);
            longueur_filtre_sommet(som_1) = max(longueur_filtre(element), longueur_filtre_sommet(som_1));
          }

      longueur_filtre=-1.;

      for (element=0; element<nbr_element; element ++)
        for (som1=0; som1<dim+1; som1++)
          {
            som_0 = les_sommets(element, som1);
            som_1 = dom.get_renum_som_perio(som_0);
            longueur_filtre(element) = max (longueur_filtre(element), longueur_filtre_sommet(som_1));
          }
    }

  assert(nbr_element == 0 || min_array(longueur_filtre)>0.);

  return longueur_filtre;
}

double distance_sommets(const int sommet1, const int sommet2, const Zone_VEF& zone)
{
  const Zone& zone_geom = zone.zone();
  const DoubleTab& xs = zone_geom.domaine().coord_sommets();
  double x1 = xs(sommet1,0);
  double y1 = xs(sommet1,1);
  double x2 = xs(sommet2,0);
  double y2 = xs(sommet2,1);

  if (Objet_U::dimension == 3)
    {
      double z1 = xs(sommet1,2);
      double z2 = xs(sommet2,2);
      return sqrt( carre(x2-x1) + carre(y2-y1) + carre(z2-z1) );
    }
  else // en dimension 2
    {
      return sqrt( carre(x2-x1) + carre(y2-y1) );
    }
}

double som_pscal(const int som0, const int som1, const int som2, const int som3, const Zone_VEF& zone)
{
  const Zone& zone_geom = zone.zone();
  const DoubleTab& xs = zone_geom.domaine().coord_sommets();

  ArrOfDouble v1(3),v2(3),v3(3);
  double n1,n2,n3;

  for(int i=0 ; i<Objet_U::dimension ; i++)
    {
      v1(i)=xs(som1,i)-xs(som0,i);
      v2(i)=xs(som2,i)-xs(som0,i);
      v3(i)=xs(som3,i)-xs(som0,i);
    }

  n1=norme_array(v1);
  n2=norme_array(v2);
  n3=norme_array(v3);

  v1/=n1;
  v2/=n2;
  v3/=n3;

  double som_psc=dabs(dotproduct_array(v1,v2))+dabs(dotproduct_array(v2,v3))+dabs(dotproduct_array(v3,v1));
  return som_psc;
}

double norm_vit_lp_k(const DoubleTab& vit,int face,int face_b,const Zone_VEF& zone,ArrOfDouble& val,int is_defilante)
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

double distance_face_elem(int fac,int elem,const Zone_VEF& zone)
{
  int dimension=Objet_U::dimension;
  if (dimension==3)
    return distance_3D(fac,elem,zone);
  else
    return distance_2D(fac,elem,zone);
}

