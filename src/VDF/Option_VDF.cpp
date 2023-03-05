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

#include <Option_VDF.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable(Option_VDF, "Option_VDF", Interprete);
// XD option_vdf interprete option_vdf 1 Class of VDF options.

double Option_VDF::coeff_P_neumann = 1.;
int Option_VDF::traitement_coins = 0;
int Option_VDF::traitement_gradients = 0;

Sortie& Option_VDF::printOn(Sortie& os) const { return Interprete::printOn(os); }
Entree& Option_VDF::readOn(Entree& is) { return Interprete::readOn(is); }

Entree& Option_VDF::interpreter(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter_non_std("traitement_coins", (this)); // XD_ADD_P chaine(into=["oui","non"]) Treatment of corners (yes or no). This option modifies slightly the calculations at the outlet of the plane channel. It supposes that the boundary continues after channel outlet (i.e. velocity vector remains parallel to the boundary).
  param.ajouter_non_std("traitement_gradients", (this)); // XD_ADD_P chaine(into=["oui","non"]) Treatment of gradient calculations (yes or no). This option modifies slightly the gradient calculation at the corners and activates also the corner treatment option.
  param.ajouter_non_std("P_imposee_aux_faces", (this)); // XD_ADD_P chaine(into=["oui","non"]) Pressure imposed at the faces (yes or no).
  param.ajouter_non_std("all_options|toutes_les_options", (this)); // XD_ADD_P rien Activates all Option_VDF options. If used, must be used alone without specifying the other options, nor combinations.
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Option_VDF::test_mot(const Motcle& mot_cle, const Motcle& mot_lu) const
{
  if (all_options_)
    {
      Cerr << "Error in Option_VDF::" << __func__ << " !!!!!!!!!!!!" << finl;
      Cerr << "You can not activate the option : " << mot_cle << " because the keyword all_options|toutes_les_options is already specified. Remove one of them !" << finl;
      Process::exit();
    }

  Motcles les_mots(2);
  les_mots[0] = "non";
  les_mots[1] = "oui";

  int rang = les_mots.search(mot_lu);
  if (rang == -1)
    {
      Cerr << "Error in Option_VDF::" << __func__ << " !!!!!!!!!!!!" << finl;
      Cerr << "The tested keyword is " << mot_lu << ", while we expected a keyword among the following : oui/non" << finl;
      throw;
    }
  else return rang; /* 0 si non et 1 si oui */
}

int Option_VDF::lire_motcle_non_standard(const Motcle& mot_cle, Entree& is)
{
  Motcle mot_lu;
  int retval = 1;

  if (mot_cle == "traitement_coins")
    {
      is >> mot_lu;
      if (test_mot(mot_cle, mot_lu))
        {
          Cerr << mot_cle << " : will affect the action of the method Domaine_Cl_VDF::completer ..." << finl;
          traitement_coins = 1;
        }
    }
  else if (mot_cle == "traitement_gradients")
    {
      is >> mot_lu;
      if (test_mot(mot_cle, mot_lu))
        {
          Cerr << mot_cle << " : will affect the gradient calculation at the corners ! This option activates also traitement_coins ..." << finl;
          traitement_gradients = 1, traitement_coins = 1;
        }
    }
  else if (mot_cle == "P_imposee_aux_faces")
    {
      is >> mot_lu;
      if (test_mot(mot_cle, mot_lu))
        {
          Cerr << mot_cle << " : will affect the action of the operator gradient for Neumann_sortie_libre boundary condition and therefore the assembleur action will also be affected ..." << finl;
          coeff_P_neumann = 2.;
        }
    }
  else if (mot_cle == "all_options" || mot_cle == "toutes_les_options") /* for experts only ;) */
    {
      if (traitement_coins == 1 || traitement_coins == 1 || coeff_P_neumann == 2.)
        {
          Cerr << "Error in Option_VDF::" << __func__ << " !!!!!!!!!!!!" << finl;
          Cerr << "You are using the keyword all_options|toutes_les_options to activate all options, but your are explicitly specifing other options ..." << finl;
          Cerr << "This is not allowed, either remove all_options|toutes_les_options keyword, or use it alone." << finl;
          Process::exit();
        }
      Cerr << "Activating all VDF options ... This will affect the action of the method Domaine_Cl_VDF::completer, the gradient calculation at the corners and " << finl;
      Cerr << "the action of the operator gradient for Neumann_sortie_libre boundary condition and therefore the assembleur action will also be affected ... " << finl;
      all_options_ = true;
      traitement_gradients = 1, traitement_coins = 1, coeff_P_neumann = 2.;
    }
  else
    retval = -1;

  return retval;
}
