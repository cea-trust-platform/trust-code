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

#include <Domaine_Cl_dis_base.h>
#include <Equation_base.h>
#include <Dirichlet.h>

Implemente_base(Dirichlet, "Dirichlet", Cond_lim_base);
// XD dirichlet condlim_base dirichlet -1 Dirichlet condition at the boundary called bord (edge) : 1). For Navier-Stokes equations, velocity imposed at the boundary; 2). For scalar transport equation, scalar imposed at the boundary.

Sortie& Dirichlet::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Dirichlet::readOn(Entree& s) { return Cond_lim_base::readOn(s); }

/*! @brief Renvoie la valeur imposee sur la i-eme composante du champ a la frontiere au temps par defaut du champ_front.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 * @throws deuxieme dimension du champ de frontiere superieur a 1
 */
double Dirichlet::val_imp(int i) const
{
  return val_imp_au_temps(le_champ_front->get_temps_defaut(), i);
}

/*! @brief Renvoie la valeur imposee sur la (i,j)-eme composante du champ a la frontiere au temps par defaut du champ_front.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @param (int j) indice suivant la deuxieme dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 */
double Dirichlet::val_imp(int i, int j) const
{
  return val_imp_au_temps(le_champ_front->get_temps_defaut(), i, j);
}

/*! @brief Renvoie la valeur imposee sur la i-eme composante du champ a la frontiere au temps precise.
 *
 */
double Dirichlet::val_imp_au_temps(double temps, int i) const
{
  const DoubleTab& tab = le_champ_front->valeurs_au_temps(temps);
  if (tab.size() == 1)
    return tab(0, 0);
  if (tab.line_size() == 1)
    return tab(i, 0);
  else
    Cerr << "Dirichlet::val_imp error" << finl;
  exit();
  return 0.;
}

/**
 * @brief Update the tab_ variable based on the given time.
 *
 * This function updates the tab_ variable by assigning the values from the val_imp_au_temps function to it.
 * If the size of tab_ is 0, it is resized to the total number of faces on the boundary. Then, the values are
 * assigned to tab_ using the val_imp_au_temps function.
 *
 * @param temps The time at which the update is performed.
 */
/*
void Dirichlet::mettre_a_jour(double temps)
{
  // ToDo Kokkos: not used
  Cond_lim_base::mettre_a_jour(temps);
  // Mise a jour de tab_
  int nb_faces_tot = frontiere_dis().frontiere().nb_faces() + frontiere_dis().frontiere().nb_faces_virt();
  int nb_comp = le_champ_front.nb_comp();
  if (tab_.size()==0)
    tab_.resize(nb_faces_tot,nb_comp);
  for (int face=0; face<nb_faces_tot; face++)
    for (int comp=0; comp<nb_comp; comp++)
      tab_(face, comp) = val_imp(face, comp);
  tab_view_ = tab_.view_ro();
}*/

/*! @brief Renvoie la valeur imposee sur la (i,j)-eme composante du champ a la frontiere au temps precise.
 *
 */
double Dirichlet::val_imp_au_temps(double temps, int i, int j) const
{
  const DoubleTab& tab = le_champ_front->valeurs_au_temps(temps);
  if (tab.dimension(0) == 1)
    return tab(0, j);
  else
    return tab(i, j);
}

void Dirichlet::verifie_ch_init_nb_comp() const
{
  if (le_champ_front.non_nul())
    {
      const Equation_base& eq = domaine_Cl_dis().equation();
      const int nb_comp = le_champ_front->nb_comp();
      eq.verifie_ch_init_nb_comp_cl(eq.inconnue(), nb_comp, *this);
    }
}
