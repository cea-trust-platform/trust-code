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

#include <Modele_turbulence_scal_base.h>
#include <Convection_Diffusion_std.h>
#include <Turbulence_paroi_scal.h>
#include <Discretisation_base.h>
#include <Modele_turbulence_hyd_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>

Implemente_instanciable(Turbulence_paroi_scal, "Turbulence_paroi_scal", DERIV(Turbulence_paroi_scal_base));

Sortie& Turbulence_paroi_scal::printOn(Sortie& s) const
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
Entree& Turbulence_paroi_scal::readOn(Entree& s)
{
  Cerr << "Lecture du type de loi de parois " << finl;
  // On recupere l'equation hydraulique
  const Equation_base& eqn = mon_modele_turb_scal->equation().probleme().equation(0);
  Motcle typ;
  s >> typ;

  const RefObjU& modele_turbulence = eqn.get_modele(TURBULENCE);
  const Modele_turbulence_hyd_base& mod_turb_hydr = ref_cast(Modele_turbulence_hyd_base, modele_turbulence.valeur());
  const Turbulence_paroi& loi = mod_turb_hydr.loi_paroi();

  if (typ != "negligeable_scalaire")
    if ((loi.valeur().que_suis_je() == "negligeable_VDF") || (loi.valeur().que_suis_je() == "negligeable_VEF"))
      {
        Cerr << "La loi de paroi de type " << typ << " choisie pour le scalaire n'est pas compatible avec" << finl;
        Cerr << "la loi de type " << loi.valeur().que_suis_je() << " choisie pour l'hydraulique" << finl;
        Cerr << "Utiliser le type 'negligeable_scalaire' pour le scalaire ou utiliser une loi de paroi" << finl;
        Cerr << "non negligeable pour l hydraulique" << finl;
        exit();
      }
  typ += "_";

  Nom discr = eqn.discretisation().que_suis_je();

  //  les operateurs de diffusion sont communs aux discretisations VEF et VEFP1B
  if (discr == "VEFPreP1B")
    discr = "VEF";
  typ += discr;

  if (typ == "loi_analytique_scalaire_VDF")
    {
      Cerr << "La loi de paroi scalaire de type loi_analytique_scalaire" << finl;
      Cerr << "n est utilisable qu avec une discretisation de type VEF" << finl;
      exit();
    }
  Cerr << "et typage :" << typ << finl;
  DERIV(Turbulence_paroi_scal_base)::typer(typ);
  valeur().associer_modele(mon_modele_turb_scal.valeur());
  valeur().associer(eqn.domaine_dis(), eqn.domaine_Cl_dis());

  return s;
}

/*! @brief Associe un modele de turbulence a l'objet.
 *
 * @param (Modele_turbulence_hyd_base& le_modele) le modele de turbulence hydraulique a associer a l'objet
 */
void Turbulence_paroi_scal::associer_modele(const Modele_turbulence_scal_base& mod)
{
  mon_modele_turb_scal = mod;
}

