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
// File:        Eq_couch_lim.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eq_couch_lim_included
#define Eq_couch_lim_included
#include <IntVect.h>

#include <Diffu_totale.h>
#include <Motcle.h>
#include <Ref_Milieu_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Eq_couch_lim
//    Classe resolvant les equations de couche limite simplifiees necessaires
//    a l'utilisation des lois de parois de type TBLE (Thin Boundary
//    Layer Equations)
//
//
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

//Macro qui transforme tous les anciens DoubleTab en ArrOfDouble
//ATTENTION : cette macro n'est valable que si il existe une dimension commune
//a tous les anciens DoubleTab, ici N_comp.

#define tabdouble(__tab__,__i__,__j__) __tab__(__i__+(__j__)*N_comp)

class Eq_couch_lim : public Objet_U
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Eq_couch_lim);

public :

  Eq_couch_lim();
  ~Eq_couch_lim();

  void initialiser(int, int, double, double, int, int) ;

  void aller_au_temps(double) ;
  void aller_jusqu_a_convergence(int, double) ;


  void mailler_fin() ;

  void set_F(int j, double f)
  {
    for(int i=0 ; i<N+1 ; i++)
      {
        tabdouble(F,j,i) = f;
      }
  };
  void set_F(int j, int i, double f)
  {
    tabdouble(F,j,i) = f;
  };
  double  get_F0(int j) const
  {
    return tabdouble(F,j,0);
  };
  double  get_F(int j, int i) const
  {
    return tabdouble(F,j,i);
  };
  void set_y0(double y)
  {
    y0 = y;
  };
  void set_yn(double y)
  {
    yn = y;
  };
  void set_u_y0(int j, double u)
  {
    tabdouble(Unp1,j,0) = u;
  };

  void set_u_yn(int j, double u)
  {
    tabdouble(Unp1,j,N) = u;
  };
  void set_nu_t_yn(double nu_t)
  {
    nu_t_yn = nu_t;
  };
  void set_Uinit_lin(int comp, double f1, double f2)
  {
    //Initialisation de la vitesse
    for (int i = 0 ; i<N+1 ; i++)
      {
        tabdouble(Un_old,comp,i) = (f2 - f1)*i/N + f1;
        tabdouble(Unp1,comp,i) = (f2 - f1)*i/N + f1;
      }
    //Initialisation du frottement
    utau_old = (f2 - f1)/N;
    //utau_old = 1.;
  };

  void set_Uinit(int comp, int i, double val)
  {
    //Initialisation de la vitesse
    tabdouble(Un_old,comp,i) = tabdouble(Unp1,comp,i) = val;
  };

  void set_diffu(Motcle modele)
  {
    a.typer(modele);
    a.valeur().associer_eqn(*this);
  };

  void set_dt(double delta_t)
  {
    dt = delta_t;
  };

  void set_Un_old(int j, int i, double u)
  {
    tabdouble(Un_old,j,i) = u;
  };
  void set_Unp1(int j, int i, double u)
  {
    tabdouble(Unp1,j,i) = u;
  };


  void set_v(int i, double u)
  {
    v[i] = u;
  };

  int get_it()
  {
    return it;
  };
  int get_N()
  {
    return N;
  };
  int get_N_comp()
  {
    return N_comp;
  };
  double get_y0()
  {
    return y0;
  };
  double get_yn()
  {
    return yn;
  };
  double get_nu_t_yn()
  {
    return nu_t_yn;
  };
  double get_nu_t_dyn()
  {
    return nu_t_dyn;
  };

  double get_visco_tot(int i) const
  {
    return visco_tot(i);
  };
  double get_nut(int i)
  {
    Diffu_totale_base& le_a  = a.valeur();
    return le_a.calculer_a_local(i)-le_a.calculer_a_local(0);
  };

  double get_D(int i)
  {
    Diffu_totale_base& le_a  = a.valeur();
    return le_a.calculer_D_local(i);
  };

  double get_Unp1(int j, int i)
  {
    return tabdouble(Unp1,j,i);
  };
  double get_Un_old(int j, int i)
  {
    return tabdouble(Un_old,j,i);
  };
  double get_v(int i)
  {
    return v[i];
  };
  double get_cis(int j)
  {
    return cis(j);
  };
  double get_utau_old()
  {
    return utau_old;
  };
  double get_y(int i)
  {
    return y_(i);
  };
  double get_yc(int i)
  {
    return yc(i);
  };

  double get_Unp1(int j, int i) const
  {
    return tabdouble(Unp1,j,i);
  };
  double get_Un_old(int j, int i) const
  {
    return tabdouble(Un_old,j,i);
  };
  double get_v(int i) const
  {
    return v[i];
  };
  double get_cis(int j) const
  {
    return cis(j);
  };
  double get_utau_old() const
  {
    return utau_old;
  };
  double get_utau() const
  {
    return utau;
  };
  double get_y(int i) const
  {
    return y_(i);
  };
  double get_yc(int i) const
  {
    return yc(i);
  };

  Diffu_totale_base& get_diffu()
  {
    return a.valeur();
  };

  void associer_milieu(const Milieu_base& m)
  {
    milieu = m;
  };
  Milieu_base& get_milieu()
  {
    return milieu.valeur();
  };


private :

  //Les DoubleTab et DoubleVect ont ete remplaces par des
  //ArrOfDouble par souci d'economie memoire.
  //Voir la macro tabdouble pour la transformation des
  //DoubleTab en ArrOfDouble.

  int it, max_it;
  int N, N_comp ; //Nombre de points du maillage fin 1D
  int nu_t_dyn;

  Diffu_totale a ; //diffusivite totale

  ArrOfDouble F ; //second membre
  ArrOfDouble Unp1, Un_old ; // inconnues et champs moyens

  ArrOfDouble v; //vitesse verticale
  ArrOfDouble cis; //gradient a la paroi
  ArrOfDouble aa, bb, cc, dd, visco_tot;
  ArrOfDouble y_, yc;

  double dt, y0, yn, nu_t_yn ;
  double utau, utau_old;
  double facteur;
  double eps;

  REF(Milieu_base) milieu;


};


#endif



