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

#include <Champ_Tabule_Temps.h>

Implemente_instanciable(Champ_Tabule_Temps,"Champ_Tabule_Temps",Champ_Uniforme_inst);


/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
Sortie& Champ_Tabule_Temps::printOn(Sortie& os) const
{
  return os;
}


/*! @brief Lit les valeurs du champ tabule en temps a partir d'un flot d'entree.
 *
 *     On lit le nombre de composante du champ (nb_comp) et on
 *     remplit une table a partir des nval couples (vrel_i, tps_i).
 *     Format:
 *      Champ_Tabule_Temps nb_comp { nval tps_1...tps_nval  vrel_1...vrel_nval }
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 * @throws accolade fermante attendue
 */
Entree& Champ_Tabule_Temps::readOn(Entree& is)
{
  Motcle motlu;
  int nb_val;
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  nb_compo_=lire_dimension(is,que_suis_je());
  if (nb_compo_ <= 0)
    {
      Cerr << "Error in Champ_Tabule_Temps::readOn : nb_compo <= 0" << finl;
      assert(0);
      exit();
    }
  dimensionner(1, nb_compo_);
  is >> motlu;
  if (motlu == accolade_ouverte)
    {
      nb_val=lire_dimension(is,que_suis_je());
      DoubleVect param(nb_val);
      DoubleTab les_valeurs(nb_val, nb_compo_);
      for (int i = 0; i < nb_val; i++)
        is >> param[i];
      for (int i = 0; i < nb_val; i++)
        for (int k = 0; k < nb_compo_; k++)
          is >> les_valeurs(i, k);
      la_table.remplir(param, les_valeurs);

      is >> motlu;
      if (motlu != accolade_fermee)
        {
          Cerr << "Error reading from an object of type Champ_Tabule_Temps" << finl;
          Cerr << "We expected keyword } instead of " << motlu << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Error reading from an object of type Champ_Tabule_Temps" << finl;
      Cerr << "We expected keyword { instead of " << motlu << finl;
      exit();
    }
  changer_temps(-1e9);
  //mettre_a_jour(0.);
  return is;
}


/*! @brief Mise a jour en temps des valeurs tabulees du champ.
 *
 * @param (double tps) le temps de mise a jour
 */
void Champ_Tabule_Temps::me_calculer(double tps)
{
  if (temps() != tps)
    {
      DoubleVect& mes_valeurs = valeurs();
      if (mes_valeurs.size() == 1)
        mes_valeurs[0] = la_table.val(tps);
      else
        la_table.valeurs(mes_valeurs,tps);
    }
}
