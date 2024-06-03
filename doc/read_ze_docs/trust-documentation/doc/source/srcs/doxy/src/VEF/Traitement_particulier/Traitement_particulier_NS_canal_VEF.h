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


#ifndef Traitement_particulier_NS_canal_VEF_included
#define Traitement_particulier_NS_canal_VEF_included

#include <Traitement_particulier_NS_canal.h>

/*! @brief classe Traitement_particulier_NS_canal_VEF Cette classe permet de faire les traitements particuliers
 *
 *      pour le calcul d'un canal plan :
 *          * conservation du debit
 *          * calculs de moyennes
 *
 *
 * @sa Navier_Stokes_Turbulent, Traitement_particulier_base,, Traitement_particulier_VEF
 */
class Traitement_particulier_NS_canal_VEF : public Traitement_particulier_NS_canal
{
  Declare_instanciable(Traitement_particulier_NS_canal_VEF);


public :

  Entree& lire(const Motcle& , Entree& );
  Entree& lire(Entree& ) override;

protected :

  void remplir_Y(DoubleVect&, DoubleVect&, int& ) const override;
  void calculer_moyenne_spatiale_vitesse_rho_mu(DoubleTab&) const override;
  void calculer_moyenne_spatiale_nut(DoubleTab&) const override;
  void calculer_moyenne_spatiale_Temp(DoubleTab&) const override;

};


#endif
