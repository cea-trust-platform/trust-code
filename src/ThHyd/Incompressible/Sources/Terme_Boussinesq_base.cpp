/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Convection_Diffusion_std.h>
#include <Terme_Boussinesq_base.h>
#include <Fluide_base.h>
#include <Motcle.h>
#include <algorithm>
#include <Param.h>

Implemente_base(Terme_Boussinesq_base,"Terme_Boussinesq_base",Source_base);
// XD boussinesq_temperature source_base boussinesq_temperature -1 Class to describe a source term that couples the movement quantity equation and energy equation with the Boussinesq hypothesis.
// XD attr t0 chaine t0 0 Reference temperature value (oC or K). It can also be a time dependant function since the 1.6.6 version.
// XD attr verif_boussinesq entier verif_boussinesq 1 Keyword to check (1) or not (0) the reference value in comparison with the mean value in the domain. It is set to 1 by default.
// XD boussinesq_concentration source_base boussinesq_concentration -1 Class to describe a source term that couples the movement quantity equation and constituent transport equation with the Boussinesq hypothesis.
// XD attr c0 list c0 0 Reference concentration field type. The only field type currently available is Champ_Uniforme (Uniform field).

Sortie& Terme_Boussinesq_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }

/*! @brief Lit les parametres du terme source a partir d'un flot d'entree.
 *
 */
Entree& Terme_Boussinesq_base::readOn(Entree& is )
{
  Param param(que_suis_je());
  Cerr << "Reading Boussinesq source term parameters." << finl;
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Terme_Boussinesq_base::set_param(Param& param)
{
  param.ajouter_non_std("T0",(this));
  param.ajouter_non_std("C0",(this));
  param.ajouter("verif_boussinesq",&verif_);
}

// Cherche dans le probleme l'equation scalaire
void Terme_Boussinesq_base::associer_pb(const Probleme_base& pb)
{
  int ok=0;
  int n_eq=pb.nombre_d_equations();
  for (int eq=0; eq<n_eq; eq++)
    {
      const Equation_base& eqn=pb.equation(eq);
      if (eqn.inconnue().le_nom()==NomScalaire_)
        {
          ok=1;
          equation_scalaire_ = ref_cast(Convection_Diffusion_std,eqn);
        }
    }
  if (ok==0)
    {
      Cerr << "Error. The Boussinesq source term can't be defined on a problem of kind " << pb.que_suis_je() << finl;
      Process::exit();
    }

  const Fluide_base& fluide = ref_cast(Fluide_base, equation().milieu());
  la_gravite_ = fluide.gravite();

  //this variable indicates if the beta field is valid or not
  int valid_beta_field = 0; // by default it is invalid
  Nom beta_field_name="??"; // only used for the error message

  if (NomScalaire_=="temperature")
    {
      valid_beta_field = fluide.beta_t( ).non_nul( );
      beta_field_name = "thermal expansion value (beta_th)";
      beta_=fluide.beta_t();
    }
  else if (NomScalaire_=="concentration")
    {
      valid_beta_field = fluide.beta_c( ).non_nul( );
      beta_field_name = "volume expansion coefficient values in concentration (beta_co)";
      beta_=fluide.beta_c();
    }

  if( ! valid_beta_field )
    {
      Cerr << "Error. Boussinesq source term is not able to access to the "<<beta_field_name<<" associated to the fluid."<<finl;
      Cerr << "Please check your data file. " << finl;
      Cerr << "Aborting..."<<finl;
      Process::abort( );
    }
}

// Fonction de lecture d'un Parser_U
void read(Entree& is, Parser_U& fct_Scalaire0_)
{
  Nom expression;
  is >> expression;
  std::string ss(expression.getString());
  std::transform(ss.begin(), ss.end(), ss.begin(), ::toupper);
  fct_Scalaire0_.setNbVar(1);
  fct_Scalaire0_.setString(ss);
  fct_Scalaire0_.addVar("t");
  fct_Scalaire0_.parseString();
}

int Terme_Boussinesq_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  int retval = 1;

  if (mot=="T0")
    {
      // Temperature
      NomScalaire_="temperature";
      fct_Scalaire0_.dimensionner(1);
      Scalaire0_.resize(1);
      read(is, fct_Scalaire0_[0]);
    }
  else if (mot=="C0")
    {
      // Concentration
      NomScalaire_="concentration";
      int dim;
      is >> dim;
      fct_Scalaire0_.dimensionner(dim);
      Scalaire0_.resize(dim);
      for (int i=0; i<dim; i++) read(is, fct_Scalaire0_[i]);
    }
  else retval = -1;

  return retval;
}
