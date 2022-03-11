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
// File:        Simpler_Base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Simpler_Base_included
#define Simpler_Base_included


#include <Solveur_non_lineaire.h>
#include <Ref_Schema_Temps_base.h>
#include <Parametre_implicite.h>
#include <Matrice_Morse.h>
#include <TRUSTTab.h>
#include <Nom.h>
class Solveur_Implicite_Base;
class Equation;
class Equation_base;
class DoubleTrav;
class Matrice;
class Sources;
class Motcle;

//Description
//
//Classe de base des classes dediees a la resolution implicite d une equation
//L equation est discretisee sous la forme suivante :
// (M/dt + C(U) + D)Xn+1 = Sv + Ss + (M/dt)Xn
// avec
// C(U) matrice de convection
// D    matrice de diffusion
// M    matrice de masse
// Sv   terme source volumique
// Ss  terme source surfacique (condition de Neumann)
// Xn et Xn+1 designe l inconnue respectivement aux temps tn et tn+1
//
// A chaque etape en temps (tn) on va traiter la resolution du systeme suivant :
//         A[Xk] = b[Xk]
// avec
// A[Xk] = (M/dt + C(Uk) + D)
// b[Xk] = Sv + Ss + (M/dt)Xk
// k designe un indice d iteration pour la resolution iterative du systeme matriciel (tn fixe)

// La methode de resolution employee pour determiner X est une methode de point fixe :
// On pose f(Xk) = A[Xk]Xk -b[Xk] ainsi Xsol solution verifie f(Xsol) = 0
// Le developpement limite au premier ordre donne la relation :
// Xk - Xk+1 = f(Xk)/f'(Xk)
// En faisant l hyposthese que f' = A (pas vrai en toute rigueur pour la partie convection)
// le systeme s ecrit : A[Xk](Xk-Xk+1) = f(Xk) ou encore
//                                 A[Xk]Xk+1 = A[Xk]Xk - (A[Xk]Xk-Ss) + Sv +(M/dt)Xk
//
//
// Methode iterer_eqn() pour traiter une equation autre que Navier_Stokes
// - fait construire la matrice (matrice) et le second membre (resu) par l equation (assembler_avec_inertie(...))
// - declenche la resolution du systeme matriciel (le_solveur_.resoudre_systeme(...))

// Methode iterer_NS() appelee par iterer_eqn() pour traiter le cas specifique de l equation de Navier_Stokes
// Les algorithmes disponibles sont : Simple - Simpler - Piso - Implicite
// Voir classes filles de Simpler_base pour la description des algorithmes respectifs


class Simpler_Base : public  Solveur_non_lineaire
{
  Declare_base_sans_constructeur(Simpler_Base);

public :

  inline int max_iter_implicite();
  inline int max_iter_implicite() const;

  bool iterer_eqn(Equation_base& equation, const DoubleTab& inconnue, DoubleTab& result, double dt, int numero_iteration, int& ok) override =0;

  virtual void iterer_NS(Equation_base&, DoubleTab& current, DoubleTab& pression, double, Matrice_Morse&, double, DoubleTrav&,int nb_iter,int& converge, int& ok)=0;

  void assembler_matrice_pression_implicite(Equation_base& eqn_NS,const Matrice_Morse& matrice,Matrice& matrice_en_pression_2);
  Parametre_implicite& get_and_set_parametre_implicite(Equation_base&);

protected :

  Parametre_implicite param_defaut_;
  int is_seuil_convg_variable;
  double facteur_convg_;
  int no_qdm_;
  int controle_residu_;

  virtual Entree& lire(const Motcle&, Entree&)=0;
};

#endif
