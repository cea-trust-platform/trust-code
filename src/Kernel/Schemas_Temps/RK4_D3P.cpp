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
// File:        RK4_D3P.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <RK4_D3P.h>
#include <Equation.h>

Implemente_instanciable(RK4_D3P,"Runge_Kutta_ordre_4_D3P",Schema_Temps_base);


// Description:
//    Simple appel a: Schema_Temps_base::printOn(Sortie& )
//    Ecrit le schema en temps sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& RK4_D3P::printOn(Sortie& s) const
{
  return  Schema_Temps_base::printOn(s);
}


// Description:
//    Lit le schema en temps a partir d'un flot d'entree.
//    Simple appel a: Schema_Temps_base::readOn(Entree& )
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& RK4_D3P::readOn(Entree& s)
{
  return Schema_Temps_base::readOn(s) ;
}


////////////////////////////////
//                            //
// Caracteristiques du schema //
//                            //
////////////////////////////////


// Description:
//    Renvoie le nombre de valeurs temporelles a conserver.
//    Ici : n et n+1, donc 2.
int RK4_D3P::nb_valeurs_temporelles() const
{
  return 2 ;
}

// Description:
//    Renvoie le nombre de valeurs temporelles futures.
//    Ici : n+1, donc 1.
int RK4_D3P::nb_valeurs_futures() const
{
  return 1 ;
}

// Description:
//    Renvoie le le temps a la i-eme valeur future.
//    Ici : t(n+1)
double RK4_D3P::temps_futur(int i) const
{
  assert(i==1);
  return temps_courant()+pas_de_temps();
}

// Description:
//    Renvoie le temps que doivent rendre les champs a
//    l'appel de valeurs()
//    Ici : t(n+1)
double RK4_D3P::temps_defaut() const
{
  return temps_courant()+pas_de_temps();
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////


// Description:
//    Effectue un pas de temps de Runge Kutta d'ordre 4,
//    sur l'equation passee en parametre.
//    Le schema de Runge Kutta  d'ordre 4
//     degnere (schema a trois points)
//     (cas 17 de Williamson) s'ecrit :
//     q1=h f(x0)
//     x1=x0+b1 q1
//     q2=h f(x1) +a2 q1
//     x2=x1+b2 q2
//     q3=h f(x2)+a3 q2
//     x3=x2+b3 q3
//      avec a1=0, a2=-1/2, a3=-2
//                              b1=1/2, b2=1, b3=1/6
// Precondition:
// Parametre: Equation_base& eqn
//    Signification: l'equation que l'on veut faire avancer d'un
//                   pas de temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int RK4_D3P::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{

  // Warning sur les 100 premiers pas de temps si facsec est egal a 1
  // pour faire reflechir l'utilisateur
  int nw = 100;
  if ( nb_pas_dt()>=0 && nb_pas_dt()<=nw && facsec_ == 1 )
    {
      Cerr << finl << "**** Advice (printed only on the first " << nw << " time steps)****" << finl;
      Cerr << "You are using Runge Kutta schema order 4 so if you wish to increase the time step, try facsec between 1 and 4." << finl;
    }

  double a2=-1./2.;
  double a3=-2.;
  double b1=1./2.;
  double b2=1.;
  double b3=1./6.;

  DoubleTab& xi=eqn.inconnue().valeurs();
  DoubleTab& xipls1=eqn.inconnue().futur();
  DoubleTab  qi(xi) ;
  DoubleTab present(xi);

  // qi=futur=f(y0)
  eqn.derivee_en_temps_inco(qi);

  // q1=h f(x0)
  qi *= dt_;

  //  x1=x0+b1 q1
  xi.ajoute(b1, qi);

  // q2=h f(x1) +a2 q1
  qi*=a2;


  eqn.derivee_en_temps_inco(xipls1);

  qi.ajoute(dt_,xipls1);

  // x2=x1+b2 q2
  xi.ajoute(b2, qi);

  // q3=h f(x2)+a3 q2
  qi*=a3;

  eqn.derivee_en_temps_inco(xipls1);

  qi.ajoute(dt_,xipls1);

  // x3=x2+b3 q3
  xi.ajoute(b3, qi);

  xipls1=xi;
  xi -= present;
  xi /= dt_;
  update_critere_statio(xi, eqn);
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(),temps_courant()+pas_de_temps());
  xipls1.echange_espace_virtuel();
  xi=present;

  return 1;
}




