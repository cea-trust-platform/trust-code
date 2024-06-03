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

#include <Discretisation_base.h>
#include <Equation_base.h>
#include <Modele_turbulence_hyd.h>
#include <Motcle.h>

Implemente_instanciable(Modele_turbulence_hyd, "Modele_turbulence_hyd", DERIV(Modele_turbulence_hyd_base));

Sortie& Modele_turbulence_hyd::printOn(Sortie& s) const
{
  return DERIV(Modele_turbulence_hyd_base)::printOn(s);
}

/*! @brief Lit les specifications d'un modele de turbulence a partir d'un flot d'entree.
 *
 *     Lit le type de modele, type l'objet et appelle
 *     une lecture specifique (par polymorphisme)
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws Les modeles sous maille sont utilisables uniquement en dimension 3
 */
Entree& Modele_turbulence_hyd::readOn(Entree& s)
{
  //  Cerr << " Modele_turbulence_hyd::readOn" << finl;
  Motcle typ;
  s >> typ;
  Motcle nom1("Modele_turbulence_hyd_");
  nom1 += typ;
  Nom discr = equation().discretisation().que_suis_je();
  if (typ.debute_par("SOUS_MAILLE") || discr == "VDF_Hyper" || typ.debute_par("LONGUEUR_MELANGE") || (typ == "K_Epsilon_V2"))
    {
      if (dimension == 2 && discr != "VDF_Hyper")
        {
          Cerr << "Vous traitez un cas turbulent en dimension 2 avec un modele sous maille" << finl;
          Cerr << "Attention a l'interpretation des resultats !!" << finl;
          //         exit();
        }

      nom1 += "_";
      // les operateurs de diffusion sont communs aux discretisations VEF et VEFP1B
      if (discr == "VEFPreP1B") discr = "VEF";
      nom1 += discr;
    }
  if (nom1 == "MODELE_TURBULENCE_HYD_SOUS_MAILLE_LM_VEF")
    {
      Cerr << "Le mot cle Sous_maille_LM s'appelle desormais Longueur_Melange pour etre coherent en VDF et VEF." << finl;
      Cerr << "Changer votre jeu de donnees." << finl;
      exit();
    }
  DERIV(Modele_turbulence_hyd_base)::typer(nom1);
  valeur().associer_eqn(equation());
  valeur().associer(equation().domaine_dis(), equation().domaine_Cl_dis());
  s >> valeur();
  return s;
}
