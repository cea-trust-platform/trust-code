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

#ifndef Turbulence_paroi_scal_included
#define Turbulence_paroi_scal_included

#include <Turbulence_paroi_scal_base.h>
#include <TRUST_Deriv.h>

class Pb_Hydraulique;
class Probleme_base;

/*! @brief Classe Turbulence_paroi_scal Classe generique de la hierarchie des turbulences au niveau de la
 *
 *     paroi, un objet Turbulence_paroi peut referencer n'importe quel
 *     objet derivant Turbulence_paroi_base.
 *     La plupart des methodes appellent les methodes de l'objet Probleme
 *     sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Turbulence_paroi_scal_base
 */
class Turbulence_paroi_scal: public DERIV(Turbulence_paroi_scal_base)
{
  Declare_instanciable(Turbulence_paroi_scal);
public:
  inline Turbulence_paroi_scal& operator=(const Turbulence_paroi_scal_base &paroi_base);
  void associer_modele(const Modele_turbulence_scal_base&);

protected:

  REF(Modele_turbulence_scal_base) mon_modele_turb_scal;
};

inline Turbulence_paroi_scal& Turbulence_paroi_scal::operator=(const Turbulence_paroi_scal_base& paroi_base)
{
  DERIV(Turbulence_paroi_scal_base)::operator=(paroi_base);
  return *this;
}

#endif /* Turbulence_paroi_scal_included */
