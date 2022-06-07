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

#include <Champ_front_var_instationnaire.h>

Implemente_base(Champ_front_var_instationnaire,"Champ_front_var_instationnaire",Champ_front_var);

// Description:
//    Imprime le nom du champ sur un flot de sortie
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_front_var_instationnaire::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Champ_front_var_instationnaire::readOn(Entree& s )
{
  return s ;
}


// Description:
//      Surcharge Champ_front_base::fixer_nb_valeurs_temporelles
void Champ_front_var_instationnaire::fixer_nb_valeurs_temporelles(int nb_cases)
{
  les_valeurs->fixer_nb_cases(nb_cases);
}

// Description:
//    Initialise le temps courant et Gpoint
// Precondition:
// Parametre: double tps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_front_var_instationnaire::initialiser(double temps, const Champ_Inc_base& inco)
{
  if (!Champ_front_var::initialiser(temps,inco))
    return 0;

  temps_defaut=temps;
  changer_temps_futur(temps,0);

  Gpoint_=valeurs(); // pour dimensionner
  Gpoint_=0.;

  return 1;
}

inline void print(const Roue_ptr& les_valeurs)
{
  Cerr << "The times available are :" << finl;
  for(int i=1; i<les_valeurs->nb_cases(); i++)
    Cerr << "  " << les_valeurs[i].temps() << finl;
}

// Description:
//    Renvoie les valeurs au temps desire.
//    Sinon, sort en erreur.
DoubleTab& Champ_front_var_instationnaire::valeurs_au_temps(double temps)
{

  for(int i=0; i<les_valeurs->nb_cases(); i++)
    {
      if(est_egal(les_valeurs[i].temps(),temps))
        return les_valeurs[i].valeurs();
    }

  Cerr << "In Champ_front_var_instationnaire::valeurs(double), "
       << "time " << temps << " not found." << finl;
  print(les_valeurs);
  exit();
  return valeurs(); // For compilers
}

// Description:
//    Renvoie les valeurs au temps desire.
//    Sinon, sort en erreur.
const DoubleTab& Champ_front_var_instationnaire::valeurs_au_temps(double temps) const
{

  for(int i=0; i<les_valeurs->nb_cases(); i++)
    {
      if(est_egal(les_valeurs[i].temps(),temps))
        return les_valeurs[i].valeurs();
    }

  Cerr << "In Champ_front_var_instationnaire::valeurs(double), "
       << "time " << temps << " not found." << finl;
  print(les_valeurs);
  exit();
  return valeurs(); // For compilers
}

// Description:
//    Avance jusqu'au temps desire.
//    Sinon, sort en erreur.
int Champ_front_var_instationnaire::avancer(double temps)
{

  // S"agit-il bien d'une avance en temps ?
  assert(temps>les_valeurs->temps());

  for(int i=1; i<les_valeurs->nb_cases(); i++)
    {
      if(est_egal(les_valeurs[i].temps(),temps))
        {
          les_valeurs=les_valeurs[i];
          return 1;
        }
    }
  Cerr << "In Champ_front_var_instationnaire::avancer(double), "
       << "time " << temps << " not found." << finl;
  print(les_valeurs);
  exit();
  return 0;
}

// Description:
//    Recule jusqu'au temps desire.
//    Sinon, sort en erreur.
int Champ_front_var_instationnaire::reculer(double temps)
{

  // S"agit-il bien d'un recul en temps ?
  assert(temps<les_valeurs->temps());

  for(int i=les_valeurs->nb_cases()-1; i>=1; i++)
    {
      if(est_egal(les_valeurs[i].temps(),temps))
        {
          les_valeurs=les_valeurs[i];
          return 1;
        }
    }
  Cerr << "In Champ_front_var_instationnaire::reculer(double), "
       << "time " << temps << " not found." << finl;
  print(les_valeurs);
  exit();
  return 0;
}

// Description :
//   Calcule le taux d'accroissement du champ entre t1 et t2
//   et le stocke dans Gpoint_
void Champ_front_var_instationnaire::Gpoint(double t1, double t2)
{
  Gpoint_=valeurs_au_temps(t1);
  if (t2>t1+DMINFLOAT)
    {
      Gpoint_ *= -1;
      Gpoint_ += valeurs_au_temps(t2);
      Gpoint_ /= (t2-t1);
    }
  else
    Gpoint_=0;
  //Gpoint_.echange_espace_virtuel(); PL inutile
}
