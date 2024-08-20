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

#include <Echange_impose_base.h>
#include <Domaine_Cl_dis_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Milieu_base.h>

Implemente_base_sans_constructeur(Echange_impose_base, "Echange_impose_base", Cond_lim_base);

Sortie& Echange_impose_base::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Echange_impose_base::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Thermique"), Motcle("Neutronique"), Motcle("fraction_massique"), Motcle("indetermine") };
  if (supp_discs.size() == 0) supp_discs = { Nom("VDF"), Nom("VEFPreP1B"), Nom("PolyMAC"), Nom("PolyMAC_P0P1NC"), Nom("PolyMAC_P0") };

  Motcle motlu;
  Motcles les_motcles(2);
  {
    les_motcles[0] = "h_imp";
    les_motcles[1] = "T_ext";
  }

  int ind = 0;
  while (ind < 2)
    {
      s >> motlu;
      int rang = les_motcles.search(motlu);

      switch(rang)
        {
        case 0:
          {
            s >> h_imp_;
            break;
          }
        case 1:
          {
            s >> T_ext();
            break;
          }
        default:
          {
            Cerr << "Error while reading BC of type Echange_impose " << finl;
            Cerr << "we expected a keyword among: " << les_motcles << " instead of " << motlu << finl;
            exit();
          }
        }

      ind++;

    }

  return s;
}

/*! @brief Renvoie la valeur de la temperature imposee sur la i-eme composante du champ de frontiere.
 *
 * @param (int i) l'indice de la composante du champ de de frontiere
 * @return (double)
 */
double Echange_impose_base::T_ext(int i) const
{
  if (T_ext()->valeurs().size() == 1)
    return T_ext()->valeurs()(0, 0);
  else if (T_ext()->valeurs().dimension(1) == 1)
    return T_ext()->valeurs()(i, 0);
  else
    {
      Cerr << "Echange_impose_base::T_ext erreur" << finl;
      assert(0);
    }
  exit();
  return 0.;
}

/*! @brief Renvoie la valeur de la temperature imposee sur la (i,j)-eme composante du champ de frontiere.
 *
 * @param (int i)
 * @param (int j)
 * @return (double)
 */
double Echange_impose_base::T_ext(int i, int j) const
{
  if (T_ext()->valeurs().dimension(0) == 1)
    return T_ext()->valeurs()(0, j);
  else
    return T_ext()->valeurs()(i, j);
}

/*! @brief Renvoie la valeur du coefficient d'echange de chaleur impose sur la i-eme composante
 *
 *     du champ de frontiere.
 *
 * @param (int i)
 * @return (double)
 */
double Echange_impose_base::h_imp(int i) const
{

  if (h_imp_->valeurs().size() == 1)
    return h_imp_->valeurs()(0, 0);
  else if (h_imp_->valeurs().dimension(1) == 1)
    return h_imp_->valeurs()(i, 0);
  else
    Cerr << "Echange_impose_base::h_imp erreur" << finl;

  exit();
  return 0.;
}

/*! @brief Renvoie la valeur du coefficient d'echange de chaleur impose sur la i-eme composante
 *
 *     du champ de frontiere.
 *
 * @param (int i)
 * @param (int j)
 * @return (double)
 */
double Echange_impose_base::h_imp(int i, int j) const
{

  if (h_imp_->valeurs().dimension(0) == 1)
    return h_imp_->valeurs()(0, j);
  else
    return h_imp_->valeurs()(i, j);

}

/*! @brief Effectue une mise a jour en temps des conditions aux limites.
 *
 *     Lors du premier appel des initialisations sont effectuees:
 *       h_imp(0,0) = (rho(0,0)*Cp(0,0))
 *
 * @param (double temps) le temp de mise a jour
 */
void Echange_impose_base::mettre_a_jour(double temps)
{
  Cond_lim_base::mettre_a_jour(temps);
  h_imp_->mettre_a_jour(temps);
}

int Echange_impose_base::initialiser(double temps)
{
  if (h_imp_.non_nul())
    h_imp_->initialiser(temps, domaine_Cl_dis().equation().inconnue()), h_imp_->mettre_a_jour(temps);
  return Cond_lim_base::initialiser(temps);
}

// ajout de methode pour ne pas operer directement su le champ_front
void Echange_impose_base::set_temps_defaut(double temps)
{
  if (h_imp_.non_nul())
    h_imp_->set_temps_defaut(temps);
  Cond_lim_base::set_temps_defaut(temps);
}
void Echange_impose_base::fixer_nb_valeurs_temporelles(int nb_cases)
{
  if (h_imp_.non_nul())
    h_imp_->fixer_nb_valeurs_temporelles(nb_cases);
  Cond_lim_base::fixer_nb_valeurs_temporelles(nb_cases);
}
//
void Echange_impose_base::changer_temps_futur(double temps, int i)
{
  if (h_imp_.non_nul())
    h_imp_->changer_temps_futur(temps, i);
  Cond_lim_base::changer_temps_futur(temps, i);
}
int Echange_impose_base::avancer(double temps)
{
  if (h_imp_.non_nul())
    h_imp_->avancer(temps);
  return Cond_lim_base::avancer(temps);
}

int Echange_impose_base::reculer(double temps)
{
  if (h_imp_.non_nul())
    h_imp_->reculer(temps);
  return Cond_lim_base::reculer(temps);
}
void Echange_impose_base::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  if (h_imp_.non_nul())
    h_imp_->associer_fr_dis_base(fr);
  Cond_lim_base::associer_fr_dis_base(fr);
}
