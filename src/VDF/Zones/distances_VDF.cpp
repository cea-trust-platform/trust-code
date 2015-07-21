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
// File:        distances_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Zones
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////
#include <distances_VDF.h>

void moy_2D_vit(const DoubleVect& vit,int elem,int iori,const Zone_VDF& zone,double& u)
{
  int num1,num2;
  if (iori == 0)
    {
      num1 = zone.elem_faces(elem,1);
      num2 = zone.elem_faces(elem,3);
    }
  else if (iori == 1)
    {
      num1 = zone.elem_faces(elem,0);
      num2 = zone.elem_faces(elem,2);
    }
  else
    {
      Cerr << "valeur de iori " << iori << " impossible en 2D" << finl;
      Process::exit();
      num1=num2=-1;
    }
  u=0.5*(vit(num1)+vit(num2));
}

double norm_2D_vit(const DoubleVect& vit,int elem,int iori,const Zone_VDF& zone,double& u)
{
  double v;
  moy_2D_vit(vit,elem,iori,zone,v);
  v=dabs(v);
  if (v == 0)
    u = 0;
  else
    u = 1;
  return v;
}

double norm_2D_vit(const DoubleVect& vit,int elem,int iori,const Zone_VDF& zone,
                   double u_paroi, double v_paroi, double& u)
{
  double vit_paroi;
  if (iori == 0)
    vit_paroi = v_paroi;
  else if (iori == 1)
    vit_paroi = u_paroi;
  else
    {
      Cerr << "valeur de iori " << iori << " impossible en 2D" << finl;
      Process::exit();
      vit_paroi=0;
    }
  double v;
  double n_v;
  moy_2D_vit(vit,elem,iori,zone,v);


  //YB:30/01/04:
  //Les valeurs du cisaillement parietal sont maintenant signees.
  //En considerant les valeurs signees de la projection de la vitesse sur le plan parietal
  v = v - vit_paroi;
  n_v = dabs(v);

  //Fin modif YB

  if (v == 0)
    u = 0;
  else if (v > 0)
    u = 1;
  else
    u = -1;
  return n_v;
}

void moy_3D_vit(const DoubleVect& vit,int elem,int iori,const Zone_VDF& zone, double& val1,double& val2)
{
  int num1,num2,num3,num4;
  if (iori == 0)
    {
      num1 = zone.elem_faces(elem,1);
      num2 = zone.elem_faces(elem,4);
      num3 = zone.elem_faces(elem,2);
      num4 = zone.elem_faces(elem,5);
    }
  else if (iori == 1)
    {
      num1 = zone.elem_faces(elem,0);
      num2 = zone.elem_faces(elem,3);
      num3 = zone.elem_faces(elem,2);
      num4 = zone.elem_faces(elem,5);
    }
  else if (iori == 2)
    {
      num1 = zone.elem_faces(elem,0);
      num2 = zone.elem_faces(elem,3);
      num3 = zone.elem_faces(elem,1);
      num4 = zone.elem_faces(elem,4);
    }
  else
    {
      Cerr << "valeur de iori " << iori << " impossible en 3D" << finl;
      Process::exit();
      num1=num2=num3=num4=-1;
    }
  val1=0.5*(vit(num1)+vit(num2));
  val2=0.5*(vit(num3)+vit(num4));
}

double norm_3D_vit(const DoubleVect& vit,int elem,int iori,const Zone_VDF& zone, double& val1,double& val2)
{
  moy_3D_vit(vit,elem,iori,zone,val1,val2);
  double v1 = dabs(val1);
  double v2 = dabs(val2);
  double norm_vit = sqrt(v1*v1+v2*v2);
  val1 = v1/(norm_vit+DMINFLOAT);
  val2 = v2/(norm_vit+DMINFLOAT);
  return norm_vit;
}

double norm_3D_vit(const DoubleVect& vit,int elem,int iori,const Zone_VDF& zone,
                   double u_paroi, double v_paroi, double w_paroi, double& val1,double& val2)
{
  double v1,v2,norm_vit;
  moy_3D_vit(vit,elem,iori,zone,val1,val2);
  if (iori == 0)
    {
      v1 = val1 - u_paroi;
      v2 = val2 - w_paroi;
    }
  else if (iori == 1)
    {
      v1 = val1 - u_paroi;
      v2 = val2 - w_paroi;
    }
  else if (iori == 2)
    {
      v1 = val1 - u_paroi;
      v2 = val2 - v_paroi;
    }
  //Fin modif YB

  else
    {
      Cerr << "valeur de iori " << iori << " impossible en 3D" << finl;
      Process::exit();
      v1=v2=0;
    }
  norm_vit = sqrt(v1*v1+v2*v2);
  val1 = v1/(norm_vit+DMINFLOAT);
  val2 = v2/(norm_vit+DMINFLOAT);
  return norm_vit;
}
double norm_vit(const DoubleVect& vit,int elem ,int ori ,const  Zone_VDF& zone,const ArrOfDouble& vit_paroi,ArrOfDouble& val)
{
  if (Objet_U::dimension==3)
    {
      return norm_3D_vit(vit,elem,ori,zone,vit_paroi[0],vit_paroi[1],vit_paroi[2],val[0],val[1]);
    }
  else
    {
      return norm_2D_vit(vit,elem,ori,zone,vit_paroi[0],vit_paroi[1],val[0]);
    }

}
