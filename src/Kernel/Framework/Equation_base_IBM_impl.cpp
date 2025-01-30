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

#include <Equation_base.h>
#include <Equation_base_IBM_impl.h>
#include <Schema_Temps_base.h>
#include <Source_PDF_base.h>
#include <Param.h>
#include <Nom.h>

void Equation_base_IBM_impl::set_param_IBM(Param& param)
{
  param.ajouter("correction_variable_initiale",&correction_variable_initiale_,Param::OPTIONAL);
}

Entree& Equation_base_IBM_impl::readOn_IBM(Entree& is, Equation_base& eq)
{
  eq_IBM = eq;

  int i_source = -1;
  int nb_source_pdf = 0;
  int nb_source  = eq.sources().size();
  Cerr << "nb_source : " << nb_source << finl;
  for(int i = 0; i<nb_source; i++)
    {
      //Cerr << (sources())[i]->que_suis_je() << ", " << (sources())[i]->que_suis_je().find("gne") << finl;
      if ((eq.sources())[i]->que_suis_je().find("Source_PDF") > -1)
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
      Cerr<<"(IBM) Immersed Interface with Source_PDF_base for equation : "<< eq_IBM->le_nom()<<finl;
      eq.sources()[i_source_pdf_]->set_description((Nom)"Sum of the PDF source term on the obstacle (~= force/power induced by the obstacle under some assumptions)");
      Nom the_suffix = eq_IBM->le_nom().getSuffix("pb");
      eq.sources()[i_source_pdf_]->set_fichier((Nom)the_suffix+"_Bilan_term_induced_by_obstacle");
    }

  return is;
}

bool Equation_base_IBM_impl::initTimeStep_IBM(double ddt)
{
  if (i_source_pdf_ == -1) return false;
  Cerr<<"(IBM) initTimeStep_IBM: update of immersed values for equation : "<< eq_IBM->le_nom() <<finl;
  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_IBM->sources())[i_source_pdf_].valeur());
  src.calculer_variable_imposee();
  return true;
}

DoubleTab& Equation_base_IBM_impl::derivee_en_temps_inco_IBM(DoubleTab& derivee)
{
  if (i_source_pdf_ == -1) return derivee;
  Cerr<<"(IBM) Immersed Interface: Dirichlet value in Equation for PDF (if any)."<<finl;
  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_IBM->sources())[i_source_pdf_].valeur());

  // Terme PDF IB value : -rho/delta_t  ksi_gamma/epsilon U_gamma
  DoubleTab secmem_pdf(derivee);
  src.calculer_pdf(secmem_pdf);
  derivee -= secmem_pdf;
  derivee.echange_espace_virtuel();

  // Terme en temps : -rho/delta_t ksi_gamma Un
  int pdf_bilan = src.get_modele().pdf_bilan();
  if (pdf_bilan == 1|| pdf_bilan == 2)
    {
      int i_traitement_special = 101;
      int nb_comp = derivee.dimension(1);
      if (nb_comp == Objet_U::dimension)
        {
          //vector; NS
          // Navier_Stokes_std& eq_NS = ref_cast_non_const(Navier_Stokes_std, eq_IBM);
          if (eq_IBM->nombre_d_operateurs() > 1)
            {
              // if (eq_NS.vitesse_pour_transport().le_nom()=="rho_u") i_traitement_special = 1;
              i_traitement_special = 101;
            }
        }
      else if (nb_comp == 1)
        {
          //scalar; terme temps en rho
          i_traitement_special = 1;
        }
      else
        {
          Cerr << "Equation_base_IBM_impl: for scalar or vector only; dim = " <<nb_comp<< finl;
          Process::exit();
        }

      DoubleTrav secmem_pdf_time(derivee);
      src.calculer(secmem_pdf_time, i_traitement_special);
      secmem_pdf += secmem_pdf_time;
    }
  else if(pdf_bilan != 0 )
    {
      Cerr<<"Source_PDF_EF: Modele pdf_bilan must be 0; 1 or 2 only"<<endl;
      Process::exit();
    }

  // Sauvegarde de secmem_pdf
  secmem_pdf.echange_espace_virtuel();
  src.set_sec_mem_pdf(secmem_pdf);

  return derivee;
}

void Equation_base_IBM_impl::preparer_calcul_IBM()
{
  if (i_source_pdf_ == -1) return;

  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_IBM->sources())[i_source_pdf_].valeur());
  src.updateChampRho();
  DoubleTab coeff;
  coeff = src.compute_coeff_matrice();
  coeff.echange_espace_virtuel();
  Cerr<<"(IBM) preparer_calcul_IBM: Min/max coeff : "<< mp_min_vect(coeff) << " " << mp_max_vect(coeff) <<finl;
  champ_coeff_pdf_som_ = coeff;
  modify_initial_variable_IBM(eq_IBM->inconnue().valeurs());
}

void Equation_base_IBM_impl::modify_initial_variable_IBM(DoubleTab& variable)
{
  if (correction_variable_initiale_==1)
    {
      Cerr<<"(IBM) Immersed Interface: modified initial variable."<<finl;
      const Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_IBM->sources())[i_source_pdf_].valeur());
      src.correct_variable(champ_coeff_pdf_som_, variable);
      variable.echange_espace_virtuel();
    }
}
