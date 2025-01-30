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
#include <Navier_Stokes_IBM_impl.h>
#include <Navier_Stokes_std.h>
#include <Source_PDF_base.h>
#include <Assembleur_base.h>
#include <Fluide_base.h>
#include <Param.h>
#include <Nom.h>

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

void Navier_Stokes_IBM_impl::set_param_IBM_NS(Param& param)
{
  param.ajouter_flag("postraiter_gradient_pression_sans_masse",&postraiter_gradient_pression_sans_masse_,Param::OPTIONAL);
  param.ajouter("correction_matrice_projection_initiale",&correction_matrice_projection_initiale_,Param::OPTIONAL);
  param.ajouter("correction_calcul_pression_initiale",&correction_calcul_pression_initiale_,Param::OPTIONAL);
  param.ajouter("correction_vitesse_projection_initiale",&correction_vitesse_projection_initiale_,Param::OPTIONAL);
  param.ajouter("correction_matrice_pression",&correction_matrice_pression_,Param::OPTIONAL);
  param.ajouter("matrice_pression_penalisee_H1",&matrice_pression_penalisee_H1_,Param::OPTIONAL);
  param.ajouter("correction_vitesse_modifie",&correction_vitesse_modifie_,Param::OPTIONAL);
  param.ajouter("correction_pression_modifie",&correction_pression_modifie_,Param::OPTIONAL);
  param.ajouter("gradient_pression_qdm_modifie",&gradient_pression_qdm_modifie_,Param::OPTIONAL);
}

Entree& Navier_Stokes_IBM_impl::readOn_IBM_NS(Entree& is, Navier_Stokes_std& eq)
{
  eq_NS = eq;
  return is;
}

void Navier_Stokes_IBM_impl::modify_initial_gradP_IBM_NS(DoubleTrav& gradP)
{
  if (correction_vitesse_projection_initiale_==1)
    {
      Cerr<<"(IBM) Immersed Interface: modified velocity corrector for initial projection."<<finl;
      const DoubleTab champ_coeff_pdf_som_ = eq_NS->get_champ_coeff_pdf_som();
      gradP /= champ_coeff_pdf_som_;
    }
}

void Navier_Stokes_IBM_impl::reprise_calcul_IBM_NS(DoubleTrav& vpoint)
{
  if ((correction_matrice_projection_initiale_==1)||(matrice_pression_penalisee_H1_==1))
    {
      Cerr<<"(IBM) Immersed Interface: modified pressure gradient for initial pressure computation."<<finl;
      const DoubleTab champ_coeff_pdf_som_ = eq_NS->get_champ_coeff_pdf_som();
      vpoint /= champ_coeff_pdf_som_;
    }
}

void Navier_Stokes_IBM_impl::pression_initiale_IBM_NS( DoubleTrav& vpoint )
{
  if ((correction_matrice_projection_initiale_==1)||(matrice_pression_penalisee_H1_==1))
    {
      const int i_source_pdf_ = eq_NS->get_i_source_pdf();
      const Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
      Cerr<<"(IBM) Immersed Interface: Dirichlet velocity in initial pressure computation for PDF (if any)."<<finl;
      DoubleTrav secmem_pdf(vpoint);
      src.calculer_pdf(secmem_pdf);
      vpoint -= secmem_pdf;

      DoubleTrav secmem_pdf_calculer(vpoint);
      src.calculer(secmem_pdf_calculer);
      vpoint += secmem_pdf_calculer;

      vpoint.echange_espace_virtuel();
    }
}

void Navier_Stokes_IBM_impl::pression_correction_IBM_NS( DoubleTrav& inc_pre )
{
  if (correction_calcul_pression_initiale_ == 1)
    {
      const int i_source_pdf_ = eq_NS->get_i_source_pdf();
      const DoubleTab champ_coeff_pdf_som_ = eq_NS->get_champ_coeff_pdf_som();
      Cerr<<"(IBM) Immersed Interface: correction of initial pressure."<<finl;
      const Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
      src.correct_incr_pressure(champ_coeff_pdf_som_, inc_pre);
      inc_pre.echange_espace_virtuel();
    }
}

void Navier_Stokes_IBM_impl::matrice_pression_IBM_NS( )
{
  const DoubleTab champ_coeff_pdf_som_ = eq_NS->get_champ_coeff_pdf_som();
  DoubleTab coeff(champ_coeff_pdf_som_);
  coeff = 1.;
  if (matrice_pression_penalisee_H1_==1)
    {
      Cerr<<"(IBM) Immersed Interface: H1 penalty of pressure matrix."<<finl;
      coeff /= champ_coeff_pdf_som_;
    }
  else if (correction_matrice_pression_==1)
    {
      Cerr<<"(IBM) Immersed Interface: modification of pressure matrix."<<finl;
      coeff *= champ_coeff_pdf_som_;
      //Cerr<<"Min/max of coefficients: "<< mp_min_vect(champ_coeff_pdf_som_) << " " << mp_max_vect(champ_coeff_pdf_som_) <<finl;
    }
  const int i_source_pdf_ = eq_NS->get_i_source_pdf();
  const Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
  src.multiply_coeff_volume(coeff);
  coeff.echange_espace_virtuel();
  eq_NS->assembleur_pression()->assembler_mat(eq_NS->matrice_pression(),coeff,1,1);
}


void Navier_Stokes_IBM_impl::preparer_calcul_IBM_NS(const bool& is_QC)
{
  const int i_source_pdf_ = eq_NS->get_i_source_pdf();
  Cerr<<"(IBM) Immersed Interface: compute pressure matrix coefficients."<<finl;
  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
  // if (src.getInterpolationBool())
  //   {
  //     src.calculer_variable_imposee();
  //   }
  if (correction_matrice_projection_initiale_==1)
    {
      Cerr<<"(IBM) Immersed Interface: modified pressure matrix for initial projection."<<finl;
      const DoubleTab champ_coeff_pdf_som_ = eq_NS->get_champ_coeff_pdf_som();
      DoubleTab inv_coeff(champ_coeff_pdf_som_);
      inv_coeff = 1. ;
      inv_coeff *= champ_coeff_pdf_som_;
      src.multiply_coeff_volume(inv_coeff);
      inv_coeff.echange_espace_virtuel();
      eq_NS->assembleur_pression()->assembler_mat(eq_NS->matrice_pression(),inv_coeff,1,1);
    }
  else
    {
      if (!is_QC)
        {
          eq_NS->assembleur_pression()->assembler(eq_NS->matrice_pression());
        }
      else
        {
          Cerr<<"Assembling for quasi-compressible"<<finl;
          eq_NS->assembleur_pression()->assembler_QC(eq_NS->fluide().masse_volumique().valeurs(),eq_NS->matrice_pression());
        }
    }
}

bool Navier_Stokes_IBM_impl::initTimeStep_IBM_NS(bool ddt)
{
  const int i_source_pdf_ = eq_NS->get_i_source_pdf();
  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
  src.updateChampRho();
  bool mat_var = src.get_matrice_pression_variable_bool_();
  if (mat_var == false) return  ddt;
  Cerr << "(IBM) Immersed Interface: update of pressure matrix coefficents."<<finl;
  DoubleTab coeff;
  coeff = src.compute_coeff_matrice();
  coeff.echange_espace_virtuel();
  eq_NS->set_champ_coeff_pdf_som(coeff);
  //Cerr<<"Min/max of coefficients: "<< mp_min_vect(coeff) << " " << mp_max_vect(coeff) <<finl;

  if ((correction_matrice_pression_==1) || (matrice_pression_penalisee_H1_==1))
    {
      Cerr<<"(IBM) Immersed Interface: update of pressure matrix."<<finl;
      DoubleTab inv_coeff(eq_NS->get_champ_coeff_pdf_som());
      inv_coeff = 1.;
      inv_coeff *= eq_NS->get_champ_coeff_pdf_som();
      src.multiply_coeff_volume(inv_coeff);
      inv_coeff.echange_espace_virtuel();
      eq_NS->assembleur_pression()->assembler_mat(eq_NS->matrice_pression(),inv_coeff,1,1);
    }

  return ddt;
}


