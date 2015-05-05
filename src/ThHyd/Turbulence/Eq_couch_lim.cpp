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
// File:        Eq_couch_lim.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Eq_couch_lim.h>
#include <SFichier.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Eq_couch_lim,"Eq_couch_lim",Objet_U);

Eq_couch_lim::Eq_couch_lim()
{

}

Eq_couch_lim::~Eq_couch_lim()
{

}

Sortie& Eq_couch_lim::printOn(Sortie& os) const
{
  return os;
}

Entree& Eq_couch_lim::readOn( Entree& is)
{
  return is;
}

// Initialisation

void Eq_couch_lim::initialiser(int n1, int n2, double n8,
                               double epsilon, int max_ite, int dyn_nu_t)
{
  N_comp = n1 ;       // Nombre de composante a resoudre dans Eq_couch_lim
  N = n2 ;            // Nbre de points dans le maillage fin
  facteur = n8;       // facteur geometrique de raffinement du maillage fin
  eps = epsilon;      // Seuil de convergence de la resolution numerique dans aller_au_temps
  max_it = max_ite;   // Nbre d'iterations maximum dans aller_au_temps

  nu_t_dyn = dyn_nu_t; //determination dynamique de nu_t dans TBLE

  utau=0;
  utau_old=0;

  F.resize_array(N_comp*(N+1));
  Un_old.resize_array(N_comp*(N+1));
  Unp1.resize_array(N_comp*(N+1));
  aa.resize_array(N_comp*N);


  cis.resize_array(N_comp);
  visco_tot.resize_array(N);
  bb.resize_array(N);
  cc.resize_array(N);
  dd.resize_array(N);

  v.resize_array(N+1);
  v=0.;

  y_.resize_array(N+1);
  yc.resize_array(N+1);

  mailler_fin();

}

void Eq_couch_lim::mailler_fin()
{

  //********************************
  // Mise en place du maillage fin *
  //********************************

  SFichier fic_mesh("tble_mesh.dat",ios::app); // ouverture du fichier conv.dat
  double delta;
  y_(0)=y0;
  y_(N)=yn;
  yc(0)=y0;
  yc(N)=yn;

  if(facteur==1.)
    {
      delta=(yn-y0)/(N-1);
      for (int i=1 ; i<N ; i++)
        {
          yc(i)=yc(i-1)+delta;
        }
    }
  else
    {
      delta = yn*(1.-facteur)/(1.-pow(facteur,N-1));
      for (int i=1 ; i<N ; i++)
        {
          yc(i)=yc(i-1)+delta*pow(facteur,i-1);
        }
    }
  for (int i=1 ; i<N ; i++)
    {
      y_(i)=0.5*(yc(i)+yc(i-1));
    }

  for(int i=0 ; i<N+1 ; i++)
    {
      fic_mesh << i << " " << y_(i) << finl;
    }
  fic_mesh << finl;

  //************************************
  // Fin mise en place du maillage fin *
  //************************************
}
//************************************************
//*************** aller_au_temps ***********
//************************************************


void Eq_couch_lim::aller_au_temps(double t_final)
{
  it=0;
  int cpt=0;

  utau=0.;
  utau_old = 0;
  double criterion;
  Diffu_totale_base& le_a  = a.valeur();

  if(t_final==0)
    {
      for (int j = 0 ; j < N_comp ; j++)
        for(int i=0 ; i<N+1 ; i++)
          {
            tabdouble(F,j,i) = 0;
          }
    }

  visco_tot = le_a.calculer_a_local(0);

  do
    {
      cpt++;

      for(int i= 0; i<N ; i++)
        visco_tot(i) = le_a.calculer_a_local(i);

      //bb evaluation
      for(int i=1 ; i<N ; i++)
        {
          bb(i) = visco_tot(i)/(y_(i+1)-y_(i));
          dd(i)=1./(yc(i)-yc(i-1));
        }

      dd(1) = 1./(yc(1)-y0);

      bb(0) = visco_tot(0)/(2.*(y_(1)-y_(0)));

      //cc evaluation
      cc(N-1) = dd(N-1)*(bb(N-1)+bb(N-2))+(1./dt);

      for(int i=N-2 ; i>0 ; i--)
        {
          cc(i) = dd(i)*(bb(i)+bb(i-1))-((dd(i)*dd(i+1)*bb(i)*bb(i))/cc(i+1))+(1./dt);
        }

      //a evaluation
      for (int j = 0 ; j < N_comp ; j++)
        tabdouble(aa,j,N-1) = tabdouble(F,j,N-1) + dd(N-1)*bb(N-1)*tabdouble(Unp1,j,N) + (tabdouble(Un_old,j,N-1)/dt);

      for (int j = 0 ; j < N_comp ; j++)
        for(int i=N-2 ; i>0 ; i--)
          {
            tabdouble(aa,j,i) = tabdouble(F,j,i) + dd(i)*bb(i)*tabdouble(aa,j,i+1)/cc(i+1) + (tabdouble(Un_old,j,i)/dt);
          }

      //Velocity computation
      for (int j = 0 ; j < N_comp ; j++)
        {
          tabdouble(Unp1,j,1) = (tabdouble(aa,j,1)+2*bb(0)*dd(1)*tabdouble(Unp1,j,0))/(cc(1)+bb(0)*dd(1));
        }
      for (int j = 0 ; j < N_comp ; j++)
        {
          for (int i=2 ; i<N ; i++)
            {
              tabdouble(Unp1,j,i) = (dd(i)*bb(i-1)/cc(i))*tabdouble(Unp1,j,i-1)+(tabdouble(aa,j,i)/cc(i));
            }
        }//FIN RESOLUTION Un



      /*criterion=-1;
        DoubleVect Vect_carre(N_comp);
        DoubleVect Vect_carre_old(N_comp);
        Vect_carre_old=0;
        IntVect itmax(N_comp);

        for(int k=0 ; k < N_comp ; k++)
        {
        itmax=0;
        Vect_carre(k)=0;

        for(int i=0 ; i < N+1 ; i++)
        {
        Vect_carre(k)+=(tabdouble(Unp1,k,i)-tabdouble(Un_old,k,i))*(tabdouble(Unp1,k,i)-tabdouble(Un_old,k,i));
        Vect_carre_old(k)+=tabdouble(Un_old,k,i)*tabdouble(Un_old,k,i);
        }

        Vect_carre(k) = sqrt(Vect_carre(k));
        Vect_carre_old(k)=sqrt(Vect_carre_old(k));

        double criterion1=0.;
        if(Vect_carre(k)<1.e-20 || Vect_carre_old(k)<1.e-20 )
        criterion1=Vect_carre(k);
        else
        criterion1=(Vect_carre(k)/(Vect_carre_old(k)+DMINFLOAT));

        criterion=max(criterion,criterion1);

        itmax(k)+=it;
        } */

      double frottement=0.;

      for (int j = 0 ; j < N_comp ; j++)
        {

          cis(j) = le_a.calculer_a_local(0)*(tabdouble(Unp1,j,1)-tabdouble(Unp1,j,0))/((y_(1)-y0));
          frottement += cis(j)*cis(j);
        }

      //frottement : carre de la densite de flux diffusifs sur le bord
      //frottement pour TBLE hydr
      //densite de flux de chaleur parietal pour TBLE scal

      utau = sqrt(sqrt(frottement));

      criterion = (dabs(utau_old-utau))/(dabs(utau)+DMINFLOAT);

      utau_old = utau;

      it++;
      if(it>=max_it)
        Cerr << "Warning  : NON convergence in the TBLE resolution !!!" << finl;
    }
  while((criterion > eps)&&(it<max_it));

  // FIN ITERATION DE CONVERGENCE

  nu_t_yn = le_a.calculer_a_local(N-1)-le_a.calculer_a_local(0);

  utau_old = utau;

  Un_old = Unp1;// Sauvegarde du champ fin de l'instant N
}


void Eq_couch_lim::aller_jusqu_a_convergence(int itmax, double erreur)
{
  int ite=0;
  double critere=1e10;
  double utau_nm1=0.;
  while (ite<itmax && critere>erreur)
    {
      aller_au_temps(1.);
      critere = (dabs(utau_nm1-utau))/(dabs(utau)+DMINFLOAT);
      utau_nm1 = utau;
      ite++;
    }
  //             Cout << "NITE TBLE = " << ite << finl;
  //             Cout << "critere TBLE = " << critere << finl;
}
