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
#include <Navier_Stokes_IBM.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Source_PDF_base.h>
#include <Assembleur_base.h>
#include <Probleme_base.h>
#include <Fluide_base.h>
#include <Debog.h>
#include <Param.h>
#include <Nom.h>

Implemente_instanciable(Navier_Stokes_IBM, "Navier_Stokes_IBM", Navier_Stokes_std);

// XD penalisation_l2_ftd_lec objet_lecture nul 0 not_set
// XD attr postraiter_gradient_pression_sans_masse entier postraiter_gradient_pression_sans_masse 1 (IBM advanced) avoid mass matrix multiplication for the gradient postprocessing
// XD attr correction_matrice_projection_initiale entier correction_matrice_projection_initiale 1 (IBM advanced) fix matrix of initial projection for PDF
// XD attr correction_calcul_pression_initiale entier correction_calcul_pression_initiale 1 (IBM advanced) fix initial pressure computation for PDF
// XD attr correction_vitesse_projection_initiale entier correction_vitesse_projection_initiale 1 (IBM advanced) fix initial velocity computation for PDF
// XD attr correction_matrice_pression entier correction_matrice_pression 1 (IBM advanced) fix pressure matrix for PDF
// XD attr matrice_pression_penalisee_H1 entier matrice_pression_penalisee_H1 1 (IBM advanced) fix pressure matrix for PDF
// XD attr correction_vitesse_modifie entier correction_vitesse_modifie 1 (IBM advanced) fix velocity for PDF
// XD attr correction_pression_modifie entier correction_pression_modifie 1 (IBM advanced) fix pressure for PDF
// XD attr gradient_pression_qdm_modifie entier gradient_pression_qdm_modifie 1 (IBM advanced) fix pressure gradient
// XD attr bord chaine bord 0 not_set
// XD attr val list val 0 not_set
// XD penalisation_l2_ftd listobj pp 1 penalisation_l2_ftd_lec 0 not_set

Sortie& Navier_Stokes_IBM::printOn(Sortie& is) const
{
  return Navier_Stokes_std::printOn(is);
}

Entree& Navier_Stokes_IBM::readOn(Entree& is)
{
  Navier_Stokes_std::readOn(is);
  readOn_ibm_proto(is, *this);
  return is;
}

void Navier_Stokes_IBM::set_param(Param& param)
{
  param.ajouter_flag("postraiter_gradient_pression_sans_masse", &postraiter_gradient_pression_sans_masse_, Param::OPTIONAL);
  param.ajouter("correction_matrice_projection_initiale", &correction_matrice_projection_initiale_, Param::OPTIONAL);
  param.ajouter("correction_calcul_pression_initiale", &correction_calcul_pression_initiale_, Param::OPTIONAL);
  param.ajouter("correction_vitesse_projection_initiale", &correction_vitesse_projection_initiale_, Param::OPTIONAL);
  param.ajouter("correction_matrice_pression", &correction_matrice_pression_, Param::OPTIONAL);
  param.ajouter("matrice_pression_penalisee_H1", &matrice_pression_penalisee_H1_, Param::OPTIONAL);
  param.ajouter("correction_vitesse_modifie", &correction_vitesse_modifie_, Param::OPTIONAL);
  param.ajouter("correction_pression_modifie", &correction_pression_modifie_, Param::OPTIONAL);
  param.ajouter("gradient_pression_qdm_modifie", &gradient_pression_qdm_modifie_, Param::OPTIONAL);
  Navier_Stokes_std::set_param(param);
  set_param_ibm_proto(param);
}

void Navier_Stokes_IBM::modify_initial_variable()
{
  correction_variable_initiale_ = correction_vitesse_projection_initiale_;
  modify_initial_variable_ibm_proto(la_vitesse->valeurs());
}

void Navier_Stokes_IBM::modify_initial_gradP(DoubleTrav& gradP)
{
  if (correction_vitesse_projection_initiale_ == 1)
    {
      Cerr << "(IBM) Immersed Interface: modified velocity corrector for initial projection." << finl;
      gradP /= champ_coeff_pdf_som_;
    }
}

// assemblage du systeme en pression
int Navier_Stokes_IBM::preparer_calcul()
{
  const double temps = schema_temps().temps_courant();
  sources().mettre_a_jour(temps);
  Equation_base::preparer_calcul();

  preparer_calcul_ibm_proto();

  bool is_dilatable = probleme().is_dilatable();
  Cerr << "(IBM) Immersed Interface: compute pressure matrix coefficients." << finl;
  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((sources())[i_source_pdf_].valeur());
  // if (src.getInterpolationBool())
  //   {
  //     src.calculer_variable_imposee();
  //   }
  if (correction_matrice_projection_initiale_ == 1)
    {
      Cerr << "(IBM) Immersed Interface: modified pressure matrix for initial projection." << finl;
      DoubleTab inv_coeff(champ_coeff_pdf_som_);
      inv_coeff = 1.;
      inv_coeff *= champ_coeff_pdf_som_;
      src.multiply_coeff_volume(inv_coeff);
      inv_coeff.echange_espace_virtuel();
      assembleur_pression()->assembler_mat(matrice_pression_, inv_coeff, 1, 1);
    }
  else
    {
      if (!is_dilatable)
        assembleur_pression()->assembler(matrice_pression_);
      else
        {
          Cerr << "Assembling for quasi-compressible" << finl;
          assembleur_pression()->assembler_QC(fluide().masse_volumique().valeurs(), matrice_pression_);
        }
    }

  // GF en cas de reprise on conserve la valeur de la pression
  // avant elle ne servait qu' a initialiser le lagrange pour la projection
  // C'est important pour le Simpler/Piso de bien repartir de la pression
  // sauvegardee...
  //la_pression->valeurs()=0.;
  Debog::verifier("Navier_Stokes_std::preparer_calcul, la_pression av projeter", la_pression->valeurs());
  if (projection_a_faire())
    projeter();

  // Au cas ou une cl de pression depend de u que l'on vient de modifier
  le_dom_Cl_dis->mettre_a_jour(temps);
  Debog::verifier("Navier_Stokes_std::preparer_calcul, la_pression ap projeter", la_pression->valeurs());

  // Initialisation du champ de pression (resolution de Laplacien(P)=0 avec les conditions limites en pression)
  // Permet de demarrer la resolution avec une bonne approximation de la pression (important pour le Piso ou P!=0)
  if (!probleme().reprise_effectuee() && methode_calcul_pression_initiale_ != 3)
    {
      Cout << "Estimation du champ de pression au demarrage:" << finl;
      DoubleTrav secmem(la_pression->valeurs());
      DoubleTrav vpoint(gradient_P->valeurs());
      gradient.calculer(la_pression->valeurs(), gradient_P->valeurs());
      vpoint -= gradient_P->valeurs();
      if ((correction_matrice_projection_initiale_ == 1) || (matrice_pression_penalisee_H1_ == 1))
        {
          Cerr << "(IBM) Immersed Interface: modified pressure gradient for initial pressure computation." << finl;
          vpoint /= champ_coeff_pdf_som_;
        }
      if (methode_calcul_pression_initiale_ >= 2)
        for (int op = 0; op < nombre_d_operateurs(); op++)
          operateur(op).ajouter(vpoint);
      if (methode_calcul_pression_initiale_ >= 1)
        {
          int mod = 0;
          if (le_schema_en_temps->pas_de_temps() == 0)
            {
              double dt = std::max(le_schema_en_temps->pas_temps_min(), calculer_pas_de_temps());
              dt = std::min(dt, le_schema_en_temps->pas_temps_max());
              le_schema_en_temps->set_dt() = (dt);
              mod = 1;
            }
          for (int i = 0; i < sources().size(); i++)
            {
              if (sources()(i).valeur().que_suis_je().find("Source_PDF") <= -1)
                {
                  sources()(i).ajouter(vpoint);
                }
            }
          if (projection_initiale == 0)
            {
              if ((correction_matrice_projection_initiale_ == 1) || (matrice_pression_penalisee_H1_ == 1))
                {
                  Cerr << "(IBM) Immersed Interface: Dirichlet velocity in initial pressure computation for PDF (if any)." << finl;
                  DoubleTrav secmem_pdf(vpoint);
                  src.calculer_pdf(secmem_pdf);
                  vpoint -= secmem_pdf;

                  DoubleTrav secmem_pdf_calculer(vpoint);
                  src.calculer(secmem_pdf_calculer);
                  vpoint += secmem_pdf_calculer;

                  vpoint.echange_espace_virtuel();
                }
            }

          if (mod)
            le_schema_en_temps->set_dt() = 0;
        }

      solveur_masse->appliquer(vpoint);
      vpoint.echange_espace_virtuel();
      divergence.calculer(vpoint, secmem);
      secmem *= -1;
      secmem.echange_espace_virtuel();

      assembleur_pression_->modifier_secmem_pour_incr_p(la_pression->valeurs(), 1, secmem);
      DoubleTrav inc_pre(la_pression->valeurs());
      solveur_pression_.resoudre_systeme(matrice_pression_.valeur(), secmem, inc_pre);
      Cerr << "Pressure increment computed successfully" << finl;

      if (correction_calcul_pression_initiale_ == 1)
        {
          Cerr << "(IBM) Immersed Interface: correction of initial pressure." << finl;
          src.correct_incr_pressure(champ_coeff_pdf_som_, inc_pre);
          inc_pre.echange_espace_virtuel();
        }

      // On veut que l'espace virtuel soit a jour, donc all_items
      operator_add(la_pression->valeurs(), inc_pre, VECT_ALL_ITEMS);
    }
  // Mise a jour pression
  la_pression->changer_temps(temps);
  calculer_la_pression_en_pa();
  // Calcul des forces de pression:
  gradient->calculer_flux_bords();

  // Calcul gradient_P (ToDo rendre coherent avec ::mettre_a_jour()):
  gradient.calculer(la_pression->valeurs(), gradient_P->valeurs());
  gradient_P->changer_temps(temps);

  // Calcul divergence_U
  divergence.calculer(la_vitesse->valeurs(), divergence_U->valeurs());
  divergence_U->changer_temps(temps);

  if (le_traitement_particulier.non_nul())
    le_traitement_particulier->preparer_calcul_particulier();

  Debog::verifier("Navier_Stokes_std::preparer_calcul, vitesse", inconnue());
  Debog::verifier("Navier_Stokes_std::preparer_calcul, pression", la_pression);

  DoubleTab coeff(champ_coeff_pdf_som_);
  coeff = 1.;
  if (matrice_pression_penalisee_H1_ == 1)
    {
      Cerr << "(IBM) Immersed Interface: H1 penalty of pressure matrix." << finl;
      coeff /= champ_coeff_pdf_som_;
    }
  else if (correction_matrice_pression_ == 1)
    {
      Cerr << "(IBM) Immersed Interface: modification of pressure matrix." << finl;
      coeff *= champ_coeff_pdf_som_;
      //Cerr<<"Min/max of coefficients: "<< mp_min_vect(champ_coeff_pdf_som_) << " " << mp_max_vect(champ_coeff_pdf_som_) <<finl;
    }
  src.multiply_coeff_volume(coeff);
  coeff.echange_espace_virtuel();
  assembleur_pression()->assembler_mat(matrice_pression(), coeff, 1, 1);

  return 1;
}

void Navier_Stokes_IBM::postraiter_gradient_pression_avec_masse()
{
  if (!postraiter_gradient_pression_sans_masse_)
    solveur_masse->appliquer(gradient_P->valeurs());
}

bool Navier_Stokes_IBM::initTimeStep(double dt)
{
  bool ddt = Navier_Stokes_std::initTimeStep(dt);

  initTimeStep_ibm_proto(dt);

  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((sources())[i_source_pdf_].valeur());
  src.updateChampRho();
  bool mat_var = src.get_matrice_pression_variable_bool_();
  if (mat_var == false)
    return ddt;
  Cerr << "(IBM) Immersed Interface: update of pressure matrix coefficents." << finl;
  DoubleTab coeff;
  coeff = src.compute_coeff_matrice();
  coeff.echange_espace_virtuel();
  set_champ_coeff_pdf_som(coeff);
  //Cerr<<"Min/max of coefficients: "<< mp_min_vect(coeff) << " " << mp_max_vect(coeff) <<finl;

  if ((correction_matrice_pression_ == 1) || (matrice_pression_penalisee_H1_ == 1))
    {
      Cerr << "(IBM) Immersed Interface: update of pressure matrix." << finl;
      DoubleTrav inv_coeff(champ_coeff_pdf_som_);
      inv_coeff = 1.;
      inv_coeff *= champ_coeff_pdf_som_;
      src.multiply_coeff_volume(inv_coeff);
      inv_coeff.echange_espace_virtuel();
      assembleur_pression()->assembler_mat(matrice_pression(), inv_coeff, 1, 1);
    }

  return ddt;
}

// ajoute les contributions des operateurs et des sources
void Navier_Stokes_IBM::assembler(Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu)
{
  assembler_ibm_proto(matrice, inco, resu);
}

// for IBM methods; on ajoute source PDF au RHS
void Navier_Stokes_IBM::derivee_en_temps_inco_sources(DoubleTrav& secmem)
{
  derivee_en_temps_inco_ibm_proto(secmem);
}

void Navier_Stokes_IBM::verify_scheme()
{
  // for IBM methods
  if ( equation_non_resolue() == 0)
    {
      Cerr<<"*******(IBM) Use an explicit time scheme (at least Euler explicit + diffusion) with Source_PDF_base.*******"<<finl;
      abort();
    }
}
