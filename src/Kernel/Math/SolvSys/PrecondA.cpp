/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        PrecondA.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <PrecondA.h>
#include <Matrice_Morse_Sym.h>
#include <Matrice_Bloc_Sym.h>
#include <Motcle.h>
#include <TRUSTTab_parts.h>

Implemente_instanciable(PrecondA,"ssor_bloc",Precond_base);
//
// printOn et readOn

Sortie& PrecondA::printOn(Sortie& s ) const
{
  return s;
}

Entree& PrecondA::readOn(Entree& is )
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcles les_parametres(6);
  {
    les_parametres[0] = "precond0";
    les_parametres[1] = "precond1";
    les_parametres[2] = "preconda";
    les_parametres[3] = "alpha_0";
    les_parametres[4] = "alpha_1";
    les_parametres[5] = "alpha_a";
    int rang;

    Motcle motlu;
    is >> motlu;
    if (motlu != accolade_ouverte)
      {
        Cerr << "Error when reading the parameters of the conjugate gradient PrecondA " << finl;
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
              is >> le_precond_0;
              break;
            }
          case 1:
            {
              is >> le_precond_1;
              break;
            }
          case 2:
            {
              is >> le_precond_a;
              break;
            }
          case 3:
            {
              is >> alpha_0;
              break;
            }
          case 4:
            {
              is >> alpha_1;
              break;
            }
          case 5:
            {
              is >> alpha_a;
              break;
            }
          default :
            {
              Cerr << "Error when reading the parameters of the preconditioning PrecondA " << finl;
              Cerr << "On expected " << les_parametres << " instead of " << motlu << finl;
              exit();
            }
          }
        is >> motlu;
      }
  }
  return is;
}

static void prepare_precond(Precond& p, const Matrice_Base& m, const DoubleVect& v,
                            Precond_base::Init_Status status)
{
  if (p.non_nul())
    {
      Precond_base& pp = p.valeur();
      pp.reinit(status);
      pp.prepare(m, v);
    }
}

void PrecondA::prepare_(const Matrice_Base& m, const DoubleVect& v)
{
  Init_Status status = get_status();
  const Matrice_Bloc_Sym& matbloc = ref_cast(Matrice_Bloc_Sym, m);
  ConstDoubleTab_parts parts(v);
  prepare_precond(le_precond_0, matbloc.get_bloc(0,0), parts[0], status);
  prepare_precond(le_precond_1, matbloc.get_bloc(1,1), parts[1], status);
  if (parts.size() > 2)
    prepare_precond(le_precond_a, matbloc.get_bloc(2,2), parts[2], status);
  Precond_base::prepare_(m, v);
}

// Description:
//    Calcule la solution du systeme lineaire: A * solution = b
//    avec la methode de relaxation PrecondA.
int PrecondA::preconditionner_(const Matrice_Base& matrice,
                               const DoubleVect& b,
                               DoubleVect& solution)
{

  double pscb = mp_prodscal(b, b);

  if(pscb<1.e-24)
    {
      solution=b;
      return 1;
    }
  // On calcule solution = inverse(C)*b avec:
  //   inverse(C) = inverse((1/w D - E)) *(2-w/w D) * inverse((1/wD -E)t)
  //   D :partie diagonale des blocs de la matrice
  //   E :partie triangulaire inferieure des blocs de la matrice
  const Matrice_Bloc_Sym& matbloc=ref_cast(Matrice_Bloc_Sym, matrice);
  if (matbloc.nb_bloc_lignes()<2
      || matbloc.nb_bloc_colonnes()<2
      || matbloc.nb_bloc_lignes()!=matbloc.nb_bloc_colonnes())
    {
      Cerr << "The preconditioning SSOR_BLOC can be used only for symetric block matrixes" << finl;
      Cerr << "containing at least 2*2 blocs. For example, when considering the following VEF discretizations :"<< finl;
      Cerr << "P0+P1 or P0+Pa or P0+P1+Pa." << finl;
      exit();
    }
  int nblocs=matbloc.nb_bloc_lignes();
  const Matrice_Base& A00=matbloc.get_bloc(0,0);
  const Matrice_Base& A01=matbloc.get_bloc(0,1);
  const Matrice_Base& A11=matbloc.get_bloc(1,1);
  const Matrice_Base& A0a=matbloc.get_bloc(0,nblocs-1);
  const Matrice_Base& A1a=matbloc.get_bloc(1,nblocs-1);
  const Matrice_Base& Aaa=matbloc.get_bloc(nblocs-1,nblocs-1);
  DoubleTab_parts parties(solution);
  DoubleVect& X0 = parties[0];
  DoubleVect& X1 = parties[1];
  DoubleVect& Xa = parties[parties.size() - 1];

  DoubleVect F(b);
  DoubleTab_parts parties_f(F);
  DoubleVect& Y0 = parties_f[0];
  DoubleVect& Y1 = parties_f[1];
  DoubleVect& Ya = parties_f[parties_f.size() - 1];

#ifdef TRACE
  Cerr << finl << "Y0 " << mp_norme_vect(Y0) << finl;
  Cerr << finl << "Y1 " << mp_norme_vect(Y1) << finl;
  if(nblocs == 3)
    Cerr << finl << "Ya " << mp_norme_vect(Ya) << finl;
  matrice.multvect(b, F);
  double psc=mp_prodscal(F,b);
  assert(psc>0);
  double pscF=mp_prodscal(F,F);
  Cout << finl <<"cos(angle) no prec " << psc*psc/(pscF*pscb) << finl;
  Cout << finl <<"(F,b) " << psc << finl;
  Cout << finl <<"(b,b) " << pscb << finl;
  Cout << finl <<"(F,F) " << pscF << finl;
  F.ajoute(-psc/pscb,b,VECT_ALL_ITEMS);

  Cout << finl <<"(F0,F0) " << mp_prodscal(Y0,Y0) << finl;
  Cout << finl <<"(F1,F1) " << mp_prodscal(Y1,Y1) << finl;
  if(nblocs == 3)
    Cout << finl <<"(Fa,Fa) " << mp_prodscal(Ya,Ya) << finl;
  F=b;
#endif

  // Descente :

  le_precond_1.preconditionner(A11,Y1,X1);
  operator_multiply(X1, -alpha_1, VECT_ALL_ITEMS);
  A11.ajouter_multvect(X1, Y1);
  A01.ajouter_multvect(X1, Y0);
  if(nblocs == 3)
    A1a.ajouter_multvectT(X1, Ya);
  operator_multiply(X1, -1, VECT_ALL_ITEMS);
  le_precond_0.preconditionner(A00,Y0,X0);
  operator_multiply(X0, -alpha_0, VECT_ALL_ITEMS);
  A00.ajouter_multvect(X0, Y0);
  A01.ajouter_multvectT(X0, Y1);
  if(nblocs == 3)
    A0a.ajouter_multvectT(X0, Ya);
  operator_multiply(X0, -1, VECT_ALL_ITEMS);
  if(nblocs == 3)
    {
      le_precond_a.preconditionner(Aaa,Ya,Xa);
      operator_multiply(Xa, -alpha_a, VECT_ALL_ITEMS);
      Aaa.ajouter_multvect(Xa, Ya);
      A1a.ajouter_multvect(Xa, Y1);
      A0a.ajouter_multvect(Xa, Y0);
      operator_multiply(Xa, -1, VECT_ALL_ITEMS);
    }
  // X contient S,
  F=solution;
  // Diagonale :

  A11.multvect(Y1,X1);
  operator_multiply(X1, 1./alpha_1, VECT_ALL_ITEMS);
  A00.multvect(Y0,X0);
  operator_multiply(X0, 1./alpha_0, VECT_ALL_ITEMS);
  if(nblocs == 3)
    {
      Aaa.multvect(Ya,Xa);
      operator_multiply(Xa, 1./alpha_a, VECT_ALL_ITEMS);
    }
  // X contient T,
  F=solution;

  // Remontee :

  if(nblocs == 3)
    {
      le_precond_a.preconditionner(Aaa,Ya,Xa);
      operator_multiply(Xa, -alpha_a, VECT_ALL_ITEMS);
      Aaa.ajouter_multvect(Xa, Ya);
      A1a.ajouter_multvect(Xa, Y1);
      A0a.ajouter_multvect(Xa, Y0);
      operator_multiply(Xa, -1., VECT_ALL_ITEMS);
    }
  le_precond_0.preconditionner(A00,Y0,X0);
  operator_multiply(X0, -alpha_0, VECT_ALL_ITEMS);
  A00.ajouter_multvect(X0, Y0);
  A01.ajouter_multvectT(X0, Y1);
  if(nblocs == 3)
    A0a.ajouter_multvectT(X0, Ya);
  operator_multiply(X0, -1., VECT_ALL_ITEMS);
  le_precond_1.preconditionner(A11,Y1,X1);
  operator_multiply(X1, -alpha_1, VECT_ALL_ITEMS);
  A11.ajouter_multvect(X1, Y1);
  A01.ajouter_multvect(X1, Y0);
  if(nblocs == 3)
    A1a.ajouter_multvectT(X1, Ya);
  operator_multiply(X1, -1., VECT_ALL_ITEMS);
#ifdef TRACE
  matrice.multvect(solution, F);
  psc=mp_prodscal(F,b);
  assert(psc>0);
  pscF=mp_prodscal(F,F);
  Cout << finl <<"cos(angle) " << psc*psc/(pscF*pscb) << finl;
  Cout << finl <<"(F,b) " << psc << finl;
  Cout << finl <<"(b,b) " << pscb << finl;
  Cout << finl <<"(F,F) " << pscF << finl;
  F.ajoute(-psc/pscb,b);

  Cout << finl <<"(F0,F0) " << mp_prodscal(Y0,Y0) << finl;
  Cout << finl <<"(F1,F1) " << mp_prodscal(Y1,Y1) << finl;
  if(nblocs == 3)
    Cout << finl <<"(Fa,Fa) " << mp_prodscal(Ya,Ya) << finl;
#endif

  return 1;
}
