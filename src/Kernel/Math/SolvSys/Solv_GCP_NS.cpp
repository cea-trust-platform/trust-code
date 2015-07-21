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
// File:        Solv_GCP_NS.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Solv_GCP_NS.h>
#include <Matrice_Bloc_Sym.h>
#include <ConstDoubleTab_parts.h>
#include <Motcle.h>

Implemente_instanciable(Solv_GCP_NS,"Solv_GCP_NS",solv_iteratif);
//
// printOn et readOn

Sortie& Solv_GCP_NS::printOn(Sortie& s ) const
{
  return s ;
}

Entree& Solv_GCP_NS::readOn(Entree& is )
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcles les_parametres(5);
  {
    les_parametres[0] = "seuil";
    les_parametres[1] = "solveur1";
    les_parametres[2] = "solveur0";
    les_parametres[3] = "impr";
    les_parametres[4] = "precond";
  }
  int rang;

  Motcle motlu;
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error when reading the parameters of the conjugate gradient SSOR " << finl;
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
            is >> seuil_;
            break;
          }
        case 1:
          {
            is >> solveur_poisson1;
            solveur_poisson1.nommer("poisson_solver1");
            break;
          }
        case 2:
          {
            is >> solveur_poisson0;
            solveur_poisson0.nommer("poisson_solver0");
            break;
          }
        case 3:
          {
            fixer_limpr(1);
            break;
          }
        case 4:
          {
            is >> le_precond_;
            break;
          }
        default :
          {
            Cerr << "Error when reading the parameters of the conjugate gradient SSOR " << finl;
            Cerr << "One expected " << les_parametres << " instead of " << motlu << finl;
            exit();
          }
        }
      is >> motlu;
    }
  return is;
}

static inline void corriger(DoubleVect& X,
                            double alpha,
                            double beta,
                            const DoubleVect& X1,
                            const DoubleVect& X0,
                            int prems, int n)
{
  int sz=prems+n;
  int i=0;
  for(; i<prems; i++)
    X(i)+=beta*X0(i);
  for(; i<sz; i++)
    X(i)+=alpha*X1(i-prems);
}


static inline void injecter(const DoubleVect& X, DoubleVect& tmp,
                            int prems, int n)
{
  int sz=prems+n;
  for(int i=prems; i<sz; i++)
    tmp(i)=X(i-prems);
}
static inline void extraire(DoubleVect& X, const DoubleVect& tmp,
                            int prems, int n)
{
  int sz=prems+n;
  for(int i=prems; i<sz; i++)
    X(i-prems)=tmp(i);
}

inline void erreur()
{
  Cerr << "The dedicated solver GCP_NS can be used only for blocks symetric matrixes" << finl;
  Cerr << "containing 2*2 blocks for VEF discretization P0+P1." << finl;
  Process::exit();
}

int Solv_GCP_NS::resoudre_systeme(const Matrice_Base& matrice,
                                  const DoubleVect& secmem,
                                  DoubleVect& solution,

                                  int niter_max)
{
  // A00 X0 + A01 X1 = F0
  // A10 X0 + A11 X1 = F1
  // X1 = A11^(-1) (F1 - A10 X0)
  // X0 = (A00 -A01 A11^(-1) A10) (F0 - A01 A11^(-1) F1)
  //
  // Gradient conjugue sur ce systeme preconditionne par A00
  //
  //Cerr << "resoudre_systeme : secmem" << secmem << finl;

  //int n = secmem.size();
  assert(solution.size_totale() == secmem.size_totale());
  const int n = secmem.size_totale();

  const Matrice_Bloc_Sym& matbloc=ref_cast(Matrice_Bloc_Sym, matrice);
  if (matbloc.nb_bloc_lignes()!=2 || matbloc.nb_bloc_colonnes()!=2) erreur();

  const Matrice_Base& A00=matbloc.get_bloc(0,0);
  const Matrice_Base& A01=matbloc.get_bloc(0,1);
  const Matrice_Base& A11=matbloc.get_bloc(1,1);
  int n0=A00.nb_lignes();
  int n1=A11.nb_lignes();
  int nmax= max(Process::mp_sum(n), 100);
  int niter = 0;
  double dold,dnew,alfa;

  solution=0.0;
  // Creation de tableaux distribues X1 et F1 sur les sommets
  ConstDoubleTab_parts mo_solution(solution);
  DoubleVect X1=mo_solution[1];
  // DoubleVect X1;
  // champ.domaine().creer_tableau_sommets(X1);
  DoubleVect F1(X1);
  if (X1.size_totale()!=n1) erreur();

  DoubleVect resu(solution);

  // Creation de tableaux distribues X0 et F0 sur les elements
  //DoubleVect X0;
  //champ.domaine().creer_tableau_elements(X0);
  //  ConstDoubleTab_parts mo_solution(solution);
  DoubleVect X0=mo_solution[0];
  DoubleVect F0(X0);
  if (X0.size_totale()!=n0) erreur();

  //F0 :
  extraire(F0, secmem, 0, n0);
  DoubleVect residu(F0);
  //F1 :
  extraire(F1, secmem, n0, n1);
  // X1 = A11^(-1) F1
  solveur_poisson1.resoudre_systeme(A11, F1, X1);
  injecter(X1, solution, n0, n1);
  // F0=A01 X1
  A01.multvect(X1, F0);
  // residu=-F0+A01 X1
  residu -= F0;
  residu *=-1;
  DoubleVect g(F0);
  double norme = mp_norme_vect(residu);
  if(le_precond_.non_nul())
    le_precond_.preconditionner(A00,residu,g);
  else
    solveur_poisson0.resoudre_systeme(A00, residu, g);

  DoubleVect p(g);
  p *= -1.;

  dold = mp_prodscal(residu,g);
  dnew = dold;
  Cout << "GCP NS residue = " << norme << " " << finl;
  double s=0;
  while ( ( norme > seuil_ ) && (niter++ < nmax) )
    {
      // F1=A10 p
      p.echange_espace_virtuel();
      A01.multvectT(p, F1);
      // X1=-A11^(-1) A10p
      if ((limpr())&& (je_suis_maitre())) Cout << "Solveur1: " ;
      solveur_poisson1.resoudre_systeme(A11, F1, X1);
      X1*=-1.;
      //F0=(A00  -A01 A11^(-1) A10) p
      X1.echange_espace_virtuel();
      A01.multvect(X1, F0);
      A00.ajouter_multvect(p, F0);
      s = mp_prodscal(F0,p);
      alfa = dold/(s);
      corriger(solution, alfa, alfa, X1, p, n0, n1);
      residu.ajoute(alfa,F0);
      norme = mp_norme_vect(residu);
      if(norme>seuil_ && dnew>0)
        {
          if ((limpr())&& (je_suis_maitre())) Cout << "Solveur0: " ;
          if(le_precond_.non_nul())
            le_precond_.preconditionner(A00,residu,g);
          else
            solveur_poisson0.resoudre_systeme(A00, residu, g);
          dnew = mp_prodscal(residu,g);
          p *= (dnew/dold);
          p -= g;
          dold = dnew;
          if (dnew < 0)
            {
              Cerr << "Error : dnew<0 in GCP NS solver." << finl;
              Cerr << "Try to reduce the threshold value of solveur0" << finl;
              Cerr << "to a value inferior to those of the GCP NS solver." << finl;
              exit();
            }
        }
      if ((limpr())&& (je_suis_maitre()))
        {
          Cout << "GCP NS residue = " << norme << " " << finl;
          if ((niter % 15) == 0) Cout << finl ;
        }
    }
  if(norme > seuil_)
    {
      Cerr << "No convergence after : " << niter << " iterations\n";
      Cerr << " Residue : "<< norme << "\n";
    }
  solution.echange_espace_virtuel();
  return(niter);
}

int Solv_GCP_NS::
resoudre_systeme(const Matrice_Base& la_matrice,
                 const DoubleVect& secmem,
                 DoubleVect& solution
                )
{
  return resoudre_systeme(la_matrice, secmem, solution,  1000000);
}




