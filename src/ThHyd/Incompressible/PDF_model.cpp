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

/////////////////////////////////////////////////////////////////////////////
//
// Model developped by Michel Belliard
// Implementation : Georis Billo
//
/////////////////////////////////////////////////////////////////////////////

#include <PDF_model.h>
#include <Param.h>


Implemente_instanciable(PDF_model,"PDF_model",Objet_U) ;
// (xdata documentation is in the TRAD_2.org because we need a special bloc_lecture object)

Sortie& PDF_model::printOn(Sortie& os) const
{
  Objet_U::printOn(os);
  return os;
}

Entree& PDF_model::readOn(Entree& is)
{
  // (xdata documentation is in the TRAD_2.org because we need a special bloc_lecture object)
  type_vitesse_imposee_ = -1; //DEFAULT VALUE = not set
  Param param(que_suis_je());
  param.ajouter("eta",&eta_, Param::REQUIRED);
  temps_relax_=1.0e+12;
  param.ajouter("temps_relaxation_coefficient_PDF",&temps_relax_,Param::OPTIONAL);
  echelle_relax_=5.0e-2;
  param.ajouter("echelle_relaxation_coefficient_PDF",&echelle_relax_,Param::OPTIONAL);
  param.ajouter_flag("local",&local_);
  param.ajouter_non_std("vitesse_imposee_data",(this),Param::OPTIONAL);
  param.ajouter_non_std("vitesse_imposee_fonction",(this),Param::OPTIONAL);
  param.lire_avec_accolades_depuis(is);
  if (type_vitesse_imposee_ == -1)
    {
      Cerr << "PDF_model: you must specify either 'vitesse_imposee_data' or 'vitesse_imposee_fonction'" << finl;
      Process::exit();
    }
  coefku_ = 1./eta_;
  return is;
}

int PDF_model::lire_motcle_non_standard(const Motcle& un_mot, Entree& is)
{
  if (un_mot == "vitesse_imposee_fonction")
    {
      if (Process::je_suis_maitre())
        {
          Cerr << " PDF_model is reading imposed velocity... " << finl;
        }
      type_vitesse_imposee_ = 1;
      Nom expr_vit_imp;
      int dim = Objet_U::dimension;
      parsers_.dimensionner(dim);
      for (int i = 0; i < dim; i++)
        {
          is >> expr_vit_imp;
          std::string sx(expr_vit_imp);
          for (auto & c: sx) c = toupper(c);
          parsers_[i].setString(sx);
          parsers_[i].setNbVar(4);
          parsers_[i].addVar("x");
          parsers_[i].addVar("y");
          parsers_[i].addVar("z");
          parsers_[i].addVar("t");
          parsers_[i].parseString();
        }
    }
  else if (un_mot == "vitesse_imposee_data")
    {
      type_vitesse_imposee_ = 0;
      is >> vitesse_imposee_lu_;
    }
  else
    {
      Cerr << "PDF_model: token not understood: " << un_mot << finl;
      Process::exit();
    }
  return 1;
}

void PDF_model::affecter_vitesse_imposee(Zone_VF& la_zone, const DoubleTab& coords)
{
  if (type_vitesse_imposee_ == 1)
    {
      int nb_som_tot = la_zone.nb_som_tot();

      DoubleTab& vitesse_imposee_ref = vitesse_imposee_.valeur().valeurs();

      int dim = Objet_U::dimension;

      ArrOfDouble x(dim);

      for (int i = 0; i < nb_som_tot; i++)
        {
          for (int j = 0; j < dim; j++)
            {
              x[j] = coords(i,j);
            }
          for (int j = 0; j < dim; j++)
            {
              vitesse_imposee_ref(i,j) = get_vitesse_imposee(x,j);
            }
        }
    }
  else
    {
      Cerr << __FILE__ << ", line " << (int)__LINE__ << " : Unexpected error." << finl;
      exit();
    }
}

double PDF_model::get_vitesse_imposee(ArrOfDouble& x,int comp)
{
  int dim = Objet_U::dimension;
  for (int i = 0; i < dim; i++)
    {
      parsers_[comp].setVar(i,x[i]);
    }
  return parsers_[comp].eval();
}
