/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Solv_Gen.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Solv_Gen.h>
#include <Motcle.h>
#include <Matrice_Morse_Sym.h>
#include <Matrice_Bloc.h>
#include <ILU.h>

Implemente_instanciable_sans_constructeur(Solv_Gen,"Solv_Gen",solv_iteratif);

Solv_Gen::Solv_Gen()
{
  seuil_ = _SEUIL_Gen_;
  nb_it_max_ = 1000000;
}

void Solv_Gen::reinit()
{
  if(le_precond_.non_nul())
    {
      le_precond_.valeur().reinit();
    }
}

Sortie& Solv_Gen::printOn(Sortie& s ) const
{
  s<<" { seuil "<<seuil_<< " solv_elem "<<le_solveur_elem_ << " precond "<< le_precond_;
  if (limpr()==1)
    s<< " impr ";
  if (limpr()==-1)
    s<< " quiet ";
  s <<" } ";
  return s ;
}

Entree& Solv_Gen::readOn(Entree& is )
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcles les_parametres(7);
  {
    les_parametres[0] = "seuil";
    les_parametres[1] = "impr";
    les_parametres[2] = "solv_elem";
    les_parametres[3] = "precond";
    les_parametres[4] = "save_matrice|save_matrix";
    les_parametres[5] = "quiet";
    les_parametres[6] = "nb_it_max";
  }
  int rang;

  Motcle motlu;
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error when reading the parameters for the generic solver " << finl;
      Cerr << "One expected : " << accolade_ouverte << finl;
      exit();
    }
  is >> motlu;
  save_matrice_=0;
  while (motlu != accolade_fermee)
    {
      rang = les_parametres.search(motlu);
      switch(rang)
        {
        case 0:
          {
            is >> seuil_;
            break;
          }
        case 1:
          {
            fixer_limpr(1);
            break;
          }
        case 2:
          {
            is >> le_solveur_elem_;
            break;
          }
        case 3:
          {
            is >> le_precond_;
            break;
          }
        case 4:
          {
            save_matrice_=1;
            break;
          }
        case 5:
          {
            fixer_limpr(-1);
            break;
          }
        case 6:
          {
            is>>nb_it_max_;
            break;
          }
        default :
          {
            Cerr << "Error when reading the parameters of the generic solver " << finl;
            Cerr << "One expected " << les_parametres << " instead of " << motlu << finl;
            exit();
          }
        }
      is >> motlu;
    }
  return is;
}




int Solv_Gen::resoudre_systeme(const Matrice_Base& la_matrice,
                               const DoubleVect& secmem,
                               DoubleVect& solution)
{

  // On definit tout nos vecteurs et leur dimension respective et nos constantes

  // n : la taille du systeme lineaire
  int n = secmem.size();
  if(solution.size() != n)
    {
      if( solution.size() ==0)
        solution.resize(n);
      else
        {
          Cerr << "Solv_Gen::resoudre_systeme : wrong dimension of the vectors" << finl;
          return(-1);
        }
    }
  //Cerr<<"La matrice : "<<la_matrice.que_suis_je()<<finl;
  if(sub_type(Matrice_Bloc,la_matrice))
    {
      const Matrice_Bloc& MB = ref_cast(Matrice_Bloc,la_matrice);

      const Matrice& MB00 = MB.get_bloc(0,0);
      if(sub_type(Matrice_Morse_Sym,MB00.valeur()))
        {
          const Matrice_Morse_Sym& M = ref_cast(Matrice_Morse_Sym,MB00.valeur());
          return solve(la_matrice,M,secmem,solution);
        }
      else if(sub_type(Matrice_Morse,MB00.valeur()))
        {
          const Matrice_Morse& mat = ref_cast(Matrice_Morse,MB00.valeur());
          return solve(la_matrice,mat,secmem,solution);
        }
      else
        {
          Cerr<<"ERROR SolvGen : one is not able to treat the case of a matrix"<<finl;
          Cerr<<"of type : "<<MB00.que_suis_je()<<finl;
          exit();
          return -1;
        }
    }
  else if(sub_type(Matrice_Morse_Sym,la_matrice))
    {
      const Matrice_Morse_Sym& M = ref_cast(Matrice_Morse_Sym,la_matrice);
      return solve(M,M,secmem,solution);
    }
  else if(sub_type(Matrice_Morse,la_matrice))
    {
      const Matrice_Morse& mat = ref_cast(Matrice_Morse,la_matrice);
      return solve(mat,mat,secmem,solution);
    }
  else
    {
      Cerr<<"ERROR SolvGen : one is not able to treat the case of a matrix"<<finl;
      Cerr<<"of type : "<<la_matrice.que_suis_je()<<finl;
      exit();
      return -1;
    }
}

int Solv_Gen::solve(const Matrice_Base& matrice,
                    const Matrice_Base& mat_loc,
                    const DoubleVect& secmem,
                    DoubleVect& solution)
{
  // n : la taille du systeme lineaire
  int n = secmem.size();
  int ntot = secmem.size_array();
  int niter=0;
  // definition de ipar et fpar
  ArrOfInt ipar(16);
  ArrOfDouble fpar(16);


  // definition de w : vecteur de travail de taille 9*n
  ArrOfDouble w;
  le_solveur_elem_.dimensionne_wks(ntot,w);

  // Initialisation
  ipar[0] = 0; //on def tjrs cela pour commencer le calcul
  if(le_precond_.non_nul())
    {
      if(sub_type(ILU,le_precond_.valeur()))
        {
          ILU& pilu = ref_cast(ILU,le_precond_.valeur());
          ipar[1] = pilu.type_precond();
        }
      else
        ipar[1]=1;
    }
  else
    ipar[1]=0;
  ipar[2] = 1; // pour le test de convergence
  ipar[2] = 2;
  ipar[3] = w.size_array();; //taille maximale de w
  ipar[4] = 10;
  int nmax = max(Process::mp_sum(n), 100);
  ipar[5] = nmax; // nb max de produit matrice vect
  // si nb negatif on s arrete a la convergence
  // Si gros calcul (Process::mp_sum(n)>2147483647), specifier nb_it_max et imposer ipar[5] = -1

  fpar[0] = seuil_; // tolerance relative
  fpar[0]= 1e-50 ; // GF les autres solveurs n'ont pas de tol relative
  fpar[1] = seuil_; // tolerance absolue
  fpar[10] = 0; // initialisation du compteur

  //solution = 1.0;
  le_solveur_elem_.iteration(ntot,secmem,solution,ipar,fpar,w);
  solution.echange_espace_virtuel();

  DoubleTab W7, W8;

  {
    // fait pointer v sur la sous partie qui commence a ipar[7]-1, de taille ntot:
    W7.ref_array(w, ipar[7]-1, ntot);
    // Remarque TUTU: resize autorise car il ne change pas la taille et necessaire
    // pour initialiser line_size() du tableau W7:
    W7.resize(ntot/secmem.line_size(), secmem.line_size());
    W7.set_md_vector(secmem.get_md_vector());
    W7.echange_espace_virtuel();
  }

  int ret = ipar[0];

  while(ret != 0 && niter < nb_it_max_ )
    {
      niter++;
      W7.ref_array(w, ipar[7]-1, ntot);
      W7.resize(ntot/secmem.line_size(), secmem.line_size()); //idem remarque TUTU
      W7.set_md_vector(secmem.get_md_vector());
      W8.ref_array(w, ipar[8]-1, ntot);
      W8.resize(ntot/secmem.line_size(), secmem.line_size()); //idem remarque TUTU
      W8.set_md_vector(secmem.get_md_vector());
      if (ret != 4 && ret != 6)
        W7.echange_espace_virtuel();

      if(ret==1)
        {
          // on doit calculer le produit A*u
          //          int ii;
          matrice.multvect(W7, W8);
          le_solveur_elem_.iteration(ntot,secmem,solution,ipar,fpar,w);
          solution.echange_espace_virtuel();
        }
      else if(ret==2)
        {
          exit();
          // on doit calculer le produit At*u
          //             matrice.multvectT(u, s);
          //int ii;
          matrice.multvectT(W7, W8);
          le_solveur_elem_.iteration(ntot,secmem,solution,ipar,fpar,w);
          solution.echange_espace_virtuel();
        }
      else if((ret == 3) || (ret == 5))
        {
          // On preconditionne
          //int ii;
          if(sub_type(ILU,le_precond_.valeur()))
            le_precond_.preconditionner(mat_loc, W7, W8);
          else
            le_precond_.preconditionner(mat_loc,secmem,solution);
          //             le_precond_.preconditionner(matrice,secmem,solution);
          le_solveur_elem_.iteration(ntot,secmem,solution,ipar,fpar,w);
          solution.echange_espace_virtuel();
        }
      else if((ret == 4) || (ret == 6))
        {
          exit();
          // On preconditionne
          if(sub_type(ILU,le_precond_.valeur()))
            le_precond_.preconditionner(mat_loc, W7, W8);
          else
            le_precond_.preconditionner(mat_loc,secmem,solution);

          le_solveur_elem_.iteration(ntot,secmem,solution,ipar,fpar,w);
          solution.echange_espace_virtuel();
        }
      else if(ret == 10)
        {
          Cout<<"Case 10"<<finl;
          Cout<<"The predefined stop test is retained"<<finl;
          le_solveur_elem_.iteration(ntot,secmem,solution,ipar,fpar,w);
          solution.echange_espace_virtuel();
        }
      else if(ret > 0)
        {
          Cerr<<"Case > 0"<<finl;
          Cerr<<"That code does not exist "<<finl;
          exit();
          return(-1);
        }
      else if(ret < 0)
        {
          Cerr<<"ERROR in SolvGen"<<finl;
          Cerr<<" The solver terminates with code number "<<ret<<finl;
          if(ret==-1)
            {
              Cerr<<"Stop since the iteration number "<<finl;
              Cerr<<"is greater than the preset limit"<<finl;
            }
          else if(ret==-2)
            {
              Cerr<<"return due to insufficient work space"<<finl;
            }
          else if(ret==-3)
            {
              Cerr<<"return due to anticipated break-down / divide by zero"<<finl;
              Cerr<<"ipar[11] : "<<ipar[11]<<finl;
              Cerr<<"w : "<<w<<finl;
              Cerr<<"ind = fpar[1O] : "<<fpar[10]<<finl;
            }
          else if(ret==-4)
            {
              Cerr<<"the values of fpar(1) and fpar(2) are both <= 0, "<<finl;
              Cerr<<"the valid ranges are 0 <= fpar(1) < 1, 0 <= fpar(2), "<<finl;
              Cerr<<"and they can not be zero at the same time"<<finl;
            }
          else if(ret==-9)
            {
              Cerr<<"while trying to detect a break-down, "<<finl;
              Cerr<<"an abnormal number is detected"<<finl;
            }
          else if(ret==-10)
            {
              Cerr<<"return due to some non-numerical reasons, "<<finl;
              Cerr<<"e.g. invalid floating-point numbers etc."<<finl;
            }
          exit();
          return(-1);
        }
      ret = ipar[0];
      if(limpr()==1)
        {
          // Cerr<<"++ Cas : "<<ret<<finl;
          //Cerr<<"fpar(5) -- current residual norm (if available) : "<<fpar[4]<<finl;
          //Cerr<<"fpar(6) -- current residual/error norm : "<<fpar[5]<<finl;
          Cout<< fpar[5]<<" ";
          if (niter%10==0) Cout<<finl;
        }
    }

  if (je_suis_maitre())
    {

      //      Cerr<<"--------------------------------------------------"<<finl;
      if (limpr()>-1)
        {
          if (niter == nb_it_max_) Cout<<finl<<"Maximum number of iteration nb_iter_max for SolvGen = "<<nb_it_max_ << " reached..." <<finl;
          Cout<<finl<<"End of the resolution by SolvGen after "<<niter <<" iterations, current residual/error norm "<<fpar[5]<<" initial residual/error norm "<<fpar[2]<<finl;
        }
      //Cerr<<"fpar(3) -- initial residual/error norm : "<<fpar[2]<<finl;
      //Cerr<<"fpar(4) -- target residual/error norm : "<<fpar[3]<<finl;
      //Cerr<<"fpar(5) -- current residual norm (if available) : "<<fpar[4]<<finl;
      //Cerr<<"fpar(6) -- current residual/error norm : "<<fpar[5]<<finl;
      //Cerr<<"fpar(7) -- convergence rate : "<<fpar[6]<<finl;
    }
  if (0)
    {
      DoubleVect tmp(solution);
      matrice.multvect(solution,tmp);
      int ii;
      /*
        Process::Journal()<<" A = "<<matrice<<finl;
        Process::Journal()<<" ipar = "<<finl;
        for(ii=0;ii<ipar.size_array();ii++)
        Process::Journal()<<ii<<": "<<ipar[ii]<<finl;
        Process::Journal()<<" w = "<<finl;
        for(ii=0;ii<w.size_array();ii++)
        Process::Journal()<<ii<<": "<<w[ii]<<finl;
        Process::Journal()<<" x = "<<solution<<finl;
        //     solution.ecrit(Process::Journal());
        Process::Journal()<<" A*x = "<<tmp<<finl;
        Process::Journal()<<" b = "<<secmem<<finl;
        tmp.ecrit(Process::Journal());
      */
      for(ii=0; ii<solution.size(); ii++)
        tmp(ii)-=secmem(ii);
      tmp.echange_espace_virtuel();
      double err=0.;
      for(ii=0; ii<solution.size(); ii++)
        err+=(tmp(ii)*tmp(ii));
      err = Process::mp_sum(err);
      Cerr<<"ERROR: "<<sqrt(err)<<finl;
    }
  return(1);
}


