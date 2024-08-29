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

#include <Champ_Don_base.h>

Implemente_base(Champ_Don_base,"Champ_Don_base",Champ_base);
// XD champ_don_base field_base champ_don_base -1 Basic class for data fields (not calculated), p.e. physics properties.


Sortie& Champ_Don_base::printOn(Sortie& os) const { return Champ_base::printOn(os); }
Entree& Champ_Don_base::readOn(Entree& is) { return Champ_base::readOn(is); }

/*! @brief Fixe le nombre de degres de liberte par composante
 *
 * @param (int nb_noeuds) le nombre de degre de liberte par composante
 */
int Champ_Don_base::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  valeurs_.resize(nb_noeuds, nb_compo_);
  return nb_noeuds;
}

/*! @brief Provoque une erreur !  A surcharger par les classes derivees ! non virtuelle pure par commoditees de developpement !
 *
 */
Champ_base& Champ_Don_base::affecter_(const Champ_base&)
{
  Cerr << "Champ_Don_base::affecter_ : " << que_suis_je() << "::affecter_ must be overloaded in derived classes" << finl;
  throw;
}

/*! @brief Provoque une erreur ! A surcharger par les classes derivees ! non virtuelle pure par commoditees de developpement !
 *
 */
Champ_base& Champ_Don_base::affecter_compo(const Champ_base&, int)
{
  Cerr << "Champ_Don_base::affecter_compo must be overloaded in derived classes" << finl;
  throw;
}

/*! @brief Mise a jour en temps.
 *
 * @param (double) le temps de mise a jour
 */
void Champ_Don_base::mettre_a_jour(double t)
{
  changer_temps(t);
  valeurs_.echange_espace_virtuel();
}

/*!
 * See comments in Probleme_base_interface_proto::resetTime_impl()
 * Here we update.
 */
void Champ_Don_base::resetTime(double time)
{
  mettre_a_jour(time);
}

/*! @brief NE FAIT RIEN.
 *
 * A surcharger dans les classes derivees. Provoque l'initialisation du champ si necessaire
 *
 */
int Champ_Don_base::initialiser(const double un_temps)
{
  mettre_a_jour(un_temps);
  return 1;
}

/*! @brief NE FAIT RIEN.
 *
 * A surcharger dans les classes derivees
 *
 */
int Champ_Don_base::reprendre(Entree& )
{
  return 1;
}

/*! @brief NE FAIT RIEN.
 *
 * A surcharger dans les classes derivees
 *
 */
int Champ_Don_base::sauvegarder(Sortie& ) const
{
  return 1;
}

/*! @brief NE FAIT RIEN.
 *
 * EXIT ! A surcharger dans les classes derivees
 *
 */
int Champ_Don_base::imprime(Sortie& os, int ncomp) const
{
  Cerr << que_suis_je() << "::imprime not coded." << finl;
  Process::exit();
  return 1;
}

/*! @brief Fixe le nombre de composantes et le nombre de valeurs nodales.
 *
 * @param (int) le nombre de noeud par composante du champ (le nombre de dl par composante)
 * @param (int) le nombre de composante du champ
 */
void Champ_Don_base::dimensionner(int nb_noeuds, int nb_compo)
{
  fixer_nb_comp(nb_compo);
  fixer_nb_valeurs_nodales(nb_noeuds);
}
