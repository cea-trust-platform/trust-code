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


#ifndef Traitement_particulier_NS_temperature_included
#define Traitement_particulier_NS_temperature_included

#include <Traitement_particulier_NS_base.h>
#include <TRUSTTabs_forward.h>
#include <Champ_Don.h>
#include <SFichier.h>

/*! @brief classe Traitement_particulier_NS_temperature Cette classe permet de faire les traitements particuliers
 *
 *      pour les calculs de  l'energie cinetique totale
 *
 *
 * @sa Navier_Stokes_Turbulent, Traitement_particulier_NS_base, Traitement_particulier_VEF
 */
class Traitement_particulier_NS_temperature : public Traitement_particulier_NS_base
{
  Declare_base_sans_constructeur_ni_destructeur(Traitement_particulier_NS_temperature);

public :

  ~Traitement_particulier_NS_temperature() override  { }
  Traitement_particulier_NS_temperature() : le_fichier(0)  { }
  Entree& lire(Entree& ) override;
  void preparer_calcul_particulier() override ;
  void post_traitement_particulier() override ;
  void en_cours_de_resolution(int , DoubleTab&, DoubleTab& ,double) override { }
  void sauver_stat()  const override  { }
  void reprendre_stat() override   { }

protected :

  virtual void calcul_temperature() =0;
  DoubleVect temperature;
  SFichier* le_fichier;

  Nom nom_cl;
  int dir = -1;
};

#endif
