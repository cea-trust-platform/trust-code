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

#include <Champ_Input_P0_Composite.h>
#include <Discretisation_base.h>
#include <Champ_Fonc_MED.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Domaine_dis_base.h>
#include <Param.h>

Implemente_instanciable(Champ_Input_P0_Composite,"Champ_Input_P0_Composite",Champ_Fonc_P0_base);
// XD champ_input_p0_composite champ_input_base champ_input_p0_composite 1 Field used to define a classical champ input p0 field (for ICoCo), but with a predefined field for the initial state.
// XD attr initial_field field_base initial_field 1 The field used for initialization
// XD attr input_field champ_input_p0 input_field 1 The input field for ICoCo

Sortie& Champ_Input_P0_Composite::printOn(Sortie& os) const { return os; }

Entree& Champ_Input_P0_Composite::readOn(Entree& is)
{
  Cerr << "Reading of data for " << que_suis_je() << finl;
  Param param(que_suis_je());
  param.ajouter("initial_field", &champ_initial_);
  param.ajouter("input_field", &champ_input_, Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);

  // Put initial field at elem if needed
  if ( is_initialized() )
    {
      if (champ_initial_->nb_comp() != champ_input_.nb_comp())
        {
          Cerr << "WHAT ?? Go to the Champ_Input_P0_Composite field and define the same number of components for the fields !!!" << finl;
          Process::exit();
        }

      const Discretisation_base& disc = champ_input_.z_probleme().discretisation();
      const int ncomp = champ_initial_->nb_comp();
      if (domaine_dis_base().nb_elem_tot() == champ_initial_->valeurs().dimension_tot(0)) { /* do nothing */ }
      else if (sub_type(Champ_Fonc_MED,champ_initial_.valeur()))
        {
          Champ_Don tmp_fld;
          disc.discretiser_champ("champ_elem",domaine_dis_base(),"neant","neant",ncomp ,0.,tmp_fld);
          tmp_fld->affecter(champ_initial_.valeur()); // interpolate ...
          champ_initial_.detach();
          disc.discretiser_champ("champ_elem",domaine_dis_base(),"neant","neant",ncomp,0.,champ_initial_);
          champ_initial_->valeurs() = tmp_fld->valeurs();
        }
      else if (sub_type(Champ_Uniforme,champ_initial_.valeur())) // blabla ...
        {
          const double val = champ_initial_->valeurs()(0,0);
          champ_initial_.detach();
          disc.discretiser_champ("champ_elem",domaine_dis_base(),"neant","neant",ncomp,0.,champ_initial_);
          champ_initial_->valeurs() = val;
        }
      else
        {
          Cerr << "Error in the readOn of : " << que_suis_je() << finl;
          Cerr << "The initial field type is not yet treated ! Call the 911 !" << finl;
          Process::exit();
        }
    }
  return is;
}
