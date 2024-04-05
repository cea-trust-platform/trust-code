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

#ifndef Problemes_Cloned_Concentration_included
#define Problemes_Cloned_Concentration_included

#include <Pb_Cloned_Concentration_Gen.h>

/// \cond DO_NOT_DOCUMENT
class Problemes_Cloned_Concentration
{ /* pour check sources */ };
/// \endcond

/*! @brief classe Pb_Thermohydraulique_Cloned_Concentration Cette classe represente un probleme de thermohydraulique avec concentrations :
 *
 *      - Equations de Navier_Stokes en regime laminaire pour un fluide incompressible
 *      - Equation d'energie en regime laminaire
 *      - Une equation de convection-diffusion pour un ou plusieurs constituants caracterises par leurs concentrations
 *        En general, on couple les equations d'energie et de concentration aux equations de Navier-Stokes par l'intermediaire du terme source
 *        des forces de volume dans lequel on prend en compte de petites variations de la masse volumique en fonction de la temperature et
 *        de la concentration.
 *
 * @sa Pb_Fluide_base
 */
class Pb_Thermohydraulique_Cloned_Concentration: public Pb_Cloned_Concentration_Gen<Pb_Thermohydraulique>
{
  Declare_instanciable(Pb_Thermohydraulique_Cloned_Concentration);
};

/*! @brief Classe Pb_Hydraulique_Cloned_Concentration Cette classe represente un probleme d'hydraulique avec transport d'un ou plusieurs constituants:
 *        - Equations de Navier_Stokes en regime laminaire pour un fluide incompressible
 *        - Equations de convection-diffusion en regime laminaire
 *          En fait si on transporte plusieurs constituants on utilisera une seule equation de convection-diffusion avec une inconnue vectorielle.
 *          En general, on couple les 2 equations par l'intermediaire du terme source des forces de volume de Navier_Stokes dans lequel on prend
 *          en compte de petites variations de la masse volumique en fonction du ou des constituants
 *
 * @sa Pb_Fluide_base
 */
class Pb_Hydraulique_Cloned_Concentration: public Pb_Cloned_Concentration_Gen<Pb_Hydraulique>
{
  Declare_instanciable(Pb_Hydraulique_Cloned_Concentration);
};

#endif /* Problemes_Cloned_Concentration_included */
