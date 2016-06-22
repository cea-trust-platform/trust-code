/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        calcul_spectres.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence/Spectres
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <calcul_spectres.h>
#include <SFichier.h>

// On trouve dans ce fichier differentes procedures necessaires pour le calcul des spectres


void ecrit_spectre(DoubleVect& E,int n, double temps,int a)
{
  Nom fichier = "spectre_";
  //   Nom aa = Nom(a);
  //   fichier += aa;  // 0 = EX, 1 = EY , 2 = EZ , 3 = E
  //   fichier += "_";
  Nom tps = Nom(temps);

  fichier += tps;
  SFichier fic7 (fichier);

  for (int j=0; j<n; j++)
    if (E(j)>1.e-30)
      {
        fic7 << j+1 << "   " << E(j) << finl;
      }
  return;
}

void ecrit_spectre_operateur(DoubleVect& E,int n, double temps, int nb_op,int a, double dt)
{
  Nom fichier = "spectre_";
  Nom op;
  if (nb_op==0)
    op="diff";
  else
    op="conv";
  fichier += op;

  fichier += "_";
  Nom tps = Nom(temps);
  //  Cerr << "tps=" << tps << finl;
  fichier += tps;
  SFichier fic7 (fichier);

  //  Cerr << "dt=" << dt << finl;

  for (int j=0; j<n; j++)
    if (E(j)>1.e-30)
      {
        fic7 << j+1 << "   " << E(j)/dt << finl;
      }
  return;
}

#if 0
void calc_sp_nouveau(DoubleTab& vit_som ,int n, double temps,double& Ec)
{
  /* Permet de calculer le spectre d energie a partir d un champ de vitesse
     exprime au sommet, sans correction pour la periodicite */
  DoubleVect E(n),EX(n),EY(n),EZ(n);
  DoubleTab fft_u(n,n,n+2),fft_v(n,n,n+2),fft_w(n,n,n+2);
  DoubleTab u(n+1,n+1,n+1),v(n+1,n+1,n+1),w(n+1,n+1,n+1);
  DoubleTab uu(n+1,n+1,n+1),vv(n+1,n+1,n+1),ww(n+1,n+1,n+1);
  DoubleVect nE(n);
  DoubleVect fx(n),fy(n),fz(n);
  F77NAME(CALCUL0SPECTRE0AP0CHP0VERIF)(vit_som.addr(),E.addr(),EX.addr(),EY.addr(),EZ.addr(),&n,fft_u.addr(),fft_v.addr(),fft_w.addr(),u.addr(),v.addr(),w.addr(),uu.addr(),vv.addr(),ww.addr(),nE.addr(),fx.addr(),fy.addr(),fz.addr());
  ecrit_spectre(E,n,temps,3);

  return;
}
#endif
void calc_sp_nouveau_2(DoubleTab& vit_som ,int n, double temps,double& Ec,double& Df)
{
  /* Permet de calculer le spectre d energie a partir d un champ de vitesse
     exprime au sommet, sans correction pour la periodicite */
  DoubleVect E(n);
  DoubleTab fft_u(n,n,n+2),fft_v(n,n,n+2),fft_w(n,n,n+2);
  DoubleVect EX(n),EY(n),EZ(n);
  DoubleTab u(n+1,n+1,n+1),v(n+1,n+1,n+1),w(n+1,n+1,n+1);
  DoubleTab uu(n+1,n+1,n+1),vv(n+1,n+1,n+1),ww(n+1,n+1,n+1);
  DoubleVect nE(n);
  DoubleVect fx(n),fy(n),fz(n);
  IntTab  wl(6*n+14),wm(4*n+14),wn(4*n+14),iwork(2*n);

  F77NAME(CALCUL0SPECTRE0AP0CHP0PER)(vit_som.addr(),E.addr(),&n,fft_u.addr(),fft_v.addr(),fft_w.addr(),u.addr(),v.addr(),w.addr(),uu.addr(),vv.addr(),ww.addr(),nE.addr(),EX.addr(),EY.addr(),EZ.addr(),fx.addr(),fy.addr(),fz.addr(),wl.addr(),wm.addr(),wn.addr(),iwork.addr());

  ecrit_spectre(E,n,temps,3);
  calc_Ec(E,n/2,Ec);
  // Calcul de D filtree!!
  int i;
  DoubleVect D(n);
  for (i=0; i<n/2-1; i++)
    D(i) = (i+1)*(i+1)*E(i);
  calc_Ec(D,n/2,Df);
  return;
}

void calc_sp_nouveau_2_operateur(DoubleTab& vit_som ,int n, double temps,int nb_op, double dt, DoubleVect& E)
{
  /* Permet de calculer le spectre d energie a partir d un champ de vitesse
     exprime au sommet, sans correction pour la periodicite */
  DoubleTab fft_u(n,n,n+2),fft_v(n,n,n+2),fft_w(n,n,n+2);
  DoubleVect EX(n),EY(n),EZ(n);
  DoubleTab u(n+1,n+1,n+1),v(n+1,n+1,n+1),w(n+1,n+1,n+1);
  DoubleTab uu(n+1,n+1,n+1),vv(n+1,n+1,n+1),ww(n+1,n+1,n+1);
  DoubleVect nE(n);
  DoubleVect fx(n),fy(n),fz(n);
  IntTab  wl(6*n+14),wm(4*n+14),wn(4*n+14),iwork(2*n);


  F77NAME(CALCUL0SPECTRE0AP0CHP0PER)(vit_som.addr(),E.addr(),&n,fft_u.addr(),fft_v.addr(),fft_w.addr(),u.addr(),v.addr(),w.addr(),uu.addr(),vv.addr(),ww.addr(),nE.addr(),EX.addr(),EY.addr(),EZ.addr(),fx.addr(),fy.addr(),fz.addr(),wl.addr(),wm.addr(),wn.addr(),iwork.addr());
  //  ecrit_spectre_operateur(E,n,temps,nb_op,3,dt);

  return;
}
#if 0
void calc_sp_nouveau_3_vit(DoubleTab& vit_u, DoubleTab& vit_v, DoubleTab& vit_w, int n, double temps,double& Ec,double& Df)
{
  //   Permet de calculer le spectre d energie a partir d un champ de vitesse
  //     exprime au sommet, sans correction pour la periodicite
  /*   cerr << "COUCOU dans calc_sp_nouveau_3_vit" << flush; */
  /*   cerr << "N = " << n << flush; */
  DoubleVect E(n);
  DoubleTab fft_u(n,n,n+2),fft_v(n,n,n+2),fft_w(n,n,n+2);
  DoubleVect EX(n),EY(n),EZ(n);
  DoubleTab u(n+1,n+1,n+1),v(n+1,n+1,n+1),w(n+1,n+1,n+1);
  DoubleTab uu(n+1,n+1,n+1),vv(n+1,n+1,n+1),ww(n+1,n+1,n+1);
  DoubleVect nE(n);
  DoubleVect fx(n),fy(n),fz(n);

  F77NAME(CALCUL0SPECTRE0AVEC030VIT)(vit_u.addr(),vit_v.addr(),vit_w.addr(),E.addr(),&n,fft_u.addr(),fft_v.addr(),fft_w.addr(),u.addr(),v.addr(),w.addr(),uu.addr(),vv.addr(),ww.addr(),nE.addr(),EX.addr(),EY.addr(),EZ.addr(),fx.addr(),fy.addr(),fz.addr());
  ecrit_spectre(E,n,temps,3);
  calc_Ec(E,n/2,Ec);
  int i;
  DoubleVect D(n);
  for (i=0; i<n/2-1; i++)
    D(i) = (i+1)*(i+1)*E(i);
  calc_Ec(D,n/2,Df);


}
#endif

void calc_sp_operateur(DoubleTab& vit_u, DoubleTab& vit_v, DoubleTab& vit_w, int n, double temps,int nb_op, double dt, DoubleVect& E)
{
  //   Permet de calculer le spectre d energie a partir d un champ de vitesse
  //     exprime au sommet, sans correction pour la periodicite
  /*   cerr << "COUCOU dans calc_sp_nouveau_3_vit" << flush; */
  /*   cerr << "N = " << n << flush; */
  //  DoubleVect E(3*n*n);
  DoubleTab fft_u(n,n,n+2),fft_v(n,n,n+2),fft_w(n,n,n+2);
  DoubleVect EX(n),EY(n),EZ(n);
  DoubleTab u(n+1,n+1,n+1),v(n+1,n+1,n+1),w(n+1,n+1,n+1);
  DoubleTab uu(n+1,n+1,n+1),vv(n+1,n+1,n+1),ww(n+1,n+1,n+1);
  DoubleVect nE(n);
  DoubleVect fx(n),fy(n),fz(n);
  IntTab  wl(6*n+14),wm(4*n+14),wn(4*n+14),iwork(2*n);

  F77NAME(CALCUL0SPECTRE0AVEC030VIT)(vit_u.addr(),vit_v.addr(),vit_w.addr(),E.addr(),&n,fft_u.addr(),fft_v.addr(),fft_w.addr(),u.addr(),v.addr(),w.addr(),uu.addr(),vv.addr(),ww.addr(),nE.addr(),EX.addr(),EY.addr(),EZ.addr(),fx.addr(),fy.addr(),fz.addr(),wl.addr(),wm.addr(),wn.addr(),iwork.addr());
  //  ecrit_spectre_operateur(E,n,temps,nb_op,3,dt);

  return;
}

void calc_Ec(DoubleVect& E,int kc,double& Integ)
{
  int i;
  Integ = 0.;
  for (i=0; i<kc-1; i++)
    Integ += (E(i+1)+E(i))/2.0;
  return;
}

