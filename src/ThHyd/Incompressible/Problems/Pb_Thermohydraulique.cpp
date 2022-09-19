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

#include <Pb_Thermohydraulique.h>
#include <Fluide_Ostwald.h>
#include <Verif_Cl.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Pb_Thermohydraulique,"Pb_Thermohydraulique",Pb_Fluide_base);

/*! @brief Simple appel a: Pb_Fluide_base::printOn(Sortie&) Ecrit le probleme sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Pb_Thermohydraulique::printOn(Sortie& os) const
{
  return Pb_Fluide_base::printOn(os);
}


/*! @brief Simple appel a: Pb_Fluide_base::readOn(Entree&) Lit le probleme a partir d'un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Pb_Thermohydraulique::readOn(Entree& is)
{
  return Pb_Fluide_base::readOn(is);
}

/*! @brief Renvoie le nombre d'equation, Renvoie 2 car il y a 2 equations a un probleme de
 *
 *     thermo-hydraulique standard:
 *         l'equation de Navier Stokes
 *         l' equation de la thermique de type Convection_Diffusion_Temperature
 *
 * @return (int) le nombre d'equation
 */
int Pb_Thermohydraulique::nombre_d_equations() const
{
  return 2;
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature si i=1
 *     (version const)
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
const Equation_base& Pb_Thermohydraulique::equation(int i) const
{
  if ( !( i==0 || i==1 ) )
    {
      Cerr << "\nError in Pb_Thermohydraulique::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else
    return eq_thermique;
}

/*! @brief Renvoie l'equation d'hydraulique de type Navier_Stokes_std si i=0 Renvoie l'equation de la thermique de type
 *
 *     Convection_Diffusion_Temperature si i=1
 *
 * @param (int i) l'index de l'equation a renvoyer
 * @return (Equation_base&) l'equation correspondante a l'index
 */
Equation_base& Pb_Thermohydraulique::equation(int i)
{
  if ( !( i==0 || i==1 ) )
    {
      Cerr << "\nError in Pb_Thermohydraulique::equation() : Wrong number of equation !" << finl;
      Process::exit();
    }
  if (i == 0)
    return eq_hydraulique;
  else
    return eq_thermique;
}

/*! @brief Associe le milieu au probleme Le milieu doit etre de type fluide incompressible
 *
 * @param (Milieu_base& mil) le milieu physique a associer au probleme
 * @throws mauvais type de milieu physique
 */
void Pb_Thermohydraulique::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(Fluide_base,mil) && ref_cast(Fluide_base, mil).is_incompressible())
    {
      eq_hydraulique.associer_milieu_base(mil);
      eq_thermique.associer_milieu_base(mil);
    }
  else if (sub_type(Fluide_Ostwald,mil))
    {
      eq_hydraulique.associer_milieu_base(mil);
      eq_thermique.associer_milieu_base(mil);
      Cerr << "Fluide non newtonien" << finl;
    }
  else
    {
      Cerr << "Un milieu de type " << mil.que_suis_je() << " ne peut etre associe a "<< finl;
      Cerr << "un probleme de type Pb_Thermohydraulique " << finl;
      exit();
    }
}

/*! @brief Teste la compatibilite des equations de la thermique et de l'hydraulique.
 *
 * Le test se fait sur les conditions
 *     aux limites discretisees de chaque equation.
 *     Appel la fonction de librairie hors classe:
 *       tester_compatibilite_hydr_thermique(const Zone_Cl_dis&,const Zone_Cl_dis&)
 *
 * @return (int) code de retour propage
 */
int Pb_Thermohydraulique::verifier()
{
  const Zone_Cl_dis& zone_Cl_hydr = eq_hydraulique.zone_Cl_dis();
  const Zone_Cl_dis& zone_Cl_th = eq_thermique.zone_Cl_dis();
  return tester_compatibilite_hydr_thermique(zone_Cl_hydr,zone_Cl_th);
}



