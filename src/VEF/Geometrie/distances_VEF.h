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

#ifndef distances_VEF_inclus
#define distances_VEF_inclus
#include <Domaine_VEF.h>
// Kokkos
KOKKOS_INLINE_FUNCTION
double distance(int dim,int fac,int elem,CDoubleTabView xp, CDoubleTabView xv, CDoubleTabView face_normale);
KOKKOS_INLINE_FUNCTION
double norm_vit1(int dim, CDoubleTabView vit, int fac, int nfac, const int* num, CDoubleTabView face_normale, double* val);
//double norm_2D_vit1(const DoubleTab& vit,int elem,int num1,int num2,const Domaine_VEF& domaine,double& val1);
double norm_2D_vit1(const DoubleTab& vit,int elem,int num1,int num2,const Domaine_VEF& domaine,double& val1, double& val2);
double norm_2D_vit1_lp(const DoubleTab& vit,int elem,int num1,int num2,const Domaine_VEF& domaine,double& val1,double& val2);
double norm_2D_vit1(const DoubleTab& vit,int elem,int num1,int num2,int num3,const Domaine_VEF& domaine,double& val1);
double norm_2D_vit1(const DoubleTab& vit,int elem,int num1,int num2,int num3,int num4,const Domaine_VEF& domaine,double& val1);
//double norm_2D_vit2(const DoubleTab& vit,int elem,int num1,const Domaine_VEF& domaine,double& val1);
double norm_2D_vit2(const DoubleTab& vit,int elem,int num1,int num2,const Domaine_VEF& domaine,double& val1);
double norm_2D_vit2(const DoubleTab& vit,int elem,int num1,int num2,int num3,const Domaine_VEF& domaine,double& val1);
double distance_2D(int fac,int elem,const Domaine_VEF& domaine);

double norm_3D_vit1(const DoubleTab& vit,int elem,int num1,const Domaine_VEF& domaine,double& val1 ,double& val2,double& val3);
double norm_3D_vit1(const DoubleTab& vit,int elem,int num1,int num2,int num3,const Domaine_VEF& domaine,double& val1 ,double& val2,double& val3);
double norm_3D_vit1(const DoubleTab& vit,int fac,int num1,int num2,int num3,int num4,const Domaine_VEF& domaine,double& val1,double& val2,double& val3);
double norm_3D_vit2(const DoubleTab& vit,int elem,int num1,const Domaine_VEF& domaine,double& val1 ,double& val2,double& val3);
double norm_3D_vit2(const DoubleTab& vit,int elem,int num1,int num2,int num3,const Domaine_VEF& domaine,double& val1 ,double& val2,double& val3);
double norm_3D_vit1_lp(const DoubleTab& vit,int elem,int num1,int num2,int num3,
                       const Domaine_VEF& domaine,
                       double& val1 ,double& val2,double& val3);
double norm_2D_vit2(const DoubleTab& vit,int elem,int num1,int num2,int num3,int num4,const Domaine_VEF& domaine,double& val1);
double norm_3D_vit1(const DoubleTab& vit,int elem,int num1,int num2,int num3,int num4,int num5, const Domaine_VEF& domaine,double& val1 ,double& val2,double& val3);
double norm_3D_vit2(const DoubleTab& vit,int elem,int num1,int num2,int num3,int num4,int num5, const Domaine_VEF& domaine,double& val1 ,double& val2,double& val3);
double norm_3D_vit2(const DoubleTab& vit,int fac,int num1,int num2,int num3,int num4,const Domaine_VEF& domaine,double& val1,double& val2,double& val3);
double distance_3D(int fac,int elem,const Domaine_VEF& domaine);
double distance_face_elem(int fac,int elem,const Domaine_VEF& domaine);
double norm_vit_lp_k(const DoubleTab& vit,int face,int face_b,const Domaine_VEF& domaine,ArrOfDouble& val,int is_defilante);

DoubleVect& calcul_longueur_filtre(DoubleVect& longueur_filtre, const Motcle& methode, const Domaine_VEF& domaine);
double distance_sommets(const int , const int , const Domaine_VEF& );
double som_pscal(const int , const int , const int , const int , const Domaine_VEF& );

// Fonctions inlinees pour optimisation
inline double vitesse_tangentielle(const double v0,const double v1,const double r0,const double r1)
{
  // On prend std::fabs car mathematiquement la valeur est >=0
  return sqrt(std::fabs(carre(v0)+carre(v1)-carre(v0*r0+v1*r1)));
}
inline double vitesse_tangentielle(const double v0,const double v1,const double v2,const double r0,const double r1,const double r2)
{
  // On prend std::fabs car mathematiquement la valeur est >=0
  return sqrt(std::fabs(carre(v0)+carre(v1)+carre(v2)-carre(v0*r0+v1*r1+v2*r2)));
}

inline void calcule_r0r1(const DoubleTab& face_normale, int& fac, double& r0, double& r1)
{
  r0=face_normale(fac,0);
  r1=face_normale(fac,1);
  double tmp=1/sqrt(r0*r0+r1*r1);
  r0*=tmp;
  r1*=tmp;
}

inline void calcule_r0r1r2(const DoubleTab& face_normale, int& fac, double& r0, double& r1, double& r2)
{
  r0=face_normale(fac,0);
  r1=face_normale(fac,1);
  r2=face_normale(fac,2);
  double tmp=1/sqrt(r0*r0+r1*r1+r2*r2);
  r0*=tmp;
  r1*=tmp;
  r2*=tmp;
}

inline double distance_face(int fac,int fac1,const Domaine_VEF& domaine)
{
  int dimension=Objet_U::dimension;
  const DoubleTab& xv = domaine.xv();    // centre de gravite des faces
  const DoubleTab& face_normale = domaine.face_normales();
  double r0,r1;
  double x0=xv(fac,0);
  double y0=xv(fac,1);
  double x1=xv(fac1,0);
  double y1=xv(fac1,1);
  if (dimension==3)
    {
      double r2;
      double z0=xv(fac,2);
      double z1=xv(fac1,2);
      calcule_r0r1r2(face_normale,fac,r0,r1,r2);
      return std::fabs(r0*(x1-x0)+r1*(y1-y0)+r2*(z1-z0));
    }
  else
    {
      calcule_r0r1(face_normale,fac,r0,r1);
      return std::fabs(r0*(x1-x0)+r1*(y1-y0));
    }
}

// Kokkos function (factorize distance_2D and distance_3D functions)
KOKKOS_INLINE_FUNCTION
double distance(int dim,int fac,int elem, CDoubleTabView xp, CDoubleTabView xv, CDoubleTabView face_normale)
{
  double norme=0;
  double ps=0;
  for (int i=0; i<dim; i++)
    {
      double fn_i = face_normale(fac, i);
      norme += fn_i * fn_i;
      ps += fn_i * (xp(elem, i) - xv(fac, i));
    }
  return std::fabs(ps/sqrt(norme));
}
// Kokkos function (factorize norm_2D_vit1 and norm_3D_vit1)
KOKKOS_INLINE_FUNCTION
double norm_vit1(int dim, CDoubleTabView vit, int fac, int nfac, const int* num,
                 CDoubleTabView face_normale,
                 double* val)
{
  // fac numero de la face a paroi fixe
  double r[3] {};
  double norme = 0;
  for (int i=0; i<dim; i++)
    {
      r[i] = face_normale(fac, i);
      norme += r[i] * r[i];
    }
  norme = sqrt(norme);
  for(int i = 0; i < dim; i++)
    r[i] /= norme;

  double v[3] {};
  for (int i=0; i<dim; i++)
    {
      v[i] = 0;
      for (int j = 0; j < dim; j++)
        v[i] += vit(num[j], i);
      v[i] /= nfac;
    }

  double sum_carre=0;
  double psc=0;
  for (int i=0; i<dim; i++)
    {
      sum_carre += carre(v[i]);
      psc += v[i] * r[i];
    }
  double norm_vit = sqrt(std::fabs(sum_carre-carre(psc)));

  // val1,val2 val3 sont les vitesses tangentielles
  for (int i=0; i<dim; i++)
    val[i]=(v[i] - psc*r[i])/(norm_vit + DMINFLOAT);

  return norm_vit;
}

// Kokkos function (similar to nomr_vit1) that fuses norm_2D_vit1_lp and norm_3D_vit1_lp
KOKKOS_INLINE_FUNCTION
double norm_vit1_lp(int dim, CDoubleTabView vit, int fac, int nfac, const int* num,
                    CDoubleTabView face_normale, double* val)
{
  double r[3] {};
  double norme = 0;
  double c[] = {1., 1., 1.};
  double c_sum = 0;
  for (int i=0; i<dim; i++)
    {
      r[i] = face_normale(fac, i);
      norme += r[i] * r[i];
      c_sum += c[i];
    }
  norme = sqrt(norme);
  for(int i = 0; i < dim; i++)
    r[i] /= norme;

  double v[3] {};
  for (int i=0; i<dim; i++)
    {
      v[i] = 0;
      for (int j = 0; j < dim; j++)
        v[i] += c[i] * vit(num[j], i);
      v[i] /= c_sum;
    }

  double sum_carre=0;
  double psc=0;
  for (int i=0; i<dim; i++)
    {
      sum_carre += carre(v[i]);
      psc += v[i] * r[i];
    }
  double norm_vit = sqrt(std::fabs(sum_carre-carre(psc)));

  for (int i=0; i<dim; i++)
    val[i]=(v[i] - psc*r[i])/(norm_vit + DMINFLOAT);

  return norm_vit;
}
#endif
