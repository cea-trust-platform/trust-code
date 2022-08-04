/****************************************************************************
* Copyright (c) 2022, CEA
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

void Navier_Stokes_IBM_impl::set_param_IBM(Param& param)
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

Entree& Navier_Stokes_IBM_impl::readOn_IBM(Entree& is, Navier_Stokes_std& eq)
{
  eq_NS = eq;

  int i_source = -1;
  int nb_source_pdf = 0;
  int nb_source  = eq.sources().size();
  Cerr << "nb_source : " << nb_source << finl;
  for(int i = 0; i<nb_source; i++)
    {
      //Cerr << (sources())[i].valeur().que_suis_je() << ", " << (sources())[i].valeur().que_suis_je().find("gne") << finl;
      if ((eq.sources())[i].valeur().que_suis_je().find("Source_PDF") > -1)
        {
          i_source = i;
          nb_source_pdf++;
        }
    }
  if (nb_source_pdf>1)
    {
      Cerr<<"(IBM) More than one Source_PDF_base detected."<<finl;
      Cerr<<"(IBM) Case not supported."<<finl;
      Cerr<<"Aborting..."<<finl;
      abort();
    }
  i_source_pdf_ = i_source;
  if (i_source_pdf_ != -1)
    {
      eq.sources()[i_source_pdf_].valeur().set_description((Nom)"Sum of the source term on the obstacle (~= force induced by the obstacle under some assumptions)");
      eq.sources()[i_source_pdf_].valeur().set_fichier((Nom)"Force_induced_by_obstacle");
    }
  return is;
}

void Navier_Stokes_IBM_impl::modify_initial_velocity_IBM(DoubleTab& tab_vitesse)
{
  if (correction_vitesse_projection_initiale_==1)
    {
      Cerr<<"(IBM) Immersed Interface: modified initial velocity for initial projection."<<finl;
      const Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
      src.correct_vitesse(champ_coeff_pdf_som_, tab_vitesse);
      tab_vitesse.echange_espace_virtuel();
    }
}

void Navier_Stokes_IBM_impl::modify_initial_gradP_IBM(DoubleTrav& gradP)
{
  if (correction_vitesse_projection_initiale_==1)
    {
      Cerr<<"(IBM) Immersed Interface: modified velocity corrector for initial projection."<<finl;
      gradP /= champ_coeff_pdf_som_;
    }
}

void Navier_Stokes_IBM_impl::reprise_calcul_IBM(DoubleTrav& vpoint)
{
  if ((correction_matrice_projection_initiale_==1)||(matrice_pression_penalisee_H1_==1))
    {
      Cerr<<"(IBM) Immersed Interface: modified pressure gradient for initial pressure computation."<<finl;
      vpoint /= champ_coeff_pdf_som_;
    }
}

void Navier_Stokes_IBM_impl::pression_initiale_IBM( DoubleTrav& vpoint )
{
  if ((correction_matrice_projection_initiale_==1)||(matrice_pression_penalisee_H1_==1))
    {
      const Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
      Cerr<<"Immersed Interface: Dirichlet velocity in initial pressure computation for PDF (if any)."<<finl;
      DoubleTrav secmem_pdf(vpoint);
      src.calculer_pdf(secmem_pdf);
      vpoint -= secmem_pdf;
      vpoint.echange_espace_virtuel();
    }
}

void Navier_Stokes_IBM_impl::pression_correction_IBM( DoubleTrav& inc_pre )
{
  if (correction_calcul_pression_initiale_ == 1)
    {
      Cerr<<"(IBM) Immersed Interface: correction of initial pressure."<<finl;
      const Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
      src.correct_incr_pressure(champ_coeff_pdf_som_, inc_pre);
      inc_pre.echange_espace_virtuel();
    }
}

void Navier_Stokes_IBM_impl::matrice_pression_IBM( )
{
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
  const Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
  src.multiply_coeff_volume(coeff);
  coeff.echange_espace_virtuel();
  eq_NS->assembleur_pression().valeur().assembler_mat(eq_NS->matrice_pression(),coeff,1,1);
}


void Navier_Stokes_IBM_impl::preparer_calcul_IBM(const Nom& nom_eq,const Nom& nom_eq2)
{
  Cerr<<"(IBM) Immersed Interface: compute pressure matrix coefficients."<<finl;
  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
  src.updateChampRho();
  // if (src.getInterpolationBool())
  //   {
  //     src.calculer_vitesse_imposee();
  //   }
  DoubleTab coeff;
  coeff = src.compute_coeff_matrice_pression();
  coeff.echange_espace_virtuel();
  //Cerr<<"Min/max : "<< mp_min_vect(coeff) << " " << mp_max_vect(coeff) <<finl;
  champ_coeff_pdf_som_ = coeff;
  if (correction_matrice_projection_initiale_==1)
    {
      Cerr<<"(IBM) Immersed Interface: modified pressure matrix for initial projection."<<finl;
      DoubleTab inv_coeff(champ_coeff_pdf_som_);
      inv_coeff = 1. ;
      inv_coeff *= champ_coeff_pdf_som_;
      src.multiply_coeff_volume(inv_coeff);
      inv_coeff.echange_espace_virtuel();
      eq_NS->assembleur_pression().valeur().assembler_mat(eq_NS->matrice_pression(),inv_coeff,1,1);
    }
  else
    {
      if ((nom_eq2==nom_eq))
        {
          eq_NS->assembleur_pression().assembler(eq_NS->matrice_pression());
        }
      else
        {
          Cerr<<"Assembling for quasi-compressible"<<finl;
          eq_NS->assembleur_pression().assembler_QC(eq_NS->fluide().masse_volumique().valeurs(),eq_NS->matrice_pression());
        }
    }
}

bool Navier_Stokes_IBM_impl::initTimeStep_IBM(bool ddt)
{
  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_NS->sources())[i_source_pdf_].valeur());
  src.updateChampRho();
  src.calculer_vitesse_imposee();
  bool mat_var = src.get_matrice_pression_variable_bool_();
  if (mat_var == false) return  ddt;
  Cerr << "(IBM) Immersed Interface: update of pressure matrix coefficents."<<finl;
  champ_coeff_pdf_som_ = src.compute_coeff_matrice_pression();
  champ_coeff_pdf_som_.echange_espace_virtuel();
  //Cerr<<"Min/max of coefficients: "<< mp_min_vect(champ_coeff_pdf_som_) << " " << mp_max_vect(champ_coeff_pdf_som_) <<finl;

  if ((correction_matrice_pression_==1) || (matrice_pression_penalisee_H1_==1))
    {
      Cerr<<"(IBM) Immersed Interface: update of pressure matrix."<<finl;
      DoubleTab inv_coeff(champ_coeff_pdf_som_);
      inv_coeff = 1.;
      inv_coeff *= champ_coeff_pdf_som_;
      src.multiply_coeff_volume(inv_coeff);
      inv_coeff.echange_espace_virtuel();
      eq_NS->assembleur_pression().valeur().assembler_mat(eq_NS->matrice_pression(),inv_coeff,1,1);
    }

  return ddt;
}


