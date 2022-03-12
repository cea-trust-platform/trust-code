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
// File:        RRK2.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <RRK2.h>
#include <Equation.h>
#include <TRUSTTrav.h>

Implemente_instanciable(RRK2,"Runge_Kutta_Rationnel_ordre_2",Schema_Temps_base);


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
Sortie& RRK2::printOn(Sortie& s) const
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
Entree& RRK2::readOn(Entree& s)
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
int RRK2::nb_valeurs_temporelles() const
{
  return 2 ;
}

// Description:
//    Renvoie le nombre de valeurs temporelles futures.
//    Ici : n+1, donc 1.
int RRK2::nb_valeurs_futures() const
{
  return 1 ;
}

// Description:
//    Renvoie le le temps a la i-eme valeur future.
//    Ici : t(n+1)
double RRK2::temps_futur(int i) const
{
  assert(i==1);
  return temps_courant()+pas_de_temps();
}

// Description:
//    Renvoie le le temps le temps que doivent rendre les champs a
//    l'appel de valeurs()
//    Ici : t(n+1)
double RRK2::temps_defaut() const
{
  return temps_courant()+pas_de_temps();
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////


// Description:
//    Effectue un pas de temps de Runge Kutta rationel d'ordre 2,
//    sur l'equation passee en parametre.
//    Le schema de Runge Kutta  rationel d'ordre 2:
//     g1=hf(y0)
//     g2=hf(y0+c2g1)
//     y1=y0+(g1g1)/(b1g1+b2g2)
//     ou ab/d = (a(b,d)+b(d,a)-d(a,b)/(d,d)
//     y1=y0+(2g1(g1,b1g1+b2g2)-(b1g1+b2g2)(g1,g1)/(b1g1+b2g2,b1g1+b2g2)
//     y1=y0+(2g1(g1,"g2")-("g2")(g1,g1)/("g2","g2")
//      ordre2 si b2c2=-1/2
//     b2c2<=-1/2 A0 stabilite et I stabilite
//     b2c2<= 1/(2cos(alpha)(2-cos(alpha))) O<=alpha<pi/2 Aalpha stabilite
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
int RRK2::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{
  // Warning sur les 100 premiers pas de temps si facsec est egal a 1
  // pour faire reflechir l'utilisateur
  int nw = 100;
  if ( nb_pas_dt()>=0 && nb_pas_dt()<=nw && facsec_ == 1 )
    {
      Cerr << finl << "**** Advice (printed only on the first " << nw << " time steps)****" << finl;
      Cerr << "You are using Runge Kutta schema order 2 so if you wish to increase the time step, try facsec between 1 and 2." << finl;
    }

  double b1=2.0;
  double c2=0.5;
  double b2=-1;

  DoubleTab& present=eqn.inconnue().valeurs();
  DoubleTab& futur=eqn.inconnue().futur();

  // g1=futur=f(y0)
  DoubleTab g1(present) ;
  DoubleTab g2(present) ;

  // sauv=y0
  DoubleTrav sauv(present);
  sauv=present;

  eqn.derivee_en_temps_inco(g1);

  // g1=hf(y0)
  g1*=dt_;

  // present=y0+c2g1
  present.ajoute(c2, g1);

  // g2=futur=f(y0+c2g1)
  eqn.derivee_en_temps_inco(g2);

  // g2=b2"g2"
  g2*=(b2*dt_);

  // g2=b2"g2" + b1g1
  g2.ajoute(b1, g1);

  // normeg2=("g2","g2")
  double normeg2 = mp_carre_norme_vect(g2) + DMINFLOAT;
  // normeg1=-(g1,g1)
  double normeg1 = - mp_carre_norme_vect(g1);
  // psc1=2(g1,"g2")
  double psc1 = 2.0 * mp_prodscal(g1, g2);

  // y1=y0+(2g1(g1,"g2")-("g2")(g1,g1)/("g2","g2")
  futur=g1;
  futur*=psc1;
  futur.ajoute(normeg1, g2);
  futur/=normeg2;
  present=sauv;
  futur /= dt_;
  update_critere_statio(futur, eqn);
  futur *= dt_;
  futur+=sauv;
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(),temps_courant()+pas_de_temps());
  futur.echange_espace_virtuel();

  return 1;
}
