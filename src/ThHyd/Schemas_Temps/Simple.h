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
// File:        Simple.h
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Simple_included
#define Simple_included

#include <Simpler_Base.h>
#include <DoubleTab.h>

class Operateur_Grad ;

//Description

//Ref. International Journal For Numerical Methods In Fluids Vol. 12 P. 81-92
// C. T. Shaw

// A = (M/dt + C(Uk) + D)
// Bt et -B designent respectivement les operateurs gradient et divergence

// Une solution (U,P) est recherchee sous la forme suivante :
// U = U* + u'
// P = P* + p'
// ou (U*,P*) est solution de l'equation de q.d.m. et (u',p') est une correction apportee
// a celle-ci pour satisfaire l equation de continuite

// (U*,P*) satisfait l equation de q.d.m. :
//        A[Uk-1]U*k = -BtP* + Sv + Ss + (M/dt)Uk-1                -> U*k
//
// En retranchant cette equation a l equation de q.d.m pour (U,P) on a une equation sur  u' et p'
//        A[Uk-1]u' = -Btp'

// En ne conservant que la partie diagonale (Da) de A la relation s ecrit :
//        u' = -Da-1Btp'

//Le champ complet U satisfait l equation de continuite ce qui donne la relation :
//        -Bu' = BU*
// soit (BDa-1Bt)p' = BU*                                         -> p' puis u'

// En fin d iteration la solution s ecrit :
//        U = U* + beta_u u'
//        P = P* + beta_p p'
// beta_u et beta_p sont des coefficients de relaxation compris entre 0 et 1.
// En pratique on applique une relaxation uniquement pour la pression

// L algorithme peut etre repete jusqu a convergence du systeme ||Uk-Uk-1|| < seuil_convergence_implicite_
// en pratique on peut ne faire qu une seule iteration (seuil_convergence_implicite_ = 1e6)


class Simple : public Simpler_Base
{
  Declare_instanciable_sans_constructeur(Simple);

public :

  Simple();
  virtual bool iterer_eqn(Equation_base& equation, const DoubleTab& inconnue, DoubleTab& result, double dt, int numero_iteration);
  virtual void iterer_NS(Equation_base&, DoubleTab& current, DoubleTab& pression, double, Matrice_Morse&, double, DoubleTrav&,int nb_iter,int& converge);
  virtual bool iterer_eqs(LIST(REF(Equation_base)) eqs, int compteur, bool test_convergence);

protected :

  DoubleTab Ustar_old;        //U* = alpha_ U*_new + (1-alpha_)*U*_old   en pratique alpha = 1
  double alpha_,beta_;  //beta_ coefficient de relaxation pour la pression  P = P* + beta_*P'  0<beta_<=1
  int with_d_rho_dt_;

  virtual Entree& lire(const Motcle&, Entree&);
  void calculer_correction_en_vitesse(const DoubleTrav& correction_en_pression, DoubleTrav& gradP, DoubleTrav& correction_en_vitesse,const Matrice_Morse& matrice, const Operateur_Grad& gradient );

};

void diviser_par_rho_np1_face(Equation_base& eqn, DoubleTab& tab);

#endif
