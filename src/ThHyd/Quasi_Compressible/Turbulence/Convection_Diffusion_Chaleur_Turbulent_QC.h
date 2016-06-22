/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Convection_Diffusion_Chaleur_Turbulent_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible/Turbulence
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Convection_Diffusion_Chaleur_Turbulent_QC_included
#define Convection_Diffusion_Chaleur_Turbulent_QC_included

#include <Convection_Diffusion_Turbulent.h>
#include <Convection_Diffusion_Chaleur_QC.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Convection_Diffusion_Chaleur_Turbulent_QC
//     Cette classe represente le cas particulier de
//     convection diffusion turbulente lorsque
//     le fluide est quasi compressible. L'inconnue est
//       la temperature si le fluide est un gaz parfait
//       l'enthalpie    si le fluide est un gaz reel
//     Cette classe herite de Convection_Diffusion_Turbulent qui contient
//     le modele de turbulence et de Convection_Diffusion_Chaleur_QC
//     qui modelise l'equation non turbulente associe a un fluide quasi compressible.
// .SECTION voir aussi
//      Convection_Diffusion_Turbulent  Convection_Diffusion_Chaleur_QC
//////////////////////////////////////////////////////////////////////////////
class Convection_Diffusion_Chaleur_Turbulent_QC : public Convection_Diffusion_Turbulent ,
  public Convection_Diffusion_Chaleur_QC
{
  Declare_instanciable(Convection_Diffusion_Chaleur_Turbulent_QC);

public :

  void set_param(Param& titi);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void completer();
  int sauvegarder(Sortie&) const;
  int reprendre(Entree&);
  void mettre_a_jour(double );
  void mettre_a_jour_modele(double );
  int preparer_calcul();
  virtual bool initTimeStep(double dt);

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////
  void imprimer(Sortie& os) const;
  const RefObjU& get_modele(Type_modele type) const;


};

#endif
