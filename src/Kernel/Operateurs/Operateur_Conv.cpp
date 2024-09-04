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

#include <Discretisation_base.h>
#include <Operateur_Conv.h>
#include <stat_counters.h>
#include <Champ_base.h>

Implemente_instanciable(Operateur_Conv,"Operateur_Conv",OWN_PTR(Operateur_Conv_base));
// XD convection_deriv objet_lecture convection_deriv 0 not_set
// XD bloc_convection objet_lecture nul 0 not_set
// XD attr aco chaine(into=["{"]) aco 0 Opening curly bracket.
// XD attr operateur convection_deriv operateur 0 not_set
// XD attr acof chaine(into=["}"]) acof 0 Closing curly bracket.

/* Here we put the doc for all shared (VDF, VEF,...) keywords: */

// XD convection_negligeable convection_deriv negligeable 0 For VDF and VEF discretizations. Suppresses the convection operator.

// XD convection_amont convection_deriv amont 0 Keyword for upwind scheme for VDF or VEF discretizations. In VEF discretization equivalent to generic amont for TRUST version 1.5 or later. The previous upwind scheme can be used with the obsolete in future amont_old keyword.

// XD convection_centre convection_deriv centre 0 For VDF and VEF discretizations.

// XD convection_centre4 convection_deriv centre4 0 For VDF and VEF discretizations.

Sortie& Operateur_Conv::printOn(Sortie& os) const
{
  return Operateur::ecrire(os);
}

Entree& Operateur_Conv::readOn(Entree& is)
{
  Operateur::lire(is);
  if ((mon_equation->le_nom() ==  "pb_sensibiliteNavier_Stokes_standard_sensibility")  && (valeur().que_suis_je()!="Op_Conv_sensibility_VEF_P1NC"))
    {
      Cerr<<" You should use the sensibility convection operator within Navier_Stokes_standard_sensibility equation "<<finl;
      Cerr<<" Exemple:  convection { sensibility { amont } }  "<<finl;
      exit();
    }

  if(mon_equation->le_nom().contient("ALE") && (valeur().que_suis_je()!="Op_Conv_ALE_VEF") )
    {
      Cerr<<" You should use the ALE convection operator within the Navier_Stokes_standard_ALE/Navier_Stokes_Turbulent_ALE equation "<<finl;
      Cerr<<" Exemple:  convection { ALE { muscl } }  "<<finl;
      Process::exit();
    }

  return is;
}

/*! @brief Type l'operateur: "Op_Conv" + motcle + discretisation + inconnue
 *
 *      Associe la vitesse transportante a l'operateur base
 *
 */
void Operateur_Conv::typer()
{
  if (Motcle(typ)==Motcle("negligeable"))
    OWN_PTR(Operateur_Conv_base)::typer("Op_Conv_negligeable");
  else
    {
      Equation_base& eqn=mon_equation.valeur();
      Nom nom_type=eqn.discretisation().get_name_of_type_for(que_suis_je(),typ,eqn,vitesse());
      OWN_PTR(Operateur_Conv_base)::typer(nom_type);
      valeur().associer_vitesse(vitesse());
    }
  Cerr << valeur().que_suis_je() << finl;
}

/*! @brief Ajoute la contribution de l'operateur au tableau passe en parametre
 *
 * @param (DoubleTab& donnee) tableau contenant les donnees sur lesquelles on applique l'operateur.
 * @param (DoubleTab& resu) tableau auquel on ajoute la contribution de l'operateur
 * @return (DoubleTab&) le tableau contenant le resultat
 */
DoubleTab& Operateur_Conv::ajouter(const DoubleTab& donnee,
                                   DoubleTab& resu) const
{
  statistiques().begin_count(convection_counter_);
  DoubleTab& tmp = valeur().ajouter(donnee, resu);
  statistiques().end_count(convection_counter_);
  return tmp;
}

/*! @brief Initialise le tableau passe en parametre avec la contribution de l'operateur.
 *
 * @param (DoubleTab& donnee) tableau contenant les donnees sur lesquelles on applique l'operateur.
 * @param (DoubleTab& resu) tableau dans lequel stocke la contribution de l'operateur
 * @return (DoubleTab&) le tableau contenant le resultat
 */
DoubleTab& Operateur_Conv::calculer(const DoubleTab& donnee,
                                    DoubleTab& resu) const
{
  statistiques().begin_count(convection_counter_);
  DoubleTab& tmp = valeur().calculer(donnee, resu);
  statistiques().end_count(convection_counter_);
  return tmp;
}

void Operateur_Conv::associer_norme_vitesse(const Champ_base& norme_vitesse)
{
  assert( op_non_nul() );
  valeur().associer_norme_vitesse(norme_vitesse);
}
