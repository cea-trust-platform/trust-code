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

#include <Champ_front_var.h>
#include <Domaine_VF.h>

Implemente_base(Champ_front_var,"Champ_front_var",Champ_front_base);


/*! @brief Imprime le nom du champ sur un flot de sortie
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Champ_front_var::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


/*! @brief NE FAIT RIEN A surcharger dans les classes derivees
 *
 *     Doit dans tous les cas fixer nb_comp
 *
 * @param (Entree& is)
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Champ_front_var::readOn(Entree& s )
{
  fixer_nb_comp(1);
  return s ;
}


/*! @brief Initialisation en debut de calcul.
 *
 * Dimensionne le tableau de valeurs et cree son espace virtuel.
 *     Les classes derivees doivent imperativement appeler cette methode.
 *
 * @return (0 en cas d'erreur, 1 sinon.)
 */
int Champ_front_var::initialiser(double temps, const Champ_Inc_base& inco)
{
  const Frontiere& frontiere = frontiere_dis().frontiere();
  const int n = les_valeurs->nb_cases();
  const int nbc = nb_comp();
  for(int i = 0; i < n; i++)
    {
      DoubleTab& tab = les_valeurs[i].valeurs();
      // B.M. la methode completer dimensionne parfois le tableau.
      // On a un peut tout et n'importe quoi en entree: parfois tableau vide,
      // parfois nb_dim==1, etc...
      // Attention, les valeurs existantes doivent etre conservees s'il y en a !
      if (tab.nb_dim()!=2 || tab.dimension(1)!=nbc)
        {
          tab.resize(tab.dimension(0), nbc);
          frontiere.creer_tableau_faces(tab);
        }
      tab.echange_espace_virtuel();
    }
  return 1;
}
