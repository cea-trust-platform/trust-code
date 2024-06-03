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
#include <Modele_turbulence_hyd_base.h>
#include <Turbulence_paroi.h>
#include <Equation_base.h>

Implemente_instanciable(Turbulence_paroi, "Turbulence_paroi", DERIV(Turbulence_paroi_base));

Sortie& Turbulence_paroi::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

/*! @brief Lit les caracteristques de la loi de parois a partir d'un flot d'entree.
 *
 *     Format:
 *       type_de_loi_de_paroi
 *     Les valeurs possibles du type de loi de paroi sont:
 *       - "loi_standard_hydr"
 *       - "negligeable"
 *       - "loi_VanDriest"
 *       - "loi_standard_hydr_scalaire"
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws type de loi de paroi incompatible avec le probleme traite
 */
Entree& Turbulence_paroi::readOn(Entree& s)
{
  Cerr << "Lecture du type de loi de parois " << finl;
  const Equation_base& eqn = mon_modele_turb_hyd->equation();
  Motcle typ;
  s >> typ;
  if (typ == "loi_standard_hydr_scalaire" || typ == "loi_paroi_2_couches_scalaire" || typ == "negligeable_scal")
    {
      Cerr << finl;
      Cerr << "Le format du jeu de donnees a change:" << finl;
      Cerr << "Chaque modele de turbulence doit avoir sa loi de paroi specifiee." << finl;
      Cerr << "Ainsi par exemple, loi_standard_hydr sera pour le modele de turbulence de l'equation de qdm" << finl;
      Cerr << "et loi_standard_hydr_scalaire pour le modele de turbulence de l'equation d'energie." << typ << finl;
      Cerr << finl;
      exit();
    }
  typ += "_";

  Nom discr = eqn.discretisation().que_suis_je();

  //  les operateurs de diffusion sont communs aux discretisations VEF et VEFP1B
  if (discr == "VEFPreP1B") discr = "VEF";
  typ += discr;

  Cerr << "et typage :" << typ << finl;
  DERIV(Turbulence_paroi_base)::typer(typ);
  valeur().associer_modele(mon_modele_turb_hyd.valeur());
  valeur().associer(eqn.domaine_dis(), eqn.domaine_Cl_dis());
  return s;
}

/*! @brief Associe un modele de turbulence a l'objet.
 *
 * @param (Modele_turbulence_hyd_base& le_modele) le modele de turbulence hydraulique a associer a l'objet
 */
void Turbulence_paroi::associer_modele(const Modele_turbulence_hyd_base& mod)
{
  mon_modele_turb_hyd = mod;
}

