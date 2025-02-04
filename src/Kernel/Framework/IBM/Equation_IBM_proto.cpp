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

#include <Discretisation_base.h>
#include <Equation_IBM_proto.h>
#include <Schema_Temps_base.h>
#include <Source_PDF_base.h>
#include <Operateur_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Operateur.h>
#include <Param.h>
#include <Nom.h>

void Equation_IBM_proto::set_param_ibm_proto(Param& param)
{
  param.ajouter("correction_variable_initiale",&correction_variable_initiale_,Param::OPTIONAL);
}

Entree& Equation_IBM_proto::readOn_ibm_proto(Entree& is, Equation_base& eq)
{
  eq_IBM_ = eq;

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
      Cerr<<"(IBM) Immersed Interface with Source_PDF_base for equation : "<< eq_IBM_->le_nom()<<finl;
      eq.sources()[i_source_pdf_]->set_description((Nom)"Sum of the PDF source term on the obstacle (~= force/power induced by the obstacle under some assumptions)");
      Nom the_suffix = eq_IBM_->le_nom().getSuffix("pb");
      eq.sources()[i_source_pdf_]->set_fichier((Nom)the_suffix+"_Bilan_term_induced_by_obstacle");
    }

  if (i_source_pdf_ == -1)
    {
      Cerr << "No PDF source term read in your equation " << eq_IBM_->que_suis_je() << " !!!"<< finl;
      Cerr << "Either add this source term or use a non_IBM problem !!!"<< finl;
      Process::exit();
    }

  return is;
}

bool Equation_IBM_proto::initTimeStep_ibm_proto(double ddt)
{
  if (i_source_pdf_ == -1) return false;
  Cerr<<"(IBM) initTimeStep_IBM: update of immersed values for equation : "<< eq_IBM_->le_nom() <<finl;
  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_IBM_->sources())[i_source_pdf_].valeur());
  src.calculer_variable_imposee();
  return true;
}

DoubleTab& Equation_IBM_proto::derivee_en_temps_inco_ibm_proto(DoubleTab& derivee)
{
  if (i_source_pdf_ == -1) return derivee;
  Cerr<<"(IBM) Immersed Interface: Dirichlet value in Equation for PDF (if any)."<<finl;
  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_IBM_->sources())[i_source_pdf_].valeur());

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
          if (eq_IBM_->nombre_d_operateurs() > 1)
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
          Cerr << "Equation_IBM_proto: for scalar or vector only; dim = " <<nb_comp<< finl;
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

void Equation_IBM_proto::preparer_calcul_ibm_proto()
{
  if (i_source_pdf_ == -1) return;

  Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_IBM_->sources())[i_source_pdf_].valeur());
  src.updateChampRho();
  DoubleTab coeff;
  coeff = src.compute_coeff_matrice();
  coeff.echange_espace_virtuel();
  Cerr<<"(IBM) preparer_calcul_IBM: Min/max coeff : "<< mp_min_vect(coeff) << " " << mp_max_vect(coeff) <<finl;
  champ_coeff_pdf_som_ = coeff;
  modify_initial_variable_ibm_proto(eq_IBM_->inconnue().valeurs());
}

void Equation_IBM_proto::modify_initial_variable_ibm_proto(DoubleTab& variable)
{
  if (correction_variable_initiale_==1)
    {
      Cerr<<"(IBM) Immersed Interface: modified initial variable."<<finl;
      const Source_PDF_base& src = dynamic_cast<Source_PDF_base&>((eq_IBM_->sources())[i_source_pdf_].valeur());
      src.correct_variable(champ_coeff_pdf_som_, variable);
      variable.echange_espace_virtuel();
    }
}

// ajoute les contributions des operateurs et des sources
void Equation_IBM_proto::assembler_ibm_proto(Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu)
{
  const double rhoCp = eq_IBM_->get_time_factor();
  int size_s = eq_IBM_->sources().size();

  // Test de verification de la methode contribuer_a_avec
  for (int op=0; op<eq_IBM_->nombre_d_operateurs(); op++)
    eq_IBM_->operateur(op).l_op_base().tester_contribuer_a_avec(inco, matrice);

  // Contribution des operateurs et des sources:
  // [Vol/dt+A]Inco(n+1)=somme(residu)+Vol/dt*Inco(n)
  // Typiquement: si Op=flux(Inco) alors la matrice implicite A contient une contribution -dflux/dInco
  // Exemple: Op flux convectif en VDF:
  // Op=T*u*S et A=-d(T*u*S)/dT=-u*S
  const Discretisation_base::type_calcul_du_residu& type_codage=eq_IBM_->probleme().discretisation().codage_du_calcul_du_residu();
  if (type_codage==Discretisation_base::VIA_CONTRIBUER_AU_SECOND_MEMBRE)
    {
      if ( eq_IBM_->probleme().discretisation().que_suis_je() == "EF")
        {
          // On calcule somme(residu) par contribuer_au_second_membre (typiquement CL non implicitees)
          // Cette approche necessite de coder 3 methodes (contribuer_a_avec, contribuer_au_second_membre et ajouter pour l'explicite)
          for (int i = 0; i < size_s; i++)
            {
              if (eq_IBM_->sources()(i).valeur().que_suis_je().find("Source_PDF") <= -1)
                {
                  const Source_base& src_base = eq_IBM_->sources()(i).valeur();
                  src_base.contribuer_a_avec(inco,matrice);
                }
            }
          statistiques().end_count(assemblage_sys_counter_,0,0);
          for (int i = 0; i < size_s; i++)
            {
              if (eq_IBM_->sources()(i).valeur().que_suis_je().find("Source_PDF") <= -1)
                {
                  eq_IBM_->sources()(i).ajouter(resu);
                }
            }
          statistiques().begin_count(assemblage_sys_counter_);
          matrice.ajouter_multvect(inco, resu); // Add source residual first
          for (int op = 0; op < eq_IBM_->nombre_d_operateurs(); op++)
            {
              eq_IBM_->operateur(op).l_op_base().contribuer_a_avec(inco, matrice);
              eq_IBM_->operateur(op).l_op_base().contribuer_au_second_membre(resu);
            }
        }
      else
        {
          Cerr << "VIA_CONTRIBUER_AU_SECOND_MEMBRE pas code pour " << eq_IBM_->que_suis_je() << ":assembler" << finl;
          Cerr << "avec discretisation " <<  eq_IBM_->probleme().discretisation().que_suis_je() << "" << finl;
          Process::exit();
        }
      // // On calcule somme(residu) par contribuer_au_second_membre (typiquement CL non implicitees)
      // // Cette approche necessite de coder 3 methodes (contribuer_a_avec, contribuer_au_second_membre et ajouter pour l'explicite)
      // sources().contribuer_a_avec(inco,matrice);
      // statistiques().end_count(assemblage_sys_counter_,0,0);
      // sources().ajouter(resu);
      // statistiques().begin_count(assemblage_sys_counter_);
      // matrice.ajouter_multvect(inco, resu); // Add source residual first
      // for (int op = 0; op < nombre_d_operateurs(); op++)
      //   {
      //     operateur(op).l_op_base().contribuer_a_avec(inco, matrice);
      //     operateur(op).l_op_base().contribuer_au_second_membre(resu);
      //   }
    }
  else if (type_codage==Discretisation_base::VIA_AJOUTER)
    {
      // On calcule somme(residu) par somme(operateur)+sources+A*Inco(n)
      // Cette approche necessite de coder seulement deux methodes (contribuer_a_avec et ajouter)
      // Donc un peu plus couteux en temps de calcul mais moins de code a ecrire/maintenir
      for (int op=0; op<eq_IBM_->nombre_d_operateurs(); op++)
        {
          Matrice_Morse mat(matrice);
          mat.get_set_coeff() = 0.0;
          eq_IBM_->operateur(op).l_op_base().contribuer_a_avec(inco, mat);
          if (op == 1) mat *= rhoCp; // la derivee est multipliee par rhoCp pour la convection
          matrice += mat;
          statistiques().end_count(assemblage_sys_counter_, 0, 0);
          {
            DoubleTab resu_tmp(resu);
            resu_tmp = 0.;
            eq_IBM_->operateur(op).ajouter(inco, resu_tmp);
            if (op == 1) resu_tmp *= rhoCp;
            resu += resu_tmp;
          }
          statistiques().begin_count(assemblage_sys_counter_);
        }
      for (int i = 0; i < size_s; i++)
        {
          if (eq_IBM_->sources()(i).valeur().que_suis_je().find("Source_PDF") <= -1)
            {
              const Source_base& src_base = eq_IBM_->sources()(i).valeur();
              src_base.contribuer_a_avec(inco,matrice);
            }
        }
      statistiques().end_count(assemblage_sys_counter_,0,0);
      for (int i = 0; i < size_s; i++)
        {
          if (eq_IBM_->sources()(i).valeur().que_suis_je().find("Source_PDF") <= -1)
            {
              eq_IBM_->sources()(i).ajouter(resu);
            }
        }
      statistiques().begin_count(assemblage_sys_counter_);
      matrice.ajouter_multvect(inco, resu); // Ajout de A*Inco(n)
      // PL (11/04/2018): On aimerait bien calculer la contribution des sources en premier
      // comme dans le cas VIA_CONTRIBUER_AU_SECOND_MEMBRE mais le cas Canal_perio_3D (keps
      // periodique plante: il y'a une erreur de periodicite dans les termes sources du modele KEps...
    }
  else
    {
      Cerr << "Unknown value in Convection_Diffusion_Temperature_IBM::assembler for " << (int)type_codage << finl;
      Process::exit();
    }

  // pour ne pas avoir des termes PDF infinis lors de l'ajout de A*Inco(n)
  for (int i = 0; i < size_s; i++)
    {
      if (eq_IBM_->sources()(i).valeur().que_suis_je().find("Source_PDF") > -1)
        {
          const Source_base& src_base = eq_IBM_->sources()(i).valeur();
          src_base.contribuer_a_avec(inco,matrice);
        }
    }
  // ajouter source PDF avec le bon signe
  derivee_en_temps_inco_ibm_proto(resu);
}
