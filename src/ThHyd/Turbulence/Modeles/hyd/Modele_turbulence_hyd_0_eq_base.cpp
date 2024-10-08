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

#include <Modele_turbulence_hyd_0_eq_base.h>
#include <Modifier_pour_fluide_dilatable.h>
#include <Domaine_Cl_dis_base.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <stat_counters.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Ecrire_MED.h>
#include <Domaine.h>
#include <Motcle.h>
#include <Param.h>

Implemente_base(Modele_turbulence_hyd_0_eq_base, "Modele_turbulence_hyd_0_eq_base", Modele_turbulence_hyd_base);

Sortie& Modele_turbulence_hyd_0_eq_base::printOn(Sortie& is) const { return Modele_turbulence_hyd_base::printOn(is); }

Entree& Modele_turbulence_hyd_0_eq_base::readOn(Entree& is) { return Modele_turbulence_hyd_base::readOn(is); }

void Modele_turbulence_hyd_0_eq_base::set_param(Param& param)
{
  Modele_turbulence_hyd_base::set_param(param);
  param.ajouter("fichier_ecriture_K_eps", &fichier_K_eps_sortie_);
}

void Modele_turbulence_hyd_0_eq_base::discretiser()
{
  Modele_turbulence_hyd_base::discretiser();
  discretiser_K(mon_equation_->schema_temps(), mon_equation_->domaine_dis(), energie_cinetique_turb_);
  champs_compris_.ajoute_champ(energie_cinetique_turb_);
}

void Modele_turbulence_hyd_0_eq_base::associer(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_Cl_dis)
{
  le_dom_VF_ = ref_cast(Domaine_VF, domaine_dis);
  le_dom_Cl_ = ref_cast(Domaine_Cl_dis_base, domaine_Cl_dis);
}

int Modele_turbulence_hyd_0_eq_base::a_pour_Champ_Fonc(const Motcle& mot,
                                                       OBS_PTR(Champ_base) &ch_ref) const
{
  Motcles les_motcles(3);
  {
    les_motcles[0] = "viscosite_turbulente";
    les_motcles[1] = "k";
    les_motcles[2] = "distance_paroi";
  }
  int rang = les_motcles.search(mot);
  switch(rang)
    {
    case 0:
      {
        ch_ref = la_viscosite_turbulente_.valeur();
        return 1;
      }
    case 1:
      {
        Cerr << "The kinetic energy cannot be post-processed for the mixing length model." << finl;
        //A coder
        return 0;
      }
    case 2:
      {
        exit();
        return 0;
      }
    default:
      return 0;
    }
}
int Modele_turbulence_hyd_0_eq_base::comprend_mot(const Motcle& mot) const
{
  return comprend_champ(mot);
}

int Modele_turbulence_hyd_0_eq_base::comprend_champ(const Motcle& mot) const
{
  Motcles les_motcles(3);
  {
    les_motcles[0] = "k";
    les_motcles[1] = "viscosite_turbulente";
    les_motcles[2] = "distance_paroi";
  }

  int rang = les_motcles.search(mot);

  if (rang == 0)
    {
      Cerr << "The kinetic energy is not known for the mixing length model." << finl;
      //A coder
      return 0;
    }
  else if (rang == 1)
    return 1;
  else if (rang == 2)
    {

      exit();
      return 0;
    }
  else
    return 0;
}

void Modele_turbulence_hyd_0_eq_base::completer()
{
  // creation de K_eps_sortie et du fichier med si on a demande ecrire_K_eps
  if (fichier_K_eps_sortie_ != Nom())
    {
      // 1) on cree le fichier med et on postraite le domaine
      const Domaine& dom = mon_equation_->domaine_dis().domaine();
      Ecrire_MED ecr_med(fichier_K_eps_sortie_.nom_me(me()), dom);
      ecr_med.ecrire_domaine(false);
      //2 on discretise le champ K_eps_pour_la_sortie
      const Discretisation_base& dis = mon_equation_->discretisation();
      Noms noms(2);
      Noms unit(2);
      noms[0] = "K";
      noms[1] = "eps";
      unit[0] = "m2/s2";
      unit[1] = "m2/s3";
      int nb_case_tempo = 1;
      double temps = mon_equation_->schema_temps().temps_courant();
      dis.discretiser_champ("CHAMP_ELEM", mon_equation_->domaine_dis(), scalaire, noms, unit, 2, nb_case_tempo, temps, K_eps_sortie_);
      K_eps_sortie_->nommer("K_eps_from_nut");
      K_eps_sortie_->fixer_unites(unit);
      K_eps_sortie_->fixer_noms_compo(noms);
    }
}

void Modele_turbulence_hyd_0_eq_base::mettre_a_jour(double)
{
  statistiques().begin_count(nut_counter_);
  calculer_viscosite_turbulente();
  calculer_energie_cinetique_turb();
  loipar_->calculer_hyd(la_viscosite_turbulente_, energie_cinetique_turbulente());
  limiter_viscosite_turbulente();
  if (mon_equation_->probleme().is_dilatable())
    correction_nut_et_cisaillement_paroi_si_qc(*this);
  energie_cinetique_turb_->valeurs().echange_espace_virtuel();
  la_viscosite_turbulente_->valeurs().echange_espace_virtuel();
  statistiques().end_count(nut_counter_);
}

void Modele_turbulence_hyd_0_eq_base::imprimer(Sortie& os) const
{
  const Schema_Temps_base& sch = mon_equation_->schema_temps();
  double temps_courant = sch.temps_courant();
  double dt = sch.pas_de_temps();
  if (limpr_ustar(temps_courant, sch.temps_precedent(), dt, dt_impr_ustar_) || limpr_ustar(temps_courant, sch.temps_precedent(), dt, dt_impr_ustar_mean_only_))
    if (K_eps_sortie_.non_nul())
      {
        double temps = mon_equation_->schema_temps().temps_courant();
        K_eps_sortie_->mettre_a_jour(temps);

        //  calcul de K_eps

        DoubleTab& K_Eps = K_eps_sortie_->valeurs();
        const DoubleTab& visco_turb = la_viscosite_turbulente_->valeurs();
        const DoubleTab& wall_length = wall_length_->valeurs();
        const int nb_elem = K_Eps.dimension(0);

        const double Kappa = 0.415;
        double Cmu = CMU;
        // PQ : 27/06/07 : expressions de k et eps basees sur :
        //
        //   nu_t = C_mu.K.L.k^(1/2)  et nu_t = C_mu.k^2/eps

        for (int elem = 0; elem < nb_elem; elem++)
          {
            K_Eps(elem, 0) = pow(visco_turb(elem) / (Cmu * Kappa * wall_length(elem)), 2);

            if (visco_turb(elem) == 0.)
              K_Eps(elem, 1) = 0.;
            else
              K_Eps(elem, 1) = Cmu * K_Eps(elem, 0) * K_Eps(elem, 0) / visco_turb(elem);
          }

        // PQ : recalibrage de k et eps d'apres resultats
        //        sur Canal plan a Re = 100 000 et Re = 1 000 000
        for (int elem = 0; elem < nb_elem; elem++)
          {
            K_Eps(elem, 0) /= 47.;
            K_Eps(elem, 1) /= 1000.;
            //Cerr<<elem <<" "<< K_Eps(elem,0)<<" "<<K_Eps(elem,1)<<" ";
            //Cerr<<visco_turb(elem)<<" "<<wall_length(elem)<<finl;
          }

        // enfin ecriture du champ aux elems (il y est deja)
        const Domaine& dom = mon_equation_->domaine_dis().domaine();
        Nom fic = fichier_K_eps_sortie_.nom_me(me());

        const Nom& nom_post = K_eps_sortie_.le_nom();
        const Nom& type_elem = dom.type_elem()->que_suis_je();
        assert(K_eps_sortie_->valeurs().dimension(0) == dom.nb_elem());
        Ecrire_MED ecr_med(fic, dom);
        ecr_med.ecrire_champ("CHAMPMAILLE", nom_post, K_eps_sortie_->valeurs(), K_eps_sortie_->unites(), K_eps_sortie_->noms_compo(), type_elem, temps);
      }
  return Modele_turbulence_hyd_base::imprimer(os);
}

