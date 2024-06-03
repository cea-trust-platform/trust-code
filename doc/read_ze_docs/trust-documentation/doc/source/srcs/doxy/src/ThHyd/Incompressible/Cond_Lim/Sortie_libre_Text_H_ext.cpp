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

#include <Sortie_libre_Text_H_ext.h>
#include <Equation_base.h>
#include <Motcle.h>

Implemente_instanciable(Sortie_libre_Text_H_ext, "Sortie_libre_Text_H_ext", Neumann);

Sortie& Sortie_libre_Text_H_ext::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

/*! @brief Type le_champ_front en "Champ_front_uniforme".
 *
 * Lit les valeurs du champ exterieur si les conditions
 *     aux limites sont specifiees: "T_ext", "C_ext", "Y_ext" ou "K_Eps_ext"
 *     Produit une erreur sinon.
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree& s) le flot d'entree modifie
 * @throws type de champ exterieur non reconnu,
 * les types reconnus sont: "T_ext", "C_ext", "Y_ext" ou "K_Eps_ext"
 */
Entree& Sortie_libre_Text_H_ext::readOn(Entree& s)
{
  if (app_domains.size() == 0)
    app_domains = {Motcle("Transport_Keps"), Motcle("Transport_Komega"),
                   Motcle("Thermique"), Motcle("Concentration"),
                   Motcle("indetermine")
                  };

  le_champ_front.typer("Champ_front_uniforme");
  le_champ_Text.typer("Champ_front_uniforme");
  le_champ_hext.typer("Champ_front_uniforme");
  Motcle motlu;
  Motcles les_motcles(6);
  {
    les_motcles[0] = "T_ext";
    les_motcles[1] = "C_ext";
    les_motcles[2] = "K_Eps_ext";
    les_motcles[3] = "H_ext";
    les_motcles[4] = "Y_ext";
    les_motcles[5] = "K_Omega_ext";
  }

  Motcle accfermee = "}";
  Motcle accouverte = "{";
  s >> motlu;
  int rang;
  if (motlu != accouverte)
    {
      Cerr << "On attendait une { dans la lecture de l'interface " << finl;
      exit();
    }
  s >> motlu;
  while (motlu != accfermee)
    {
      rang = les_motcles.search(motlu);
      switch(rang)
        {
        case 0:
          {
            s >> le_champ_Text;
            break;
          }
        case 1:
        case 2:
        case 3:
        case 4:
          {
            s >> le_champ_hext;
            break;
          }
        default:
          {
            Cerr << "Erreur a la lecture de la condition aux limites de type: " << finl;
            Cerr << que_suis_je() << finl;
            Cerr << "On attendait " << les_motcles << " a la place de " << motlu << finl;
            exit();
          }
        }
      s >> motlu;
    }

  return s;
}

/*! @brief Renvoie la valeur de la i-eme composante du champ impose a l'exterieur de la frontiere.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 * @throws deuxieme dimension du champ de frontiere superieur a 1
 */
double Sortie_libre_Text_H_ext::val_ext(int i) const
{
  if (type_cond_lim == 0)
    {
      //      Cerr<<"Sortie_libre_Text_H_ext Condition limite en temperature"<<finl;
      if (le_champ_Text.valeurs().size() == 1)
        return le_champ_Text(0, 0);
      else if (le_champ_Text.valeurs().dimension(1) == 1)
        return le_champ_Text(i, 0);
      else
        Cerr << "Sortie_libre_Text_H_ext::val_ext" << finl;
      exit();
      return 0.;
    }
  else
    {
      //      Cerr<<"Sortie_libre_Text_H_ext Condition limite en enthalpie"<<finl;
      if (le_champ_hext.valeurs().size() == 1)
        return le_champ_hext(0, 0);
      else if (le_champ_hext.valeurs().dimension(1) == 1)
        return le_champ_hext(i, 0);
      else
        Cerr << "Sortie_libre_Text_H_ext::val_ext" << finl;
      exit();
      return 0.;
    }
}

/*! @brief Renvoie la valeur de la (i,j)-eme composante du champ impose a l'exterieur de la frontiere.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @param (int j) indice suivant la deuxieme dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 */
double Sortie_libre_Text_H_ext::val_ext(int i, int j) const
{
  if (type_cond_lim == 0)
    {
      // Condition limite en temperature
      if (le_champ_Text.valeurs().dimension(0) == 1)
        return le_champ_Text(0, j);
      else
        return le_champ_Text(i, j);
    }
  else
    {
      // Condition limite en enthalpie
      if (le_champ_hext.valeurs().dimension(0) == 1)
        return le_champ_hext(0, j);
      else
        return le_champ_hext(i, j);
    }
}

/*! @brief Renvoie la valeur du flux impose sur la i-eme composante du champ representant le flux a la frontiere.
 *
 *     Le champ a la frontiere est considere constant sur tous
 *     les elements de la frontiere.
 *     La valeur du flux impose a la frontiere est egale
 *     a la valeur du champ (considere constant) a la frontiere
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 * @throws deuxieme dimension du champ de frontiere superieur a 1
 */
double Sortie_libre_Text_H_ext::flux_impose(int i) const
{
  if (type_cond_lim == 0)
    {
      if (le_champ_Text.valeurs().size() == 1)
        return le_champ_Text(0, 0);
      else if (le_champ_Text.valeurs().dimension(1) == 1)
        return le_champ_Text(i, 0);
      else
        Cerr << "Sortie_libre_Text_H_ext::flux_impose" << finl;
      exit();
      return 0.;
    }
  else
    {
      if (le_champ_hext.valeurs().size() == 1)
        return le_champ_hext(0, 0);
      else if (le_champ_hext.valeurs().dimension(1) == 1)
        return le_champ_hext(i, 0);
      else
        Cerr << "Sortie_libre_Text_H_ext::flux impose" << finl;
      exit();
      return 0.;
    }
}

/*! @brief Renvoie la valeur du flux impose sur la (i,j)-eme composante du champ representant le flux a la frontiere.
 *
 *     Le champ a la frontiere n'est PAS constant sur tous les elements
 *     la frontiere.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @param (int j) indice suivant la deuxieme dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 */
double Sortie_libre_Text_H_ext::flux_impose(int i, int j) const
{
  if (type_cond_lim == 0)
    {
      // Condition limite en temperature
      if (le_champ_Text.valeurs().dimension(0) == 1)
        return le_champ_Text(0, j);
      else
        return le_champ_Text(i, j);
    }
  else
    {
      // Condition limite en enthalpie
      if (le_champ_hext.valeurs().dimension(0) == 1)
        return le_champ_hext(0, j);
      else
        return le_champ_hext(i, j);
    }
}
