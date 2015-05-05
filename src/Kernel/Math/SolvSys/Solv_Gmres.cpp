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
// File:        Solv_Gmres.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/38
//
//////////////////////////////////////////////////////////////////////////////

#include <Solv_Gmres.h>
#include <Matrice_Morse_Sym.h>
#include <Matrice_Bloc.h>
#include <Motcle.h>

#ifdef CRAY
#endif


Implemente_instanciable_sans_constructeur(Solv_Gmres,"Solv_Gmres",solv_iteratif);

Solv_Gmres::Solv_Gmres()
{
  seuil_ = 1.e-12;
  is_local_gmres=0;
  precond_diag=0;
  nb_it_max_ = 1000000;
  controle_residu_ =0;
  dim_espace_Krilov_=10;
}

// printOn et readOn
Sortie& Solv_Gmres::printOn(Sortie& s ) const
{
  s<<" { seuil "<<seuil_;
  if (precond_diag)
    s <<" diag ";
  else if (is_local_gmres) s<<" sans_precond ";

  if (controle_residu_) s<< " controle_residu "<<controle_residu_;
  if (nb_it_max_!=1000000) s<<" nb_it_max "<<nb_it_max_;
  if (limpr()) s<<" impr ";
  if (save_matrice_) s<< " save_matrice ";
  s<<" dim_espace_krilov "<<dim_espace_Krilov_;
  s<<" } ";
  return s;
}

Entree& Solv_Gmres::readOn(Entree& is )
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcles les_parametres(8);
  {
    les_parametres[0] = "impr";
    les_parametres[1] = "seuil";
    les_parametres[2] = "diag";
    les_parametres[3] = "sans_precond";
    les_parametres[4] = "nb_it_max";
    les_parametres[5] = "controle_residu";
    les_parametres[6] = "save_matrice";
    les_parametres[7] = "dim_espace_krilov";
  }
  int rang;

  Motcle motlu;
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error when reading the GMRES parameters " << finl;
      Cerr << "One expected " << accolade_ouverte
           << " instead of : " << motlu << finl;
      exit();
    }
  is >> motlu;
  while (motlu != accolade_fermee)
    {
      rang = les_parametres.search(motlu);
      switch(rang)
        {
        case 0:
          {
            fixer_limpr(1);
            break;
          }
        case 1:
          {
            is >> seuil_;
            break;
          }
        case 2:
          {
            is_local_gmres=1;
            precond_diag=1;
            break;
          }
        case 3:
          {
            is_local_gmres=1;
            precond_diag=0;
            break;
          }
        case 4:
          {
            is>>nb_it_max_;
            break;
          }
        case 5:
          {
            is>>controle_residu_;
            break;
          }
        case 6:
          {
            save_matrice_=1;
            break;
          }
        case 7:
          {
            is >>dim_espace_Krilov_;
            break;
          }

        default :
          {
            Cerr << "Error when reading the Gmres parameters " << finl;
            Cerr << "One expected a word among " << les_parametres
                 << " instead of " << motlu << finl;
            exit();
          }
        }
      is >> motlu;
    }
  return is;
}



int Solv_Gmres::resoudre_systeme(const Matrice_Base& la_matrice,
                                 const DoubleVect& secmem,
                                 DoubleVect& solution)
{
  if(sub_type(Matrice_Morse,la_matrice))
    {
      const Matrice_Morse& matrice = ref_cast(Matrice_Morse, la_matrice);
      int retour= Gmres(matrice,secmem,solution);
      DoubleVect b(secmem);
      matrice.multvect(solution, b);
      //       Cerr << " b :" << b<< finl;
      //       Cerr << " secmem :" << secmem<< finl;
      b-=secmem;
      // Cerr << "||residu|| = " << b.norme() << finl;
      return retour;
    }
  else
    {
      if(sub_type(Matrice_Bloc,la_matrice))
        {
          const Matrice_Bloc& matrice = ref_cast(Matrice_Bloc,la_matrice);
          if(matrice.nb_lignes()>1)
            {
              Cerr<<"Solv_Gmres : WARNING : one is not able to carry out Gmres by blocks"<<finl;
              exit();
              return(-1);
            }

          if (Process::nproc() > 1)
            {
              Cerr<<"Solv_Gmres : WARNING : one is not able to carry out parallel calculation with Gmres"<<finl;
              exit();
              return(-1);
            }

          const Matrice_Morse& MB00 = ref_cast(Matrice_Morse,matrice.get_bloc(0,0).valeur());
          int retour= Gmres(MB00,secmem,solution);
          return retour;
        }
      else
        {
          Cerr<<"Solv_Gmres : WARNING : only linear systems based on Matrice_Morse_Sym or Matrice_Bloc type matrixes can be solved"<<finl;
          exit();
          return(-1);
        }
    }
}



int gmres_local( const Matrice_Morse& A, const DoubleVect& b,DoubleVect& x1,double& seuil,int precond_diag,int& nb_it_max,int&
                 controle_residu,VECT(DoubleVect)& v,int dim_espace_Krilov, int limpr_=0)

// Copied from SouConPhase_field (scalar)//
{
  //return le_solveur_.resoudre_systeme(A,b,x1);
  //A.inverse(b,x1,seuil); return 1;

  int i,j,nk,i0,im,it,ii;
  double tem=1.,res,ccos,ssin ;
  // PL c'est pas joli et c'est de moi mais je ne comprends pas pourquoi
  // l'utilisation de b.size_reelle() plante sur la matrice en pression depuis la 1.6.0 (non teste)
  // Qu'est ce qui est fait en implicite pour b.size_reelle() soit >=0 ???
  const int ns=(b.size_reelle_ok()?b.size_reelle():b.size_array());

  int nb_ligne_tot=(int)Process::mp_sum((double) ns);
  // Ajoute par DJ
  //--------------
  //  int nb_elem_tot = b.size_totale();
  //DoubleVect x1(x);
  //x1=0.;
  //for(int n_elem=0; n_elem<nb_elem_tot; n_elem++)
  //  {
  //     x1(n_elem)=x(n_elem);
  // }
  //--------------

  // A present dans le jdd
  double epsGMRES=1.e-10*0;
  int nkr=max(10,nb_ligne_tot/2);                         // dimension de l'espace de Krylov
  //nkr=10;
  nkr=dim_espace_Krilov;
  int nit1=max(20,nb_ligne_tot);
  int nit=min(nb_it_max,nit1);
  double rec_min = seuil;
  double rec_max = 0.1  ;
  double res2_old=-1;
  if (v.size()==0)
    v.dimensionner(nkr);                         // Krilov vectors
  DoubleTab h(nkr+1,nkr);                // Heisenberg maatrix of coefficients
  DoubleVect r(nkr+1);
  DoubleVect v0(x1);
  DoubleVect v1(x1) ;
  int ns2=ns;
  DoubleVect Diag(ns,1.);
  if (precond_diag)
    for ( i=0; i<ns; i++)
      Diag[i]=1./A(i,i);

  // Initialisation
  v0 = 0. ;
  v1 = 0. ;

  A.multvect_(x1,v0);
  v0 *= -1. ;
  v0+=b;
  v0.echange_espace_virtuel();
  for (ii=0; ii<ns2; ii++)
    v0(ii)*=Diag(ii);

  res = mp_norme_vect(v0);
  double res0 = res;
  if (limpr_)
    Cout<<"Gmres : initial residual = "<<res0<<finl;
  // See http://stackoverflow.com/questions/3437085/check-nan-number
  // May be could be interesting to implement isnan function somewhere
  if (res0!=res0)
    {
      Cerr << "Nan detected in Solv_Gmres::gmres_local()" << finl;
      Cerr << "Contact TRUST support." << finl;
      Process::exit();
    }
  rec_min = (rec_min<res*epsGMRES) ? res*epsGMRES : rec_min;
  rec_min = (rec_min<rec_max) ? rec_min : rec_max ;

  // iterations
  for(it=0; it<nit; it++)
    {
      if (res==0) return 0; // nothing to do
      nk = nkr;

      //...Orthogonalisation of Arnoldi
      v0 /= res;
      r = 0. ;
      r[0] = res;
      h=0.;
      for(j=0; j<nkr; j++)
        {
          v0.echange_espace_virtuel();
          v[j]=v0;
          A.multvect(v0,v1);
          for (ii=0; ii<ns2; ii++) v1(ii)*=Diag(ii);
          v0 = v1 ;
          // Modifie par DJ
          //---------------
          for(i=0; i<=j; i++)
            {
              DoubleVect& vvi=v[i];
              h(i,j)+=mp_prodscal(v0,vvi);
              for (ii=0; ii<ns; ii++) v0(ii) -= h(i,j) * vvi(ii);
              v0.echange_espace_virtuel();
            }
          tem=mp_norme_vect(v0);

          h(j+1,j) = tem;
          if(tem<rec_min)
            {
              nk = j+1;
              goto l5;
            }
          v0 /= tem;
        }
      //...Triangularisation
l5:
      for(i=0; i<nk; i++)
        {
          im = i+1;
          tem = 1./sqrt(h(i,i)*h(i,i) + h(im,i)*h(im,i));
          ccos = h(i,i) * tem;
          ssin = - h(im,i) * tem;
          for(j=i; j<nk; j++)
            {
              tem = h(i,j);
              h(i,j) = ccos * tem - ssin * h(im,j);
              h(im,j) =  ssin * tem + ccos * h(im,j);
            }
          r[im] = ssin * r[i];
          r[i] *= ccos;
        }

      //...Solution of linear system
      for(i=nk-1; i>=0; i--)
        {
          r[i] /= h(i,i);
          for(i0=i-1; i0>=0; i0--)
            r[i0] -= h(i0,i)* r[i];
        }
      for(i=0; i<nk; i++)
        {
          DoubleVect& vvi=v[i];
          for(ii=0; ii<ns; ii++)  x1(ii) += r[i]*vvi(ii);
        }
      x1.echange_espace_virtuel();
      A.multvect_(x1,v0);
      v0 *= -1. ;
      v0+=b;

      // calcul du residu sans le precond....
      double res2=mp_norme_vect(v0);
      if ((it>0) && (controle_residu==1) && (sup_strict(res2,res2_old)))
        {
          Cout << "The Gmres iterative system is stopped after : " << it+1 <<" iterations "<<finl;
          Cout << "since an increase of the residue is detected."<< finl;
          return it;
        }

      res2_old = res2;
      if (limpr_)
        Cout<<" - At it = "<< it+1 <<", residu scalar = "<< res2 << finl;

      // Test d'arret sur le residu
      if(res2<rec_min)
        {
          // Ajoute par DJ
          //--------------
          if (limpr_)
            {
              Cout << "Gmres : Number of iterations to reach convergence : " << it+1 << finl;
              double residu_relatif = (res0>0?res2/res0:res2);
              Cout << "Final residue: " << res2 << " ( " << residu_relatif << " )";
            }
          return it+1;
        }
      // Test d'arret sur le nombre d'iterations max
      else if (it==nit-1)
        {
          // Ajoute par DJ
          //--------------
          // On fait le choix de mettre a jour c_demi et mutilde_demi meme s'il n'y a pas convergence...

          // x=x1;
          //--------------
          if (limpr_)
            {
              Cout << "Gmres : Stopped after "<< it+1 <<" iterations (=nb_it_max)"<< finl;
              double residu_relatif = (res0>0?res2/res0:res2);
              Cout << "Final residue: " << res2 << " ( " << residu_relatif << " )";
            }
          return it+1;
        }

      // Calcul du residu avec preconditionnement
      for (ii=0; ii<ns2; ii++) v0(ii)*=Diag(ii);
      res = mp_norme_vect(v0);
    }

  return -1;
}


int Solv_Gmres::Gmres(const Matrice_Morse& matrice,
                      const DoubleVect& secmem,
                      DoubleVect& solution)
{
  if (!is_local_gmres)
    return matrice.inverse(secmem, solution, seuil_);
  else
    return gmres_local(matrice,secmem,solution,seuil_,precond_diag,nb_it_max_,controle_residu_,v,dim_espace_Krilov_,limpr());
}


