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
// File:        Traitement_particulier_NS_THI_VEF_new.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_THI_VEF_new.h>
#include <Navier_Stokes_std.h>
#include <Champ_P1NC.h>
#include <Domaine.h>
#include <Schema_Temps_base.h>
#include <Schema_Temps.h>
#include <SFichier.h>

extern "C" void cftfax(int ,int* ,double* );
extern "C" void rfftmlt(double*, double*, double*, int*,  int, int, int, int, int);

Implemente_instanciable(Traitement_particulier_NS_THI_VEF_new,"Traitement_particulier_NS_THI_new_VEF",Traitement_particulier_NS_THI_new);


// Description:
//
// Precondition:
// Parametre: Sortie& is
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Traitement_particulier_NS_THI_VEF_new::printOn(Sortie& is) const
{
  return is;
}


// Description:
//
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Traitement_particulier_NS_THI_VEF_new::readOn(Entree& is)
{
  return is;
}

void Traitement_particulier_NS_THI_VEF_new::init_calc_spectre(void)
{
  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  calcul_nb_som_dir(zone);
  int nsize, nsizes2, nl;

  nl = nb_som_dir; // anciennement nb_som_dir
  nsize = 3*nl;
  nsizes2=nsize/2;

  if( nsize%2 != 0 )
    {
      Cerr << " Trait_part_NS_THI_VEF_new : nsize n'est pas un nombre paire !! " <<  nsize << finl;
      exit();
    }

  DoubleTab vit_u(nl,nl,nsize);
  DoubleTab vit_v(nl,nl,nsize);
  DoubleTab vit_w(nl,nl,nsize);
  DoubleTab vit_u_s(nl,nl,nl);
  DoubleTab vit_v_s(nl,nl,nl);
  DoubleTab vit_w_s(nl,nl,nl);

  vit_u = 0.;
  vit_v = 0.;
  vit_w = 0.;
  vit_u_s= 0.;
  vit_v_s = 0.;
  vit_w_s = 0.;

  tab_calc_fft.resize(nl,nl,nsize+1);
  tab_calc_fft_1.resize(nl,nl,nsize+1);
  tab_calc_fft_2.resize(nl,nl,nsize+1);
  tab_calc_fft_s.resize(nl+1,nl+1,nl+1);

  DoubleVect Ek(nsizes2+1);
  DoubleVect Dk(nsizes2+1);
  //double Ec1,Ec2,Ec3,D1,D2,D3;
  double Ec1,D1;

  determine_new_tab_fft_VEF();

  // spectre 3D, a l'instant initial
  ch_vit_pour_fft_VEF_s(vit_u_s,vit_v_s,vit_w_s);

  // spectre avec des points a x=cst, a l'instant initial
  ch_vit_pour_fft_VEF(vit_u,vit_v,vit_w);
  calculer_spectre_new(vit_u, vit_v, vit_w, nsize, nl, 1000., Ek, Ec1, Dk, D1);
  Cerr << " Ec_x = " << Ec1 << finl;


  //// spectre avec des points a y=cst, a l'instant initial
  //  ch_vit_pour_fft_VEF_1(vit_u,vit_v,vit_w);
  //  calculer_spectre_new(vit_u, vit_v, vit_w, nsize, nl, 2000., Ek, Ec2, Dk, D2);
  //  Cerr << " Ec_y = " << Ec2 << finl;


  //// spectre avec des points a z=cst, a l'instant initial
  //  ch_vit_pour_fft_VEF_2(vit_u,vit_v,vit_w);
  //  calculer_spectre_new(vit_u, vit_v, vit_w, nsize, nl, 3000., Ek, Ec3, Dk, D3);
  //  Cerr << " Ec_z = " << Ec3 << finl;


  SFichier fic2 ("Sorties_THI_2",ios::app);
  double temps_crt = mon_equation->inconnue().temps();
  //  fic2 << temps_crt << (Ec1+Ec2+Ec3)/3.  << (D1+D2+D3)/3.  << finl;
  //  Cerr << " Ec_tot = " << (Ec1+Ec2+Ec3)/3. << finl;
  fic2 << temps_crt << Ec1  << D1  << finl;
  Cerr << " Ec_tot = " << Ec1 << finl;

}


void Traitement_particulier_NS_THI_VEF_new::calcul_spectre(void)
{
  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  calcul_nb_som_dir(zone);
  int nsize, nsizes2, nl;

  nl = nb_som_dir; // anciennement nb_som_dir
  nsize = 3*nl;
  nsizes2=nsize/2;

  DoubleTab vit_u(nl,nl,nsize);
  DoubleTab vit_v(nl,nl,nsize);
  DoubleTab vit_w(nl,nl,nsize);
  DoubleTab vit_u_s(nl,nl,nl);
  DoubleTab vit_v_s(nl,nl,nl);
  DoubleTab vit_w_s(nl,nl,nl);

  vit_u = 0.;
  vit_v = 0.;
  vit_w = 0.;
  vit_u_s= 0.;
  vit_v_s = 0.;
  vit_w_s = 0.;


  DoubleVect Ek(nsizes2+1),Ek1(nsizes2+1),Ek2(nsizes2+1);
  DoubleVect Dk(nsizes2+1),Dk1(nsizes2+1),Dk2(nsizes2+1);
  double Ec,Ec1,Ec2,Df,Df1,Df2;
  Ec=0.;
  Ec1=0.;
  Ec2=0.;
  Df=0.;
  Df1=0.;
  Df2=0.;

  double temps_crt = mon_equation->inconnue().temps();
  double dt_post_inst = 0.5;


  static double temps_dern_post_inst = -100.;
  if (dabs(temps_crt-temps_dern_post_inst)>=dt_post_inst)
    {


      // ******** sauvegarde champ pour calcul de spectre 3D ***********

      ch_vit_pour_fft_VEF_s(vit_u_s,vit_v_s,vit_w_s);


      // ******** Hypothese turbulence homogene isotrope ***********
      // ****(on ne calcule le spectre que suivant 1 direction) ****


      // ****(De plus : Ec_x ne doit prendre en compte que u et non les 3 composantes de vitesses) ****

      ch_vit_pour_fft_VEF(vit_u,vit_v,vit_w);
      calculer_spectre_new(vit_u, vit_v, vit_w, nsize, nl, temps_crt, Ek, Ec, Dk, Df);

      double eps=100.;

      ch_vit_pour_fft_VEF_1(vit_u,vit_v,vit_w);
      calculer_spectre_new(vit_u, vit_v, vit_w, nsize, nl, temps_crt+eps, Ek1, Ec1, Dk1, Df1);

      ch_vit_pour_fft_VEF_2(vit_u,vit_v,vit_w);
      calculer_spectre_new(vit_u, vit_v, vit_w, nsize, nl, temps_crt+eps+eps, Ek2, Ec2, Dk2, Df2);


      temps_dern_post_inst =temps_crt ;
    }



  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const int nb_faces = zone_VEF.nb_faces();
  double Ectot=0.0;
  double Ec_ub=0.0;
  double Ec_up=0.0;
  double Ec_ubp=0.0;
  DoubleTab ub(vitesse);
  const Champ_P1NC& chp=ref_cast(Champ_P1NC, mon_equation->inconnue().valeur());
  chp.filtrer_L2(ub);


  int cpt=0;
  int num_face;
  for (num_face=0; num_face <nb_faces; num_face++)
    {
      Ectot+=vitesse(num_face,0)*vitesse(num_face,0);
      Ectot+=vitesse(num_face,1)*vitesse(num_face,1);
      Ectot+=vitesse(num_face,2)*vitesse(num_face,2);
      Ec_ub+=ub(num_face,0)*ub(num_face,0);
      Ec_ub+=ub(num_face,1)*ub(num_face,1);
      Ec_ub+=ub(num_face,2)*ub(num_face,2);
      Ec_up+=(vitesse(num_face,0)-ub(num_face,0))*(vitesse(num_face,0)-ub(num_face,0));
      Ec_up+=(vitesse(num_face,1)-ub(num_face,1))*(vitesse(num_face,1)-ub(num_face,1));
      Ec_up+=(vitesse(num_face,2)-ub(num_face,2))*(vitesse(num_face,2)-ub(num_face,2));
      Ec_ubp+=(vitesse(num_face,0)-ub(num_face,0))*ub(num_face,0);
      Ec_ubp+=(vitesse(num_face,1)-ub(num_face,1))*ub(num_face,1);
      Ec_ubp+=(vitesse(num_face,2)-ub(num_face,2))*ub(num_face,2);
      cpt++;
    }
  Ectot/=(2*nb_faces);
  Ec_ub/=(2*nb_faces);
  Ec_up/=(2*nb_faces);
  Ec_ubp/=(2*nb_faces);

  SFichier fic ("Sorties_THI",ios::app);
  fic << temps_crt << Ectot << Ec << Df << finl;
  Cerr << "temps=" << temps_crt << "  Ectot=" << Ectot << "  Ec=" << Ec << "  D=" << Df << finl;

  SFichier fic2("dt_evol",ios::app);
  double dt = mon_equation->schema_temps().pas_de_temps();
  fic2 << temps_crt << dt << finl;

  SFichier fic3 ("Sorties_THI_3",ios::app);
  fic3 << temps_crt << Ectot << Ec_ub << Ec_up << Ec_ubp << finl;

}


void Traitement_particulier_NS_THI_VEF_new::renorm_Ec(void)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const int nb_faces = zone_VEF.nb_faces();
  double Ectot=0.0;


  // petite verification qui coute pas cher !!!

  int cpt=0;
  int num_face;
  for (num_face=0; num_face <nb_faces; num_face++)
    {
      Ectot+=vitesse(num_face,0)*vitesse(num_face,0);
      Ectot+=vitesse(num_face,1)*vitesse(num_face,1);
      Ectot+=vitesse(num_face,2)*vitesse(num_face,2);
      cpt++;
    }
  Ectot/=(2*nb_faces);
  Cerr << "cpt=" << cpt << finl;
  Cerr << "Ec par traitement direct du champ =" << Ectot << finl;

  /*
  //  if (fac_init!=0)
  {
  Ectot=0.0;

  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  int nb_som = zone.nb_som();
  double nb=pow(nb_som*1.,1./3.);
  int nsize, nsizes2, nl;

  nl = (int)(nb); // anciennement nb_som_dir
  nsize = 3*nl;
  nsizes2=nsize/2;

  DoubleTab vit_u(nl,nl,nsize);
  DoubleTab vit_v(nl,nl,nsize);
  DoubleTab vit_w(nl,nl,nsize);
  vit_u = 0.; vit_v = 0.; vit_w = 0.;

  DoubleVect Ek(nsizes2+1);
  DoubleVect Dk(nsizes2+1);
  double Ec,Ec1,Ec2,Df,Df1,Df2;

  double temps_crt = mon_equation->inconnue().temps();

  ch_vit_pour_fft_VEF(vit_u,vit_v,vit_w);
  calculer_spectre_new(vit_u, vit_v, vit_w, nsize, nl, 1000.+temps_crt, Ek, Ec, Dk, Df);

  // ****(Ec unidirectionnel et suivant UNE composante de vitesse) ****

  //  ch_vit_pour_fft_VEF_1(vit_u,vit_v,vit_w);
  //  calculer_spectre_new(vit_u, vit_v, vit_w, nsize, nl, 2000.+temps_crt, Ek, Ec1, Dk, Df1);

  //  ch_vit_pour_fft_VEF_2(vit_u,vit_v,vit_w);
  //  calculer_spectre_new(vit_u, vit_v, vit_w, nsize, nl, 3000.+temps_crt, Ek, Ec2, Dk, Df2);

  //  Ectot=(Ec+Ec1+Ec2)/3.;
  Ectot=Ec;

  }
  */
  double nEc=pow(Ectot/Ec_init,0.5);

  Cerr << " Ec_calcule : " << Ectot << " Ec_init : " << Ec_init << finl;
  Cerr << "Renormalisation!!" << finl;
  vitesse/=nEc;
  return;
}

void Traitement_particulier_NS_THI_VEF_new::ch_vit_pour_fft_VEF(DoubleTab& vit_u, DoubleTab& vit_v, DoubleTab& vit_w) const
{
  int nsize, nl;

  nl = nb_som_dir; // anciennement nb_som_dir
  nsize = 3*nl;
  //  nsizes2=nsize/2;

  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  int num_face;
  int i,j,k;

  DoubleTab ubar(vitesse);
  const Champ_P1NC& chp=ref_cast(Champ_P1NC, mon_equation->inconnue().valeur());
  chp.filtrer_L2(ubar);

  for (i=0; i<nl; i++)
    for (j=0; j<nl; j++)
      for (k=0; k<nsize; k++)
        {
          num_face = tab_calc_fft(i,j,k);
          vit_u(i,j,k)=ubar(num_face,0);
          vit_v(i,j,k)=ubar(num_face,1);
          vit_w(i,j,k)=ubar(num_face,2);
        }
}

void Traitement_particulier_NS_THI_VEF_new::ch_vit_pour_fft_VEF_1(DoubleTab& vit_u, DoubleTab& vit_v, DoubleTab& vit_w) const
{
  int nsize,  nl;

  nl = nb_som_dir; // anciennement nb_som_dir
  nsize = 3*nl;
  //  nsizes2=nsize/2;

  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  int num_face;
  int i,j,k;

  DoubleTab ubar(vitesse);
  const Champ_P1NC& chp=ref_cast(Champ_P1NC, mon_equation->inconnue().valeur());
  chp.filtrer_L2(ubar);

  for (i=0; i<nl; i++)
    for (j=0; j<nl; j++)
      for (k=0; k<nsize; k++)
        {
          num_face = tab_calc_fft(i,j,k);
          vit_u(i,j,k)=vitesse(num_face,0)-ubar(num_face,0);
          vit_v(i,j,k)=vitesse(num_face,1)-ubar(num_face,1);
          vit_w(i,j,k)=vitesse(num_face,2)-ubar(num_face,2);
        }
}

void Traitement_particulier_NS_THI_VEF_new::ch_vit_pour_fft_VEF_2(DoubleTab& vit_u, DoubleTab& vit_v, DoubleTab& vit_w) const
{
  int nsize,  nl;

  nl = nb_som_dir; // anciennement nb_som_dir
  nsize = 3*nl;
  //  nsizes2=nsize/2;

  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  int num_face;
  int i,j,k;

  for (i=0; i<nl; i++)
    for (j=0; j<nl; j++)
      for (k=0; k<nsize; k++)
        {
          num_face = tab_calc_fft(i,j,k);
          vit_u(i,j,k)=vitesse(num_face,0);
          vit_v(i,j,k)=vitesse(num_face,1);
          vit_w(i,j,k)=vitesse(num_face,2);
        }
}


void Traitement_particulier_NS_THI_VEF_new::ch_vit_pour_fft_VEF_s(DoubleTab& vit_u_s, DoubleTab& vit_v_s, DoubleTab& vit_w_s) const
{
  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  int nb_som = zone.nb_som();
  int nl;

  nl = nb_som_dir; // anciennement nb_som_dir

  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  double temps_crt = mon_equation->inconnue().temps();
  int num_som;
  int i,j,k;

  //int nb_faces=vitesse.dimension(0);
  Champ_P1NC& chp=ref_cast_non_const(Champ_P1NC, mon_equation->inconnue().valeur());
  DoubleTab vit_fac(chp.valeurs());
  int nb_comp=1;
  if(vitesse.nb_dim()!=1)
    nb_comp=vitesse.dimension(1);
  DoubleTab vit_som(nb_som, nb_comp);
  vit_som=valeur_P1_L2(chp, chp.domaine());

  for (i=0; i<nl; i++)
    for (j=0; j<nl; j++)
      for (k=0; k<nl; k++)
        {
          num_som = tab_calc_fft_s(i,j,k);
          vit_u_s(i,j,k)=vit_som(num_som,0);
          vit_v_s(i,j,k)=vit_som(num_som,1);
          vit_w_s(i,j,k)=vit_som(num_som,2);
        }

  ////////////////
  // Rajoute pour ecriture du champ des vitesses dans un fichier
  // pour nouveau calcul de spectre (postraitement separe).
  // Ecriture du champ pour un spectre calcule sur la grille a z=cte.
  ////////////////

  Nom fichier_vit= "chp_vit_VEF_";
  Nom tps = Nom(temps_crt);
  fichier_vit += tps;
  SFichier fic79 (fichier_vit);
  fic79 << nl <<finl;
  fic79 << nl <<finl;
  fic79 << nl <<finl;

  for (i=0; i<nl; i++)
    for (j=0; j<nl; j++)
      for (k=0; k<nl; k++)
        fic79 << vit_u_s(i,j,k) <<finl;

  for (i=0; i<nl; i++)
    for (j=0; j<nl; j++)
      for (k=0; k<nl; k++)
        fic79 << vit_v_s(i,j,k) <<finl;

  for (i=0; i<nl; i++)
    for (j=0; j<nl; j++)
      for (k=0; k<nl; k++)
        fic79 << vit_w_s(i,j,k) <<finl;


  Cerr << " OK " << finl;
}

void Traitement_particulier_NS_THI_VEF_new::determine_new_tab_fft_VEF()
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdisbase);
  const DoubleTab& xv = zone_VEF.xv();
  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  const Domaine& dom = zone.domaine();
  const DoubleTab& coord = dom.coord_sommets();
  int nb_som = zone.nb_som();
  calcul_nb_som_dir(zone);
  int nsize,  nl, ntot;

  nl = nb_som_dir; // anciennement nb_som_dir
  nsize = 3*nl;
  //  nsizes2=nsize/2;
  ntot= nl*nl*(nsize+1);

  int nb_faces = zone_VEF.nb_faces();
  int num_face,num_som;
  int ix,iy,iz,ii,jj,kk;
  //int i,j,k;

  double longueur,d,d3,eps;
  double xx,yy,zz;

  int compteurx=0;
  int compteury=0;
  int compteurz=0;

  // ***************

  longueur = 6.283185307;

  // ***************

  d=longueur/(nl);
  eps=d/10.;
  d3=longueur/(nsize);
  //eps3=d3/10.;

  for(num_face=0; num_face<nb_faces; num_face++)
    {
      xx=xv(num_face,0);
      yy=xv(num_face,1);
      zz=xv(num_face,2);

      ix=(int)((xx+eps)/d3);
      iy=(int)((yy+eps)/d3);
      iz=(int)((zz+eps)/d3);

      if( iz%3==1 && iy%3==1)
        {
          compteurx+=1;
          ii=ix;
          jj=(int)(iy/3);
          kk=(int)(iz/3);

          tab_calc_fft(jj,kk,ii)=num_face;
        }

      if( iz%3==1 && ix%3==2) // le nombre 2 dans le second test n'est pas une erreur :)
        {
          compteury+=1;
          ii=(int)(ix/3);
          jj=iy;
          kk=(int)(iz/3);

          tab_calc_fft_1(ii,kk,jj)=num_face;
        }

      if( ix%3==1 && iy%3==1)
        {
          compteurz+=1;
          ii=(int)(ix/3);
          jj=(int)(iy/3);
          kk=iz;

          tab_calc_fft_2(ii,jj,kk)=num_face;
        }
    }

  for(num_som=0; num_som<nb_som; num_som++)
    {
      xx=coord(num_som,0);
      yy=coord(num_som,1);
      zz=coord(num_som,2);

      ii=(int)((xx+eps)/d);
      jj=(int)((yy+eps)/d);
      kk=(int)((zz+eps)/d);

      tab_calc_fft_s(ii,jj,kk)=num_som;
    }


  // Verification geometrie

  if( compteurx!=ntot || compteury!=ntot || compteurz!=ntot )
    {
      Cerr << " ATTENTION (Trait_part_NS_THI_VEF_new) : nombre de points trouves differe de  " << ntot << finl;
      Cerr << " assurez-vous que le nombre de points dans le fichier .data" << finl;
      Cerr << "est bien le meme suivant les 3 directions d'espace et que la longueur du pave etudie correspond bien a : " << longueur << finl;
      exit();
    }

  //            Nom fic_vit_ = "chp_vit";
  //          SFichier fic79 (fic_vit_);

  //          fic79 << 3*(nbpts-1) << finl;
  //          fic79 << (nbpts-1)*(nbpts-1) << finl;

  //          for (i=0;i<nbpts-1;i++)
  //          for (j=0;j<nbpts-1;j++)
  //          for (k=0;k<3*(nbpts-1);k++)
  //        fic79 << vit(tab_ind_num(i,j,k),0) << vit(tab_ind_num(i,j,k),1) <<vit(tab_ind_num(i,j,k),2) <<finl;

}


void Traitement_particulier_NS_THI_VEF_new::calculer_spectre_new(DoubleTab& vit_u, DoubleTab& vit_v, DoubleTab& vit_w,
                                                                 int nsize, int nl, double temps,
                                                                 DoubleVect& Ek, double& Ec, DoubleVect& Dk, double& Df)
{

  DoubleVect trig(2*(nsize));
  IntVect ifax(19);
  int lot=1;
  DoubleVect x(nsize+2),y(nsize+2),z(nsize+2);
  DoubleVect  work(2*nsize*lot);
  int inc=1;
  int jump=inc*(nsize+2);
  int isign=-1;
  int i,j,k;
  int nsizes2=nsize/2;
  Ek=0.;
  Ec=0.;
  Dk=0.;
  Df=0;

  // Initialisation des FFT

  cftfax(nsize,ifax.addr(),trig.addr());

  // Calcul des FFT pour chacune des lignes stockees


  for (i=0; i<nl; i++)
    {
      for (j=0; j<nl; j++)
        {
          for (k=0; k<nsize; k++)
            {
              x(k)=vit_u(i,j,k);
              y(k)=vit_v(i,j,k);
              z(k)=vit_w(i,j,k);
            }
          x(nsize)=x(0);
          y(nsize)=y(0);
          z(nsize)=z(0);
          x(nsize+1)=x(1);
          y(nsize+1)=y(1);
          z(nsize+1)=z(1);


          rfftmlt(x.addr(),work.addr(),trig.addr(),ifax.addr(),inc,jump,nsize,lot,isign);
          //                  rfftmlt(y.addr(),work.addr(),trig.addr(),ifax.addr(),inc,jump,nsize,lot,isign);
          //                  rfftmlt(z.addr(),work.addr(),trig.addr(),ifax.addr(),inc,jump,nsize,lot,isign);


          for (k=0; k<nsizes2+1; k++)
            {
              Ek(k)+=x(2*k)*x(2*k)+x(2*k+1)*x(2*k+1);
              //                          y(2*k)*y(2*k)+y(2*k+1)*y(2*k+1);
              //                         z(2*k)*z(2*k)+z(2*k+1)*z(2*k+1);
            }
        }
    }

  // Renormalisation de l'Energie et ecriture

  Ek/=(nl*nl);
  Ek(0)/=2.; // renormalisation specifique pour le mode 0

  Nom fichier = "spectre_";
  Nom tps = Nom(temps);
  Cerr << "tps=" << tps << finl;
  fichier += tps;
  SFichier fic7 (fichier);

  for (k=0; k<nsizes2; k++)
    {
      Dk(k)=k*k*Ek(k);
      fic7 << k << " " << Ek(k) << " " << Dk(k) << finl ;
      Ec+=Ek(k);
      Df+=Dk(k);
    }
}


