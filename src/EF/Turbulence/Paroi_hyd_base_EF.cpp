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

#include <Modele_turbulence_hyd_base.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_paroi_fixe.h>
#include <Discretisation_base.h>
#include <Domaine_Cl_dis_base.h>
#include <Paroi_hyd_base_EF.h>
#include <Schema_Temps_base.h>
#include <communications.h>

#include <EcrFicPartage.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <TRUSTTrav.h>
#include <Scatter.h>

Implemente_base(Paroi_hyd_base_EF, "Paroi_hyd_base_EF", Turbulence_paroi_base);

Sortie& Paroi_hyd_base_EF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Paroi_hyd_base_EF::readOn(Entree& s) { return Turbulence_paroi_base::readOn(s); }

void Paroi_hyd_base_EF::creer_champ(const Motcle& motlu)
{
  if (motlu == "u_star" && !champ_u_star_.non_nul())
    {
      int nb_comp = 1;
      Noms noms(1);
      noms[0] = "u_star";
      Noms unites(1);
      unites[0] = "m/s";
      double temps = 0.;
      const Equation_base& equation = mon_modele_turb_hyd->equation();
      const Discretisation_base& discr = equation.probleme().discretisation();
      discr.discretiser_champ("champ_elem", equation.domaine_dis(), scalaire, noms, unites, nb_comp, temps, champ_u_star_);
      champs_compris_.ajoute_champ(champ_u_star_);
    }
}

const Champ_base& Paroi_hyd_base_EF::get_champ(const Motcle& nom) const
{
  const Domaine_EF& domaine_EF = le_dom_EF.valeur();
  const IntTab& face_voisins = domaine_EF.face_voisins();
  if (nom == champ_u_star_.le_nom())
    {
      // Initialisation a 0 du champ volumique u_star
      DoubleTab& valeurs = champ_u_star_->valeurs();
      valeurs = 0;
      const Equation_base& my_eqn = mon_modele_turb_hyd->equation();
      if (tab_u_star_.size_array() > 0)
        {
          // Boucle sur les frontieres pour recuperer u_star si tab_u_star dimensionne
          int nb_front = my_eqn.domaine_dis().nb_front_Cl();
          for (int n_bord = 0; n_bord < nb_front; n_bord++)
            {
              const Cond_lim& la_cl = my_eqn.domaine_Cl_dis().les_conditions_limites(n_bord);
              if (sub_type(Dirichlet_paroi_fixe, la_cl.valeur()))
                {
                  const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
                  int ndeb = le_bord.num_premiere_face();
                  int nfin = ndeb + le_bord.nb_faces();
                  for (int num_face = ndeb; num_face < nfin; num_face++)
                    {
                      int elem = face_voisins(num_face, 0);
                      valeurs(elem) = tab_u_star_(num_face);
                    }
                }
            }
        }
      valeurs.echange_espace_virtuel();
      // Met a jour le temps du champ:
      champ_u_star_->mettre_a_jour(my_eqn.schema_temps().temps_courant());
      return champs_compris_.get_champ(nom);
    }
  else
    return champs_compris_.get_champ(nom);
}

void Paroi_hyd_base_EF::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  Turbulence_paroi_base::get_noms_champs_postraitables(nom, opt);
}

void Paroi_hyd_base_EF::associer(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_Cl_dis)
{
  le_dom_EF = ref_cast(Domaine_EF, domaine_dis);
  le_dom_Cl_EF = ref_cast(Domaine_Cl_EF, domaine_Cl_dis);
}

void Paroi_hyd_base_EF::init_lois_paroi_()
{
  const Domaine_VF& zvf = le_dom_EF.valeur();
  const int nb_faces_bord = le_dom_EF->nb_faces_bord();
  tab_u_star_.resize(nb_faces_bord);
  tab_d_plus_.resize(nb_faces_bord);
  if (!Cisaillement_paroi_.get_md_vector().non_nul())
    {
      Cisaillement_paroi_.resize(0, dimension);
      zvf.creer_tableau_faces_bord(Cisaillement_paroi_);
    }
}

DoubleTab& Paroi_hyd_base_EF::corriger_derivee_impl(DoubleTab& d) const
{
  if (flag_face_keps_imposee_)
    {
      int size = d.dimension_tot(0);
      assert(size == face_keps_imposee_.size_array());
      for (int face = 0; face < size; face++)
        {
          if (face_keps_imposee_(face) != -2)
            {
              d(face, 0) = 0;
              d(face, 1) = 0;
            }
        }
    }
  return d;
}

void Paroi_hyd_base_EF::imprimer_premiere_ligne_ustar(int boundaries_, const LIST(Nom) &boundaries_list, const Nom& nom_fichier_) const
{
  EcrFicPartage fichier;
  ouvrir_fichier_partage(fichier, nom_fichier_, "out");
  const Domaine_EF& domaine_EF = le_dom_EF.valeur();
  Nom ligne, err;

  err = "";
  ligne = "# Time   \tMean(u*) \tMean(d+)";

  for (int n_bord = 0; n_bord < domaine_EF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_EF->les_conditions_limites(n_bord);
      const Nom& nom_bord = la_cl->frontiere_dis().le_nom();
      if (je_suis_maitre() && (boundaries_list.contient(nom_bord) || boundaries_list.size() == 0))
        {
          if ( sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante, la_cl.valeur()))
            {
              ligne += " \t";
              ligne += nom_bord;
              ligne += "(u*)";
              ligne += " \t";
              ligne += nom_bord;
              ligne += "(d*)";
            }
          else if (boundaries_list.size() > 0)
            {
              err += "The boundary named '";
              err += nom_bord;
              err += "' is not of type Dirichlet_paroi_fixe or Dirichlet_paroi_defilante.\n";
              err += "So TRUST will not write his u_star and d_plus means.\n\n";
            }
        }
    }
  if (je_suis_maitre())
    {
      fichier << err;
      fichier << ligne;
      fichier << finl;
    }
  fichier.syncfile();
}

void Paroi_hyd_base_EF::imprimer_ustar_mean_only(Sortie& os, int boundaries_, const LIST(Nom) &boundaries_list, const Nom& nom_fichier_) const
{
  const Domaine_EF& domaine_EF = le_dom_EF.valeur();
  const Probleme_base& pb = mon_modele_turb_hyd->equation().probleme();
  const Schema_Temps_base& sch = pb.schema_temps();
  int ndeb, nfin, size0, num_bord;
  num_bord = 0;

  if (boundaries_list.size() != 0)
    {
      size0 = boundaries_list.size();
    }
  else
    {
      size0 = domaine_EF.nb_front_Cl();
    }
  DoubleTrav moy_bords(size0 + 1, 3);
  moy_bords = 0.;

  EcrFicPartage fichier;
  ouvrir_fichier_partage(fichier, nom_fichier_, "out");

  for (int n_bord = 0; n_bord < domaine_EF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_EF->les_conditions_limites(n_bord);
      if ((sub_type(Dirichlet_paroi_fixe, la_cl.valeur())) || (sub_type(Dirichlet_paroi_defilante, la_cl.valeur())))
        {
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          if (boundaries_ == 0 || (boundaries_ == 1 && boundaries_list.contient(le_bord.le_nom())))
            {
              for (int num_face = ndeb; num_face < nfin; num_face++)
                {
                  // Calcul des valeurs moyennes par bord (en supposant maillage regulier)
                  moy_bords(0, 0) += tab_u_star(num_face);
                  moy_bords(0, 1) += 1;
                  moy_bords(0, 2) += tab_d_plus(num_face);
                  moy_bords(num_bord + 1, 0) += tab_u_star(num_face);
                  moy_bords(num_bord + 1, 1) += 1;
                  moy_bords(num_bord + 1, 2) += tab_d_plus(num_face);
                }
              num_bord += 1;
            }
        }
    }
  mp_sum_for_each_item(moy_bords);

// affichages des lignes dans le fichier
  if (je_suis_maitre() && moy_bords(0, 1) != 0)
    {
      fichier << sch.temps_courant() << " \t" << moy_bords(0, 0) / moy_bords(0, 1) << " \t" << moy_bords(0, 2) / moy_bords(0, 1);
    }

  num_bord = 0;
  for (int n_bord = 0; n_bord < domaine_EF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_EF->les_conditions_limites(n_bord);
      if ((sub_type(Dirichlet_paroi_fixe, la_cl.valeur())) || (sub_type(Dirichlet_paroi_defilante, la_cl.valeur())))
        {
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
          if (boundaries_ == 0 || (boundaries_ == 1 && boundaries_list.contient(le_bord.le_nom())))
            {
              if (je_suis_maitre())
                {
                  fichier << " \t" << moy_bords(num_bord + 1, 0) / moy_bords(num_bord + 1, 1) << " \t" << moy_bords(num_bord + 1, 2) / moy_bords(num_bord + 1, 1);
                }
              num_bord += 1;
            }
        }
    }

  if (je_suis_maitre())
    fichier << finl;
  fichier.syncfile();
}

