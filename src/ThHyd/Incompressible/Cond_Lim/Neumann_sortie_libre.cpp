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

#include <Motcle.h>
#include <Neumann_sortie_libre.h>
#include <Navier_Stokes_std.h>
#include <Equation_base.h>

Implemente_instanciable(Neumann_sortie_libre, "Frontiere_ouverte", Neumann_val_ext);
// XD frontiere_ouverte neumann frontiere_ouverte -1 Boundary outlet condition on the boundary called bord (edge) (diffusion flux zero). This condition must be associated with a boundary outlet hydraulic condition.
// XD attr var_name chaine(into=["T_ext","C_ext","Y_ext","K_Eps_ext","Fluctu_Temperature_ext","Flux_Chaleur_Turb_ext","V2_ext","a_ext","tau_ext","k_ext","omega_ext","H_ext"]) var_name 0 Field name.
// XD attr ch front_field_base ch 0 Boundary field type.


Sortie& Neumann_sortie_libre::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

/*! @brief Type le_champ_front en "Champ_front_uniforme".
 *
 * Lit les valeurs du champ exterieur si les conditions
 *     aux limites sont specifiees: "T_ext","C_ext","Y_ext","K_Eps_ext","Fluctu_Temperature_ext","Flux_Chaleur_Turb_ext","V2_ext","a_ext","tau_ext","k_ext","omega_ext","H_ext"
 *     Produit une erreur sinon.
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree& s) le flot d'entree modifie
 * @throws type de champ exterieur non reconnu,
 * les types reconnus sont: "T_ext","C_ext","Y_ext","K_Eps_ext","Fluctu_Temperature_ext","Flux_Chaleur_Turb_ext","V2_ext","a_ext","tau_ext","k_ext","omega_ext","H_ext"
 */
Entree& Neumann_sortie_libre::readOn(Entree& s)
{
  if (app_domains.size() == 0)
    app_domains = {Motcle("Thermique"), Motcle("Thermique_H"),
                   Motcle("Transport_Keps"), Motcle("Transport_Keps_V2"),
                   Motcle("Transport_Keps_Bas_Re"), Motcle("Transport_Keps_Rea"),
                   Motcle("Concentration"), Motcle("Fraction_massique"),
                   Motcle("Fraction_volumique"), Motcle("Transport_V2"),
                   Motcle("Turbulence"), Motcle("Interfacial_area"),
                   Motcle("indetermine"), Motcle("Transport_Komega")
                  };

  Motcle motlu;
  Motcles les_motcles(16);
  {
    les_motcles[0] = "T_ext";
    les_motcles[1] = "C_ext";
    les_motcles[2] = "K_Eps_ext";
    les_motcles[3] = "F_M_ext";
    les_motcles[4] = "Fluctu_Temperature_ext";
    les_motcles[5] = "Flux_Chaleur_Turb_ext";
    les_motcles[6] = "V2_ext";
    les_motcles[7] = "Y_ext";
    les_motcles[8] = "A_ext";
    les_motcles[9] = "k_ext";
    les_motcles[10] = "tau_ext";
    les_motcles[11] = "omega_ext";
    les_motcles[12] = "k_WIT_ext";
    les_motcles[13] = "a_i_ext";
    les_motcles[14] = "K_Omega_ext";
    les_motcles[15] = "H_ext"; // enthalpie
  }
  s >> motlu;
  int rang = les_motcles.search(motlu);
  if (rang >= 0)
    s >> le_champ_ext;
  else
    {
      Cerr << "Erreur a la lecture de la condition aux limites de type: " << finl;
      Cerr << que_suis_je() << finl;
      Cerr << "On attendait " << les_motcles << " a la place de " << motlu << finl;
      exit();
    }

  le_champ_front = le_champ_ext;

  return s;
}

//Dans le cas ou la condition limite porte sur la pression
//il faut le specifier explicitement sinon inconnue() rend
//la vitesse

void Neumann_sortie_libre::verifie_ch_init_nb_comp() const
{
  if (le_champ_front.non_nul())
    {
      const Equation_base& eq = domaine_Cl_dis().equation();
      const int nb_comp = le_champ_front.valeur().nb_comp();

      if ((que_suis_je() == "Frontiere_ouverte") || (que_suis_je() == "Frontiere_ouverte_rayo_semi_transp") || (que_suis_je() == "Frontiere_Ouverte_Rayo_transp")
          || (que_suis_je() == "Sortie_libre_temperature_imposee_H"))
        eq.verifie_ch_init_nb_comp_cl(eq.inconnue(), nb_comp, *this);
      else
        {
          const Navier_Stokes_std& eq_ns = ref_cast(Navier_Stokes_std, eq);
          eq.verifie_ch_init_nb_comp_cl(eq_ns.pression(), nb_comp, *this);
        }
    }
}

/*! @brief Renvoie la valeur de la i-eme composante du champ impose a l'exterieur de la frontiere.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 * @throws deuxieme dimension du champ de frontiere superieur a 1
 */
double Neumann_sortie_libre::val_ext(int i) const
{
  if (le_champ_ext.valeurs().size() == 1)
    return le_champ_ext(0, 0);
  else if (le_champ_ext.valeurs().dimension(1) == 1)
    return le_champ_ext(i, 0);
  else
    {
      Cerr << "Neumann_sortie_libre::val_ext" << finl;
      Cerr << le_champ_ext << finl;
    }
  exit();
  return 0.;
}

int Neumann_sortie_libre::initialiser(double temps)
{
  Cond_lim_base::initialiser(temps);
  assert(le_champ_ext.non_nul());
  return le_champ_ext->initialiser(temps, domaine_Cl_dis().equation().inconnue());
}

void Neumann_sortie_libre::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  Cond_lim_base::associer_fr_dis_base(fr);
  assert(le_champ_ext.non_nul());
  le_champ_ext->associer_fr_dis_base(fr);
  modifier_val_imp = 0;
}

/*! @brief Renvoie la valeur de la (i,j)-eme composante du champ impose a l'exterieur de la frontiere.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @param (int j) indice suivant la deuxieme dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 */
double Neumann_sortie_libre::val_ext(int i, int j) const
{
  if (le_champ_ext.valeurs().dimension(0) == 1)
    return le_champ_ext(0, j);
  else
    return le_champ_ext(i, j);
}

const DoubleTab& Neumann_sortie_libre::tab_ext() const
{
  return le_champ_ext.valeurs();
}

DoubleTab& Neumann_sortie_libre::tab_ext()
{
  return le_champ_ext.valeurs();
}

void Neumann_sortie_libre::fixer_nb_valeurs_temporelles(int nb_cases)
{
  Neumann::fixer_nb_valeurs_temporelles(nb_cases);
  le_champ_ext->fixer_nb_valeurs_temporelles(nb_cases);
}

void Neumann_sortie_libre::mettre_a_jour(double temps)
{
  Neumann::mettre_a_jour(temps);
  le_champ_ext->mettre_a_jour(temps);
}

void Neumann_sortie_libre::set_temps_defaut(double temps)
{
  Neumann::set_temps_defaut(temps);
  le_champ_ext->set_temps_defaut(temps);
}

void Neumann_sortie_libre::changer_temps_futur(double temps, int i)
{
  Neumann::changer_temps_futur(temps, i);
  le_champ_ext->changer_temps_futur(temps, i);
}

int Neumann_sortie_libre::avancer(double temps)
{
  return Neumann::avancer(temps) && le_champ_ext->avancer(temps);
}

int Neumann_sortie_libre::reculer(double temps)
{
  return Neumann::reculer(temps) && le_champ_ext->reculer(temps);
}
