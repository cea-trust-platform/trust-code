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
// File:        Leap_frog.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Leap_frog.h>
#include <Equation.h>

Implemente_instanciable(Leap_frog,"Leap_frog",Schema_Temps_base);


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
Sortie& Leap_frog::printOn(Sortie& s) const
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
Entree& Leap_frog::readOn(Entree& s)
{
  //
  // VERIFIER ICI QU'ON A BIEN TOUT LU;
  //
  return Schema_Temps_base::readOn(s) ;
}


////////////////////////////////
//                            //
// Caracteristiques du schema //
//                            //
////////////////////////////////


// Description:
//    Renvoie le nombre de valeurs temporelles a conserver.
//    Ici : n-2, n-1, n, et n+1, donc 4.
int Leap_frog::nb_valeurs_temporelles() const
{
  return 4;
}

// Description:
//    Renvoie le nombre de valeurs temporelles futures.
//    Ici : n+1, donc 1.
int Leap_frog::nb_valeurs_futures() const
{
  return 1 ;
}

// Description:
//    Renvoie le le temps a la i-eme valeur future.
//    Ici : t(n+1)
double Leap_frog::temps_futur(int i) const
{
  assert(i==1);
  return temps_courant()+pas_de_temps();
}

// Description:
//    Renvoie le le temps le temps que doivent rendre les champs a
//    l'appel de valeurs()
//    Ici : t(n+1)
double Leap_frog::temps_defaut() const
{
  return temps_courant()+pas_de_temps();
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////


// Description:
//    Effectue un pas de temps Leap_frog, sur l'equation
//    passee en parametre.
// Precondition:
// Parametre: Equation_base& eq
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
int Leap_frog::faire_un_pas_de_temps_eqn_base(Equation_base& eq)
{
  // EXEMPLE POUR UN SCHEMA EXPLICITE UTILISANT U(n) et U(n+1)
  // Un+1=Un+dt*F
  //  Un
  DoubleTab& present = eq.inconnue().valeurs();

  // Un+1
  DoubleTab& futur = eq.inconnue().futur();

  // Un+1=F
  eq.derivee_en_temps_inco(futur);
  futur.echange_espace_virtuel();

  // ECRIRE ICI LE CALCUL de U(n+1) EN FONCTION DE U(n) ET DE derivee
  // futur*=dt; Un+1=dt*F
  // futur+=present; Un+1=Un+dt*F
  // SI IL S'AGISSAIT D'UN SCHEMA QUI FAIT INTERVENIR U(n-1)
  //    if ((nb_pas_dt_>4) && ((nb_pas_dt_ % 10) != 0)) {
  // if (nb_pas_dt_>4) {
  if ((nb_pas_dt_>4) && ((nb_pas_dt_ % 10) != 0))
    {
      DoubleTab& passe = eq.inconnue().passe();
      double gamma=0.2;
      DoubleTab& passe2 = eq.inconnue().passe(2);
      DoubleTab& correcteur=passe;
      correcteur*=(1.-2.*gamma);
      correcteur.ajoute(gamma, present, VECT_REAL_ITEMS);
      correcteur.ajoute(gamma, passe2, VECT_REAL_ITEMS);
      futur*=(2*dt_);
      futur+=correcteur;
    }
  else
    {
      futur*=dt_;
      futur+=present;
    }

  eq.zone_Cl_dis().imposer_cond_lim(eq.inconnue(),temps_courant()+pas_de_temps());
  DoubleTab tmp(futur);
  tmp -= present;
  tmp /= dt_;
  update_critere_statio(tmp, eq);
  futur.echange_espace_virtuel();

  return 1;
}

