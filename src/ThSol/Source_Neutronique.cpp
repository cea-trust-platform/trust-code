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
// File:        Source_Neutronique.cpp
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Neutronique.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Champ_Uniforme.h>
#include <Equation_base.h>
#include <Schema_Temps_base.h>
#include <EChaine.h>
#include <Motcle.h>
#include <SFichier.h>

Implemente_base(Source_Neutronique,"Source_Neutronique",Source_base);

//// printOn
//

Sortie& Source_Neutronique::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//
Entree& Source_Neutronique::readOn(Entree& is )
{
  Motcles les_mots(11);
  {
    les_mots[0] = "N";
    les_mots[1] = "lambda";
    les_mots[2] = "Tvie";
    les_mots[3] = "beta";
    les_mots[4] = "rho_cte";
    les_mots[5] = "P0";
    les_mots[6] = "dt_impr";
    les_mots[7] = "schema_temps";
    les_mots[8] = "rho";
    les_mots[9] = "repartition";
    les_mots[10] = "Ci0";
  }

  EChaine ec("Champ_Uniforme 1 1");
  ec >> la_puissance;
  Ci0_ok=0;
  f_xyz = "1.";
  n_ssz="defaut";
  N = -1;
  init = 1;
  dt_impr = 1e10;
  faire_un_pas_de_temps =  &Source_Neutronique::faire_un_pas_de_temps_RK;
  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  is >> motlu;
  if(motlu!=accolade_ouverte)
    {
      Cerr << "On attendait une { a la lecture d'une " << que_suis_je() << finl;
      Cerr << "et non : " << motlu << finl;
      exit();
    }
  is >> motlu;
  while (motlu != accolade_fermee)
    {

      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            is >> N;
            if (N<1)
              {
                Cerr << "Le nombre de groupes doit etre >=1 dans  " << que_suis_je() << finl;
                exit();
              }
            break;
          }
        case 1 :
          {
            if (N<1)
              {
                Cerr << "Il faut rentrer le nb de groupes N avant lambda dans  " << que_suis_je() << finl;
                exit();
              }
            lambda.resize(N);
            for (int i=0; i<N; i++) is>>lambda(i);
            break;
          }
        case 2 :
          {
            is >> Tvie;
            break;
          }
        case 3 :
          {
            if (N<1)
              {
                Cerr << "Il faut rentrer le nb de groupes N avant beta dans  " << que_suis_je() << finl;
                exit();

              }
            beta.resize(N);
            beta_som=0.;
            for (int i=0; i<N; i++)
              {
                is>>beta(i);
                beta_som+=beta(i);
              }
            break;
          }
        case 5 :
          {
            is >> P0;
            break;
          }
        case 6 :
          {
            is >> dt_impr;
            break;
          }
        case 7 :
          {
            Motcle nom_sch;
            is >> nom_sch;
            if (nom_sch == "euler_explicite")
              {
                faire_un_pas_de_temps =  &Source_Neutronique::faire_un_pas_de_temps_EE;
              }
            else if (nom_sch == "runge_kutta")
              {
                faire_un_pas_de_temps =  &Source_Neutronique::faire_un_pas_de_temps_RK;
              }
            else
              {
                Cerr << "On ne comprend le mot : " << motlu << "dans " << que_suis_je() << finl;
                exit();
              }

            break;
          }
        case 8 :
          {
            Nom tmp;
            is >> tmp;
            Cerr << "Lecture de la reactivite " << tmp << finl;
            fct_tT.setNbVar(2);
            fct_tT.setString(tmp);
            fct_tT.addVar("t");
            fct_tT.addVar("T");
            fct_tT.parseString();

            break;
          }
        case 9 :
          {
            is >> f_xyz;
            is >> n_ssz;
            break;
          }
        case 10 :
          {
            if (N<1)
              {
                Cerr << "Il faut rentrer le nb de groupes N avant les Ci initiaux dans  " << que_suis_je() << finl;
                exit();
              }
            Ci0_ok = 1;
            Ci0.resize(N);
            for (int i=0; i<N; i++)
              {
                is>>Ci0(i);
              }
            break;
          }
        default :
          {
            Cerr << "On ne comprend le mot : " << motlu << "dans " << que_suis_je() << finl;
            exit();
          }
        }
      is >> motlu;
    }

  if (N<1)
    {
      Cerr << "Il faut rentrer le nb de groupes N  dans  " << que_suis_je() << finl;
      exit();

    }

  return is;
}

double Source_Neutronique::rho(double t, double T)
{
  fct_tT.setVar("t",t);
  fct_tT.setVar("T",T);
  return fct_tT.eval();
}


/**
 * Renvoie la chaine de caracere representant la fonction de repartition f(x,y,z) de la puissance
 */
const Nom& Source_Neutronique::repartition() const
{
  return f_xyz;
}

/**
 * Renvoie le nom de la sous zone de degagement de puissance
 */
const Nom& Source_Neutronique::nom_ssz() const
{
  return n_ssz;
}


void Source_Neutronique::completer()
{

  Source_base::completer();


  Un.resize(N+1);
  Unp1.resize(N+1);
  matA.resize(N+1,N+1);
  matA = 0.;
  for (int i=1; i<N+1; i++)
    {
      matA(0,i) = lambda(i-1);
      matA(i,i) = -lambda(i-1);
      matA(i,0) = beta(i-1)/Tvie;
    }
  Un(0) = P0;
  if (Ci0_ok == 0)
    for (int i=1; i<N+1; i++)
      Un(i) = beta(i-1)*Un(0)/Tvie/lambda(i-1);
  else
    for (int i=1; i<N+1; i++)
      Un(i) = Ci0(i-1);
}


/**
 * Effectue resu = m*v
 */void Source_Neutronique::mul(DoubleTab& m, DoubleVect& v, DoubleVect& resu)
{
  resu = 0.;
  resu(0) = m(0,0)*v(0);
  for (int i=1; i<N+1; i++)
    {
      resu(0)+= m(0,i)*v(i);
      resu(i) = m(i,0)*v(0) + m(i,i)*v(i);
    }
}


/**
 * Schema de Runge Kutta classique
 */
void Source_Neutronique::faire_un_pas_de_temps_RK()
{
  static const double a1 = 1./6.;
  static const double a2 = 1./3.;

  DoubleVect fn1(Un);
  DoubleVect fn2(Un);
  DoubleVect fn3(Un);
  DoubleVect fn4(Un);
  const double dt2 = dt/2.;

  // Un1 = Un+dt/2*f(tn,un)
  mul(matA,Un, fn1);
  Unp1 = Un;
  Unp1.ajoute(dt2, fn1);

  // Un2 = Un+dt/2*f(tn+dt/2,Un1)
  mettre_a_jour_matA(temps_courant+dt2);

  mul(matA,Unp1, fn2);
  Unp1 = Un;
  Unp1.ajoute(dt2, fn2);

  // Un3 = Un+dt*f(tn+dt/2,Un2)
  mul(matA,Unp1, fn3);
  Unp1 = Un;
  Unp1.ajoute(dt, fn3);

  // Un+1 = Un+dt*(a1*f(tn,un) + a2*f(tn+dt/2,Un1) + a2*f(tn+dt/2,Un2) + a1*f(tn+dt/2,Un3))
  mettre_a_jour_matA(temps_courant+dt);
  mul(matA,Unp1, fn4);
  Unp1 = Un;
  Unp1.ajoute(a1*dt, fn4);
  Unp1.ajoute(a2*dt, fn3);
  Unp1.ajoute(a2*dt, fn2);
  Unp1.ajoute(a1*dt, fn1);
}


/**
 * Met a jour les coefficients de la matrice A
 */
void Source_Neutronique::mettre_a_jour_matA(double temps)
{
  matA(0,0) = beta_som/Tvie*(rho(temps,Tmoy)-1);
}



/**
 * Schema Euler explicite
 */
void Source_Neutronique::faire_un_pas_de_temps_EE()
{
  mul(matA,Un, Unp1);
  Unp1*=dt;
  Unp1 += Un;
  /* Cout << "matA " << matA << finl;
     Cout << "Un " << Un << finl;
     Cout << "Unp1 " << Unp1 << finl;
  */
}

/**
 * Aller_au_temps : permet d'avancer avec le pas de temps de stabilite propre de l'equation.
 */
void Source_Neutronique::aller_au_temps(double temps)
{
  while (temps_courant<temps)
    {
      const double dt_stab = 0.8*Tvie/beta_som*max(fabs(rho(temps_courant,Tmoy)-1),1.);
      dt = min(dt_stab, temps-temps_courant);
      (this->*faire_un_pas_de_temps)();
      Un = Unp1;
      temps_courant += dt;
      mettre_a_jour_matA(temps_courant);
      if (limpr(temps_courant,dt)) imprimer(temps_courant);
    }

}

/**
 * Imprime les resultats dans le fichier puissance.dat
 */
void Source_Neutronique::imprimer(double temps) const
{

  SFichier fic("puissances.dat",ios::app);
  fic << temps ;
  for (int i =0; i<N+1; i++)
    {
      fic << " " << Un(i);
    }
  fic << finl;

  /*Cout << "Impression du pas de temps pour l'equation de la cinetique point " << finl;
    Cout << ">>>> Pas de temps utilise dt = " << dt << finl;
    Cout << ">>>> Pas de temps de stabilite dt_stab = " << dt_stab << finl;
  */
}

int Source_Neutronique::limpr(double le_temps_courant,double ddt) const
{
  static const double epsilon = 1.e-9;
  const Schema_Temps_base& sch = mon_equation->schema_temps();
  // Si impression a chaque pas de temps, alors que pas demande, faire comme limpr() de Echange_contact_Correlation_VDF.cpp
  if (sch.nb_pas_dt()==0)
    return 0;
  if (dt_impr<=ddt || ((sch.temps_final_atteint() || sch.nb_pas_dt_max_atteint() || sch.nb_pas_dt()<=1 || sch.stationnaire_atteint()) && dt_impr!=1e10))
    return 1;
  else
    {
      // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
      double i, j;
      modf(le_temps_courant/dt_impr + epsilon, &i);
      modf((le_temps_courant-ddt)/dt_impr + epsilon, &j);
      return ( i>j );
    }
}


void Source_Neutronique::mettre_a_jour(double temps)
{

  if (init)
    {
      init = 0;
      Tmoy = calculer_Tmoyenne();
      temps_courant = temps;
      mettre_a_jour_matA(temps);
      la_puissance(0) = Un(0);
    }
  else
    {
      Tmoy = calculer_Tmoyenne();
      aller_au_temps(temps);
      temps_courant = temps;
      la_puissance(0) = Un(0);
    }


}

