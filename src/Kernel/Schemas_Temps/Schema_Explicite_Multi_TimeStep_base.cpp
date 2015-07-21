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
// File:        Schema_Explicite_Multi_TimeStep_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_Explicite_Multi_TimeStep_base.h>
#include <Equation.h>
#include <Debog.h>
#include <DoubleTrav.h>

Implemente_base(Schema_Explicite_Multi_TimeStep_base,"Schema_Explicite_Multi_TimeStep_base",Schema_Temps_base);

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
// Postcondition:
Sortie& Schema_Explicite_Multi_TimeStep_base::printOn(Sortie& s) const
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
Entree& Schema_Explicite_Multi_TimeStep_base::readOn(Entree& s)
{
  return Schema_Temps_base::readOn(s) ;
}


////////////////////////////////
//                            //
// Caracteristiques du schema //
//                            //
////////////////////////////////


// Description:
//    Renvoie le nombre de valeurs temporelles futures.
//    Ici : n+1, donc 1.
int Schema_Explicite_Multi_TimeStep_base::nb_valeurs_futures() const
{
  return 1 ;
}

// Description:
//    Renvoie le le temps a la i-eme valeur future.
//    Ici : t(n+1)
double Schema_Explicite_Multi_TimeStep_base::temps_futur(int i) const
{
  assert(i==1);
  return temps_courant()+pas_de_temps();
}

// Description:
//    Renvoie le le temps le temps que doivent rendre les champs a
//    l'appel de valeurs()
//    Ici : t(n+1)
double Schema_Explicite_Multi_TimeStep_base::temps_defaut() const
{
  return temps_courant()+pas_de_temps();
}

/////////////////////////////////////////
//                                     //
// Fin des caracteristiques du schema  //
//                                     //
/////////////////////////////////////////

// Description:
//    Effectue un pas de temps d'Euler explicite
//    sur l'equation passee en parametre.
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
int Schema_Explicite_Multi_TimeStep_base::faire_un_pas_de_temps_eqn_base(Equation_base& eqn)
{
  authorized_equation(eqn);

  double time_step             = pas_de_temps(); //tn+1-tn
  double inv_time_step         = 1/time_step;

  //  Un
  const DoubleTab& present = eqn.inconnue().valeurs();
  Debog::verifier("Schema_Explicite_Multi_TimeStep_base::faire_un_pas_de_temps_eqn_base, present",present);

  // Un+1
  DoubleTab& futur   = eqn.inconnue().futur();

  // sert pour la pression et les couplages
  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(),temps_courant()+pas_de_temps());

  DoubleTab dudt(futur);
  /*
    if (nb_pas_dt() > nb_pas_dt_seuil())
      {
        Cout <<"Use of "<<que_suis_je()<<" scheme"<<finl;
      }
    else
      {
        Cout <<"Use of explicit Euler scheme for time step "<<nb_pas_dt()<<" in "<<que_suis_je()<<" scheme"<<finl;
      } */

  // Compute du/dt
  eqn.derivee_en_temps_inco(dudt);

  //Contribution de l'inconnue au temps n
  futur = dudt;
  futur *= time_step;
  futur += present;

  dudt=futur;
  dudt-=present;
  dudt*=inv_time_step;

  Debog::verifier("Schema_Explicite_Multi_TimeStep_base::faire_un_pas_de_temps_eqn_base, dudt",dudt);

  eqn.zone_Cl_dis().imposer_cond_lim(eqn.inconnue(),temps_courant()+pas_de_temps());
  update_critere_statio(dudt, eqn);
  futur.echange_espace_virtuel();

  //Debog::verifier("Schema_Explicite_Multi_TimeStep_base::faire_un_pas_de_temps_eqn_base, futur fin",futur);
  return 1;
}

void Schema_Explicite_Multi_TimeStep_base::authorized_equation(const Equation_base& eqn)
{
  Nom  equation_name(eqn.que_suis_je());
  bool authorized =  true;

  authorized &= (bool) equation_name.find("FT");
  authorized &= (bool) equation_name.find("Front_Tracking");
  authorized &= (bool) equation_name.find("QC");
  authorized &= (bool) equation_name.find("Quasi_Compressible");

  if (!authorized)
    {
      Cerr<<"Error in "<<que_suis_je()<<"::authorized_equation()"<<finl;
      Cerr<<"Equation "<<equation_name<<" is not allowed with "<<que_suis_je()<<" time scheme"<<finl;
      Process::exit();
    }
}

void Schema_Explicite_Multi_TimeStep_base::authorized_equation(const Equation_base& eqn) const
{
  Nom  equation_name(eqn.que_suis_je());
  bool authorized =  true;

  authorized &= (bool) equation_name.find("FT");
  authorized &= (bool) equation_name.find("Front_Tracking");
  authorized &= (bool) equation_name.find("QC");
  authorized &= (bool) equation_name.find("Quasi_Compressible");

  if (!authorized)
    {
      Cerr<<"Error in "<<que_suis_je()<<"::authorized_equation()"<<finl;
      Cerr<<"Equation "<<equation_name<<" is not allowed with "<<que_suis_je()<<" time scheme"<<finl;
      Process::exit();
    }
}
