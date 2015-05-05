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
// File:        Simpler.h
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Simpler_included
#define Simpler_included
#include <Simple.h>

//Description

//Ref. Numerical heat Transfer Vol. 10 P. 209-228
// D. S. Jang - R. Jetli - S. Acharya

// La convergence de la pression dans l'algorithme Simple (voir classe Simple) est faible
// du fait de la non prise en compte des termes non diagonaux pour resoudre l equation de
// Poisson sur la correction en pression.
// L objectif de l algorithme Simpler est de realiser une evaluation plus precise de la pression
// et d utiliser celle ci pour appliquer ensuite l algorithme Simple.

// A = (M/dt + C(Uk) + D)
// Bt et -B designent respectivement les operateurs gradient et divergence

// -Estimation de la pression
// On evalue dans un premier temps le champ de vitesse UPk en resolvant le systeme
// (Da partie diagonale de A et E partie non diagonale) :
//        Da[Uk-1]UPk = E[Uk-1]Uk-1 + Sv + Ss                        -> UPk
//
// L equation de q.d.m. peut alors s ecrire sous la forme :
//        Da[Uk-1]Uk = Da[Uk-1]UPk -BtPk
// En combinant cette equation de q.d.m. avec celle de continuite (-BUk = 0) on a :
//        (BDa-1Bt)Pk = BUPk                                -> Pk
//
//
// -Application de l algorithme Simple pour determiner la solution (Uk,Pk)
//
// (U*k,Pk) satisfait l equation de q.d.m. suivante :
//        A[Uk-1]U*k = -BtPk + Sv + Ss + (M/dt)Uk-1        -> U*k
//
// p'k est evalue en resolvant le systeme suivant :
//     (BDa-1Bt)p'k = BU*k                                -> p'k
//
// La correction de vitesse u'k est deduite en resolvant le systeme :
//        Da[Uk-1] (Uk-U*k) = -Btp'k                        -> U'k
//
// Le champ de pression Pk n est pas modifie par la correction p'k
// alors que la vitesse est modifiee par la relation : Uk = U*k + U'k
//
// L algorithme peut etre repete jusqu a convergence du systeme ||Uk-Uk-1|| < seuil_convergence_implicite_
// en pratique on peut ne faire qu une seule iteration (seuil_convergence_implicite_ = 1e6)

// L algorithme code dans cette classe (iterer_NS) differe legerement de celui rappele ci-dessus :
// Dans l etape d evaluation UPk :
//   -le gradient de pression est pris en compte et la relation traitee est
//         Da[Uk-1]UPk = E[Uk-1]Uk-1 + Sv + Ss -BtPk
//
//   -la resolution de (BDa-1Bt)Pcor = BUPk fournit alors une correction de pression
//    et le champ de pression est evalue par : Pk = Pk-1 + Pcor
//    (Pcor remplace la notation Pk utilisee plus haut)
//
//   -resu est reajuste (BtPk = BtPk-1 + BtPcor) pour contenir -BtPk

class Simpler : public Simple
{

  Declare_instanciable(Simpler);

public :
  virtual void iterer_NS(Equation_base&, DoubleTab& current, DoubleTab& pression, double, Matrice_Morse&, double, DoubleTrav&,int nb_iter,int& converge);



protected :


};

int inverser_par_diagonale(const Matrice_Morse& matrice,const DoubleTrav& resu,const DoubleTab& present,DoubleTrav& correction_en_vitesse);

#endif

