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

#include <Turbulence_paroi_base.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Modele_turbulence_hyd_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <Front_VF.h>
#include <Param.h>
#include <Noms.h>

Implemente_base_sans_constructeur(Turbulence_paroi_base, "Turbulence_paroi_base", Objet_U);
Turbulence_paroi_base::Turbulence_paroi_base()
{
  nb_impr0_ = 0;
  nb_impr_ = 0;
  champs_compris_.ajoute_nom_compris("u_star");
}

Sortie& Turbulence_paroi_base::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Turbulence_paroi_base::readOn(Entree& is)
{
  return is;
}

void Turbulence_paroi_base::set_param(Param& param)
{
}

/*! @brief Give a boolean indicating if we need to use shear by default we consider that we use the shear
 *
 * @return (boolean)
 */
bool Turbulence_paroi_base::use_shear() const
{
  return true;
}

void Turbulence_paroi_base::creer_champ(const Motcle& motlu)
{
  if (motlu == "u_star" && champ_u_star_.est_nul())
    {
      int nb_comp = 1;
      Noms noms(1);
      noms[0] = "u_star";
      Noms unites(1);
      unites[0] = "m2/s2";
      double temps = 0.;
      const Equation_base& equation = mon_modele_turb_hyd->equation();
      const Discretisation_base& discr = equation.probleme().discretisation();
      discr.discretiser_champ("champ_face", equation.domaine_dis(), scalaire, noms, unites, nb_comp, temps, champ_u_star_);
      champs_compris_.ajoute_champ(champ_u_star_);
    }
}

const Champ_base& Turbulence_paroi_base::get_champ(const Motcle& nom) const
{
  if (nom == champ_u_star_.le_nom())
    {
      // Initialisation a 0 du champ volumique u_star
      DoubleTab& valeurs = champ_u_star_->valeurs();
      valeurs = 0;
      const Equation_base& my_eqn = mon_modele_turb_hyd->equation();
      if (tab_u_star_.size_array() > 0)
        {
          // Boucle sur les frontieres pour recuperer u_star si tab_u_star dimensionne
          const Domaine_Cl_dis& zcl = my_eqn.domaine_Cl_dis();
          int nb_front = my_eqn.domaine_dis().valeur().nb_front_Cl();
          for (int n_bord = 0; n_bord < nb_front; n_bord++)
            {
              const Cond_lim& la_cl = zcl.les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();
              for (int num_face = ndeb; num_face < nfin; num_face++)
                valeurs(num_face) = tab_u_star_(num_face);
            }
        }
      valeurs.echange_espace_virtuel();
      // Met a jour le temps du champ:
      champ_u_star_.mettre_a_jour(my_eqn.schema_temps().temps_courant());
      return champs_compris_.get_champ(nom);
    }
  else
    return champs_compris_.get_champ(nom);
}

void Turbulence_paroi_base::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  if (opt == DESCRIPTION)
    Cerr << que_suis_je() << " : " << champs_compris_.liste_noms_compris() << finl;
  else
    nom.add(champs_compris_.liste_noms_compris());

}
/*! @brief Ouverture/creation d'un fichier d'impression de Face, uplus_, dplus_, tab_u_star, Cisaillement_paroi_
 *
 */
void Turbulence_paroi_base::ouvrir_fichier_partage(EcrFicPartage& Ustar, const Nom& extension) const
{

  const Probleme_base& pb = mon_modele_turb_hyd->equation().probleme();
  const Schema_Temps_base& sch = pb.schema_temps();

  Nom fichier = Objet_U::nom_du_cas() + "_" + pb.le_nom() + "_" + extension + ".face";

  // On cree le fichier au premier pas de temps si il n'y a pas reprise
  if (nb_impr_ == 0 && !pb.reprise_effectuee())
    {
      Ustar.ouvrir(fichier);
    }
  // Sinon on l'ouvre
  else
    {
      Ustar.ouvrir(fichier, ios::app);
    }

  if (je_suis_maitre())
    {
      EcrFicPartage& fic = Ustar;
      fic << "Temps : " << sch.temps_courant();
    }

  nb_impr_++;
}

/*! @brief Ouverture/creation d'un fichier d'impression de moyennes de uplus_, dplus_, tab_u_star
 *
 */
void Turbulence_paroi_base::ouvrir_fichier_partage(EcrFicPartage& fichier, const Nom& nom_fichier, const Nom& extension) const
{
  const Probleme_base& pb = mon_modele_turb_hyd->equation().probleme();
  Nom nom_fic = nom_fichier + "." + extension;

  // On cree le fichier nom_fichier au premier pas de temps si il n'y a pas reprise
  if (nb_impr0_ == 0 && !pb.reprise_effectuee())
    {
      fichier.ouvrir(nom_fic);
    }
  // Sinon on l'ouvre
  else
    {
      fichier.ouvrir(nom_fic, ios::app);
    }
  nb_impr0_++;
}

