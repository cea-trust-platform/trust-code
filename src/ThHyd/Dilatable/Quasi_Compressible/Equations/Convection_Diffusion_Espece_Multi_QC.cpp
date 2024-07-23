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

#include <Convection_Diffusion_Espece_Multi_QC.h>
#include <Fluide_Quasi_Compressible.h>
#include <Neumann_sortie_libre.h>
#include <Loi_Etat_Multi_GP_QC.h>
#include <Discretisation_base.h>
#include <Navier_Stokes_QC.h>
#include <Probleme_base.h>
#include <Statistiques.h>
#include <Dirichlet.h>
#include <TRUSTTrav.h>
#include <EChaine.h>
#include <Param.h>

extern Stat_Counter_Id assemblage_sys_counter_;
extern Stat_Counter_Id source_counter_;

Implemente_instanciable(Convection_Diffusion_Espece_Multi_QC,"Convection_Diffusion_Espece_Multi_QC",Convection_Diffusion_Espece_Multi_base);
// XD convection_diffusion_espece_multi_QC eqn_base convection_diffusion_espece_multi_QC -1 Species conservation equation for a multi-species quasi-compressible fluid.

Sortie& Convection_Diffusion_Espece_Multi_QC::printOn(Sortie& is) const
{
  return Convection_Diffusion_Espece_Multi_base::printOn(is);
}

Entree& Convection_Diffusion_Espece_Multi_QC::readOn(Entree& is)
{
  return Convection_Diffusion_Espece_Multi_base::readOn(is);
}

void Convection_Diffusion_Espece_Multi_QC::set_param(Param& param)
{
  Convection_Diffusion_Espece_Multi_base::set_param(param);
  param.ajouter("espece",&mon_espece_); // XD_ADD_P espece Assosciate a species (with its properties) to the equation
}

int Convection_Diffusion_Espece_Multi_QC::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;
      //associe mu_sur_Sc dans la diffusivite
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      ref_cast_non_const(Champ_base,terme_diffusif.diffusivite()).nommer("mu_sur_Schmidt");
      is >> terme_diffusif;
      // Il faut appeler associer_diffusivite_pour_pas_de_temps
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(diffusivite_pour_pas_de_temps());
      return 1;
    }
  else if (mot=="convection")
    {
      const Probleme_base& pb = probleme();
      const Champ_Inc& vit_transportante = ref_cast(Navier_Stokes_QC,pb.equation(0)).rho_la_vitesse();
      associer_vitesse(vit_transportante);
      terme_convectif.associer_vitesse(vit_transportante);
      is >> terme_convectif;
      terme_convectif.associer_vitesse(vit_transportante);
      return 1;
    }
  else
    return Convection_Diffusion_Espece_Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);
}

const Champ_base& Convection_Diffusion_Espece_Multi_QC::diffusivite_pour_pas_de_temps() const
{
  // TODO : FIXME : on passe actuellement en parametre mu_sur_Schmidt qu il faut remplacer par nu_sur_Schmidt
  return le_fluide->mu_sur_Schmidt();
}

/*! @brief Associe l inconnue de l equation a la loi d etat,
 *
 */
void Convection_Diffusion_Espece_Multi_QC::completer()
{
  Convection_Diffusion_Espece_Multi_base::completer();
  Fluide_Quasi_Compressible& le_fluideQC = ref_cast(Fluide_Quasi_Compressible, fluide());
  Loi_Etat_Multi_GP_QC& loi_etat = ref_cast_non_const(Loi_Etat_Multi_GP_QC, le_fluideQC.loi_etat().valeur());
  loi_etat.associer_inconnue(l_inco_ch.valeur());
  loi_etat.associer_espece(*this);

  // remplissage du domaine cl modifiee avec 1 partout au bord...
  zcl_modif_ = domaine_Cl_dis();

  Conds_lim& condlims = zcl_modif_->les_conditions_limites();
  int nb = condlims.size();
  for (int i = 0; i < nb; i++)
    {
      // pour chaque condlim on recupere le champ_front et on met 1 meme si la cond lim est un flux (dans ce cas la convection restera nulle.)
      if (sub_type(Neumann_sortie_libre, condlims[i].valeur()))
        {
          ref_cast(Neumann_sortie_libre,condlims[i].valeur()).tab_ext() = 1;
        }
      if (sub_type(Dirichlet, condlims[i].valeur()))
        {
          const Frontiere_dis_base& frdis = condlims[i]->frontiere_dis();
          EChaine toto(" Champ_front_uniforme 1 1");
          toto >> condlims[i].valeur();
          condlims[i]->associer_fr_dis_base(frdis);
        }
      DoubleTab& T = condlims[i]->champ_front().valeurs();
      T = 1.;
    }
}

/*! @brief Renvoie la derivee en temps de l'inconnue de l'equation.
 *
 * Le calcul est le suivant:
 *          d(inconnue)/dt = M^{-1} * (sources - somme(Op_{i}(inconnue))) / rho
 *
 * @param (DoubleTab& derivee) le tableau des valeurs de la derivee en temps du champ inconnu
 * @return (DoubleTab&) le tableau des valeurs de la derivee en temps du champ inconnu
 */
DoubleTab& Convection_Diffusion_Espece_Multi_QC::derivee_en_temps_inco(DoubleTab& derivee)
{
  derivee = 0;

  les_sources.ajouter(derivee);
  solveur_masse.appliquer(derivee);
  DoubleTrav derivee_bis(derivee);

  // on commence par retirer phi*div(1 U)
  const DoubleTab& frac_mass = inconnue().valeurs();

  Convection_Diffusion_Fluide_Dilatable_Proto::calculer_div_rho_u_impl(derivee_bis,*this);

  int n = frac_mass.dimension_tot(0);
  for (int i = 0; i < n; i++)
    derivee_bis(i) = -derivee_bis(i) * frac_mass(i);

  // suite + standard
  operateur(1).ajouter(derivee_bis);
  operateur(0).ajouter(derivee_bis);

  solveur_masse->set_name_of_coefficient_temporel("masse_volumique");
  solveur_masse.appliquer(derivee_bis);
  solveur_masse->set_name_of_coefficient_temporel("no_coeff");
  derivee += derivee_bis;
  return derivee;
}

void Convection_Diffusion_Espece_Multi_QC::assembler(Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu)
{
  resu = 0;
  const IntVect& tab1 = matrice.get_tab1();
  DoubleVect& coeff = matrice.get_set_coeff();

  const DoubleTab& rho = get_champ("masse_volumique").valeurs();
  operateur(0).l_op_base().contribuer_a_avec(inco, matrice);
  operateur(0).ajouter(resu);
  int ndl = rho.dimension(0);

  // on retire Divu1 *inco
  DoubleTrav divu1(inco);
  Convection_Diffusion_Fluide_Dilatable_Proto::calculer_div_rho_u_impl(divu1,*this);

  // ajout de la convection
  operateur(1).l_op_base().contribuer_a_avec(inco, matrice);
  operateur(1).ajouter(resu);

  for (int i = 0; i < ndl; i++)
    {
      resu(i) -= divu1(i) * inco(i);
      matrice(i, i) += divu1(i);
    }
  // on divise par rho chaque ligne
  for (int som = 0; som < ndl; som++)
    {
      double inv_rho = 1 / rho(som);
      for (int k = tab1(som) - 1; k < tab1(som + 1) - 1; k++)
        coeff(k) *= inv_rho;
      resu(som) *= inv_rho;
    }

  les_sources.contribuer_a_avec(inco, matrice);
  les_sources.ajouter(resu);

  int test_op = 0;
  {
    char *theValue = getenv("TRUST_TEST_OPERATEUR_IMPLICITE_BLOQUANT");
    if (theValue != nullptr)
      test_op = 1;
  }

  if (test_op)
    {
      DoubleTab test(resu);
      DoubleTab test2(resu);
      DoubleTrav resu2(resu);
      derivee_en_temps_inco(resu2);
      solveur_masse.appliquer(test2);
      resu2 -= test2;
      Cerr << " here " << mp_max_abs_vect(resu2) << finl;
      matrice.ajouter_multvect(inco, test);
      solveur_masse.appliquer(test);
      const double max_test = mp_max_abs_vect(test);
      Cerr << "iii " << max_test << finl;

      if (max_test > 0)
        {

          for (int i = 0; i < resu.size(); i++)
            if (std::fabs(test(i)) > 1e-5)
              Cerr << i << " " << test(i) << finl;
          Process::exit();
        }
    }
  matrice.ajouter_multvect(inco, resu);
}

void Convection_Diffusion_Espece_Multi_QC::assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl)
{
  statistiques().begin_count(assemblage_sys_counter_);
  const std::string& nom_inco = inconnue().le_nom().getString();
  const DoubleTab& inco = inconnue().valeurs();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;

  secmem = 0;
  const IntVect& tab1 = mat->get_tab1();

  DoubleVect& coeff = mat->get_set_coeff();

  const DoubleTab& rho = get_champ("masse_volumique").valeurs();
  operateur(0).l_op_base().ajouter_blocs(matrices, secmem, semi_impl);

  int ndl = rho.dimension(0);
  // on divise par rho chaque ligne
  for (int som = 0; som < ndl; som++)
    {
      double inv_rho = 1 / rho(som);
      for (int k = tab1(som) - 1; k < tab1(som + 1) - 1; k++)
        coeff(k) *= inv_rho;
      secmem(som) *= inv_rho;
    }

  // ajout de la convection
  operateur(1).l_op_base().ajouter_blocs(matrices, secmem, semi_impl);

  // on retire Divu1 *inco
  DoubleTrav divu1(inco);
  Convection_Diffusion_Fluide_Dilatable_Proto::calculer_div_rho_u_impl(divu1,*this);

  for (int i = 0; i < ndl; i++)
    {
      secmem(i) -= divu1(i) * inco(i);
      if (mat)
        (*mat)(i, i) += divu1(i);
    }
  statistiques().end_count(assemblage_sys_counter_);

  statistiques().begin_count(source_counter_);
  for (int i = 0; i < sources().size(); i++)
    sources()(i)->ajouter_blocs(matrices, secmem, semi_impl);
  statistiques().end_count(source_counter_);

  statistiques().begin_count(assemblage_sys_counter_);
  if (mat)
    mat->ajouter_multvect(inco, secmem);

  schema_temps().ajouter_blocs(matrices, secmem, *this);

  if (!discretisation().is_polymac_family())
    modifier_pour_Cl(*mat, secmem);

  statistiques().end_count(assemblage_sys_counter_);
}
