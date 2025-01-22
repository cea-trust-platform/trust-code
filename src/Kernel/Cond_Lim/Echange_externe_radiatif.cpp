/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Echange_externe_radiatif.h>
#include <Domaine_Cl_dis_base.h>
#include <Equation_base.h>

Implemente_instanciable(Echange_externe_radiatif, "Echange_externe_radiatif", Cond_lim_base);
// XD paroi_echange_externe_radiatif condlim_base paroi_echange_externe_radiatif -1 External type Radiation exchange condition with an emissivity coefficient and an imposed external temperature.
// XD attr emissivite chaine emissivite 0 Emissivity coefficient value.
// XD attr emissivitebc front_field_base emissivitebc 0 Boundary field type.
// XD attr text chaine text 0 External temperature value (expressed in oC or K).
// XD attr ch front_field_base ch 0 Boundary field type.


Sortie& Echange_externe_radiatif::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Echange_externe_radiatif::readOn(Entree& s)
{
  if (app_domains.size() == 0)
    app_domains = { Motcle("Thermique") };

  if (supp_discs.size() == 0)
    supp_discs = { Nom("VDF"), Nom("VEFPreP1B"), Nom("PolyMAC"), Nom("PolyMAC_P0P1NC"), Nom("PolyMAC_P0") };

  Motcle motlu;
  Motcles les_motcles(2);
  {
    les_motcles[0] = "emissivite";
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
            s >> emissivite_;
            break;
          }
        case 1:
          {
            s >> le_champ_front;
            break;
          }
        default:
          {
            Cerr << "Error while reading BC of type Echange_externe_radiatif " << finl;
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
double Echange_externe_radiatif::T_ext(int i) const
{
  if (T_ext().valeurs().size() == 1)
    return T_ext().valeurs()(0, 0);
  else if (T_ext().valeurs().dimension(1) == 1)
    return T_ext().valeurs()(i, 0);
  else
    {
      Cerr << "Echange_externe_radiatif::T_ext erreur" << finl;
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
double Echange_externe_radiatif::T_ext(int i, int j) const
{
  if (T_ext().valeurs().dimension(0) == 1)
    return T_ext().valeurs()(0, j);
  else
    return T_ext().valeurs()(i, j);
}

/*! @brief Renvoie la valeur de l'emissivite impose sur la i-eme composante
 *
 *     du champ de frontiere.
 *
 * @param (int i)
 * @return (double)
 */
double Echange_externe_radiatif::emissivite(int i) const
{

  if (emissivite_->valeurs().size() == 1)
    return emissivite_->valeurs()(0, 0);
  else if (emissivite_->valeurs().dimension(1) == 1)
    return emissivite_->valeurs()(i, 0);
  else
    Cerr << "Echange_externe_radiatif::emissivite erreur" << finl;

  exit();
  return 0.;
}

/*! @brief Renvoie la valeur de l'emissivite impose sur la i-eme composante
 *
 *     du champ de frontiere.
 *
 * @param (int i)
 * @param (int j)
 * @return (double)
 */
double Echange_externe_radiatif::emissivite(int i, int j) const
{

  if (emissivite_->valeurs().dimension(0) == 1)
    return emissivite_->valeurs()(0, j);
  else
    return emissivite_->valeurs()(i, j);

}

void Echange_externe_radiatif::mettre_a_jour(double temps)
{
  Cond_lim_base::mettre_a_jour(temps);
  emissivite_->mettre_a_jour(temps);
}

int Echange_externe_radiatif::initialiser(double temps)
{
  if (emissivite_.non_nul())
    emissivite_->initialiser(temps, domaine_Cl_dis().equation().inconnue()), emissivite_->mettre_a_jour(temps);
  return Cond_lim_base::initialiser(temps);
}

void Echange_externe_radiatif::set_temps_defaut(double temps)
{
  if (emissivite_.non_nul())
    emissivite_->set_temps_defaut(temps);
  Cond_lim_base::set_temps_defaut(temps);
}

void Echange_externe_radiatif::fixer_nb_valeurs_temporelles(int nb_cases)
{
  if (emissivite_.non_nul())
    emissivite_->fixer_nb_valeurs_temporelles(nb_cases);
  Cond_lim_base::fixer_nb_valeurs_temporelles(nb_cases);
}

void Echange_externe_radiatif::changer_temps_futur(double temps, int i)
{
  if (emissivite_.non_nul())
    emissivite_->changer_temps_futur(temps, i);
  Cond_lim_base::changer_temps_futur(temps, i);
}

int Echange_externe_radiatif::avancer(double temps)
{
  if (emissivite_.non_nul())
    emissivite_->avancer(temps);
  return Cond_lim_base::avancer(temps);
}

int Echange_externe_radiatif::reculer(double temps)
{
  if (emissivite_.non_nul())
    emissivite_->reculer(temps);
  return Cond_lim_base::reculer(temps);
}

void Echange_externe_radiatif::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  if (emissivite_.non_nul())
    emissivite_->associer_fr_dis_base(fr);
  Cond_lim_base::associer_fr_dis_base(fr);
}

void Echange_externe_radiatif::verifie_ch_init_nb_comp() const
{
  if (le_champ_front.non_nul())
    {
      const Equation_base& eq = domaine_Cl_dis().equation();
      const int nb_comp = le_champ_front->nb_comp();
      eq.verifie_ch_init_nb_comp_cl(eq.inconnue(), nb_comp, *this);
    }
}
