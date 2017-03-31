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
// File:        Solv_Cholesky.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/31
//
//////////////////////////////////////////////////////////////////////////////

#include <Solv_Cholesky.h>
#include <Matrice_Bloc_Sym.h>
#include <Motcle.h>
#include <Sparskit.h>
#include <Lapack.h>
#include <EFichier.h>


//Solv_Cholesky::deja_factorise_=0;

Implemente_instanciable_sans_constructeur_ni_destructeur(Solv_Cholesky,"Solv_Cholesky",SolveurSys_base);

// printOn et readOn

Sortie& Solv_Cholesky::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Solv_Cholesky::readOn(Entree& is )
{
  if ( Process::nproc()>1 )
    {
      Cerr << finl;
      Cerr << "Cholesky solver not parallelized in TRUST!!!" << finl;
      Cerr << "Change your solver." << finl;
      Cerr << "Try using Petsc Cholesky solver." << finl;
      exit();
    }

  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcles les_parametres(3);
  {
    les_parametres[0] = "impr";
    les_parametres[1] = "save_matrice|save_matrix";
    les_parametres[2] = "quiet";
  }
  int rang;

  Motcle motlu;
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error when reading parameters for Cholesky " << finl;
      Cerr << "One expected : " << accolade_ouverte << finl;
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
            save_matrice_ = 1;
            break;
          }
        case 2:
          {
            fixer_limpr(-1);
            break;
          }
        default :
          {
            Cerr << "Error when reading parameters for Cholesky " << finl;
            Cerr << "On expected " << les_parametres << " instead of " << motlu << finl;
            exit();
          }
        }
      is >> motlu;
    }
  return is;
}

int Solv_Cholesky::resoudre_systeme(const Matrice_Base& la_matrice,
                                    const DoubleVect& secmem,
                                    DoubleVect& solution)
{
  if(sub_type(Matrice_Morse_Sym,la_matrice))
    {
      const Matrice_Morse_Sym& matrice = ref_cast(Matrice_Morse_Sym,la_matrice);
      return Cholesky(matrice,secmem,solution);
    }
  else
    {
      if(sub_type(Matrice_Bloc_Sym,la_matrice))
        {
          // Conversion d'une matrice au format Matrice_Bloc_Sym au format Matrice Morse
          if (nouvelle_matrice())
            ref_cast(Matrice_Bloc_Sym,la_matrice).BlocSymToMatMorseSym(matrice_de_travail);
          return Cholesky(matrice_de_travail,secmem,solution);
        }
      else if(sub_type(Matrice_Bloc,la_matrice))
        {
          const Matrice_Bloc& matrice = ref_cast(Matrice_Bloc,la_matrice);
          if((matrice.nb_bloc_lignes()>1 && ref_cast(Matrice_Morse,matrice.get_bloc(1,0).valeur()).nb_lignes()>0))
            {
              Cerr<<"Solv_Cholesky : WARNING : Cholesky by blocks cannot be done"<<finl;
              exit();
              return(-1);
            }
          const Matrice_Morse_Sym& MB00 = ref_cast(Matrice_Morse_Sym,matrice.get_bloc(0,0).valeur());
          return Cholesky(MB00,secmem,solution);
        }
      else
        {
          Cerr<<"Solv_Cholesky : Warning : one is not able to carry out preconditionning for linear systems based on Matrice_Morse_Sym or Matrice_Bloc type matrixes"<<finl;
          exit();
          return(-1);
        }
    }
}

int Solv_Cholesky::Cholesky(const Matrice_Morse_Sym& matrice,
                            const DoubleVect& secmem,
                            DoubleVect& solution)
{
  int avec_renumerotation=1;
  if (nouvelle_matrice())
    {
      fixer_nouvelle_matrice(0);
      // On rend definie la matrice dans le cas sequentiel ou Cholesky est le solveur en pression
      // En parallele, Cholesky est utilise en preconditionnement local et il ne faut
      // pas modifier la matrice locale.
      Cerr << "Order of the matrix : " << matrice.ordre() << finl;
      // Les champs P1bulle n'ont pas de size_reelle et en parallele ca ne marche pas...
      const int sz = secmem.size_totale();
      if(matrice.get_est_definie()==0 && nproc()==1)
        {
          Cerr << "Matrix not defined : " << finl;
          Cerr << "zero value is imposed at point 0" << finl;
          Matrice_Morse_Sym& mat = ref_cast_non_const(Matrice_Morse_Sym,matrice);
          mat(0,0)*=1E+6;
        }
      if (avec_renumerotation)
        {
          matrice.renumerote();
          Fact_Cholesky(matrice.matrice_renumerotee(),sz);
        }
      else
        Fact_Cholesky(matrice, sz);
    }


  //int ordre = la_matrice.ordre();
  DoubleVect vecteur(secmem);
  double* vect_data = vecteur.addr();
  //solution = 0.0;
  const int n = secmem.size_totale();//ordre;
  int N=n;

  // permutation du second membre
  // subroutine dvperm (n, x, perm)
  // SPARSKIT2/FORMATS/unary.f
  if (avec_renumerotation) F77NAME(DVPERM)(&n, vect_data, matrice.permutation().addr());


  char UPLO = 'U';                          // A est triangulaire superieure
  int KD   = largeur_de_bande_-1;           // largeur de bande sup
  //int N    = n;                             //m_ // ordre de A (=nb col de abd_)
  int LDAB = largeur_de_bande_;             //n_;// nb de lg de abd_
  int LDB  = n;                             //m_;// nb de lg de b
  int NRHS = 1;                             // nb de col de b (=vecteur)
  int INFO = 0;


  // resolution du systeme
#ifdef CRAY
  Nom UPLOCpp(UPLO);
  _fcd UPLOF90;
  nomCtonomF90(UPLOCpp,UPLOF90);
  F77NAME(SPBTRS)(UPLOF90, &N, &KD, &NRHS,
                  matrice_bande_factorisee_fortran_.addr() ,
                  &LDAB, vecteur.addr(), &LDB, &INFO);
#else
  F77NAME(DPBTRS)(&UPLO, &N, &KD, &NRHS, matrice_bande_factorisee_fortran_.addr() , &LDAB, vecteur.addr(), &LDB, &INFO);
#endif

  if(INFO)
    {
      Cerr << "Solv_Cholesky::resoudre_systeme error : ";
      if(INFO<0) Cerr << "F77NAME(DPBTRS) param. " << -INFO << " invalid" << finl;
      if(INFO>0) Cerr << "singular matrix ! resolution impossible" << finl;
      exit();
    };


  // permutation inverse du resultat
  // subroutine dvperm (n, x, perm)
  // SPARSKIT2/FORMATS/unary.f
  if (avec_renumerotation) F77NAME(DVPERM)(&n, vect_data, matrice.permutation_inverse().addr());

  solution = vecteur;
  solution.echange_espace_virtuel();
  //   test
  //   DoubleVect res;
  //   Cout<<"second membre "<<secmem.norme()<<finl;
  //   Cout<<"solution "<<solution.norme()<<finl;
  //   res = matrice*solution - secmem;
  //   Cout<<"residu "<<res.norme()<<finl;

  return 1;
}



int Solv_Cholesky::Fact_Cholesky(const Matrice_Morse_Sym& mat1, const int size_reelle)
{
  // int i;
  int j;
  int k;
  //int N = mat1.ordre();
  int N = size_reelle;

  largeur_de_bande_ = mat1.largeur_de_bande();
  // stockage de la matrice en format bande FORTRAN
  matrice_bande_factorisee_fortran_.resize_array(N*largeur_de_bande_);

  for (j=0; j<N; j++)
    {
      int  i;
      i = j+1 - largeur_de_bande_;
      k = 0;
      while (k < largeur_de_bande_)
        {
          if (i >= 0)
            {
              //Cout<<"i j "<<i<<" "<<j<<" "<<mat1(i,j)<<" "<<me()<<finl;
              const double tmpa = mat1(i,j);
              matrice_bande_factorisee_fortran_[j * largeur_de_bande_ + k] = tmpa;
            }

          i++;
          k++;
        }
    }

  // Factorisation de Cholesky

  // int DBG=1;
  // int PRC=0;
  char UPLO = 'U';
  int KD = largeur_de_bande_ - 1;// mu nbre de diagonales superieures
  int LDAB = largeur_de_bande_; //n_
  int INFO = 0;


  Cerr << "Starting factorisation ..." << finl;
#ifdef CRAY
  Nom UPLOCpp(UPLO);
  _fcd UPLOF90;
  nomCtonomF90(UPLOCpp,UPLOF90);
  SPBTRF(UPLOF90, &N, &KD,
         matrice_bande_factorisee_fortran_.addr() , &LDAB, &INFO);
#else
  F77NAME(DPBTRF)(&UPLO, &N, &KD,
                  matrice_bande_factorisee_fortran_.addr() , &LDAB, &INFO);
#endif
  Cerr << "End factorization." << finl;

  //   {
  //     double ANORM=1.0;
  //     double RCOND;
  //     ArrOfDouble WORK(3*N);
  //     ArrOfInt IWORK(N);
  //     F77NAME(DPBCON)(&UPLO, &N, &KD,
  //                     matrice_bande_factorisee_fortran_.addr() , &LDAB,
  //                     &ANORM, &RCOND, WORK.addr(), IWORK.addr(), &INFO);
  //     Cerr<<"//////////// RCOND : "<<RCOND<<finl;
  //   }
  if(INFO)
    {
      Cerr << "Solv_Cholesky::Fact_Cholesky() error : ";
      if(INFO<0) Cerr << "F77NAME(DPBTRF) param. " << -INFO
                        << " invalid" << finl;
      if(INFO>0) Cerr << "submatrix of order " << INFO
                        << " not positive definite!" << finl;
      if (INFO<100)
        {
          for (int i=0; i<INFO; i++)
            Cerr << "A(" <<i<<","<<i<<")="<<mat1(i,i)<<finl;
          Cerr << "non-positive definite matrix ="<<finl;
          mat1.imprimer_formatte(Cerr);
        }
      exit();
    };

  // rovisoire : on pourrait vider matrice et matrice_renumerotee
  return 1;

}



