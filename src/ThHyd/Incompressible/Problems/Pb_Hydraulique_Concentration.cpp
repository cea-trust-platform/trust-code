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

#include <Pb_Hydraulique_Concentration.h>
#include <Fluide_Incompressible.h>
#include <Constituant.h>
#include <Verif_Cl.h>

Implemente_instanciable(Pb_Hydraulique_Concentration,"Pb_Hydraulique_Concentration",Pb_Fluide_base);
// XD pb_hydraulique_concentration Pb_base pb_hydraulique_concentration -1 Resolution of Navier-Stokes/multiple constituent transport equations.
// XD   attr fluide_incompressible fluide_incompressible fluide_incompressible 0 The fluid medium associated with the problem.
// XD   attr constituant constituant constituant 1 Constituents.
// XD   attr navier_stokes_standard navier_stokes_standard navier_stokes_standard 1 Navier-Stokes equations.
// XD   attr convection_diffusion_concentration convection_diffusion_concentration convection_diffusion_concentration 1 Constituent transport vectorial equation (concentration diffusion convection).

/*! @brief Simple appel a: Pb_Fluide_base::printOn(Sortie&) Ecrit le probleme sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Pb_Hydraulique_Concentration::printOn(Sortie& os) const
{
  return Pb_Fluide_base::printOn(os);
}


/*! @brief Simple appel a: Pb_Fluide_base::readOn(Entree&) Lit le probleme a partir d'un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Pb_Hydraulique_Concentration::readOn(Entree& is)
{
  return Pb_Fluide_base::readOn(is);
}

/*! @brief Renvoie le nombre d'equation, Renvoie 2 car il y a 2 equations a un probleme
 *
 *     hydraulique avec transport:
 *       - l'equation de Navier Stokes
 *       - une equation de convection-diffusion (eventuellement vectorielle)
 *
 * @return (int) le nombre d'equation
 */
int Pb_Hydraulique_Concentration::nombre_d_equations() const
{
  return 2;
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0 Renvoie l'equation de convection-diffusion de type
 *
 *     Convection_Diffusion_Concentration si i=1
 *     (l'equation de convection diffusion peut-etre vectorielle)
 *     (version const)
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
const Equation_base& Pb_Hydraulique_Concentration::equation(int i) const
{
  if ( !( i==0 || i==1 ) )
    {
      Cerr << "\nError in Pb_Hydraulique_Concentration::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else
    return eq_concentration;
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0 Renvoie l'equation de convection-diffusion de type
 *
 *     Convection_Diffusion_Concentration si i=1
 *     (l'equation de convection diffusion peut-etre vectorielle)
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
Equation_base& Pb_Hydraulique_Concentration::equation(int i)
{
  if ( !( i==0 || i==1 ) )
    {
      Cerr << "\nError in Pb_Hydraulique_Concentration::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else
    return eq_concentration;
}


/*! @brief Associe un milieu au probleme, Si le milieu est de type
 *
 *       - Fluide_Incompressible, il sera associe a l'equation de l'hydraulique
 *       - Constituant, il sera associe a l'equation de convection-diffusion
 *     Un autre type de milieu provoque une erreur
 *
 * @param (Milieu_base& mil) le milieu physique a associer au probleme
 * @throws mauvais type de milieu physique
 */
void Pb_Hydraulique_Concentration::associer_milieu_base(const Milieu_base& mil)
{
  if ( sub_type(Fluide_Incompressible,mil) )
    eq_hydraulique.associer_milieu_base(mil);
  else if ( sub_type(Constituant,mil) )
    eq_concentration.associer_milieu_base(mil);
  else
    {
      Cerr << "Un milieu de type " << mil.que_suis_je() << " ne peut etre associe a " << finl;
      Cerr << "un probleme de type Pb_Hydraulique_Concentration " << finl;
      exit();
    }
}


/*! @brief Teste la compatibilite des equations de convection-diffusion et de l'hydraulique.
 *
 * Le test se fait sur les conditions
 *     aux limites discretisees de chaque equation.
 *     Appel la fonction de librairie hors classe:
 *       tester_compatibilite_hydr_concentration(const Domaine_Cl_dis_base&,const Domaine_Cl_dis_base&)
 *
 * @return (int) code de retour propage
 */
int Pb_Hydraulique_Concentration::verifier()
{
  const Domaine_Cl_dis_base& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis_base& domaine_Cl_co = eq_concentration.domaine_Cl_dis();
  return tester_compatibilite_hydr_concentration(domaine_Cl_hydr,domaine_Cl_co);
}
