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

#include <Paroi_hyd_base_VDF.h>
#include <Champ_Uniforme.h>
#include <Domaine_Cl_VDF.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Fluide_Quasi_Compressible.h>

#include <EcrFicPartage.h>
#include <Modele_turbulence_hyd_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <TRUSTTrav.h>
#include <communications.h>

Implemente_base(Paroi_hyd_base_VDF, "Paroi_hyd_base_VDF", Turbulence_paroi_base);

Sortie& Paroi_hyd_base_VDF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Paroi_hyd_base_VDF::readOn(Entree& s) { return Turbulence_paroi_base::readOn(s); }

void Paroi_hyd_base_VDF::associer(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_Cl_dis)
{
  le_dom_VDF = ref_cast(Domaine_VDF, domaine_dis);
  le_dom_Cl_VDF = ref_cast(Domaine_Cl_VDF, domaine_Cl_dis);
}

void Paroi_hyd_base_VDF::init_lois_paroi_()
{
  tab_u_star_.resize(le_dom_VDF->nb_faces_bord());
  tab_d_plus_.resize(le_dom_VDF->nb_faces_bord());
  if (!Cisaillement_paroi_.get_md_vector().non_nul())
    {
      Cisaillement_paroi_.resize(0, dimension);
      le_dom_VDF->creer_tableau_faces_bord(Cisaillement_paroi_);
    }
}

void Paroi_hyd_base_VDF::imprimer_premiere_ligne_ustar(int boundaries_, const LIST(Nom) &boundaries_list, const Nom& nom_fichier_) const
{
  EcrFicPartage fichier;
  ouvrir_fichier_partage(fichier, nom_fichier_, "out");
  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
  Nom ligne, err;
  err = "";
  ligne = "# Time   \tMean(u*) \tMean(d+)";

  for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_VDF->les_conditions_limites(n_bord);
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
              ligne += "(d+)";
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

void Paroi_hyd_base_VDF::imprimer_ustar_mean_only(Sortie& os, int boundaries_, const LIST(Nom) &boundaries_list, const Nom& nom_fichier_) const
{
  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
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
      size0 = domaine_VDF.nb_front_Cl();
    }
  DoubleTrav moy_bords(size0 + 1, 3);
  moy_bords = 0.;

  EcrFicPartage fichier;
  ouvrir_fichier_partage(fichier, nom_fichier_, "out");

  for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_VDF->les_conditions_limites(n_bord);
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
  for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_VDF->les_conditions_limites(n_bord);
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
