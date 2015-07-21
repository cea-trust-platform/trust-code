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
// File:        Piso.h
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Piso_included
#define Piso_included

#include <Simpler.h>

//Description

// Ref. Journal of Computational Physics 62 P. 40-65
// R. I. Issa

// A = (M/dt + C(Uk) + D)
// Bt et -B designent respectivement les operateurs gradient et divergence
// delta_x designe l operateur Laplacien
// H designe l operateur convection+diffusion

// L algorithme PISO (Pressure Implicit Splitting Operator) est non iteratif
// et se decompose en trois etapes * ** et ***.
// Les etapes de l algorithme sont :

// -ETAPE DE PREDICTION
//
// Recherche du champ de vitesse U* staisfaisant l equation de q.d.m. (avec pression Pn)
//        (rho/dt)*(U*-Un) = H(U*) - BtP + S
// U* ne satisfait pas l equation de continuite

// L etape de prediction est realisee en traitant le systeme suivant :
//        AU* = -BtPn + Sv + Ss + (M/dt)Un                -> U*


// -PREMIERE ETAPE DE CORRECTION
//
// Recherche de U** et P* satisfaisant la q.d.m. et l equation de continuite (a partir de U* et Un)
//        (rho/dt)*(U**-Un) = H(U*) - BtP* + S
//        delta_x U** = 0

// Afin de stabiliser le systeme a traiter on implicite la partie diagonale de convection-diffusion
//        (rho/dt-Ao)*U** - (rho/dt)*Un = H'(U*) -BtP* + S avec H(U) = H'(U) + AoU
// En retranchant l equation de prediction :
//        (rho/dt-Ao)*(U**-U*) = -Bt(P*-Pn)        et d autre part delta_x U** = 0
//
// L etape de premiere correction est realisee en traitant (Da = rho/dt-Ao):
//        (BDa-1Bt)P' = BU*                                -> P' -> P* = Pn + P'
// puis   Da[Un]U' = -BtP'                                -> U' -> U** = U* + U'


//-SECONDE ETAPE DE CORRECTION
//
// Recherche de U*** et P** satisfaisant la q.d.m. et l equation de continuite (a partir de U** et Un)
//        (rho/dt)*(U***-Un) = H(U**) - BtP** + S
//        delta_x U*** = 0

// Afin de stabiliser le systeme a traiter on implicite la partie diagonale de convection-diffusion
//        (rho/dt-Ao)*U*** - (rho/dt)*Un = H'(U**) -BtP** + S
// En retranchant l equation de premiere correction reformulee a l equation de seconde correction reformulee :
//        (rho/dt-Ao)*(U***-U**) = H'(U**-U*) -Bt(P**-P*)        et d autre part delta_x U*** = 0 et delta_x U** = 0
//
// L etape de seconde correction est realisee en traitant (E = H'):
//        (BDa-1Bt)P'' =  (BDa-1E)U'                        -> P'' -> P** = P* + P''
//         DaU'' = EU' -BtP''                                -> U'' -> U*** = U** + U''

// Rq : Des etapes de correction supplementaires peuvent etre realisees.
// La version codee ici poursuit les corrections (compt_max-1 maximum)
// sauf si le residu augmente par rapport a la correction precedente.


class Piso : public Simpler
{

  Declare_instanciable_sans_constructeur(Piso);

public :

  Piso();
  virtual void iterer_NS(Equation_base&, DoubleTab& current, DoubleTab& pression, double, Matrice_Morse&, double, DoubleTrav&,int nb_iter,int& converge);

protected :

  int nb_corrections_max_; //nombre de corrections maximun pour affinet la projection
  int avancement_crank_;   // on ne fait pas vraiment du piso mais plutot du CN

  virtual Entree& lire(const Motcle&, Entree&);

};

//Description
// Ref. G. Fauchet

// L algorithme IMPLICITE est non iteratif et se decompose en deux etapes * et **.
// Les etapes de l algorithme sont :

// -ETAPE DE PREDICTION
//
// Recherche du champ de vitesse U* staisfaisant l equation de q.d.m. (avec pression Pn)
// De facon identique a l algorithme PISO le terme de diffusion est exprime sous forme implicite
// et le terme de convection sous forme semi-implicite.

//         (U*-Un)/dt = H(U*) - BtPn + Sv + Ss
// U* ne satisfait pas l equation de continuite

// L etape de prediction est realisee en traitant le systeme suivant :
//        AU* = -BtPn + Sv + Ss + (M/dt)Un                -> U*

// -ETAPE DE CORRECTION
//
// L equation de q.d.m. a l etape n+1 peut s ecrire :
//         (Un+1-Un)/dt = H(Un+1) - BtPn+1 + Sv + Ss
// En retranchant l 'equation de q.d.m. ecrite pour U* et en negligeant
// les termes de convection et diffusion sur U' = Un+1 - U* on a la relation :
//         (Un+1-U*)/dt = -BtP'        avec P' = Pn+1 - Pn
//
// L etape de correction est realisee en traitant (M matrice de masse) :
//        (BM-1Bt)dt*P' = BU*                                -> dt*P' -> P'
//         Un+1 = U* -dt*BtP'                                -> Un+1
//
// Rq : La matrice de masse est constante par consequent le systeme (BM-1Bt) n est assemble qu une seule fois.


class Implicite : public Piso
{
  Declare_instanciable(Implicite);
};


#endif

