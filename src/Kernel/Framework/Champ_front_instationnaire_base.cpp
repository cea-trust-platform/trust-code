/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Champ_front_instationnaire_base.h>

Implemente_base(Champ_front_instationnaire_base,"Champ_front_instationnaire_base",Champ_front_base);

/*! @brief Lit le temps du champ instationnaire sur un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Champ_front_instationnaire_base::readOn(Entree& is)
{
  return is;
}

/*! @brief Ecrit le temps du champ instationnaire sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Champ_front_instationnaire_base::printOn(Sortie& os) const
{
  return os;
}

int Champ_front_instationnaire_base::initialiser(double temps, const Champ_Inc_base& inco)
{
  if (!Champ_front_base::initialiser(temps,inco))
    return 0;

  temps_defaut=temps;
  changer_temps_futur(temps,0);

  for(int i=0; i<les_valeurs->nb_cases(); i++)
    {
      les_valeurs[i].valeurs().resize(1,nb_comp());
    }
  set_instationnaire();
  return 1;
}


/*! @brief Surcharge Champ_front_base::fixer_nb_valeurs_temporelles
 *
 */
void Champ_front_instationnaire_base::fixer_nb_valeurs_temporelles(int nb_cases)
{
  les_valeurs->fixer_nb_cases(nb_cases);
}

inline void print(const Roue_ptr& les_valeurs)
{
  Cerr << "The times available are :" << finl;
  for(int i=1; i<les_valeurs->nb_cases(); i++)
    Cerr << "  " << les_valeurs[i].temps() << finl;
}

/*! @brief Renvoie les valeurs au temps desire.
 *
 * Sinon, sort en erreur.
 *
 */
DoubleTab& Champ_front_instationnaire_base::valeurs_au_temps(double temps)
{

  for(int i=0; i<les_valeurs->nb_cases(); i++)
    {
      if(est_egal(les_valeurs[i].temps(),temps))
        return les_valeurs[i].valeurs();
    }

  Cerr << "In Champ_front_instationnaire_base::valeurs(double), "
       << "time " << temps << " not found." << finl;
  print(les_valeurs);
  exit();
  return valeurs(); // For compilers
}

/*! @brief Renvoie les valeurs au temps desire.
 *
 * Sinon, sort en erreur.
 *
 */
const DoubleTab& Champ_front_instationnaire_base::valeurs_au_temps(double temps) const
{

  for(int i=0; i<les_valeurs->nb_cases(); i++)
    {
      if(est_egal(les_valeurs[i].temps(),temps))
        return les_valeurs[i].valeurs();
    }

  Cerr << "In Champ_front_instationnaire_base::valeurs(double), "
       << "time " << temps << " not found." << finl;
  print(les_valeurs);
  exit();
  return valeurs(); // For compilers
}

/*! @brief Avance jusqu'au temps desire.
 *
 * Sinon, sort en erreur.
 *
 */
int Champ_front_instationnaire_base::avancer(double temps)
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
  Cerr << "In Champ_front_instationnaire_base::avancer(double), "
       << "time " << temps << " not found." << finl;
  print(les_valeurs);
  exit();
  return 0;
}

/*! @brief Recule jusqu'au temps desire.
 *
 * Sinon, sort en erreur.
 *
 */
int Champ_front_instationnaire_base::reculer(double temps)
{

  // S"agit-il bien d'une avance en temps ?
  assert(temps<les_valeurs->temps());

  for(int i=les_valeurs->nb_cases()-1; i>=1; i++)
    {
      if(est_egal(les_valeurs[i].temps(),temps))
        {
          les_valeurs=les_valeurs[i];
          return 1;
        }
    }
  Cerr << "In Champ_front_instationnaire_base::reculer(double), "
       << "time " << temps << " not found." << finl;
  print(les_valeurs);
  exit();
  return 0;
}

void Champ_front_instationnaire_base::valeurs_face(int face,DoubleVect& var) const
{
  var.resize(nb_compo_);
  int i;
  for (i=0 ; i<nb_compo_ ; i++)
    var(i) = valeurs()(0,i);
}
