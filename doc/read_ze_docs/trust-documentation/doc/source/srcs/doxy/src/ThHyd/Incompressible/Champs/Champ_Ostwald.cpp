/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Champ_Ostwald.h>
#include <Domaine_dis_base.h>

Implemente_instanciable(Champ_Ostwald,"Champ_Ostwald",Champ_Fonc_P0_base);

Sortie& Champ_Ostwald::printOn(Sortie& os) const { return os; }

Entree& Champ_Ostwald::readOn(Entree& is) { return is; }

void Champ_Ostwald::mettre_a_jour(double)
{
  Cerr << "Champ_Ostwald::mettre_a_jour() ne fait rien" << finl;
  Cerr << que_suis_je() << "doit la surcharger !" << finl;
  Process::exit();
}

int Champ_Ostwald::initialiser(const double un_temps)
{
  Cerr << "Champ_Ostwald::initialiser(temps) must be overloaded" << finl;
  Cerr << " by " << que_suis_je() << finl;
  Process::exit();
  return 0;
}

void Champ_Ostwald::me_calculer(double tps)
{
  Cerr << "Champ_Ostwald::me_calculer() ne fait rien" << finl;
  Cerr << que_suis_je() << "doit la surcharger !" << finl;
  Process::exit();
}

/*! @brief Fixe le nombre de degres de liberte par composante
 *
 * @param (int nb_noeuds) le nombre de degre de liberte par composante
 * @return (int) le nombre de degres de liberte par composante
 */
int Champ_Ostwald::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  // Note B.M.: encore un heritage a la noix qui m'empeche de factoriser
  // en utilisant creer_tableau_distribue:
  const Champ_Don_base& cdb = *this;
  const Domaine& domaine = cdb.domaine_dis_base().domaine();

  assert(nb_noeuds == domaine.nb_elem());

  if (nb_compo_ == 1)
    valeurs_.resize(0);
  else
    valeurs_.resize(0, nb_compo_);
  domaine.creer_tableau_elements(valeurs_);
  return nb_noeuds;
}

Champ_base& Champ_Ostwald::affecter_(const Champ_base& ch)
{
  DoubleTab noeuds;
  IntVect polys;
  if (!remplir_coord_noeuds_et_polys(noeuds, polys))
    {
      remplir_coord_noeuds(noeuds);
      ch.valeur_aux(noeuds, valeurs());
    }
  else
    ch.valeur_aux_elems(noeuds, polys, valeurs());
  return *this;
}
