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

// Model developped by Michel Belliard
// Implementation : Georis Billo

#include <PDF_model.h>
#include <algorithm>
#include <Param.h>

Implemente_instanciable(PDF_model,"PDF_model",Objet_U) ;
// XD bloc_pdf_model objet_lecture nul 1 not_set

Sortie& PDF_model::printOn(Sortie& os) const
{
  Objet_U::printOn(os);
  return os;
}

Entree& PDF_model::readOn(Entree& is)
{
  // (xdata documentation is in the TRAD_2.org because we need a special bloc_lecture object)
  Param param(que_suis_je());
  param.ajouter("eta",&eta_, Param::REQUIRED); // XD_ADD_P floattant penalization coefficient
  param.ajouter("bilan_PDF",&pdf_bilan_,Param::OPTIONAL); // XD_ADD_P type de bilan du terme PDF (seul/avec temps/avec convection)
  param.ajouter("temps_relaxation_coefficient_PDF",&temps_relax_,Param::OPTIONAL); // XD_ADD_P floattant time relaxation on the forcing term to help
  param.ajouter("echelle_relaxation_coefficient_PDF",&echelle_relax_,Param::OPTIONAL); // XD_ADD_P floattant time relaxation on the forcing term to help convergence
  param.ajouter_flag("local",&local_); // XD_ADD_P rien whether the prescribed velocity is expressed in the global or local basis
  param.ajouter_non_std("vitesse_imposee_data",(this),Param::OPTIONAL); // XD_ADD_P field_base Prescribed velocity as a field
  param.ajouter_non_std("vitesse_imposee_fonction",(this),Param::OPTIONAL); // XD_ADD_P troismots Prescribed velocity as a set of ananlytical component
  param.ajouter_non_std("variable_imposee_data",(this),Param::OPTIONAL); // XD_ADD_P field_base Prescribed variable as a field
  param.ajouter_non_std("variable_imposee_fonction",(this),Param::OPTIONAL); // XD_ADD_P troismots Prescribed variable as a set of ananlytical component
  param.lire_avec_accolades_depuis(is);
  if (type_variable_imposee_ == -1)
    {
      Cerr << "PDF_model: you must specify either 'variable(vitesse)_imposee_data' or 'variable(vitesse)_imposee_fonction'" << finl;
      Process::exit();
    }
  coefku_ = 1./eta_;
  return is;
}

int PDF_model::lire_motcle_non_standard(const Motcle& un_mot, Entree& is)
{
  Cerr << " PDF_model is reading imposed variable... " << finl;
  if (un_mot == "variable_imposee_fonction" || un_mot == "vitesse_imposee_fonction")
    {
      type_variable_imposee_ = 1;
      Nom expr_vit_imp;
      is >> dim_variable_;
      parsers_.dimensionner(dim_variable_);
      for (int i = 0; i < dim_variable_; i++)
        {
          is >> expr_vit_imp;
          std::string sx(expr_vit_imp);
          std::transform(sx.begin(), sx.end(), sx.begin(), ::toupper);
          parsers_[i].setString(sx);
          parsers_[i].setNbVar(4);
          parsers_[i].addVar("x");
          parsers_[i].addVar("y");
          parsers_[i].addVar("z");
          parsers_[i].addVar("t");
          parsers_[i].parseString();
        }
    }
  else if (un_mot == "variable_imposee_data" || un_mot == "vitesse_imposee_data")
    {
      type_variable_imposee_ = 0;
      is >> variable_imposee_lu_;
      dim_variable_ = variable_imposee_lu_->valeurs().dimension(1);
    }
  else
    {
      Cerr << "PDF_model: token not understood: " << un_mot << finl;
      Process::exit();
    }

  if (un_mot == "vitesse_imposee_fonction" || un_mot == "vitesse_imposee_data")
    {
      if (dim_variable_ != Objet_U::dimension)
        {
          Cerr << "PDF_model pour un vecteur: Objet_U::dimension != dim_variable_: " << dim_variable_ << finl;
          Process::exit();
        }
    }
  Cerr << " imposed variable dimension = " << dim_variable_ << finl;
  if (local_ == 1 && (dim_variable_ != Objet_U::dimension))
    {
      Cerr << "PDF_model avec repere local pour un vecteur seulement (Objet_U::dimension != dim_variable_) = " << dim_variable_ << finl;
      Process::exit();
    }

  return 1;
}

void PDF_model::affecter_variable_imposee(Domaine_VF& le_dom, const DoubleTab& coords)
{
  if (type_variable_imposee_ == 1)
    {
      // pour des data aux sommets
      int nb_som_tot = le_dom.nb_som_tot();

      DoubleTab& variable_imposee_ref = variable_imposee_->valeurs();

      int dim = Objet_U::dimension;

      ArrOfDouble x(dim);

      for (int i = 0; i < nb_som_tot; i++)
        {
          for (int j = 0; j < dim; j++)
            {
              x[j] = coords(i,j);
            }
          for (int j = 0; j < dim_variable_; j++)
            {
              variable_imposee_ref(i,j) = get_variable_imposee(x,j);
            }
          // Cerr << "variable_imposee_ref(i)  = " << variable_imposee_ref(i,0)<<" "<< variable_imposee_ref(i,1)<<" " << variable_imposee_ref(i,2) << finl;
        }
    }
  else
    {
      Cerr << __FILE__ << ", line " << (int)__LINE__ << " : Unexpected error." << finl;
      exit();
    }
}

double PDF_model::get_variable_imposee(ArrOfDouble& x,int comp)
{
  int dim = Objet_U::dimension;
  for (int i = 0; i < dim; i++)
    {
      parsers_[comp].setVar(i,x[i]);
    }
  return parsers_[comp].eval();
}
